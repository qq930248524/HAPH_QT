#include "helper.h"

Helper::Helper(QObject *parent) : QObject(parent)
{
    dasConfig = new DasConfig();
    if(dasConfig->init("/etc/dasconfig.json") == false){
        qDebug("detail json error!");
    }
    initSerial();
    initMqtt();
}

void Helper::initSerial()
{
    QSerialPort *serialPort = new QSerialPort();
    serialPort->setPortName("ttyUSB0");
    serialPort->setBaudRate(QSerialPort::Baud9600);
    serialPort->setParity(QSerialPort::EvenParity);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);
    deviceOperator = new DeviceOperator(serialPort);
    serialPort->open(QIODevice::ReadWrite);
}

void Helper::initMqtt()
{
    qmqttClient = new QMQTT::Client();
}

bool Helper::checkSerial()
{
    if(deviceOperator == NULL || deviceOperator->port == NULL || !deviceOperator->port->isOpen()){
        return false;
    }
    if(equArray.size() == 0){
        return false;
    }
    return true;
}
