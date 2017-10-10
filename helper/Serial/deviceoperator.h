#ifndef DEVICEOPERATOR_H
#define DEVICEOPERATOR_H

#include <QThread>
#include "deviceconfiguration.h"


class QSerialPort;

class DeviceOperator : public QThread
{
    Q_OBJECT

public:
    static const int N_DEV_BAUDRATE = 11;
    static const int DeviceBaudrateList[N_DEV_BAUDRATE];

    static bool devIsLegal(int devID)
    {
        if(devID >= MB_MIN_DEVICE_ADDR && devID <= MB_MAX_DEVICE_ADDR)
            return true;
        return false;
    }

public:
    DeviceOperator(QSerialPort* port, int maxID = 10);
    QSerialPort* port;

    void stop();

    void setSearchMaxID(int max) { maxSearchID = max; }
    int  getSearchMaxID(void) const { return maxSearchID; }

public slots:
    void searchDevices();
    void getDeviceInfo(int dev);
    void setDeviceConfig(DataGatherConfiguration, DataGatherConfiguration);
    void calibrateDevice(int dev);
    void getDeviceADCRes(int dev);

signals:
    void deviceInformationGot(bool fSuccess, DataGatherConfiguration);
    void finishedDevSearching();
    void finishedDevConfigSet(bool fSuccess);
    void finishedDevCalibrate(bool fSuccess);
    void deviceADCResultGot(int dev, uint16_t* pRes);

    void sendMsg(QByteArray);
    void recvMsg(QByteArray);
    void test(QString);

protected:
    void run();

private:
    bool readDevRegister(uint16_t* pRegs, int dev, uint16_t startreg, uint16_t nAddress);
    bool writeDevRegister(int dev, uint16_t address, uint16_t value);
    bool parseDeviceConfig(DataGatherConfiguration&, uint16_t*, int);

private:


    bool stopped;

    int maxSearchID;
};

#endif // DEVICEOPERATOR_H
