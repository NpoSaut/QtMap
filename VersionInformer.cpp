#include "VersionInformer.h"

VersionInformer::VersionInformer(int version, int subversion, int checksum, Can *can, QObject *parent) :
    QObject(parent),
    can (can),
    version (version),
    subversion (subversion),
    checksum (checksum)
{
    QObject::connect(can, SIGNAL(messageReceived(CanFrame)), this, SLOT(proccessCanFrame(CanFrame)));
}

void VersionInformer::proccessCanFrame(CanFrame frame)
{
    if (frame.getDescriptor() == 0x8D07) {
        if (frame[1] == 2 && frame[2] == 5) { // module: MM, operation: GET_VERSION
            CanFrame aux (0xC208);
            aux[1] = 0;
            aux[2] = version;
            aux[3] = subversion;
            aux[4] = 0;
            aux[5] = checksum;
            aux[6] = 0;
            aux[7] = 0;
            aux[8] = 0;
            can->transmitMessage(aux);
        }
    }
}

