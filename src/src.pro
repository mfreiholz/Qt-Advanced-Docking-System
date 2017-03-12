include($$(cetoni_repository)/build/qt/qtprojectsettings/shared_library.pri)
include(src/v2/v2.pri)

TARGET = $$qtLibraryTarget(AdvancedDockingSystem)
TEMPLATE = lib
#VERSION = 1.0.0

CONFIG += adsBuildShared

QT += core gui widgets
DEFINES += ADS_NAMESPACE_ENABLED

adsBuildShared {
	CONFIG += shared
	DEFINES += ADS_EXPORT
}
!adsBuildShared {
	CONFIG += staticlib
}

windows {
	# MinGW
	*-g++* {
		QMAKE_CXXFLAGS += -std=c++11
		QMAKE_CXXFLAGS += -Wall -Wextra -pedantic
	}
	# MSVC
	*-msvc* {
	}
}

RESOURCES += ads.qrc

HEADERS += \
    ads_globals.h \
    DockAreaWidget.h \
    DockContainerWidget.h \
    DockManager.h \
    DockWidget.h \
    DockWidgetTitleBar.h \
    FloatingDockContainer.h \
    DockOverlay.h
    
    
    
SOURCES += \
    ads_globals.cpp \
    DockAreaWidget.cpp \
    DockContainerWidget.cpp \
    DockManager.cpp \
    DockWidget.cpp \
    DockWidgetTitleBar.cpp \
    FloatingDockContainer.cpp \
    DockOverlay.cpp
