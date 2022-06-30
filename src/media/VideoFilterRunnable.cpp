#include "VideoFilterRunnable.hpp"

#include <TrafficMapper/Media/MediaPlayer>
#include <TrafficMapper/Modules/VehicleModel>
#include <TrafficMapper/Types/Vehicle>


// ======================================================================
//   CONSTRUCTORS
// ======================================================================

VideoFilterRunnable::VideoFilterRunnable(VehicleModel * vehicleModel_ptr) : m_vehicleModel_ptr(vehicleModel_ptr)
{
    m_playbackSettings[0] = true;
    m_playbackSettings[1] = true;
    m_playbackSettings[2] = true;
    m_playbackSettings[3] = true;
}


// ======================================================================
//   PUBLIC FUNCTIONS
// ======================================================================

QVideoFrame
  VideoFilterRunnable::run(QVideoFrame * input, const QVideoSurfaceFormat & surfaceFormat, RunFlags flags)
{
    if (input->map(QAbstractVideoBuffer::ReadOnly)) {
        QImage frame(
          input->bits(),
          input->width(),
          input->height(),
          input->bytesPerLine(),
          QVideoFrame::imageFormatFromPixelFormat(input->pixelFormat()));

        const int frameIdx = std::round(input->startTime() * 0.000001 * MediaPlayer::m_videoMeta.getFPS());

        m_painter.begin(&frame);
        for (auto vehicle_ptr : m_vehicleModel_ptr->getVehiclesInFrame(frameIdx))
            vehicle_ptr->drawOnFrame(m_painter, frameIdx, m_playbackSettings);
        m_painter.end();

        emit frameDisplayed(frameIdx);
    }

    input->unmap();

    return *input;
}


// ======================================================================
//   PUBLIC SLOTS
// ======================================================================

void
  VideoFilterRunnable::onPlaybackOptionsSet(bool detection, bool trajectory, bool labelVType, bool labelSpeed)
{
    m_playbackSettings[0] = detection;
    m_playbackSettings[1] = trajectory;
    m_playbackSettings[2] = labelVType;
    m_playbackSettings[3] = labelSpeed;
}