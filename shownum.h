#ifndef SHOWNUM_H
#define SHOWNUM_H

#include <QFrame>

namespace Ui {
class ShowNum;
}

class ShowNum : public QFrame
{
    Q_OBJECT

public:
    explicit ShowNum(QWidget *parent = 0);
    ~ShowNum();

    void setTitle(QString);
    void setName(QString);
    void setUnit(QString);
    void setValue(double);
    void setOn(int);

private:
    Ui::ShowNum *ui;
};

#endif // SHOWNUM_H
