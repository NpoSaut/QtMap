#if defined WITH_CAN

#include "emapcanemitter.h"

using namespace std;
using namespace Navigation;

EMapCanEmitter::EMapCanEmitter(QObject *parent) :
    QObject(parent),
    timer(parent), // Частота выдача в can объекта. Должна быть 100 мс, но из-за проблем со временем 50 -TiME-
    step (0)
{
    timer.setInterval (50);
    timer.start ();
    QObject::connect (&timer, SIGNAL(timeout()), this, SLOT(engine()));
}

void EMapCanEmitter::engine()
{
    if (step == 0)
    {
        mutex.lock ();
        sendingObjects = receivedObjects;
        mutex.unlock ();
    }

    if (step < sendingObjects.size ())
    {
        RailObject &obj = *sendingObjects[step].object;
        int &x = sendingObjects[step].x;

        can_frame canFrame;
        canFrame.can_id = 0x21F; // 0x43E8
        canFrame.can_dlc = 8;

        CanMessageData &canMessage = *((CanMessageData *) &canFrame.data);
        canMessage.errors = 0;
        canMessage.number = step;
        canMessage.type = obj.getType ();
        canMessage.x = x;
        canMessage.radioChanel = obj.isRadioChannel ();
        canMessage.station = 0; // ??? Откуда взять
        canMessage.alsEn = obj.isAlsEn ();
        canMessage.lengthLowByte = char (obj.getLength ());
        canMessage.lengthHighByte = char (obj.getLength ()>> 8);
        canMessage.alsnFreqCode = obj.getAlsnFreq ();
        canMessage.alsEnNewTable = 0; // ??? Откуда взять
        canMessage.pullforthForFreightTrain =  obj.isPullforthForFreightTrain ();
        canMessage.pullforthForPassengerTrain = obj.isPullforthForPassengerTrain ();
        canMessage.conditionallyAllow = obj.isConditionallyAllowForFreightTrain ();
        canMessage.targetSpeed = char (obj.getSpeedRestriction ());
        canMessage.targetSpeedHightBit = char (obj.getSpeedRestriction ()>> 8);

        emit sendNextObjectToCan (canFrame);

        // Вывод названия цели
        if (step == 0)
        {
            canFrame.can_id = 0x603; // 0xC068
            canFrame.can_dlc = 8;
            memcpy (&canFrame.data, obj.getName().toAscii().data(), 8);

            emit sendNextObjectToCan (canFrame);
        }
    }

    if (++step == 10)
        step = 0;
}

void EMapCanEmitter::setObjectsList(const vector<EMapTarget> &objects)
{
    receivedObjects = objects;
}

#endif
