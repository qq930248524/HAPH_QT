#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QWidget>
#include <QTextEdit>
#include <QPushButton>

#include "helper/helper.h"

extern Helper *helper;

class MainWindow : public QMainWindow
{
        Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initMqtt();
    void initSerial();

    const int ROW = 4;
    const int COL = 3;
    static const int passSize = 12;
    int modeSize = 0;
    int mode = 0;

    QGridLayout *gridLayout;
    QVector<QWidget *> gridWidget;
    QTextEdit *log;
    QPushButton *setting;
    QLabel *timing;

protected:
    void timerEvent(QTimerEvent *event);

private:
    bool checkSerial();
    void flushWidgets(int mode);
    void showWidget();
    int     timeId;

public slots:
    void startSet();
    void recvADCResult(int, uint16_t *);

};

#endif // MAINWINDOW_H
