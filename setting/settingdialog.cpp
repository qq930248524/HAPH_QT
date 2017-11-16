#include "settingdialog.h"

#include <QTextEdit>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QDebug>

SettingDialog::SettingDialog(QWidget *par)
    :QDialog(par)
{
//////////////////////////////  title   //////////////////////////////////////
    QLabel  *titleLabel = new QLabel("     环保治理设施设备行为数据设置", this);
    titleLabel->setStyleSheet("background-color:green;"
                              "font: bold 30px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFixedHeight(titleSize);

    homeButton = new QPushButton("Home", titleLabel);
    homeButton->setAttribute(Qt::WA_TranslucentBackground);
    homeButton->setFixedHeight(titleSize);
    homeButton->setFocusPolicy(Qt::NoFocus);
    homeButton->setStyleSheet("font:  20px;");
    connect(homeButton, SIGNAL(pressed()), this, SLOT(close()));

/////////////////////////////////////   tableView   ///////////////////////////////////////

    objArray.append(new CollectionSetting());
    objArray.append(new ZigbeeSetting());
    objArray.append(new SystemSetting());
    DMSNavigation *navi = new DMSNavigation();
    navi->addTab(objArray[1], "ZigBee设置");
    navi->addTab(objArray[0], "数据采集设置");
    navi->addTab(objArray[2], "系统设置");
    connect(navi, SIGNAL(currentChanged(int)), this, SLOT(changedTO(int)));

    navi->setHorizontalAlignment(navi->AlignLeft);
    navi->resize(800, 70);
    navi->show();
    navi->setFixedHeight(420);

///////////////////////////////////     mainLayout  //////////////////////////////////////////////
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(titleLabel,1);
    mainLayout->addWidget(navi,10);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    setLayout(mainLayout);
    this->setGeometry(0,0,800,480);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
}

void SettingDialog::changedTO(int index)
{
    switch (index) {
    case 0:
        ((CollectionSetting *)objArray[0])->getU();
        break;
    case 1:
        ((ZigbeeSetting *)objArray[1])->getU();
        break;
    default:
        break;
    }
}

SettingDialog:: ~SettingDialog()
{
    for(int i = 0; i < objArray.size(); i++){
        delete objArray[i];
        objArray[i] = NULL;
    }
    objArray.clear();
}
