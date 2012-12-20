#include <QApplication>
#include "qmlapplicationviewer.h"
#include "systemstateviewmodel.h"
#include "qtconcurrentrun.h"

#include "sktcanl/src/sktcanl.h"
#include <iostream>

SystemStateViewModel *systemState ;

void getSpeed (double* speed)
{
    systemState->setSpeed(*speed);
}

//extern void aFunction();
//QFuture<void> future = QtConcurrent::run(aFunction);
void getQtSpeed ()
{
    while (true)
    {
        sktcanl_read_can_msg();
    }
}

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    qmlRegisterType<SystemStateViewModel>("views", 1, 0, "SystemStateView");

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/QtMon/main.qml"));
    viewer.showExpanded();

    QObject *object = viewer.rootObject();
    systemState = object->findChild<SystemStateViewModel*>("stateView");

    sktcanl_init();
    sktcanl_set_callbacks(getSpeed, NULL, NULL);
    QtConcurrent::run(getQtSpeed);
    //sktcanl_read_can_msg();

    return app->exec();
}
