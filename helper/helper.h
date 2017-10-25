#ifndef HELPER_H
#define HELPER_H

#include <QObject>
#include <QVector>
#include <QSerialPort>

#include "helper/Mqtt/mqttoperator.h"
#include "helper/JsonConfig/dasConfig.h"
#include "helper/Serial/deviceoperator.h"
#include "helper/Serial/deviceconfiguration.h"

#define CHANNELSIZE 12

class Helper : public QObject
{
    Q_OBJECT
public:
    explicit Helper(QObject *parent = 0);

    DasConfig *dasConfig;
    DeviceOperator  *deviceOperator = NULL;
    MqttOperator    *mqttOperator   = NULL;

    QVector<DataGatherConfiguration>   equArray;

    bool checkSerial();
    bool checkMqtt();

    bool initMqtt();
    bool initSerial();

signals:

public slots:
};

#endif // HELPER_H
