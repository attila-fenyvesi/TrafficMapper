#include "VehicleModel.hpp"

#include <TrafficMapper/Modules/CameraCalibration>
#include <TrafficMapper/Tools/Helpers>
#include <TrafficMapper/Types/Vehicle>


// ======================================================================
//   CONSTRUCTORS
// ======================================================================

VehicleModel::VehicleModel(QObject * parent) : QObject(parent), m_cameraCalibration_ptr(nullptr) {}


// ======================================================================
//   SETTERS & GETTERS
// ======================================================================

VehicleModel::VehicleList
  VehicleModel::getVehiclesInFrame(int frameIdx) const
{
    try {
        return m_vehicleMap.at(frameIdx);
    } catch (const std::out_of_range &) {
        return VehicleList();
    }
}


// ======================================================================
//   PUBLIC FUNCTIONS
// ======================================================================

Q_INVOKABLE void
  VehicleModel::setCameraCalibration(CameraCalibration * calibrationModule)
{
    m_cameraCalibration_ptr = calibrationModule;
}


// ======================================================================
//   PUBLIC SLOTS
// ======================================================================

void
  VehicleModel::onAnalysisStarted()
{
    clearVehicles();
}

void
  VehicleModel::onAnalysisEnded()
{
    Helpers::debugMessage("VehicleModel", "Vehicle post-processing started...");

    cleanUpVehicles();

    Helpers::debugMessage("VehicleModel", QString("Vehicle count: %1").arg(m_vehicles.size()));

    for (auto vehicle_ptr : m_vehicles) {
        vehicle_ptr->calcPosition();
        vehicle_ptr->calcSpeed(m_cameraCalibration_ptr->getHomographyMatrix());
        vehicle_ptr->calcVehicleType();

        const int firstIdx = vehicle_ptr->getFirstFrameIdx();
        const int lastIdx  = vehicle_ptr->getLastFrameIdx();

        for (int frameIdx(firstIdx); frameIdx <= lastIdx; ++frameIdx)
            m_vehicleMap[frameIdx].push_back(vehicle_ptr);
    }

    Helpers::debugMessage("VehicleModel", "Vehicle post-processing finished.");

    emit vehiclePostProcessingFinished(m_vehicles);
}

void
  VehicleModel::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::LoadingMedia)
        clearVehicles();
}

void
  VehicleModel::onVehicleTrackingFinished(Vehicle * newVehicle_ptr)
{
    //for (auto vehicle_ptr : m_vehicles) {
    //    if (vehicle_ptr->mergeWith(newVehicle_ptr)) {
    //        delete newVehicle_ptr;
    //        return;
    //    }
    //}
    m_vehicles.push_back(newVehicle_ptr);
}


// ======================================================================
//   PRIVATE FUNCTIONS
// ======================================================================

inline void
  VehicleModel::cleanUpVehicles()
{
    m_vehicles.erase(
      std::remove_if(
        m_vehicles.begin(),
        m_vehicles.end(),
        [](Vehicle * vehicle_ptr) {
            if (vehicle_ptr->weaksFallFirst()) {
                delete vehicle_ptr;
                return true;
            } else {
                return false;
            }
        }),
      m_vehicles.end());
}

inline void
  VehicleModel::clearVehicles()
{
    for (auto vehicle_ptr : m_vehicles)
        delete vehicle_ptr;

    m_vehicles.clear();
    m_vehicleMap.clear();
}
