#include "print.h"
#include "ui_print.h"

Print::Print(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Print)
{
    ui->setupUi(this);
    initUI();
}

Print::~Print()
{
    delete ui;
}

void Print::initUI()
{
    ui->name->setText(QString("企业名称: ") + helper->dasConfig->dasData.enterprise.Name);
    ui->serialNo->setText(QString("企业编码: ") + helper->dasConfig->dasData.enterprise.SerialNo);
    ui->id->setText(QString("设备编号: ") + helper->dasConfig->dasData.enterprise.Id);
    ui->deviceId->setText(QString("企业编号: ") + helper->dasConfig->dasData.enterprise.DeviceId);

    ui->modNum->clear();
    foreach (Module oneModule, helper->dasConfig->dasData.enterprise.Modules) {
        ui->modNum->addItem(QString("%1").arg(oneModule.Id));
    }
    ui->modNum->setCurrentIndex(0);

    connect(ui->modNum, SIGNAL(activated(int)), this, SLOT(on_comMode(int)));
    connect(ui->channelNum, SIGNAL(activated(int)), this, SLOT(on_comChannel(int)));
    connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(on_btnPrint()));
}

void Print::on_comMode(int index)
{
    Module oneModule = helper->dasConfig->dasData.enterprise.Modules[index];
    ui->channelNum->clear();
    foreach (Channel oneChannel, oneModule.Channels) {
        ui->channelNum->addItem(QString("%1").arg(oneChannel.Id));
    }
    ui->channelNum->setCurrentIndex(0);
    ui->log->clear();
    ui->log->append(QString("模块 ID: %1").arg(oneModule.Id));
    ui->log->append(QString("模块 ZigbeeId: %1").arg(oneModule.ZigBeeId));
}

void Print::on_comChannel(int index)
{
    Channel oneChannel = helper->dasConfig->dasData.enterprise.Modules[ui->modNum->currentIndex()].Channels[index];
    ui->log->append(QString("通道 ID: %1").arg(oneChannel.Id));
    ui->log->append(QString(" NAME: %1").arg(oneChannel.Name));
    ui->log->append(QString(" InputValueMin: %1").arg(oneChannel.InputValueMin));
    ui->log->append(QString(" InputValueMax: %1").arg(oneChannel.InputValueMax));
    ui->log->append(QString(" OutputValueMin: %1").arg(oneChannel.OutputValueMin));
    ui->log->append(QString(" OutputValueMax: %1").arg(oneChannel.OutputValueMax));
    ui->log->append(QString(" ACOrDC: %1").arg(oneChannel.ACOrDC));
    ui->log->append(QString(" DataUnit: %1").arg(oneChannel.DataUnit));
}

// 打印DC报表
void Print::on_btnPrint()
{
    if(ui->modNum->currentIndex() == -1 || ui->channelNum->currentIndex() == -1){
        ui->log->append("请选择模块和通道！ ");
        return;
    }
    //不打印DC数据
    int moduleIndex     = ui->modNum->currentIndex();
    int channelIndex    = ui->channelNum->currentIndex();
    Module  oneModule   = helper->dasConfig->dasData.enterprise.Modules[moduleIndex];
    Channel oneChannel  = oneModule.Channels[channelIndex];
    if(oneChannel.ACOrDC == "DC"){      //不统计直流
        ui->log->append("当前通道为DC直流，不需要统计开停。");
        return;
    }

    //判断时间合法性、文件正确性
    QDateTime startDate(ui->startDate->date(), QTime(0, 0, 0));
    QDateTime endDate(ui->endDate->date(), QTime(23, 59, 59));
    if(startDate > endDate){
        ui->log->append("请重新选择时间！ ");
    }
    QString fileName = chanlStateDir +
            QString("/%1-%2")
            .arg(ui->modNum->currentText())
            .arg(ui->channelNum->currentText());
    QFile channelFile(fileName);
    if(!channelFile.exists()){
        ui->log->append("本地没有通道状态文件！ fileName:" + fileName);
    }
    if(!channelFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        ui->log->append("本地通道状态文件打开失败！ fileName:" + fileName);
    }

    //开始读取数据
    QTextStream out(&channelFile);
    bool isGoOn = true;
    bool chanelState = true;
    QDateTime closeTime;
    QDateTime itemTime;
    long long allTime = 0;
    while (!out.atEnd()) {
        QStringList oneItem_list = out.readLine().remove('\n').split('@');
        itemTime = QDateTime::fromString(oneItem_list[0].simplified(), "yyyy-MM-dd hh:mm:ss");
        int val     = oneItem_list[1].toInt();  //1=true 0=false

        if(startDate <= itemTime && endDate >= itemTime){
            isGoOn = false;
            //check()
            if(val != chanelState){ //状态改变了，计算时间
                ui->log->append(QString("%1 @%2").arg(oneItem_list[0]).arg(QString(oneItem_list[1])));
                if(val == false){ //从通道关闭开始计时
                    closeTime = itemTime;
                }
                if(val == true && chanelState == false){//通道状态由关到开，算是一个计时
                    allTime += closeTime.secsTo(itemTime);
                }
                chanelState = val;
            }
        }else if(isGoOn == false){//跳出时间范围就不在读取内容
            break;
        }
    }//end while
    if(chanelState == false){//如果最后状态为关，则加上剩余时间
        allTime += itemTime.secsTo(QDateTime(itemTime.date().addDays(1), QTime(0,0,0)));
    }
    channelFile.close();
    ui->log->append(QString("allTime:%1 s").arg(allTime));
}

