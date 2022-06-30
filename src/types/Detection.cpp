#include "Detection.hpp"

#include <TrafficMapper/Media/MediaPlayer>

#include <filesystem>


// ======================================================================
//   CONSTRUCTORS
// ======================================================================

Detection::Detection() : QRectF(), m_vehicleType(VType::undefined), m_confidence(0) {}

Detection::Detection(
  const float x,
  const float y,
  const float width,
  const float height,
  const VType vehicleType,
  const float confidence)
  : QRectF(x, y, width, height), m_vehicleType(vehicleType), m_confidence(confidence)
{}

Detection::Detection(const cv::Rect2d & old, const VType vehicleType, const float confidence)
  : QRectF(old.x, old.y, old.width, old.height), m_vehicleType(vehicleType),
    m_confidence(confidence)
{}


// ======================================================================
//   OPERATORS
// ======================================================================

Detection &
  Detection::operator=(Detection other)
{
    if (this != &other) {
        setX(other.x());
        setY(other.y());
        setWidth(other.width());
        setHeight(other.height());
        std::swap(m_confidence, other.m_confidence);
        std::swap(m_vehicleType, other.m_vehicleType);
    }

    return *this;
}

std::istream &
  operator>>(std::istream & iStream, Detection & det)
{
    float x, y, width, height;

    iStream >> det.m_vehicleType >> det.m_confidence >> x >> y >> width >> height;

    const int videoWidth  = MediaPlayer::m_videoMeta.getWIDTH();
    const int videoHeight = MediaPlayer::m_videoMeta.getHEIGHT();

    x *= videoWidth;
    y *= videoHeight;
    width *= videoWidth;
    height *= videoHeight;

    det.setX(x);
    det.setY(y);
    det.setWidth(width);
    det.setHeight(height);

    return iStream;
}

Detection::operator cv::Rect2d() const
{
    return cv::Rect2d(x(), y(), width(), height());
}


// ======================================================================
//   SETTERS & GETTERS
// ======================================================================

Detection::VType
  Detection::getVehicleType() const
{
    return m_vehicleType;
}

float
  Detection::getConfidence() const
{
    return m_confidence;
}


// ======================================================================
//   STATIC FUNCTIONS
// ======================================================================

float
  Detection::iou(const QRectF & rect_1, const QRectF & rect_2)
{
    const QRectF intersection = rect_1 & rect_2;

    if (intersection.isEmpty())
        return 0.f;

    const float area_int    = intersection.width() * intersection.height();
    const float area_rect_1 = rect_1.width() * rect_1.height();
    const float area_rect_2 = rect_2.width() * rect_2.height();

    return area_int / (area_rect_1 + area_rect_2 - area_int);
}