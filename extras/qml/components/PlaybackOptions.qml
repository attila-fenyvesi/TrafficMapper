import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.15

//import TrafficMapper 1.0

Item {
    // height: cbWrapper.height + title.font.pixelSize + 35
    implicitHeight: gbPlaybackOptions.height
    // height: 250

    GroupBox {
        id: gbPlaybackOptions
        title: "PLAYBACK OPTIONS"
        anchors.fill: parent
        topPadding: title.height + 8

        background: Rectangle {
            y: gbPlaybackOptions.topPadding - gbPlaybackOptions.bottomPadding
            width: parent.width
            height: parent.height - gbPlaybackOptions.topPadding + gbPlaybackOptions.bottomPadding
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
                text: gbPlaybackOptions.title
                color: "#444444"
                width: parent.width
                topPadding: 10
                bottomPadding: 10
                font.pixelSize: 12
                font.bold: true
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight
//                renderType: Text.NativeRendering
            }
        }

        Column {
            id: cbWrapper
            //anchors.fill: parent
            // y: title.font.pixelSize + 20
            
            // Rectangle {
            //     anchors.fill: parent
            //     color: "#444444"
            //     z: 2
            // }
            
            Switch {
                id: swDetections
                text: "Detections"
                checked: true
                onClicked: changePlaybackOptions()
            }
            
            Switch {
                id: swTrajectory
                text: "Trajectory"
                checked: true
                onClicked: changePlaybackOptions()
            }
            
            Switch {
                id: swLabelType
                text: "Label (vehicle type)"
                checked: true
                onClicked: changePlaybackOptions()
            }
            
            Switch {
                id: swLabelSpeed
                text: "Label (speed)"
                checked: true
                onClicked: changePlaybackOptions()
            }
        }
    }

    // Connections {
    //     target: mediaPlayer
    //     function onMediaStatusChanged(status) {
    //         if (status == 6)
    //             changePlaybackOptions()
    //     }
    // }

    function changePlaybackOptions() {
        videoFilter.setPlaybackOptions(
            swDetections.checked,
            swTrajectory.checked,
            swLabelType.checked,
            swLabelSpeed.checked
        );
    }
}
