#include "nmeareceiver.h"

#include <QString>


NmeaReceiver::NmeaReceiver(QSerialPort *port, QObject *parent) :
    port(port), QObject(parent),
    _lat (-1000), _lon (-1000), _dt(), _speed (-1), _valid (false)
{
    connect(port, SIGNAL(readyRead()), this, SLOT(readPort()));
    port->open(QIODevice::ReadOnly);
}

void NmeaReceiver::readLine()
{
    while (port->canReadLine())
    {
        QString nmeaMessage = QString(port.readLine());
        decodeRmc(nmeaMessage);
    }
}

void NmeaReceiver::readPort()
{
    while (port.canReadLine())
    {
        gps_data gpsData;

        QString nmeaMessage = QString(port.readLine());

        if ( nmea::decode_nmea_message(nmeaMessage, &gpsData) )
        {
            QDateTime dt (QDate(gpsData.year, gpsData.month, gpsData.day), QTime(gpsData.hours, gpsData.minutes, gpsData.seconds));
            dt.addSecs(60*60*3);

            QString time = QString("%1:%2:%3").arg(dt.time().hour(), 2, 10, QChar('0')).arg(dt.time().minute(), 2, 10, QChar('0')).arg(dt.time().second(), 2, 10, QChar('0'));
            emit signal_time(time);

            QString monthString[13] = {"n/a", "января", "февраля", "марта", "апреля", "мая", "июня", "июля", "августа", "сентября", "октября", "ноября", "декабря"};
            int month = dt.date().month();
            month  = ( month >= 1 && month <= 12 ) ? month : 0;
            QString date = QString("%1 %2 %3").arg(dt.date().day(), 2, 10, QChar('0')).arg(monthString[month]).arg(dt.date().year(), 2, 10, QChar('0'));
            emit signal_date(date);

            emit signal_lat(gpsData.lat);
            emit signal_lon(gpsData.lon);
            emit signal_lat_lon (gpsData.lat, gpsData.lon);

            static double speed_old = 0;

            if (gpsData.speed < 1.2)
                gpsData.speed = 0;

            if (speed_old != -1000 && abs(speed_old - gpsData.speed) > 5)
            {
                gpsData.speed = speed_old;
                speed_old = -1000;
            }
            else
                speed_old = gpsData.speed;

            emit signal_speed_sky(gpsData.is_reliable ? gpsData.speed : -1);

            if ( c_is_on_road == 1 &&
                    pgd.lat != 0 )
            {
                total_passed_distance += DistanceBetweenCoordinates(gpsData.lat, gpsData.lon, pgd.lat, pgd.lon);

                if ( (total_passed_distance - stored_passed_distance) >= 100 )
                {
                    if( distance_store_file.open(QIODevice::ReadWrite) )
                    {
                        QTextStream distance_store_stream (&distance_store_file);
                        distance_store_stream << int(total_passed_distance) << endl;
                        distance_store_stream.flush();
                        distance_store_file.close();
                        stored_passed_distance = total_passed_distance;
                    }
                    else
                    {
                        qDebug() << "Error open milage.txt!" << endl;
                    }
                }

                emit signal_passed_distance(total_passed_distance);
            }

            pgd = gpsData;

            // Отправка в CAN

            wframe_mmaltlon = can_encoder::encode_mm_alt_long(gpsData.lat, gpsData.lon, (bool)gpsData.is_reliable);
            wframe_ipddate = can_encoder::encode_ipd_date(dt.date().year(), dt.date().month(), dt.date().day(), dt.time().hour(), dt.time().minute(), dt.time().second());
            wframe_mmdata = can_encoder::encode_mm_data(qRound(gpsData.speed), total_passed_distance);

            write_canmsg_async(write_socket_0, wframe_mmaltlon);
            write_canmsg_async(write_socket_1, wframe_mmaltlon);
            write_canmsg_async(write_socket_0, wframe_ipddate);
            write_canmsg_async(write_socket_1, wframe_ipddate);
            write_canmsg_async(write_socket_0, wframe_mmdata);
            write_canmsg_async(write_socket_1, wframe_mmdata);
        }
    }
}

//    QList<QSerialPortInfo> spinfo = QSerialPortInfo::availablePorts();
//    if (spinfo.count() == 0)
//    {
//        fprintf(stderr, "Не найдено ни одного последовательного порта\n"); fflush(stderr);
//        // Для обхода бага библиотеки QtSerialPort под arm обходим эту проверку
//        //return 0;
//    }
//    port.setPort(spinfo.at(0));

//    port.setPortName("/dev/ttySAC1");
//    port.setDataBits(QSerialPort::Data8);
//    port.setBaudRate(QSerialPort::Baud115200);
//    port.setParity(QSerialPort::NoParity);
//    port.setStopBits(QSerialPort::OneStop);
//    port.setFlowControl(QSerialPort::NoFlowControl);


void NmeaReceiver::decodeRmc(QString message)
{
    /* Формат сообщения RMC
     $GPRMC,024607.000,V,5651.27857,N,06035.91777,E,0.0,0.0,241212,,,N*70
    -$GхRMC,HHMMSS.SS,A,BBBB.BBBB,a,LLLLL.LLLL,a,v.v,z.z,DDMMYY,x.x,a,b* hh
     0|  $GNRMC,
     1|  HHMMSS.SSSS,
     2|  A,
     3|  BBBB.BBBB,
     4|  a,
     5|  LLLLL.LLLL,
     6|  a,
     7|  v.v,
     8|  z.z,
     9|  DDMMYY,
    10|  x.x,
    11|  a,
    12|  b* hh<CR><LF> */

    if (message.mid(3,3) == "RMC")
    {
        QStringList fields = message.split(",");

        // Geo Position
        double latd = fields.at(3).mid(0,2).toDouble();
        double latmmm = fields.at(3).mid(2,-1).toDouble();
        double lat = latd + latmmm/60;
        if (fields.at(4) == "S")
            lat = -lat;

        double lond = fields.at(5).mid(0,3).toDouble();
        double lonmmm = fields.at(5).mid(3,-1).toDouble();
        double lon = lond + lonmmm/60;
        if (fields.at(6) == "W")
            lon = -lon;

        if (_lat != lat || _lon != lon)
        {
            _lat = lat;
            _lon = lon;
            emit geoPositionChanged(_lat, _lon);
        }

        // Datetime
        int dth = fields.at(1).mid(0,2).toInt();
        int dtm = fields.at(1).mid(2,2).toInt();
        int dts = fields.at(1).mid(4,2).toInt();
        int dtss = fields.at(1).mid(7,-1).toInt();
        int dtd = fields.at(9).mid(0,2).toInt();
        int dtmn = fields.at(9).mid(2,2).toInt();
        int dty = ("20" + fields.at(9).mid(4,2)).toInt();
        QDateTime dt (QDate(dty, dtmn, dtd), QTime(dth, dtm, dts, dtss), Qt::UTC);

        if (_dt != dt)
        {
            _dt = dt;
            emit dateTimeChanged(_dt);
        }

        // Speed
        //QStringList v_list = fields.at(7).split("."); // И целая, и дробная части могут быть переменной длины.
        //double speed_kmh = ( v_list.at(0).toDouble() + v_list.at(1).toDouble() ) * 1.852;
        double speed_kmh = fields.at(7).toDouble() * 1.852;

        if (_speed != speed_kmh)
        {
            _speed = speed_kmh;
            emit speedChanged(_speed);
        }

        // Reliability
        bool valid = fields.at(2).toLower().contains("a");

        if (_valid != valid)
        {
            _valid = valid;
            emit validChanged(_valid);
        }
    }
}
