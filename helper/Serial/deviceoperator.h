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
    int32_t *data = NULL;

    //函数指针，根据通信方式，使用不同的通信函数
    bool readDevRegister(int32_t* pRegs, int32_t moduleId, int32_t zigbeeId,
                         uint16_t startreg, uint16_t nAddress);
    bool readDevRegister_zigbee(int32_t* pRegs, int32_t zigbeeId,
                                uint16_t startreg, uint16_t nAddress);
    bool readDevRegister_modbus(int32_t* pRegs, int32_t moduleId,
                                uint16_t startreg, uint16_t nAddress);


public:
    DeviceOperator(QSerialPort* port, int hostId, int maxID = 1);
    QSerialPort* port;
    int hostId = 0;
    int slaveId = 0;
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

    bool writeDevRegister(int dev, uint16_t address, uint16_t value);

    bool sendSerial(bool isRead, int devId, uint16_t startReg, uint16_t nORv);

    bool parseDeviceConfig(DataGatherConfiguration&, int32_t*, int);

private:


    bool stopped;
    int maxSearchID = 1;
};

#endif // DEVICEOPERATOR_H
