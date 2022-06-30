#include "GateModel.hpp"

#include <TrafficMapper/Tools/Helpers>
#include <TrafficMapper/Types/Gate>

#include <cppitertools/enumerate.hpp>


// ======================================================================
//   CONSTRUCTORS
// ======================================================================

GateModel::GateModel(QObject * parent) : QAbstractListModel(parent) {}


// ======================================================================
//   INHERITED FUNCTIONS via QAbstractListModel
// ======================================================================

Q_INVOKABLE int
  GateModel::rowCount(const QModelIndex & parent) const
{
    return m_gateList.size();
}

Q_INVOKABLE QVariant
  GateModel::data(const QModelIndex & index, int role) const
{
    if (!index.isValid())
        return QVariant();

    Gate * item;
    try {
        item = m_gateList.at(index.row());
    } catch (const std::out_of_range &) {
        Helpers::debugMessage("GateModel", "Requested gate index is out of range!", Helpers::DBG_WARNING);
        return QVariant();
    }

    switch (role) {
        case StartPosRole:
            return QVariant(item->m_startPos);
        case EndPosRole:
            return QVariant(item->m_endPos);
        case NameRole:
            return QVariant(item->m_name);
        case CounterRole:
            return QVariant(item->m_counter);
        case SumRole:
            return QVariant(item->m_sumVehicleNr);
    }

    return QVariant();
}

Q_INVOKABLE Qt::ItemFlags
  GateModel::flags(const QModelIndex & index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable;
}

QHash<int, QByteArray>
  GateModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[StartPosRole] = "startPos";
    names[EndPosRole]   = "endPos";
    names[NameRole]     = "name";
    names[CounterRole]  = "counter";
    names[SumRole]      = "sumVehicleNr";

    return names;
}

void
  GateModel::insertData(Gate * newGate)
{
    const int index = m_gateList.size();

    emit beginInsertRows(QModelIndex(), index, index);
    m_gateList.push_back(newGate);
    emit endInsertRows();
}

void
  GateModel::removeData(const int & index)
{
    const int itemCount = m_gateList.size();

    if (index < 0 || index >= itemCount)
        return;

    emit beginRemoveRows(QModelIndex(), index, index);
    delete m_gateList[index];
    m_gateList.erase(m_gateList.begin() + index);
    emit endRemoveRows();
}

Gate *
  GateModel::getData(const int & index)
{
    try {
        return m_gateList.at(index);
    } catch (const std::out_of_range &) {
        return nullptr;
    }
}


// ======================================================================
//   SETTERS & GETTERS
// ======================================================================

void
  GateModel::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::LoadingMedia) {
        emit beginRemoveRows(QModelIndex(), 0, m_gateList.size() - 1);

        for (auto gate_ptr : m_gateList)
            delete gate_ptr;
        m_gateList.clear();

        emit endRemoveRows();
    }
}

void
  GateModel::onAnalysisStarted()
{
    for (auto gate_ptr : m_gateList)
        gate_ptr->initGate();
}

void
  GateModel::onVehiclePostProcessingFinished(VehicleList vehicles)
{
    Helpers::debugMessage("GateModel", "Checking vehicles passing through gates...");

    for (auto vehicle_ptr : vehicles) {
        for (auto && [idx, gate] : iter::enumerate(m_gateList)) {
            const int frameIdx = gate->checkVehiclePass(vehicle_ptr);

            if (frameIdx > 0) {
                emit dataChanged(createIndex(idx, 0), createIndex(idx, 0), QVector<int>() << SumRole);
                emit vehiclePassedThroughGate(vehicle_ptr, gate, frameIdx);
            }
        }
    }

    for (auto gate_ptr : m_gateList)
        gate_ptr->buildTimeline();

    Helpers::debugMessage("GateModel", "Vehicle checking finished.");
}

void
  GateModel::onFrameDisplayed(int frameIdx)
{
    for (auto & gate_ptr : m_gateList)
        gate_ptr->onFrameDisplayed(frameIdx);
}