#include "mqttoperator.h"
#include "qmqtt.h"

MqttOperator::MqttOperator(QObject *parent) : QObject(parent)
{
}

MqttOperator::MqttOperator(QObject *parent, QMQTT::Client *client, DasData *dasData)
    :client(client),dasData(dasData)
{
    connect(client, SIGNAL(connected()),    this, SLOT(mqttConnectted()));
    connect(client, SIGNAL(disconnected()), this, SLOT(mqttDisConnectted()));
}

void MqttOperator::mqttConnectted()
{
    isOnline = true;
    qDebug("[MQTT] connect mqtt [OK].");

    if(client->isConnectedToHost() == true){
        return;
    }
}
void MqttOperator::mqttDisConnectted()
{
    isOnline = false;
    qDebug("[MQTT] connect mqtt [FAILD].");
}

void MqttOperator::sendData(QString payLoad)
{
    if(client->isConnectedToHost() == false){
        qDebug() << "[MQTT] [sendData] false. data:" << payLoad;
        return;
    }
    int qos = 1;
    QString dataTopic = QString("haph/cep/data/%1/%2")
            .arg(dasData->enterprise.Id)
            .arg(dasData->enterprise.SerialNo);

    QMQTT::Message msg(0, dataTopic, payLoad.toLatin1(), qos);
    client->publish(msg);
    qDebug() << "[MQTT] [sendData]: " << dataTopic << payLoad;
}
