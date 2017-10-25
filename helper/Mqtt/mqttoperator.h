/*******************************************
 * https://github.com/emqtt/qmqtt-client
 * *********************************************/
#ifndef MQTTOPERATOR_H
#define MQTTOPERATOR_H
#include "qmqtt.h"
#include "helper/JsonConfig/dasConfig.h"

#include <QObject>

class MqttOperator : public QObject
{
    Q_OBJECT
public:
    explicit MqttOperator(QObject *parent = 0);
    MqttOperator(QObject *parent = 0, QMQTT::Client *client = NULL,  DasData *dasData = NULL);

    bool isOnline = false;

    QMQTT::Client *client = NULL;
    DasData *dasData;

    void sendData(QString );

signals:

public slots:
    void mqttConnectted();
    void mqttDisConnectted();
};

#endif // MQTTOPERATOR_H