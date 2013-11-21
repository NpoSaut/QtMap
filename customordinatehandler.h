#ifndef CUSTOMORDINATEHANDLER_H
#define CUSTOMORDINATEHANDLER_H

#include <QObject>

#include "qtCanLib/can.h"
#include "qtBlokLib/cookies.h"

// Обеспечивает сохранение и загрузку кастомных ординаты и направления в МПХ
class CustomOrdinateHandler : public QObject
{
    Q_OBJECT
public:
    explicit CustomOrdinateHandler(Can *can, Cookies *cookies, QObject *parent = 0);
    
signals:
    // Испускается при изменении кастомной ординаты из вне
    void ordinateChanged (int ordinate);
    // Испускается при изменении кастомного направления
    void directionChanged (int dir); // +1 - вперёд, -1 - назад
    
public slots:
    // Должна вызываться при каждом изменении текущей ординаты ЭК
    //  Если ордината существенно отличается от хранимой в МПХ,
    //  то пытается сохранить в МПХ текущее значение
    void setCurrentOrdinate (int ordinate);

protected slots:
    // Обпеспечивает проверку того, произошло ли изменение по нашей инициативе или извне
    //  При изменеии извне испускает сигнал ordinateChanged()
    void processCookieOrdinate (int ordinate);
    // Преобразует формат напрвления движения
    //  и испускает directionChanged()
    void processCookieDirection (int dir); // 1 - вперёд, 0 - назад
    // При отсутсвии записи в МПХ
    //  отрабатывать хотя бы по попытке записи
    void processCanWriteCookieMessage (CanFrame frame);

protected:
    Can *can;
    Cookies *cookies;

    int ordinateToStore;
};

#endif // CUSTOMORDINATEHANDLER_H
