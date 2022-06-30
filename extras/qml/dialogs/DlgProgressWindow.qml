import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.15

Dialog {
    id: progressWindow
    width: 600
    height: contentWrapper.height + 60
    title: ""
    standardButtons: Dialog.NoButton
    visible: false

    Connections {
        target: tracker
        function onProgressUpdated(currentFrameIdx) {
            var allFrameNr = mediaPlayer.frameCount

            progressMessage.text = internal.message + " [" + (currentFrameIdx + 1) + " / " + allFrameNr + "]"
            progressBar.value = currentFrameIdx / allFrameNr * 100
        }
        function onAnalysisEnded() { close() }
    }

//    Connections {
//        target: mediaPlayer
//        function onProgressUpdated(currentFrameIdx, allFrameNr) {
//            progressMessage.text = internal.message + " [" + (currentFrameIdx + 1) + " / " + allFrameNr + "]"
//            progressBar.value = currentFrameIdx / allFrameNr * 100
//        }
//    }

    QtObject {
        id: internal
        property string message: ""
        property int value: 0
        property var handler
    }

    ColumnLayout {
        id: contentWrapper
        spacing: 20
//        anchors.fill: parent
//        anchors.margins: 15
        width: parent.width
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20

        Text {
            id: progressMessage
            Layout.fillWidth: true
            text: internal.message
        }

        ProgressBar {
            id: progressBar
            Layout.fillWidth: true
            height: 20
            from: 0
            to: 100
            value: 0
        }

        Button {
            id: btnCancel
            text: "Cancel"
            Layout.alignment: Qt.AlignRight
            onClicked: {
                progressMessage.text = "Cancelling process..."
                internal.handler.stop()
            }
        }
    }


    function openForTracker() {
        title = "Analysing traffic video"
        internal.message = "Processing frames: "
        internal.handler = tracker
        progressBar.value = 0
        open()
    }

    function openForVideoExport() {
        title = "Exporting video"
        internal.message = "Processing frames: "
        internal.handler = mediaPlayer
        progressBar.value = 0
        open()
    }
}