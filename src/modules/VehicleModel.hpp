#pragma once

#include <opencv2/core/mat.hpp>

#include <QMediaPlayer>


class Vehicle;
class CameraCalibration;


class VehicleModel : public QObject
{
    Q_OBJECT

    using VehicleList = std::vector<Vehicle *>;
    using VehicleMap  = std::unordered_map<int, VehicleList>;

    CameraCalibration * m_cameraCalibration_ptr;

    VehicleList m_vehicles;
    VehicleMap m_vehicleMap;

public:

    VehicleModel(QObject * parent = nullptr);

    VehicleList getVehiclesInFrame(int frameIdx) const;

    Q_INVOKABLE void setCameraCalibration(CameraCalibration * calibrationModule);

public slots:

    void onAnalysisStarted();
    void onAnalysisEnded();
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);

    void onVehicleTrackingFinished(Vehicle * newVehicle_ptr);

private:

    void cleanUpVehicles();
    void clearVehicles();

signals:

    void vehiclePostProcessingFinished(VehicleList);
};