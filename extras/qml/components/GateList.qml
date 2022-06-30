import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.15

import TrafficMapper 1.0

import "../custom-items"

Item {

    property alias isGatePlacingEnabled: btnAddGate.checked
    property alias gateCount: gateList.count

    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        GroupBox {
            id: gbGateList
            title: "GATES"
            topPadding: title.height + 8
            width: parent.width
            Layout.fillWidth: true
            Layout.fillHeight: true

            background: Rectangle {
                y: gbGateList.topPadding - gbGateList.bottomPadding
                width: parent.width
                height: parent.height - gbGateList.topPadding + gbGateList.bottomPadding
                border.color: "#AAAAAA"
                color: "transparent"
            }
            label: Rectangle {
                width: parent.width
                height: title.height
                border.color: "#AAAAAA"
                color: "transparent"

                Text {
                    id: title
                    text: gbGateList.title
                    color: "#444444"
                    width: parent.width
                    topPadding: 10
                    bottomPadding: 10
                    font.pixelSize: 12
                    font.bold: true
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideRight
                    //renderType: Text.NativeRendering
                }
            }

            ListView {
                id: gateList
                anchors.fill: parent
                model: gateModel
                focus: true

                delegate: Rectangle {
                    width: parent.width
                    height: 40
                    color: (index % 2 == 0) ? "#DDDDDD" : "#EEEEEE"

                    Label {
                        x: 5; y: 5
                        text: "Name: " + name
                    }

                    Label {
                        x: 5; y: 20
                        text: "Passed vehicles: " + sumVehicleNr
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: gateList.currentIndex = index
                    }
                }

                highlight: Rectangle {
                    border.color: "#cd5555"
                    border.width: 1
                    color: "transparent"
                    z: 2
                }
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 30

            RowLayout {
                anchors.fill: parent
                spacing: 10

                CustomButton {
                    id: btnAddGate
                    text: "ADD GATE"
                    enabled: videoPlayer.isVideoLoaded
                    checkable: true
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                CustomButton {
                    id: btnDeleteGate
                    text: "DELETE GATE"
                    enabled: videoPlayer.isVideoLoaded && gateList.count > 0
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    onClicked: gateModel.removeData(gateList.currentIndex)
                }
            }
        }
    }
}
