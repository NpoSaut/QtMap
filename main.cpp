#include <QCoreApplication>

#include "cDoodahLib/masqarade.h"
#ifdef WIN32
    HANDLE winConsoleandler;
#endif

#include "electroincmap.h"
#include "iodrv/emapcanemitter.h"
#include "qtBlokLib/iodrv.h"
#include "qtBlokLib/cookies.h"
#include "qtCanLib/socketcan.h"

Can *can;
Navigation::ElectroincMap* elMap;
iodrv* iodriver;
EMapCanEmitter* emapCanEmitter;
Cookies *cookies;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#ifdef WIN32
    // Masqarade
    winConsoleandler = GetStdHandle(STD_OUTPUT_HANDLE);
    system("chcp 65001");
#endif

    can = new SocketCan();
    elMap = new Navigation::ElectroincMap();
    iodriver = new iodrv(can);
    emapCanEmitter = new EMapCanEmitter(can);
    cookies = new Cookies(can);


    iodriver->start(gps_data_source_gps);

    cookies->trackNumberInMph.requestValue ();

    qDebug() << "Loading map...";
    elMap->load ("./map.gps");
    qDebug() << "Map loaded.";

    QObject::connect (iodriver, SIGNAL(signal_lat_lon(double,double)), elMap, SLOT(checkMap(double,double)));
    QObject::connect (&cookies->trackNumberInMph, SIGNAL(onChange(int)), elMap, SLOT(setTrackNumber(int)));
    QObject::connect (emapCanEmitter, SIGNAL(metrometerChanged(int)), elMap, SLOT(setMetrometer(int)));
    QObject::connect (emapCanEmitter, SIGNAL(metrometerReset()), elMap, SLOT(resetMetrometer()));
    QObject::connect (elMap, SIGNAL(onUpcomingTargets(std::vector<EMapTarget>)), emapCanEmitter, SLOT(setObjectsList(std::vector<EMapTarget>)));
    QObject::connect (elMap, SIGNAL(ordinateChanged(int)), emapCanEmitter, SLOT(setOrdinate(int)));
    QObject::connect (elMap, SIGNAL(isLocatedChanged(bool)), emapCanEmitter, SLOT(setActivity(bool)));
    QObject::connect (elMap, SIGNAL(activityChanged(bool)), emapCanEmitter, SLOT(setActivity(bool)));
    
    return a.exec();
}
