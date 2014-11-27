#include "gpsemitter.h"

GpsEmitter::GpsEmitter(Can *can, bool autoSend, QObject *parent) :
     can (can), QObject(parent),
     valid (false), latitude (-1), longitude (-1), speed (-1), milage (-1), dateTime (), timeZone (3)
{
    if (autoSend)
        startTimer(500);
}

void GpsEmitter::sendData()
{
    CanBlokMessage *messages [] = {
        makeMmAltLongMessage(),
        makeIpdDateMessage(),
        makeMmDataMessage()
    };

    for (CanBlokMessage *m : messages)
    {
        can->transmitMessage(m->encode());
        delete m;
    }
}

void GpsEmitter::timerEvent(QTimerEvent *)
{
    sendData();
    startTimer(500);
}

CanBlokMessage *GpsEmitter::makeMmAltLongMessage()
{
    MmAltLong *a = new MmAltLong ();
    a->setLatitude(latitude);
    a->setLongitude(longitude);
    a->setValid(valid);
    return a;
}

CanBlokMessage *GpsEmitter::makeIpdDateMessage()
{
    IpdDate *a = new IpdDate;
    a->setDateTime(dateTime.addSecs(60*60*timeZone));
    return a;
}

CanBlokMessage *GpsEmitter::makeMmDataMessage()
{
    MmData *a = new MmData;
    a->setSpeed(speed);
    a->setMilage(milage);
    return a;
}
