#-------------------------------------------------
#
# Project created by QtCreator 2013-08-29T15:32:42
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = QtMap
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    qtCanLib/canframe.cpp \
    qtCanLib/socketcan/sktcan.cpp \
    qtCanLib/socketcan.cpp \
    qtDoodahLib/queues.cpp \
    qtCanLib/socketcan/cansendqueue.cpp \
    main.cpp \
    emap/routesection.cpp \
    emap/railobject.cpp \
    emap/rail.cpp \
    emap/kilometerpost.cpp \
    emap/emaptarget.cpp \
    iodrv/emapcanemitter.cpp \
    qtBlokLib/sysdiagnostics.cpp \
    qtBlokLib/modulesactivity.cpp \
    qtBlokLib/iodrv.cpp \
    qtBlokLib/endecs.cpp \
    qtBlokLib/cookies.cpp \
    electroincmap.cpp \
    customordinatehandler.cpp \
    MapcodeInformer.cpp \
    VersionInformer.cpp

HEADERS += \
    emap/routesection.h \
    emap/railobject.h \
    emap/rail.h \
    emap/kilometerpost.h \
    emap/emaptarget.h \
    emap/almanac.h \
    iodrv/emapcanemitter.h \
    cDoodahLib/packing.h \
    cDoodahLib/masqarade.h \
    cDoodahLib/lowlevel.h \
    qtBlokLib/sysdiagnostics.h \
    qtBlokLib/modulesactivity.h \
    qtBlokLib/iodrvmain.h \
    qtBlokLib/iodrv.h \
    qtBlokLib/endecs.h \
    qtBlokLib/cookies.h \
    qtCanLib/socketcan/sktcan.h \
    qtCanLib/socketcan/cansendqueue.h \
    qtCanLib/socketcan.h \
    qtCanLib/dummycan.h \
    qtCanLib/canframe.h \
    qtCanLib/can.h \
    qtDoodahLib/queues.h \
    electroincmap.h \
    customordinatehandler.h \
    MapcodeInformer.h \
    VersionInformer.h

CONFIG += console
CONFIG += serialport

QMAKE_CXXFLAGS += -std=c++0x
DEFINES+=CPP11

# Чтобы работало, нужно компилировать с:
# DEFINES+=LIB_SOCKET_CAN DEFINES+=WITH_SERIALPORT
