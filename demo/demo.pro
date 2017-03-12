include($$(cetoni_repository)/build/qt/qtprojectsettings/common.pri)

TARGET = AdvancedDockingSystemDemo

QT += core gui widgets
DEFINES += ADS_NAMESPACE_ENABLED

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


# Dependency: AdvancedDockingSystem (shared)
win32:CONFIG(release, debug|release): LIBS += -l$$qtLinkLibrary(AdvancedDockingSystem)
else:win32:CONFIG(debug, debug|release): LIBS += -l$$qtLinkLibrary(AdvancedDockingSystem)
else:unix: LIBS += -L$$OUT_PWD/../AdvancedDockingSystem/ -lAdvancedDockingSystem

INCLUDEPATH += ../src

DEPENDPATH += ../src
