#include "VideoFilter.hpp"

#include <TrafficMapper/Media/VideoFilterRunnable>
#include <TrafficMapper/Modules/GateModel>


// ======================================================================
//   CONSTRUCTORS
// ======================================================================

VideoFilter::VideoFilter(QObject * parent)
  : QAbstractVideoFilter(parent), m_filter_ptr(nullptr), m_vehicleModel_ptr(nullptr)
{}

VideoFilter::VideoFilter(const VideoFilter & other)
{
    m_filter_ptr       = other.m_filter_ptr;
    m_vehicleModel_ptr = other.m_vehicleModel_ptr;
}


// ======================================================================
//   SETTERS & GETTERS
// ======================================================================

void
  VideoFilter::setVehicleModel(VehicleModel * vehicleModel_ptr)
{
    m_vehicleModel_ptr = vehicleModel_ptr;
}


// ======================================================================
//   PUBLIC FUNCTIONS
// ======================================================================

QVideoFilterRunnable *
  VideoFilter::createFilterRunnable()
{
    m_filter_ptr = new VideoFilterRunnable(m_vehicleModel_ptr);

    QObject::connect(m_filter_ptr, &VideoFilterRunnable::frameDisplayed, this, &VideoFilter::frameDisplayed);
    QObject::connect(this, &VideoFilter::setPlaybackOptions, m_filter_ptr, &VideoFilterRunnable::onPlaybackOptionsSet);

    return m_filter_ptr;
}