#pragma once

#include <QQuickPaintedItem>


class Vehicle;


class Gate : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(QString name MEMBER m_name)

    QPointF m_startPos;
    QPointF m_endPos;
    QString m_name;

    int m_counter;
    int m_sumVehicleNr;
    std::vector<int> m_timelineCounter;

    friend class GateModel;

public:

    Gate(QQuickItem * parent = nullptr);

    Q_INVOKABLE void setStartPos(QPoint pos);
    Q_INVOKABLE void setEndPos(QPoint pos);

    QString getName() const;

    void initGate();
    void buildTimeline();
    void onFrameDisplayed(int frameIdx);
    int checkVehiclePass(Vehicle * vehicle_ptr);

private:

    virtual void paint(QPainter * painter) override;

    void setPainterForNumberDrawing(QPainter * painter, const float fontSize);
    void setPainterForLineDrawing(QPainter * painter, const float lineWidth);
};