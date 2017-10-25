#include "watchdog.h"
#include <qdebug.h>

WatchDog::WatchDog(QObject *parent) : QObject(parent)
{}

//放狗
void WatchDog::startWatchDog()
{
    pid_t dogPid = fork();
    if(dogPid < 0){
        qCritical("[WatchDog] start watchdog [faild].");
        exit(-1);
    }else if(dogPid == 0){//children
        int fd = open("/dev/watchdog",O_RDWR);
        if(fd < 0){
            qCritical("[WatchDog] can not find dog at \"/dev/watchdog\"");
            exit(-1);
        }

        int timeout = 30;
        struct watchdog_info wi;
        ioctl(fd, WDIOC_GETSUPPORT, &wi);
        ioctl(fd, WDIOC_SETTIMEOUT, &timeout);
        qDebug() << LABEL + "set timeout: " << timeout;
        int i=WDIOS_ENABLECARD;
        ioctl(fd,WDIOC_SETOPTIONS,&i);
        qDebug() << LABEL + "The dog was let out! ";

        while(1){
            sleep(20);
            ioctl(fd,WDIOC_KEEPALIVE,NULL);//喂狗
        }

    }
}
