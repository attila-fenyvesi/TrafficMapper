#pragma once

#include <QAbstractListModel>
#include <QMediaPlayer>


class Gate;
class Vehicle;


class GateModel : public QAbstractListModel
{
    Q_OBJECT

    using VehicleList = std::vector<Vehicle *>;

    std::vector<Gate *> m_gateList;

public:

    enum
    {
        StartPosRole = Qt::UserRole + 1,
        EndPosRole,
        NameRole,
        CounterRole,
        SumRole
    };

    GateModel(QObject * parent = nullptr);

    // Inherited via QAbstractListModel
    virtual Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    virtual Q_INVOKABLE QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    virtual Q_INVOKABLE Qt::ItemFlags flags(const QModelIndex & index) const override;
    virtual QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void insertData(Gate * newGate);
    Q_INVOKABLE void removeData(const int & index);
    Q_INVOKABLE Gate * getData(const int & index);

public slots:

    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onAnalysisStarted();
    void onVehiclePostProcessingFinished(VehicleList vehicles);
    void onFrameDisplayed(int frameIdx);

signals:

    void vehiclePassedThroughGate(Vehicle *, Gate *, int);
};