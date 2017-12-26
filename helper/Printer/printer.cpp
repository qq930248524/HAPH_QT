#include "printer.h"
#include <QTextCodec>

Printer::Printer(QObject *parent) : QObject(parent)
{
    ttyS4_232 = new QSerialPort("/dev/ttyS4");
    ttyS4_232->setBaudRate(QSerialPort::Baud9600);
    ttyS4_232->setDataBits(QSerialPort::Data8);
    ttyS4_232->setParity(QSerialPort::NoParity);
    ttyS4_232->setStopBits(QSerialPort::OneStop);
    ttyS4_232->open(QIODevice::ReadWrite);
}

/**************************************************
 * @brief:  打印一行数据
 * @param：  dataStr:需要打印的数据
 * @return: true=ok false=failed
 **************************************************/
bool Printer::print_data(QString dataStr)
{
    QTextCodec* gbk = QTextCodec::codecForName("gbk");
    QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(utf8);

    QString strUnicode      = utf8->toUnicode(dataStr.toLocal8Bit().data());
    QByteArray dataArray    = gbk->fromUnicode(strUnicode);
    dataArray.append(0x0a);

    int ret = ttyS4_232->write(dataArray);

    if(ret != dataArray.size()+1 || !ttyS4_232->waitForBytesWritten(200)){
        return false;
    }
    return true;
}

Printer::~Printer()
{
    ttyS4_232->close();
    delete ttyS4_232;
}
