#ifndef HELPER_H
#define HELPER_H

#include <QTimer>
#include <QObject>
#include <QVector>
#include <QDateTime>
#include <QSerialPort>

#include "Mqtt/mqttoperator.h"
#include "Gpio/GPIOSet.h"
#include "JsonConfig/dasConfig.h"
#include "Serial/deviceoperator.h"
#include "Data/dataoperator.h"

#define CHANNELSIZE 12

class Helper : public QObject
{
    Q_OBJECT
public:
    QString LABEL = QString("[Helper] ");
    explicit Helper(QObject *parent = 0);

    QVector<DataGatherConfiguration>   equArray;

    DasConfig *dasConfig;
    DataOperator    *dataOperator   = NULL;
    MqttOperator    *mqttOperator   = NULL;
    DeviceOperator  *deviceOperator = NULL;
    DeviceOperator  *setting_deviceOperator = NULL;

    bool checkSerial();
    bool checkMqtt();

    bool initDataControl();
    bool initMqtt();
    bool initSerial();
    bool initGPIO();

    /*************** UI ****************/
    int32_t modeSize = -1;
    int *dasDataBuf     = NULL;
    int *dasDataCounter = NULL;
    QTimer   *dasTimer  = NULL;
    void getModeData(int modeNum, int32_t *pData);
    void stopRun();
    void gotoRun();    

signals:

public slots:
    void onGetAllpRef_ret(int32_t *);
    void onGetAllpRef_time();
    void sendDoor(bool);
    void sendPower(bool);
};

#endif // HELPER_H
