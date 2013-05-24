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
    RouteSection();
    std::vector<KilometerPost> Posts;
};

}

#endif // ROUTESECTION_H
