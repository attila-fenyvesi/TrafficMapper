//import QtQuick 2.13
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.4
import QtMultimedia 5.15

import "./components"
import "./dialogs"
import "./custom-items"

ApplicationWindow {
    id: mainWindow
    title: Qt.application.name + " v" + Qt.application.version
    width: 1015
    height: header.height + 500 + footer.height
    visible: true
    // menuBar: MainMenu {}
    background: Rectangle { color: "#FFFFFF" }
    footer: Footer { id: toolbar }
    header: ToolBar {
        height: 40
        leftPadding: 5
        rightPadding: 5
        topPadding: 2
        bottomPadding: 2
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

        RowLayout {
            // anchors.fill: parent
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            ToolButton {
                text: qsTr("Open video...")
                onClicked: dlgOpenVideo.open()
            }
            // Label {
            //     text: "<font color=\"#336699\" >➜ </font><font color=\"#aaaaaa\"> (</font>"
            //     font.pixelSize: 20
            //     elide: Label.ElideRight
            //     // verticalAlignment: Qt.AlignVCenter
            // }
            ToolButton {
                text: qsTr("Open cache...")
                enabled: videoPlayer.isVideoLoaded
                onClicked: dlgOpenCache.open()
            }
            
            ToolSeparator {}

            // Label {
            //     text: "<font color=\"#aaaaaa\">) </font><font color=\"#336699\" > ➜ </font><font color=\"#aaaaaa\"> (</font>"
            //     font.pixelSize: 20
            //     elide: Label.ElideRight
            //     verticalAlignment: Qt.AlignVCenter
            // }
            ToolButton {
                text: qsTr("Calibrate camera...")
                enabled: videoPlayer.isVideoLoaded
                onClicked: dlgCameraCalibration.initAndOpen()
            }
            // Label {
            //     text: "<font color=\"#aaaaaa\">) </font><font color=\"#336699\" > ➜ </font>"
            //     font.pixelSize: 20
            //     elide: Label.ElideRight
            //     verticalAlignment: Qt.AlignVCenter
            // }
            ToolButton {
                text: qsTr("Analize traffic video...")
                enabled: videoPlayer.isVideoLoaded
                onClicked: dlgAnalyzerSettings.open()
            }
            
            ToolSeparator {}

            ToolButton {
                text: qsTr("Open stat window...")
                enabled: statModel.hasStatData
                onClicked: statWindow.initAndOpen()
            }
        }
    }

    // property Gate currentGateItem: null

    // Connections {
    //     target: tracker
    //     function onProgressUpdated(currentFrameIdx, allFrameNr) {
    //         sliderBg.width = (slider.width - 2) * currentFrameIdx / allFrameNr
    //     }
    // }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                id: playerWrapper
                anchors.fill: parent
                spacing: 10

                VideoPlayer {
                    id: videoPlayer    
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                VideoControl {
                    id: videoControls
                    Layout.preferredHeight: 30
                    Layout.fillWidth: true
                }
            }
        }

        Item {
            Layout.preferredWidth: 200
            Layout.fillHeight: true

            ColumnLayout {
                spacing: 10
                anchors.fill: parent

                PlaybackOptions {
                    Layout.fillWidth: true
                    Layout.preferredHeight: implicitHeight
                }

                GateList {
                    id: gateList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
    }





    DlgOpenVideo {
        id: dlgOpenVideo
    }

    DlgGateProperties {
        id: gatePropertiesDialog
    }

    DlgAnalyzerSettings {
        id: dlgAnalyzerSettings
    }

    DlgProgressWindow {
        id: dlgProgressWindow
    }

    DlgStatWindow {
        id: statWindow
    }

    DlgOpenCache {
        id: dlgOpenCache
    }

    DlgCameraCalibration {
        id: dlgCameraCalibration
    }

    // DlgExportVideo {
    //     id: dlgExportVideo
    // }

    Window{
        id: videoWindow
        flags: Qt.FramelessWindowHint
        color: "black"

        Item {
            id: videoWindowWrapper
            anchors.fill: parent

            Keys.onEscapePressed: {
                videoWindow.close()
                videoControls.state = "embedded"
            }
        }
    }
}
