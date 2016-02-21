#-------------------------------------------------
#
# Project created by QtCreator 2016-02-21T17:31:46
#
#-------------------------------------------------

QT       += widgets

TARGET = LayoutManager
TEMPLATE = lib
CONFIG += staticlib

SOURCES += LayoutManagerWidget.cpp

HEADERS += LayoutManagerWidget.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../AdvancedDockingSystem/release/ -lAdvancedDockingSystem
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../AdvancedDockingSystem/debug/ -lAdvancedDockingSystem
else:unix: LIBS += -L$$OUT_PWD/../AdvancedDockingSystem/ -lAdvancedDockingSystem

INCLUDEPATH += $$PWD/../AdvancedDockingSystem/include
DEPENDPATH += $$PWD/../AdvancedDockingSystem/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../AdvancedDockingSystem/release/libAdvancedDockingSystem.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../AdvancedDockingSystem/debug/libAdvancedDockingSystem.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../AdvancedDockingSystem/release/AdvancedDockingSystem.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../AdvancedDockingSystem/debug/AdvancedDockingSystem.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../AdvancedDockingSystem/libAdvancedDockingSystem.a

FORMS += \
    layoutmanagerwidget.ui
