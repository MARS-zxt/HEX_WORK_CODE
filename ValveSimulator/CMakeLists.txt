cmake_minimum_required(VERSION 3.16)
project(ValveSimulator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets Network QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Widgets Network)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
endif()

set(SOURCES
    main.cpp
    valve_simulator.cpp
    custom_titlebar.cpp
    valve_display.cpp
    valve_params_dialog.cpp
    waveform_chart.cpp
    test_panel.cpp
    main_window.cpp
)

set(HEADERS
    valve_types.h
    valve_simulator.h
    custom_titlebar.h
    valve_display.h
    valve_params_dialog.h
    waveform_chart.h
    test_panel.h
    main_window.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} resources.qrc)

if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Network)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Network)
endif()

# Windows: hide console window for release builds
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
