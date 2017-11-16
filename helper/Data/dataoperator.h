#ifndef DATAOPERATOR_H
#define DATAOPERATOR_H

#include <QObject>
#include <QThread>
#include <QDateTime>
#include "helper/Mqtt/mqttoperator.h"

class DataOperator : public QThread
{
public:
    const QString LABEL = QString("[DATA_OPT] ");
    DataOperator();
    DataOperator(MqttOperator *);

    const QString dataHomeDir = QString("/var/dasData");
    const QString dataLocaDir = dataHomeDir + QString("/localData");
    const QString dataPendDir = dataHomeDir + QString("/pendingData");
    enum MsgType {GeneralData=1, DoorOn, DoorOff, PowerOn, PowerOff, SensorOn, SensorOff};

    void save(MsgType, QString);
    MqttOperator *mqttOperator = NULL;
    void initDir();
    void rePushPendingData();

protected:
    void run();
};

#endif // DATAOPERATOR_H
