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
        qDebug() << "[serial] comName is empty!";
        return false;
    }

    QSerialPort *serialPort = new QSerialPort();
    serialPort->setPortName(dasData.comName);
    serialPort->setBaudRate(dasData.BaudRate);

    serialPort->setParity(QSerialPort::EvenParity);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);
    deviceOperator = new DeviceOperator(serialPort);
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
            deviceOperator = new DeviceOperator(serialPort);
        }
        return true;
    }
}

bool Helper::initMqtt()
{
    qDebug() << "[helper] into initMqtt!";
    qmqttClient = new QMQTT::Client();
    return false;
}

bool Helper::checkSerial()
{
    if(deviceOperator == NULL || deviceOperator->port == NULL || !deviceOperator->port->isOpen()){
        return false;
    }
    return true;
}
bool Helper::checkMqtt()
{
    return false;
}
