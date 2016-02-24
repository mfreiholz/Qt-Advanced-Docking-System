TARGET = AdvancedDockingSystem
#VERSION = 0.1.0
CONFIG += adsBuildShared

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib

adsBuildShared {
    CONFIG += shared
    DEFINES += ADS_EXPORT
}
!adsBuildShared {
    CONFIG += staticlib
}

INCLUDEPATH += $$PWD/src
INCLUDEPATH += $$PWD/include

greaterThan(QT_MAJOR_VERSION, 4): DEFINES += ADS_NAMESPACE_ENABLED

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

SOURCES += \
	src/API.cpp \
	src/ContainerWidget.cpp \
	src/SectionWidget.cpp \
	src/SectionContent.cpp \
	src/SectionTitleWidget.cpp \
	src/SectionContentWidget.cpp \
	src/DropOverlay.cpp \
	src/FloatingWidget.cpp \
	src/Internal.cpp

HEADERS += \
	include/ads/API.h \
	include/ads/ContainerWidget.h \
	include/ads/SectionWidget.h \
	include/ads/SectionContent.h \
	include/ads/SectionTitleWidget.h \
	include/ads/SectionContentWidget.h \
	include/ads/DropOverlay.h \
	include/ads/FloatingWidget.h \
	include/ads/Internal.h

RESOURCES += \
	res/ads.qrc
