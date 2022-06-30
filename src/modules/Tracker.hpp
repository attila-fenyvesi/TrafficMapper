#pragma once

#include <TrafficMapper/Types/Detection>

#include <opencv2/dnn/dnn.hpp>

#include <QMediaPlayer>
#include <QObject>


class CameraCalibration;
class FrameGrabberWorker;
class Detection;
class Vehicle;


class Tracker : public QObject
{
    Q_OBJECT

    enum class OpticalTrackerAlgorithm;

    using TrackerAlg    = OpticalTrackerAlgorithm;
    using DetectionList = std::vector<Detection>;
    using DetectionMap  = std::unordered_map<int, DetectionList>;
    using FramePtr      = QSharedPointer<cv::Mat>;

    Q_PROPERTY(int cacheSize READ getCacheSize NOTIFY cacheSizeChanged)

    Q_PROPERTY(bool useGPU MEMBER m_param_useGPU)
    Q_PROPERTY(float YoloNMSThreshold MEMBER m_param_YoloNMSThreshold)
    Q_PROPERTY(float YoloConfThreshold MEMBER m_param_YoloConfThreshold)
    Q_PROPERTY(float IOUThreshold MEMBER m_param_IOUThreshold)
    Q_PROPERTY(TrackerAlg trackerAlorithm MEMBER m_param_trackingAlg)
    Q_PROPERTY(int trackingDistance MEMBER m_param_trackingDistance)

    inline static bool m_param_useGPU;
    inline static float m_param_YoloNMSThreshold;
    inline static float m_param_YoloConfThreshold;
    inline static float m_param_IOUThreshold;
    inline static TrackerAlg m_param_trackingAlg;
    inline static int m_param_trackingDistance;

    DetectionMap m_detections;
    cv::dnn::Net m_net;

public:

    enum class OpticalTrackerAlgorithm
    {
        KCF,
        CSRT,
        MOSSE
    };
    Q_ENUMS(OpticalTrackerAlgorithm)

    Tracker(QObject * parent = nullptr);

    static bool paramUseGPU();
    static float paramIOUThreshold();
    static TrackerAlg paramTrackingAlg();
    static int paramTrackingDistance();

    void getDetections(const int frameIdx, FramePtr frame_ptr, DetectionList & frameDetections);

    Q_INVOKABLE void openCacheFile(QUrl fileUrl);
    Q_INVOKABLE void clearCache();
    Q_INVOKABLE void analizeVideo();

public slots:

    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);

private:

    int getCacheSize() const;
    DetectionList getFrameDetections(const cv::Mat & frame);

signals:

    void stop();

    void analysisStarted();
    void analysisEnded();

    void vehicleTrackingFinished(Vehicle *);

    void cacheSizeChanged();
    void progressUpdated(int);
};

Q_DECLARE_METATYPE(Tracker::OpticalTrackerAlgorithm)