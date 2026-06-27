cmake_minimum_required(VERSION 3.16)
project(ValveSimulator VERSION 1.0.0 LANGUAGES CXX RC)

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

# ============================================================================
# Sources — organized by module
# ============================================================================
set(CORE_SOURCES
    src/core/valve_simulator.cpp
)

set(UI_SOURCES
    src/ui/main.cpp
    src/ui/main_window.cpp
    src/ui/custom_titlebar.cpp
    src/ui/valve_display.cpp
    src/ui/valve_params_dialog.cpp
    src/ui/version_dialog.cpp
    src/ui/waveform_chart.cpp
    src/ui/test_panel.cpp
)

set(NETWORK_SOURCES
    src/network/valve_api.cpp
)

set(RESOURCE_SOURCES
    resources/app_icon.rc
)

set(SOURCES
    ${CORE_SOURCES}
    ${UI_SOURCES}
    ${NETWORK_SOURCES}
    ${RESOURCE_SOURCES}
)

set(HEADERS
    src/core/valve_types.h
    src/core/version.h
    src/core/valve_simulator.h
    src/ui/main_window.h
    src/ui/custom_titlebar.h
    src/ui/valve_display.h
    src/ui/valve_params_dialog.h
    src/ui/version_dialog.h
    src/ui/waveform_chart.h
    src/ui/test_panel.h
    src/network/valve_api.h
)

# ============================================================================
# Target
# ============================================================================
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} resources/resources.qrc)

# Include paths — preserve flat #include style (e.g. #include "valve_simulator.h")
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/network
)

if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Network)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Network)
endif()

# Windows: hide console window and set app icon
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
        RC_ICONS "resources/app_icon.ico"
    )
endif()
