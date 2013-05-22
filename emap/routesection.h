#ifndef ROUTESECTION_H
#define ROUTESECTION_H

#include "kilometerpost.h"
#include "vector"

class RouteSection
{
public:
    RouteSection();
    std::vector<KilometerPost> Posts;
};

#endif // ROUTESECTION_H
