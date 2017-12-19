#include "dataoperator.h"
#include "helper/AES/aes.h"
#include <QDir>
#include <QFile>
#include <QTimer>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdio.h>
#include <time.h>

DataOperator::DataOperator()
{
}

DataOperator::DataOperator(MqttOperator *mqttOperator, bool isEncrypt)
    :mqttOperator(mqttOperator), isEncrypt(isEncrypt)
{
    initDir();
}

/*
 * 1、获取传入的dascfg文件
 * 2、申请根据cfg申请内存，用于存储每个通道状态
 */
void DataOperator::transmitCfg(DasConfig *cfg)
{
    this->cfg = cfg;
    this->modules = cfg->dasData.enterprise.Modules;
    staArray = (bool *)malloc(modules.size()*sizeof(bool)*CHANNELSIZE);
    memset(staArray, 0, modules.size()*sizeof(bool)*CHANNELSIZE);
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

/*******************************************
 * 保存数据到本地
 * 1/保存数据到local
 * 2/如果mqtt离线，则复制数据到pending
 * *******************************************/
void DataOperator::save(MsgType type, QString payload)
{
    //获取数据p_con 和 长度len
    QString con = QString("%1 %2").arg(type).arg(payload);
    char *p_con = con.toLatin1().data();

    int len = 0;
    len = getAesLen(p_con);
    char data[len];
    memset(data, '\0', sizeof(data));
    strcpy(data, p_con);
    if(isEncrypt){
        if(aes(data, len, key) == false){
            qCritical() << LABEL << "加密错误！";
        }
    }else{
        len = strlen(p_con)+1;
    }
    p_con = data;

    //保存到local
    QString locDir = dataLocaDir + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd");
    QDir localDir(locDir);
    if(localDir.exists() == false){
        localDir.mkpath(locDir);
    }

    QString locFile = locDir + "/" + QDateTime::currentDateTime().toString("hh-mm-ss");
    FILE *fw = fopen(locFile.toLatin1().data(), "w+b");
    for(int i = 0; i < len; i++){
        putc(p_con[i], fw);
    }
    putc(' ', fw);//添加空格，作为结束。在读取的时候会自动认为空格未结束标示
    fclose(fw);

    //需要写入pending
    if(mqttOperator->isOnline == false){
        QString penDir = dataPendDir + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd");
        QString penFile = penDir + "/" +QDateTime::currentDateTime().toString("hh-mm-ss");
        QDir pendingDir(penDir);
        QFile pendingFile(penFile);

        if(pendingDir.exists() == false){
            pendingDir.mkpath(penDir);
        }
        if(pendingFile.exists() == true){
            pendingFile.remove();
        }

        if(QFile::copy(locFile, penFile) == false){
            qCritical()<<LABEL << "pendingFile 复制失败！ ";
        }
    }

    if(type == GeneralData){
        createReport(payload);        //写入报表
    }
}

/*-----------------------------------
 *     123456789012
 *mod0 000000000000
 *mod1 000000000000
 *mod2 001001001001
 * ----------------------------------
 * 2017-07-01 11:30:30 @1
 * 2017-07-01 11:31:30 @0
 * 2017-07-01 11:32:30 @1
 */
//当通道状态变化时，写入报表
void DataOperator::createReport(QString payload)
{
    QDir chanlDir(chanlStateDir);
    if(!chanlDir.exists()){
        chanlDir.mkpath(chanlStateDir);
    }

    QStringList list = payload.simplified().split(QRegExp("[,]"));
    list.removeAt(0);
    static bool first = true;
    foreach (QString str, list) {
        //获取通道、模块的id
        int moduleId   = str.split(":")[0].split("-")[1].toInt();
        int channelId  = str.split(":")[0].split("-")[2].toInt();
        double value   = str.split(":")[1].toDouble();

        //获取通道、模块的索引
        int moduleIndex = cfg->getModeNumByDevid(moduleId);
        int channelIndex = modules[moduleIndex].getIndexByChannelId(channelId);

        if(modules[moduleIndex].Channels[channelIndex].ACOrDC == "DC"){
            continue;
        }

        //qDebug() << QString("%1-%2:%3").arg(moduleIndex).arg(channelIndex).arg(value);

        //写入
        bool isOn = value >= modules[moduleIndex].Channels[channelIndex].OutputValueMax*0.05;
        bool isDif = ChannelArrayType(staArray)[moduleIndex][channelIndex] != isOn;
        ChannelArrayType(staArray)[moduleIndex][channelIndex] = isOn;
        QFile file(chanlStateDir + QString("/%1-%2").arg(moduleId).arg(channelId));

        //状态变化写入
        if(isDif || first){
            if(file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
                QTextStream out(&file);
                out <<  QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss @") << isOn << endl;
                file.close();
            }
        }

        //0点写入状态
        time_t curtime = time(NULL);
        struct tm *t = localtime(&curtime);

        if(t->tm_hour*3600 + t->tm_min*60 + t->tm_sec < cfg->dasData.SamplingFrequency.toInt()){
            if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
                QTextStream out(&file);
                out <<  QDateTime::currentDateTime().toString("yyyy-MM-dd 00-00-00 @") << isOn << endl;
                file.close();
            }
        }//结束写入
    }//结束遍历
    if(!list.empty()){
        first = false;
    }
}

