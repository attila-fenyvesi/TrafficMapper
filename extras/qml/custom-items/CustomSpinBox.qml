import QtQuick 2.15
import QtQuick.Controls 2.15

SpinBox {
    id: customSpinBox
    implicitWidth: 60
    width: 40
    from: 0
    to: 100
    value: 0

    property real realValue: value / 100
    property bool realDisplay: false
    property int wheelStep: 5

    validator: DoubleValidator {
        bottom: Math.min(customSpinBox.from, customSpinBox.to)
        top:  Math.max(customSpinBox.from, customSpinBox.to)
    }

    textFromValue: function(value, locale) {
        return realDisplay ? Number(value / 100).toLocaleString(locale, 'f', 2) : value
    }

    valueFromText: function(text, locale) {
        return realDisplay ? Number.fromLocaleString(locale, text) * 100 : value
    }

    MouseArea {
        anchors.fill: parent
        anchors.rightMargin: 15

        onWheel: {
            if (wheel.angleDelta.y > 0)
                customSpinBox.value += wheelStep
            else
                customSpinBox.value -= wheelStep
        }
    }
}
