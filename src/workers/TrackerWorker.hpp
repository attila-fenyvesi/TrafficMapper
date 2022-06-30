#pragma once

#include <TrafficMapper/Types/Vehicle>

#include <opencv2/core/mat.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/videoio.hpp>

#include <QMutex>
#include <QQueue>
#include <QThread>
#include <QUrl>
#include <QWaitCondition>


class Detection;
class FrameGrabberWorker;


class TrackerWorker : public QThread
{
    Q_OBJECT

    using FramePtr      = QSharedPointer<cv::Mat>;
    using FrameBuffer   = boost::circular_buffer<FramePtr>;
    using Matrix_d      = std::vector<std::vector<double>>;
    using DetectionList = std::vector<Detection>;
    using VehicleList   = std::vector<Vehicle *>;

    Tracker * m_tracker_prt;

    bool m_isRunning;

    cv::VideoCapture m_video;
    FrameBuffer m_frameBuffer;

    int m_trackingNumber;
    int m_detectionNumber;

    DetectionList m_frameDetections;
    VehicleList m_activeTrackings;

public:

    explicit TrackerWorker(QObject * parent = nullptr);
    ~TrackerWorker() override;

public slots:

    void run() override;
    void stop();

private:

    void runVehicleDetection();
    FramePtr getNextVideoFrame();
    void prepIOUmatrix(Matrix_d & iouMatrix);
    void matchDetectionsWithVehicles(std::vector<int> & assignments, Matrix_d & iouMatrix);
    void trackVehicles(int frameIdx, std::vector<int> & assignments, Matrix_d & iouMatrix);
    void createNewVehicles(int frameIdx, std::vector<int> & assignments);
    void removeFinishedVehicles();
    void finishLeftoverVehicleTrackings();

signals:

    void progressUpdated(int);
    void vehicleTrackingFinished(Vehicle *);
};