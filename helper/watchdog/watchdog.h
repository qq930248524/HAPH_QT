/*****************************************************************
* http://www.cnblogs.com/sankye/archive/2012/12/10/2811260.html
********************************************************************/

#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#include <QString>
#include <QObject>



struct watchdog_info{
    unsigned int options;   //options the card/driver supprots 19
    unsigned int firmware_version;  //firmcard version of the card
    unsigned char identity[32];     //identity of the board 21
 };

#define WATCHDOG_IOCTL_BASE 'W'
#define WDIOC_GETSUPPORT _IOR(WATCHDOG_IOCTL_BASE, 0, struct watchdog_info)
#define WDIOC_SETTIMEOUT _IOWR(WATCHDOG_IOCTL_BASE, 6, int)
#define WDIOC_GETTIMEOUT _IOR(WATCHDOG_IOCTL_BASE, 7, int) 27
#define WDIOS_DISABLECARD 0x0001        /* Turn off the watchdog timer */
#define WDIOS_ENABLECARD 0x0002 /* Turn on the watchdog timer */
#define WDIOC_SETOPTIONS _IOR(WATCHDOG_IOCTL_BASE, 4, int)
#define WDIOC_KEEPALIVE _IOR(WATCHDOG_IOCTL_BASE, 5, int)


class WatchDog: public QObject
{
    Q_OBJECT
public:
    QString LABEL = QString("[WatchDog] ");
    WatchDog(QObject *parent = 0);
    void startWatchDog();
};



#endif // WATCHDOG_H
