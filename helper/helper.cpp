#include "helper.h"

Helper::Helper(QObject *parent) : QObject(parent)
{
    dasConfig = new DasConfig();
    if(dasConfig->init("/etc/dasconfig.json") == true){
        modeSize = dasConfig->dasData.enterprise.Modules.size();

        dasDataBuf = new int [modeSize*CHANNELSIZE];
        dasDataCounter = new int[modeSize];
        for(int i = 0; i < CHANNELSIZE*modeSize; i++){
            dasDataBuf[i] = -1;
        }
        for(int i = 0; i < modeSize; i++){
            dasDataCounter[i] = -1;
        }
        initSerial();
        initMqtt();
        initGPIO();
        initDataControl();
        gotoRun();//UI run
    }else{
        qWarning("das解析失败，后台将不会运行和提供数据!");
    }
}

bool Helper::initGPIO()
{
    //设置GPIO
    GPIOset *gpioSet = new GPIOset();
    connect(gpioSet, SIGNAL(door(bool)), this, SLOT(sendDoor(bool)));
    connect(gpioSet, SIGNAL(dcac(bool)), this, SLOT(sendPower(bool)));

    if(gpioSet->initUart() == true){
        gpioSet->start();
    }
    //TODO

    return true;
}

bool Helper::initDataControl()
{
    if(dataOperator == NULL){
        dataOperator = new DataOperator(mqttOperator, dasConfig->dasData.EncryptLog);
        dataOperator->transmitCfg(dasConfig);
        connect(dataOperator, SIGNAL(needPush(int,QString)),
                mqttOperator, SLOT(onNeedPush(int,QString)));
    }
    return true;
}

bool Helper::initSerial()
{
    DasData dasData = dasConfig->dasData;
    if(checkSerial()){ // don't reOpen
        qDebug("serial is oppend!");
        return true;
    }

    //构建serialList和deviceOpeList
    foreach (Module oneModule, dasData.enterprise.Modules) {//拒绝重复
        QSerialPort *oneSerial = NULL;
        foreach (QSerialPort *ser, serialList) {
            QString str1 = oneModule.comName;
            QString str2 = ser->portName();
            if(oneModule.comName.indexOf(ser->portName())){
                oneSerial = ser;
            }
        }

        if(oneSerial == NULL){ //添加一个串口
            oneSerial = new QSerialPort();
            oneSerial->setPortName(oneModule.comName);
            oneSerial->setBaudRate(oneModule.BaudRate);
            oneSerial->setParity(QSerialPort::EvenParity);
            oneSerial->setDataBits(QSerialPort::Data8);
            oneSerial->setStopBits(QSerialPort::OneStop);
            oneSerial->setFlowControl(QSerialPort::NoFlowControl);
            serialList.append(oneSerial);
            if(!oneSerial->open(QIODevice::ReadWrite)){
                qDebug() << QString("[serial] open serial faild!");
                qDebug() << "   ==>comName: "    << oneSerial->portName();
                qDebug() << "   ==>baudRate: "   << oneSerial->baudRate();
                qDebug() << "   ==>Odd|Even: QSerialPort::EvenParity";
                qDebug() << "   ==>dataBit: QSerialPort::Data8";
                qDebug() << "   ==>stopBit: QSerialPort::OneStop";
                return false;
            }
        }

        int hostId = 0;
        hostId |= dasData.ZigBeeId.mid(2,2).toInt()<<8;
        hostId |= dasData.ZigBeeId.mid(4,2).toInt();
        deviceOptList.append(new DeviceOperator(oneSerial, hostId));//添加一个devopt
    }
    return true;
}

bool Helper::initMqtt()
{
    DasData dasData = dasConfig->dasData;

    if(checkMqtt()){//had been open
        qDebug() << "[MQTT] mqtt is connectted. so don't reopen!";
        return true;
    }

    QMQTT::Client *client = NULL;
    if(mqttOperator == NULL){
        client = new QMQTT::Client();
    }else{
        client = mqttOperator->client;
    }

    //        client->setHostName("haph.mqtt.iot.gz.baidubce.com");
    QHostAddress hostAdd(dasData.Server);
    client->setHost(hostAdd);
    client->setPort(dasData.Port);
    client->setUsername(dasData.Username);
    client->setPassword(dasData.Password.toUtf8());
    //        client->setUsername("haph/dev1");
    //        client->setPassword(QString("/NNfooo+0EMPM6M/JINujITl9ADlQKlwvGg8p7ZLSg8=").toUtf8());
    //        client->setKeepAlive(10);
    client->setCleanSession(true);
    client->setClientId(dasData.enterprise.SerialNo);
    client->connectToHost();
    qDebug() << "[MQTT] run connectToHost();" ;

    if(mqttOperator == NULL){
        mqttOperator = new MqttOperator(this, client, &dasConfig->dasData);
    }
    return true;
}

