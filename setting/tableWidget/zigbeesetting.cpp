#include "zigbeesetting.h"
#include "ui_zigbeesetting.h"

ZigbeeSetting::ZigbeeSetting(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ZigbeeSetting)
{
    ui->setupUi(this);    
    updatePar();
    connect(ui->pushButton_getPar, SIGNAL(pressed()), this, SLOT(getPtr()));
    connect(ui->pushButton_setPar, SIGNAL(pressed()), this, SLOT(setPtr()));

    DeviceOperator *deviceOperator = helper->setting_deviceOperator;
    connect(deviceOperator, SIGNAL(finishedDevConfigSet(bool)),
            this, SLOT(onSetDevConfigFinished(bool)));
    connect(deviceOperator, SIGNAL(sendMsg(QByteArray)),
            this, SLOT(updateSendText(QByteArray)));
    connect(deviceOperator, SIGNAL(recvMsg(QByteArray)),
            this, SLOT(updateRecvText(QByteArray)));
}

void ZigbeeSetting::updatePar()
{
    if(helper->equArray.isEmpty()){
        ui->logshow->setText("请先搜索设备!");
        return;
    }
    QVector<DataGatherConfiguration> &equArray = helper->equArray;
    ui->comboBox_pan->setCurrentText(QString("0x%1").arg(equArray[0].zigbeePanID, 4, 16, QChar('0')));
    ui->comboBox_channel->setCurrentText(QString("%1").arg(equArray[0].zigbeeChannel));
    ui->comboBox_localNetAddr->setCurrentText(QString("0x%1").arg(equArray[0].zigbeeLocalAddress, 4, 16, QChar('0')));
    ui->comboBox_targetNetAddr->setCurrentText(QString("0x%1").arg(equArray[0].zigbeeTargetAddress, 4, 16, QChar('0')));
}

void ZigbeeSetting::updateSendText(QByteArray msg)
{
    QString str =  msg.toHex().toUpper();
    int len = str.length()/2;
    for(int i = 0; i < len; i++){
        str.insert(2*i+i-1, " ");
    }
    ui->logshow->append("[send]>>" + str);
}

void ZigbeeSetting::updateRecvText(QByteArray msg)
{
    QString str = msg.toHex().toUpper();
    int len = str.length()/2;
    for(int i = 0; i < len; i++){
        str.insert(2*i+i-1, " ");
    }
    ui->logshow->append("[recv]<<" + str);
}

void ZigbeeSetting::onSetDevConfigFinished(bool isOK)
{
    if(isOK){
        ui->logshow->append(QString("update success!"));
    }else{
        ui->logshow->append(QString("update faild!"));
    }
}

void ZigbeeSetting::getPar()
{}

void ZigbeeSetting::setPar()
{}

ZigbeeSetting::~ZigbeeSetting()
{
    delete ui;
}
