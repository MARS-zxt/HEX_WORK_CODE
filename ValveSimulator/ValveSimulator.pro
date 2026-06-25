QT       += core gui widgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = ValveSimulator
TEMPLATE = app

SOURCES += \
    main.cpp \
    valve_simulator.cpp \
    custom_titlebar.cpp \
    valve_display.cpp \
    valve_params_dialog.cpp \
    waveform_chart.cpp \
    test_panel.cpp \
    main_window.cpp

# NOTE: valve_api.cpp is a standalone C library for external test programs.
# Compile it separately with your C test program, e.g.:
#   gcc test_client_example.c valve_api.cpp -o test_client.exe -lws2_32

HEADERS += \
    valve_types.h \
    valve_simulator.h \
    custom_titlebar.h \
    valve_display.h \
    valve_params_dialog.h \
    waveform_chart.h \
    test_panel.h \
    main_window.h \
    valve_api.h

RESOURCES += resources.qrc

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Windows specific
win32 {
    RC_ICONS = app_icon.ico
    LIBS += -lws2_32
}
