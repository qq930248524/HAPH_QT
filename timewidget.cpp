#include "timewidget.h"

#include <QTimer>
#include <QDateTime>
#include <QVBoxLayout>

timeWidget::timeWidget(QWidget *parent) : QWidget(parent)
{
    dataLcd = new QLCDNumber();
    dataLcd->setDigitCount(10);
    dataLcd->setMode(QLCDNumber::Dec);
    dataLcd->setSegmentStyle(QLCDNumber::Flat);
    //dataLcd->setStyleSheet("border: 1px solid green; color: green; background: silver;");

    timeLcd = new QLCDNumber();
    timeLcd->setDigitCount(8);
    timeLcd->setMode(QLCDNumber::Dec);
    timeLcd->setSegmentStyle(QLCDNumber::Flat);
    //timeLcd->setStyleSheet("border: 1px solid green; color: green; background: silver;");
    this->setStyleSheet("border: 1px solid green; color: green; background: silver;");


    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(dataLcd);
    mainLayout->addWidget(timeLcd);

    QTimer *pTimer = new QTimer(this);
    pTimer->setInterval(1000);
    connect(pTimer, SIGNAL(timeout()), this, SLOT(onTimeOut()));
    pTimer->start();

    setLayout(mainLayout);
}

void timeWidget::onTimeOut()
{
    QDateTime dataTime = QDateTime::currentDateTime();
    dataLcd->display(dataTime.toString("yyyy-MM-dd"));
    timeLcd->display(dataTime.toString("HH:mm:ss"));
}
