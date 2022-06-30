#include "Gate.hpp"

#include <TrafficMapper/Media/MediaPlayer>
#include <TrafficMapper/Tools/Helpers>
#include <TrafficMapper/Types/Vehicle>

#include <QPainter>


// ======================================================================
//   CONSTRUCTORS
// ======================================================================

Gate::Gate(QQuickItem * parent) : QQuickPaintedItem(parent), m_counter(0), m_sumVehicleNr(0)
{
    initGate();
}


// ======================================================================
//   SETTERS & GETTERS
// ======================================================================

void
  Gate::setStartPos(QPoint pos)
{
    m_startPos = QPointF(pos.x() / width(), pos.y() / height());
}

void
  Gate::setEndPos(QPoint pos)
{
    m_endPos = QPointF(pos.x() / width(), pos.y() / height());

    update();
}

QString
  Gate::getName() const
{
    return m_name;
}


// ======================================================================
//   PUBLIC FUNCTIONS
// ======================================================================

void
  Gate::initGate()
{
    m_counter      = 0;
    m_sumVehicleNr = 0;

    m_timelineCounter.clear();
    m_timelineCounter.resize(MediaPlayer::m_videoMeta.getFRAMECOUNT(), 0);
}

void
  Gate::buildTimeline()
{
    for (int i(1); i < m_timelineCounter.size(); ++i) {
        m_timelineCounter[i] = m_timelineCounter[i - 1] + m_timelineCounter[i];
    }
}

void
  Gate::onFrameDisplayed(int frameIdx)
{
    try {
        m_counter = m_timelineCounter.at(frameIdx);
    } catch (const std::out_of_range &) {
        Helpers::debugMessage(
          "Gate", QString("Invalid timeline index: %1").arg(frameIdx), Helpers::DBG_WARNING);
        m_counter = -1;
    };

    update();
}

int
  Gate::checkVehiclePass(Vehicle * vehicle_ptr)
{
    QLineF gateLine(
      QPointF(
        m_startPos.x() * MediaPlayer::m_videoMeta.getWIDTH(),
        m_startPos.y() * MediaPlayer::m_videoMeta.getHEIGHT()),
      QPointF(
        m_endPos.x() * MediaPlayer::m_videoMeta.getWIDTH(),
        m_endPos.y() * MediaPlayer::m_videoMeta.getHEIGHT()));

    for (const auto & pathSegment : vehicle_ptr->getTrajectory()) {
        if (gateLine.intersects(pathSegment.second, nullptr) == QLineF::BoundedIntersection) {
            ++m_sumVehicleNr;
            ++m_timelineCounter[pathSegment.first];
            return pathSegment.first;
        }
    }

    return -1;
}


// ======================================================================
//   PRIVATE FUNCTIONS
// ======================================================================

void
  Gate::paint(QPainter * painter)
{
    painter->setRenderHint(QPainter::Antialiasing);

    const float lineWidth = width() * 0.014f;
    const float fontSize  = width() * 0.020f;

    const QPointF startPoint(m_startPos.x() * width(), m_startPos.y() * height());
    const QPointF endPoint(m_endPos.x() * width(), m_endPos.y() * height());

    QPointF labelCenter = startPoint - (startPoint - endPoint) * 0.5f;
    QPointF offset(30, 10);
    QRectF counterPos(labelCenter - offset, labelCenter + offset);

    setPainterForLineDrawing(painter, lineWidth);
    painter->drawLine(startPoint, endPoint);

    setPainterForNumberDrawing(painter, fontSize);
    painter->drawText(counterPos, Qt::AlignCenter, QString::number(m_counter));
}

inline void
  Gate::setPainterForNumberDrawing(QPainter * painter, const float fontSize)
{
    QFont font = painter->font();
    font.setPointSize(fontSize);
    font.setBold(true);
    QPen pen;
    pen.setColor(Qt::white);

    painter->setPen(pen);
    painter->setFont(font);
    painter->setOpacity(1.0);
}

inline void
  Gate::setPainterForLineDrawing(QPainter * painter, const float lineWidth)
{
    QPen pen;
    pen.setColor(QColor("#CD5555"));
    pen.setWidth(lineWidth);
    pen.setCapStyle(Qt::RoundCap);

    painter->setPen(pen);
    painter->setOpacity(0.7);
}
