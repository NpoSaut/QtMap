#include "kilometerpost.h"

#include "math.h"

#define MATH_PI 3.1415926535897932384626433832795
#define EARTH_RADIUS 6372795

// Представление километрового столба
KilometerPost::KilometerPost()
{
}

double KilometerPost::distanceTo(double to_lat, double to_lon)
{
    return estimateDistances(lat, lon, to_lat, to_lon);
}

double KilometerPost::estimateDistanceTo(KilometerPost p2)
{
    return estimateDistanceTo(p2.lat, p2.lon);
}

double KilometerPost::estimateDistanceTo(double to_lat, double to_lon)
{
    return estimateDistances(lat, lon, to_lat, to_lon);
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
    kp.id = index;
    kp.ordinate = gpost.ordinate;
    kp.lat = gpost.latitudeCode  * 10e-9 * 180 / MATH_PI;
    kp.lon = gpost.longitudeCode * 10e-9 * 180 / MATH_PI;
    *((int *) &kp.position) = gpost.position;
    kp.position = kp.position;

    return kp;
}

double KilometerPost::distanceBetween(KilometerPost p1, KilometerPost p2)
{
    return EARTH_RADIUS * 2 * asin(sqrt(estimateDistances(p1, p2)));
}

double KilometerPost::estimateDistances(KilometerPost p1, KilometerPost p2)
{
    return estimateDistances(p1.lat, p1.lon, p2.lat, p2.lon);
}

double KilometerPost::estimateDistances(double lat1, double lon1, double lat2, double lon2)
{
    return pow(sin((lat2 - lat1) / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(lon2 - lon1), 2);
}

double KilometerPost::metersToEstimation(double meters)
{
    return sin(pow(meters / (2 * EARTH_RADIUS), 2));
}
