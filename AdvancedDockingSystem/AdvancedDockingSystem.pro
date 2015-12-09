TARGET = AdvancedDockingSystem

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib

CONFIG += staticlib

INCLUDEPATH += $$PWD/src

SOURCES += \
	src/ads/section_content.cpp \
	src/ads/ads.cpp \
	src/ads/section_widget.cpp \
	src/ads/section_title_widget.cpp \
	src/ads/container_widget.cpp \
	src/ads/drop_overlay.cpp \
	src/ads/floating_widget.cpp \
	src/ads/section_content_widget.cpp

HEADERS += \
	src/ads/ads.h \
	src/ads/container_widget.h \
	src/ads/section_widget.h \
	src/ads/section_content.h \
	src/ads/section_title_widget.h \
	src/ads/section_content_widget.h \
	src/ads/drop_overlay.h \
	src/ads/floating_widget.h

RESOURCES += \
	res/ads.qrc
