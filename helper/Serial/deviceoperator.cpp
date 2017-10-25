

#include <QtSerialPort/QSerialPort>
#include <iostream>
#include <QDebug>

#include "modbuscrc.h"
#include "deviceoperator.h"
#include "deviceregistermap.h"

const int DeviceOperator::DeviceBaudrateList[N_DEV_BAUDRATE] = {
    300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200
};

DeviceOperator::DeviceOperator(QSerialPort* port, bool useZigbee, int maxID) :
    port(port),
    stopped(true),
    useZigbee(useZigbee),
    maxSearchID(maxID)
{
}

void DeviceOperator::stop()
{
    stopped = false;
}

void DeviceOperator::run()
{
    while(!stopped)
    {
        sleep(2);
    }

    stopped = true;
}

void DeviceOperator::searchDevices()
{

    std::clog << "get into DeviceOperator::searchDevice(), search end at ID " << maxSearchID << std::endl;

    for(int devID = 1; devID <= maxSearchID; ++devID)
    {
        getDeviceInfo(devID);
    }

    emit finishedDevSearching();
}

void DeviceOperator::getDeviceInfo(int dev)
{
    uint16_t    registerBuffer[32];
    bool        success = false;
    const int   nRegsToRead = MB_REG_ADCRES_ADDR - REG_HOLDING_START;

    if(dev >= MB_MIN_DEVICE_ADDR && dev <= MB_MAX_DEVICE_ADDR)
        success = readDevRegister(registerBuffer, dev, REG_HOLDING_START, nRegsToRead);

    DataGatherConfiguration cfg;
    cfg.devID = dev;

    if(success)
    {
        std::clog << "find a new device with id = " << dev << std::endl;
        bool currect = parseDeviceConfig(cfg, registerBuffer, nRegsToRead);
        emit deviceInformationGot(currect, cfg);
    }
    else
    {
        emit deviceInformationGot(false, cfg);
    }
}

void DeviceOperator::setDeviceConfig(DataGatherConfiguration oldCfg, DataGatherConfiguration newCfg)
{
    bool writeSuccess = false;
    int dev = oldCfg.devID;

    uint16_t modbusConfigRegSetting = (newCfg.devID << 8) | (newCfg.parity << 4) | (newCfg.modbusBaudrate & 0x0F);

    if(!devIsLegal(newCfg.devID))
        goto jobFailed;

    if(newCfg.modbusBaudrate >= N_DEV_BAUDRATE)
        goto jobFailed;

    if(newCfg.parity == MB_INVALID_PARITY)
        goto jobFailed;

    // 修改Zigbee配置信息
    if(oldCfg.zigbeePanID != newCfg.zigbeePanID)
    {
        writeSuccess = writeDevRegister(dev, MB_REG_ZG_PANID_ADDR, newCfg.zigbeePanID);
        if(!writeSuccess)
            goto jobFailed;
    }

    if(oldCfg.zigbeeLocalAddress != newCfg.zigbeeLocalAddress)
    {
        writeSuccess = writeDevRegister(dev, MB_REG_ZG_LOCAL_ADDR, newCfg.zigbeeLocalAddress);
        if(!writeSuccess)
            goto jobFailed;
    }

    if(oldCfg.zigbeeTargetAddress != newCfg.zigbeeTargetAddress)
    {
        writeSuccess = writeDevRegister(dev, MB_REG_ZG_TARGET_ADDR, newCfg.zigbeeTargetAddress);
        if(!writeSuccess)
            goto jobFailed;
    }

    if(oldCfg.zigbeeChannel != newCfg.zigbeeChannel)
    {
        writeSuccess = writeDevRegister(dev, MB_REG_ZG_CNTL_ADDR, newCfg.zigbeeChannel);
        if(!writeSuccess)
            goto jobFailed;
    }
    // 修改输入模式寄存器
    if((oldCfg.inputMode&0xFFFU) != (newCfg.inputMode&0xFFFU))
    {
        writeSuccess = writeDevRegister(dev, MB_REG_INMODE_ADDR, newCfg.inputMode);
        if(!writeSuccess)
            goto jobFailed;
    }

    // 最后修改modbus配置寄存器
    if(oldCfg.devID != newCfg.devID || oldCfg.modbusBaudrate != newCfg.modbusBaudrate || oldCfg.parity != newCfg.parity)
    {
        writeSuccess = writeDevRegister(dev, MB_REG_CFG_ADDR, modbusConfigRegSetting);
        if(!writeSuccess)
            goto jobFailed;
    }

    emit finishedDevConfigSet(true);

    return;

// 出错了，发送出错信号
    jobFailed:
    emit finishedDevConfigSet(false);
}

