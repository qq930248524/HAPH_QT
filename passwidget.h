#ifndef PASSWIDGET_H
#define PASSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>


class PassWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PassWidget(QWidget *parent = 0);
    PassWidget(int mode, int passNum);
    ~PassWidget();
    QLabel *T;
    QLabel *C;

    void flush(int mode, uint16_t data, int pass);
};

#endif // PASSWIDGET_H
