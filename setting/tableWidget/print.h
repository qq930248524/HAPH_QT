#ifndef PRINT_H
#define PRINT_H

#include <QWidget>
#include <QTextCodec>

#include "helper/helper.h"
#include "helper/Printer/printer.h"
extern Helper *helper;


namespace Ui {
class Print;
}

class Print : public QWidget
{
    Q_OBJECT

public:
    QString LABEL = "[PRINT]";
    explicit Print(QWidget *parent = 0);
    ~Print();

    Printer *printer = NULL;
    void initUI();

public slots:
    void on_comMode(int);
    void on_comChannel(int);
    void on_btnPrint();

private:
    Ui::Print *ui;
};

#endif // PRINT_H
