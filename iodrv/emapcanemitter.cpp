#if defined WITH_CAN

#include "lowlevel.h"
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

    CanFrame canFrame (0x8448, std::vector<unsigned char>(8)); // Должно быть 0x43E8
    if (step < sendingObjects.size ())
    {
        CanMessageData canMessage = encodeEMapTarget(sendingObjects[step], step);
        canFrame.setData (std::vector<unsigned char>((unsigned char *)(&canMessage), (unsigned char *)(&canMessage) + 8) );
    }
    else
    {
        std::vector<unsigned char> data (8);
        data[0] = 0;
        data[1] = step; // Пустой объект
        canFrame.setData (data);
    }
    emit sendNextObjectToCan (canFrame);

    if (++step == 10)
    {
        step = 0;

        // Вывод названия цели
//        emit sendNextObjectToCan (
//                    CanFrame (0xC068,
//                              std::vector<unsigned char> ( sendingObjects[step].object->getName().toAscii().data(),
//                                                           sendingObjects[step].object->getName().toAscii().data() + 8) )
//                                 );
    }
}

void EMapCanEmitter::setObjectsList(const vector<EMapTarget> &objects)
{
    receivedObjects = objects;
}

#endif
