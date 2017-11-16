#ifndef COLLECTIONSETTING_H
#define COLLECTIONSETTING_H


#include <QWidget>
#include    <QHBoxLayout>
#include    <QVBoxLayout>
#include    <QPushButton>
#include    <QLabel>
#include    <QLineEdit>
#include    <QTextEdit>
#include    <QComboBox>
#include    <QButtonGroup>
#include    <QRadioButton>
#include    <QComboBox>

#include <QtSerialPort/QSerialPort>
#include "helper/Serial/deviceoperator.h"
#include "helper/SwitchButton/switchbutton.h"

#include "helper/helper.h"
extern Helper *helper;

class CollectionSetting : public QWidget
{
    Q_OBJECT
public:
    explicit CollectionSetting(QWidget *parent = 0);
    ~CollectionSetting();
    void closeCollection();

    DeviceOperator * deviceOperator = NULL;

    QHBoxLayout *mainLayout;

    QVector<SwitchButton *>    radioArray;
    QVector<QLineEdit *>            lineArray;

    QComboBox   *portNumBox;
    QComboBox   *modNumBox;
    QComboBox   *checkDigitBox;
    QComboBox   *baudRateBox;
    QTextEdit   *logshow;

signals:

public slots:
    void    test();
    void    getU();
    void    showPortInfo(QString);
    void    searchModNum();
    void    setModNum();
    void    setPassType();
    void    readPassData();
    void    readZigbeeData();

    void    onGotDevInfo(bool,DataGatherConfiguration);
    void    onSearchDeviceFinished();
    void    onSetDevConfigFinished(bool);

    void    updateSendText(QByteArray msg);
    void    updateRecvText(QByteArray msg);
    void    updateCheckArray(int);
    void    updateADCLine(int devId, int32_t *data);

private :
    void    initDev();
    void    initCheckArrayUI();
    void    initMidWidgetUI();
    void    initButtonArrayUI();
};

#endif // COLLECTIONSETTING_H
