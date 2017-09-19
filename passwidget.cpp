#include "passwidget.h"
#include "mainwindow.h"

#include <QString>
#include <QLabel>

PassWidget::PassWidget(QWidget *parent) :
    QWidget(parent)
{
    PassWidget(0, 0);
}

PassWidget::PassWidget(int mode, int passNum)
{
    QPalette pal(this->palette());
    pal.setColor(QPalette::Background, Qt::green);
    this->setAutoFillBackground(true);
    this->setPalette(pal);
    this->setMinimumHeight(110);
    this->setMinimumWidth(190);

    T = new QLabel(this);
    C = new QLabel(this);
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(T);
    mainLayout->addWidget(C);
    setLayout(mainLayout);
}

void PassWidget::flush(int devId, uint16_t data, int passNum)
{
    int index = 0;
    while(equArray[index++].devID != devId){}
    index--;
    const uint16_t devInputMode = equArray[index].inputMode;
    const double ACK = equArray[index].AcSensorSpan * sqrt(2.0) / (2047.0 * 16);
    const double DCK = (equArray[index].DcSensorSpan == 0 ? 20 : equArray[index].DcSensorSpan) / (4095.0 * 16);
    int ch = 0;
    double adcRes;

    if(devInputMode & (0x01U << passNum))
        adcRes = DCK * data;
    else
        adcRes = ACK * data;
    T->setText(QString("MODE:%1  PASS:%2").arg(devId).arg(passNum));
    T->setFixedHeight(20);
    C->setText(QString::number(adcRes, 'f') + tr("mA"));
}

PassWidget::~PassWidget()
{

}
