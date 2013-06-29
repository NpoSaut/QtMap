#ifndef ROUTESECTION_H
#define ROUTESECTION_H

#include <vector>

#include "almanac.h"
#include "kilometerpost.h"

namespace Navigation
{

class RouteSection
{
public:
    RouteSection(int id);
    std::vector<KilometerPost *> posts;

    inline bool operator == (const RouteSection &s)    { return id == s.id; }
    inline bool operator != (const RouteSection &s)    { return !(*this == s); }

    int id;
};

}

#endif // ROUTESECTION_H
