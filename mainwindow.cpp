
#include "mainwindow.h"
#include "timewidget.h"
#include "helper/QGauge/qgauge.h"
#include "setting/settingdialog.h"
#include "numpad.h"
#include <QDateTime>

#include <QFrame>
#include <QLayout>
#include <QDebug>

Helper *helper  = NULL;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    helper = new Helper();
    connect(helper->deviceOperator, SIGNAL(deviceADCResultGot(int, int32_t*)),
            this, SLOT(recvADCResult(int, int32_t *)));
    connect(helper->mqttOperator->client, SIGNAL(connected()),
            this, SLOT(mqttConnectted()));
    connect(helper->mqttOperator->client, SIGNAL(disconnected()),
            this, SLOT(mqttDisConnectted()));

    showWidget();//show grid widget
    checkInternet();
    checkSerial();
    checkMqtt();

    DasData dasData = helper->dasConfig->dasData;
    if(helper->checkSerial()){
        log->append("serial open success!");
        log->append("start run timer!");
        timeId = startTimer(dasData.QueryDelay.toInt(), Qt::VeryCoarseTimer);
    }else{
        log->append("serial open error!");
        log->append(QString("comName:%1 baund:%2")
                    .arg(dasData.comName)
                    .arg(dasData.BaudRate));
    }

    setGeometry(0,0,800,480);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    log->append("======= timeEvent ======");
    if(!helper->checkSerial()){
        log->append("serial open error!");
        killTimer(timeId);
        return;
    }

    DasData dasData = helper->dasConfig->dasData;
    QVector<Module> modeArr = dasData.enterprise.Modules;

    //zigbee
    int hostId = 0;
    int slaveId = 0;

    //modbus
    if(dasData.UseZigBee){
        QString host = dasData.enterprise.Modules[modeNum++].ZigBeeId;
        QString slave = dasData.enterprise.Modules[modeNum++].ZigBeeId;
        hostId |= host.mid(2,2).toInt()<<8;
        hostId |= host.mid(4,2).toInt();
        slaveId |= slave.mid(2,2).toInt()<<8;
        slaveId |= slave.mid(4,2).toInt();
    }else{
        hostId = 0;
        slaveId = dasData.enterprise.Modules[modeNum++].Id;
    }

    helper->deviceOperator->getDeviceADCRes(hostId, slaveId);
    if(modeNum == modeArr.size()){
        modeNum = 0;
    }
}

