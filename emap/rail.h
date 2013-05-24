#ifndef RAIL_H
#define RAIL_H

#include <vector>
using namespace std;

#include <QByteArray>

#include "almanac.h"
#include "object.h"

namespace Navigation
{

class Rail
{
public:
    Rail ()
    {}

    static Rail loadRawData (QByteArray rawData, int offset, int index);

    void setParentKilometerPostNumber (int kilometerPostNumber) { Rail::parentKilometerPostNumber = kilometerPostNumber; }
    int getParentKilometerPost () const { return parentKilometerPostNumber; }

    const vector<Object> &getObjects () const { return objects; }
    int getNumber () const { return number; }

private:
    int parentKilometerPostNumber;
    vector<Object> objects;
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
