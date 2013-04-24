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
SpeedAgregator* speedAgregator;
rmp_key_handler* rmp_key_hdlr;
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
            systemState->setAlsnFreqTarget( cmd.at(1).toInt() );
            out << "AlsnFreqTarget: " << systemState->getAlsnFreqTarget() << endl;
        }
        else if (cmd.at(0) == "epv")
        {
            systemState->setIsEpvReady( cmd.at(1) == "1" );
            out << "IsEpvReady: " << systemState->getIsEpvReady() << endl;
        }
        else if (cmd.at(0) == "dm")
        {
            systemState->setDriveModeFact( cmd.at(1).toInt() );
            out << "DriveModeFact: " << systemState->getDriveModeFact() << endl;
        }
        else if (cmd.at(0) == "iw")
        {
            systemState->setIronWheels( cmd.at(1) == "1" );
            out << "Iron Wheels: " << systemState->getIronWheels() << endl;
        }
        // Тяга
        else if (cmd.at(0) == "tr")
        {
            systemState->setIsTractionOn( cmd.at(1) == "1" );
            out << "Traction is: " << systemState->getIsTractionOn() << endl;
        }
        else
        {
            out << "! unknown command. Try this:" << endl;
            out << "iw {1/0} IronWheels" << endl;
            out << "tr {1/0} Тяга: вкл/выкл" << endl;
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
    //Здесь подключаюсь я.
    iodriver = new iodrv(systemState);
    speedAgregator = new SpeedAgregator();

    // Создание и подключение «обработчиков»
    // -> Отбработчик нажатия РМП <-
    rmp_key_hdlr = new rmp_key_handler();

    QObject::connect(systemState, SIGNAL(ChangeDrivemodeButtonPressed()), rmp_key_hdlr, SLOT(rmp_key_pressed()));
    QObject::connect(iodriver, SIGNAL(signal_ssps_mode(int)), rmp_key_hdlr, SLOT(ssps_mode_received(int)));
    QObject::connect(iodriver, SIGNAL(signal_driving_mode(int)), rmp_key_hdlr, SLOT(driving_mode_received(int)));

    QObject::connect(rmp_key_hdlr, SIGNAL(target_driving_mode_changed(int)), systemState, SLOT(setDriveModeTarget(int)));
    QObject::connect(rmp_key_hdlr, SIGNAL(actual_driving_mode_changed(int)), systemState, SLOT(setDriveModeFact(int)));
    QObject::connect(rmp_key_hdlr, SIGNAL(rmp_key_pressed_send()), iodriver, SLOT(slot_rmp_key_down()));
    // <- Отбработчик нажатия РМП ->

    // Переносить ли эти события из iodrv в обработчики
    QObject::connect(iodriver, SIGNAL(signal_iron_wheels(bool)), systemState, SLOT(setIronWheels(bool)));



    // Для отладки
    QObject::connect(iodriver, SIGNAL(signal_speed_earth(double)), systemState, SLOT(setSpeedFromEarth(double)));
    QObject::connect(iodriver, SIGNAL(signal_speed_sky(double)), systemState, SLOT(setSpeedFromSky(double)));

    //Скорость и ограничения
    QObject::connect(iodriver, SIGNAL(signal_speed_earth(double)), speedAgregator, SLOT(getSpeedFromEarth(double)));
    QObject::connect(iodriver, SIGNAL(signal_speed_sky(double)), speedAgregator, SLOT(getSpeedFromSky(double)));
    QObject::connect(iodriver, SIGNAL(signal_is_on_road(bool)), speedAgregator, SLOT(getIsOnRoad(bool)));
    QObject::connect(speedAgregator, SIGNAL(speedChanged(double)), systemState, SLOT(setSpeed(double)));
    QObject::connect(speedAgregator, SIGNAL(speedIsValidChanged(bool)), systemState, SLOT(setSpeedIsValid(bool)));
    QObject::connect(iodriver, SIGNAL(signal_speed_limit(int)), systemState, SLOT(setSpeedRestriction(int)));
    QObject::connect(iodriver, SIGNAL(signal_target_speed(int)), systemState, SLOT(setTargetSpeed(int)));
    QObject::connect(iodriver, SIGNAL(signal_acceleration(double)), systemState, SLOT(setAcceleration(double)));
    //Состояние системы
    QObject::connect(iodriver, SIGNAL(signal_epv_state(bool)), systemState, SLOT(setIsEpvReady(bool)));
    QObject::connect(iodriver, SIGNAL(signal_epv_key(bool)), systemState, SLOT(setIsEpvReleased(bool)));
    //Одометр
    QObject::connect(iodriver, SIGNAL(signal_passed_distance(int)), systemState, SLOT(setMilage(int)));
    //Светофоры
    QObject::connect(iodriver, SIGNAL(signal_trafficlight_light(int)), systemState, SLOT(setLight(int)));
    QObject::connect(iodriver, SIGNAL(signal_trafficlight_freq(int)), systemState, SLOT(setAlsnFreqFact(int)));

    //QObject::connect(iodriver, SIGNAL(signal_driving_mode(int)), systemState, SLOT(setDriveModeFact(int)));
    QObject::connect(iodriver, SIGNAL(signal_vigilance(bool)), systemState, SLOT(setIsVigilanceRequired(bool)));
    QObject::connect(iodriver, SIGNAL(signal_movement_direction(int)), systemState, SLOT(setDirection(int)));
    QObject::connect(iodriver, SIGNAL(signal_reg_tape_avl(bool)), systemState, SLOT(setIsRegistrationTapeActive(bool)));

    QObject::connect(iodriver, SIGNAL(signal_pressure_tc(QString)), systemState, SLOT(setPressureTC(QString)));
    QObject::connect(iodriver, SIGNAL(signal_pressure_tm(QString)), systemState, SLOT(setPressureTM(QString)));

    QObject::connect(iodriver, SIGNAL(signal_lat(double)), systemState, SLOT(setLatitude(double)));
    QObject::connect(iodriver, SIGNAL(signal_lon(double)), systemState, SLOT(setLongitude(double)));
    QObject::connect(iodriver, SIGNAL(signal_time(QString)), systemState, SLOT(setTime(QString)));
    QObject::connect(iodriver, SIGNAL(signal_date(QString)), systemState, SLOT(setDate(QString)));

    QObject::connect(iodriver, SIGNAL(signal_traction(bool)), systemState, SLOT(setIsTractionOn(bool)));
    QObject::connect(iodriver, SIGNAL(signal_is_on_road(bool)), systemState, SLOT(setPropertyView(bool)));

    //QObject::connect(systemState, SIGNAL(AlsnFreqTargetChanged()), iodriver, SLOT(slot_f_key_down()));
    QObject::connect(systemState, SIGNAL(DisableRedButtonPressed()), iodriver, SLOT(slot_vk_key_down()));
    QObject::connect(systemState, SIGNAL(DisableRedButtonReleased()), iodriver, SLOT(slot_vk_key_up()));
    //QObject::connect(systemState, SIGNAL(ChangeDrivemodeButtonPressed()), iodriver, SLOT(slot_rmp_key_down()));
    //QObject::connect(systemState, SIGNAL(ChangeDrivemodeButtonReleased()), iodriver, SLOT(slot_rmp_key_up()));

    // TODO: QObject::connect(systemState, SIGNAL(), iodriver, SLOT(slot_vk_key_up()));
    // TODO: QObject::connect(systemState, SIGNAL(), iodriver, SLOT(slot_rmp_key_up()));

    iodriver->start(argv[1], argv[2], (QString(argv[3]).toInt() == 0) ? gps : can);

#else
    QtConcurrent::run(getParamsFromConsole);
#endif

    return app->exec();
}
