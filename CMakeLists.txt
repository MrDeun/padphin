cmake_minimum_required(VERSION 3.16)
project(FileManagerQml LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Change to Qt5 if that's what you have (see README for the Qt5 diff).
find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2)

qt_standard_project_setup()

qt_add_executable(FileManagerQml
    src/main.cpp
    src/FileOperations.h
    src/FileOperations.cpp
)

qt_add_qml_module(FileManagerQml
    URI FileManager
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/Sidebar.qml
        qml/FileGrid.qml
    SOURCES
        src/FileOperations.h
        src/FileOperations.cpp
)

target_link_libraries(FileManagerQml PRIVATE
    Qt6::Quick
    Qt6::QuickControls2
)

if(WIN32 OR APPLE)
    qt_generate_deploy_app_script(
        TARGET FileManagerQml
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script})
endif()
