#ifndef HELPER_H
#define HELPER_H

#include <QObject>
#include <QVector>
#include <QSerialPort>

#include "helper/Mqtt/qmqtt.h"
#include "helper/JsonConfig/dasConfig.h"
#include "helper/Serial/deviceoperator.h"
#include "helper/Serial/deviceconfiguration.h"

#define CHANNELSIZE 12

class Helper : public QObject
{
    Q_OBJECT
public:
    explicit Helper(QObject *parent = 0);

    int TIME_OUT;
    DasConfig *dasConfig;
    DeviceOperator  *deviceOperator;
    QVector<DataGatherConfiguration>   equArray;
    QMQTT::Client   *qmqttClient;
    bool checkSerial();

private:
    void initMqtt();
    void initSerial();

signals:

public slots:
};

#endif // HELPER_H
