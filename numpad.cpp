#include "numpad.h"
#include "ui_numpad.h"

NumPad::NumPad(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NumPad)
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    ui->setupUi(this);
    connect(ui->pushButton_0, SIGNAL(clicked()), this, SLOT(getOneNum()));
    connect(ui->pushButton_1, SIGNAL(clicked()), this, SLOT(getOneNum()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(getOneNum()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(getOneNum()));
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(getOneNum()));
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(getOneNum()));
    connect(ui->pushButton_6, SIGNAL(clicked()), this, SLOT(getOneNum()));
    connect(ui->pushButton_7, SIGNAL(clicked()), this, SLOT(getOneNum()));
    connect(ui->pushButton_8, SIGNAL(clicked()), this, SLOT(getOneNum()));
    connect(ui->pushButton_9, SIGNAL(clicked()), this, SLOT(getOneNum()));
    connect(ui->pushButton_e, SIGNAL(clicked()), this, SLOT(enter()));
}

NumPad::~NumPad()
{
    delete ui;
}

void NumPad::getOneNum()
{
    QPushButton *pButton = qobject_cast<QPushButton*>(sender());
    mmm.append(pButton->text());
    ui->lineEdit->setText(mmm);
}

void NumPad::enter()
{
    if(mmm == "000000"){
        done(Accepted);
    }else{
        done(Rejected);
    }
}
