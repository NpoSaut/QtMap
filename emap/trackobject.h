#ifndef TRACKOBJECT_H
#define TRACKOBJECT_H

#include <QObject>
#include "vector"

class TrackObject
{
public:
    TrackObject();
    int type;
    int length;
    int ordinate;
    QString name;
};

#endif // TRACKOBJECT_H
