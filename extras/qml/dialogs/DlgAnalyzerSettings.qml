import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.15

import TrafficMapper 1.0

import "../custom-items"

Dialog {
    id: dlgCameraCalibration
    title: "Start video analysis"
    standardButtons: StandardButton.Ok | StandardButton.Cancel
    visible: false

    ColumnLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        spacing: 10

        GroupBox {
            title: qsTr("YOLO Object Detector")
            Layout.fillWidth: true

            ColumnLayout {
                anchors.left: parent.left
                anchors.right: parent.right

                Switch {
                    id: swUseGPU
                    text: qsTr("Use GPU")
                    checked: false
                    checkable: true
                }

                RowLayout {

                    Text {
                        text: qsTr("NMS Threshold:")
                        leftPadding: 5
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    CustomSpinBox {
                        id: sbYoloNMS
                        realDisplay: true
                        value: 60
                    }
                }

                RowLayout {

                    Text {
                        text: qsTr("Confidence Threshold:")
                        leftPadding: 5
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    CustomSpinBox {
                        id: sbYoloConfidence
                        realDisplay: true
                        value: 50
                    }
                }
            }
        }

        GroupBox {
            title: qsTr("Hungarian algorithm")
            Layout.fillWidth: true

            ColumnLayout {
                anchors.left: parent.left
                anchors.right: parent.right

                RowLayout {

                    Text {
                        text: qsTr("IOU Threshold:")
                        leftPadding: 5
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    CustomSpinBox {
                        id: sbIOU
                        realDisplay: true
                        value: 70
                    }
                }
            }
        }

        GroupBox {
            title: qsTr("Optical tracker algorithm")
            Layout.fillWidth: true

            ColumnLayout{
                anchors.fill: parent

                RowLayout {

                    RadioButton {
                        id: rbTrackerKCF
                        text: qsTr("KCF")
                    }
                    RadioButton {
                        id: rbTrackerCSRT
                        text: qsTr("CSRT")
                    }
                    RadioButton {
                        id: rbTrackerMOSSE
                        checked: true
                        text: qsTr("MOSSE")
                    }
                }

                RowLayout {

                    Text {
                        text: qsTr("Tracking distance (frames):")
                        leftPadding: 5
                    }

                    CustomSpinBox {
                        id: sbTrackingDistance
                        realDisplay: false
                        value: 2
                    }
                }
            }
        }
    }

    function setTrackerParams() {
       	tracker.useGPU = swUseGPU.checked
        tracker.IOUTreshold = sbIOU.realValue
        tracker.YoloNMSTreshold = sbYoloNMS.realValue
        tracker.YoloConfTreshold = sbYoloConfidence.realValue
        tracker.trackerAlorithm = rbTrackerKCF.checked ? Tracker.KCF : rbTrackerCSRT.checked ? Tracker.CSRT : Tracker.MOSSE
        tracker.trackingDistance = sbTrackingDistance.value
    }

    onAccepted: {
        setTrackerParams()
        dlgProgressWindow.openForTracker()
        tracker.analizeVideo()
    }
}
