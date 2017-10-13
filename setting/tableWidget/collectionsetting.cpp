#include "collectionsetting.h"
//#include "mainwindow.h"

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
    if(helper->deviceOperator != NULL && helper->deviceOperator->port->isOpen()){
        onSearchDeviceFinished();
    }
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
    modNumBox   = new QComboBox();
    checkDigitBox   = new QComboBox();
    baudRateBox    = new QComboBox();
    modNumBox->setFixedWidth(160);
    checkDigitBox->setFixedWidth(160);
    baudRateBox->setFixedWidth(160);
    modNumBox->setMaxVisibleItems(5) ;
    checkDigitBox->setMaxVisibleItems(2) ;
    baudRateBox->setMaxVisibleItems(10) ;
    checkDigitBox->addItems(QStringList()<<"偶校验"<<"奇校验");
    baudRateBox->addItems(QStringList()<<"300"<<"600"<<"1200"<<"4800"<<"9600"<<"19200"<<"38400"<<"56000"<<"115200");
    baudRateBox->setCurrentIndex(4);

    QHBoxLayout *hLayout1 = new QHBoxLayout();
    hLayout1->addWidget(new QLabel("模块编号:"), 1, Qt::AlignRight);
    hLayout1->addWidget(modNumBox);
    connect(modNumBox, SIGNAL(activated(QString)), this, SLOT(updateCheckArray(QString)));

    QHBoxLayout *hLayout2 = new QHBoxLayout();
    hLayout2->addWidget(new QLabel("校验位:"), 1, Qt::AlignRight);
    hLayout2->addWidget(checkDigitBox);

    QHBoxLayout *hLayout3 = new QHBoxLayout();
    hLayout3->addWidget(new QLabel("波特率:"), 1, Qt::AlignRight);
    hLayout3->addWidget(baudRateBox);

    logshow            =  new QTextEdit();
    //logshow->document ()->setMaximumBlockCount (5);
    logshow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    logshow->setFixedHeight(250);
    logshow->setReadOnly(true);

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addLayout(hLayout1);
    vLayout->addLayout(hLayout2);
    vLayout->addLayout(hLayout3);
    vLayout->addStretch(1);
    vLayout->addWidget(new QLabel("结果:"));
    vLayout->addWidget(logshow);

    mainLayout->addStretch(1);
    mainLayout->addLayout(vLayout);
}

void CollectionSetting::initButtonArrayUI()
{
    QPushButton *searchModNum   = new QPushButton("搜索模块编号");
    QPushButton *setModNum          = new QPushButton("设置模块编号");
    QPushButton *setPassType           = new QPushButton("设置通道类型");
    QPushButton *readEquPar           = new QPushButton("读取设备参数");
    QPushButton *readPassData            = new QPushButton("读取通道数据");
    QPushButton *readZigbeeData            = new QPushButton("zigbee取数据");

    connect(searchModNum, SIGNAL(clicked(bool)), this, SLOT(searchModNum()));
    connect(setPassType, SIGNAL(clicked(bool)), this, SLOT(setPassType()));
    connect(readEquPar, SIGNAL(clicked(bool)), this, SLOT(readEquPar()));
    connect(readPassData, SIGNAL(clicked(bool)), this, SLOT(readPassData()));
    connect(readZigbeeData, SIGNAL(clicked(bool)), this, SLOT(readZigbeeData()));

    QVBoxLayout *vLayout    = new QVBoxLayout();
    vLayout->addWidget(searchModNum, 3, Qt::AlignTop);
    vLayout->addWidget(setModNum, 3, Qt::AlignTop);
    vLayout->addWidget(setPassType, 3, Qt::AlignTop);
    vLayout->addStretch(3);
    vLayout->addWidget(readEquPar, 3, Qt::AlignTop);
    vLayout->addWidget(readPassData, 3, Qt::AlignTop);
    vLayout->addWidget(readZigbeeData, 3, Qt::AlignTop);

//    setStyleSheet("QPushButton {"
//                  "Background:rgb(110, 190, 10);"
//                  "color:white;"
//                  "font:bold;"
//                  "font-size:30px;"
//                  "font-weight:90px;"
//                  "border-width:90px;"
//                  "border-radius:5px;}");
    setStyleSheet("QPushButton {"
                  "border: 1px solid #000000;border-radius: 5px;"
                  "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                  "stop: 0 #dedede, "
                  "stop: 0.5 #434343,"
                  "stop: 0.51 #000000, "
                  "stop: 1 #656a6d);"
                  "color: #FFFFFF;"
                  "font: bold 20px;"
                  "min-width: 90px;"
                  "min-height: 50px}"

                  "QPushButton:pressed {"
                  "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                  "stop: 0 #cfcccc, stop: 0.5 #333232,"
                  "stop: 0.51 #000000, stop: 1 #585c5f);"
                  "color: #00CC00;}"
                  "QPushButton:flat {"
                  "border: none;}");

    QFrame *frame = new QFrame();
    frame->setLayout(vLayout);
    frame->setFrameStyle(QFrame::Panel|QFrame::Raised);
    mainLayout->addStretch(1);
    mainLayout->addWidget(frame);
}
void CollectionSetting::initDev()
{
    connect(helper->deviceOperator, SIGNAL(deviceInformationGot(bool,DataGatherConfiguration)), this, SLOT(onGotDevInfo(bool,DataGatherConfiguration)));
    connect(helper->deviceOperator, SIGNAL(finishedDevSearching()), this, SLOT(onSearchDeviceFinished()));
    connect(helper->deviceOperator, SIGNAL(finishedDevCalibrate(bool)), this, SLOT(onCalibrateDeviceFinished(bool)));
    connect(helper->deviceOperator, SIGNAL(finishedDevConfigSet(bool)), this, SLOT(onSetDevConfigFinished(bool)));

    connect(helper->deviceOperator,SIGNAL(sendMsg(QByteArray)), this, SLOT(updateSendText(QByteArray)));
    connect(helper->deviceOperator, SIGNAL(recvMsg(QByteArray)), this, SLOT(updateRecvText(QByteArray)));

    connect(helper->deviceOperator, SIGNAL(test(QString)),    this, SLOT(test(QString)));

}

