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
    gridLayout->setHorizontalSpacing(50);

    QLabel *label0 = new QLabel("企业编号:");
    label0->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    layoutArray[0]->addWidget( label0, Qt::AlignRight);
    layoutArray[0]->addWidget(new QLineEdit(), Qt::AlignRight);

    layoutArray[2]->addWidget(new QLabel("企业编号:"), Qt::AlignRight);
    layoutArray[2]->addWidget(new QLineEdit(), Qt::AlignRight);

    layoutArray[3]->addWidget(new QLabel("企业编号:"), Qt::AlignRight);
    layoutArray[3]->addWidget(new QLineEdit(), Qt::AlignRight);

    layoutArray[4]->addWidget(new QLabel("企业编号:"), Qt::AlignRight);
    layoutArray[4]->addWidget(new QLineEdit(), Qt::AlignRight);

    layoutArray[5]->addWidget(new QLabel("企业编号:"), Qt::AlignRight);
    layoutArray[5]->addWidget(new QLineEdit(), Qt::AlignRight);

    layoutArray[6]->addWidget(new QLabel("企业编号:"), Qt::AlignRight);
    layoutArray[6]->addWidget(new QLineEdit(), Qt::AlignRight);

    layoutArray[7]->addWidget(new QLabel("企业编号:"), Qt::AlignRight);
    layoutArray[7]->addWidget(new QLineEdit(), Qt::AlignRight);

    layoutArray[8]->addWidget(new QLabel("企业编号:"), Qt::AlignRight);
    layoutArray[8]->addWidget(new QLineEdit(), Qt::AlignRight);

    layoutArray[9]->addWidget(new QLabel("企业编号:"), Qt::AlignRight);
    layoutArray[9]->addWidget(new QLineEdit(), Qt::AlignRight);

    layoutArray[10]->addWidget(new QLabel("企业编号:"), Qt::AlignRight);
    layoutArray[10]->addWidget(new QLineEdit(), Qt::AlignRight);

    layoutArray[11]->addWidget(new QLabel("企业编号:"), Qt::AlignRight);
    layoutArray[11]->addWidget(new QLineEdit(), Qt::AlignRight);

    layoutArray[12]->addWidget(new QLabel("企业编号:"), Qt::AlignRight);
    layoutArray[12]->addWidget(new QLineEdit(), Qt::AlignRight);

    layoutArray[13]->addWidget(new QLabel("企业编号:"), Qt::AlignRight);
    layoutArray[13]->addWidget(new QLineEdit(), Qt::AlignRight);

    setStyleSheet(QString("QLabel{font:34}"));

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




