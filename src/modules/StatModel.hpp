#pragma once

#include <TrafficMapper/Types/Vehicle>

#include <QAbstractItemModel>


class Gate;
class GateModel;
class VehicleModel;


class StatModel : public QAbstractItemModel
{
    Q_OBJECT

    Q_PROPERTY(QString graphTitle MEMBER m_graphTitle NOTIFY titleChanged)
    Q_PROPERTY(int intervalNr MEMBER m_intervalNr NOTIFY intervalChanged)
    Q_PROPERTY(QStringList axis_X_labels MEMBER m_axisX_labels NOTIFY axisXchanged)
    Q_PROPERTY(int axis_Y_maxval READ axisYmaxval NOTIFY axisYchanged)
    Q_PROPERTY(bool hasStatData MEMBER m_hasStatData NOTIFY statDataStatusChanged)

    bool m_hasStatData;
    QString m_graphTitle;
    int m_intervalNr;
    int m_axis_Y_maxval_counter;
    int m_axis_Y_maxval_speed;
    QStringList m_axisX_labels;

    bool m_displaySpeed;
    Gate * m_displayedGate_ptr;
    int m_displayedIntervalSize;

    std::unordered_map<Gate *, std::unordered_map<Vehicle::VehicleType, std::vector<std::pair<int, int>>>> m_rawData;
    std::vector<std::vector<int>> m_displayedData;

public:

    StatModel(QObject * parent = nullptr);

    // Inherited via QAbstractItemModel
    virtual Q_INVOKABLE QModelIndex
      index(int row, int column, const QModelIndex & parent = QModelIndex()) const override;
    virtual Q_INVOKABLE QModelIndex parent(const QModelIndex & child) const override;
    virtual Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    virtual Q_INVOKABLE int columnCount(const QModelIndex & parent = QModelIndex()) const override;
    virtual Q_INVOKABLE QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE QVariant
      headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE void setDisplaySpeed(bool displaySpeed);
    Q_INVOKABLE void setDisplayedGate(Gate * gate_ptr);
    Q_INVOKABLE void setDisplayedIntervalSize(int intervalSize);
    Q_INVOKABLE void update();

public slots:

    void onAnalysisStarted();
    void onVehiclePassedThroughGate(Vehicle * vehicle_ptr, Gate * gate_ptr, int frameIdx);

private:

    inline void setupXaxis();
    inline void setupYaxis();
    inline void setupGraphTitle();
    inline void generateDisplayData();
    inline void fillDisplayedData(
      const std::unordered_map<Vehicle::VehicleType, std::vector<std::pair<int, int>>> & gateData);

    int axisYmaxval() const;

signals:

    void titleChanged();
    void intervalChanged();
    void axisXchanged();
    void axisYchanged();
    void statDataStatusChanged();
    void reportUpdated();
};
