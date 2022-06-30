#include "VideoMeta.hpp"


QUrl
  VideoMeta::getURL() const
{
    return URL;
}

float
  VideoMeta::getFPS() const
{
    return FPS;
}

int
  VideoMeta::getFRAMECOUNT() const
{
    return FRAMECOUNT;
}

int
  VideoMeta::getWIDTH() const
{
    return WIDTH;
}

int
  VideoMeta::getHEIGHT() const
{
    return HEIGHT;
}

int
  VideoMeta::getLENGTH() const
{
    return LENGTH;
}

int
  VideoMeta::getLENGTH_MIN() const
{
    return LENGTH_MIN;
}

int
  VideoMeta::getLENGTH_SEC() const
{
    return LENGTH_SEC;
}

void
  VideoMeta::setURL(QUrl url)
{
    URL = url;
}

void
  VideoMeta::setFPS(float fps)
{
    FPS = fps;
}

void
  VideoMeta::setFRAMECOUNT(int frameCount)
{
    FRAMECOUNT = frameCount;
}

void
  VideoMeta::setWIDTH(int width)
{
    WIDTH = width;
}

void
  VideoMeta::setHEIGHT(int height)
{
    HEIGHT = height;
}

void
  VideoMeta::setLENGTH(int length)
{
    LENGTH     = length;
    LENGTH_MIN = LENGTH / 60000;
    LENGTH_SEC = (LENGTH - (LENGTH_MIN * 60000)) / 1000;
}


void
  VideoMeta::clear()
{
    URL.clear();
    FPS        = 0.f;
    FRAMECOUNT = 0;
    WIDTH      = 0;
    HEIGHT     = 0;
    LENGTH     = 0;
    LENGTH_MIN = 0;
    LENGTH_SEC = 0;
}
