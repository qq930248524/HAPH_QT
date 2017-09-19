#ifndef TIMEWIDGET_H
#define TIMEWIDGET_H

#include <QWidget>
#include <QLCDNumber>

class timeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit timeWidget(QWidget *parent = 0);
private:
    QLCDNumber *dataLcd;
    QLCDNumber *timeLcd;

signals:

public slots:
    void onTimeOut();
};

#endif // TIMEWIDGET_H
