#ifndef HELPER_H
#define HELPER_H

#include <QTimer>
#include <QObject>
#include <QVector>
#include <QDateTime>
#include <QSerialPort>

#include "helper/Mqtt/mqttoperator.h"
#include "helper/Gpio/GPIOSet.h"
#include "helper/JsonConfig/dasConfig.h"
#include "helper/Serial/deviceoperator.h"


#define CHANNELSIZE 12

class Helper : public QObject
{
    Q_OBJECT
public:
    explicit Helper(QObject *parent = 0);

    DasConfig *dasConfig;
    QString LABEL = QString("[Helper] ");
    QVector<DataGatherConfiguration>   equArray;
    DeviceOperator  *setting_deviceOperator = NULL;

    DeviceOperator  *deviceOperator = NULL;
    MqttOperator    *mqttOperator   = NULL;

    bool checkSerial();
    bool checkMqtt();

    bool initMqtt();
    bool initSerial();
    bool initGPIO();

    /*************** UI ****************/
    int32_t modeSize = -1;
    int *dasDataBuf = NULL;
    QTimer   *dasTimer = NULL;
    void getModeData(int modeNum, int32_t *pData);
    void stopRun();
    void gotoRun();    

signals:

public slots:
    void onGetAllpRef_ret(int32_t *);
    void onGetAllpRef_time();
};

#endif // HELPER_H
