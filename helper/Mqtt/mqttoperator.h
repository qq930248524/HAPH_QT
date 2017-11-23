/*******************************************
 * https://github.com/emqtt/qmqtt-client
 * *********************************************/


#ifndef MQTTOPERATOR_H
#define MQTTOPERATOR_H

#include "qmqtt.h"
#include "helper/JsonConfig/dasConfig.h"
//#include "helper/Data/dataoperator.h"

#include <QObject>

class MqttOperator : public QObject
{
    Q_OBJECT
public:
    enum MsgType {GeneralData=1, DoorOn, DoorOff, PowerOn, PowerOff, SensorOn, SensorOff};
    explicit MqttOperator(QObject *parent = 0);
    MqttOperator(QObject *parent = 0, QMQTT::Client *client = NULL,  DasData *dasData = NULL);

    bool isOnline = false;

    QMQTT::Client *client = NULL;
    DasData *dasData;
signals:

public slots:
    void mqttConnectted();
    void mqttDisConnectted();
    void onNeedPush(int, QString);
    bool sendData(QString );
    bool sendPower(bool isDC, QString payload);
    bool sendDoor(bool isOpen, QString payload);
    bool sendSensor(bool isOn, QString payload);
};

#endif // MQTTOPERATOR_H
