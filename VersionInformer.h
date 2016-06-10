#ifndef VERSIONINFORMER_H
#define VERSIONINFORMER_H

#include <QObject>

#include "qtCanLib/can.h"

class VersionInformer : public QObject
{
    Q_OBJECT
public:
    explicit VersionInformer(int version, int subversion, int checksum, Can *can, QObject *parent = 0);

private slots:
    void proccessCanFrame (CanFrame frame);

private:
    Can *can;
    int version;
    int subversion;
    int checksum;
};

#endif // VERSIONINFORMER_H
