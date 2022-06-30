import QtQuick 2.15
import QtQuick.Controls 2.15

TabButton {

    id: customTabButton
    checkable: true

    property Item tabBar: unidentified
    property int tabBarPosition: unidentified

    QtObject {
        id: internal
        property bool selected: customTabButton.parent.currentIndex == tabBarPosition
    }

    background: CustomItemBackground { }
    contentItem: Text {
        text: parent.text
        font.bold: true
        font.pixelSize: 18
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.fill: parent
        color: tabBar.currentIndex == tabBarPosition ? "white" : "black"
    }
    states: [
        State {
            name: "hovered"; when: customTabButton.hovered && !customTabButton.checked && customTabButton.enabled
            PropertyChanges { target: customTabButton.background; color: "#DDDDDD" }
        },
        State {
            name: "selected"; when: customTabButton.checked && customTabButton.enabled
            PropertyChanges { target: customTabButton.background; color: "#222222"; border.color: "#222222" }
            PropertyChanges { target: customTabButton.contentItem; color: "#FFFFFF" }
        },
        State {
            name: "disabled"; when: !customTabButton.enabled
            PropertyChanges { target: customTabButton.background; border.color: "#DDDDDD" }
            PropertyChanges { target: customTabButton.contentItem; color: "#AAAAAA" }
        }
    ]

}
