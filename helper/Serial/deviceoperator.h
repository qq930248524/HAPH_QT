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

public:
    DeviceOperator(QSerialPort* port, bool useZigbee = true, int maxID = 10);
    QSerialPort* port;
    bool useZigbee;

    void stop();

    void setSearchMaxID(int max) { maxSearchID = max; }
    int  getSearchMaxID(void) const { return maxSearchID; }

public slots:
    void searchDevices();
    void getDeviceInfo(int dev);
    void setDeviceConfig(DataGatherConfiguration, DataGatherConfiguration);
    void calibrateDevice(int dev);
    void getDeviceADCRes(int, int );

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
    bool setNetAddress(uint16_t);
    bool readDevRegister(uint16_t* pRegs, int hostId, int slaveId
                         , uint16_t startreg, uint16_t nAddress);
    bool writeDevRegister(int dev, uint16_t address, uint16_t value);
    bool parseDeviceConfig(DataGatherConfiguration&, uint16_t*, int);

private:


    bool stopped;

    int maxSearchID = 10;
};

#endif // DEVICEOPERATOR_H
