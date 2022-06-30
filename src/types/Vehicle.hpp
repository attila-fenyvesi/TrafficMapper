#pragma once

#include <boost/circular_buffer.hpp>
#include <opencv2/tracking/tracker.hpp>
#include <opencv2/video/tracking.hpp>

#include <QMutex>
#include <QObject>

#include <bitset>


class Detection;
class QPainter;


class Vehicle : public QObject
{
    Q_OBJECT

    enum class VehicleType;

    using FramePtr     = QSharedPointer<cv::Mat>;
    using FrameBuffer  = boost::circular_buffer<FramePtr>;
    using VehicleType  = Vehicle::VehicleType;
    using DetectionMap = std::unordered_map<int, Detection>;
    using PositionMap  = std::unordered_map<int, QPointF>;
    using Trajectory   = std::vector<std::pair<int, QLineF>>;
    using SpeedMap     = std::unordered_map<int, int>;
    using TrackerPtr   = cv::Ptr<cv::Tracker>;

    friend class Tracker;

    QMutex m_mutex;
    TrackerPtr m_tracker;

    DetectionMap m_detections;
    PositionMap m_positions;
    Trajectory m_trajectory;
    SpeedMap m_speed;

    int m_firstFrame;
    int m_lastFrame;
    bool m_isTracked;
    int m_timeSinceLastHit;
    VehicleType m_vehicleType;

public:

    enum class VehicleType
    {
        undefined  = -1,
        CAR        = 0,
        BUS        = 1,
        TRUCK      = 2,
        MOTORCYCLE = 3,
        BICYCLE    = 4
    };
    Q_ENUMS(VehicleType)

    friend std::istream & operator>>(std::istream & iStream, VehicleType & vType);
    friend std::ostream & operator<<(std::ostream & oStream, const VehicleType & vType);

    Vehicle();
    Vehicle(const int frameIdx, const Detection & detection);
    Vehicle(const Vehicle & other);

    bool isTracked() const;
    bool weaksFallFirst();
    VehicleType getVehicleType() const;
    int getFirstFrameIdx() const;
    int getLastFrameIdx() const;
    float getSpeedAt(int frameIdx) const;
    float getInvIOU(const Detection & detection) const;
    const Trajectory & getTrajectory() const;

    void updatePosition(const int frameIdx, const Detection & detection);
    void trackPositionForward(const FrameBuffer frameBuffer);
    void trackPositionBackwards(const FrameBuffer frameBuffer);

    bool mergeWith(Vehicle * other_ptr);
    void calcVehicleType();
    void calcPosition();
    void calcSpeed(const cv::Mat & homographyMatrix);

    void drawOnFrame(QPainter & painter, const int frameIdx, const std::bitset<4> & options);

private:

    QString getVehicleTypeName();

    inline void loadTracker(TrackerPtr & tracker);
    inline void updateForwardTracker(const FrameBuffer & frameBuffer);
    inline void updateBackwardTracker(const FrameBuffer & frameBuffer, TrackerPtr & tracker);

    bool checkMergeConditions(const Vehicle * other_ptr) const;
    bool checkOverlapWithOtherVehicle(const Vehicle * otherVehicle) const;
    inline void mergeOverlappingSection(Vehicle * other_ptr);
    inline void mergeOverhangingSections(Vehicle * other_ptr);

    inline void pushFrontDetection(const int frameIdx, const Detection & detection);
    inline void pushBackDetection(const int frameIdx, const Detection & detection);
    inline void stopTracking();
    inline void getAvgSpeedAt(const int frameIdx, QPointF & speed, const int window = 10);
};

Q_DECLARE_METATYPE(Vehicle::VehicleType)