#include <QCoreApplication>

#include "cDoodahLib/masqarade.h"
#ifdef _WIN32
    #include <windows.h>
    HANDLE winConsoleandler;
#endif

#include "iodrv/emapcanemitter.h"
#include "qtBlokLib/iodrv.h"
#include "qtBlokLib/cookies.h"

#ifdef LIB_SOCKET_CAN
#include "qtCanLib/socketcan.h"
#else
#include "qtCanLib/dummycan.h"
#endif
#include "qtBlokLib/parser.h"

#include "nmeareceiver.h"
#include "iodrv/gpsemitter.h"
#include "electroincmap.h"
#include "customordinatehandler.h"

Can *can;
Parser *parser;
NmeaReceiver *nmea;
GpsEmitter *gpsEmitter;
Navigation::ElectroincMap* elMap;
iodrv* iodriver;
EMapCanEmitter* emapCanEmitter;
Cookies *cookies;
CustomOrdinateHandler *customOrdinateHandler;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#ifdef _WIN32
    // Masqarade
    winConsoleandler = GetStdHandle(STD_OUTPUT_HANDLE);
    system("chcp 65001");
#endif

#ifdef LIB_SOCKET_CAN
    can = new SocketCan();
#else
    can = new DummyCan();
#endif
    parser = new Parser (can);
    cookies = new Cookies(can);

    QSerialPort serialPort;
//    QList<QSerialPortInfo> spinfo = QSerialPortInfo::availablePorts();
//    if (spinfo.count() == 0)
//    {
//        fprintf(stderr, "Не найдено ни одного последовательного порта\n"); fflush(stderr);
//        // Для обхода бага библиотеки QtSerialPort под arm обходим эту проверку
//        //return 0;
//    }
//    serialPort.setPort(spinfo.at(0));
    serialPort.setPortName("/dev/ttySAC1");

    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setBaudRate(QSerialPort::Baud115200);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setStopBits(QSerialPort::OneStop);
    serialPort.setFlowControl(QSerialPort::NoFlowControl);
    nmea = new NmeaReceiver (&serialPort);

    gpsEmitter = new GpsEmitter(can);
    gpsEmitter->connect(nmea, SIGNAL(validChanged(bool)), SLOT(setValid(bool)));
    gpsEmitter->connect(nmea, SIGNAL(geoPositionChanged(double,double)), SLOT(setPosition(double,double)));
    gpsEmitter->connect(nmea, SIGNAL(speedChanged(double)), SLOT(setSpeed(double)));
    gpsEmitter->connect(nmea, SIGNAL(dateTimeChanged(QDateTime)), SLOT(setDateTime(QDateTime)));
    gpsEmitter->connect(nmea, SIGNAL(messageReceived()), gpsEmitter,  SLOT(sendData()));
    gpsEmitter->connect(&cookies->timeZone, SIGNAL(onChange(int)), SLOT(setTimeZone(int)));
    cookies->timeZone.requestValue();

    elMap = new Navigation::ElectroincMap();
    iodriver = new iodrv(can);
    emapCanEmitter = new EMapCanEmitter(can);
    customOrdinateHandler = new CustomOrdinateHandler (can, cookies);

    iodriver->start(gps_data_source_gps);

    qDebug() << "Loading map...";
    elMap->load ("/media/dat/QtMap/map.gps");
    qDebug() << "Map loaded.";

    QObject::connect (nmea, SIGNAL(geoPositionChanged(double,double)), elMap, SLOT(checkMap(double,double)));
    QObject::connect (&cookies->trackNumberInMph, SIGNAL(onChange(int)), elMap, SLOT(setTrackNumber(int)));
    QObject::connect (emapCanEmitter, SIGNAL(metrometerChanged(int)), elMap, SLOT(setMetrometer(int)));
    QObject::connect (emapCanEmitter, SIGNAL(metrometerReset()), elMap, SLOT(resetMetrometer()));
    QObject::connect (customOrdinateHandler, SIGNAL(ordinateChanged(int)), elMap, SLOT(setCustomOrdinate(int)));
    QObject::connect (customOrdinateHandler, SIGNAL(directionChanged(int)), elMap, SLOT(setCustomDirection(int)));
    QObject::connect (elMap, SIGNAL(onUpcomingTargets(std::vector<EMapTarget>)), emapCanEmitter, SLOT(setObjectsList(std::vector<EMapTarget>)));
    QObject::connect (elMap, SIGNAL(ordinateUpdated(int)), emapCanEmitter, SLOT(sendOrdinate(int)));
    QObject::connect (elMap, SIGNAL(isLocatedChanged(bool)), emapCanEmitter, SLOT(setActivity(bool)));
//    QObject::connect (elMap, SIGNAL(activityChanged(bool)), emapCanEmitter, SLOT(setActivity(bool)));
//    QObject::connect (elMap, SIGNAL(ordinateUpdated(int)), customOrdinateHandler, SLOT(setCurrentOrdinate(int)));

    cookies->trackNumberInMph.requestValue ();
    customOrdinateHandler->requestValues();
    
    return a.exec();
}
