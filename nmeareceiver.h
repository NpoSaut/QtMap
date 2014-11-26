#ifndef NMEARECEIVER_H
#define NMEARECEIVER_H

#include <QObject>
#include <QDateTime>
#include <QtSerialPort/QSerialPort>

class NmeaReceiver : public QObject
{
    Q_OBJECT
public:
    explicit NmeaReceiver(QSerialPort *port, QObject *parent = 0);

signals:
    void geoPositionChanged (double lat, double lon);
    void dateTimeChanged (QDateTime dt);
    void speedChanged (double kmh);
    void validChanged (bool valid);

public slots:

private slots:
    void readLine ();

private:
    double _lat, _lon;
    QDateTime _dt;
    double _speed;
    bool _valid;

    bool isNmea (QString message);
    void decodeRmc (QString message);

    QSerialPort *port;
};

#endif // NMEARECEIVER_H
