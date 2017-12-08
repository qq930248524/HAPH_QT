#ifndef DASCONFIG_H
#define DASCONFIG_H

#include <QObject>
#include <QVector>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

class Channel{
public:
    quint16 Id;
    QString Name;
    quint16 InputValueMin;
    quint16 InputValueMax;
    quint16 OutputValueMin;
    quint16 OutputValueMax;
    QString ACOrDC;
    QString DataUnit;
};

class Module{
public:
    quint16 Id;
    QString ZigBeeId;
    QVector<Channel> Channels;
    int getIndexByChannelId(int index){
        for(int i = 0; i < Channels.size(); i++){
            if(Channels[i].Id == index){
                return i;
            }
        }
        return -1;
    }
};
class Enterprise{
public:
    QString Id;
    QString DeviceId;
    QString SerialNo;
    QString Name;
    QVector<Module> Modules;
};

class DasData{
public:
    QString comName;
    QString Server;
    quint32 Port;
    QString Username;
    QString Password;
    QString QueryDelay;
    QString RetryCount;
    QString SamplingFrequency;
    bool    UseZigBee;
    quint32 BaudRate;
    QString ZigBeeId;
    bool    EncryptLog;
    Enterprise enterprise;
};


class DasConfig : public QObject
{
    Q_OBJECT
public:
    explicit DasConfig(QObject *parent = 0);
    bool init(QString filepath);
    int getModeNumByDevid(int devid);
    QJsonParseError jsonError;
    DasData dasData;


signals:

public slots:
};

#endif // DASCONFIG_H