bool Helper::checkSerial()
{
    if(serialList.size() == 0){
        return false;
    }
    return true;
}

bool Helper::checkMqtt()
{
    if(mqttOperator == NULL
            || mqttOperator->client == NULL
            || mqttOperator->isOnline ==false)

        return false;
    else
        return true;
}

/************************ UI fun *********************/
void Helper::gotoRun()
{
    if(initSerial() == false){
        return;
    }
    if(timerId == -1){
        foreach (QSerialPort *ser, serialList) {
            ser->open(QIODevice::ReadWrite);
        }
        timerId = startTimer(dasConfig->dasData.SamplingFrequency.toInt()*1000);
    }
}

void Helper::stopRun()
{
    if(timerId != -1){
        killTimer(timerId);
        foreach (QSerialPort *ser, serialList) {
            ser->close();
        }
        timerId = -1;
    }
}

void Helper::getModeData(int modeNum, int32_t *pData)//ui slot
{
    memcpy(pData, (void *)&dasDataBuf[modeNum*CHANNELSIZE], CHANNELSIZE*sizeof(int32_t));
}

/*/**************************************************
 * @brief:  helper后端定时器，间隔获取模块数据，并3次容错，最后mqtt发送+数据保存
 * @param：无用
 * @return: NULL
 **************************************************/
void Helper::timerEvent ( QTimerEvent * event )
{
    QString msg = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");


    //开始获取数据
    for (int i = 0; i < deviceOptList.size(); i++){
        DeviceOperator * oneDevOpt = deviceOptList[i];
        Module oneModule = dasConfig->dasData.enterprise.Modules[i];
        int32_t zigbeeId = 0;
        zigbeeId |= oneModule.ZigBeeId.mid(2,2).toInt()<<8;
        zigbeeId |= oneModule.ZigBeeId.mid(4,2).toInt();
        if(!oneDevOpt->readDevRegister(&dasDataBuf[i*CHANNELSIZE],
                                       oneModule.Id,
                                       zigbeeId,
                                       MB_REG_ADCRES_ADDR,
                                       MB_REG_ADCRES_LENGTH))
        {//读取错误,计数加一
            if(++dasDataCounter[i] == dasConfig->dasData.RetryCount.toInt()){//次数用尽
                dasDataCounter[i] = 0;
                for(int j = 0; j < CHANNELSIZE; j++){
                    dasDataBuf[i*CHANNELSIZE+j] = -1;
                }
            }
        }else{
            dasDataCounter[i] = 0;
        }

        for(int j = 0; j < oneModule.Channels.size(); j++){//便利通道
            int32_t channelId = oneModule.Channels[j].Id;
            int32_t channelIndex = i*CHANNELSIZE + channelId-1;

            msg.append(QString(",%1-%2-%3:%4")
                       .arg(dasConfig->dasData.enterprise.DeviceId)
                       .arg(oneModule.Id, 2, 10, QChar('0'))
                       .arg(channelId, 2, 10, QChar('0'))
                       .arg(dasDataBuf[channelIndex]));
        }//over 通道
    }

    if(mqttOperator->sendData(msg) == false){
        initMqtt();
    }
    dataOperator->save(DataOperator::GeneralData, msg);
}

/*********************** GPIO SLOT *************************/
void Helper::sendDoor(bool isOn)
{
    QString msg = QDateTime::currentDateTime().toString(" yyyy-MM-dd hh:mm:ss");
    mqttOperator->sendDoor(isOn, msg);
    dataOperator->save(isOn?(DataOperator::DoorOn):(DataOperator::DoorOff), msg);
}

void Helper::sendPower(bool isOn)
{
    QString msg = QDateTime::currentDateTime().toString(" yyyy-MM-dd hh:mm:ss");
    mqttOperator->sendPower(isOn, msg);
    dataOperator->save(isOn?(DataOperator::PowerOn):(DataOperator::PowerOff), msg);
}
