TARGET = AdvancedDockingSystem

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib

CONFIG += staticlib

INCLUDEPATH += $$PWD/src
INCLUDEPATH += $$PWD/include

windows {
	# MinGW
	*-g++* {
		QMAKE_CXXFLAGS += -std=c++11
		QMAKE_CXXFLAGS += -Wall -Wextra -pedantic
	}
	# MSVC
	*-msvc* {
		#QMAKE_CXXFLAGS += /Za
		#QMAKE_CXXFLAGS += /FS
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
