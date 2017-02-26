include($$(cetoni_repository)/build/qt/qtprojectsettings/common.pri)

TARGET = AdvancedDockingSystemDemo_v2

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 4): DEFINES += ADS_NAMESPACE_ENABLED

windows {
	# MinGW
	*-g++* {
		QMAKE_CXXFLAGS += -std=c++11
	}
	# MSVC
	*-msvc* {
	}
}

SOURCES += \
	src/main.cpp \
	src/mainwindow.cpp


HEADERS += \
	src/mainwindow.h

FORMS += \
	src/mainwindow.ui


# Dependency: AdvancedDockingSystem (shared)
win32:CONFIG(release, debug|release): LIBS += -l$$qtLinkLibrary(AdvancedDockingSystem)
else:win32:CONFIG(debug, debug|release): LIBS += -l$$qtLinkLibrary(AdvancedDockingSystem)
else:unix: LIBS += -L$$OUT_PWD/../AdvancedDockingSystem/ -lAdvancedDockingSystem

INCLUDEPATH += $$PWD/../AdvancedDockingSystem/src \
    $$PWD/../AdvancedDockingSystem/src/v2

DEPENDPATH += $$PWD/../AdvancedDockingSystem/src \
    $$PWD/../AdvancedDockingSystem/src/v2
