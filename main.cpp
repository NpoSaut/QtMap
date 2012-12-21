#include <QApplication>
#include "qmlapplicationviewer.h"
#include "systemstateviewmodel.h"
#include "qtconcurrentrun.h"
#include <QTextStream>

#include "sktcanl/src/sktcanl.h"
#include <iostream>

SystemStateViewModel *systemState ;

void getSpeed (double* speed)
{
    systemState->setSpeed(*speed);
}

//extern void aFunction();
//QFuture<void> future = QtConcurrent::run(aFunction);
void getParamsFromCan ()
{
    sktcanl_init();
    sktcanl_set_callbacks(getSpeed, NULL, NULL);

    while (true)
    {
        sktcanl_read_can_msg();
    }
}

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
        else if (cmd.at(0) == "c")
        {
            systemState->setLight( cmd.at(1).toInt() );
            out << "Liht: " << systemState->getLight() << endl;
        }
        else
        {
            out << "! unknown command. Try this: s r map property g" << endl;
        }
    }
}

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    qmlRegisterType<SystemStateViewModel>("views", 1, 0, "SystemStateView");

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/QtMon/main.qml"));
#ifdef QT_DEBUG
    viewer.showExpanded();
#else
    viewer.showFullScreen();
#endif

    QObject *object = viewer.rootObject();
    systemState = object->findChild<SystemStateViewModel*>("stateView");

    QtConcurrent::run(getParamsFromConsole);
//    QtConcurrent::run(getParamsFromCan);

    return app->exec();
}
