#include "collectionsetting.h"

#include <QDebug>
#include <QSerialPortInfo>

CollectionSetting::CollectionSetting(QWidget *parent) : QWidget(parent)
{
    mainLayout = new QHBoxLayout();
    initCheckArrayUI();
    initMidWidgetUI();
    initButtonArrayUI();
    setLayout(mainLayout);

    initDev();//初始化串/devOperator
}

void CollectionSetting::initCheckArrayUI()
{
    QVBoxLayout *pVBoxLayout = new QVBoxLayout();

    radioArray.resize(CHANNELSIZE);
    lineArray.resize(CHANNELSIZE);
    for(int i = 0; i < radioArray.size(); i++){
        QLabel          *label      = new QLabel(QString("No.%1").arg(i+1));
        SwitchButton    *sbtn       = new SwitchButton(this);
        QLineEdit       *lineEdit   = new QLineEdit();

        label->setFixedWidth(40);
        sbtn->setButtonStyle(SwitchButton::ButtonStyle::ButtonStyle_Rect);
        sbtn->setText("交流", "直流");
        sbtn->setTextColor(QColor("black"), QColor("black"));
        sbtn->setFixedWidth(60);
        lineEdit->setReadOnly(true);
        lineEdit->setAlignment(Qt::AlignLeft);
        lineEdit->setFixedWidth(80);

        radioArray[i]   =     sbtn;
        lineArray[i]    =     lineEdit;

        QHBoxLayout *pHBoxLayout = new QHBoxLayout();
        pHBoxLayout->addWidget(label);
        pHBoxLayout->addWidget(sbtn);
        pHBoxLayout->addWidget(lineEdit, 1, Qt::AlignLeft);
        pVBoxLayout->addLayout(pHBoxLayout);
    }

    QFrame *frame = new QFrame();
    frame->setLayout(pVBoxLayout);
    frame->setFrameStyle(QFrame::WinPanel|QFrame::Raised);
    mainLayout->addWidget(frame);
}

