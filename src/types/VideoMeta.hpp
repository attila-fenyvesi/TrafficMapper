#pragma once

#include <QObject>
#include <QUrl>

struct VideoMeta
{
    Q_GADGET

    Q_PROPERTY(QUrl url READ getURL NOTIFY videoMetaChanged)
    Q_PROPERTY(int width READ getWIDTH NOTIFY videoMetaChanged)
    Q_PROPERTY(int height READ getHEIGHT NOTIFY videoMetaChanged)
    Q_PROPERTY(float fps READ getFPS NOTIFY videoMetaChanged)
    Q_PROPERTY(int length READ getLENGTH NOTIFY videoMetaChanged)
    Q_PROPERTY(int length_min READ getLENGTH_MIN NOTIFY videoMetaChanged)
    Q_PROPERTY(int length_sec READ getLENGTH_SEC NOTIFY videoMetaChanged)
    Q_PROPERTY(int framecount READ getFRAMECOUNT NOTIFY videoMetaChanged)

    friend class MediaPlayer;

public:

    QUrl getURL() const;
    float getFPS() const;
    int getFRAMECOUNT() const;
    int getWIDTH() const;
    int getHEIGHT() const;
    int getLENGTH() const;
    int getLENGTH_MIN() const;
    int getLENGTH_SEC() const;

    void setURL(QUrl url);
    void setFPS(float fps);
    void setFRAMECOUNT(int frameCount);
    void setWIDTH(int width);
    void setHEIGHT(int height);
    void setLENGTH(int length);

private:

    QUrl URL       = QUrl();
    float FPS      = 0.f;
    int FRAMECOUNT = 0;
    int WIDTH      = 0;
    int HEIGHT     = 0;
    int LENGTH     = 0;
    int LENGTH_MIN = 0;
    int LENGTH_SEC = 0;

    void clear();
};

Q_DECLARE_METATYPE(VideoMeta)