#pragma once

#include <QPainter>
#include <QVideoFilterRunnable>

#include <bitset>


class VehicleModel;


class VideoFilterRunnable : public QObject, public QVideoFilterRunnable
{
    Q_OBJECT

    VehicleModel * m_vehicleModel_ptr;

    QPainter m_painter;

    std::bitset<4> m_playbackSettings;

public:

    VideoFilterRunnable(VehicleModel * tracker_ptr);

    virtual QVideoFrame run(QVideoFrame * input, const QVideoSurfaceFormat & surfaceFormat, RunFlags flags) override;

public slots:

    void onPlaybackOptionsSet(bool detection, bool trajectory, bool labelVType, bool labelSpeed);

signals:

    void frameDisplayed(int);
};