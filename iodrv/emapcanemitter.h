#ifndef EMAPCANEMITTER_H
#define EMAPCANEMITTER_H

#include <vector>
#include <utility>

#include <QObject>
#include <QTimer>
#include <QMutex>

#include "cDoodahLib/packing.h"
#include "emap/railobject.h"
#include "emap/emaptarget.h"
#include "qtCanLib/can.h"


class EMapCanEmitter : public QObject
{
    Q_OBJECT
public:
    explicit EMapCanEmitter(Can *can, QObject *parent = 0);
    
    PACKED(
    struct CanMessageData
    {
        unsigned int errors                     :8;
        unsigned int number                     :4;
        unsigned int type                       :4;
        unsigned int xHight                     :8;
        unsigned int xLow                       :8;
        unsigned int lengthHighByte             :5;
        unsigned int radioChanel                :1;
        unsigned int station                    :1;
        unsigned int alsEn                      :1;
        unsigned int lengthLowByte              :8;
        unsigned int alsnFreqCode               :2;
        unsigned int targetSpeedHightBit        :1;
        unsigned int                            :1;
        unsigned int alsEnNewTable              :1;
        unsigned int pullforthForFreightTrain   :1;
        unsigned int pullforthForPassengerTrain :1;
        unsigned int conditionallyAllow         :1;
        unsigned int targetSpeed                :8;
    });

signals:
    void targetDistanceChanged (int distance);
    void targetNameChanged (QString name);
    void targetTypeChanged (int type);
    void metrometerChanged (int meters);
    void metrometerReset ();
    viod latLonChanged(double lat, double lon);

public slots:
    void setObjectsList (const std::vector<EMapTarget> objects);
    void sendOrdinate (int ordinate);
    void setActivity (bool active);

private:
    CanMessageData encodeEMapTarget(const EMapTarget &t, int targetNumber = 0);

    Can *can;
    QTimer timer;
    QMutex mutex;
    std::vector<EMapTarget> sendingObjects, receivedObjects;
    unsigned step;
    unsigned targetNumber;
    unsigned targetDistance;
    bool active;
    bool ipdRestart;

private slots:
    void engine ();
    void getTargetDistanceFromMcoState (CanFrame canFrame);
    void getTargetNumberFromMcoLimits (CanFrame canFrame);
    void getMetrometerFromIpdState (CanFrame canFrame);
    void getIpdRestartFromSautInfo (CanFrame canFrame);
    void getLatLonFromMmLatLong (CanFrame canFrame);
};

#endif // EMAPCANEMITTER_H
