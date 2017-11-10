#ifndef DEVICEOPERATOR_H
#define DEVICEOPERATOR_H

#include <QThread>
#include "deviceconfiguration.h"


class QSerialPort;

class DeviceOperator : public QThread
{
    Q_OBJECT

public:
    bool isArm;
    static const int N_DEV_BAUDRATE = 11;
    static const int DeviceBaudrateList[N_DEV_BAUDRATE];

public:
    DeviceOperator(QSerialPort* port, bool useZigbee = true, int maxID = 1);
    QSerialPort* port;
    int hostId = 0;
    bool useZigbee;
    QString LABEL = QString("[Serial] ");

    void stop();

    void setSearchMaxID(int max) { maxSearchID = max; }
    int  getSearchMaxID(void) const { return maxSearchID; }

public slots:
    void searchDevices();
    void getDeviceInfo(int dev);
    void setDeviceConfig(DataGatherConfiguration, DataGatherConfiguration);
    void calibrateDevice(int dev);
    void getDeviceADCRes(int );
    void onGetAllpRef(int32_t *idArray);

signals:
    void deviceInformationGot(bool fSuccess, DataGatherConfiguration);
    void finishedDevSearching();
    void finishedDevConfigSet(bool fSuccess);
    void finishedDevCalibrate(bool fSuccess);
    void deviceADCResultGot(int dev, int32_t* pRes);
    void getAllpRef(int32_t *pRes);

    void sendMsg(QByteArray);
    void recvMsg(QByteArray);
    void test(QString);

protected:
    void run();

private:
    bool setNetAddress(uint16_t);
    bool readDevRegister(int32_t* pRegs, int slaveId
                         , uint16_t startreg, uint16_t nAddress);
    bool writeDevRegister(int dev, uint16_t address, uint16_t value);
    bool parseDeviceConfig(DataGatherConfiguration&, int32_t*, int);

private:


    bool stopped;

    int maxSearchID = 1;
};

#endif // DEVICEOPERATOR_H
