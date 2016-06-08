#include <QCoreApplication>

#include "cDoodahLib/masqarade.h"
#ifdef _WIN32
    #include <windows.h>
    HANDLE winConsoleandler;
#endif

#include "electroincmap.h"
#include "customordinatehandler.h"

#include "iodrv/emapcanemitter.h"
#include "qtBlokLib/iodrv.h"
#include "qtBlokLib/cookies.h"

#ifdef LIB_SOCKET_CAN
#include "qtCanLib/socketcan.h"
#else
#include "qtCanLib/dummycan.h"
#endif

Can *can;
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
    elMap = new Navigation::ElectroincMap();
    iodriver = new iodrv(can);
    emapCanEmitter = new EMapCanEmitter(can);
    cookies = new Cookies(can);
    customOrdinateHandler = new CustomOrdinateHandler (can, cookies);

    iodriver->start(gps_data_source_gps);

    qDebug() << "Loading map...";
    elMap->load ("/media/dat/QtMap/map.gps");
    qDebug() << "Map loaded.";

    QObject::connect (iodriver, SIGNAL(signal_lat_lon(double,double)), elMap, SLOT(checkMap(double,double)));
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
