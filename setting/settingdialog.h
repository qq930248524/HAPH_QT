#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QWidget>
#include <QDialog>

#include "DMSNavigation.h"
#include "tableWidget/systemsetting.h"
#include "tableWidget/collectionsetting.h"
#include "tableWidget/zigbeesetting.h"

class SettingDialog : public QDialog
{
Q_OBJECT
public:
    SettingDialog(QWidget *par = 0);
    ~SettingDialog();

    QPushButton *homeButton;

private:
    const int titleSize = 60;
    DeviceOperator *deviceOperator;

    QVector<QWidget *>  objArray;

public slots:
    void changedTO(int);
};

#endif // SETTINGDIALOG_H
