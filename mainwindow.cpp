#include "mainwindow.h"
#include "timewidget.h"
#include "shownum.h"
#include "setting/settingdialog.h"
#include "numpad.h"
#include <QDateTime>

#include <QFrame>
#include <QLayout>
#include <QDebug>
#include <iostream>
#include <QTimer>

Helper *helper  = NULL;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //1.serial mqtt dasconfig
    helper = new Helper();

    //2.ui
    setGeometry(0,0,800,480);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    showWidget();//show grid widget
    switchFullScreen();
    checkInternet();
    checkSerial();
    checkMqtt();

    //3.uiTimer
    if(helper->checkSerial()){
        startTimer(helper->dasConfig->dasData.QueryDelay.toInt(), Qt::VeryCoarseTimer);
    }
    //helper->dataOperator->test();
    QTimer::singleShot(1000*60*60*24, this, SLOT(reboot()));
}
void MainWindow::reboot()
{
    system("reboot");
}

/**************************************************
 * @brief:  object类内部定时器的槽函数
 * @param：  enevt：事件（暂时未使用）
 * @return:
 **************************************************/
void MainWindow::timerEvent(QTimerEvent *event)
{
    qDebug()<<"[UI] ================= timeEvent =========================";
    log->append("======= timeEvent ======");
    int32_t modeData[CHANNELSIZE];
    helper->getModeData(modeNum, &modeData[0]);

    Module oneModule = helper->dasConfig->dasData.enterprise.Modules[modeNum];


    for(int i = 0; i< CHANNELSIZE; i++){
        ShowNum *oneShow = (ShowNum *)gridWidget[i];
        oneShow->setTitle(QString("%1-%2").arg(modeNum+1).arg(i+1));
        oneShow->setName("-- ");
        oneShow->setUnit("-");

        Channel oneChannel;
        int channelIndex = oneModule.getIndexByChannelId(i+1);
        if(channelIndex != -1){//找到了通道
            oneChannel = oneModule.Channels[channelIndex];
            oneShow->setName(oneChannel.Name);
            oneShow->setUnit(oneChannel.DataUnit);
        }

        if(modeData[i] == -1 || channelIndex == -1){
            oneShow->setValue(-1);
            oneShow->setOn(-1);
        }else{            
            //数据采集器采样原始电流换算公式如下： //量程为输入量程
            //  1、DC（0~20mA）：数据采集器采用通道上传数据为DC_Data，
            //      采样实际电流I实际=（DC_Data*20）/（4096*16）；
            //  2、AC（0~20mA）：数据采集器采用通道上传数据为AC_Data，
            //      采样实际电流I实际=（AC_Data*20*sqrt(2.0)）/（2047*16）；
            //  3、AC（0~100mA）：数据采集器采用通道上传数据为AC_Data，
            //      采样实际电流I实际=（AC_Data*100*sqrt(2.0)）/（2047*16）；
            //  Iout实际=(Iin实际-Iinmin )*(Ioutmax-Ioutmin)/( Iinmax-Iinmin)+ Ioutmin

            double Iin;
            if(oneChannel.ACOrDC == "DC"){
                Iin = (double)(modeData[i]*20)/(4096*16.0);
            }else{
                if(oneChannel.InputValueMax == 20){
                    Iin = (double)(modeData[i]*20*sqrt(2.0))/(2047*16.0);
                }else if(oneChannel.InputValueMax == 100){
                    Iin = (double)(modeData[i]*100*sqrt(2.0))/(2047*16.0);
                }
            }
            Iin = Iin<oneChannel.InputValueMin ? oneChannel.InputValueMin:Iin;

            double Iout;
            Iout = (double)(Iin - oneChannel.InputValueMin)
                    *(oneChannel.OutputValueMax-oneChannel.OutputValueMin)
                    /(oneChannel.InputValueMax-oneChannel.InputValueMin)
                    +oneChannel.OutputValueMin;

            if(isOriginal == false){
                oneShow->setValue(Iout);
            }else{
                oneShow->setValue(modeData[i]);
            }

            //set on off label
            if(oneChannel.ACOrDC == "DC"){
                oneShow->setOn(-1);
            }else{
                if(oneChannel.OutputValueMax*0.05 > Iout){
                    oneShow->setOn(0);
                }else{
                    oneShow->setOn(1);
                }
            }
            //qDebug() << "[UI] Iin = "<< Iin << " Iout = " << Iout << "origData = " << modeData[i];
        }
    }
    if(++modeNum == helper->dasConfig->dasData.enterprise.Modules.size()){
        modeNum = 0;
    }
}

/**************************************************
 * @brief:  初始化UI控件，主界面的控件全部在这里初始化
 * @param：
 * @return:
 **************************************************/
