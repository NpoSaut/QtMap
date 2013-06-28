#include <iostream>
#include <math.h>

#include <QApplication>
#include <QtConcurrentRun>
#include <QTextStream>
#include <QTextCodec>
#include <QString>
#include <QtConcurrentRun>
#include <qmlapplicationviewer.h>

#include "systemstateviewmodel.h"
#include "electroincmap.h"

#include "masqarade.h"
#ifdef WIN32
    HANDLE winConsoleandler;
#endif

#include "iodrv/can.h"
#ifdef WITH_CAN
#include "iodrv/iodrv.h"
#include "iodrv/cookies.h"
#include "iodrv/emapcanemitter.h"
#endif


SystemStateViewModel *systemState ;
Navigation::ElectroincMap* elMap;

#ifdef WITH_CAN
iodrv* iodriver;
SpeedAgregator* speedAgregator;
rmp_key_handler* rmp_key_hdlr;
EMapCanEmitter* emapCanEmitter;
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
        else if (cmd.at(0) == "b")
        {
            systemState->setIsEpvReleased( cmd.at(1) == "1" );
            out << "IsEpvReady: " << systemState->getIsEpvReleased() << endl;
        }
        else if (cmd.at(0) == "dm")
        {
            systemState->setDriveModeFact( cmd.at(1).toInt() );
            out << "DriveModeFact: " << systemState->getDriveModeFact() << endl;
        }
        else if (cmd.at(0) == "tdm")
        {
            systemState->setDriveModeTarget( cmd.at(1).toInt() );
            out << "DriveModeTarget: " << systemState->getDriveModeTarget() << endl;
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
        // Направление движения
        else if (cmd.at(0) == "dir")
        {
            systemState->setDirection( cmd.at(1).toInt() );
            out << "Now Direction is: " << systemState->getDirection() << endl;
        }
        else if (cmd.at(0) == "wt")
        {
            systemState->setWarningText( cmd.at(1) );
            out << "Now Warning Text is: " << systemState->getWarningText() << endl;
        }
        else if (cmd.at(0) == "it")
        {
            systemState->setInfoText( cmd.at(1) );
            out << "Now Info Text is: " << systemState->getInfoText() << endl;
        }
        else
        {
            out << "! unknown command. Try this:" << endl;
            out << "tdm {0/1/2/3/4} Целевой режим движения: П/М/Р/Д/Т" << endl;
            out << "dm {0/1/2/3/4} Фактический режим движения: П/М/Р/Д/Т" << endl;
            out << "epb {1/0} Ключ ЭПК: вкл/выкл" << endl;
            out << "b {1/0} Экстренное торможение: вкл/выкл" << endl;
            out << "iw {1/0} IronWheels" << endl;
            out << "tr {1/0} Тяга: вкл/выкл" << endl;
            out << "dir {1/-1/0} Направление движения: вперёд/назад/стоим" << endl;
            out << "wt {text} Текст предупреждения" << endl;
            out << "it {text} Текст совета" << endl;
        }
    }
}



