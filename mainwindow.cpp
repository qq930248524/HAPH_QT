#include "mainwindow.h"
#include "setting/settingdialog.h"
#include "timewidget.h"

#include <QFrame>
#include <QLayout>

#include <QDebug>

DeviceOperator  *deviceOperator = NULL;
QVector<DataGatherConfiguration>   equArray;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    TIME_OUT    = 5000;

    QSerialPort *serialPort = new QSerialPort();
    serialPort->setPortName("ttyUSB0");
    serialPort->setBaudRate(QSerialPort::Baud9600);
    serialPort->setParity(QSerialPort::EvenParity);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);
    deviceOperator = new DeviceOperator(serialPort);
    serialPort->open(QIODevice::ReadWrite);
    connect(deviceOperator, SIGNAL(deviceADCResultGot(int, uint16_t*)), this, SLOT(recvADCResult(int, uint16_t *)));

    flushWidgets(0);//get lasted grid
    showWidget();//show grid widget

    if(checkSerial()){
        timeId = startTimer(TIME_OUT, Qt::VeryCoarseTimer);
    }

    setGeometry(0,0,800,480);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
}

bool MainWindow::checkSerial()
{
    QString str;
    if(deviceOperator == NULL || deviceOperator->port == NULL || !deviceOperator->port->isOpen()){
        str = "1/please set serialPort and click search device!";
        log->append(str);
        return false;
    }
    if(equArray.size() == 0){
        log->append(str);
        str.append("2/not found any device,please set serialPort and search device!");
        return false;
    }
    modeSize = equArray.size();
    return true;
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if(!checkSerial()){
        killTimer(timeId);
        return;
    }
    flushWidgets(mode++);
    if(mode == modeSize){
        mode = 0;
    }
}

void MainWindow::flushWidgets(int mode)
{
    if(gridWidget.isEmpty()){
        gridWidget.resize(passSize);
        for(int i = 0; i < gridWidget.size(); i++){
            gridWidget[i] = new PassWidget(mode, i);
        }
    }else{
        log->append(QString("equArray.size() = %1").arg(equArray.size()));
        deviceOperator->getDeviceADCRes(equArray[mode].devID);
    }
}

void MainWindow::recvADCResult(int devId, uint16_t *pRef)
{
    log->append("recv one ADC data!  ");
    for(int i = 0; i< gridWidget.size(); i++){
        ((PassWidget *)gridWidget[i])->flush(devId, pRef[i], i);
    }
    delete[] pRef;
}

void MainWindow::showWidget()
{
    gridLayout = new QGridLayout();
    for(int i = 0; i < gridWidget.size(); i++){
        gridLayout->addWidget(gridWidget.at(i),i/ROW, i%ROW, Qt::AlignCenter);
    }
    QFrame *frame = new QFrame();
    frame->setLayout(gridLayout);
    frame->setFrameStyle(QFrame::Panel | QFrame::Raised);

    log = new QTextEdit();
    log->setReadOnly(true);
    setting = new QPushButton();
    QIcon icon(":/myPic/setting.png");
    setting->setIcon(icon);
    setting->setIconSize(QSize(100,100));
    connect(setting, SIGNAL(clicked(bool)), this, SLOT(startSet()));

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(log);
    bottomLayout->addWidget(setting);
    bottomLayout->addWidget(new timeWidget());

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(frame);
    mainLayout->addLayout(bottomLayout);
    /////////////////////////////////////////////////////////////////
    QWidget *mainwidget = new QWidget();
    mainwidget->setLayout(mainLayout);
    this->setCentralWidget(mainwidget);
}

void MainWindow::startSet()
{
    if(deviceOperator != NULL && deviceOperator->port->isOpen()){
        killTimer(timeId);
    }

    SettingDialog *setting = new SettingDialog();
    this->hide();
    setting->show();
    setting->exec();

    log->clear();
    if(checkSerial()){
        timeId = startTimer(TIME_OUT, Qt::VeryCoarseTimer);
    }

    delete setting;
    this->show();
}

MainWindow::~MainWindow()
{

}
