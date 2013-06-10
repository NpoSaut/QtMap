#if defined WITH_CAN

#include "emapcanemitter.h"
#include <QDebug>

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

EMapCanEmitter::CanMessageData EMapCanEmitter::encodeEMapTarget(const EMapTarget& t, int targetNumber)
{
    CanMessageData canMessage;

    canMessage.errors = 0;
    canMessage.number = targetNumber;
    canMessage.type = t.object->getType ();
    canMessage.xLow = t.x & 0xFF;
    canMessage.xHight = t.x >> 8;
    canMessage.radioChanel = t.object->isRadioChannel ();
    canMessage.station = 0; // ??? Откуда взять
    canMessage.alsEn = t.object->isAlsEn ();
    canMessage.lengthLowByte = char (t.object->getLength ());
    canMessage.lengthHighByte = char (t.object->getLength ()>> 8);
    canMessage.alsnFreqCode = t.object->getAlsnFreq ();
    canMessage.alsEnNewTable = 0; // ??? Откуда взять
    canMessage.pullforthForFreightTrain =  t.object->isPullforthForFreightTrain ();
    canMessage.pullforthForPassengerTrain = t.object->isPullforthForPassengerTrain ();
    canMessage.conditionallyAllow = t.object->isConditionallyAllowForFreightTrain ();
    canMessage.targetSpeed = char (t.object->getSpeedRestriction ());
    canMessage.targetSpeedHightBit = char (t.object->getSpeedRestriction ()>> 8);

    return canMessage;
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
//    if (step < sendingObjects.size () && step == 0)
    {
        can_frame canFrame;
        canFrame.can_id = 0x422; // 0x8448
//        canFrame.can_id = 0x21F; // 0x43E8
        canFrame.can_dlc = 8;

        CanMessageData &canMessage = *((CanMessageData *) &canFrame.data);
        canMessage = encodeEMapTarget(sendingObjects[step], step); // Это просто очень плохо

//        qDebug() << "target: " << sendingObjects[step].x << " .." << canFrame.data[2] << " " << canFrame.data[3];

        emit sendNextObjectToCan (canFrame);

        // Вывод названия цели
        if (step == 0)
        {
            canFrame.can_id = 0x603; // 0xC068
            canFrame.can_dlc = 8;
            memcpy (&canFrame.data, sendingObjects[step].object->getName().toAscii().data(), 8);

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
