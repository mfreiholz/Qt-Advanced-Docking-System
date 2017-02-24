include($$(cetoni_repository)/build/qt/qtprojectsettings/shared_library.pri)
include(src/v2/v2.pri)

TARGET = $$qtLibraryTarget(AdvancedDockingSystem)
TEMPLATE = lib
#VERSION = 1.0.0

CONFIG += adsBuildShared

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 4): DEFINES += ADS_NAMESPACE_ENABLED

adsBuildShared {
	CONFIG += shared
	DEFINES += ADS_EXPORT
}
!adsBuildShared {
	CONFIG += staticlib
}

INCLUDEPATH += $$PWD/include

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

RESOURCES += \
	res/ads.qrc

SOURCES += \
    $$PWD/src/API.cpp \
    $$PWD/src/MainContainerWidget.cpp \
    $$PWD/src/SectionWidget.cpp \
    $$PWD/src/SectionContent.cpp \
    $$PWD/src/SectionTitleWidget.cpp \
    $$PWD/src/SectionContentWidget.cpp \
    $$PWD/src/DropOverlay.cpp \
    $$PWD/src/FloatingWidget.cpp \
    $$PWD/src/Internal.cpp \
    $$PWD/src/Serialization.cpp \
    $$PWD/src/ContainerWidget.cpp

HEADERS += \
    $$PWD/src/API.h \
    $$PWD/src/MainContainerWidget.h \
    $$PWD/src/SectionWidget.h \
    $$PWD/src/SectionContent.h \
    $$PWD/src/SectionTitleWidget.h \
    $$PWD/src/SectionContentWidget.h \
    $$PWD/src/DropOverlay.h \
    $$PWD/src/FloatingWidget.h \
    $$PWD/src/Internal.h \
    $$PWD/src/Serialization.h \
    $$PWD/src/ContainerWidget.h
