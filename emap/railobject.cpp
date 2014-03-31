#include <QTextCodec>

#include "railobject.h"

using namespace Navigation;

QTextDecoder *cp1251Decoder = QTextCodec::codecForName("CP1251")->makeDecoder ();

RailObject *RailObject::loadFrom(const QByteArray &rawData, int offset, int index)
{
    RawObjectData &data = *((RawObjectData *) (rawData.data () + offset + index * rawObjectDataSize));
    RailObject *obj = new RailObject();

    obj->ordinate = data.ordinate;
    obj->type = (ObjectType) data.type;

    switch (data.type) {
    case TrafficLight:
        obj->length = data.length;
        obj->pullforthForFreightTrain = data.pullforthForFreightTrain;
        obj->alsEn = data.alsEn;
        obj->pullforthForPassengerTrain = data.pullforthForPassengerTrain;
        obj->conditionallyAllowForFreightTrain = data.radioOrConditionallyAllow;
        obj->speedRestriction = data.speedRestriction;
        obj->alsnFreq = data.alsnFreq;
        obj->name = cp1251Decoder->toUnicode (data.name, 8).trimmed ();
        break;
    case Station:
        obj->length = data.length;
        obj->alsnFreq = data.alsnFreq;
        obj->name = cp1251Decoder->toUnicode (data.name, 8).trimmed ();
        obj->speedRestriction = data.speedRestriction;
        break;
    case DangerousPlace:
        obj->length = data.length;
        obj->alsEn = data.alsEn;
        obj->alsnFreq = data.alsnFreq;
        obj->speedRestriction = data.speedRestriction;
        break;
    case Bridge:
        obj->length = data.length;
        obj->alsEn = data.alsEn;
        obj->alsnFreq = data.alsnFreq;
        obj->speedRestriction = data.speedRestriction;
        break;
    case Crossing:
        obj->alsEn = data.alsEn;
        obj->alsnFreq = data.alsnFreq;
        obj->speedRestriction = data.speedRestriction;
        break;
    case Platform:
        obj->length = data.length;
        obj->alsEn = data.alsEn;
        obj->alsnFreq = data.alsnFreq;
        obj->speedRestriction = data.speedRestriction;
        break;
    case Tunnel:
        obj->length = data.length;
        obj->alsEn = data.alsEn;
        obj->alsnFreq = data.alsnFreq;
        obj->speedRestriction = data.speedRestriction;
        break;
    case Switch:
        obj->alsEn = data.alsEn;
        obj->alsnFreq = data.alsnFreq;
        obj->name = cp1251Decoder->toUnicode (data.name, 8).trimmed ();
        obj->speedRestriction = data.speedRestriction;
        break;
    case Tks:
        obj->alsEn = data.alsEn;
        obj->alsnFreq = data.alsnFreq;
        obj->speedRestriction = data.speedRestriction;
        break;
    case GpuSaut:
        obj->alsEn = data.alsEn;
        obj->alsnFreq = data.alsnFreq;
        obj->speedRestriction = data.speedRestriction;
        break;
    case DeadEnd:
        obj->alsEn = data.alsEn;
        obj->alsnFreq = data.alsnFreq;
        obj->speedRestriction = data.speedRestriction;
        break;
    default:
        break;
    }

    return obj;
}
