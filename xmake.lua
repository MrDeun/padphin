add_rules("mode.debug", "mode.release")

add_requires("libsdl3")
target("PadPhin")
    set_kind("binary")
    set_languages("c++17")

    -- Qt Quick (QML) application rule. xmake auto-detects your Qt SDK
    -- install; if it can't find it, point it at one explicitly with:
    --   xmake f --qt=/path/to/Qt/6.x.x/gcc_64
    add_rules("qt.quickapp_static")
    add_packages("libsdl3")
    -- Swap for "qt.quickapp_static" if you're linking a statically-built
    -- Qt SDK instead of the usual shared-library install.
    add_frameworks("QtQuick", "QtQml", "QtQuickControls2", "QtGui")

    add_files("src/*.cpp")

    -- FileOperations.h declares Q_OBJECT, so it needs to go through moc.
    -- Adding a Q_OBJECT header via add_files (not add_headerfiles) is what
    -- tells xmake's qt rules to run moc on it.
    add_files("src/FileOperations.h")

    -- Bundles the .qml files listed in qml.qrc as Qt resources.
    add_files("qml/qml.qrc")