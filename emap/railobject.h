#ifndef OBJECT_H
#define OBJECT_H

#include <QByteArray>
#include <QString>

#include "almanac.h"

namespace Navigation
{

class RailObject
{

public:
    RailObject ()
        : parentRail (nullptr), ordinate (0)
    {}

    static RailObject *loadFrom (QByteArray rawData, int offset, int index);

    void setParentRoad (Rail *parentRail) { RailObject::parentRail = parentRail; }
    Rail *getParentRail () const { return parentRail; }

    int getOrdinate () const { return ordinate; }
    int calcDistance (int currentRwOrdinate) const { return ordinate - currentRwOrdinate; }

    enum ObjectType { TrafficLight = 1, Station = 2, DangerousPlace = 3, Bridge = 4,
                     Crossing = 5, Platform = 6, Tunnel = 7,  Switch = 8,
                     Tks = 9, GpuSaut = 10, DeadEnd = 11 };

    ObjectType getType () const { return type; }
    int getAlsnFreq () const { return alsnFreq; }
    bool isAlsEn () const { return alsEn; }
    int getLength () const { return length; }
    int getSpeedRestriction () const { return speedRestriction; }
    bool isConditionallyAllowForFreightTrain () const { return conditionallyAllowForFreightTrain; }
    bool isRadioChannel () const { return radioChannel; }
    bool isPullforthForFreightTrain () const { return pullforthForFreightTrain; }
    bool isPullforthForPassengerTrain () const { return pullforthForPassengerTrain; }
    QString getName () const { return name; }

private:
    Rail *parentRail;
    int ordinate;

    ObjectType type;
    int alsnFreq;
    bool alsEn;
    int length;
    int speedRestriction;
    bool conditionallyAllowForFreightTrain;
    bool radioChannel;
    bool pullforthForFreightTrain;
    bool pullforthForPassengerTrain;
    QString name;

    struct RawObjectData
    {
        unsigned int type                       :8;     // 1
        unsigned int length                     :16;    // 2
        unsigned int                            :8;     // 3

        unsigned int                            :4;     // 4
        unsigned int pullforthForFreightTrain   :1;
        unsigned int alsEn                      :1;
        unsigned int pullforthForPassengerTrain :1;
        unsigned int radioOrConditionallyAllow  :1;

        unsigned int speedRestriction           :8;     // 5
        unsigned int alsnFreq                   :8;     // 6
        unsigned int ordinate                   :8*3;   // 7
        char         name                       [8];
    } __attribute__((packed));
    static constexpr size_t rawObjectDataSize = 20;
};

}

#endif // OBJECT_H
