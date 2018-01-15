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
    int32_t timerId = -1;
    int *dasDataBuf     = NULL;// helper的数据缓存
    int *dasDataCounter = NULL;
    QTimer   *dasTimer  = NULL;
    void getModeData(int modeNum, int32_t *pData);
    void stopRun();
    void gotoRun();    

signals:

protected:
    void timerEvent(QTimerEvent *event);//定时器槽函数

public slots:
    void onGetAllpRef_ret(int32_t *);//获取串口返回的数据，更新dasDataBuf+保存+发送
    void onGetAllpRef_time();//定时器主动发送获取数据的请求
    void sendDoor(bool);
    void sendPower(bool);
};

#endif // HELPER_H
