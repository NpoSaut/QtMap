#ifndef KILOMETERPOST_H
#define KILOMETERPOST_H

#include "QByteArray"

#include "vector"
using namespace std;

enum KilometerPostPosition { kpp_start = 1, kpp_middle = 0, kpp_end = 2 };

class KilometerPost
{
public:
    KilometerPost();
    double lat;
    double lon;
    double ordinate;
    KilometerPostPosition position;
    static KilometerPost loadFrom(const QByteArray& data, int offset, int index);
};

#endif // KILOMETERPOST_H
