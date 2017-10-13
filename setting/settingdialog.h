#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QWidget>
#include <QDialog>

#include "tableWidget/systemsetting.h"
#include "tableWidget/collectionsetting.h"
#include "tableWidget/zigbeesetting.h"

class SettingDialog : public QDialog
{
Q_OBJECT
public:
    SettingDialog(QWidget *par = 0);
    QPushButton *homeButton;

private:
    const int titleSize = 60;
    CollectionSetting *tableCollection;
};

#endif // SETTINGDIALOG_H
