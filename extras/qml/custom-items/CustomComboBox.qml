import QtQuick 2.15
import QtQuick.Controls 2.15

ComboBox {
    id: customComboBox
    background: CustomItemBackground {
        implicitWidth: parent.width
        implicitHeight: parent.height
    }
    popup: Popup {
        y: customComboBox.height - 1
        width: customComboBox.width
        implicitHeight: contentItem.implicitHeight + 2
        padding: 1
        contentItem: ListView {
            implicitHeight: contentHeight
            model: customComboBox.popup.visible ? customComboBox.delegateModel : null
            currentIndex: customComboBox.highlightedIndex

            ScrollIndicator.vertical: ScrollIndicator { }
        }
        background: CustomItemBackground {}
    }
}