void DeviceOperator::calibrateDevice(int dev)
{
    bool fSuccess = writeDevRegister(dev, MB_REG_CNTR_ADDR, MB_CTRL_ACCESS_KEY | 0x01);

    emit finishedDevCalibrate(fSuccess);
}

void DeviceOperator::getDeviceADCRes(int dev)
{
    // 保存读取到的采集器采集结果的数组，如果读取成功，需要由相应的类在接收到消息时释放，否则由该函数释放。
    uint16_t* pRegs = new uint16_t[MB_REG_ADCRES_LENGTH];

    // 检查是否设备地址合法
    if(!devIsLegal(dev))
        return;

    // 读取设备采集结果寄存器组
    if(readDevRegister(pRegs, dev, MB_REG_ADCRES_ADDR, MB_REG_ADCRES_LENGTH))
    {
        emit deviceADCResultGot(dev, pRegs);
    }
    else
    {
        emit deviceADCResultGot(dev, NULL);
        delete [] pRegs;
    }
}

bool DeviceOperator::readDevRegister(uint16_t* pRegs, int dev, uint16_t startreg, uint16_t nRegisters)
{

    if(!pRegs)
        return false;

    if(!port->isOpen())
        return false;

    if(!devIsLegal(dev))
        return false;

    char msgBuffer[32] = {};
    int  msgLen = 0;

    if(useZigbee){
        msgBuffer[msgLen++] = 0x00;
    }
    msgBuffer[msgLen++] = dev;
    msgBuffer[msgLen++] = MB_CODE_FUNC_READ_REG;
    msgBuffer[msgLen++] = HI_BYTE(startreg);
    msgBuffer[msgLen++] = LO_BYTE(startreg);
    msgBuffer[msgLen++] = HI_BYTE(nRegisters);
    msgBuffer[msgLen++] = LO_BYTE(nRegisters);

    uint16_t crc = usMBCRC16((uint8_t*)msgBuffer, msgLen);

    msgBuffer[msgLen++] = LO_BYTE(crc);
    msgBuffer[msgLen++] = HI_BYTE(crc);

    // 清空接收和发送数据，准备发送消息并接受响应消息。
    port->clear();

    // 启动数据发送
    if(0 < port->write(msgBuffer, msgLen))
    {
        port->waitForBytesWritten(250);
        port->flush();
        emit    sendMsg(QByteArray(msgBuffer, msgLen));
    }
    else
    {
        return false;
    }


    // 等待数据发送完成
    if(port->waitForReadyRead(500))
    {
        uint8_t recvBuffer[256] = { 0 };
        int     recvLength = 0;

        do
        {
            recvLength += port->read((char *)(recvBuffer + recvLength), 256-recvLength);
        }
        while(port->waitForReadyRead(50));
        emit    recvMsg(QByteArray((char *)recvBuffer, recvLength));

        /* 响应数据已经接收完，开始消息处理 */
        if(recvLength != nRegisters*2 + (useZigbee?6:5))
            return false;

        // 检查消息CRC是否正确
        if(usMBCRC16(recvBuffer, recvLength))
            return false;

        int msgOffset = 0;

        // 检查发送方ID
        if(useZigbee){
            msgOffset++;
        }

        if(recvBuffer[msgOffset++] != dev)
            return false;

        // 检查发送方反馈功能码
        if(recvBuffer[msgOffset++] != MB_CODE_FUNC_READ_REG)
            return false;

        // 检查是否收到完整的数据
        if(recvBuffer[msgOffset++] != nRegisters * 2)
            return false;

        // ok! 收到期待的反馈响应，提取接收到的寄存器值
        for(int regIndex = 0; regIndex < nRegisters; ++regIndex)
        {
            pRegs[regIndex]  = recvBuffer[msgOffset++] << 8U;
            pRegs[regIndex] |= recvBuffer[msgOffset++];
        }

        return true;
    }
    else    // 没有收到设备的反馈
    {
        qDebug("[serial] read data timeout!");
        return false;
    }
}

