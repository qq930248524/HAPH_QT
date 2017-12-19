/*******************************************
 * https://github.com/emqtt/qmqtt-client
 * *********************************************/


#ifndef MQTTOPERATOR_H
#define MQTTOPERATOR_H

#include "qmqtt.h"
#include "helper/JsonConfig/dasConfig.h"
#include <QTimer>

#include <QObject>

class MqttOperator : public QObject
{
    Q_OBJECT
public:
    enum MsgType {GeneralData=1, DoorOn, DoorOff, PowerOn, PowerOff, SensorOn, SensorOff};
    explicit MqttOperator(QObject *parent = 0);
    MqttOperator(QObject *parent = 0, QMQTT::Client *client = NULL,  DasData *dasData = NULL);

    bool isOnline = false;
    bool isOnline2 = false;
    const int aliave = 10;

    QMQTT::Client *client = NULL;
    DasData *dasData;
    QTimer *dog = NULL;
signals:

public slots:
    void onNeedPush(int, QString);
    void onPingresp();
    void checkDog();
    bool sendData(QString );
    bool sendPower(bool isDC, QString payload);
    bool sendDoor(bool isOpen, QString payload);
    bool sendSensor(bool isOn, QString payload);
};

#endif // MQTTOPERATOR_H
