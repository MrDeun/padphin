import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// A simple list of shortcut folders (Home, Desktop, Documents, root).
// Emits folderRequested(path) when the user picks one.
Rectangle {
    id: root
    color: "#f2f2f2"

    signal folderRequested(string path)

    property var places: []

    ListView {
        anchors.fill: parent
        anchors.margins: 6
        spacing: 2
        model: root.places

        delegate: ItemDelegate {
            width: ListView.view.width
            text: modelData.label
            icon.name: "folder"

            onClicked: root.folderRequested(modelData.path)
        }
    }
}
