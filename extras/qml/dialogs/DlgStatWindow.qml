import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtCharts 2.15
import QtGraphicalEffects 1.15
import QtWebView 1.15

//import TrafficMapper 1.0

import "../custom-items"

Window {
    id: statWindow
    width: 1000
    height: 700
    title: "Traffic statistics"
    visible: false
    // standardButtons: Dialog.NoButton

    function initAndOpen() {
        proxyGateModel.clear()
        proxyGateModel.append({ modelData: "Show all" })
        for (var i = 0; i < gateModel.rowCount(); i++) {
            proxyGateModel.append({ modelData: gateModel.getData(i).name })
        }
        cbGate.currentIndex = 0

        statModel.setDisplaySpeed(tabBar.currentIndex == 1)
        statModel.setDisplayedGate(gateModel.getData(cbGate.currentIndex - 1))
        statModel.setDisplayedIntervalSize(intervalOptions.get(cbInterval.currentIndex).value)
        statModel.update()

       // setFilters()

        show()
    }

    function setFilters() {
        proxyModel.filter(
            cbFilterCar.checked,
            cbFilterBus.checked,
            cbFilterTruck.checked,
            cbFilterMotorcycle.checked,
            cbFilterBicycle.checked
        )
    }

    function changeTab() {
        statModel.setDisplaySpeed(tabBar.currentIndex == 1)
        statModel.update()
    }

    Rectangle {
        anchors.fill: parent
        color: "#eeeeee"
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        TabBar {
            id: tabBar
            Layout.fillWidth: true
            contentHeight: 30
            spacing: 10
            background: Rectangle { color: "transparent" }

            CustomTabButton {
                text: "Gate vehicle counts"
                tabBar: parent
                tabBarPosition: 0
                onClicked: changeTab()
            }

            CustomTabButton {
                text: "Gate speed statistics"
                enabled: dlgCameraCalibration.isCameraCalibrated
                tabBar: parent
                tabBarPosition: 1
                onClicked: changeTab()
            }
        }

        RowLayout {
            spacing: 10

            Label {
                height: 30
                verticalAlignment: Text.AlignVCenter
                text: "Gate:"
            }

            CustomComboBox {
                id: cbGate
                width: 150
                height: 30
                model: ListModel { id: proxyGateModel }

                onActivated: function(index) {
                    statModel.setDisplayedGate(gateModel.getData(index - 1))
                    statModel.update()
                }
            }

            Label {
                height: 30
                verticalAlignment: Text.AlignVCenter
                text: "Interval:"
            }

            CustomComboBox {
                id: cbInterval
                width: 100
                height: 30
                textRole: "key"
                model: ListModel {
                    id: intervalOptions
                    ListElement { key: "10 seconds"; value: 10 }
                    ListElement { key: "30 seconds"; value: 30 }
                    ListElement { key: "1 minute"; value: 60 }
                }

                onActivated: function(index) {
                    statModel.setDisplayedIntervalSize(intervalOptions.get(index).value)
                    statModel.update()
                }
            }

            CheckBox {
                id: cbShowLabels
                height: 30
                checked: true
                visible: tabBar.currentIndex == 0
                text: "Show bar labels"
            }

            Item {
                Layout.fillWidth: true
            }
 
            CustomButton {
                text: "Save chart..."
                visible: tabBar.currentIndex < 2
                onClicked: {
                    proxyModel.setupYaxis()
                }
                // onClicked: dlgSaveChart.open()
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: tabBar.currentIndex == 0

            DropShadow {
                anchors.fill: chartGateCounter
                horizontalOffset: 0
                verticalOffset: 0
                radius: 5.0
                samples: 17
                color: "#80000000"
                source: chartGateCounter
            }

            ChartView {
                id: chartGateCounter
                anchors.fill: parent
                anchors.margins: -8 // Hogy ez mifaszért kell ide, azt nem tudom...
                antialiasing: true
                legend.alignment: Qt.AlignBottom
                legend.font.pointSize: 10
                legend.font.bold: true
                title: statModel.graphTitle
                titleFont.pointSize: 11
                titleFont.bold: true
                titleFont.letterSpacing: 2
                backgroundRoundness : 0

                StackedBarSeries {
                    visible: tabBar.currentIndex == 0
                    labelsVisible: cbShowLabels.checked
                    axisX: BarCategoryAxis {
                        categories: statModel.axis_X_labels
                        labelsAngle: -90
                    }
                    axisY: ValueAxis {
                        // max: proxyModel.axisY_maxval
                        max: statModel.axis_Y_maxval
                    }

                    VBarModelMapper {
                        id: modelMapper
                        // model: proxyModel
                        model: statModel
                        firstBarSetColumn: 0
                        lastBarSetColumn: 4
                        firstRow: 0
                        rowCount: statModel.intervalNr
                    }
                }
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: tabBar.currentIndex == 1

            DropShadow {
                anchors.fill: chartGateCounter_2
                horizontalOffset: 0
                verticalOffset: 0
                radius: 5.0
                samples: 17
                color: "#80000000"
                source: chartGateCounter_2
            }

            ChartView {
                id: chartGateCounter_2
                anchors.fill: parent
                anchors.margins: -8 // Meg ide is...
                antialiasing: true
                legend.alignment: Qt.AlignBottom
                legend.font.pointSize: 10
                legend.font.bold: true
                title: statModel.graphTitle
                titleFont.pointSize: 11
                titleFont.bold: true
                titleFont.letterSpacing: 2
                backgroundRoundness : 0

                BarSeries {
                    labelsVisible: cbShowLabels.checked
                    axisX: BarCategoryAxis {
                        categories: statModel.axis_X_labels
                        labelsAngle: -90
                    }
                    axisY: ValueAxis {
                        // max: proxyModel.axisY_maxval
                        max: statModel.axis_Y_maxval
                    }

                    VBarModelMapper {
                        id: modelMapper_2
                        // model: proxyModel
                        model: statModel
                        firstBarSetColumn: 0
                        lastBarSetColumn: 4
                        firstRow: 0
                        rowCount: statModel.intervalNr
                    }
                }
            }
        }
    }

    DlgSaveChart { id: dlgSaveChart }
}
