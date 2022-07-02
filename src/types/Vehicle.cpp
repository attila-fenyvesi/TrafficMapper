#include "Vehicle.hpp"

#include <TrafficMapper/Media/MediaPlayer>
#include <TrafficMapper/Modules/Tracker>
#include <TrafficMapper/Tools/Helpers>
#include <TrafficMapper/Tools/KalmanSmoother>
#include <TrafficMapper/Types/Detection>

#include <cppitertools/imap.hpp>
#include <cppitertools/range.hpp>
#include <cppitertools/sliding_window.hpp>
#include <cppitertools/zip.hpp>

#include <Eigen/Core>
#include <Eigen/LU>

#include <QDebug>
#include <QFont>
#include <QFontMetrics>
#include <QMutexLocker>
#include <QPainter>
#include <QPen>
#include <QtConcurrent/QtConcurrent>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <ranges>


using TrackerAlg = Tracker::OpticalTrackerAlgorithm;


// ======================================================================
//   CONSTRUCTORS
// ======================================================================

Vehicle::Vehicle()
  : m_isTracked(true), m_timeSinceLastHit(0), m_vehicleType(VehicleType::undefined), m_firstFrame(0), m_lastFrame(0),
    m_mutex()
{
}

Vehicle::Vehicle(const int frameIdx, const Detection & detection)
  : m_isTracked(true), m_timeSinceLastHit(0), m_vehicleType(VehicleType::undefined), m_firstFrame(frameIdx),
    m_lastFrame(frameIdx), m_mutex()
{
    m_detections[frameIdx] = detection;
}

Vehicle::Vehicle(const Vehicle & other)
{
    m_detections = other.m_detections;
    m_positions  = other.m_positions;
    m_trajectory = other.m_trajectory;
    m_speed      = other.m_speed;

    m_isTracked        = other.m_isTracked;
    m_timeSinceLastHit = other.m_timeSinceLastHit;
    m_firstFrame       = other.m_firstFrame;
    m_lastFrame        = other.m_lastFrame;
    m_vehicleType      = other.m_vehicleType;
}


// ======================================================================
//   SETTERS & GETTERS
// ======================================================================

bool
  Vehicle::isTracked() const
{
    return m_isTracked;
}

bool
  Vehicle::weaksFallFirst()
{
    return m_lastFrame - m_firstFrame < 60;
}

Vehicle::VehicleType
  Vehicle::getVehicleType() const
{
    return m_vehicleType;
}

int
  Vehicle::getFirstFrameIdx() const
{
    return m_firstFrame;
}

int
  Vehicle::getLastFrameIdx() const
{
    return m_lastFrame;
}

float
  Vehicle::getSpeedAt(int frameIdx) const
{
    const int firstIdx = std::max(m_firstFrame, frameIdx - 10);
    const int lastIdx  = std::min(m_lastFrame, frameIdx + 10);

    if (firstIdx <= lastIdx) {
        float sum = 0;
        for (int i(firstIdx); i <= lastIdx; ++i) {
            sum += m_speed.at(i);
        }

        return sum / (lastIdx - firstIdx + 1);
    }

    return 0.f;
}

float
  Vehicle::getInvIOU(const Detection & detection) const
{
    const float iou = Detection::iou(m_detections.at(m_lastFrame), detection);

    return iou > Tracker::paramIOUThreshold() ? (1.f - iou) : 1.f;
}

const Vehicle::Trajectory &
  Vehicle::getTrajectory() const
{
    return m_trajectory;
}


// ======================================================================
//   PUBLIC FUNCTIONS
// ======================================================================

void
  Vehicle::updatePosition(const int frameIdx, const Detection & detection)
{
    if (!m_tracker.empty())
        m_tracker.release();

    m_timeSinceLastHit = 0;

    pushBackDetection(frameIdx, detection);
}

void
  Vehicle::trackPositionForward(const FrameBuffer frameBuffer)
{
    if (frameBuffer.size() == 0) {
        stopTracking();
        return;
    }

    if (m_tracker.empty()) {
        loadTracker(m_tracker);

        const auto & initFrame = **(frameBuffer.rbegin() + 1);
        const auto & initBBox  = m_detections[m_lastFrame];

        if (m_tracker.empty() || !m_tracker->init(initFrame, initBBox)) {
            Helpers::debugMessage("Vehicle", "Forward tracker couldn't be initialized!", Helpers::DBG_WARNING);
            stopTracking();
            return;
        }
    }

    updateForwardTracker(frameBuffer);
}

