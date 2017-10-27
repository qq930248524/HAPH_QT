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

GPIOset::GPIOset(QObject *parent)
{

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
        //perror("gpio/fd_open");
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


void GPIOset::run1()
{/*
    int ret = 0;
    ret += gpio_export(Door);
    ret += gpio_set_edge(Door, "both");
    ret += gpio_export(AC_DC);
    ret += gpio_set_edge(AC_DC, "both");
    ret += gpio_export(Beeper);
    ret += gpio_set_dir(Beeper, "out");
    if(ret != 0){
        qCritical() << LABEL + "set gpio export or dir error! ";
        return;
    }

    const int dfsSize = 2;
    struct pollfd fds[dfsSize];
    dfs[0].fd = gpio_fd_open(Door, O_RDONLY);
    dfs[0].events = POLLIN;
    dfs[1].fd = gpio_fd_open(AC_DC, O_RDONLY);
    dfs[1].events = POLLIN;

    while(1){
        ret = poll(fds, dfsSize, -1);
        if(ret = -1){
            qCritical()<<LABEL + "poll error! ";
            exit(-1);
        }
        char result = 0x00;
        for(int i = 0; i < dfsSize; i++){
            if(dfs[i].reevents & POLLIN){
                 if(lseek(dfs[1].fd, 0, SEEK_SET) == -1){}
                 if(read(dfs[i].fd, &ch, 1) == -1){}
                 switch (i) {
                 case 0:
                     gpio_get_value(Door, &result);
                     isDoor = (result == '1');
                     break;
                 case 1:
                     gpio_get_value(AC_DC, &result);
                     isDcAc = (result == '1');
                     break;
                 default:
                     break;
                 }
            }
        }
        if(isDoor || isDcAc){
            gpio_set_value(Beeper, 1);
        }else{
            gpio_set_value(Beeper, 0);
        }
    }*/

}