/******************************
 * mqtt链接后，再次push未发送数据 *
 * 每次都开启一个子线程来操作数据  *
 * ****************************/
void DataOperator::rePushPendingData()
{
    qDebug()<< "================== rePushPendingData()";
    if(!this->isRunning() && QDir(dataPendDir).entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot).size() > 0){
        qDebug()<< "================== rePushPendingData() -> start()";
        this->start();
    }
}

void DataOperator::run()
{
    qDebug()<< "================== run()";
    if(this->mqttOperator->isOnline == false){
        qDebug()<< "================== return....";
        return;
    }

    qDebug()<< "================== run(1)";
    QFileInfoList dirList = QDir(dataPendDir).entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot, QDir::Name|QDir::Reversed);
    foreach (QFileInfo oneDir, dirList) {
        QFileInfoList fileList = QDir(oneDir.absoluteFilePath()).entryInfoList(QDir::Files|QDir::NoDotAndDotDot, QDir::Name|QDir::Reversed);
        foreach (QFileInfo oneFile, fileList) {
            qDebug()<< "================== fileName:" << oneFile.baseName();
            FILE *fr = fopen(oneFile.absoluteFilePath().toLatin1().data(), "r+b");
            char c_data[512] = {0};
            int len = 0;
            for(len = 0; len < 512 && (c_data[len] = getc(fr)) != EOF; len++);
            fclose(fr);

            if(isEncrypt){//加密时，写入数据为16倍数，解密时，读取数据同样为16倍数。可以以此判断是否加密
                if(deAes(c_data, len, key) == false){
                    qCritical()<< LABEL << "解密错误！";
                }
            }

            QString str_data = QString(QLatin1String(&c_data[0]));
            MsgType type = (MsgType)str_data.mid(0,1).toInt();
            QString payload = str_data.mid(2);
            //qDebug() << QString("repush +++++++++++++++++ type:%1  payload:%2").arg(type).arg(payload);

            emit needPush((int)type, payload);

            if( mqttOperator->isOnline == false ){
                return;
            }
            QFile::remove(oneFile.absoluteFilePath());

            sleep(5);//发送间隔，后期改为15s
        }
        oneDir.dir().rmdir(oneDir.absoluteFilePath());
    }
}

void DataOperator::test()
{
    char data[32] = {0};
    strcpy(data, "12345678901234567");
    char * key1 = "1234567890123456";
    aes(data, 32, key1);
    qDebug() << "**************************  " << data;
    FILE *fw = fopen("/root/AES/aaa", "wb");
    for(int i = 0; i < 32; i++){
        putc(data[i], fw);
    }
    fclose(fw);

    qDebug() << "**************************  " << data;
    FILE *fr = fopen("/root/AES/aaa", "rb");
    char data1[32];
    for(int i = 0; i < 32 && (data1[i] = getc(fr)) != EOF; i++);
    fclose(fr);

    qDebug() << "**************************  " << data1;
    deAes(data1, 16, key1);
    qDebug() << "**************************  " << data1;
}
