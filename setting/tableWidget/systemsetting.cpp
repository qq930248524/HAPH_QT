#include "systemsetting.h"
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

/*
 * ------------------------------------
 *  00  |   01
 *  02  |   03
 *  04  |   05
 *  06  |   07
 *  08  |   09
 *  10  |   11
 *  12  |   13
 * ------------------------------------
 *      save
 * ---------------------------------------
 */

SystemSetting::SystemSetting(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QGridLayout *gridLayout = new QGridLayout();

    QVector<QHBoxLayout *> layoutArray;
    layoutArray.resize(14);
    for(int i = 0; i<14; i++){
        layoutArray[i] = new QHBoxLayout();
        gridLayout->addLayout(layoutArray[i], i/2, i%2, Qt::AlignRight);
    }
    gridLayout->setMargin(30);
    gridLayout->setHorizontalSpacing(70);

    QLabel *label0 = new QLabel("企业编号 :  ");
    label0->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    layoutArray[0]->addWidget( label0, Qt::AlignRight);
    QLineEdit *lineEdit0 = new QLineEdit();
    lineEdit0->setReadOnly(true);
    layoutArray[0]->addWidget(lineEdit0, Qt::AlignRight);

    QLabel *label2 = new QLabel("模块编号 :  ");
    label2->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    layoutArray[2]->addWidget(label2, Qt::AlignRight);
    layoutArray[2]->addWidget(new QLineEdit(), Qt::AlignRight);

    QLabel *label3 = new QLabel("通道编号 :  ");
    label3->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    layoutArray[3]->addWidget(label3, Qt::AlignRight);
    layoutArray[3]->addWidget(new QLineEdit(), Qt::AlignRight);

    QLabel *label4 = new QLabel("设备编号 :  ");
    label4->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    layoutArray[4]->addWidget(label4, Qt::AlignRight);
    QLineEdit *lineEdit4 = new QLineEdit();
    lineEdit4->setReadOnly(true);
    layoutArray[4]->addWidget(lineEdit4, Qt::AlignRight);

    QLabel *label5 = new QLabel("设备名称 :  ");
    label5->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    layoutArray[5]->addWidget(label5, Qt::AlignRight);
    QLineEdit *lineEdit5 = new QLineEdit();
    lineEdit5->setReadOnly(true);
    layoutArray[5]->addWidget(lineEdit5, Qt::AlignRight);

    QLabel *label6 = new QLabel("输入电流下限 :  ");
    label6->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    layoutArray[6]->addWidget(label6, Qt::AlignRight);
    QLineEdit *lineEdit6 = new QLineEdit();
    lineEdit6->setReadOnly(true);
    layoutArray[6]->addWidget(lineEdit6, Qt::AlignRight);

    QLabel *label7 = new QLabel("接入电流上限 :  ");
    label7->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    layoutArray[7]->addWidget(label7, Qt::AlignRight);
    QLineEdit *lineEdit7 = new QLineEdit();
    lineEdit7->setReadOnly(true);
    layoutArray[7]->addWidget(lineEdit7, Qt::AlignRight);

    QLabel *label8 = new QLabel("输出对应值下限 :  ");
    label8->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    layoutArray[8]->addWidget(label8, Qt::AlignRight);
    QLineEdit *lineEdit8 = new QLineEdit();
    lineEdit8->setReadOnly(true);
    layoutArray[8]->addWidget(lineEdit8, Qt::AlignRight);

    QLabel *label9 = new QLabel("输出对应值上限 :  ");
    label9->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    layoutArray[9]->addWidget(label9, Qt::AlignRight);
    QLineEdit *lineEdit9 = new QLineEdit();
    lineEdit9->setReadOnly(true);
    layoutArray[9]->addWidget(lineEdit9, Qt::AlignRight);

    QLabel *label10 = new QLabel("校准值1 :  ");
    label10->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    layoutArray[10]->addWidget(label10, Qt::AlignRight);
    layoutArray[10]->addWidget(new QLineEdit(), Qt::AlignRight);

    QLabel *label11 = new QLabel("校准值2 :  ");
    label11->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    layoutArray[11]->addWidget(label11, Qt::AlignRight);
    layoutArray[11]->addWidget(new QLineEdit(), Qt::AlignRight);

    QLabel *label12 = new QLabel("数据单位 :  ");
    label12->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    layoutArray[12]->addWidget(label12, Qt::AlignRight);
    QLineEdit *lineEdit12 = new QLineEdit();
    lineEdit12->setReadOnly(true);
    layoutArray[12]->addWidget(lineEdit12, Qt::AlignRight);

    QLabel *label13 = new QLabel("采集数据类型 :  ");
    label13->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    layoutArray[13]->addWidget(label13, Qt::AlignRight);
    layoutArray[13]->addWidget(new QLineEdit(), Qt::AlignRight);


    setStyleSheet(QString(
                      "QLabel{"
                          "font-size:20px;"
                          "width:200;}"
                      ));

    QFrame *frame = new QFrame();
    frame->setLayout(gridLayout);
    frame->setFrameStyle(QFrame::Panel|QFrame::Raised);
    mainLayout->addWidget(frame);

    QPushButton *saveButton = new QPushButton("保存治污设备参数");
    saveButton->setFixedHeight(50);
    saveButton->setFont(QFont("Times", 18, QFont::Bold));
    mainLayout->addWidget(saveButton, Qt::AlignCenter);
    setLayout(mainLayout);
}