void
  Vehicle::trackPositionBackwards(const FrameBuffer frameBuffer)
{
    if (frameBuffer.size() == 0)
        return;

    QtConcurrent::run(
      [this](FrameBuffer frameBuffer) {
          TrackerPtr tracker;

          loadTracker(tracker);

          const auto & initFrame = **frameBuffer.rbegin();
          const auto & initBBox  = m_detections[m_firstFrame];

          if (tracker.empty() || !tracker->init(initFrame, initBBox)) {
              Helpers::debugMessage("Vehicle", "Backward tracker couldn't be initialized!", Helpers::DBG_WARNING);
              return;
          }

          updateBackwardTracker(frameBuffer, tracker);

          tracker.release();
      },
      frameBuffer);
}

bool
  Vehicle::mergeWith(Vehicle * otherVehicle_ptr)
{
    if (!checkMergeConditions(otherVehicle_ptr))
        return false;

    mergeOverlappingSection(otherVehicle_ptr);
    mergeOverhangingSections(otherVehicle_ptr);

    return true;
}

void
  Vehicle::calcVehicleType()
{
    using VTypeSum = std::unordered_map<VehicleType, float>;

    auto sum =
      std::reduce(m_detections.begin(), m_detections.end(), VTypeSum(), [](VTypeSum prev, const auto detection) {
          prev[detection.second.getVehicleType()] += detection.second.getConfidence();
          return prev;
      });

    auto max_element = std::max_element(
      sum.begin(), sum.end(), [](const decltype(sum)::value_type & p1, const decltype(sum)::value_type & p2) {
          return p1.second < p2.second;
      });

    m_vehicleType = max_element->first;
}

void
  Vehicle::calcPosition()
{
    KalmanSmoother<double> kalmanSmoother(4, 4);

    const int n = m_detections.size();
    QPointF speed;
    getAvgSpeedAt(m_firstFrame, speed);
    const double pos_X = m_detections[m_firstFrame].center().x();
    const double pos_Y = m_detections[m_firstFrame].center().y();

    kalmanSmoother.x = Eigen::Vector4d {
        {pos_X, pos_Y, speed.x(), speed.y()}
    };

    kalmanSmoother.P = Eigen::Matrix4d {
        {1.1, 0.0, 0.0, 0.0},
        {0.0, 1.1, 0.0, 0.0},
        {0.0, 0.0, 0.1, 0.0},
        {0.0, 0.0, 0.0, 0.1}
    };

    kalmanSmoother.F = Eigen::Matrix4d {
        {1.0, 0.0, 1.0, 0.0},
        {0.0, 1.0, 0.0, 1.0},
        {0.0, 0.0, 1.0, 0.0},
        {0.0, 0.0, 0.0, 1.0}
    };

    kalmanSmoother.H = Eigen::Matrix<double, 4, 4> {
        {1.0, 0.0, 0.0, 0.0},
        {0.0, 1.0, 0.0, 0.0},
        {0.0, 0.0, 1.0, 0.0},
        {0.0, 0.0, 0.0, 1.0}
    };

    kalmanSmoother.R = Eigen::Matrix4d {
        {5.0, 0.0, 0.0, 0.0},
        {0.0, 5.0, 0.0, 0.0},
        {0.0, 0.0, 1.0, 0.0},
        {0.0, 0.0, 0.0, 1.0}
    } * 2;

    kalmanSmoother.Q = Eigen::Matrix4d {
        {0.25,  0.0, 0.5, 0.0},
        { 0.0, 0.25, 0.0, 0.5},
        { 0.5,  0.0, 1.0, 0.0},
        { 0.0,  0.5, 0.0, 1.0}
    } * 0.04;

    std::vector<std::vector<double>> input;
    input.reserve(m_lastFrame - m_firstFrame);
    for (int frameIdx(m_firstFrame + 1); frameIdx <= m_lastFrame; ++frameIdx) {
        QPointF speed;
        getAvgSpeedAt(frameIdx, speed);
        input.push_back({ static_cast<double>(m_detections[frameIdx].center().x()),
                          static_cast<double>(m_detections[frameIdx].center().y()),
                          static_cast<double>(speed.x()),
                          static_cast<double>(speed.y()) });
    };

    const auto f = kalmanSmoother.forwardPass(input);
    const auto b = kalmanSmoother.backwardPass();

    for (const auto & [frameIdx, data] : iter::zip(iter::range(m_firstFrame, m_lastFrame + 1), f)) {
        m_positions[frameIdx] = QPointF(data[0], data[1]);
        if (frameIdx > m_firstFrame)
            m_trajectory.push_back(std::make_pair(frameIdx, QLineF(m_positions[frameIdx - 1], m_positions[frameIdx])));
        m_speed[frameIdx] = 0;
    }
}

