import QtQuick 2.12
import QtMultimedia 5.15
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.15

import TrafficMapper 1.0

import "../custom-items"

Item {
    id: videoPlayer    

    property real aspectRatio: 16 / 9
    property bool isVideoLoaded: false
    property Gate currentGateItem: null

    property alias videoWrapper: videoWrapper
    property alias videoOutput: videoOutput

    Item {
        id: videoWrapper
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        width: (parent.width / parent.height) > aspectRatio ? parent.height * aspectRatio : parent.width
        height: width / aspectRatio

        Image {
            anchors.fill: parent
            source: "qrc:/img/placeholder.png"
        }

        VideoOutput {
            id: videoOutput
            source: mediaPlayer
            anchors.fill: parent
            filters: [videoFilter]

            Connections {
                target: mediaPlayer
                function onMediaStatusChanged(status) {
                    if (status === MediaPlayer.InvalidMedia) {
                        videoControls.slider.to = 10
                        videoControls.slider.value = 0
                        dlgVideoLoadingError.visible = true
                        isVideoLoaded = false
                    } else if (status === MediaPlayer.Loaded) {
                        videoControls.slider.to = mediaPlayer.duration
                        isVideoLoaded = true
                    }
                }
                function onStateChanged(state) {
                    if (state === MediaPlayer.PlayingState)
                        videoControls.btnPlay.icon.source = "qrc:/svg/media-pause.svg"
                    else
                        videoControls.btnPlay.icon.source = "qrc:/svg/media-play.svg"
                }
                function onPositionChanged(position) {
                    videoControls.slider.value = position
                }
            }
        }
    
        Item {
            id: gateContainer
            anchors.fill: parent

            MouseArea{
                id: gateMouseArea
                anchors.fill: parent

                onPressed: {
                   if (gateList.isGatePlacingEnabled) {
                       currentGateItem = Qt.createQmlObject('import TrafficMapper 1.0; Gate {}', gateContainer)

                       currentGateItem.anchors.fill = gateContainer
                       currentGateItem.setStartPos(Qt.point(mouseX, mouseY))
                       currentGateItem.setEndPos(Qt.point(mouseX, mouseY))
                   }
                }
                onPositionChanged: {
                   if (gateList.isGatePlacingEnabled)
                       currentGateItem.setEndPos(Qt.point(mouseX, mouseY))
                }
                onReleased: {
                   if (gateList.isGatePlacingEnabled) {
                       currentGateItem.setEndPos(Qt.point(mouseX, mouseY))
                       gatePropertiesDialog.open()
                       gateList.isGatePlacingEnabled = false
                   }
                }
            }
        }
    }

    MessageDialog {
        id: dlgVideoLoadingError
        title: "Video file could not be loaded!"
        icon: StandardIcon.Critical
        text: "The video file may be invalid or the required codecs are not installed!"
        standardButtons: StandardButton.Ok
    }
}