void MainWindow::showWidget()
{
    gridWidget.resize(CHANNELSIZE);
    for(int i = 0; i < CHANNELSIZE; i++){
        gridWidget[i] =  new ShowNum(this);
        gridWidget[i]->setFixedSize(150,200);
        ((ShowNum *)gridWidget[i])->setOn(-1);
    }

    gridLayout = new QGridLayout();
    for(int i = 0; i < gridWidget.size(); i++){
        gridLayout->addWidget(gridWidget.at(i),i/ROW, i%ROW, Qt::AlignCenter);
    }
    gridLayout->setHorizontalSpacing(0);
    gridLayout->setVerticalSpacing(0);


    /*---------  right frame  --------*/
    QFrame *rightFrame = new QFrame();
    rightFrame->setFixedWidth(80);

    btn_internet   = new QPushButton("网络");
    btn_serial     = new QPushButton("串口");
    btn_mqtt       = new QPushButton("MQTT");
    btn_data       = new QPushButton("原始数据");
    btn_full        = new QPushButton("全屏");

    connect(btn_internet,   SIGNAL(pressed()), this, SLOT(checkInternet()));
    connect(btn_serial,     SIGNAL(pressed()), this, SLOT(checkSerial()));
    connect(btn_mqtt,       SIGNAL(pressed()), this, SLOT(checkMqtt()));
    connect(btn_data,       SIGNAL(pressed()), this, SLOT(conversionData()));
    connect(btn_full,       SIGNAL(clicked()), this, SLOT(switchFullScreen()));
    btn_internet->setFocusPolicy(Qt::NoFocus);
    btn_serial->setFocusPolicy(Qt::NoFocus);
    btn_mqtt->setFocusPolicy(Qt::NoFocus);
    btn_data->setFocusPolicy(Qt::NoFocus);
    btn_full->setFocusPolicy(Qt::NoFocus);
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
    topLayout->addSpacing(20);
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

/**************************************************
 * @brief:  setting的槽函数，用来启动设置界面
 * @param：
 * @return:
 **************************************************/
void MainWindow::startSet()
{

    NumPad numPad(this);
    if(numPad.exec() == QDialog::Accepted){
        helper->stopRun();
        settingDialog = new SettingDialog();
        this->hide();
        settingDialog->show();

        settingDialog->exec();
        this->show();
        delete settingDialog;
        helper->gotoRun();
    }
}

void MainWindow::switchFullScreen()
{
    QSize gaugeSize;
    if(isFull){
        bottomFrame->hide();
        gaugeSize = QSize(170,155);
        btn_full->setStyleSheet(onStr);
        isFull = false;
    }else{
        bottomFrame->show();
        gaugeSize = QSize(170,115);
        btn_full->setStyleSheet(ofStr);
        isFull = true;
    }

    for(int i = 0; i < gridWidget.size(); i++){
        //gridWidget[i]->setMinimumSize(gaugeSize);
        gridWidget[i]->setFixedSize(gaugeSize);
    }
}

void MainWindow::checkInternet()
{

    connect(&networkManager, SIGNAL(onlineStateChanged(bool)),
                    this, SLOT(networkStatusChanges(bool)));

    if(networkManager.isOnline() == true){
        btn_internet->setStyleSheet(onStr);
        log->append("[internet] online!");
        qDebug() << "[internet] online";
    }else{
        btn_internet->setStyleSheet(ofStr);
        log->append("[internet] offline!");
        qDebug() << "[internet] offline";
    }
}
void MainWindow::checkSerial()
{
    DasData dasData = helper->dasConfig->dasData;
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
void MainWindow::checkMqtt()
{
    connect(helper->mqttOperator->client, SIGNAL(connected()),
            this, SLOT(mqttConnectted()));
    connect(helper->mqttOperator->client, SIGNAL(disconnected()),
            this, SLOT(mqttDisConnectted()));

    if(helper->checkMqtt()){
        log->append("[MQTT] is connectted! ");
    }else{
        log->append("[MQTT] is closed and will reStart mqtt...");
        helper->initMqtt();
    }
}

void MainWindow::networkStatusChanges(bool isOnline)
{
    if(isOnline == true){
        btn_internet->setStyleSheet(onStr);
        log->append("[internet] online!");
        qDebug() << "[internet] online";
    }else{
        btn_internet->setStyleSheet(ofStr);
        log->append("[internet] offline!");
        qDebug() << "[internet] offline";
    }
    if(isOnline){
        helper->initMqtt();
    }else{
        mqttDisConnectted();
    }
}
void MainWindow::mqttConnectted()
{
    log->append("[MQTT] has connectted. ");
    qDebug() << "[MQTT] connectted !!! ";
    btn_mqtt->setStyleSheet(onStr);
    helper->mqttOperator->isOnline = true;
    helper->dataOperator->rePushPendingData_start();
}
void MainWindow::mqttDisConnectted()
{
    log->append("[MQTT] has disConnectted. ");
    qDebug() << "[MQTT] disConnectted! --! ";
    btn_mqtt->setStyleSheet(ofStr);
    helper->mqttOperator->isOnline = false;
    helper->dataOperator->rePushPendingData_stop();
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
