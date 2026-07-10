# Qt Quick (QML) File Manager — Starter Project

A file manager UI built with QML: sidebar of shortcuts, icon-grid folder
view, address bar, back/forward/up navigation, and new-folder/rename/delete
actions. Uses `Qt.labs.folderlistmodel` for browsing (it's read-only, so a
small C++ helper class handles the actual filesystem mutations).

## Layout

```
qt-quick-file-manager-xmake/
├── xmake.lua
├── qml.qrc                    # bundles the .qml files as Qt resources
├── README.md
├── src/
│   ├── main.cpp                # also registers FileOperations for QML
│   ├── FileOperations.h        # mkdir/rename/remove, exposed to QML
│   └── FileOperations.cpp
└── qml/
    ├── Main.qml                # window, toolbar, dialogs, navigation state
    ├── Sidebar.qml              # Home/Desktop/Documents shortcuts
    └── FileGrid.qml             # icon-grid view of the current folder
```

## Requirements

- Qt 6 (Quick + Qml + QuickControls2 modules) — Qt 5 works too, xmake's
  `qt.quickapp` rule handles either.
- xmake >= 2.7 (for reliable Qt rule support), C++17 compiler

## Build

```bash
xmake
xmake run FileManagerQml
```

If xmake can't auto-detect your Qt SDK:

```bash
xmake f --qt=/path/to/Qt/6.x.x/gcc_64
xmake
```

(On Windows that's typically something like `C:\Qt\6.x.x\msvc2019_64`; on
macOS, `~/Qt/6.x.x/macos`.)

## A note on how QML type registration differs from the CMake version

The CMake version used Qt6's `QML_ELEMENT` macro + `qt_add_qml_module()` to
auto-register `FileOperations` for `import FileManager 1.0`. That mechanism
is CMake-specific codegen, so it doesn't carry over to xmake. Here,
`FileOperations` is registered manually instead:

```cpp
qmlRegisterType<FileOperations>("FileManager", 1, 0, "FileOperations");
```

called once in `main.cpp` before the QML engine loads anything. This is the
traditional way to register QML types and works identically under any build
system (xmake, qmake, plain Makefiles, etc.) — it's arguably more portable
than the CMake-only macro, just slightly more verbose.

Similarly, the QML files are bundled via a plain `.qrc` file (`qml.qrc`)
rather than `qt_add_qml_module`'s automatic resource generation — this is
the same mechanism qmake-based Qt projects have used for years, and xmake's
`qt.quickapp` rule expects it.

## How it fits together

- **`FolderListModel`** (`Qt.labs.folderlistmodel`) does the actual directory
  listing in `FileGrid.qml` — set its `folder` property to a `file://` URL and
  it populates itself, with roles like `fileName`, `filePath`, `fileIsDir`.
- It's **read-only**, so creating folders, renaming, and deleting go through
  `FileOperations`, a plain `QObject` with `Q_INVOKABLE` methods, registered
  via `QML_ELEMENT` and instantiated directly in `Main.qml` as
  `FileOperations { id: fileOps }`.
- After a mutation (new folder, rename, delete), the code reassigns
  `fileGrid.folder` (clear then reset) to force `FolderListModel` to
  re-scan — it doesn't always pick up filesystem changes made by other
  processes in real time the way `QFileSystemModel` does with its built-in
  watcher.
- Navigation history (back/forward) is a plain QML array (`history` +
  `historyIndex`) maintained in `Main.qml` — there's no built-in Qt type for
  this, it's just bookkeeping.

## Where to go from here

Roughly in order of how likely you are to want them:

1. **Open files on double-click** — in `Main.qml`'s `onFolderActivated`
   handler, call `Qt.openUrlExternally(fileOps.pathToUrl(path))` for
   non-directory items instead of leaving it as a placeholder.
2. **Real icons** — the current delegate uses emoji as filler. For proper
   file-type icons, either bundle an icon theme as an image provider, or add
   a `Q_INVOKABLE QString iconForFile(const QString &path)` to
   `FileOperations` that maps extensions to bundled `.svg`/`.png` assets.
3. **Thumbnails for images** — swap the icon `Rectangle` for an `Image`
   element with `source: fileIsDir ? "" : fileOps.pathToUrl(filePath)` when
   the file is a recognized image type; Qt will decode and cache it via its
   image cache automatically.
4. **List view / details view toggle** — add a second delegate using
   `ListView` with columns (name, size, modified date) and a toolbar toggle
   to switch `FileGrid` between grid and list layouts.
5. **Drag-and-drop** — QML's `Drag` and `DropArea` types can be added to the
   `FileGrid` delegate and the sidebar to support drag-to-move; you'd still
   call into `FileOperations` (e.g. add a `move()` method) to perform the
   actual file move on drop.
6. **Background copy/move with progress** — for large operations, do the
   actual I/O on a `QThread` (or `QtConcurrent`) inside a C++ class that
   emits progress signals QML can bind a `ProgressBar` to, rather than
   blocking on synchronous `QFile`/`QDir` calls as `FileOperations` does now.
7. **Multi-selection** — `FileGrid` currently tracks a single `selectedPath`;
   for multi-select you'd track a list of selected paths (e.g. a `QML`
   `ListModel` or JS array) and adjust the delegate's highlight + the
   toolbar actions to operate over the whole selection.
8. **Context menu** — QML's `Menu` type anchored to a right-click
   `MouseArea` on each grid delegate, wired to the same rename/delete calls
   used by the toolbar.

## Notes / caveats

- Deletion is immediate and permanent (`QDir::removeRecursively()` /
  `QFile::remove()`) — it does **not** go through the OS trash/recycle bin.
  There's no built-in Qt API for trash semantics; you'd need
  platform-specific code (e.g. `SHFileOperation` on Windows, `NSWorkspace`
  on macOS, `gio trash` / freedesktop trash spec on Linux) if you want that.
- `FileOperations::pathToUrl`/`urlToPath` wrap `QUrl::fromLocalFile` /
  `QUrl::toLocalFile`, which is the correct way to convert between local
  paths and the `file://` URLs `FolderListModel` expects — avoid
  string-concatenating `"file://" + path` yourself, it breaks on Windows
  drive letters and paths with special characters.