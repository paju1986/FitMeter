cmake_minimum_required(VERSION 3.16)

project(H9HeartRate VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    Bluetooth
)

qt_standard_project_setup()

# Use the modern QML resource prefix.
qt_policy(SET QTP0001 NEW)

qt_add_executable(H9HeartRateApp
    main.cpp
    HeartRateManager.cpp
    HeartRateManager.h
)

qt_add_qml_module(H9HeartRateApp
    URI H9HeartRate
    VERSION 1.0
    QML_FILES
        Main.qml
)

target_link_libraries(H9HeartRateApp PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::Bluetooth
)

set_target_properties(H9HeartRateApp PROPERTIES
    OUTPUT_NAME "h9-heartrate"
)
