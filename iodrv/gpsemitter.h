#ifndef GPSEMITTER_H
#define GPSEMITTER_H

#include <QObject>
#include <QDateTime>

#include "qtCanLib/can.h"
#include "qtBlokLib/parser.h"

class GpsEmitter : public QObject
{
    Q_OBJECT
public:
    explicit GpsEmitter(Can *can, bool autoSend = false,  QObject *parent = 0);

public slots:
    void sendData ();

    void setValid (bool v) { valid = v; }
    void setPosition (double lat, double lon) { latitude = lat; longitude = lon; }
    void setLatitude (double lat) { latitude = lat; }
    void setLongitude (double lon) { longitude = lon; }
    void setSpeed (double s) { speed = s; }
    void setMilage (int m) { milage = m; }
    void setDateTime (QDateTime utc) { dateTime = utc; }
    void setTimeZone (int tz) { timeZone = tz; }

protected:
    Can *can;

    void timerEvent(QTimerEvent *event);
    CanBlokMessage *makeMmAltLongMessage ();
    CanBlokMessage *makeIpdDateMessage ();
    CanBlokMessage *makeMmDataMessage ();

    bool valid;
    double latitude, longitude;
    double speed;
    int milage;
    QDateTime dateTime;
    int timeZone;
};

#endif // GPSEMITTER_H