void
  Vehicle::calcSpeed(const cv::Mat & homographyMatrix)
{
    if (homographyMatrix.empty())
        return;

    const auto detection_nr = m_positions.size();

    std::vector<cv::Point2f> imgPoints, planePoints;
    imgPoints.reserve(detection_nr);
    planePoints.reserve(detection_nr);

    for (auto & [frameIdx, position] : m_positions) {
        imgPoints.push_back(cv::Point2f((float) position.x(), (float) position.y()));
    }

    cv::perspectiveTransform(imgPoints, planePoints, homographyMatrix);

    int idx                = m_firstFrame + 1;
    const float multiplier = MediaPlayer::m_videoMeta.getFPS() * 3.6f;
    for (auto && window : iter::sliding_window(planePoints, 3)) {
        m_speed[idx++] = cv::norm(window[0] - window[2]) * multiplier * 0.5f;
    }
}

void
  Vehicle::drawOnFrame(QPainter & painter, const int frameIdx, const std::bitset<4> & options)
{
    static QPen pen;
    static const QFont painterFont("Arial", MediaPlayer::m_videoMeta.getHEIGHT() / 55, 700);
    static const QFontMetrics fm(painterFont);
    static const int marginSize = MediaPlayer::m_videoMeta.getHEIGHT() * 0.005f;
    static const QMargins margin(marginSize, 0, marginSize, 0);

    if (options[1])    // TRAJECTORY
    {
        pen.setColor(QColor("#EEEEEE"));
        pen.setWidth(2);
        painter.setPen(pen);

        for (const auto & line : m_trajectory) {
            painter.drawLine(line.second);
        }
    }

    if (options[0])    // DETECTIONS
    {
        switch (m_detections[frameIdx].getVehicleType()) {
            case VehicleType::CAR:
                pen.setColor(QColor("blue"));
                break;
            case VehicleType::BUS:
                pen.setColor(QColor("yellow"));
                break;
            case VehicleType::TRUCK:
                pen.setColor(QColor("red"));
                break;
            case VehicleType::MOTORCYCLE:
                pen.setColor(QColor("purple"));
                break;
            case VehicleType::BICYCLE:
                pen.setColor(QColor("green"));
                break;
            default:
                pen.setColor(QColor("white"));
                break;
        }

        pen.setWidth(2);
        painter.setPen(pen);

        painter.drawRect(m_detections[frameIdx]);
    }

    if (options[2] || options[3])    // LABELS (Type and Speed)
    {
        pen.setColor("#FFFFFF");
        painter.setPen(pen);
        painter.setFont(painterFont);

        QString labelText;
        if (options[2])
            labelText.append(getVehicleTypeName());
        if (options[2] && options[3])
            labelText.append("\n");
        if (options[3])
            labelText.append(QString("%1 km/h").arg((int) getSpeedAt(frameIdx)));

        QRectF labelBackground = fm.boundingRect(QRect(0, 0, 1000, 1000), Qt::AlignLeft, labelText);
        labelBackground.translate(m_positions[frameIdx]);

        painter.fillRect(labelBackground + margin, QColor("#CD4444"));
        painter.drawText(labelBackground, labelText);
    }
}


// ======================================================================
//   PRIVATE FUNCTIONS
// ======================================================================

QString
  Vehicle::getVehicleTypeName()
{
    switch (m_vehicleType) {
        case VehicleType::BICYCLE:
            return QStringLiteral("Bicycle");
        case VehicleType::BUS:
            return QStringLiteral("Bus");
        case VehicleType::CAR:
            return QStringLiteral("Car");
        case VehicleType::MOTORCYCLE:
            return QStringLiteral("Motorcycle");
        case VehicleType::TRUCK:
            return QStringLiteral("Truck");
        default:
            return QStringLiteral("unidentified");
    }
}

inline void
  Vehicle::loadTracker(TrackerPtr & tracker)
{
    switch (Tracker::paramTrackingAlg()) {
        case TrackerAlg::KCF:
            tracker = cv::TrackerKCF::create();
            break;
        case TrackerAlg::CSRT:
            tracker = cv::TrackerCSRT::create();
            break;
        case TrackerAlg::MOSSE:
            tracker = cv::TrackerMOSSE::create();
            break;
    }
}

inline void
  Vehicle::updateForwardTracker(const FrameBuffer & frameBuffer)
{
    // cv::Rect2d newTrack;
    stopTracking();

    // if (m_tracker->update(*(frameBuffer.back()), newTrack)) {
    //     pushBackDetection(m_lastFrame + 1, newTrack);
    //     if (++m_timeSinceLastHit == Tracker::paramTrackingDistance())
    //         stopTracking();
    // } else {
    //     stopTracking();
    // }
}

