#pragma once

#include <TrafficMapper/Types/Vehicle>

#include <opencv2/core/types.hpp>

#include <QRectF>


class Detection : public QRectF
{
    using VType = Vehicle::VehicleType;

    VType m_vehicleType;
    float m_confidence;

public:

    Detection();
    Detection(
      const float x,
      const float y,
      const float width,
      const float height,
      const VType vehicleType = VType::undefined,
      const float confidence  = 0.f);
    Detection(const cv::Rect2d & old, const VType vehicleType = VType::undefined, const float confidence = 0.f);

    Detection & operator=(Detection other);
    friend std::istream & operator>>(std::istream & iStream, Detection & det);
    operator cv::Rect2d() const;

    VType getVehicleType() const;
    float getConfidence() const;

    static float iou(const QRectF & rect_1, const QRectF & rect_2);
};