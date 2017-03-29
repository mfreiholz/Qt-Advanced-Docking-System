ADS_ROOT = $${PWD}/..
ADS_OUT_ROOT = $${OUT_PWD}/..

TARGET = AdvancedDockingSystemDemo
DESTDIR = $${ADS_OUT_ROOT}/lib
QT += core gui widgets

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
	main.cpp \
	mainwindow.cpp


HEADERS += \
	mainwindow.h

FORMS += \
	mainwindow.ui

LIBS += -L$${ADS_OUT_ROOT}/lib

# Dependency: AdvancedDockingSystem (shared)
win32:CONFIG(release, debug|release): LIBS += -lAdvancedDockingSystem
else:win32:CONFIG(debug, debug|release): LIBS += -lAdvancedDockingSystemd
else:unix: LIBS += -lAdvancedDockingSystem

INCLUDEPATH += ../src
DEPENDPATH += ../src
