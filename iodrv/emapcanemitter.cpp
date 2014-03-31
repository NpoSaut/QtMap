#include <QTextCodec>

#include "cDoodahLib/lowlevel.h"
#include "emapcanemitter.h"

#include <QDebug>

using namespace std;
using namespace Navigation;

EMapCanEmitter::EMapCanEmitter(Can *can, QObject *parent) :
    QObject(parent),
    can (can),
    timer(parent),
    sendingObjects(), receivedObjects(),
    step (0), targetNumber(0), targetDistance(0), active(false), ipdRestart(false)
{
    timer.setInterval (100); // Частота выдача в can объекта.
    timer.start ();
    QObject::connect (&timer, SIGNAL(timeout()), this, SLOT(engine()));

    // Получение данных о текущей цели из ЦО
    QObject::connect (can, SIGNAL(messageReceived(CanFrame)), this, SLOT(getTargetDistanceFromMcoState(CanFrame)));
    QObject::connect (can, SIGNAL(messageReceived(CanFrame)), this, SLOT(getTargetNumberFromMcoLimits(CanFrame)));
    QObject::connect (can, SIGNAL(messageReceived(CanFrame)), this, SLOT(getMetrometerFromIpdState(CanFrame)));
    QObject::connect (can, SIGNAL(messageReceived(CanFrame)), this, SLOT(getIpdRestartFromSautInfo(CanFrame)));
    QObject::connect (can, SIGNAL(messageReceived(CanFrame)), this, SLOT(getLatLonFromMmLatLong(CanFrame)));
}

EMapCanEmitter::CanMessageData EMapCanEmitter::encodeEMapTarget(const EMapTarget& t, int targetNumber)
{
    CanMessageData canMessage;

    canMessage.errors = (active ? 0 : 1);
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
    CanFrame canFrame (0x43E8, std::vector<unsigned char>(8)); // MM_STATE id: 21F
    if (step < sendingObjects.size ())
    {
        CanMessageData canMessage = encodeEMapTarget(sendingObjects[step], step);
        canFrame.setData (std::vector<unsigned char>((unsigned char *)(&canMessage), (unsigned char *)(&canMessage) + 8) );
    }
    else
    {
        std::vector<unsigned char> data (8);
        data[0] = (active ? 0 : 1);
        data[1] = step; // Пустой объект
        canFrame.setData (data);
    }
    can->transmitMessage (canFrame); // ^(o,0)^

    if (++step == 10)
    {
        step = 0;

        // Вывод названия ближайщей цели
        if ( targetNumber < sendingObjects.size () )
          if (active)
          {
              QTextCodec *codec = QTextCodec::codecForName("CP1251");

              char *name = codec->fromUnicode (
                          sendingObjects[targetNumber].object->getName().leftJustified(8, ' ')
                                                ).data ();

              can->transmitMessage (
                          CanFrame(0xC068, std::vector<unsigned char> (name, name + 8)) );
          }

        // Обновление списка целей
        mutex.lock ();
        sendingObjects = receivedObjects;
        mutex.unlock ();
    }
}

void EMapCanEmitter::getTargetDistanceFromMcoState(CanFrame canFrame)
{
    if ( canFrame.getDescriptor () == 0x0A08 )
    {
        signed char high = unsigned(canFrame.getData ()[3] & 0x1F);
        Complex<int16_t> newTargetDistance ({canFrame.getData ()[4], high & (1 << 4) ? high | 0xF0 : high });
        if ( newTargetDistance != targetDistance )
        {
            targetDistance = newTargetDistance;
            emit targetDistanceChanged (targetDistance);
        }
    }
}

void EMapCanEmitter::getTargetNumberFromMcoLimits(CanFrame canFrame)
{
    if ( canFrame.getDescriptor () == 0x0A48 )
    {
        unsigned newTargetNumber = canFrame.getData ()[6] >> 4;
        if ( newTargetNumber != targetNumber )
        {
            targetNumber = newTargetNumber;
            if ( targetNumber < sendingObjects.size () )
            {
                emit targetNameChanged (sendingObjects[targetNumber].object->getName());
                emit targetTypeChanged (sendingObjects[targetNumber].object->getType());
            }
            else
            {
                emit targetNameChanged ("");
                emit targetTypeChanged (0);
            }
        }
    }
}

void EMapCanEmitter::getMetrometerFromIpdState(CanFrame canFrame)
{
    if ( canFrame.getDescriptor () == 0x1888 )
    {
        int32_t meters;
        meters = Complex<int32_t> ({canFrame.getData () [4],
                                    canFrame.getData () [3],
                                    canFrame.getData () [5],
                                    (canFrame.getData () [5] & (1 << 7)) ? 0xFF : 0});
        emit metrometerChanged (meters);
    }
}

void EMapCanEmitter::getLatLonFromMmLatLong(CanFrame canFrame)
{
    if ( canFrame.getDescriptor () == 0x4268 )
    {
        int iLat = (canFrame[0]) | (canFrame[1] << 8) | (canFrame[2] << 16) | (canFrame[3] << 24);
        double lat = iLat * 180.0 / ((double)1e8 * 3.14159265359);

        int iLon = (canFrame[4]) | (canFrame[5] << 8) | (canFrame[6] << 16) | ((canFrame[7] << 24) & 0x7f);
        double lat = iLat * 180.0 / ((double)1e8 * 3.14159265359);

        emit latLonChanged (lat, lon);
    }
}

void EMapCanEmitter::getIpdRestartFromSautInfo(CanFrame canFrame)
{
    if ( canFrame.getDescriptor () == 0x4668 )
    {
        bool restart = canFrame.getData ()[7] & (1 << 6);
        if ( ipdRestart == false && restart == true )
            emit metrometerReset ();
        ipdRestart = restart;
    }
}

void EMapCanEmitter::setObjectsList(const vector<EMapTarget> objects)
{
    receivedObjects = objects;
}

void EMapCanEmitter::sendOrdinate(int ordinate)
{
    can->transmitMessage (
                CanFrame (0xC0A3, // MM_COORD
                          ByteArray<3> (ordinate).msbFirst() )
                );
}

void EMapCanEmitter::setActivity(bool active)
{
    this->active = active;
}
