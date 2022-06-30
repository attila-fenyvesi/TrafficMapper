#include "CameraCalibration.hpp"

#include <TrafficMapper/Media/MediaPlayer>

#include <opencv2/calib3d.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include <QPainter>


// ======================================================================
//   CONSTRUCTORS
// ======================================================================

CameraCalibration::CameraCalibration(QQuickItem * parent)
  : QQuickPaintedItem(parent), m_pointSet(0), m_distance_0(0.f), m_distance_1(0.f)
{}


// ======================================================================
//   SETTERS & GETTERS
// ======================================================================

const cv::Mat &
  CameraCalibration::getHomographyMatrix() const
{
    return m_homography_i2p;
}


// ======================================================================
//   PUBLIC FUNCTIONS
// ======================================================================

Q_INVOKABLE void
  CameraCalibration::calculateHomography()
{
    const double ratio_X = MediaPlayer::m_videoMeta.getWIDTH() / (float) property("width").toFloat();
    const double ratio_Y = MediaPlayer::m_videoMeta.getHEIGHT() / (float) property("height").toFloat();

    std::vector<cv::Point2d> planePoints = { cv::Point2d(0, 0),
                                             cv::Point2d(m_distance_0, 0),
                                             cv::Point2d(m_distance_0, m_distance_1),
                                             cv::Point2d(0, m_distance_1) };
    std::vector<cv::Point2d> imgPoints   = { cv::Point2d(m_point_0.x() * ratio_X, m_point_0.y() * ratio_Y),
                                           cv::Point2d(m_point_1.x() * ratio_X, m_point_1.y() * ratio_Y),
                                           cv::Point2d(m_point_2.x() * ratio_X, m_point_2.y() * ratio_Y),
                                           cv::Point2d(m_point_3.x() * ratio_X, m_point_3.y() * ratio_Y) };

    m_homography_p2i = cv::findHomography(planePoints, imgPoints);
    m_homography_i2p = cv::findHomography(imgPoints, planePoints);
}

Q_INVOKABLE void
  CameraCalibration::loadRandomFrame()
{
    cv::Mat frame;

    MediaPlayer::getRandomFrame(frame);
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

    m_image =
      QImage((uchar *) frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888).scaled(QSize(960, 540));
}

void
  CameraCalibration::paint(QPainter * painter)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawImage(QPoint(0, 0), m_image);

    switch (m_pointSet) {
        case 1:
            drawRect1(painter);
            break;
        case 2:
            drawRect2(painter);
            break;
        case 3:
            drawRect3(painter);
            break;
        case 4:
            drawRect4(painter);
            drawPlanePoints(painter);
    }
}


// ======================================================================
//   PRIVATE FUNCTIONS
// ======================================================================

inline void
  CameraCalibration::drawRect1(QPainter * painter)
{
    QPen pen;
    pen.setWidth(3);
    pen.setCapStyle(Qt::RoundCap);
    pen.setColor(QColor("#CD5555"));
    painter->setPen(pen);
    painter->drawLine(m_point_0, m_point_hover);
}

inline void
  CameraCalibration::drawRect2(QPainter * painter)
{
    QPen pen;
    pen.setWidth(3);
    pen.setCapStyle(Qt::RoundCap);
    pen.setColor(QColor("#CD5555"));
    painter->setPen(pen);
    painter->drawLine(m_point_0, m_point_1);
    pen.setColor(QColor("#336699"));
    painter->setPen(pen);
    painter->drawLine(m_point_0, m_point_hover);
    painter->drawLine(m_point_1, m_point_hover);
}

inline void
  CameraCalibration::drawRect3(QPainter * painter)
{
    QPen pen;
    pen.setWidth(3);
    pen.setCapStyle(Qt::RoundCap);
    pen.setColor(QColor("#CD5555"));
    painter->setPen(pen);
    painter->drawLine(m_point_0, m_point_1);
    painter->drawLine(m_point_2, m_point_hover);
    pen.setColor(QColor("#336699"));
    painter->setPen(pen);
    painter->drawLine(m_point_1, m_point_2);
    painter->drawLine(m_point_0, m_point_hover);
}

inline void
  CameraCalibration::drawRect4(QPainter * painter)
{
    QPen pen;
    pen.setWidth(3);
    pen.setCapStyle(Qt::RoundCap);
    pen.setColor(QColor("#CD5555"));
    painter->setPen(pen);
    painter->drawLine(m_point_0, m_point_1);
    painter->drawLine(m_point_2, m_point_3);
    pen.setColor(QColor("#336699"));
    painter->setPen(pen);
    painter->drawLine(m_point_1, m_point_2);
    painter->drawLine(m_point_0, m_point_3);
}

inline void
  CameraCalibration::drawPlanePoints(QPainter * painter)
{
    std::vector<cv::Point2f> planePoints;
    std::vector<cv::Point2f> imagePoints;

    generatePlanePoints(planePoints);

    cv::perspectiveTransform(planePoints, imagePoints, m_homography_p2i);

    const float ratio_X = property("width").toFloat() / MediaPlayer::m_videoMeta.getWIDTH();
    const float ratio_Y = property("height").toFloat() / MediaPlayer::m_videoMeta.getHEIGHT();

    QPen pen;
    pen.setWidth(3);
    pen.setColor(QColor("yellow"));
    painter->setPen(pen);

    for (const auto & point : imagePoints)
        painter->drawPoint(QPoint(point.x * ratio_X, point.y * ratio_Y));
}

inline void
  CameraCalibration::generatePlanePoints(std::vector<cv::Point2f> & input)
{
    for (int x(-m_distance_0); x < 2 * m_distance_0; ++x) {
        for (int y(-m_distance_1); y < 2 * m_distance_1; ++y) {
            input.push_back(cv::Point2f(x, y));
        }
    }
}