inline void
  Vehicle::updateBackwardTracker(const FrameBuffer & frameBuffer, TrackerPtr & tracker)
{
    cv::Rect2d newTrack;
    int frameIdx = m_firstFrame;

    for (auto r_it = frameBuffer.rbegin() + 1; r_it != frameBuffer.rend(); ++r_it) {
        if (tracker->update(**r_it, newTrack))
            pushFrontDetection(--frameIdx, newTrack);
        else
            break;
    }
}

bool
  Vehicle::checkMergeConditions(const Vehicle * otherVehicle_ptr) const
{
    bool readyToMerge = false;

    if (!checkOverlapWithOtherVehicle(otherVehicle_ptr))
        return readyToMerge;

    const int firstIdx = std::max(m_firstFrame, otherVehicle_ptr->m_firstFrame);
    const int lastIdx  = std::min(m_lastFrame, otherVehicle_ptr->m_lastFrame);

    for (int i = firstIdx; i <= lastIdx; ++i) {
        const Detection & detThis  = m_detections.at(i);
        const Detection & detOther = otherVehicle_ptr->m_detections.at(i);

        if (Detection::iou(detThis, detOther) > Tracker::paramIOUThreshold()) {
            // If both detection have a getConfidence score (both are YOLO detections),
            // then they are different vehicles.
            if (detThis.getConfidence() > 0 && detOther.getConfidence() > 0)
                return false;

            readyToMerge = true;
        }
    }

    return readyToMerge;
}

bool
  Vehicle::checkOverlapWithOtherVehicle(const Vehicle * otherVehicle) const
{
    const int a1 = m_firstFrame;
    const int a2 = m_lastFrame;
    const int b1 = otherVehicle->m_firstFrame;
    const int b2 = otherVehicle->m_lastFrame;

    const int la = a2 - a1;
    const int lb = b2 - b1;

    const int m1 = std::min(a1, b1);
    const int m2 = std::max(a2, b2);

    const int l = m2 - m1;

    return l < la + lb + 2 && l != la && l != lb;
}

inline void
  Vehicle::mergeOverlappingSection(Vehicle * other_ptr)
{
    const int firstIdx = std::max(m_firstFrame, other_ptr->m_firstFrame);
    const int lastIdx  = std::min(m_lastFrame, other_ptr->m_lastFrame);

    // THIS:  --------+--++
    // Other:       ++-++-------
    for (int i = firstIdx; i <= lastIdx; ++i) {
        Detection & detThis  = m_detections[i];
        Detection & detOther = other_ptr->m_detections[i];

        if (detThis.getConfidence() < detOther.getConfidence())
            detThis = std::move(detOther);
    }
}

inline void
  Vehicle::mergeOverhangingSections(Vehicle * other_ptr)
{
    // THIS:  -------------
    // Other:       -------+++++
    for (int i = m_lastFrame + 1; i <= other_ptr->m_lastFrame; ++i) {
        pushBackDetection(i, other_ptr->m_detections[i]);
    }

    // THIS:       -------------
    // Other: +++++-------
    for (int i = other_ptr->m_firstFrame; i < m_firstFrame; ++i) {
        pushFrontDetection(i, other_ptr->m_detections[i]);
    }
}

inline void
  Vehicle::pushFrontDetection(const int frameIdx, const Detection & detection)
{
    QMutexLocker locker(&m_mutex);
    m_detections[frameIdx] = detection;
    m_firstFrame           = frameIdx;
}

inline void
  Vehicle::pushBackDetection(const int frameIdx, const Detection & detection)
{
    QMutexLocker locker(&m_mutex);
    m_detections[frameIdx] = detection;
    m_lastFrame            = frameIdx;
}

inline void
  Vehicle::stopTracking()
{
    m_isTracked = false;
    m_tracker.release();
}

inline void
  Vehicle::getAvgSpeedAt(const int frameIdx, QPointF & speed, const int window)
{
    const int i_min = std::max(m_firstFrame, frameIdx - window);
    const int i_max = std::min(m_lastFrame, frameIdx + window);

    speed = (m_positions[i_max] - m_positions[i_min]) / (i_max - i_min + 1);
}


std::istream &
  operator>>(std::istream & iStream, Vehicle::VehicleType & vType)
{
    std::string value;
    iStream >> value;
    vType = Vehicle::VehicleType(std::stoi(value));

    return iStream;
}

std::ostream &
  operator<<(std::ostream & oStream, const Vehicle::VehicleType & vType)
{
    oStream << static_cast<int>(vType);

    return oStream;
}