void CollectionSetting::initMidWidgetUI()
{
    //set QComboBox
    portNumBox      = new QComboBox();
    modNumBox       = new QComboBox();
    checkDigitBox   = new QComboBox();
    baudRateBox     = new QComboBox();

    portNumBox->setFixedWidth(160);
    modNumBox->setFixedWidth(160);
    checkDigitBox->setFixedWidth(160);
    baudRateBox->setFixedWidth(160);

    portNumBox->setMaxVisibleItems(5);
    modNumBox->setMaxVisibleItems(5) ;
    checkDigitBox->setMaxVisibleItems(2) ;
    baudRateBox->setMaxVisibleItems(10) ;

    QStringList comNum;
    foreach(const QSerialPortInfo &onePort, QSerialPortInfo::availablePorts()){
        comNum.append(onePort.portName());
    }
    portNumBox->addItems(comNum);
    portNumBox->setCurrentIndex(0);
    modNumBox->addItems(QStringList()<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8");
    modNumBox->setCurrentIndex(0);
    checkDigitBox->addItems(QStringList()<<"NO"<<"Odd"<<"Even");
    checkDigitBox->setCurrentIndex(2);
    for(int i = 0; i < DeviceOperator::N_DEV_BAUDRATE; i++){
        baudRateBox->addItem(QString("%1").arg(DeviceOperator::DeviceBaudrateList[i]));
    }
    baudRateBox->setCurrentIndex(5);

    //////////////////////////////////////////////////////////////////////////
    QHBoxLayout *hLayout1 = new QHBoxLayout();
    hLayout1->addWidget(new QLabel("串口名称:"), 1, Qt::AlignRight);
    hLayout1->addWidget(portNumBox);
    connect(portNumBox, SIGNAL(activated(QString)), this, SLOT(showPortInfo(QString)));

    QHBoxLayout *hLayout2 = new QHBoxLayout();
    hLayout2->addWidget(new QLabel("校验位:"), 1, Qt::AlignRight);
    hLayout2->addWidget(checkDigitBox);

    QHBoxLayout *hLayout3 = new QHBoxLayout();
    hLayout3->addWidget(new QLabel("波特率:"), 1, Qt::AlignRight);
    hLayout3->addWidget(baudRateBox);

    QHBoxLayout *hLayout4 = new QHBoxLayout();
    hLayout4->addWidget(new QLabel("模块编号:"), 1, Qt::AlignRight);
    hLayout4->addWidget(modNumBox);
//    connect(modNumBox, SIGNAL(activated(QString)), this, SLOT(updateCheckArray(QString)));

    logshow            =  new QTextEdit();
    //logshow->document ()->setMaximumBlockCount (5);
    logshow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    logshow->setFixedSize(310, 220);
    logshow->setReadOnly(true);

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addLayout(hLayout1);
    vLayout->addLayout(hLayout2);
    vLayout->addLayout(hLayout3);
    vLayout->addLayout(hLayout4);
    vLayout->addStretch(1);
    vLayout->addWidget(new QLabel("结果:"));
    vLayout->addWidget(logshow);

    mainLayout->addStretch(1);
    mainLayout->addLayout(vLayout);
}

void CollectionSetting::initButtonArrayUI()
{
    QPushButton *searchModNum   = new QPushButton("搜索模块编号");
    QPushButton *setModNum      = new QPushButton("设置模块编号");
    QPushButton *setPassType    = new QPushButton("设置通道类型");
    QPushButton *readPassData   = new QPushButton("读取通道数据");
    QPushButton *readZigbeeData = new QPushButton("zigbee取数据");

    connect(searchModNum,   SIGNAL(pressed()), this, SLOT(searchModNum()));
    connect(setModNum,      SIGNAL(pressed()), this, SLOT(setModNum()));
    connect(setPassType,    SIGNAL(pressed()), this, SLOT(setPassType()));
    connect(readPassData,   SIGNAL(pressed()), this, SLOT(readPassData()));
    connect(readZigbeeData, SIGNAL(pressed()), this, SLOT(readZigbeeData()));
//    connect(readZigbeeData, SIGNAL(pressed()), this, SLOT(test()));

    QVBoxLayout *vLayout    = new QVBoxLayout();
    vLayout->addWidget(searchModNum, 3, Qt::AlignTop);
    vLayout->addWidget(setModNum, 3, Qt::AlignTop);
    vLayout->addWidget(setPassType, 3, Qt::AlignTop);
    vLayout->addStretch(3);
    vLayout->addWidget(readPassData, 3, Qt::AlignTop);
    vLayout->addWidget(readZigbeeData, 3, Qt::AlignTop);

        setStyleSheet("QPushButton {"
                      "Background:rgb(18, 165, 53);"
                      "font-size:30px;}");

    QFrame *frame = new QFrame();
    frame->setLayout(vLayout);
    frame->setFrameStyle(QFrame::Panel|QFrame::Raised);
    mainLayout->addStretch(1);
    mainLayout->addWidget(frame);
}
void CollectionSetting::initDev()
{    
    DeviceOperator * deviceOperator = new DeviceOperator(NULL);
    deviceOperator->useZigbee = false;
    helper->setting_deviceOperator = deviceOperator;

    connect(deviceOperator, SIGNAL(deviceInformationGot(bool,DataGatherConfiguration)),
            this, SLOT(onGotDevInfo(bool,DataGatherConfiguration)));
    connect(deviceOperator, SIGNAL(finishedDevSearching()),
            this, SLOT(onSearchDeviceFinished()));
    connect(deviceOperator, SIGNAL(finishedDevCalibrate(bool)),
            this, SLOT(onCalibrateDeviceFinished(bool)));
    connect(deviceOperator, SIGNAL(finishedDevConfigSet(bool)),
            this, SLOT(onSetDevConfigFinished(bool)));
    connect(deviceOperator, SIGNAL(sendMsg(QByteArray)),
            this, SLOT(updateSendText(QByteArray)));
    connect(deviceOperator, SIGNAL(recvMsg(QByteArray)),
            this, SLOT(updateRecvText(QByteArray)));
    connect(deviceOperator, SIGNAL(deviceADCResultGot(int,int32_t*)),
            this, SLOT(updateADCLine(int, int32_t *)));
}

/*************** btn SLOT************************/
void    CollectionSetting::searchModNum()
{
    DeviceOperator *deviceOperator = helper->setting_deviceOperator;

    QSerialPort *serialPort = deviceOperator->port;
    if(serialPort == NULL){
        serialPort = new QSerialPort();
        deviceOperator->port = serialPort;
    }

    if(serialPort->isOpen()){
        serialPort->close();
    }

    serialPort->setPortName(portNumBox->currentText());
    serialPort->setBaudRate(DeviceOperator::DeviceBaudrateList[baudRateBox->currentIndex()]);
    switch(checkDigitBox->currentIndex())
    {
    case MB_EVEN_PARITY:
        serialPort->setParity(QSerialPort::EvenParity);
        break;
    case MB_ODD_PARITY:
        serialPort->setParity(QSerialPort::OddParity);
        break;
    default:
        serialPort->setParity(QSerialPort::NoParity);
        break;
    }


    if(serialPort->open(QIODevice::ReadWrite)){
        logshow->append(QString("打开串口<%1>成功! ").arg(serialPort->portName()));
    }else{
        logshow->append(QString("打开串口<%1>失败! party:%2 baud:%3")
                        .arg(serialPort->portName())
                        .arg(serialPort->EvenParity)
                        .arg(serialPort->baudRate())
                        );
        return;
    }

    helper->equArray.clear();
    deviceOperator->searchDevices();
}

void    CollectionSetting::setModNum()
{
    QVector<DataGatherConfiguration> &equArray = helper->equArray;
    DeviceOperator *deviceOperator = helper->setting_deviceOperator;

    DataGatherConfiguration newCfg = equArray[0];
    newCfg.devID = modNumBox->currentText().toInt();
    newCfg.modbusBaudrate = baudRateBox->currentIndex();
    newCfg.parity = static_cast<ModbusParityMode> (checkDigitBox->currentIndex());

    deviceOperator->setDeviceConfig(equArray[0], newCfg);
}

void    CollectionSetting::setPassType()
{
    QVector<DataGatherConfiguration> &equArray = helper->equArray;
    DeviceOperator *deviceOperator = helper->setting_deviceOperator;

    DataGatherConfiguration newCfg = equArray[0];
    newCfg.inputMode = 0;
    for(int i = 0; i < CHANNELSIZE; i++){
        if(radioArray[i]->getChecked()){
            newCfg.inputMode |=( 0x01U << i);
        }
    }

    deviceOperator->setDeviceConfig(equArray[0], newCfg);
}

void    CollectionSetting::readPassData()
{
    DeviceOperator *deviceOperator = helper->setting_deviceOperator;
    deviceOperator->getDeviceADCRes(0, helper->equArray[0].devID);
    qDebug() << "===================" << modNumBox->currentData().toInt() << endl;
}

void    CollectionSetting::readZigbeeData(){}


/*********** devOperator SLOT ******************/
void    CollectionSetting::onGotDevInfo(bool isok,DataGatherConfiguration cfg)
{
    QVector<DataGatherConfiguration> &equArray = helper->equArray;

    if(!isok){
        return;
    }
    for(int i =0; i < equArray.size();i++){
        if(cfg.devID == equArray[i].devID){
            equArray[i] = cfg;
            logshow->append(QString("update dev config success! devId = %1")
                            .arg(cfg.devID));
            return;
        }
    }
    logshow->append(QString("get one dev:devID=%1").arg(cfg.devID));
    equArray.append(cfg);
}

void    CollectionSetting::onSearchDeviceFinished()
{
    QVector<DataGatherConfiguration> &equArray = helper->equArray;
    DeviceOperator *deviceOperator = helper->setting_deviceOperator;

    if(equArray.size() == 0){
        logshow->append("没有搜索到任何设备");
        return;
    }
    logshow->clear();

    DataGatherConfiguration cfg = equArray[0];

    logshow->append(QString("搜索到%1 个设备,默认选中第一个设备!").arg(equArray.size()));
    logshow->append("[硬件版本]" + QString("V%1.%2").arg(cfg.hardwareVersion>>8).arg(cfg.hardwareVersion&0xFFU));
    logshow->append("[软件版本]" + QString(QString("V%1.%2.%3").arg(cfg.firmwareVersion>>8).arg((cfg.firmwareVersion&0xF0U)>>4).arg(cfg.firmwareVersion&0x0FU)));
    QString str;
    for(size_t i = 0; i < sizeof(cfg.serialID); ++i)
    {
        str += QString("%1").arg(static_cast<int>(cfg.serialID[i]), 2, 16, QLatin1Char('0'));
    }
    logshow->append("[设备序列号]" + str);
    str = QString("%1%2%3%4").arg(QString(cfg.productName[0])).arg(QString(cfg.productName[1]))
                             .arg(QString(cfg.productName[2])).arg(QString(cfg.productName[3]));
    logshow->append("[设备类型]" + str);

    modNumBox->setCurrentIndex(cfg.devID - 1);
    switch(cfg.parity)
    {
    case MB_NO_PARITY:
        checkDigitBox->setCurrentText("无校验");
        break;
    case MB_ODD_PARITY:
        checkDigitBox->setCurrentText("奇校验");
        break;
    case MB_EVEN_PARITY:
        checkDigitBox->setCurrentText("偶校验");
        break;
    default:
        checkDigitBox->setCurrentText("未知");
        break;
    }

    if(cfg.modbusBaudrate < DeviceOperator::N_DEV_BAUDRATE)
        baudRateBox->setCurrentText(QString::number(DeviceOperator::DeviceBaudrateList[cfg.modbusBaudrate]));
    else
        baudRateBox->setCurrentText(tr("未知"));

    DataGatherConfiguration curCfg = equArray[0];
    for(int i = 0; i < CHANNELSIZE; i++){
        radioArray[i]->setChecked( curCfg.inputMode & (0x01U << i));
        //logshow->append(curCfg.inputMode& (0x01U << i) ? "1":"0");    //打印checkArray
    }
}

void    CollectionSetting::onSetDevConfigFinished(bool isok)
{
    if(isok){
        logshow->append(QString("update success!"));
    }else{
        logshow->append(QString("update faild!"));
    }
}

void    CollectionSetting::updateSendText(QByteArray msg)
{
    QString str =  msg.toHex().toUpper();
    int len = str.length()/2;
    for(int i = 0; i < len; i++){
        str.insert(2*i+i-1, " ");
    }
    logshow->append("[send]>>" + str);
}

void    CollectionSetting::updateRecvText(QByteArray msg)
{
    QString str = msg.toHex().toUpper();
    int len = str.length()/2;
    for(int i = 0; i < len; i++){
        str.insert(2*i+i-1, " ");
    }
    logshow->append("[recv]<<" + str);
}

////////////////////////////////////////////////////////////////

void CollectionSetting::showPortInfo(QString portName)
{
    if(helper->dasConfig->dasData.comName.indexOf(portName) != -1){
        logshow->append(QString("warning!!! port:%1 is dasconfig.comName ").arg(portName));
    }
}

void CollectionSetting::updateCheckArray(int index)
{
    QVector<DataGatherConfiguration> &equArray = helper->equArray;
    DeviceOperator *deviceOperator = helper->setting_deviceOperator;

    qDebug() << "equArray.size = " << equArray.size() << endl;
    qDebug() << "first dev's devId is " << equArray[0].devID << endl;

    DataGatherConfiguration curCfg = equArray[index];
    for(int i = 0; i < CHANNELSIZE; i++){
        radioArray[i]->setChecked( curCfg.inputMode & (0x01U << i));
        //logshow->append(curCfg.inputMode& (0x01U << i) ? "1":"0");    //打印checkArray
    }
    //update adcLine
    deviceOperator->getDeviceADCRes(0 ,equArray[0].devID);
}

void CollectionSetting::updateADCLine(int devId, int32_t *pRes)
{
    QVector<DataGatherConfiguration> &equArray = helper->equArray;
    const uint16_t devInputMode = equArray[0].inputMode;
    const double ACK = equArray[0].AcSensorSpan * sqrt(2.0) / (2047.0 * 16);
    const double DCK = (equArray[0].DcSensorSpan == 0 ? 20 : equArray[0].DcSensorSpan) / (4095.0 * 16);
    int ch = 0;
    double adcRes;

    for(int i = 0; i < CHANNELSIZE; i++){
        if(devInputMode & (0x01U << i))
            adcRes = DCK * pRes[i];
        else
            adcRes = ACK * pRes[i];
        lineArray[i]->setText(QString::number(adcRes, 'f') + tr("mA"));
    }
}

CollectionSetting::~CollectionSetting()
{
    DeviceOperator *deviceOperator = helper->setting_deviceOperator;

    if(deviceOperator->port != NULL){
        deviceOperator->port->close();
        delete deviceOperator->port;
        deviceOperator->port = NULL;
    }
    if(deviceOperator != NULL){
        delete deviceOperator;
        deviceOperator = NULL;
    }
}

void CollectionSetting::test()
{
    static bool flag = false;
    flag = !flag;
    radioArray[0]->setChecked(flag);
}
