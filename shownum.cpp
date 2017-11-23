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
