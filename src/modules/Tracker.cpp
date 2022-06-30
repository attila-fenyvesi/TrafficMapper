#include "Tracker.hpp"

#define _USE_MATH_DEFINES

#include <TrafficMapper/Media/MediaPlayer>
#include <TrafficMapper/Workers/TrackerWorker>

#include <cppitertools/enumerate.hpp>

#include <QImage>
#include <QPainter>
#include <QUrl>

#include <filesystem>
#include <fstream>
#include <math.h>


using VType = Vehicle::VehicleType;


// ======================================================================
//   CONSTRUCTORS
// ======================================================================

Tracker::Tracker(QObject * parent) : QObject(parent)
{
    const auto currentPath     = std::filesystem::current_path();
    const auto yoloWeightsFile = currentPath.string() + "\\data\\models\\yolov3.weights";
    const auto yoloConfigFile  = currentPath.string() + "\\data\\models\\yolov3.cfg";

    m_net = cv::dnn::readNet(yoloWeightsFile, yoloConfigFile);
    m_net.setPreferableBackend(cv::dnn::Backend::DNN_BACKEND_DEFAULT);
}


// ======================================================================
//   SETTERS & GETTERS
// ======================================================================

bool
  Tracker::paramUseGPU()
{
    return m_param_useGPU;
}

float
  Tracker::paramIOUThreshold()
{
    return m_param_IOUThreshold;
}

Tracker::TrackerAlg
  Tracker::paramTrackingAlg()
{
    return m_param_trackingAlg;
}

int
  Tracker::paramTrackingDistance()
{
    return m_param_trackingDistance;
}


// ======================================================================
//   PUBLIC FUNCTIONS
// ======================================================================

void
  Tracker::getDetections(const int frameIdx, FramePtr frame_ptr, DetectionList & frameDetections)
{
    try {
        frameDetections = m_detections.at(frameIdx);
    } catch (const std::out_of_range &) {
        frameDetections        = getFrameDetections(*frame_ptr);
        m_detections[frameIdx] = frameDetections;
        emit cacheSizeChanged();
    }
}

void
  Tracker::openCacheFile(QUrl fileUrl)
{
    m_detections.clear();

    int frameIdx;
    int detNum;

    std::ifstream ifs(fileUrl.toLocalFile().toStdString());

    while (ifs >> frameIdx) {
        ifs >> detNum;

        std::vector<Detection> frameDetections;

        for (int i(0); i < detNum; ++i) {
            Detection detection;
            ifs >> detection;

            if (detection.isValid())
                frameDetections.push_back(detection);
        }

        m_detections[frameIdx] = frameDetections;
    }

    emit cacheSizeChanged();

    ifs.close();
}

void
  Tracker::clearCache()
{
    m_detections.clear();
    emit cacheSizeChanged();
}

void
  Tracker::analizeVideo()
{
    emit analysisStarted();

    if (m_param_useGPU)
        m_net.setPreferableTarget(cv::dnn::Target::DNN_TARGET_OPENCL);
    else
        m_net.setPreferableTarget(cv::dnn::Target::DNN_TARGET_CPU);

    TrackerWorker * trackerWorker_ptr = new TrackerWorker(this);

    connect(trackerWorker_ptr, &QThread::finished, trackerWorker_ptr, &QObject::deleteLater);
    connect(this, &Tracker::stop, trackerWorker_ptr, &TrackerWorker::stop);
    connect(trackerWorker_ptr, &TrackerWorker::progressUpdated, this, &Tracker::progressUpdated);
    connect(trackerWorker_ptr, &QObject::destroyed, this, &Tracker::analysisEnded);
    connect(trackerWorker_ptr, &TrackerWorker::vehicleTrackingFinished, this, &Tracker::vehicleTrackingFinished);

    trackerWorker_ptr->start();
}


// ======================================================================
//   PUBLIC SLOTS
// ======================================================================

void
  Tracker::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::LoadingMedia)
        m_detections.clear();
}


// ======================================================================
//   PRIVATE FUNCTIONS
// ======================================================================

int
  Tracker::getCacheSize() const
{
    return m_detections.size();
}

inline Tracker::DetectionList
  Tracker::getFrameDetections(const cv::Mat & frame)
{
    const float videoWidth  = frame.cols;
    const float videoHeight = frame.rows;

    std::vector<cv::Mat> yoloOutput;

    cv::Mat blob;
    cv::dnn::blobFromImage(frame, blob, 1 / 255.0, cv::Size(416, 416), cv::Scalar(), false, false, CV_32F);
    m_net.setInput(blob);
    m_net.forward(yoloOutput, m_net.getUnconnectedOutLayersNames());

    DetectionList unfilteredDetections;
    std::vector<cv::Rect2i> boxes;
    std::vector<float> scores;
    std::vector<int> indices;

    for (auto layer : yoloOutput) {
        for (int detIdx(0); detIdx < layer.rows; ++detIdx) {
            const float score = layer.at<float>(detIdx, 4);

            if (score > 0.01) {
                cv::Point classIdPoint;
                cv::Mat det = layer.row(detIdx);

                cv::minMaxLoc(det.colRange(5, 10), 0, 0, 0, &classIdPoint);

                const float width  = det.at<float>(2) * videoWidth;
                const float height = det.at<float>(3) * videoHeight;
                const float x      = (det.at<float>(0) * videoWidth) - (width * 0.5f);
                const float y      = (det.at<float>(1) * videoHeight) - (height * 0.5f);

                unfilteredDetections.push_back(Detection(x, y, width, height, VType(classIdPoint.x), score));
                boxes.push_back(cv::Rect2f(x, y, width, height));
                scores.push_back(score);
            }
        }
    }

    cv::dnn::NMSBoxes(boxes, scores, m_param_YoloConfThreshold, m_param_YoloNMSThreshold, indices);

    DetectionList frameDetections;
    for (int i : indices)
        frameDetections.push_back(unfilteredDetections[i]);

    return frameDetections;
}