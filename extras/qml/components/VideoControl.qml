import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import TrafficMapper 1.0

import "../custom-items"

Item {
    id: videoControls
    state: "embedded"
    states: [
        State {
            name: "fullscreen"
            ParentChange { target: videoPlayer.videoOutput; parent: videoWindowWrapper }
            ParentChange { target: videoControls; parent: videoWindowWrapper }
            PropertyChanges { target: videoControls; anchors.bottom: parent.bottom; anchors.left: parent.left; anchors.right: parent.right; anchors.margins: 10 }
            PropertyChanges { target: btnFullScreen; icon.source: "qrc:/svg/minimize.svg" }
        },
        State {
            name: "embedded"
            PropertyChanges { target: videoControls; anchors.bottom: false; anchors.left: false; anchors.right: false }
            ParentChange { target: videoPlayer.videoOutput; parent: videoPlayer.videoWrapper }
            ParentChange { target: videoControls; parent: playerWrapper }
            PropertyChanges { target: btnFullScreen; icon.source: "qrc:/svg/fullscreen.svg" }
        }
    ]

    property alias slider: slider
    property alias btnPlay: btnPlay

    RowLayout {
        anchors.fill: parent
        spacing: 10

        CustomButton {
            id: btnMute
            Layout.fillHeight: true
            Layout.preferredWidth: height
            icon.source: checked ? "qrc:/svg/sound-mute.svg" : "qrc:/svg/sound-on.svg"
            checkable: true
            enabled: videoPlayer.isVideoLoaded
            background: CustomItemBackground {}

            onClicked: mediaPlayer.muted = checked
        }

        CustomButton {
            id: btnPlay
            Layout.fillHeight: true
            Layout.preferredWidth: height
            icon.source: "qrc:/svg/media-play.svg"
            enabled: videoPlayer.isVideoLoaded
            background: CustomItemBackground {}

            onClicked: switchPlayState()
        }

        ProgressBar {
            id: slider
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.topMargin: (parent.height - positionLabel.font.pixelSize) / 2 + 1
            Layout.bottomMargin: (parent.height - positionLabel.font.pixelSize) / 2 + 1
            from: 0
            to: 10
            value: 0
            background: CustomItemBackground {
                anchors.fill: parent
                Rectangle {
                    x: 1; y: 1
                    height: parent.height - 2
                    color: "#B0CEFF"
                }
            }
            contentItem: Rectangle {
                anchors.left: parent.left
                width: parent.visualPosition * parent.width
                height: parent.height
                radius: 2
                color: "#CD5555"
                z: 2
            }

            MouseArea {
                anchors.fill: parent

                property bool isSeeking: false

                onPressed: {
                    if (!videoPlayer.isVideoLoaded) return;
                    isSeeking = true
                    mediaPlayer.setPosition(mediaPlayer.duration * mouseX / width)
                }
                onPositionChanged: {
                    if (isSeeking && mediaPlayer.seekable)
                        mediaPlayer.setPosition(mediaPlayer.duration * mouseX / width)
                }
                onReleased: {
                    isSeeking = false
                }
            }
        }

        Text {
            id: positionLabel
            Layout.fillHeight: true
            width: 100
            verticalAlignment: Text.AlignVCenter
            text: mediaPlayer.positionLabel
            font.pixelSize: 12
        }

        CustomButton {
            id: btnFullScreen
            Layout.fillHeight: true
            Layout.preferredWidth: height
            enabled: videoPlayer.isVideoLoaded
            // background: CustomItemBackground {}

            onClicked: {
                if (videoControls.state == "fullscreen") {
                    videoWindow.close()
                    videoControls.state = "embedded"
                } else {
                    videoWindow.showFullScreen()
                    videoControls.state = "fullscreen"
                }
            }
        }
    }

    function switchPlayState() {
        if (mediaPlayer.state === MediaPlayer.PlayingState)
            mediaPlayer.pause()
        else
            mediaPlayer.play()
    }
}
