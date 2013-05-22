#include "kilometerpost.h"

#define MATH_PI 3.1415926535897932384626433832795

// Представление километрового столба
KilometerPost::KilometerPost()
{
}

// Структура, описывающая формат хранение электронного столба
struct GreenPost
{
    unsigned int ordinate            :8*3;
    unsigned int position            :2;
    unsigned int alsnFreq            :4;
    unsigned int                     :1;
    unsigned int ordinateDirection   :1;
    unsigned int latitudeCode        :8*4;
    unsigned int longitudeCode       :8*4;
    unsigned int trackAddress        :8*3;
};
const int GreenPostSize = 15;

KilometerPost KilometerPost::loadFrom(const QByteArray& data, int offset, int index)
{
    GreenPost gpost;
    for (int i = 0; i < GreenPostSize; i ++)
    {
        *((char *) &gpost + i) = *(data.data () + offset + GreenPostSize * index + i);
    }

    KilometerPost kp;
    kp.ordinate = gpost.ordinate;
    kp.lat = gpost.latitudeCode  * 10e-9 * 180 / MATH_PI;
    kp.lon = gpost.longitudeCode * 10e-9 * 180 / MATH_PI;
    *((int *) &kp.position) = gpost.position;
    kp.position = kp.position;

    return kp;
}
