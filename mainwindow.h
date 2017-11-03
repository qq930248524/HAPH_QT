#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QHostInfo>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <setting/settingdialog.h>

#include "helper/helper.h"

extern Helper *helper;


class MainWindow : public QMainWindow
{
        Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    const int ROW = 4;
    const int COL = 3;

    int modeNum = 0;
    bool isFull = true;
    bool isOriginal = false;

    QGridLayout *gridLayout;
    QVector<QWidget *> gridWidget;
    QFrame *bottomFrame;
    QTextEdit *log;
    QPushButton *setting;
    QLabel *timing;

    SettingDialog *settingDialog;
protected:
    void timerEvent(QTimerEvent *event);

private:
    void showWidget();

    const QString onStr = QString("background-color:green; min-width:60px; min-height:40px;");
    const QString ofStr = QString("background-color:gray; min-width:60px; min-height:40px;");
    QPushButton *btn_internet;
    QPushButton *btn_serial;
    QPushButton *btn_mqtt;
    QPushButton *btn_data;
    QPushButton *btn_full;
    int     timeId;

public slots:
    void startSet();
    void recvADCResult(int, int32_t *);

    void switchFullScreen();
    void checkInternet();
    void checkSerial();
    void checkMqtt();
    void conversionData();

    void mqttConnectted();
    void mqttDisConnectted();
};

#endif // MAINWINDOW_H
