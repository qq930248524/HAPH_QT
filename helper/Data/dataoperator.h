#ifndef DATAOPERATOR_H
#define DATAOPERATOR_H

#include <QObject>
#include <QThread>
#include <QDateTime>
#include "helper/Mqtt/mqttoperator.h"

class DataOperator : public QThread
{
    Q_OBJECT
public:
    const QString LABEL = QString("[DATA_OPT] ");
    DataOperator();
    DataOperator(MqttOperator *, bool isEncrypt = false);

    const QString dataHomeDir = QString("/var/dasData");
    const QString dataLocaDir = dataHomeDir + QString("/localData");
    const QString dataPendDir = dataHomeDir + QString("/pendingData");
    enum MsgType {GeneralData=1, DoorOn, DoorOff, PowerOn, PowerOff, SensorOn, SensorOff};

    void save(MsgType, QString);
    bool isEncrypt = false;
    MqttOperator *mqttOperator = NULL;
    void initDir();
    void rePushPendingData();

public slots:

signals:
    void needPush(int, QString);

protected:
    void run();

public:
    void test();
};

#endif // DATAOPERATOR_H
