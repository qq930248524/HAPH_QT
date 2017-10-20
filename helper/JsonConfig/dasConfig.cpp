#include "dasConfig.h"
#include <QDebug>


DasConfig::DasConfig(QObject *parent) : QObject(parent)
{
    dasData.comName = "";
}


bool DasConfig::init(QString filepath)
{
    QFile file(filepath);
    if(!file.exists()){
        qWarning("dasconfig.json not exists!");
        return false;
    }
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QString val = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8(), &jsonError);
    if(jsonError.error != QJsonParseError::NoError){
        qWarning() << "detail json error!" << jsonError.errorString();
        return false;
    }

    QJsonObject obj = doc.object();
    dasData.comName = obj["comName"].toString();
    dasData.Server  = obj["Server"].toString();
    dasData.Port    = obj["Port"].toInt();
    dasData.Username    = obj["Username"].toString();
    dasData.Password    = obj["Password"].toString();
    dasData.QueryDelay  = obj["QueryDelay"].toString();
    dasData.RetryCount  = obj["RetryCount"].toString();
    dasData.SamplingFrequency   = obj["SamplingFrequency"].toString();
    dasData.UseZigBee   = obj["UseZigBee"].toBool();
    dasData.BaudRate    = obj["BaudRate"].toInt();
    dasData.ZigBeeId    = obj["ZigBeeId"].toString();
    QJsonObject enterObj = obj["Enterprise"].toObject();
    dasData.enterprise.Id = enterObj["Id"].toString();
    dasData.enterprise.DeviceId = enterObj["QJsonObject"].toString();
    dasData.enterprise.Name = enterObj["Name"].toString();

    QJsonArray  modeArray   = enterObj["Modules"].toArray();
    qDebug() << "----------------modesize=" << modeArray.size();
    dasData.enterprise.Modules.resize(modeArray.size());
    for(int i = 0; i < modeArray.size(); i++){
        Module mode;
        QJsonObject modeObj = modeArray.at(i).toObject();
        mode.Id = modeObj["Id"].toInt();
        mode.ZigBeeId   = modeObj["ZigBeeId"].toString();

        QJsonArray  channelArray    = modeObj["Channels"].toArray();
        qDebug() << "------------------------channelArray.size:" << channelArray.size();
        mode.Channels.resize(channelArray.size());
        for(int j = 0; j < channelArray.size(); j++){
            Channel channel;
            QJsonObject channelObj = channelArray.at(j).toObject();
            channel.Id      = channelObj["Id"].toInt();
            channel.Name    = channelObj["Name"].toString();
            channel.InputValueMin   = channelObj["InputValueMin"].toInt();
            channel.InputValueMax   = channelObj["InputValueMax"].toInt();
            channel.OutputValueMin  = channelObj["OutputValueMin"].toInt();
            channel.OutputValueMax  = channelObj["OutputValueMax"].toInt();
            channel.ACOrDC          = channelObj["ACOrDC"].toString();
            channel.DataUnit        = channelObj["DataUnit"].toString();
            mode.Channels[j] = channel;
        }
        dasData.enterprise.Modules[i] = mode;
    }
}

int DasConfig::getModeNumByDevid(int devid)
{
    if(dasData.comName.isEmpty()){
        return -1;
    }
    for(int i = 0; i < dasData.enterprise.Modules.size(); i++){
        if(dasData.enterprise.Modules[i].Id == devid){
            return i;
        }
    }
    return -1;
}
