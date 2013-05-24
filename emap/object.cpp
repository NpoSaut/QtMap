#include "object.h"

using namespace Navigation;

Object Object::loadRawData(QByteArray rawData, int offset, int index)
{
    RawObjectData &data = *((RawObjectData*) (rawData.data () + offset + index * rawObjectDataSize));

    Object obj;

    obj.zdOrdinate = data.zdOrdinate;

    switch (data.type) {
    case Flashlight:
        obj.length = data.length;
        obj.pullupForFreightTrain = data.pullupForFreightTrain;
        obj.alsEn = data.alsEn;
        obj.pullupForPassengerTrain = data.pullupForPassengerTrain;
        obj.conditionallyAllowForFreightTrain = data.radioOrConditionallyAllow;
        obj.speedLimit = data.speedLimit;
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