void MainWindow::recvADCResult(int devId, int32_t *pRef)
{
    log->append(QString("======= recvADCResult %1 ======").arg(devId));
    for(int i = 0; i< CHANNELSIZE; i++){
        QGauge *oneGauge = (QGauge *)gridWidget[i];
        oneGauge->setLabel(QString("%1-%2").
                           arg(helper->dasConfig->getModeNumByDevid(devId)+1).
                           arg(i+1));
        oneGauge->setUnits("");
        oneGauge->setValue(0);
        oneGauge->setDigitCount(1);
        oneGauge->setMaxValue(100);
        oneGauge->setMinValue(0);
        oneGauge->setThreshold(90);
    }

    int modeNum = helper->dasConfig->getModeNumByDevid(devId);    
    if(modeNum == -1 ){
        qDebug() << "[UI] mode" << modeNum << " ofLine.";
        goto Ext;
    }

    {
        Module oneModule = helper->dasConfig->dasData.enterprise.Modules[modeNum];

        int32_t *newVal = new int32_t[MB_REG_ADCRES_LENGTH];
        for (int t = 0; t < CHANNELSIZE; t++){
            newVal[t] = -1;
        }
        if(pRef != NULL){
            foreach (Channel onechannel, oneModule.Channels) {
                newVal[onechannel.Id] = pRef[onechannel.Id - 1];
            }
        }

        //dasconfig mode and channel 编号从1开始
        for(int i = 0; i < CHANNELSIZE; i++)
        {
            QGauge *oneGauge = (QGauge *)gridWidget[i];
            oneGauge->setMinValue(0);
            oneGauge->setMaxValue(100);
            oneGauge->setValue(0);
            oneGauge->setUnits("--");
            oneGauge->setDigitCount(0);

            int channelId = -1;
            if(( channelId = oneModule.getChannelIdByIndex(i+1)) == -1){
                continue;
            }
            Channel oneChannel = oneModule.Channels[channelId];

            //数据采集器采样原始电流换算公式如下：
            //  1、DC（0~20mA）：数据采集器采用通道上传数据为DC_Data，
            //      采样实际电流I实际=（DC_Data*20）/（4096*16）；
            //  2、AC（0~20mA）：数据采集器采用通道上传数据为AC_Data，
            //      采样实际电流I实际=（AC_Data*20*sqrt(2.0)）/（2047*16）；
            //  3、AC（0~100mA）：数据采集器采用通道上传数据为AC_Data，
            //      采样实际电流I实际=（AC_Data*100*sqrt(2.0)）/（2047*16）；
            //  Iout实际=(Iin实际-Iinmin )*(Ioutmax-Ioutmin)/( Iinmax-Iinmin)+ Ioutmin

            double Iin;
            if(oneChannel.ACOrDC == "DC"){
                Iin = (newVal[oneChannel.Id-1]*20)/(4096*16);
            }else{
                Iin = (newVal[oneChannel.Id-1]*20*sqrt(2.0))/(2047*16);
            }
            Iin = Iin<oneChannel.InputValueMin ? oneChannel.InputValueMin:Iin;

            double Iout;
            Iout = (Iin - oneChannel.InputValueMin)
                    *(oneChannel.OutputValueMax-oneChannel.OutputValueMin)
                    /(oneChannel.InputValueMax-oneChannel.InputValueMin)
                    +oneChannel.OutputValueMin;

            if(isOriginal){                
                oneGauge->setMinValue(oneChannel.OutputValueMin);
                oneGauge->setMaxValue(oneChannel.OutputValueMax);
                oneGauge->setThreshold(oneChannel.OutputValueMax*0.9);
                oneGauge->setUnits(oneChannel.DataUnit);
                oneGauge->setValue(Iout);
            }else{
                oneGauge->setMinValue(oneChannel.InputValueMin);
                oneGauge->setMaxValue(oneChannel.InputValueMax);
                oneGauge->setThreshold(oneChannel.InputValueMax*0.9);
                oneGauge->setUnits(oneChannel.DataUnit);
                oneGauge->setValue(Iin);
            }
            qDebug() << "[UI] Iin = "<< Iin << " Iout = " << Iout;

        }

        // MQTT sendData
        // don't care data format
        DasData dasData = helper->dasConfig->dasData;
        QString current_date_time = QDateTime::currentDateTime().toString(" yyyy-MM-dd hh:mm:ss");
        QString msg;
        msg.append(current_date_time);
        for(int i = 0; i < CHANNELSIZE; i++){
            msg.append(QString(",%1-%2-%3:%4")
                       .arg(dasData.enterprise.DeviceId, 3, 10)
                       .arg(devId)
                       .arg(i+1)
                       .arg(newVal[i]));
        }
        if(helper->mqttOperator->sendData(msg) == false){
            checkInternet();
            checkMqtt();
        }

        delete newVal;
        newVal = NULL;
    }

Ext:
    if(pRef != NULL){
        delete pRef;
    }
    return;
}

void MainWindow::showWidget()
{
    gridWidget.resize(CHANNELSIZE);
    for(int i = 0; i < CHANNELSIZE; i++){
        QGauge *gauge = new QGauge();

        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        gauge->setSizePolicy(sizePolicy);
        gauge->setMinimumSize(120, 120);

        gauge->setLabel(QString("%1-%2").arg(modeNum+1).arg(i+1));
        gauge->setUnits("");

        gauge->setMaxValue(100);
        gauge->setMinValue(0);
        gauge->setThreshold(90);
        gauge->setValue(0);

        QFont font;
        font.setFamily(QStringLiteral("Ubuntu"));
        font.setPointSize(11);
        gauge->setFont(font);

        gridWidget[i] =  gauge;
    }

    gridLayout = new QGridLayout();
    for(int i = 0; i < gridWidget.size(); i++){
        gridLayout->addWidget(gridWidget.at(i),i/ROW, i%ROW, Qt::AlignCenter);
    }

    /*---------  right frame  --------*/
    QFrame *rightFrame = new QFrame();
    rightFrame->setFixedWidth(80);

    btn_internet   = new QPushButton("网络");
    btn_serial     = new QPushButton("串口");
    btn_mqtt       = new QPushButton("MQTT");
    btn_data       = new QPushButton("原始数据");
    btn_full       = new QPushButton("全屏");

    connect(btn_internet,   SIGNAL(pressed()), this, SLOT(checkInternet()));
    connect(btn_serial,     SIGNAL(pressed()), this, SLOT(checkSerial()));
    connect(btn_mqtt,       SIGNAL(pressed()), this, SLOT(checkMqtt()));
    connect(btn_data,       SIGNAL(pressed()), this, SLOT(conversionData()));
    connect(btn_full,       SIGNAL(pressed()), this, SLOT(switchFullScreen()));
    btn_mqtt->setStyleSheet(ofStr);
    btn_data->setStyleSheet(ofStr);
    btn_full->setStyleSheet(ofStr);

    QVBoxLayout *rightLayout    = new QVBoxLayout();
    rightLayout->addWidget(btn_internet);
    rightLayout->addWidget(btn_serial);
    rightLayout->addWidget(btn_mqtt);
    rightLayout->addWidget(btn_data);
    rightLayout->addWidget(btn_full);
    rightFrame->setLayout(rightLayout);
    rightFrame->setFrameStyle(QFrame::WinPanel|QFrame::Raised);

    /*****************  top Main  ***************/
    QHBoxLayout *topLayout      = new QHBoxLayout();
    topLayout->addLayout(gridLayout);
    topLayout->addWidget(rightFrame);

    QFrame *topFrame = new QFrame();
    topFrame->setLayout(topLayout);
    topFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
    topFrame->setStyleSheet(QStringLiteral("background-color: rgb(56, 56, 56);"));

    /*---------------  bottom widget  --------------*/
    log = new QTextEdit();
    log->setReadOnly(true);

    setting = new QPushButton();
    setting->setStyleSheet("border-image:url(:/myPic/setting.png);"
                           " min-width:90px;"
                           "min-height:90px;");
    setting->setFocusPolicy(Qt::NoFocus);
    connect(setting, SIGNAL(pressed()), this, SLOT(startSet()));

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(log);
    bottomLayout->addWidget(setting);
    bottomLayout->addWidget(new timeWidget(this));

    bottomFrame = new QFrame();
    bottomFrame->setLayout(bottomLayout);
    bottomFrame->setStyleSheet("background-color:gray;border:0;");
    bottomFrame->setFixedHeight(110);

    /*-------------------------  MAIN LAYOUT  --------------------------*/
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(topFrame);
    mainLayout->addWidget(bottomFrame);
    //mainLayout->addLayout(bottomLayout);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    QWidget *mainwidget = new QWidget();
    mainwidget->setLayout(mainLayout);
    this->setCentralWidget(mainwidget);
}

