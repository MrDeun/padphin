add_rules("mode.debug", "mode.release")

add_requires("libsdl3")

target("PadPhin")
    set_kind("binary")
    set_languages("c++17")

    add_rules("qt.widgetapp")
    add_frameworks("QtWidgets", "QtGui", "QtSvg")

    add_packages("libsdl3")

    add_files("src/*.cpp")

    add_files("src/*.h")