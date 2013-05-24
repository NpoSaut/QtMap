#include "railobject.h"

using namespace Navigation;

RailObject RailObject::loadFrom(QByteArray rawData, int offset, int index)
{
    RawObjectData &data = *((RawObjectData*) (rawData.data () + offset + index * rawObjectDataSize));

    RailObject obj;

    obj.ordinate = data.ordinate;

    switch (data.type) {
    case TrafficLight:
        obj.length = data.length;
        obj.pullforthForFreightTrain = data.pullforthForFreightTrain;
        obj.alsEn = data.alsEn;
        obj.pullforthForPassengerTrain = data.pullforthForPassengerTrain;
        obj.conditionallyAllowForFreightTrain = data.radioOrConditionallyAllow;
        obj.speedRestriction = data.speedRestriction;
        obj.alsnFreq = data.alsnFreq;
        obj.name = QString(*(((char *) &data) + 10));
        break;
    case Station:
        obj.length = data.length;
        obj.alsnFreq = data.alsnFreq;
        obj.name = QString(*(((char *) &data) + 10));
        break;
    case DangerousPlace:
        obj.length = data.length;
        obj.alsEn = data.alsEn;
        obj.alsnFreq = data.alsnFreq;
        break;
    case Bridge:
        obj.length = data.length;
        obj.alsEn = data.alsEn;
        obj.alsnFreq = data.alsnFreq;
        break;
    case Crossing:
        obj.alsEn = data.alsEn;
        obj.alsnFreq = data.alsnFreq;
        break;
    case Platform:
        obj.length = data.length;
        obj.alsEn = data.alsEn;
        obj.alsnFreq = data.alsnFreq;
        break;
    case Tunnel:
        obj.length = data.length;
        obj.alsEn = data.alsEn;
        obj.alsnFreq = data.alsnFreq;
        break;
    case Switch:
        obj.alsEn = data.alsEn;
        obj.alsnFreq = data.alsnFreq;
        break;
    case Tks:
        obj.alsEn = data.alsEn;
        obj.alsnFreq = data.alsnFreq;
        break;
    case GpuSaut:
        obj.alsEn = data.alsEn;
        obj.alsnFreq = data.alsnFreq;
        break;
    case DeadEnd:
        obj.alsEn = data.alsEn;
        obj.alsnFreq = data.alsnFreq;
        break;
    default:
        break;
    }

    return obj;
}
