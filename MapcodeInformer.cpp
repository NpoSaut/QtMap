#include "MapcodeInformer.h"

MapcodeInformer::MapcodeInformer(Navigation::ElectroincMap *elMap, Can *can, QObject *parent) :
    QObject (parent),
    elMap (elMap),
    can (can)
{
    QObject::connect(can, SIGNAL(messageReceived(CanFrame)), this, SLOT(proccessCanFrame(CanFrame)));
}

void MapcodeInformer::proccessCanFrame(CanFrame frame)
{
    if (frame.getDescriptor() == 0x8D07) {
        if (frame[1] == 2 && frame[2] == 10) { // module: MM, operation: GET_MAPCODE
            CanFrame aux (0xC208);
            int v = elMap->getMapNumber();
            if (v == -1)
                v = 0x65536;
            aux[1] = 209;
            aux[7] = v >> 8;
            aux[8] = v;
            can->transmitMessage(aux);
        }
    }
}

