import QtQuick
import QtQuick.Controls
import SdlQtGamepad 1.0

ApplicationWindow {
    id: window
    width: 480
    height: 360
    visible: true
    title: "SDL Gamepad + QML"

    property var pad: GamepadManager.primaryGamepad

    Column {
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 12
        spacing: 4

        Text {
            text: pad ? ("Connected: " + pad.name) : "No gamepad connected"
            font.pixelSize: 16
        }
        Text {
            visible: pad !== null
            text: pad ? ("Left trigger: " + pad.leftTrigger.toFixed(2)
                          + "   Right trigger: " + pad.rightTrigger.toFixed(2)) : ""
        }
    }

    // Left stick position, driven live by the leftStickX/Y properties.
    Rectangle {
        id: stickArea
        width: 160; height: 160
        radius: width / 2
        anchors.centerIn: parent
        border.color: "gray"
        color: "transparent"

        Rectangle {
            width: 24; height: 24
            radius: width / 2
            color: pad && pad.isButtonPressed(SdlGamepad.ButtonLeftStick) ? "orange" : "steelblue"

            // Bindings recalculate automatically because leftStickX/Y have NOTIFY signals.
            x: stickArea.width / 2 - width / 2 + (pad ? pad.leftStickX * 60 : 0)
            y: stickArea.height / 2 - height / 2 + (pad ? pad.leftStickY * 60 : 0)

            Behavior on x { NumberAnimation { duration: 30 } }
            Behavior on y { NumberAnimation { duration: 30 } }
        }
    }

    ListView {
        id: log
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top: stickArea.bottom
        anchors.margins: 8
        model: ListModel { id: logModel }
        delegate: Text { text: message }
        verticalLayoutDirection: ListView.BottomToTop
    }

    // React to button events. Connections' target updates automatically if
    // pad changes (e.g. controller reconnects with a different instance).
    Connections {
        target: pad
        function onButtonPressed(button) {
            logModel.insert(0, { message: "Pressed: " + button })
            if (button === SdlGamepad.ButtonSouth) {
                // Wire this to whatever your app should do, e.g.:
                // stackView.currentItem.confirm()
            }
        }
        function onButtonReleased(button) {
            logModel.insert(0, { message: "Released: " + button })
        }
    }

    Connections {
        target: GamepadManager
        function onGamepadConnected(gp) {
            logModel.insert(0, { message: "Connected: " + gp.name })
        }
        function onGamepadDisconnected(gp) {
            logModel.insert(0, { message: "Disconnected: " + gp.name })
        }
    }
}
