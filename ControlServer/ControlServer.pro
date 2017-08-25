#-------------------------------------------------
#
# Project created by QtCreator 2017-08-24T18:05:37
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ControlServer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

HEADERS += \
    ../libmodbus_helper.h \
    ../phidget_helper.h

win32: INCLUDEPATH += ../Phidgets/Phidget22
win32: INCLUDEPATH += ../libmodbus/src/

unix: LIBS += -lphidget22

win32: LIBS += -L"../Phidgets/Phidget22/x86/" -lphidget22

unix: LIBS += -lmodbus

win32: LIBS += -L../libmodbus/src/win32/ -lmodbus

CONFIG += c++11
