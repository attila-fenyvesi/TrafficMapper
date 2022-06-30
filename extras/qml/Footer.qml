import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3

ToolBar {
    height: 34
    background: Rectangle {
        gradient: Gradient {
            GradientStop { position: 1; color: "#FFFFFF" }
            GradientStop { position: 0.5; color: "#FFFFFF" }
            GradientStop { position: 0; color: "#DDDDDD" }
        }
        Rectangle {
            width: parent.width
            height: 1
            anchors.top: parent.top
            color: "#BBBBBB"
        }
    }

    property int cacheLoaded: 0
    property int cacheAllFrames: 0

    RowLayout {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        spacing: 15

        Label {
            text: "Cached frames: <b>" + tracker.cacheSize + "</b> / <b>" + mediaPlayer.frameCount + "</b>"
        }

        ToolSeparator {}

        ToolButton {
            text: "Clear cache"
            enabled: tracker.cacheSize > 0
            onClicked: dlgClearCache.open()
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        ToolButton {
            text: "LOAD VIDEO"
            onClicked: mediaPlayer.loadVideo("file:///E:/Szakdolgozat/Videos/video_05.mp4")
        }

//        ToolButton {
//            text: "LOAD INVALID VIDEO"
//            onClicked: mediaPlayer.loadVideo("file:///E:/Szakdolgozat/Videos/jhfg.mp4")
//        }
//
        ToolButton {
            text: "LOAD CACHE"
            onClicked: tracker.openCacheFile("file:///E:/Szakdolgozat/Videos/video_05.nms.tmc")
            enabled: videoPlayer.isVideoLoaded
        }
//
//        ToolButton {
//            text: "[ Custom code ]"
//            onClicked: tracker.customCode()
//        }
    }

    MessageDialog {
        id: dlgClearCache
        title: "Cache delete warning"
        text: "Are you sure you want to delete all cached detection data?"
        icon: StandardIcon.Warning
        standardButtons: StandardButton.Yes | StandardButton.No
        onAccepted: tracker.clearCache()
    }
}
