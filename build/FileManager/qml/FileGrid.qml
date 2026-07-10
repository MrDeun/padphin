import QtQuick
import QtQuick.Controls
import Qt.labs.folderlistmodel

// Icon-grid view of one folder's contents, backed by FolderListModel.
// Emits folderActivated(path) on double-click into a directory, and
// selectionChanged(path, isDir) as the current selection changes.
Rectangle {
    id: root
    color: "white"

    property alias folder: folderModel.folder
    property string selectedPath: ""
    property bool selectedIsDir: false

    signal folderActivated(string path)
    signal selectionChanged(string path, bool isDir)

    FolderListModel {
        id: folderModel
        showDotAndDotDot: false
        showDirsFirst: true
        sortField: FolderListModel.Name
    }

    GridView {
        id: grid
        anchors.fill: parent
        anchors.margins: 8
        cellWidth: 96
        cellHeight: 96
        clip: true
        model: folderModel

        delegate: Item {
            width: grid.cellWidth
            height: grid.cellHeight

            required property string fileName
            required property string filePath
            required property bool fileIsDir
            required property int index

            Column {
                anchors.fill: parent
                anchors.margins: 6
                spacing: 4

                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 48
                    height: 40
                    radius: 4
                    color: fileIsDir ? "#5b9bd5" : "#c0c0c0"

                    Text {
                        anchors.centerIn: parent
                        text: fileIsDir ? "\uD83D\uDCC1" : "\uD83D\uDCC4" // folder / file emoji
                        font.pixelSize: 20
                    }
                }

                Text {
                    width: parent.width
                    text: fileName
                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideMiddle
                    font.pixelSize: 11
                    color: "#222"
                }
            }

            Rectangle {
                anchors.fill: parent
                color: "#3399ff"
                opacity: root.selectedPath === filePath ? 0.15 : 0
                radius: 4
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    root.selectedPath = filePath;
                    root.selectedIsDir = fileIsDir;
                    root.selectionChanged(filePath, fileIsDir);
                }
                onDoubleClicked: {
                    if (fileIsDir)
                        root.folderActivated(filePath);
                    else
                        root.folderActivated(filePath); // Main.qml decides what "open" means for files
                }
            }
        }

        // Empty-folder hint
        Text {
            anchors.centerIn: parent
            visible: folderModel.count === 0
            text: "This folder is empty"
            color: "#999"
        }
    }
}
