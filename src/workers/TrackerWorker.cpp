#include "TrackerWorker.hpp"

#include <TrafficMapper/Media/MediaPlayer>
#include <TrafficMapper/Modules/Tracker>
#include <TrafficMapper/Tools/Helpers>
#include <TrafficMapper/Tools/HungarianAlgorithm>

#include <cppitertools/enumerate.hpp>

#include <QSharedPointer>


TrackerWorker::TrackerWorker(QObject * parent) : QThread(parent), m_isRunning(false)
{
    Helpers::debugMessage("TrackerWorker", "Worker started!");

    m_frameBuffer.set_capacity(Tracker::paramTrackingDistance());
    m_tracker_prt = qobject_cast<Tracker *>(parent);
    m_video       = cv::VideoCapture(MediaPlayer::m_videoMeta.getURL().toString().toStdString());
}

TrackerWorker::~TrackerWorker()
{
    QThread::quit();
    QThread::wait();

    Helpers::debugMessage("TrackerWorker", "Worker destroyed!");
}


void
  TrackerWorker::run()
{
    Helpers::debugMessage("TrackerWorker", "THREAD Started!");

    m_isRunning = true;

    runVehicleDetection();
    finishLeftoverVehicleTrackings();

    Helpers::debugMessage("TrackerWorker", "THREAD ended!");
}

void
  TrackerWorker::stop()
{
    m_isRunning = false;
}


inline void
  TrackerWorker::runVehicleDetection()
{
    for (int frameIdx(0); m_isRunning; ++frameIdx) {
        auto frame_ptr = getNextVideoFrame();
        if (frame_ptr->empty())
            break;

        m_frameBuffer.push_back(frame_ptr);
        m_tracker_prt->getDetections(frameIdx, frame_ptr, m_frameDetections);

        vector<int> assignments;
        vector<vector<double>> iouMatrix;

        matchDetectionsWithVehicles(assignments, iouMatrix);
        trackVehicles(frameIdx, assignments, iouMatrix);
        createNewVehicles(frameIdx, assignments);
        removeFinishedVehicles();

        emit progressUpdated(frameIdx);
    }
}

QSharedPointer<cv::Mat>
  TrackerWorker::getNextVideoFrame()
{
    QSharedPointer<cv::Mat> frame_ptr = QSharedPointer<cv::Mat>(new cv::Mat);
    m_video >> *frame_ptr;

    return frame_ptr;
}

inline void
  TrackerWorker::matchDetectionsWithVehicles(vector<int> & assignments, Matrix_d & iouMatrix)
{
    m_trackingNumber  = m_activeTrackings.size();
    m_detectionNumber = m_frameDetections.size();

    if (m_trackingNumber && m_detectionNumber) {
        prepIOUmatrix(iouMatrix);
        HungarianAlgorithm::Solve(iouMatrix, assignments);
    }
}

inline void
  TrackerWorker::prepIOUmatrix(Matrix_d & iouMatrix)
{
    iouMatrix.resize(m_trackingNumber);

    for (auto && [vehicleIdx, vehicle_ptr] : iter::enumerate(m_activeTrackings)) {
        iouMatrix[vehicleIdx].resize(m_detectionNumber);

        for (auto && [detectionIdx, detection] : iter::enumerate(m_frameDetections)) {
            iouMatrix[vehicleIdx][detectionIdx] = vehicle_ptr->getInvIOU(detection);
        }
    }
}

inline void
  TrackerWorker::trackVehicles(int frameIdx, std::vector<int> & assignments, Matrix_d & iouMatrix)
{
    for (auto && [i, vehicle_ptr] : iter::enumerate(m_activeTrackings)) {
<<<<<<< HEAD
		const int assignment = assignments.at(i);

=======
        //int assignment = -1;

        //try {
        //    assignment = assignments.at(i);
        //} catch (const std::out_of_range &) {}

		const int assignment = assignments.at(i);

>>>>>>> b11c027b401d519d5082617939a6bc347c6a1b8c
        if (m_detectionNumber && assignment != -1 && iouMatrix[i][assignment] <= 1.f - Tracker::paramIOUThreshold())
            vehicle_ptr->updatePosition(frameIdx, m_frameDetections[assignment]);
        else
            vehicle_ptr->trackPositionForward(m_frameBuffer);
    }
}

inline void
  TrackerWorker::createNewVehicles(int frameIdx, std::vector<int> & assignments)
{
    for (auto && [i, detection] : iter::enumerate(m_frameDetections)) {
        if (std::find(assignments.begin(), assignments.end(), i) == assignments.end()) {
            auto * newVehicle = new Vehicle(frameIdx, detection);
            m_activeTrackings.push_back(newVehicle);
            //newVehicle->trackPositionBackwards(m_frameBuffer);
        }
    }
}

inline void
  TrackerWorker::removeFinishedVehicles()
{
    m_activeTrackings.erase(
      std::remove_if(
        m_activeTrackings.begin(),
        m_activeTrackings.end(),
        [this](Vehicle * vehicle_ptr) {
            if (!vehicle_ptr->isTracked()) {
                emit vehicleTrackingFinished(vehicle_ptr);
                return true;
            }
            return false;
        }),
      m_activeTrackings.end());
}

inline void
  TrackerWorker::finishLeftoverVehicleTrackings()
{
    for (auto vehicle_ptr : m_activeTrackings)
        emit vehicleTrackingFinished(vehicle_ptr);
}
