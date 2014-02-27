#include "customordinatehandler.h"

CustomOrdinateHandler::CustomOrdinateHandler(Can *can, Cookies *cookies, QObject *parent) :
    QObject(parent),
    can (can),
    cookies (cookies),
    ordinateToStore (0)
{
    QObject::connect (can, SIGNAL(messageReceived(CanFrame)), this, SLOT(processCanWriteCookieMessage(CanFrame)));
    QObject::connect (&cookies->startOrdinate, SIGNAL(onChange(int)), this, SLOT(processCookieOrdinate(int)));
    QObject::connect (&cookies->ordinateIncreaseDirection, SIGNAL(onChange(int)), this, SLOT(processCookieDirection(int)));
    cookies->startOrdinate.requestValue ();
    cookies->ordinateIncreaseDirection.requestValue ();
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

void CustomOrdinateHandler::processCanWriteCookieMessage(CanFrame frame)
{
    if ( frame.getDescriptor () == 0x6205 )
    {
        if ( frame[1] == 9 )
        {
            emit ordinateChanged ( quint32(frame[2])*256*256*256
                                   + quint32(frame[3])*256*256
                                   + quint32(frame[4])*256
                                   + quint32(frame[5])
                                   );
        }
    }
}
