#include "customordinatehandler.h"

CustomOrdinateHandler::CustomOrdinateHandler(Cookies *cookies, QObject *parent) :
    QObject(parent),
    cookies (cookies),
    ordinateToStore (0)
{
    QObject::connect (&cookies->startOrdinate, SIGNAL(onChange(int)), this, SLOT(processCookieOrdinate(int)));
    QObject::connect (&cookies->ordinateIncreaseDirection, SIGNAL(onChange(int)), this, SLOT(processCookieDirection(int)));
}


void CustomOrdinateHandler::setCurrentOrdinate(int ordinate)
{
    if ( abs (cookies->startOrdinate.getValue () - ordinate) > 100 )
    {
        ordinateToStore = ordinate;
        cookies->startOrdinate.setVaule (ordinateToStore); // Не будем следить за тем, записалось ли
                                                           // Если не записалось, то в следующий раз опять сюда зайдём
    }
}

void CustomOrdinateHandler::processCookieOrdinate(int ordinate)
{
    if ( ordinate != ordinateToStore ) // Ордината записана "слева", а не подтверждение на нашу запись
    {
        emit ordinateChanged (ordinate);
    }
}

void CustomOrdinateHandler::processCookieDirection(int dir)
{
    emit directionChanged ( dir == 1 ? 1 : -1 );
}