/*************** btn SLOT************************/
void    CollectionSetting::searchModNum()
{
    QSerialPort *serialPort = helper->deviceOperator->port;
    if(serialPort->isOpen()){
        logshow->append("串口已经打开!");
    }else{
        serialPort->setBaudRate(baudRateBox->currentText().toInt());
        serialPort->setParity(checkDigitBox->currentIndex() == 0?QSerialPort::EvenParity:QSerialPort::OddParity);
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
    }
    helper->equArray.clear();
    helper->deviceOperator->searchDevices();
}
void    CollectionSetting::setModNum(){}
void    CollectionSetting::setPassType()
{
    DataGatherConfiguration newCfg = helper->equArray[modNumBox->currentIndex()];
    newCfg.inputMode = 0;
    for(int i = 0; i < CHANNELSIZE; i++){
        if(radioArray[i]->getChecked()){
            newCfg.inputMode |=( 0x01U << i);
        }
    }
    helper->deviceOperator->setDeviceConfig(helper->equArray[modNumBox->currentIndex()], newCfg);
}

void    CollectionSetting::readEquPar(){}
void    CollectionSetting::readPassData(){
    helper->deviceOperator->getDeviceADCRes(modNumBox->currentData().toInt());
    qDebug() << "===================" << modNumBox->currentData().toInt() << endl;
}
void    CollectionSetting::readZigbeeData(){}




/*********** devOperator SLOT ******************/
void    CollectionSetting::onGotDevInfo(bool isok,DataGatherConfiguration cfg)
{
    if(!isok){
        return;
    }
    for(int i =0; i < helper->equArray.size();i++){
        if(cfg.devID == helper->equArray[0].devID){
            return;
        }        
    }
    logshow->append(QString("get one dev:devID=%1").arg(cfg.devID));
    helper->equArray.append(cfg);
}
void    CollectionSetting::onSearchDeviceFinished()
{
    if(helper->equArray.size() == 0){
        return;
    }

    modNumBox->setMaxCount(helper->equArray.size());
    for(int i = 0; i < helper->equArray.size(); i++){
        modNumBox->addItem(QString("%1").arg(helper->equArray[i].devID));
    }
    updateCheckArray(modNumBox->currentText());
}
void    CollectionSetting::onSetDevConfigFinished(bool isok)
{
    if(isok){
        logshow->append(QString("update success!"));
    }else{
         logshow->append(QString("update faild!"));
    }
}

void    CollectionSetting::updateSendText(QByteArray msg){
    QString str =  msg.toHex().toUpper();
    int len = str.length()/2;
    for(int i = 0; i < len; i++){
        str.insert(2*i+i-1, " ");
    }
    logshow->append("[send]>>" + str);
}
void    CollectionSetting::updateRecvText(QByteArray msg){
    QString str = msg.toHex().toUpper();
    int len = str.length()/2;
    for(int i = 0; i < len; i++){
        str.insert(2*i+i-1, " ");
    }
    logshow->append("[recv]<<" + str);
}

void    CollectionSetting::test(QString str){
    qDebug() << "========================================----" << str << endl;
}

////////////////////////////////////////////////////////////////
void CollectionSetting::updateCheckArray(QString str)
{
    qDebug() << "equArray.size = " << helper->equArray.size() << endl;
    qDebug() << "radioArray.size = " << radioArray.size() << endl;
    qDebug()<<"str=" << str << endl;
    DataGatherConfiguration curCfg = helper->equArray[str.toInt() -1];
    for(int i = 0; i < CHANNELSIZE; i++){
        radioArray[i]->setChecked( curCfg.inputMode & (0x01U << i));
        //logshow->append(curCfg.inputMode& (0x01U << i) ? "1":"0");    //打印checkArray
    }
    connect(helper->deviceOperator, SIGNAL(deviceADCResultGot(int,uint16_t*)), this, SLOT(updateADCLine(int, uint16_t *)));
    helper->deviceOperator->getDeviceADCRes(helper->equArray[modNumBox->currentIndex()].devID);
}

void CollectionSetting::updateADCLine(int devId, uint16_t *pRes)
{
    const uint16_t devInputMode = helper->equArray[modNumBox->currentIndex()].inputMode;
    const double ACK = helper->equArray[modNumBox->currentIndex()].AcSensorSpan * sqrt(2.0) / (2047.0 * 16);
    const double DCK = (helper->equArray[modNumBox->currentIndex()].DcSensorSpan == 0 ? 20 : helper->equArray[modNumBox->currentIndex()].DcSensorSpan) / (4095.0 * 16);
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
}
