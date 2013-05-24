#ifndef OBJECT_H
#define OBJECT_H

#include <QByteArray>
#include <QString>

#include "almanac.h"

namespace Navigation
{

class Object
{
public:
    Object ()
        : parentRail (nullptr), zdOrdinate (0)
    {}

    static Object loadRawData (QByteArray rawData, int offset, int index);

    void setParentRoad (Rail *parentRail) { Object::parentRail = parentRail; }
    Rail *getParentRail () const { return parentRail; }

    int getZdOrdinate () const { return zdOrdinate; }
    int calcDistance (int currentZdOrdinate) const { return zdOrdinate - currentZdOrdinate; }

    int getAlsnFreq () const { return alsnFreq; }
    bool isAlsEn () const { return alsEn; }
    int getLength () const { return length; }
    int getSpeedLimit () const { return speedLimit; }
    bool isConditionallyAllowForFreightTrain () const { return conditionallyAllowForFreightTrain; }
    bool isRadioChannel () const { return radioChannel; }
    bool isPullupForFreightTrain () const { return pullupForFreightTrain; }
    bool isPullupForPassengerTrain () const { return pullupForPassengerTrain; }
    QString getName () const { return name; }

private:
    Rail *parentRail;
    int zdOrdinate;

    int alsnFreq;
    bool alsEn;
    int length;
    int speedLimit;
    bool conditionallyAllowForFreightTrain;
    bool radioChannel;
    bool pullupForFreightTrain;
    bool pullupForPassengerTrain;
    QString name;

    enum OjectType { Flashlight = 1, Station = 2, DangerousPlace = 3, Bridge = 4,
                     Crossing = 5, Platform = 6, Tunnel = 7,  Switch = 8,
                     Tks = 9, GpuSaut = 10, DeadEnd = 11 };

    struct RawObjectData
    {
        unsigned int type                       :8;     // 1
        unsigned int length                     :16;    // 2
        unsigned int                            :8;     // 3

        unsigned int                            :4;     // 4
        unsigned int pullupForFreightTrain      :1;
        unsigned int alsEn                      :1;
        unsigned int pullupForPassengerTrain    :1;
        unsigned int radioOrConditionallyAllow  :1;

        unsigned int speedLimit                 :8;     // 5
        unsigned int alsnFreq                   :8;     // 6
        unsigned int zdOrdinate                 :8*3;   // 7
        unsigned int name                       :8*8;
        unsigned int                            :8*2;
    };
    static constexpr size_t rawObjectDataSize = 20;
};

}

#endif // OBJECT_H
