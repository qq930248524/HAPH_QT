#ifndef PRINT_H
#define PRINT_H

#include <QWidget>

#include "helper/helper.h"
extern Helper *helper;


namespace Ui {
class Print;
}

class Print : public QWidget
{
    Q_OBJECT

public:
    explicit Print(QWidget *parent = 0);
    ~Print();

    void initUI();

public slots:
    void on_comMode(int);
    void on_comChannel(int);
    void on_btnPrint();

private:
    Ui::Print *ui;
};

#endif // PRINT_H
