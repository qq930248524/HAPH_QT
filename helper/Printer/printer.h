#ifndef PRINTER_H
#define PRINTER_H

#include <QObject>
#include <QSerialPort>

class Printer : public QObject
{
    Q_OBJECT
public:
    explicit Printer(QObject *parent = 0);
    ~Printer();

    QSerialPort *ttyS4_232 = NULL;

    bool print_data(QString);

signals:

public slots:
};

#endif // PRINTER_H
