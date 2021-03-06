#ifndef GPIOSET_H
#define GPIOSET_H

//cat /sys/kernel/debug/gpio


/*****************************************/
#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define POLL_TIMEOUT (3 * 1000) /* 3 seconds */
#define MAX_BUF 64
//int keepgoing = 1;

#define Beeper  117
#define Door    115
#define AC_DC   51
#define G3      60
//enabel 485 read/write
#define UART_READ   0
#define UART_WRIT   1
#define UART2   12
#define UART4   13

enum PIN_DIRECTION{
    INPUT_PIN=0,
    OUTPUT_PIN=1
};

enum PIN_VALUE{
    LOW=0,
    HIGH=1
};

#include <QThread>
#include <QObject>
#include "helper/Mqtt/mqttoperator.h"

class GPIOset : public QThread
{
    Q_OBJECT
protected:
    volatile bool stopped;
protected:
    void run();

public:
    QString LABEL = QString("[GPIO]");
    GPIOset();
    bool isDoor = false;
    bool isDcAc = false;
    bool initUart();

    QTimer *beeper;
    enum TYPE{DOOR=0, ACDC, NON};
    enum TYPE type;

    /*****************************************/
    /* gpio export */
    int gpio_export(unsigned int gpio);
    /* gpio unexport */
    int gpio_unexport(unsigned int gpio);
    /* gpio set dir */
    int gpio_set_dir(unsigned int gpio, const char *dir);
    /* gpio set value */
    static int gpio_set_value(unsigned int gpio, unsigned int value);
    /* gpio get value */
    static int gpio_get_value(unsigned int gpio, unsigned int *value);
    /* gpio set edge */
    int gpio_set_edge(unsigned int gpio, const char *edge);
    /* gpio fd open */
    int gpio_fd_open(unsigned int gpio, unsigned int dir);
    /* gpio fd close */
    int gpio_fd_close(int fd);
    /*// Callback called when SIGINT is sent to the process (Ctrl-C)
    void signal_handler(int sig);*/

public slots:
    void on_beep();
    void on_beepSwitch(bool);
signals:
    void beepSwitch(bool);
    void door(bool isOpen);
    void dcac(bool isDc);

};

#endif // GPIOSET_H
