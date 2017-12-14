#include "mqttoperator.h"
#include "qmqtt.h"

#include <QDateTime>

MqttOperator::MqttOperator(QObject *parent) : QObject(parent)
{
}

MqttOperator::MqttOperator(QObject *parent, QMQTT::Client *client, DasData *dasData)
    :client(client),dasData(dasData)
{
    client->setKeepAlive(aliave);
    connect(client, SIGNAL(pingresp()), this, SLOT(onPingresp()));
}

void MqttOperator::onPingresp()
{
    if(dog == NULL){
        dog = new QTimer();
        dog->setInterval(aliave * 1000 * 2);
        connect(dog, SIGNAL(timeout()), this, SLOT(checkDog()));
        dog->start();
    }
    isOnline = true;
}

//只判断离线
void MqttOperator::checkDog()
{
    static bool first = true;

    if(isOnline){
        first = true;
    }else{
        if(first){
            client->disconnectFromHost();
        }
        first = false;
    }
    isOnline = false;
}

void MqttOperator::onNeedPush(int type, QString payload)
{
    if(client->isConnectedToHost() == false){
        qDebug() << "[MQTT] [sendData] -- false. data:" << payload;
    }
    switch (type) {
    case GeneralData:
        sendData(payload);
        break;
    case DoorOn:
        sendDoor(true, payload);
        break;
    case DoorOff:
        sendDoor(false, payload);
        break;
    case PowerOn:
        sendPower(true, payload);
        break;
    case PowerOff:
        sendPower(false, payload);
        break;
    case SensorOn:
        sendSensor(true, payload);
        break;
    case SensorOff:
        sendSensor(false, payload);
        break;
    default:
        break;
    }
}

bool MqttOperator::sendData(QString payLoad)
{
    int qos = 1;
    QString dataTopic = QString("haph/cep/data/%1/%2")
            .arg(dasData->enterprise.Id)
            .arg(dasData->enterprise.SerialNo);

    if(client->isConnectedToHost() == false ){
        isOnline = false;
        qDebug() << "[MQTT] [sendData] false. data:" << dataTopic << payLoad;
        qDebug() << "inOnline = " << isOnline;
        qDebug() << "client->isConnectedToHost() = " << client->isConnectedToHost();
        return false;
    }else{
        isOnline = true;
        QMQTT::Message msg(0, dataTopic, payLoad.toLatin1(), qos);
        int abc = client->publish(msg);
        qDebug() << "[MQTT] [sendData]: " << dataTopic << payLoad;
        return true;
    }
}

bool MqttOperator::sendPower(bool isDC, QString payLoad)
{
    if(client->isConnectedToHost() == false){
        isOnline = false;
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
        isOnline = false;
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
        isOnline = false;
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
