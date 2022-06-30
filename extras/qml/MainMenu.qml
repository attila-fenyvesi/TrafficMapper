import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3


MenuBar {
    height: 25
    background: Rectangle {
        gradient: Gradient {
            GradientStop { position: 0; color: "#FFFFFF" }
            GradientStop { position: 1; color: "#DDDDDD" }
        }
        Rectangle {
            width: parent.width
            height: 1
            anchors.bottom: parent.bottom
            color: "#BBBBBB"
        }
    }

    Menu {
        title: "File"

        MenuItem {
            text: "Open video..."
            onTriggered: dlgOpenVideo.open()
        }

        MenuItem {
            text: "Open cache..."
            enabled: videoPlayer.isVideoLoaded
            onTriggered: dlgOpenCache.open()
        }

//        MenuSeparator {}
//
//        MenuItem {
//            text: "Export video..."
//            enabled: videoPlayer.isVideoLoaded
//            onTriggered: dlgExportVideo.open()
//        }
    }

    Menu {
        title: "Traffic analysis"

        MenuItem {
            text: "Calibrate camera..."
            enabled: videoPlayer.isVideoLoaded
            onTriggered: dlgCameraCalibration.initAndOpen()
        }

        MenuSeparator {}

        MenuItem {
            text: "Analize traffic video"
            enabled: videoPlayer.isVideoLoaded
            onTriggered: dlgAnalyzerSettings.open()
        }
    }

    Menu {
        title: "Statistics"

        MenuItem {
            text: "Open stat window"
            // enabled: statModel.hasStatData
            onTriggered: statWindow.initAndOpen()
        }
    }
}
