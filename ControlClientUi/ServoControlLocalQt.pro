#-------------------------------------------------
#
# Project created by QtCreator 2017-08-10T19:30:51
#
#-------------------------------------------------

QT       += core gui

QMAKE_CXXFLAGS += -Wunused-parameter

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ControlClientUi
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    ../libmodbus_helper.h

INCLUDEPATH += ..\Phidgets\Phidget22

FORMS    += mainwindow.ui

unix: LIBS += -lmodbus

win32: LIBS += -L../libmodbus/src/win32/ -lmodbus

INCLUDEPATH += ../libmodbus/src/
