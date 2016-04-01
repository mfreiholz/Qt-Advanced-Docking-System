TARGET = AdvancedDockingSystemUnitTests

QT += core gui testlib
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 4): DEFINES += ADS_NAMESPACE_ENABLED
DEFINES += ADS_IMPORT

INCLUDEPATH += $$PWD/src

INCLUDEPATH += $$PWD/../AdvancedDockingSystem/include
DEPENDPATH += $$PWD/../AdvancedDockingSystem/include

include(AdvancedDockingSystemUnitTests.pri)
include(../AdvancedDockingSystem/AdvancedDockingSystem.pri)