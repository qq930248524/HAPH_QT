#include "shownum.h"
#include "ui_shownum.h"

ShowNum::ShowNum(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ShowNum)
{
    ui->setupUi(this);
}

ShowNum::~ShowNum()
{
    delete ui;
}

void ShowNum::setTitle(QString str)
{
    ui->title->setText(str);
}
void ShowNum::setName(QString str)
{
    ui->name->setText(str);
}
void ShowNum::setUnit(QString str)
{
    ui->unit->setText(str);
}
void ShowNum::setValue(double val)
{
    ui->number->display(val);
}

/*
 * par: st 1=on 0=off -1=hidden
 *
 */
void ShowNum::setOn(int st)
{
    switch (st) {
    case 1:
        ui->onoff->show();
        ui->onoff->setText("开");
        break;
    case 0:
        ui->onoff->show();
        ui->onoff->setText("关");
    case -1:
        ui->onoff->hide();
    default:
        break;
    }
}
