#ifndef EMAPTARGET_H
#define EMAPTARGET_H

#include "emap/almanac.h"

class EMapTarget
{
public:
    EMapTarget(Navigation::RailObject *object, int x)
        :object(object), x(x)
    { }

    Navigation::RailObject *object;
    int x;
};

#endif // EMAPTARGET_H
