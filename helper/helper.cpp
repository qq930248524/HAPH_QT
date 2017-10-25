#include "helper.h"

Helper::Helper(QObject *parent) : QObject(parent)
{
    dasConfig = new DasConfig();
    if(dasConfig->init("/etc/dasconfig.json") == false){
        qWarning("detail json error!");
    }
    initSerial();
    initMqtt();
}

bool Helper::initSerial()
{    
    DasData dasData = dasConfig->dasData;
    if(dasData.comName.isEmpty()){
        qDebug("open serial failed. dasconfig.comName is empty!");
        return false;
    }
    if(checkSerial()){
        qDebug("serial is oppend!");
        return true;
    }


    QSerialPort *serialPort = NULL;
    if(deviceOperator == NULL){
        serialPort = new QSerialPort();
    }else{
        serialPort = deviceOperator->port;
    }

    serialPort->setPortName(dasData.comName);
    serialPort->setBaudRate(dasData.BaudRate);
    serialPort->setParity(QSerialPort::EvenParity);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if(deviceOperator == NULL){
        deviceOperator = new DeviceOperator(serialPort, dasData.UseZigBee);
    }

    if(!serialPort->open(QIODevice::ReadWrite)){
        qDebug() << QString("[serial] open serial faild!");
        qDebug() << "   ==>comName: "    << dasData.comName;
        qDebug() << "   ==>baudRate: "   << dasData.BaudRate;
        qDebug() << "   ==>Odd|Even: QSerialPort::EvenParity";
        qDebug() << "   ==>dataBit: QSerialPort::Data8";
        qDebug() << "   ==>stopBit: QSerialPort::OneStop";
        return false;
    }else{
        qDebug() << "[serial] open serial success!";
        if(deviceOperator == NULL){
            deviceOperator = new DeviceOperator(serialPort, dasData.UseZigBee);
        }
        return true;
    }
}

bool Helper::initMqtt()
{
    DasData dasData = dasConfig->dasData;

    if(dasData.comName.isEmpty()){//can't be open
        qDebug() << "[MQTT] dasConfig.comName is Empty! ";
        return false;
    }
    if(checkMqtt()){//had been open
        qDebug() << "[MQTT] mqtt is connectted. so don't reopen!";
        return true;
    }

    QMQTT::Client *client = NULL;
    if(mqttOperator == NULL){
        client = new QMQTT::Client();
    }else{
        client = mqttOperator->client;
    }

//        client->setHostName("haph.mqtt.iot.gz.baidubce.com");
    QHostAddress hostAdd("114.242.17.65");
    client->setHost(hostAdd);
    client->setPort(dasData.Port);
    client->setUsername(dasData.Username);
    client->setPassword(dasData.Password.toUtf8());
    //        client->setUsername("haph/dev1");
    //        client->setPassword(QString("/NNfooo+0EMPM6M/JINujITl9ADlQKlwvGg8p7ZLSg8=").toUtf8());
    client->setKeepAlive(10);
    client->setCleanSession(true);
    client->setClientId(dasData.enterprise.DeviceId);
    client->connectToHost();
    qDebug() << "[MQTT] run connectToHost();" ;

    if(mqttOperator == NULL){
        mqttOperator = new MqttOperator(this, client, &dasConfig->dasData);
    }

    if(client->isConnectedToHost()){
        return true;
    }else{
        return false;
    }
}

bool Helper::checkSerial()
{
    if(deviceOperator == NULL
            || deviceOperator->port == NULL
            || !deviceOperator->port->isOpen()){
        return false;
    }
    return true;
}

bool Helper::checkMqtt()
{
    if(mqttOperator == NULL
            || mqttOperator->client == NULL
            || mqttOperator->client->isConnectedToHost()==false)
        return false;
    else
        return true;
}
