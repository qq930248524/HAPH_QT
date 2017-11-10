#include "helper.h"

Helper::Helper(QObject *parent) : QObject(parent)
{
    dasConfig = new DasConfig();
    if(dasConfig->init("/etc/dasconfig.json") == false){
        qWarning("detail json error!");
    }else{
        modeSize = dasConfig->dasData.enterprise.Modules.size();

        dasDataBuf = new int [modeSize*CHANNELSIZE];
        for(int i = 0; i < CHANNELSIZE*modeSize; i++){
            dasDataBuf[i] = -1;
        }
    }
    initSerial();
    initMqtt();
    initGPIO();
    gotoRun();
}

bool Helper::initGPIO()
{
    //设置GPIO
    GPIOset *gpioSet = new GPIOset();
    connect(gpioSet, SIGNAL(door(bool)), mqttOperator, SLOT());
    connect(gpioSet, SIGNAL(dcac(bool)), mqttOperator, SLOT());

    if(gpioSet->initUart() == true){
        gpioSet->start();
    }
    //TODO

    return true;
}

bool Helper::initSerial()
{    
    DasData dasData = dasConfig->dasData;
    if(dasData.comName.isEmpty()){
        qDebug("open serial failed. dasconfig.comName is empty!");
        return false;
    }
    if(checkSerial()){ // don't reOpen
        qDebug("serial is oppend!");
        return true;
    }


    QSerialPort *serialPort = NULL;
    if(deviceOperator == NULL){
        serialPort = new QSerialPort();
    }else{
        serialPort = deviceOperator->port;
    }

    serialPort->setPortName(dasData.comName);
    serialPort->setBaudRate(dasData.BaudRate);
    serialPort->setParity(QSerialPort::EvenParity);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if(deviceOperator == NULL){
        deviceOperator = new DeviceOperator(serialPort, dasData.UseZigBee);
    }

    if (dasData.UseZigBee == false){
        deviceOperator->useZigbee = false;
        deviceOperator->hostId = 0;
    }else{
        deviceOperator->useZigbee = true;
        deviceOperator->hostId |= dasData.ZigBeeId.mid(2,2).toInt()<<8;
        deviceOperator->hostId |= dasData.ZigBeeId.mid(4,2).toInt();
    }

    if(!serialPort->open(QIODevice::ReadWrite)){
        qDebug() << QString("[serial] open serial faild!");
        qDebug() << "   ==>comName: "    << dasData.comName;
        qDebug() << "   ==>baudRate: "   << dasData.BaudRate;
        qDebug() << "   ==>Odd|Even: QSerialPort::EvenParity";
        qDebug() << "   ==>dataBit: QSerialPort::Data8";
        qDebug() << "   ==>stopBit: QSerialPort::OneStop";
        return false;
    }else{
        qDebug() << "[serial] open serial success!";
        if(deviceOperator == NULL){
            deviceOperator = new DeviceOperator(serialPort, dasData.UseZigBee);
        }
        return true;
    }
}

bool Helper::initMqtt()
{
    DasData dasData = dasConfig->dasData;

    if(dasData.comName.isEmpty()){//can't be open
        qDebug() << "[MQTT] dasConfig.comName is Empty! ";
        return false;
    }
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
    client->setKeepAlive(10);
    client->setCleanSession(true);
    client->setClientId(dasData.enterprise.DeviceId);
    client->connectToHost();
    qDebug() << "[MQTT] run connectToHost();" ;

    if(mqttOperator == NULL){
        mqttOperator = new MqttOperator(this, client, &dasConfig->dasData);
    }

    if(client->isConnectedToHost()){
        return true;
    }else{
        return false;
    }
}

bool Helper::checkSerial()
{
    if(deviceOperator == NULL
            || deviceOperator->port == NULL
            || !deviceOperator->port->isOpen()){
        return false;
    }
    return true;
}

bool Helper::checkMqtt()
{
    if(mqttOperator == NULL
            || mqttOperator->client == NULL
            || mqttOperator->client->isConnectedToHost()==false)
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
    if(dasTimer == NULL){
        dasTimer = new QTimer(this);
        dasTimer->setInterval(dasConfig->dasData.SamplingFrequency.toInt()*1000);
        connect(dasTimer, SIGNAL(timeout()), this, SLOT(onGetAllpRef_time()));
        connect(deviceOperator, SIGNAL(getAllpRef(int32_t *)), this, SLOT(onGetAllpRef_ret(int32_t *)));
    }
    if(dasTimer->isActive() == false){
        dasTimer->start();
    }
}

void Helper::stopRun()
{
    if(checkSerial() == true){
        deviceOperator->port->close();
    }
    if(dasTimer != NULL && dasTimer->isActive() == true){
        dasTimer->stop();
    }
}

void Helper::getModeData(int modeNum, int32_t *pData)
{
    if(modeNum < 0 || modeNum >= modeSize || checkSerial() == false){
        for(int i = 0; i < CHANNELSIZE; i++){
            pData[i] = -1;
            return;
        }
    }
    memcpy(pData, (void *)&dasDataBuf[modeNum*CHANNELSIZE], CHANNELSIZE*sizeof(int32_t));
}

/********************** SLOT deviceOpt *********************/
//TODO
void Helper::onGetAllpRef_ret(int32_t *data)
{
    QString msg = QDateTime::currentDateTime().toString(" yyyy-MM-dd hh:mm:ss");

    int32_t devNum = dasConfig->dasData.enterprise.Modules.size();

    for(int i = 0; i < devNum; i++){
        Module oneModule = dasConfig->dasData.enterprise.Modules[i];
        for(int j = 0; j < oneModule.Channels.size(); j++){
            int32_t channelId = oneModule.Channels[j].Id;
            int32_t channelIndex = i*CHANNELSIZE + channelId-1;
            dasDataBuf[channelIndex] = data[channelIndex];
            msg.append(QString(", %1-%2-%3:%4")
                       .arg(dasConfig->dasData.ZigBeeId.mid(2,4))
                       .arg(oneModule.Id)
                       .arg(channelId)
                       .arg(data[channelIndex]));
        }
    }
    mqttOperator->sendData(msg);
}

//timer slot
void Helper::onGetAllpRef_time()
{
    DasData dasData = dasConfig->dasData;

    /*************************************************************
     *  |0      |1      |2      |3      |4      |n
     *  len     slaveId slaveId slaveId slaveId slaveId
     * ************************************************************/
    int32_t idArray[modeSize+1];
    idArray[0] = modeSize+1;

    if(dasData.UseZigBee){
        for(int i = 0; i < dasData.enterprise.Modules.size(); i++){
            idArray[1+i] |= dasData.enterprise.Modules[i].ZigBeeId.mid(2,2).toInt()<<8;
            idArray[1+i] |= dasData.enterprise.Modules[i].ZigBeeId.mid(4,2).toInt();
        }
    }else{
        for(int i = 0; i < dasData.enterprise.Modules.size(); i++){
            idArray[1+i] = dasData.enterprise.Modules[i].Id;
        }
    }
//TODO
    deviceOperator->onGetAllpRef(idArray);
}
