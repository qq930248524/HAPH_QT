#include "mqttoperator.h"
#include "qmqtt.h"

#include <QDateTime>

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

bool MqttOperator::sendData(QString payLoad)
{
    if(client->isConnectedToHost() == false){
        qDebug() << "[MQTT] [sendData] false. data:" << payLoad;
        return false;
    }
    int qos = 1;
    QString dataTopic = QString("haph/cep/data/%1/%2")
            .arg(dasData->enterprise.Id)
            .arg(dasData->enterprise.SerialNo);

    QMQTT::Message msg(0, dataTopic, payLoad.toLatin1(), qos);
    client->publish(msg);
    qDebug() << "[MQTT] [sendData]: " << dataTopic << payLoad;
    return true;
}

bool MqttOperator::sendPower(bool isDC, QString payLoad)
{
    if(client->isConnectedToHost() == false){
        qDebug() << "[MQTT] [sendNotify] false. dc is" << isDC;
        return false;
    }

    int qos = 1;
    QString notifyTopic;
    if(isDC){
        notifyTopic = QString("haph/cep/notify/poweron/%1/%2")
                .arg(dasData->enterprise.Id)
                .arg(dasData->enterprise.SerialNo);
    }else{
        notifyTopic = QString("haph/cep/notify/poweroff/%1/%2")
                .arg(dasData->enterprise.Id)
                .arg(dasData->enterprise.SerialNo);
    }
    QMQTT::Message msg(0, notifyTopic, payLoad.toLatin1(), qos);
    if(client->publish(msg) <= 0){
        qWarning() << "[MQTT] [sendNotify] false: " << notifyTopic;
        return false;
    }
    qDebug() << "[MQTT] [sendNotify]: " << notifyTopic;
    return true;
}

bool MqttOperator::sendDoor(bool isOpen, QString payload)
{
    if(client->isConnectedToHost() == false){
        qDebug() << "[MQTT] [sendNotify] false. door is" << isOpen;
        return false;
    }

    int qos = 1;
    QString notifyTopic;
    if(isOpen){
        notifyTopic = QString("haph/cep/notify/lidopened/%1/%2")
                .arg(dasData->enterprise.Id)
                .arg(dasData->enterprise.SerialNo);
    }else{
        notifyTopic = QString("haph/cep/notify/lidclosed/%1/%2")
                .arg(dasData->enterprise.Id)
                .arg(dasData->enterprise.SerialNo);
    }

    QMQTT::Message msg(0, notifyTopic, payload.toLatin1(), qos);
    if(client->publish(msg) <= 0)
    {
        qWarning() << "[MQTT] [sendNotify] false: " << notifyTopic;
        return false;
    }
    qDebug() << "[MQTT] [sendNotify]: " << notifyTopic;
    return true;
}

bool MqttOperator::sendSensor(bool isOn, QString payload)
{
    if(client->isConnectedToHost() == false){
        qDebug() << "[MQTT] [sendNotify] false. ";
        return false;
    }

    int qos = 1;
    QString notifyTopic;
    if(isOn){
        notifyTopic = QString("haph/cep/notify/online/%1/%2")
                .arg(dasData->enterprise.Id)
                .arg(dasData->enterprise.SerialNo);
    }else{
        notifyTopic = QString("haph/cep/notify/offline/%1/%2")
                .arg(dasData->enterprise.Id)
                .arg(dasData->enterprise.SerialNo);
    }

    QMQTT::Message msg(0, notifyTopic, payload.toLatin1(), qos);
    if(client->publish(msg) <= 0)
    {
        qCritical() << "[MQTT] [sendNotify] false: " << notifyTopic;
        return false;
    }
    qDebug() << "[MQTT] [sendNotify]: " << notifyTopic;
    return true;
}
