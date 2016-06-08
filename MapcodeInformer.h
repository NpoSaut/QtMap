#ifndef MAPCODEINFORMER_H
#define MAPCODEINFORMER_H

#include <QObject>

#include "qtCanLib/can.h"
#include "electroincmap.h"

class MapcodeInformer : public QObject
{
    Q_OBJECT
public:
    explicit MapcodeInformer(Navigation::ElectroincMap *elMap, Can *can, QObject *parent = 0);

private slots:
    void proccessCanFrame (CanFrame frame);

private:
    Navigation::ElectroincMap *elMap;
    Can *can;
};

#endif // MAPCODEINFORMER_H
