#include "mainwindow.h"
#include "setting/settingdialog.h"
#include "timewidget.h"
#include "helper/QGauge/qgauge.h"

#include <QFrame>
#include <QLayout>

#include <QDebug>

Helper *helper  = NULL;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    helper = new Helper();
    connect(helper->deviceOperator, SIGNAL(deviceADCResultGot(int, uint16_t*)),
            this, SLOT(recvADCResult(int, uint16_t *)));

    flushWidgets(0);//get lasted grid
    showWidget();//show grid widget

    if(helper->checkSerial()){
        timeId = startTimer(helper->TIME_OUT, Qt::VeryCoarseTimer);
    }

    Module module = helper->dasConfig->dasData.enterprise.Modules[0];

    setGeometry(0,0,800,480);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if(!helper->checkSerial()){
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
            //gridWidget[i] = new PassWidget(mode, i);
            QGauge *gauge = new QGauge();

            QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            sizePolicy.setHorizontalStretch(0);
            sizePolicy.setVerticalStretch(0);
            //sizePolicy.setHeightForWidth(gauge->sizePolicy().hasHeightForWidth());
            gauge->setSizePolicy(sizePolicy);
            gauge->setFixedWidth(120);
            gauge->setFixedHeight(120);

            gauge->setValue(24);
            gauge->setLabel("thsi si label");
            gauge->setUnits("This is unit!");

            QFont font;
            font.setFamily(QStringLiteral("Ubuntu"));
            font.setPointSize(5);
            gauge->setFont(font);

            gridWidget[i] =  gauge;
        }
    }else{
        log->append(QString("equArray.size() = %1").arg(helper->equArray.size()));
        helper->deviceOperator->getDeviceADCRes(helper->equArray[mode].devID);
    }
}

void MainWindow::recvADCResult(int devId, uint16_t *pRef)
{
    log->append("recv one ADC data!  ");
    for(int i = 0; i< gridWidget.size(); i++){
        //((PassWidget *)gridWidget[i])->flush(devId, pRef[i], i);
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
    frame->setStyleSheet(QStringLiteral("background-color: rgb(56, 56, 56);"));
/*---------------------------------------------------*/
    log = new QTextEdit();
    log->setReadOnly(true);

    setting = new QPushButton();
    QIcon icon(":/myPic/setting.png");
    setting->setIcon(icon);
    setting->setIconSize(QSize(80,80));
    connect(setting, SIGNAL(clicked(bool)), this, SLOT(startSet()));

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(log);
    bottomLayout->addWidget(setting);
    bottomLayout->addWidget(new timeWidget());
/*---------------------------------------------------*/
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(frame);
    mainLayout->addLayout(bottomLayout);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    QWidget *mainwidget = new QWidget();
    mainwidget->setLayout(mainLayout);
    this->setCentralWidget(mainwidget);
}

void MainWindow::startSet()
{
    if(helper->deviceOperator != NULL && helper->deviceOperator->port->isOpen()){
        killTimer(timeId);
    }

    SettingDialog *setting = new SettingDialog();
    this->hide();
    setting->show();
    setting->exec();

    log->clear();
    if(helper->checkSerial()){
        timeId = startTimer(helper->TIME_OUT, Qt::VeryCoarseTimer);
    }

    delete setting;
    this->show();
}

MainWindow::~MainWindow()
{
}
