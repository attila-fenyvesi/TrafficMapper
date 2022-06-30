#include "StatModel.hpp"

#include <QFile>
#include <QPrinter>
#include <QTextDocument>
#include <QWebEnginePage>
#include <QtConcurrent/QtConcurrent>

#include <TrafficMapper/Media/MediaPlayer>
#include <TrafficMapper/Modules/GateModel>
#include <TrafficMapper/Types/Gate>

#include <cppitertools/starmap.hpp>

#include <random>


StatModel::StatModel(QObject * parent)
  : QAbstractItemModel(parent), m_intervalNr(0), m_axis_Y_maxval_counter(5), m_axis_Y_maxval_speed(0),
    m_hasStatData(false)
{}


Q_INVOKABLE QModelIndex
  StatModel::index(int row, int column, const QModelIndex & parent) const
{
    return QAbstractItemModel::createIndex(row, column);
}

Q_INVOKABLE QModelIndex
  StatModel::parent(const QModelIndex & child) const
{
    return Q_INVOKABLE QModelIndex();
}

Q_INVOKABLE int
  StatModel::rowCount(const QModelIndex & parent) const
{
    return m_displayedData.size();
}

Q_INVOKABLE int
  StatModel::columnCount(const QModelIndex & parent) const
{
    return m_displayedData[0].size();
}

Q_INVOKABLE QVariant
  StatModel::data(const QModelIndex & index, int role) const
{
    int columnShift = 0;

    if (m_displaySpeed)
        columnShift = 5;

    return Q_INVOKABLE QVariant(m_displayedData[index.row()][index.column() + columnShift]);
}

QVariant
  StatModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    const static QStringList legend = { "Cars", "Buses", "Trucks", "Motorcycles", "Bicycles" };
    const static int legendSize     = legend.size();

    if (section >= 0 && section < legendSize * 2)
        return QVariant(legend[section % legendSize]);
    else
        return QVariant(QStringLiteral("unknown"));
}


Q_INVOKABLE void
  StatModel::setDisplaySpeed(bool displaySpeed)
{
    m_displaySpeed = displaySpeed;
}

Q_INVOKABLE void
  StatModel::setDisplayedGate(Gate * gate_ptr)
{
    m_displayedGate_ptr = gate_ptr;
}

Q_INVOKABLE void
  StatModel::setDisplayedIntervalSize(int intervalSize)
{
    m_displayedIntervalSize = intervalSize;
}

Q_INVOKABLE void
  StatModel::update()
{
    setupGraphTitle();
    generateDisplayData();
    setupXaxis();
    setupYaxis();
}


void
  StatModel::onAnalysisStarted()
{
    m_rawData.clear();
    m_displayedData.clear();

    m_hasStatData = false;
    emit statDataStatusChanged();
}

void
  StatModel::onVehiclePassedThroughGate(Vehicle * vehicle_ptr, Gate * gate_ptr, int frameIdx)
{
    if (gate_ptr == nullptr)
        return;

    const Vehicle::VehicleType vType = vehicle_ptr->getVehicleType();

    if (vType == Vehicle::VehicleType::undefined) {
        return;
    }

    // If there's no entry for this gate in the database, then create one.
    if (m_rawData.find(gate_ptr) == std::end(m_rawData)) {
        for (int i(0); i < 5; ++i) {
            m_rawData[gate_ptr][Vehicle::VehicleType(i)].resize(MediaPlayer::m_videoMeta.getFRAMECOUNT(), { 0, 0 });
        }
    }

    ++m_rawData[gate_ptr][vType][frameIdx].first;
    m_rawData[gate_ptr][vType][frameIdx].second += vehicle_ptr->getSpeedAt(frameIdx);

    if (!m_hasStatData) {
        m_hasStatData = true;
        emit statDataStatusChanged();
    }
}


inline void
  StatModel::setupXaxis()
{
    m_axisX_labels.clear();
    for (int i(1); i <= m_intervalNr; ++i) {
        const int minutes = i * m_displayedIntervalSize / 60;
        const int seconds = i * m_displayedIntervalSize % 60;

        m_axisX_labels << QString("%1:%2").arg(minutes).arg(seconds, 2, 10, QChar('0'));
    }

    emit axisXchanged();
}

inline void
  StatModel::setupYaxis()
{
    m_axis_Y_maxval_counter = 0;
    m_axis_Y_maxval_speed   = 0;

    for (int i(0); i < m_intervalNr; ++i) {
        int val = 0;

        for (int j(0); j < 5; ++j)
            val += m_displayedData[i][j];

        if (val > m_axis_Y_maxval_counter)
            m_axis_Y_maxval_counter = val;

        for (int j(5); j < 10; ++j)
            if (m_axis_Y_maxval_speed < m_displayedData[i][j])
                m_axis_Y_maxval_speed = m_displayedData[i][j];
    }

    emit axisYchanged();
}

inline void
  StatModel::setupGraphTitle()
{
    if (m_displayedGate_ptr == nullptr)
        m_graphTitle = QStringLiteral("Traffic statistics of all gates");
    else
        m_graphTitle = QStringLiteral("Traffic statistics of ") + m_displayedGate_ptr->getName();

    emit titleChanged();
}

inline void
  StatModel::generateDisplayData()
{
    const int intervalSize = std::ceil(MediaPlayer::m_videoMeta.getFPS() * m_displayedIntervalSize);

    m_intervalNr = std::ceil(MediaPlayer::m_videoMeta.getFRAMECOUNT() / (float) intervalSize);

    m_displayedData.clear();
    m_displayedData.resize(m_intervalNr);

    for (auto & dataRow : m_displayedData)
        dataRow.resize(10, 0);

    if (m_displayedGate_ptr == nullptr) {
        // If no gate pointer is provided, then summarize all gates' data.
        for (auto & gateData : m_rawData)
            fillDisplayedData(gateData.second);
    } else {
        // Otherwise use the provided gate's data.
        fillDisplayedData(m_rawData.at(m_displayedGate_ptr));
    }

    // // Let's calculate the average speeds.
    for (auto & dataRow : m_displayedData) {
        for (int vClass(5); vClass < 10; ++vClass) {
            const int vehicleNr = dataRow[vClass - 5];
            if (vehicleNr > 0)
                dataRow[vClass] /= vehicleNr;
        }
    }

    emit intervalChanged();
    emit dataChanged(createIndex(0, 0), createIndex(m_intervalNr - 1, 9));
}

inline void
  StatModel::fillDisplayedData(
    const std::unordered_map<Vehicle::VehicleType, std::vector<std::pair<int, int>>> & gateData)
{
    const int intervalSize = std::ceil(MediaPlayer::m_videoMeta.getFPS() * m_displayedIntervalSize);

    for (int vType(0); vType < 5; ++vType) {
        const auto & data = gateData.at(Vehicle::VehicleType(vType));

        for (int frameIdx(0); frameIdx < MediaPlayer::m_videoMeta.getFRAMECOUNT(); ++frameIdx) {
            m_displayedData[frameIdx / intervalSize][vType] += data[frameIdx].first;
            m_displayedData[frameIdx / intervalSize][vType + 5] += data[frameIdx].second;
        }
    }
}


int
  StatModel::axisYmaxval() const
{
    return m_displaySpeed ? m_axis_Y_maxval_speed : m_axis_Y_maxval_counter;
}
