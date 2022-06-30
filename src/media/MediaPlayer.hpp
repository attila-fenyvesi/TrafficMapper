#pragma once

#include <TrafficMapper/Types/VideoMeta>

#include <QMediaPlayer>


class QAbstractVideoSurface;
namespace cv {
    class Mat;
}


class MediaPlayer : public QMediaPlayer
{
    Q_OBJECT

    Q_PROPERTY(QAbstractVideoSurface * videoSurface READ getVideoSurface WRITE setVideoSurface)

    Q_PROPERTY(QString positionLabel READ getPositionLabel NOTIFY positionLabelChanged)
    Q_PROPERTY(int frameCount READ getFrameCount NOTIFY videoMetaChanged)

    QAbstractVideoSurface * m_surface;

    QString m_positionLabel;

public:

    inline static VideoMeta m_videoMeta;

    MediaPlayer(QObject * parent = nullptr, Flags flags = 0);

    static void getRandomFrame(cv::Mat & frame);

    Q_INVOKABLE void loadVideo(QUrl videoUrl);

private:

    void setVideoSurface(QAbstractVideoSurface * surface);
    QAbstractVideoSurface * getVideoSurface();

    QString getPositionLabel() const;
    int getFrameCount() const;

    void printVideoMetaData() const;
    void setMetaData();

private slots:

    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onPositionChanged(qint64 position);

signals:

    void videoMetaChanged();
    void positionLabelChanged();
};