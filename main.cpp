#include <QApplication>
#include "qmlapplicationviewer.h"
#include "systemstateviewmodel.h"
#include "qtconcurrentrun.h"
#include <QTextStream>
#include <QTextCodec>

#ifdef WITH_CAN
#include "iodrv/iodrv.h"
#endif

#include <iostream>

SystemStateViewModel *systemState ;

#ifdef WITH_CAN
iodrv* iodriver;
#endif

/*void getSpeed (double* speed)
{
    systemState->setSpeed( int(*speed) );
}

void getSpeedLimits (int* val)
{
    systemState->setSpeedRestriction(*val);
}

void getLights (int* code)
{
    systemState->setLight(*code-1);
}

void getAlsn (int* code)
{
    if (*code == 0)
        systemState->setAlsnFreq(50);
    else if (*code == 1)
        systemState->setAlsnFreq(75);
    else if (*code == 2)
        systemState->setAlsnFreq(50);
    else if (*code == 3)
        systemState->setAlsnFreq(25);
}

void getMilage (int* val)
{
    systemState->setMilage( (*val)/1000 );
}

void getGps (double* lat, double* lon)
{
    systemState->setLongitude(*lon);
    systemState->setLatitude(*lat);
}

void getDateTime (int* h, int* m, int* s)
{
    QString date = QString("%1:%2:%3").arg(*h).arg(*m).arg(*s);
    systemState->setTime(date);
}*/

#ifdef WITH_CAN
//extern void aFunction();
//QFuture<void> future = QtConcurrent::run(aFunction);
/*void getParamsFromCan ()
{
    sktcanl_init();
    sktcanl_set_callbacks(getSpeed, getSpeedLimits, NULL, NULL, getLights, getAlsn, getMilage, NULL, NULL, getGps, getDateTime);

    while (true)
    {
        sktcanl_read_can_msg();
    }
}*/
#endif

void getParamsFromConsole ()
{
    QString str;
    QTextStream in(stdin);
    QTextStream out(stdout);

    while (true)
    {
        str = in.readLine();
        QStringList cmd = str.split(" ");

        if (cmd.at(0) == "s")
        {
            systemState->setSpeed( cmd.at(1).toInt() );
            out << "Speed: " << systemState->getSpeed() << endl;
        }
        else if (cmd.at(0) == "r")
        {
            systemState->setSpeedRestriction( cmd.at(1).toInt() );
            out << "Speed restriction: " << systemState->getSpeedRestriction() << endl;
        }
        else if (cmd.at(0) == "map")
        {
            systemState->setPropertyView(false);
            out << "PropertyView mode: " << systemState->getPropertyView() << endl;
        }
        else if (cmd.at(0) == "property")
        {
            systemState->setPropertyView(true);
            out << "PropertyView mode: " << systemState->getPropertyView() << endl;
        }
        else if (cmd.at(0) == "g")
        {
            if (cmd.size() == 3)
            {
                systemState->setLongitude( cmd.at(1).toDouble() );
                systemState->setLatitude( cmd.at(2).toDouble() );
            }
            out << "Longitude: " << systemState->getLongitude()
                << "; Lattitude: " << systemState->getLatitude() << endl;
        }
        else if (cmd.at(0) == "gg")
        {
            if (cmd.size() == 3)
            {
                systemState->setLongitude(systemState->getLongitude() + cmd.at(1).toDouble() );
                systemState->setLatitude(systemState->getLatitude() + cmd.at(2).toDouble() );
            }
            out << "Longitude: " << systemState->getLongitude()
                << "; Lattitude: " << systemState->getLatitude() << endl;
        }
        else if (cmd.at(0) == "c")
        {
            systemState->setLight( cmd.at(1).toInt() );
            out << "Liht: " << systemState->getLight() << endl;
        }
        else if (cmd.at(0) == "a")
        {
            systemState->setAlsnFreq( cmd.at(1).toInt() );
            out << "AlsnFreq: " << systemState->getAlsnFreq() << endl;
        }
        else
        {
            out << "! unknown command. Try this: s r map property g c a" << endl;
        }
    }
}

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    QTextCodec* codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForCStrings(codec);

    qmlRegisterType<SystemStateViewModel>("views", 1, 0, "SystemStateView");

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/QtMon/main.qml"));
#ifdef ON_DEVICE
    viewer.showFullScreen();
#else
    viewer.showExpanded();
#endif

    QObject *object = viewer.rootObject();
    systemState = object->findChild<SystemStateViewModel*>("stateView");

#ifdef WITH_CAN
    //QtConcurrent::run(getParamsFromCan);
    // Здесь подключаюсь я.
    iodriver = new iodrv();

    QObject::connect(iodriver, SIGNAL(signal_speed(double)), systemState, SLOT(setSpeed(double)));
    QObject::connect(iodriver, SIGNAL(signal_speed_limit(int)), systemState, SLOT(setSpeedRestriction(int)));
    //connect(iodriver, SIGNAL(signal_stop_flag(int)), systemState, SLOT((int)));
    //connect(iodriver, SIGNAL(signal_movement_direction(int)), systemState, SLOT((int)));
    QObject::connect(iodriver, SIGNAL(signal_trafficlight_light(int)), systemState, SLOT(setLight(int)));
    QObject::connect(iodriver, SIGNAL(signal_trafficlight_freq(int)), systemState, SLOT(setAlsnFreq(int)));
    QObject::connect(iodriver, SIGNAL(signal_passed_distance(int)), systemState, SLOT(setMilage(int)));
    //connect(iodriver, SIGNAL(signal_epv_state(int)), systemState, SLOT((int)));
    //connect(iodriver, SIGNAL(signal_epv_key(int)), systemState, SLOT((int)));
    QObject::connect(iodriver, SIGNAL(signal_lat(double)), systemState, SLOT(setLatitude(double)));
    QObject::connect(iodriver, SIGNAL(signal_lon(double)), systemState, SLOT(setLongitude(double)));
    QObject::connect(iodriver, SIGNAL(signal_time(QString)), systemState, SLOT(setTime(QString)));
    QObject::connect(iodriver, SIGNAL(signal_date(QString)), systemState, SLOT(setDate(QString)));
    QObject::connect(systemState, SIGNAL(AlsnFreqChanged()), iodriver, SLOT(slot_fkey_down()));

    iodriver->start(argv[1], (QString(argv[2]).toInt() == 0) ? gps : can);

#else
    QtConcurrent::run(getParamsFromConsole);
#endif

    return app->exec();
}
