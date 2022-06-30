#include "MediaPlayer.hpp"

#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp>

#include <iostream>


// ======================================================================
//   CONSTRUCTORS
// ======================================================================

MediaPlayer::MediaPlayer(QObject * parent, Flags flags)
  : QMediaPlayer(parent, flags), m_positionLabel("00:00 / 00:00"), m_surface(nullptr)
{
    connect(this, &QMediaPlayer::mediaStatusChanged, this, &MediaPlayer::onMediaStatusChanged);
    connect(this, &QMediaPlayer::positionChanged, this, &MediaPlayer::onPositionChanged);
}


// ======================================================================
//   STATIC FUNCTIONS
// ======================================================================

void
  MediaPlayer::getRandomFrame(cv::Mat & frame)
{
    cv::VideoCapture video(m_videoMeta.URL.toString().toStdString());
    video.set(cv::CAP_PROP_POS_FRAMES, rand() % m_videoMeta.FRAMECOUNT);
    video >> frame;
}


// ======================================================================
//   SETTERS & GETTERS
// ======================================================================

QAbstractVideoSurface *
  MediaPlayer::getVideoSurface()
{
    return m_surface;
}

void
  MediaPlayer::setVideoSurface(QAbstractVideoSurface * surface)
{
    m_surface = surface;
    setVideoOutput(m_surface);
}

QString
  MediaPlayer::getPositionLabel() const
{
    return m_positionLabel;
}

int
  MediaPlayer::getFrameCount() const
{
    return m_videoMeta.FRAMECOUNT;
}


// ======================================================================
//   PUBLIC FUNCTIONS
// ======================================================================

Q_INVOKABLE void
  MediaPlayer::loadVideo(QUrl videoUrl)
{
    setMedia(videoUrl);
}


// ======================================================================
//   PRIVATE FUNCTIONS
// ======================================================================

inline void
  MediaPlayer::printVideoMetaData() const
{
    auto metaDataToPrint = QString("\n"
                                   "%8========================================================================%11\n"
                                   "%9                         VIDEO FILE OPENED                              %11\n"
                                   "%8========================================================================%11\n"
                                   "%10  File name        :%11 %1\n"
                                   "%10  Resolution       :%11 %2 x %3\n"
                                   "%10  FPS              :%11 %4\n"
                                   "%10  Length (msec)    :%11 %5\n"
                                   "%10  Length (min:sec) :%11 %6:%7\n"
                                   "%8========================================================================%11\n\n")
                             .arg(m_videoMeta.URL.toLocalFile())
                             .arg(m_videoMeta.WIDTH)
                             .arg(m_videoMeta.HEIGHT)
                             .arg(m_videoMeta.FPS)
                             .arg(m_videoMeta.LENGTH)
                             .arg(m_videoMeta.LENGTH_MIN, 2, 10, QChar('0'))
                             .arg(m_videoMeta.LENGTH_SEC, 2, 10, QChar('0'))
                             .arg("\u001b[38;5;215m")
                             .arg("\u001b[38;5;45m")
                             .arg("\u001b[38;5;45m")
                             .arg("\u001b[0m");

    qDebug() << metaDataToPrint.toStdString().c_str();
}

inline void
  MediaPlayer::setMetaData()
{
    cv::VideoCapture video = cv::VideoCapture(media().request().url().toLocalFile().toStdString().c_str());

    m_videoMeta.setURL(media().request().url());
    m_videoMeta.setFPS(video.get(cv::CAP_PROP_FPS));
    m_videoMeta.setFRAMECOUNT(video.get(cv::CAP_PROP_FRAME_COUNT));
    m_videoMeta.setWIDTH(video.get(cv::CAP_PROP_FRAME_WIDTH));
    m_videoMeta.setHEIGHT(video.get(cv::CAP_PROP_FRAME_HEIGHT));
    m_videoMeta.setLENGTH(duration());

    emit videoMetaChanged();
    emit positionChanged(0);
}


// ======================================================================
//   PRIVATE SLOTS
// ======================================================================

void
  MediaPlayer::onPositionChanged(qint64 position)
{
    const int currentMin = position / 60000;
    const int currentSec = (position - (currentMin * 60000)) / 1000;

    m_positionLabel = QString("%1:%2 / %3:%4")
                        .arg(currentMin, 2, 10, QChar('0'))
                        .arg(currentSec, 2, 10, QChar('0'))
                        .arg(m_videoMeta.LENGTH_MIN, 2, 10, QChar('0'))
                        .arg(m_videoMeta.LENGTH_SEC, 2, 10, QChar('0'));

    emit positionLabelChanged();
}

void
  MediaPlayer::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::LoadedMedia) {
        play();     // Necessary to display the first frame.
        pause();    // At least, I don't know any other method.

        setMetaData();
        printVideoMetaData();
    } else if (status == QMediaPlayer::InvalidMedia) {
        m_videoMeta.clear();

        emit videoMetaChanged();
        emit positionChanged(position());
    }
}