Q_DECL_EXPORT int main(int argc, char *argv[])
{

#ifdef WIN32
    // Masqarade
    winConsoleandler = GetStdHandle(STD_OUTPUT_HANDLE);
    system("chcp 65001");
#endif
    CPRINTF(CL_VIOLET_L, "ДОБРЫЙ ДЕНЬ\n");

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
    elMap = new Navigation::ElectroincMap();

#ifdef WITH_CAN
    //QtConcurrent::run(getParamsFromCan);
    //Здесь подключаюсь я.
    iodriver = new iodrv(systemState);
    speedAgregator = new SpeedAgregator();
    emapCanEmitter = new EMapCanEmitter();

    // Создание и подключение «обработчиков»
    // -> Отбработчик нажатия РМП <-
    rmp_key_hdlr = new rmp_key_handler();

    // Передача сообщения в новый CAN-класс
    qRegisterMetaType<CanFrame>("CanFrame");
    QObject::connect (iodriver, SIGNAL(signal_new_message(CanFrame)), &canDev, SLOT(receiveFromIoDrv(CanFrame)));
    QObject::connect (&canDev, SIGNAL(transmitToIoDrv(CanFrame)), iodriver, SLOT(slot_send_message(CanFrame)));

    QObject::connect(systemState, SIGNAL(ChangeDrivemodeButtonPressed()), rmp_key_hdlr, SLOT(rmp_key_pressed()));
    QObject::connect(iodriver, SIGNAL(signal_ssps_mode(int)), rmp_key_hdlr, SLOT(ssps_mode_received(int)));
    QObject::connect(iodriver, SIGNAL(signal_driving_mode(int)), rmp_key_hdlr, SLOT(driving_mode_received(int)));

    QObject::connect(rmp_key_hdlr, SIGNAL(target_driving_mode_changed(int)), systemState, SLOT(setDriveModeTarget(int)));
    QObject::connect(rmp_key_hdlr, SIGNAL(actual_driving_mode_changed(int)), systemState, SLOT(setDriveModeFact(int)));
    QObject::connect(rmp_key_hdlr, SIGNAL(rmp_key_pressed_send()), iodriver, SLOT(slot_rmp_key_down()));
//    QObject::connect(rmp_key_hdlr, SIGNAL(rmp_key_pressed_send()), iodriver, SLOT(slot_rmp_key_down()));
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
    QObject::connect(iodriver, SIGNAL(signal_epv_key(bool)), systemState, SLOT(setIsEpvReady(bool)));
    QObject::connect(iodriver, SIGNAL(signal_epv_released(bool)), systemState, SLOT(setIsEpvReleased(bool)));
    QObject::connect (iodriver, SIGNAL(signal_modules_activity(QString)), systemState, SLOT(setModulesActivityString(QString)));
    //Одометр
    QObject::connect(iodriver, SIGNAL(signal_passed_distance(int)), systemState, SLOT(setMilage(int)));
    //Светофоры
    QObject::connect(iodriver, SIGNAL(signal_trafficlight_light(int)), systemState, SLOT(setLight(int)));
    QObject::connect(iodriver, SIGNAL(signal_trafficlight_freq(int)), systemState, SLOT(setAlsnFreqFact(int)));

    QObject::connect(iodriver, SIGNAL(signal_vigilance(bool)), systemState, SLOT(setIsVigilanceRequired(bool)));
    QObject::connect(iodriver, SIGNAL(signal_movement_direction(int)), systemState, SLOT(setDirection(int)));
    QObject::connect(iodriver, SIGNAL(signal_reg_tape_avl(bool)), systemState, SLOT(setIsRegistrationTapeActive(bool)));

    QObject::connect(iodriver, SIGNAL(signal_autolock_type(int)), systemState, SLOT(setAutolockTypeFact(int)));
    QObject::connect(systemState, SIGNAL(AutolockTypeTargetChanged(int)), iodriver, SLOT(slot_autolock_type_target_changed(int)));
//    QObject::connect(systemState, SIGNAL(AutolockTypeTargetChanged()), iodriver, SLOT(slot_autolock_type_target_changed()));

    QObject::connect(iodriver, SIGNAL(signal_pressure_tc(QString)), systemState, SLOT(setPressureTC(QString)));
    QObject::connect(iodriver, SIGNAL(signal_pressure_tm(QString)), systemState, SLOT(setPressureTM(QString)));

    QObject::connect(iodriver, SIGNAL(signal_lat(double)), systemState, SLOT(setLatitude(double)));
    QObject::connect(iodriver, SIGNAL(signal_lon(double)), systemState, SLOT(setLongitude(double)));
    QObject::connect(iodriver, SIGNAL(signal_time(QString)), systemState, SLOT(setTime(QString)));
    QObject::connect(iodriver, SIGNAL(signal_date(QString)), systemState, SLOT(setDate(QString)));

    QObject::connect(iodriver, SIGNAL(signal_traction(bool)), systemState, SLOT(setIsTractionOn(bool)));

    QObject::connect(systemState, SIGNAL(DisableRedButtonPressed()), iodriver, SLOT(slot_vk_key_down()));
    QObject::connect(systemState, SIGNAL(DisableRedButtonReleased()), iodriver, SLOT(slot_vk_key_up()));

    // Автоблокировка
    QObject::connect(iodriver, SIGNAL(signal_autolock_type_target(int)), systemState, SLOT(setAutolockTypeTarget(int)));

    // Ввод параметров
    QObject::connect (systemState, SIGNAL(TrackNumberChanged(int)), &cookies.trackNumberInMph, SLOT(setVaule(int)));
    QObject::connect (systemState, SIGNAL(MachinistNumberChanged(int)), &cookies.machinistNumber, SLOT(setVaule(int)));
    QObject::connect (systemState, SIGNAL(TrainNumberChanged(int)), &cookies.trainNumber, SLOT(setVaule(int)));
    QObject::connect (systemState, SIGNAL(AxlesCountChanged(int)), &cookies.lengthInWheels, SLOT(setVaule(int)));
    QObject::connect (systemState, SIGNAL(WagonCountChanged(int)), &cookies.lengthInWagons, SLOT(setVaule(int)));
    QObject::connect (systemState, SIGNAL(TrainMassChanged(int)), &cookies.mass, SLOT(setVaule(int)));
    // Чтение параметров
    QObject::connect (&cookies.trackNumberInMph, SIGNAL(onChange(int)), systemState, SLOT(setTrackNumber(int)));
    QObject::connect (&cookies.machinistNumber, SIGNAL(onChange(int)), systemState, SLOT(setMachinistNumber(int)));
    QObject::connect (&cookies.trainNumber, SIGNAL(onChange(int)), systemState, SLOT(setTrainNumber(int)));
    QObject::connect (&cookies.lengthInWheels, SIGNAL(onChange(int)), systemState, SLOT(setAxlesCount(int)));
    QObject::connect (&cookies.lengthInWagons, SIGNAL(onChange(int)), systemState, SLOT(setWagonCount(int)));
    QObject::connect (&cookies.mass, SIGNAL(onChange(int)), systemState, SLOT(setTrainMass(int)));

//    QObject::connect(systemState, SIGNAL(DisableRedButtonPressed()), iodriver, SLOT(slot_vk_key_down()));
//    QObject::connect(systemState, SIGNAL(DisableRedButtonReleased()), iodriver, SLOT(slot_vk_key_up()));

    // Электронная карта
    QObject::connect (iodriver, SIGNAL(signal_orig_passed_distance(int)), elMap, SLOT(setMetrometer(int)));
    QObject::connect (iodriver, SIGNAL(signal_lat_lon(double,double)), elMap, SLOT(checkMap(double,double)));
    QObject::connect (elMap, SIGNAL(onUpcomingTargets(std::vector<EMapTarget>)), emapCanEmitter, SLOT(setObjectsList(std::vector<EMapTarget>)));

    iodriver->start(argv[1], argv[2], (QString(argv[3]).toInt() == 0) ? gps_data_source_gps : gps_data_source_can);

    cookies.trackNumberInMph.requestValue ();
    cookies.machinistNumber.requestValue ();
    cookies.trainNumber.requestValue ();
    cookies.lengthInWheels.requestValue ();
    cookies.lengthInWagons.requestValue ();
    cookies.mass.requestValue ();

#else
    QtConcurrent::run(getParamsFromConsole);
#endif

    qDebug() << "Loading map...";
    elMap->load ("./map.gps");
    qDebug() << "Map loaded.";

//    elMap->setTrackNumber (1);
    QObject::connect (&cookies.trackNumberInMph, SIGNAL(onChange(int)), elMap, SLOT(setTrackNumber(int)));
    QObject::connect (elMap, SIGNAL(ordinateChanged(int)), systemState, SLOT(setOrdinate(int)));
    QObject::connect (elMap, SIGNAL(ordinateChanged(int)), emapCanEmitter, SLOT(setOrdinate(int)));
    QObject::connect (emapCanEmitter, SIGNAL(targetDistanceChanged(int)), systemState, SLOT(setNextTargetDistance(int)));
    QObject::connect (emapCanEmitter, SIGNAL(targetNameChanged(QString)), systemState, SLOT(setNextTargetName(QString)));
    QObject::connect (emapCanEmitter, SIGNAL(targetTypeChanged(int)), systemState, SLOT(setNextTargetKind(int)));

    return app->exec();
}
