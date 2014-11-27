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
    qtBlokLib/modulesactivity.cpp \
    qtBlokLib/iodrv.cpp \
    qtBlokLib/endecs.cpp \
    qtBlokLib/cookies.cpp \
    electroincmap.cpp \
    customordinatehandler.cpp \
    nmeareceiver.cpp \
    qtBlokLib/parsers/canblokmessage.cpp \
    qtBlokLib/parsers/mm_alt_long.cpp \
    qtBlokLib/parser.cpp \
    qtBlokLib/parsers/ipd_date.cpp \
    qtBlokLib/parsers/mm_data.cpp \
    iodrv/gpsemitter.cpp \
    qtBlokLib/parsers/aux_resource.cpp \
    qtBlokLib/parsers/console_key.cpp \
    qtBlokLib/parsers/display_state.cpp \
    qtBlokLib/parsers/ipd_state.cpp \
    qtBlokLib/parsers/mco_limits.cpp \
    qtBlokLib/parsers/mco_mode.cpp \
    qtBlokLib/parsers/mco_state.cpp \
    qtBlokLib/parsers/mm_coord.cpp \
    qtBlokLib/parsers/mp_state.cpp \
    qtBlokLib/parsers/mvd_dd.cpp \
    qtBlokLib/parsers/sys_key.cpp \
    qtBlokLib/parsers/tskbm_state.cpp \
    qtBlokLib/parsers/uktol_dd1.cpp \
    qtBlokLib/parsers/vds_state.cpp \
    qtBlokLib/parsers/sys_diagnostics.cpp

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
    nmeareceiver.h \
    qtBlokLib/parsers/canblokmessage.h \
    qtBlokLib/parsers/mm_alt_long.h \
    qtBlokLib/parser.h \
    qtBlokLib/parsers/ipd_date.h \
    qtBlokLib/parsers/mm_data.h \
    iodrv/gpsemitter.h \
    qtBlokLib/parsers/autolockmode.h \
    qtBlokLib/parsers/aux_resource.h \
    qtBlokLib/parsers/console_key.h \
    qtBlokLib/parsers/display_state.h \
    qtBlokLib/parsers/drivemode.h \
    qtBlokLib/parsers/ipd_state.h \
    qtBlokLib/parsers/mco_limits.h \
    qtBlokLib/parsers/mco_mode.h \
    qtBlokLib/parsers/mco_state.h \
    qtBlokLib/parsers/mm_coord.h \
    qtBlokLib/parsers/mp_state.h \
    qtBlokLib/parsers/mvd_dd.h \
    qtBlokLib/parsers/pressure.h \
    qtBlokLib/parsers/sys_key.h \
    qtBlokLib/parsers/trafficlight.h \
    qtBlokLib/parsers/tskbm_state.h \
    qtBlokLib/parsers/uktol_dd1.h \
    qtBlokLib/parsers/vds_state.h \
    qtBlokLib/parsers/sys_diagnostics.h

CONFIG += console
CONFIG += serialport

QMAKE_CXXFLAGS += -std=c++0x
DEFINES+=CPP11

# Чтобы работало, нужно компилировать с:
# DEFINES+=LIB_SOCKET_CAN DEFINES+=WITH_SERIALPORT
