#include "zigbeesetting.h"
#include "ui_zigbeesetting.h"

ZigbeeSetting::ZigbeeSetting(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ZigbeeSetting)
{
    ui->setupUi(this);
//    ui->logshow->clear();

//    connect(ui->pushButton_getPar, SIGNAL(pressed()), this, SLOT(getPar()));
//    connect(ui->pushButton_setPar, SIGNAL(pressed()), this, SLOT(setPar()));

//    initDev();
}

void ZigbeeSetting::initDev()
{
    if(helper->setting_deviceOperator == NULL){
        helper->setting_deviceOperator = new DeviceOperator(NULL);
    }
    deviceOperator = helper->setting_deviceOperator;

    connect(deviceOperator, SIGNAL(sendMsg(QByteArray)),
            this, SLOT(z_updateSendText(QByteArray)));
    connect(deviceOperator, SIGNAL(recvMsg(QByteArray)),
            this, SLOT(z_updateRecvText(QByteArray)));
    connect(deviceOperator, SIGNAL(deviceInformationGot(bool,DataGatherConfiguration)),
            this, SLOT(onGetOneDevCfg(bool,DataGatherConfiguration)));
    connect(deviceOperator, SIGNAL(finishedDevConfigSet(bool)),
            this, SLOT(onSetDevConfigFinished(bool)));
}

void ZigbeeSetting::getU()
{
//    ui->logshow->clear();
//    if(helper->setting_deviceOperator != NULL) {
//        disconnect(helper->setting_deviceOperator);
//        delete helper->setting_deviceOperator;
//        helper->setting_deviceOperator = NULL;
//    }

//    initDev();
}

/************* deviceOperator slot ***************/
void ZigbeeSetting::onGetOneDevCfg(bool fSuccess, DataGatherConfiguration cfg)
{
    if(fSuccess == false){
        ui->logshow->append("获取设备参数失败!");
        return;
    }else{
        ui->logshow->clear();
        ui->logshow->append("成功获取设备参数! ");
        ui->logshow->append(QString("localAddr:%1").arg(cfg.zigbeeLocalAddress, 4, 16, QChar('0')));
        ui->logshow->append(QString("targeAddr:%1").arg(cfg.zigbeeTargetAddress, 4, 16, QChar('0')));
        ui->logshow->append(QString("panId:%1").arg(cfg.zigbeePanID, 4, 16, QChar('0')));
    }

    QVector<DataGatherConfiguration> &equArray = helper->equArray;
    equArray[0] = cfg;
    ui->comboBox_pan->setCurrentText(QString("0x%1").arg(cfg.zigbeePanID, 4, 16, QChar('0')));
    ui->comboBox_channel->setCurrentText(QString("%1").arg(cfg.zigbeeChannel));
    ui->comboBox_localNetAddr->setCurrentText(QString("0x%1").arg(cfg.zigbeeLocalAddress, 4, 16, QChar('0')));
    ui->comboBox_targetNetAddr->setCurrentText(QString("0x%1").arg(cfg.zigbeeTargetAddress, 4, 16, QChar('0')));
}

void ZigbeeSetting::onSetDevConfigFinished(bool isOK)
{
    if(isOK){
        ui->logshow->append(QString("update success!"));
    }else{
        ui->logshow->append(QString("update faild!"));
    }
}
void ZigbeeSetting::z_updateSendText(QByteArray msg)
{
    QString str =  msg.toHex().toUpper();
    int len = str.length()/2;
    for(int i = 0; i < len; i++){
        str.insert(2*i+i-1, " ");
    }
    ui->logshow->append("[send]>>" + str);
}

void ZigbeeSetting::z_updateRecvText(QByteArray msg)
{
    QString str = msg.toHex().toUpper();
    int len = str.length()/2;
    for(int i = 0; i < len; i++){
        str.insert(2*i+i-1, " ");
    }
    ui->logshow->append("[recv]<<" + str);
}


/************* btn slot *******************/
void ZigbeeSetting::getPar()
{
    if(helper->equArray.isEmpty() ||
            helper->setting_deviceOperator == NULL  ||
            helper->setting_deviceOperator->port == NULL ||
            !helper->setting_deviceOperator->port->isOpen()){
        ui->logshow->setText("请先搜索设备!");
        ui->logshow->append(QString("equArray.size:%1").arg(helper->equArray.size()));
        return;
    }

    ui->logshow->append("开始获取设备信息");
    helper->setting_deviceOperator->getDeviceInfo(helper->equArray[0].devID);
}

void ZigbeeSetting::setPar()
{
    if(helper->equArray.isEmpty() ||
            !helper->setting_deviceOperator->port->isOpen()){
        ui->logshow->setText("请先搜索设备!");
        return;
    }

    ui->logshow->append("开始设置设备信息!");
    DataGatherConfiguration newCfg = helper->equArray[0];
    newCfg.zigbeePanID      = ui->comboBox_pan->currentText().remove(0,2).toInt(NULL, 16);
    newCfg.zigbeeChannel    = ui->comboBox_channel->currentText().toInt();
    newCfg.zigbeeLocalAddress   = ui->comboBox_localNetAddr->currentText().remove(0,2).toInt(NULL, 16);
    newCfg.zigbeeTargetAddress  = ui->comboBox_targetNetAddr->currentText().remove(0,2).toInt(NULL, 16);
    helper->setting_deviceOperator->setDeviceConfig(helper->equArray[0], newCfg);
}

ZigbeeSetting::~ZigbeeSetting()
{
    delete ui;
}
