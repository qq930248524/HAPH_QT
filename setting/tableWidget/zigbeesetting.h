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
    void updatePar();

protected slots:
    /*slot btn*/
    void getPar();
    void setPar();
    /*slot deviceOperator*/
    void    onSetDevConfigFinished(bool isOK);
    void    updateSendText(QByteArray msg);
    void    updateRecvText(QByteArray msg);

private:
    Ui::ZigbeeSetting *ui;
};

#endif // ZIGBEESETTING_H
