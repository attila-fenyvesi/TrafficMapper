import QtQuick 2.0
import QtQuick.Dialogs 1.2

import TrafficMapper 1.0

FileDialog {
    id: dlgOpenVideo
    title: "Open traffic video file"
    nameFilters: ["Video files (*.mp4)"]
    selectedNameFilter: "Video files (*.mp4)"
    folder: "file:///E:/Szakdolgozat/Videos"
//    folder: shortcuts.movies

    onAccepted: {
        mediaPlayer.loadVideo(dlgOpenVideo.fileUrl)
    }
}
