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
};
class Enterprise{
public:
    QString Id;
    QString DeviceId;
    QString Name;
    QVector<Module> Modules;
};

class DasData{
public:
    QString comName;
    QString Server;
    quint16 Port;
    QString Username;
    QString Password;
    QString QueryDelay;
    QString RetryCount;
    QString SamplingFrequency;
    bool    UseZigBee;
    quint16 BaudRate;
    QString ZigBeeId;
    Enterprise enterprise;
};


class DasConfig : public QObject
{
    Q_OBJECT
public:
    explicit DasConfig(QObject *parent = 0);
    bool init(QString filepath);
    QJsonParseError jsonError;
    DasData dasData;

signals:

public slots:
};

#endif // DASCONFIG_H
