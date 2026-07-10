import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FileManager 1.0

ApplicationWindow {
    id: window
    width: 1000
    height: 650
    visible: true
    title: "File Manager — " + currentPath

    property string currentPath: fileOps.homePath()

    FileOperations {
        id: fileOps
    }

    // --- Navigation history for Back/Forward ---
    property var history: [currentPath]
    property int historyIndex: 0

    function navigateTo(path, recordHistory) {
        if (path === "" || path === undefined)
            return;
        currentPath = path;
        addressField.text = path;
        fileGrid.folder = fileOps.pathToUrl(path);
        grid_selection_clear();

        if (recordHistory !== false) {
            // Trim any forward history, then append.
            history = history.slice(0, historyIndex + 1);
            history.push(path);
            historyIndex = history.length - 1;
        }
    }

    function grid_selection_clear() {
        fileGrid.selectedPath = "";
        fileGrid.selectedIsDir = false;
    }

    function goBack() {
        if (historyIndex > 0) {
            historyIndex -= 1;
            navigateTo(history[historyIndex], false);
        }
    }

    function goForward() {
        if (historyIndex < history.length - 1) {
            historyIndex += 1;
            navigateTo(history[historyIndex], false);
        }
    }

    function goUp() {
        const parent = fileOps.parentPath(currentPath);
        if (parent !== "")
            navigateTo(parent);
    }

    Component.onCompleted: {
        fileGrid.folder = fileOps.pathToUrl(currentPath);
        addressField.text = currentPath;
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            anchors.margins: 6
            spacing: 6

            ToolButton {
                text: "\u2190" // back
                enabled: window.historyIndex > 0
                onClicked: window.goBack()
            }
            ToolButton {
                text: "\u2192" // forward
                enabled: window.historyIndex < window.history.length - 1
                onClicked: window.goForward()
            }
            ToolButton {
                text: "\u2191" // up
                onClicked: window.goUp()
            }

            ToolSeparator {}

            TextField {
                id: addressField
                Layout.fillWidth: true
                selectByMouse: true
                onAccepted: window.navigateTo(text)
            }

            ToolSeparator {}

            ToolButton {
                text: "New Folder"
                onClicked: newFolderDialog.open()
            }
            ToolButton {
                text: "Rename"
                enabled: fileGrid.selectedPath !== ""
                onClicked: {
                    renameField.text = fileGrid.selectedPath.split("/").pop();
                    renameDialog.open();
                }
            }
            ToolButton {
                text: "Delete"
                enabled: fileGrid.selectedPath !== ""
                onClicked: deleteDialog.open()
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Sidebar {
            Layout.preferredWidth: 180
            Layout.fillHeight: true
            places: [
                { label: "Home", path: fileOps.homePath() },
                { label: "Desktop", path: fileOps.desktopPath() },
                { label: "Documents", path: fileOps.documentsPath() }
            ]
            onFolderRequested: (path) => window.navigateTo(path)
        }

        FileGrid {
            id: fileGrid
            Layout.fillWidth: true
            Layout.fillHeight: true

            onFolderActivated: (path) => {
                if (fileGrid.selectedIsDir)
                    window.navigateTo(path);
                // else: hook up opening files in their default app here, e.g.
                // Qt.openUrlExternally(fileOps.pathToUrl(path))
            }
            onSelectionChanged: (path, isDir) => {
                statusLabel.text = path;
            }
        }
    }

    footer: ToolBar {
        Label {
            id: statusLabel
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 8
            text: window.currentPath
            elide: Text.ElideMiddle
        }
    }

    // --- New Folder dialog ---
    Dialog {
        id: newFolderDialog
        title: "New Folder"
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        anchors.centerIn: parent

        TextField {
            id: newFolderField
            width: 260
            placeholderText: "Folder name"
            text: "New Folder"
            selectByMouse: true
        }

        onAccepted: {
            if (fileOps.mkdir(window.currentPath, newFolderField.text)) {
                // Force FolderListModel to notice the new entry.
                fileGrid.folder = "";
                fileGrid.folder = fileOps.pathToUrl(window.currentPath);
            }
        }
        onOpened: {
            newFolderField.selectAll();
            newFolderField.forceActiveFocus();
        }
    }

    // --- Rename dialog ---
    Dialog {
        id: renameDialog
        title: "Rename"
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        anchors.centerIn: parent

        TextField {
            id: renameField
            width: 260
            selectByMouse: true
        }

        onAccepted: {
            if (fileOps.rename(fileGrid.selectedPath, renameField.text)) {
                fileGrid.folder = "";
                fileGrid.folder = fileOps.pathToUrl(window.currentPath);
                window.grid_selection_clear();
            }
        }
        onOpened: {
            renameField.selectAll();
            renameField.forceActiveFocus();
        }
    }

    // --- Delete confirmation ---
    Dialog {
        id: deleteDialog
        title: "Delete"
        modal: true
        standardButtons: Dialog.Yes | Dialog.No
        anchors.centerIn: parent

        Label {
            width: 280
            wrapMode: Text.WordWrap
            text: "Delete \"" + fileGrid.selectedPath.split("/").pop()
                  + "\"? This cannot be undone."
        }

        onAccepted: {
            fileOps.remove(fileGrid.selectedPath, fileGrid.selectedIsDir);
            fileGrid.folder = "";
            fileGrid.folder = fileOps.pathToUrl(window.currentPath);
            window.grid_selection_clear();
        }
    }
}
