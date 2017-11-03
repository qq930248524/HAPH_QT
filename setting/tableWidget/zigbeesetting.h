#ifndef ZIGBEESETTING_H
#define ZIGBEESETTING_H

#include <QFrame>

#include "helper/helper.h"
extern Helper *helper;


namespace Ui {
class ZigbeeSetting;
}

class ZigbeeSetting : public QFrame
{
    Q_OBJECT

public:
    explicit ZigbeeSetting(QWidget *parent = 0);
    ~ZigbeeSetting();


protected slots:
    /*slot btn*/
    void getPar();
    void setPar();
    /*slot deviceOperator*/
    void    onGetOneDevCfg(bool fSuccess, DataGatherConfiguration);
    void    onSetDevConfigFinished(bool isOK);
    void    z_updateSendText(QByteArray msg);
    void    z_updateRecvText(QByteArray msg);

private:
    Ui::ZigbeeSetting *ui;
};

#endif // ZIGBEESETTING_H