void MainWindow::startSet()
{
    helper->deviceOperator->port->close();
    NumPad numPad(this);
    if(numPad.exec() == QDialog::Accepted){
        settingDialog = new SettingDialog();
        this->hide();
        settingDialog->show();
        settingDialog->exec();
        this->show();
        delete settingDialog;
    }
    helper->initSerial();
}

void MainWindow::switchFullScreen()
{
    QSize gaugeSize;
    if(isFull){
        bottomFrame->hide();
        gaugeSize = QSize(150,150);
        btn_full->setStyleSheet(onStr);
        isFull = false;
    }else{
        bottomFrame->show();
        gaugeSize = QSize(120,120);
        btn_full->setStyleSheet(ofStr);
        isFull = true;
    }

    for(int i = 0; i < gridWidget.size(); i++){
        gridWidget[i]->setMinimumSize(gaugeSize);
    }
}

void MainWindow::checkInternet(){
//    QString cmd("ping -c 3 -i 0.5 www.baidu.com");
//    if(!system(cmd.toLatin1().data())){
//        btn_internet->setStyleSheet(onStr);
//        log->append("[internet] online!");
//        qDebug() << "[internet] online";
//    }else{
//        btn_internet->setStyleSheet(ofStr);
//        log->append("[internet] offline!");
//        qDebug() << "[internet] offline";
//    }
    btn_internet->setStyleSheet(ofStr);
}

void MainWindow::checkSerial(){
    if(helper->checkSerial()){//on
        btn_serial->setStyleSheet(onStr);
        log->append("[serial] open success!");
    }else{//off
        if(helper->initSerial()){//open ok
            btn_serial->setStyleSheet(onStr);
            log->append("[serial] open success!");
        }else{//open failed
            btn_serial->setStyleSheet(ofStr);
            log->append("[serial] open faild!");
        }
    }
}

void MainWindow::checkMqtt(){
    if(helper->checkMqtt()){
        log->append("[MQTT] is connectted! ");
    }else{
        log->append("[MQTT] is closed and will reStart mqtt...");
        helper->initMqtt();
    }
}

void MainWindow::mqttConnectted()
{
    log->append("[MQTT] has connectted. ");
    btn_mqtt->setStyleSheet(onStr);
    helper->mqttOperator->isOnline = true;
}
void MainWindow::mqttDisConnectted()
{
    log->append("[MQTT] has disConnectted. ");
    btn_mqtt->setStyleSheet(ofStr);
    helper->mqttOperator->isOnline = false;
}

void MainWindow::conversionData()
{
    if(isOriginal){
        btn_data->setStyleSheet(ofStr);
    }else{
        btn_data->setStyleSheet(onStr);
    }
    isOriginal = !isOriginal;
}

MainWindow::~MainWindow()
{
}
