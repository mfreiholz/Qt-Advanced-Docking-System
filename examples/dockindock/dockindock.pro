ADS_OUT_ROOT = $${OUT_PWD}/../..

QT += core gui widgets

TARGET = DockInDock
DESTDIR = $${ADS_OUT_ROOT}/lib
TEMPLATE = app
CONFIG += c++14
CONFIG += debug_and_release
adsBuildStatic {
    DEFINES += ADS_STATIC
}

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        dockindock.cpp \
        dockindockmanager.cpp \
        perspectiveactions.cpp \
        perspectives.cpp \
        main.cpp \
        mainframe.cpp

HEADERS += \
        dockindock.h \
        dockindockmanager.h \
        perspectiveactions.h \
        perspectives.h \
        mainframe.h      

LIBS += -L$${ADS_OUT_ROOT}/lib
include(../../ads.pri)
INCLUDEPATH += ../../src
DEPENDPATH += ../../src    

