import QtQuick 2.12
import QtQuick.Controls 2.12

CheckBox {
    id: control
    text: qsTr("CheckBox")
    checked: true
    spacing: 0

    property color color: "#333333"

    states: [
        State {
            name: "Checked"; when: control.checkState == Qt.Checked
            PropertyChanges { target: bullet_1; visible: true }
            PropertyChanges { target: bullet_2; visible: false }
        },
        State {
            name: "PartiallyChecked"; when: control.checkState == Qt.PartiallyChecked
            PropertyChanges { target: bullet_1; visible: true; width: 10; height: 10; opacity: 0.7}
            PropertyChanges { target: bullet_2; visible: true }
        },
        State {
            name: "Unchecked"; when: control.checkState == Qt.Unchecked
            PropertyChanges { target: bullet_1; visible: false }
            PropertyChanges { target: bullet_2; visible: false }
        }
    ]

    indicator: CustomItemBackground {
        implicitWidth: 16
        implicitHeight: 16
        anchors.verticalCenter: parent.verticalCenter
        radius: 2

        Rectangle {
            id: bullet_1
            width: 12
            height: 12
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            radius: 2
            color: control.color
        }

        Rectangle {
            id: bullet_2
            width: 14
            height: 4
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            color: "white"
            transform: Rotation { origin.x: 7; origin.y: 2; angle: -45}
        }
    }

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: control.color
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
        renderType: Text.NativeRendering
    }
}