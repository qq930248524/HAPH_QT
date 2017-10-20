#ifndef NUMPAD_H
#define NUMPAD_H

#include <QDialog>

namespace Ui {
class NumPad;
}

class NumPad : public QDialog
{
    Q_OBJECT

public:
    explicit NumPad(QWidget *parent = 0);
    ~NumPad();


public slots:
    void getOneNum();
    void enter();

private:
    QString mmm;

private:
    Ui::NumPad *ui;
};

#endif // NUMPAD_H
