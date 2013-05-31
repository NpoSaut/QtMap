#ifndef RAIL_H
#define RAIL_H

#include <vector>
using namespace std;

#include <QByteArray>

#include "almanac.h"
#include "railobject.h"

namespace Navigation
{

class Rail
{
public:
    Rail ()
        : kilometerPost (nullptr), objects(), number(0), existence (false)
    {}

    static Rail *loadFrom (QByteArray rawData, int offset, int index);

    void setKilometerPost (KilometerPost *kilometerPost) { Rail::kilometerPost = kilometerPost; }
    KilometerPost *getKilometerPost () const { return kilometerPost; }

    KilometerIncreaseDirectoin direction;

    const vector<RailObject *> &getObjects () const { return objects; }
    unsigned int getNumber () const { return number; }
    bool isExist () const { return existence; }
    static KilometerIncreaseDirectoin getDirectoin(int railNumber);

private:
    KilometerPost *kilometerPost;
    vector<RailObject *> objects;
    unsigned int number;
    bool existence;

    PACKED(
    struct RawRailData
    {
        char number                         :8      ;
        char childObjectsCount              :8      ;
        unsigned int childObjectsAddress    :3*8    ;
    });
    static const int RawRailDataSize = 5;
};

}

#endif // RAIL_H
