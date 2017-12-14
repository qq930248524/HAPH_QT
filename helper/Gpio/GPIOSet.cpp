#include "GPIOSet.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/epoll.h>

#include <QFileInfo>
#include <QDebug>

GPIOset::GPIOset()
{
    initUart();
}

bool GPIOset::initUart()
{
#ifndef ARM
    return false;
#endif

    int ret = 0;
    ret += gpio_export(UART2);
    ret += gpio_set_dir(UART2, "out");
    ret += gpio_export(UART4);
    ret += gpio_set_dir(UART4, "out");
    ret += gpio_export(Door);
    ret += gpio_set_edge(Door, "both");
    ret += gpio_export(AC_DC);
    ret += gpio_set_edge(AC_DC, "both");
    ret += gpio_export(Beeper);
    ret += gpio_set_dir(Beeper, "out");

    if(ret != 0){
        qCritical() << LABEL + "gpio初始化错误 ! ";
        return false;
    }
    gpio_set_value(UART2, UART_READ);
    gpio_set_value(UART4, UART_READ);

    return true;

}


void GPIOset::run()
{
    enum {DOOR=0, ACDC};
    const int dfsSize = 2;
    struct pollfd fds[dfsSize];
    fds[DOOR].fd = gpio_fd_open(Door, O_RDONLY);
    fds[DOOR].events = POLLPRI;
    fds[ACDC].fd = gpio_fd_open(AC_DC, O_RDONLY);
    fds[ACDC].events = POLLPRI;

    while(1){
        qDebug() << LABEL + "start poll!=============================";
        if(poll(fds, dfsSize, -1) == -1){
            qCritical()<<LABEL + "poll error! ";
            return;
        }
        char result = 0x00;
        for(int i = 0; i < dfsSize; i++){
            if(fds[i].revents & POLLPRI){
                 if(lseek(fds[i].fd, 0, SEEK_SET) == -1){}
                 if(read(fds[i].fd, &result, 1) == -1){}
                 switch (i) {
                 case DOOR:
                     isDoor = (result == '1');
                     qDebug()<<LABEL + " --------------- Door is " + result;
                     emit door(isDoor);
                     break;
                 case ACDC:
                     isDcAc = (result == '1');
                     qDebug()<<LABEL + "DC_AC is " + result;
                     emit dcac(isDcAc);
                     break;
                 default:
                     break;
                 }
            }
        }
        if(isDoor || isDcAc){
            gpio_set_value(Beeper, 1);
            qDebug() << LABEL + "[Beaper] ========= beaper = 1 ";
        }else{
            gpio_set_value(Beeper, 0);
            qDebug() << LABEL + "[Beaper] ========= beaper = 0 ";
        }

        if(isDcAc){//掉电后，beep 10s
            sleep(10);
            gpio_set_value(Beeper, 0);
        }
    }
}

/* gpio export */
int GPIOset::gpio_export(unsigned int gpio)
{
    int fd, len;
    char buf[MAX_BUF];

    QFileInfo fileInfo(QString("/sys/class/gpio/export/gpio%1").arg(gpio));
    if(fileInfo.isDir()){
        return 0;
    }

    fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
    if (fd < 0) {
        //printf ("\nFailed export GPIO-%d\n", gpio);
        return fd;
    }

    len = snprintf(buf, sizeof(buf), "%d", gpio);
    write(fd, buf, len);
    close(fd);
    //printf ("\nSucessfully export GPIO-%d\n", gpio);
    return 0;
}
/* gpio unexport */
int GPIOset::gpio_unexport(unsigned int gpio)
{
    int fd, len;
    char buf[MAX_BUF];

    fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
    if (fd < 0) {
        //printf ("\nFailed unexport GPIO-%d\n", gpio);
        return fd;
    }

    len = snprintf(buf, sizeof(buf), "%d", gpio);
    write(fd, buf, len);
    close(fd);
    //printf ("\nSucessfully unexport GPIO-%d\n", gpio);
    return 0;
}
/* gpio set dir */
int GPIOset::gpio_set_dir(unsigned int gpio, const char *dir)
{
    int fd, len;
    char buf[MAX_BUF];

    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/direction", gpio);
    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        //printf ("\nFailed set GPIO-%d direction\n", gpio);
        return fd;
    }

    write(fd, dir, strlen(dir)+1);
    close(fd);
    //printf ("\nSucessfully set GPIO-%d direction\n", gpio);
    return 0;
}
/* gpio set value */
int GPIOset::gpio_set_value(unsigned int gpio, unsigned int value)
{
    int fd, len;
    char buf[MAX_BUF];

    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        //printf ("\nFailed set GPIO-%d value\n", gpio);
        return fd;
    }

    if (value!=0)
        {
        int i = write(fd, "1", 2);
        //printf ("\nGPIO-%d value set high\n", gpio);
        }
    else
        {
        write(fd, "0", 2);
        //printf ("\nGPIO-%d value set low\n", gpio);
        }

    close(fd);
    //printf ("\nSucessfully set GPIO-%d value\n", gpio);
    return 0;
}
/* gpio get value */
int GPIOset::gpio_get_value(unsigned int gpio, unsigned int *value)
{
    int fd, len;
    char buf[MAX_BUF];
    char ch;

    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

    fd = open(buf, O_RDONLY);
    if (fd < 0) {
        //printf ("\nFailed get GPIO-%d value\n", gpio);
        return fd;
    }

    read(fd, &ch, 1);

    if (ch != '0') {
        *value = 1;
    } else {
        *value = 0;
    }

    close(fd);
    //printf ("\nSucessfully get GPIO-%d value\n", gpio);
    return 0;
}
/* gpio set edge */
int GPIOset::gpio_set_edge(unsigned int gpio, const char *edge)
{
    int fd, len;
    char buf[MAX_BUF];

    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/edge", gpio);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        //printf ("\nFailed set GPIO-%d edge\n", gpio);
        return fd;
    }

    write(fd, edge, strlen(edge) + 1);
    close(fd);
    return 0;
}
/* gpio fd open */
int GPIOset::gpio_fd_open(unsigned int gpio, unsigned int dir)
{
    int fd, len;
    char buf[MAX_BUF];

    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

    fd = open(buf, dir | O_NONBLOCK );
    if (fd < 0) {
        qCritical() << LABEL + "open gpio export error! ";
        return -1;
    }
    return fd;
}
/* gpio fd close */
int GPIOset::gpio_fd_close(int fd)
{
    return close(fd);
}

// Callback called when SIGINT is sent to the process (Ctrl-C)
/*void GPIOset::signal_handler(int sig)
{
    printf( "Ctrl-C pressed, cleaning up and exiting..\n" );
    keepgoing = 0;
}*/

