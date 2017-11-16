#include "dataoperator.h"
#include <QDir>
#include <iostream>

DataOperator::DataOperator()
{

}

DataOperator::DataOperator(MqttOperator *mqttOperator)
    :mqttOperator(mqttOperator)
{
    initDir();
}

void DataOperator::initDir()
{
    QDir localDir(dataLocaDir);
    QDir pendingDir(dataPendDir);
    if(localDir.exists() == false){
        localDir.mkpath(dataLocaDir);
    }
    if(pendingDir.exists() == false){
        pendingDir.mkpath(dataPendDir);
    }
}

void DataOperator::save(MsgType type, QString payload)
{
    QString locDir = dataLocaDir + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd");
    QString locFile = locDir + "/" + QDateTime::currentDateTime().toString("hh-mm-ss");
    QDir localDir(locDir);
    if(localDir.exists() == false){
        localDir.mkpath(locDir);
    }
    QFile file(locFile);
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << type << " " << payload;
    file.flush();
    file.close();

    if(mqttOperator->isOnline == false){
        QString penDir = dataPendDir + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd");
        QString penFile = penDir + "/" +QDateTime::currentDateTime().toString("hh-mm-ss");
        QDir pendingDir(penDir);
        if(pendingDir.exists() == false){
            pendingDir.mkpath(penDir);
        }
        QFile file(penFile);
        file.open(QIODevice::ReadWrite | QIODevice::Append);
        QTextStream text_stream(&file);
        text_stream << type << " " << payload;
        file.flush();
        file.close();
    }

}

void DataOperator::rePushPendingData()
{
    if(!this->isRunning() && QDir(dataPendDir).entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot).size() > 0){
        this->start();
    }
}

void DataOperator::run()
{
    if(this->mqttOperator->isOnline == false){
        return;
    }
    QFileInfoList dirList = QDir(dataPendDir).entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot, QDir::Name|QDir::Reversed);
    foreach (QFileInfo oneDir, dirList) {
        QString s1 = oneDir.absoluteFilePath();
        QFileInfoList fileList = QDir(oneDir.absoluteFilePath()).entryInfoList(QDir::Files|QDir::NoDotAndDotDot, QDir::Name|QDir::Reversed);
        foreach (QFileInfo oneFile, fileList) {
            QString s2 = oneFile.absoluteFilePath();
            QFile file(oneFile.absoluteFilePath());
            QTextStream in(&file);
            QString data = in.readAll();
            MsgType type = (MsgType)data.mid(0,1).toInt();
            QString payload = data.mid(2);
            int ret = 0;
            switch (type) {
            case GeneralData:
                ret = mqttOperator->sendData(payload);
                break;
            case DoorOn:
                ret = mqttOperator->sendDoor(true, payload);
                break;
            case DoorOff:
                ret = mqttOperator->sendDoor(false, payload);
                break;
            case PowerOn:
                ret = mqttOperator->sendPower(true, payload);
                break;
            case PowerOff:
                ret = mqttOperator->sendPower(false, payload);
                break;
            case SensorOn:
                ret = mqttOperator->sendSensor(true, payload);
                break;
            case SensorOff:
                ret = mqttOperator->sendSensor(false, payload);
                break;
            default:
                break;
            }
            file.close();

            if(ret <= 0 || mqttOperator->isOnline == false){
                return;
            }
            oneFile.dir().rmpath(oneFile.absoluteFilePath());
        }
        oneDir.dir().rmpath(oneDir.absoluteFilePath());
    }
}
