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
    {}

    static Rail loadFrom (QByteArray rawData, int offset, int index);

    void setKilometerPost (KilometerPost *kilometerPost) { Rail::kilometerPost = kilometerPost; }
    KilometerPost *getKilometerPost () const { return kilometerPost; }

    const vector<RailObject> &getObjects () const { return objects; }
    int getNumber () const { return number; }

private:
    KilometerPost *kilometerPost;
    vector<RailObject> objects;
    int number;

    struct RawRoadData
    {
        unsigned int number                 :8;
        unsigned int childObjectsAmount     :8;
        unsigned int childObjectsAddress    :8;
    };
};

}

#endif // RAIL_H
