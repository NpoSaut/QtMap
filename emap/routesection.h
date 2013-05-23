#ifndef ROUTESECTION_H
#define ROUTESECTION_H

#include "vector"
#include "kilometerpost.h"


class RouteSection
{
public:
    RouteSection(int id);
    std::vector<KilometerPost> Posts;

    inline bool operator == (const RouteSection &s)    { return id == s.id; }
    inline bool operator != (const RouteSection &s)    { return !(*this == s); }

    int id;
};

#endif // ROUTESECTION_H
