#if defined WITH_CAN

#ifndef EMAPCANEMITTER_H
#define EMAPCANEMITTER_H

#include <vector>
#include <utility>
#include <linux/can.h>

#include <QObject>
#include <QTimer>
#include <QMutex>

#include "packing.h"
#include "emap/railobject.h"
#include "emap/emaptarget.h"


class EMapCanEmitter : public QObject
{
    Q_OBJECT
public:
    explicit EMapCanEmitter(QObject *parent = 0);
    
    PACKED(
    struct CanMessageData
    {
        unsigned int errors                     :8;
        unsigned int number                     :4;
        unsigned int type                       :4;
        unsigned int xHight                     :8;
        unsigned int xLow                       :8;
        unsigned int lengthHighByte             :4;
        unsigned int                            :1;
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

    static CanMessageData encodeEMapTarget(const EMapTarget &t, int targetNumber = 0);
signals:
    void sendNextObjectToCan(can_frame frame);
    
public slots:
    void setObjectsList (const std::vector<EMapTarget> &objects);

private:
    QTimer timer;
    QMutex mutex;
    std::vector<EMapTarget> sendingObjects, receivedObjects;
    int step;


private slots:
    void engine ();
};

#endif // EMAPCANEMITTER_H

#endif
