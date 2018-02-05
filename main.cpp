#include "mainwindow.h"
#include "helper/watchdog/watchdog.h"
#include "helper/Gpio/GPIOSet.h"
#include <QApplication>
#include <QFontDatabase>

#include <QDebug>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>
#include <QFile>
#define LOGFILEMAX 10000

//qDebug("");
//qWarning("");
//qCritical("");
//qFatal("");
void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();

    QString text;
    switch(type)
    {
    case QtDebugMsg:
        text = QString("Debug:");
        break;

    case QtWarningMsg:
        text = QString("Warning:");
        break;

    case QtCriticalMsg:
        text = QString("Critical:");
        break;

    case QtFatalMsg:
        text = QString("Fatal:");
    }

    QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date = QString("(%1)").arg(current_date_time);
    QString message = QString("%1 %2 %3 %4").arg(text).arg(msg).arg(context_info).arg(current_date);

    QFile file("/var/daslog.txt");
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();

    mutex.unlock();
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //设置日志
    //注册MessageHandler
    qInstallMessageHandler(outputMessage);
    qDebug("======================================================");
    qDebug("[MAIN] =============== QT 开始运行 ====================");
    qDebug("======================================================");

    //设置看门狗
    WatchDog watchDog;
    watchDog.startWatchDog();

    //设置字体
    int id = QFontDatabase::addApplicationFont("/lib/fonts/DroidSansFallback.ttf");
    QString msyh = QFontDatabase::applicationFontFamilies (id).at(0);
    QFont font(msyh,10);
    font.setPointSize(10);
    a.setFont(font);

    //设置MainWindow
    MainWindow w;
    w.setFixedSize(800,480);
    w.show();

    return a.exec();
}
