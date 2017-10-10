#ifndef COLLECTIONSETTING_H
#define COLLECTIONSETTING_H
#include "mainwindow.h"

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

class CollectionSetting : public QWidget
{
    Q_OBJECT
public:
    explicit CollectionSetting(QWidget *parent = 0);
    ~CollectionSetting();
    void closeCollection();
    QHBoxLayout *mainLayout;

    QVector<SwitchButton *>    radioArray;
    QVector<QLineEdit *>            lineArray;

    QComboBox   *modNumBox;
    QComboBox   *checkDigitBox;
    QComboBox   *baudRateBox;
    QTextEdit       *logshow;

signals:

public slots:
    void    searchModNum();
    void    setModNum();
    void    setPassType();
    void    readEquPar();
    void    readPassData();
    void    readZigbeeData();

    void    onGotDevInfo(bool,DataGatherConfiguration);
    void    onSearchDeviceFinished();
    void    onSetDevConfigFinished(bool);

    void    updateSendText(QByteArray msg);
    void    updateRecvText(QByteArray   msg);
    void    updateCheckArray(QString);
    void    updateADCLine(int devId, uint16_t *data);
    void    test(QString);

private :
    void    initDev();
    void    initCheckArrayUI();
    void    initMidWidgetUI();
    void    initButtonArrayUI();

//    extern QSerialPort *serialPort;
//    extern DeviceOperator *deviceOperator;
};

#endif // COLLECTIONSETTING_H
