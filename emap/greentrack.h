#ifndef GREENTRACK_H
#define GREENTRACK_H
#include "trackobject.h"
#include "vector"

class GreenTrack
{
public:
    GreenTrack();
    int number;
    std::vector<TrackObject> objects;
};

#endif // GREENTRACK_H
