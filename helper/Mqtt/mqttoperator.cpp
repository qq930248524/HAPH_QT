#include "mqttoperator.h"
#include "qmqtt.h"

#include <QDateTime>
#include <QProcess>

MqttOperator::MqttOperator(QObject *parent) : QObject(parent)
{
}

MqttOperator::MqttOperator(QObject *parent, QMQTT::Client *client, DasData *dasData)
    :client(client),dasData(dasData)
{
    client->setKeepAlive(aliave);

    connect(client, SIGNAL(connected()), this, SLOT(on_connected()));
    connect(client, SIGNAL(pingresp()), this, SLOT(onPingresp()));
    connect(client, SIGNAL(received(const QMQTT::Message &)), this, SLOT(on_recvMsg(const QMQTT::Message &)));
}

void MqttOperator::on_connected()
{
    //一定要先链接，后订阅
    client->subscribe("haph/cep/query", 0);
}

/**************************************************
 * @brief:
 *      没次获取到心跳，执行一次此函数
 *      isOnline：记录MQTT模块是否在线的标志
 *      isOnline2：用来临时判断isOnline状态的标志，局部使用
 * @param：
 * @return:
 **************************************************/
void MqttOperator::onPingresp()
{
    if(dog == NULL){//初始化定时器dog，用来判断mqtt是否在线
        dog = new QTimer();
        dog->setInterval(aliave * 1000 * 6);
        connect(dog, SIGNAL(timeout()), this, SLOT(checkDog()));
        dog->start();
    }
    isOnline = true;
    isOnline2 = true;
}


/**************************************************
 * @brief:
 *      定时器dog的槽函数，用来判断一个时间段内isOnline2是否被心跳置为true
 *      如果isOnline2被置为true，则isOnline=true
 *      如果isOnline2没有置true，则isOnline=false
 * @param：
 * @return:
 **************************************************/
void MqttOperator::checkDog()
{
    static bool first = true;
    if(isOnline2){
        first = true;
        isOnline = true;
        isOnline2 = false;
    }else{
        if(first){
            client->disconnectFromHost();
            first = false;
        }
        isOnline = false;
    }
}


/**************************************************
 * @brief:  用来响应数据（Data）操作needPush的槽函数。
 *          当mqtt链接时，data模块的续传功能（子线程）启动，检测到有需要续传的数据后，会发送needPush信号
 * @param：type：消息的类型，payload：负载
 * @return: void
 **************************************************/
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

/**************************************************
 * @brief:  发送数据
 * @param：payload是负载
 * @return: true=发送成功，false=发送失败
 **************************************************/
bool MqttOperator::sendData(QString payLoad)
{
    int qos = 1;
    QString dataTopic = QString("haph/cep/data/%1/%2")
            .arg(dasData->enterprise.Id)
            .arg(dasData->enterprise.SerialNo);

    if(isOnline == false){
        qDebug() << "[MQTT] [sendData] false. data:" << dataTopic << payLoad;
        //        qDebug() << "inOnline = " << isOnline;
        //        qDebug() << "client->isConnectedToHost() = " << client->isConnectedToHost();
        return false;
    }else{
        QMQTT::Message msg(0, dataTopic, payLoad.toLatin1(), qos);
        int abc = client->publish(msg);
        qDebug() << "[MQTT] [sendData]: " << dataTopic << payLoad;
        return true;
    }
}


/**************************************************
 * @brief:  发送电源状态
 * @param：isDC是否为直流，payload是负载
 * @return: true=发送成功，false=发送失败
 **************************************************/
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
        notifyTopic = QString("haph/cep/notify/poweroff/%1/%2")
                .arg(dasData->enterprise.Id)
                .arg(dasData->enterprise.SerialNo);
    }else{
        notifyTopic = QString("haph/cep/notify/poweron/%1/%2")
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

/**************************************************
 * @brief:  发送门的状态
 * @param：  isOpen：开或关， payload：负载
 * @return: true=发送成功，  false=发送失败
 **************************************************/
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

/**************************************************
 * @brief:  发送传感器的状态
 * @param：  isOn：开或关， payload：负载
 * @return: true=发送成功，  false=发送失败
 **************************************************/
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

/**************************************************
 * @brief:  订阅mqtt
 * @param：
 * @return:
 **************************************************/
void MqttOperator::on_recvMsg(const QMQTT::Message &msg)
{
    qDebug()<<"========================================= recv";
    QString top = msg.topic();
    QByteArray payload = msg.payload();
    QString pay = payload;
    qDebug()<< "[SUB]" << "top=" << top << " pay=" << pay;

    QProcess pro(this);
    if(pay == "update"){
        qDebug() << "[MQTT] received update cmd!";
        pro.startDetached("/home/HAPH/cmd/update.sh");
    }
    if(pay == "reboot"){
        qDebug() << "[MQTT] received reboot cmd!";
        pro.startDetached("/home/HAPH/cmd/reboot.sh");
    }
    qDebug() << "[MQTT] received cmd:" << pay;
    pro.startDetached(pay);
}
