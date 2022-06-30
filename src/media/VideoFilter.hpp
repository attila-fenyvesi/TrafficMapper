#pragma once

#include <QAbstractVideoFilter>


class VideoFilterRunnable;
class VehicleModel;


class VideoFilter : public QAbstractVideoFilter
{
    Q_OBJECT

    VideoFilterRunnable * m_filter_ptr;
    VehicleModel * m_vehicleModel_ptr;

public:

    VideoFilter(QObject * parent = nullptr);
    VideoFilter(const VideoFilter & other);

    virtual QVideoFilterRunnable * createFilterRunnable() override;

    void setVehicleModel(VehicleModel * vehicleModel_ptr);

signals:

    void setPlaybackOptions(bool, bool, bool, bool);
    void frameDisplayed(int);
};