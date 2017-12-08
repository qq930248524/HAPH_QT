#ifndef DATAOPERATOR_H
#define DATAOPERATOR_H

#include <QObject>
#include <QThread>
#include <QDateTime>
#include "helper/Mqtt/mqttoperator.h"
#include "helper/JsonConfig/dasConfig.h"

#define CHANNELSIZE 12
#define ChannelArrayType(arr)    ((bool (*)[CHANNELSIZE])arr)


#define dataHomeDir QString("/var/dasData")
#define dataLocaDir (dataHomeDir + QString("/localData"))
#define dataPendDir (dataHomeDir + QString("/pendingData"))

#define chanlStateDir (dataHomeDir + QString("/channelStateDir"))


class DataOperator : public QThread
{
    Q_OBJECT
public:
    const QString LABEL = QString("[DATA_OPT] ");
    DataOperator();
    DataOperator(MqttOperator *, bool isEncrypt = false);

    enum MsgType {GeneralData=1, DoorOn, DoorOff, PowerOn, PowerOff, SensorOn, SensorOff};

    void save(MsgType, QString);
    void createReport(QString payload);
    bool isEncrypt = false;
    MqttOperator *mqttOperator = NULL;

    DasConfig *cfg;
    QVector<Module> modules;
    bool *staArray = NULL;

    void transmitCfg(DasConfig *);
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