bool DeviceOperator::writeDevRegister(int dev, uint16_t regAddr, uint16_t value)
{
    if(!port->isOpen())
        return false;

    if(!devIsLegal(dev))
        return false;

    char msgBuffer[32];
    int  msgLen = 0;

    if(useZigbee){
        msgBuffer[msgLen++] = 0x00;
    }
    msgBuffer[msgLen++] = dev;
    msgBuffer[msgLen++] = MB_CODE_FUNC_WRITE_REG;
    msgBuffer[msgLen++] = HI_BYTE(regAddr);
    msgBuffer[msgLen++] = LO_BYTE(regAddr);
    msgBuffer[msgLen++] = HI_BYTE(value);
    msgBuffer[msgLen++] = LO_BYTE(value);

    uint16_t crc = usMBCRC16((uint8_t*)msgBuffer, msgLen);

    msgBuffer[msgLen++] = LO_BYTE(crc);
    msgBuffer[msgLen++] = HI_BYTE(crc);

    // 清空接收和发送数据，准备发送消息并接受响应消息。
    port->clear();

    // 启动数据发送
    if(0 < port->write(msgBuffer, msgLen))
    {
        port->waitForBytesWritten(250);
        port->flush();
    }
    else
        return false;
    emit    sendMsg(QByteArray(msgBuffer, msgLen));

    // 等待数据发送完成
    if(port->waitForReadyRead(250))
    {
        uint8_t recvBuffer[256] = { 0 };
        int recvLength = 0;

        do
        {
            recvLength += port->read((char *)(recvBuffer + recvLength), 256-recvLength);
        }
        while(port->waitForReadyRead(50));
        emit    recvMsg(QByteArray((char *)recvBuffer, recvLength));

        // 检查消息CRC是否正确
        if(usMBCRC16(recvBuffer, recvLength))
        {
            std::cout << "write register error, received " << recvLength << " bytes esponse." << std::endl;
            return false;
        }

        // 检查写设备寄存器是否成功
        if(recvBuffer[0] != dev)
            return false;

        // 如果这个反馈功能码不对，说明写寄存器操作出错
        if(recvBuffer[1] != MB_CODE_FUNC_WRITE_REG)
            return false;

        // write success
        return true;
    }

    return false;
}

/*
 * 从获取到的采集器设备寄存器中解析该设备的配置信息
 * devRegs指针指向从设备硬件版本寄存器开始的设备寄存器数组。
 */
bool DeviceOperator::parseDeviceConfig(DataGatherConfiguration& cfg, uint16_t* devRegs, int nRegs)
{
    // 确保读取设备寄存器时不会越出边界
    if(nRegs < MB_REG_CTRL_OFFSET)
        return false;

    int regOffset;
    regOffset = MB_HW_VER_OFFSET;
    cfg.hardwareVersion = devRegs[regOffset];
    regOffset = MB_AC_SENSOR_OFFSET;
    cfg.AcSensorSpan = devRegs[regOffset];
    regOffset = MB_DC_SENSOR_OFFSET;
    cfg.DcSensorSpan = devRegs[regOffset];

    regOffset = ZG_REG_PANID_OFFSET;
    cfg.zigbeePanID = devRegs[regOffset];
    regOffset = ZG_REG_LADDR_OFFSET;
    cfg.zigbeeLocalAddress = devRegs[regOffset];
    regOffset = ZG_REG_TADDR_OFFSET;
    cfg.zigbeeTargetAddress = devRegs[regOffset];
    regOffset = ZG_REG_CNTL_OFFSET;
    cfg.zigbeeChannel = devRegs[regOffset] & ZG_REG_CNTL_CHANNEL_MSK;
    cfg.zigbeeExist = (devRegs[regOffset] & ZG_REG_CNTL_STATE_MSK) != 0;

    regOffset = MB_REG_CFG_OFFSET;
    cfg.devID = devRegs[regOffset] >> MB_REG_CFG_ADDR_OFF;
    cfg.modbusBaudrate = (devRegs[regOffset] & MB_REG_CFG_BAUD_MSK) >> MB_REG_CFG_BAUD_OFF;
    cfg.parity = static_cast<ModbusParityMode>((devRegs[regOffset] & MB_REG_CFG_PARITY_MSK) >> MB_REG_CFG_PARITY_OFF);

    regOffset = MB_REG_QUERY_OFFSET;
    cfg.modbusQuery = devRegs[regOffset];
    regOffset = MB_REG_INTYPE_OFFSET;
    cfg.inputType = devRegs[regOffset];
    regOffset = MB_REG_INMODE_OFFSET;
    cfg.inputMode = devRegs[regOffset];
    regOffset = MB_REG_ADCST_OFFSET;
    cfg.adcStatus = devRegs[regOffset];
    regOffset = MB_REG_ADCREF_OFFSET;
    cfg.adcRefVoltage = devRegs[regOffset];
    regOffset = MB_REG_SW_VER_OFFSET;
    cfg.firmwareVersion = devRegs[regOffset];

    regOffset = MB_REG_SID_OFFSET;
    for(int i = 0; i < MB_REG_SERIAL_LENGTH;)
    {
        cfg.serialID[i++] = LO_BYTE(devRegs[regOffset]);
        cfg.serialID[i++] = HI_BYTE(devRegs[regOffset]);
        ++regOffset;
    }

    regOffset = MB_REG_PROD_OFFSET;
    for(int i = 0;i < MB_REG_NAME_LENGTH;)
    {
        cfg.productName[i++] = LO_BYTE(devRegs[regOffset]);
        cfg.productName[i++] = HI_BYTE(devRegs[regOffset]);
        ++regOffset;
    }

    return true;
}
