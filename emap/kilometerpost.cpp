#include "kilometerpost.h"

#define MATH_PI 3.1415926535897932384626433832795

// Представление километрового столба
KilometerPost::KilometerPost()
{
}

// Структура, описывающая формат хранение электронного столба
struct GreenPost
{
    int ordinate            :8*3;
    int position            :2;
    int alsnFreq            :4;
    int                     :1;
    int ordinateDirection   :1;
    int latitudeCode        :8*4;
    int longitudeCode       :8*4;
    int trackAddress        :3;
};

KilometerPost KilometerPost::loadFrom(const vector<char> &data, int offset, int index)
{
    GreenPost gpost;
    for (int i = 0; i < sizeof(GreenPost); i ++)
    {
        *((char *) &gpost + i) = data[offset + sizeof(GreenPost) * index + i];
    }

    KilometerPost kp;
    kp.ordinate = gpost.ordinate;
    kp.lat = gpost.latitudeCode  * 10e-9 * 180 / MATH_PI;
    kp.lon = gpost.longitudeCode * 10e-9 * 180 / MATH_PI;
    *((int *) &kp.position) = gpost.position;

    return kp;
}
