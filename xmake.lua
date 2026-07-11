add_rules("mode.debug", "mode.release")

add_requires("libsdl3")

target("PadPhin")
    set_kind("binary")
    set_languages("c++17")

    -- Qt Quick (QML) application rule. xmake auto-detects your Qt SDK
    -- install; if it can't find it, point it at one explicitly with:
    --   xmake f --qt=/path/to/Qt/6.x.x/gcc_64
    --
    -- Use "qt.quickapp" for the common case of a shared-library Qt SDK
    -- install (the default from the online installer). Swap it for
    -- "qt.quickapp_static" only if you've built/are linking a
    -- statically-built Qt SDK - the two aren't interchangeable, since
    -- static Qt plugins need to be registered differently at link time.
    add_rules("qt.quickapp")
    add_frameworks("QtQuick", "QtQml", "QtQuickControls2", "QtGui")

    add_packages("libsdl3")

    add_files("src/*.cpp")

    -- sdlgamepad.h and sdlgamepadmanager.h declare Q_OBJECT, so they need
    -- to go through moc. Adding Q_OBJECT headers via add_files (not
    -- add_headerfiles) is what tells xmake's qt rules to run moc on them.
    add_files("src/*.h")

    -- Bundles qml/Main.qml as a Qt resource (qrc:/qml/Main.qml), loaded
    -- from main.cpp via engine.load(QUrl("qrc:/qml/Main.qml")).
    add_files("qml/qml.qrc")