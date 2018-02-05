
#include <QtSerialPort/QSerialPort>
#include <iostream>
#include <QDebug>
#include <QtGlobal>
#include <sys/time.h>

#include "modbuscrc.h"
#include "deviceoperator.h"
#include "deviceregistermap.h"
#include "../Gpio/GPIOSet.h"

const int DeviceOperator::DeviceBaudrateList[N_DEV_BAUDRATE] = {
    300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200
};

DeviceOperator::DeviceOperator(QSerialPort* port, int hostId, int maxID) :
    port(port),
    hostId(hostId),
    maxSearchID(maxID)
{
    //ARM在编译器中定义
#ifdef ARM
    isArm = true;
#else
    isArm = false;
#endif
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

//搜索10个设备
void DeviceOperator::searchDevices()
{

    std::clog << "get into DeviceOperator::searchDevice(), search end at ID " << maxSearchID << std::endl;

    for(int devID = 1; devID <= maxSearchID; ++devID)
    {
        getDeviceInfo(devID);
    }

    emit finishedDevSearching();
}

//获取单个设备信息
void DeviceOperator::getDeviceInfo(int dev)
{
    int32_t    registerBuffer[32];
    bool        success = false;
    const int   nRegsToRead = MB_REG_ADCRES_ADDR - REG_HOLDING_START;

    if(dev >= MB_MIN_DEVICE_ADDR && dev <= MB_MAX_DEVICE_ADDR)
        success = readDevRegister_modbus(registerBuffer,dev, REG_HOLDING_START, nRegsToRead);

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

//设置设备配置信息
void DeviceOperator::setDeviceConfig(DataGatherConfiguration oldCfg, DataGatherConfiguration newCfg)
{
    bool writeSuccess = false;
    int dev = oldCfg.devID;

    uint16_t modbusConfigRegSetting = (newCfg.devID << 8) | (newCfg.parity << 4) | (newCfg.modbusBaudrate & 0x0F);

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

//获取单个通道数据
void DeviceOperator::getDeviceADCRes(int slaveId)
{
    // 保存读取到的采集器采集结果的数组，如果读取成功，需要由相应的类在接收到消息时释放，否则由该函数释放。
    int32_t* pRegs = new int32_t[MB_REG_ADCRES_LENGTH];

    // 读取设备采集结果寄存器组
    if(readDevRegister_modbus(pRegs, slaveId, MB_REG_ADCRES_ADDR, MB_REG_ADCRES_LENGTH))
    {
        emit deviceADCResultGot(slaveId, pRegs);
    }
    else
    {
        emit deviceADCResultGot(slaveId, NULL);
        delete [] pRegs;
    }
}

//获取数组中所有设备通道数据
/**************************************************
 * @brief:  1/使用Serial模块的缓存data来做一级缓存，
 *          2/调用readDevRegister函数来获取一个模块的数据，到data
 *          3/异步使用信号getAllpRef(data)，来发送缓存数据。（注意：这里是异步）
 * @param：idArray：设备数组，数组结构如下
 *      ————————————————————————————————————————————————————————
 *      |    |0      |1      |2      |3      |4      |n        |
 *      |    len     slaveId slaveId slaveId slaveId slaveId   |
 *      ————————————————————————————————————————————————————————
 * @return:
 **************************************************/
void DeviceOperator::onGetAllpRef(int32_t *idArray)
{
    int32_t devNum = idArray[0]-1;
    int32_t dataSize = devNum * MB_REG_ADCRES_LENGTH;

    static bool isFirst = true;
    if(isFirst){
        isFirst = false;
        data = (int32_t *)malloc(dataSize * sizeof(int32_t));
        for(int i = 0; i < dataSize; i++){
            data[i] = -1;
        }
    }

    for(int i = 0; i < devNum; i++){
        readDevRegister_modbus(&data[i*MB_REG_ADCRES_LENGTH], idArray[1+i],MB_REG_ADCRES_ADDR, MB_REG_ADCRES_LENGTH);
    }

    emit getAllpRef(data);
}

//设置本地目标设备地址 only 232/S1
bool DeviceOperator::setNetAddress(uint16_t addr)
{
    if(!port->isOpen())
        return false;
    char msgBuffer[32] = {};
    int  msgLen = 0;

    //DE DF EF D2 01 01
    msgBuffer[msgLen++] = 0xde;
    msgBuffer[msgLen++] = 0xdf;
    msgBuffer[msgLen++] = 0xef;
    msgBuffer[msgLen++] = 0xd2;
    msgBuffer[msgLen++] = HI_BYTE(addr);
    msgBuffer[msgLen++] = LO_BYTE(addr);

    // 清空接收和发送数据，准备发送消息并接受响应消息。
    port->clear();
    // --------------------------------启动数据发送
    if(0 < port->write(msgBuffer, msgLen))
    {
        port->waitForBytesWritten(500);
        port->flush();
        emit    sendMsg(QByteArray(msgBuffer, msgLen));
        QString str1 =  QByteArray(msgBuffer, msgLen).toHex().toUpper();
        int len1 = str1.length()/2;
        for(int i = 0; i < len1; i++){
            str1.insert(2*i+i-1, " ");
        }
        //        qDebug() << "[slaveId] = " << addr;
        //        qDebug() << "[send]" << str1;
    } else {
        return false;
    }

    // --------------------------------启动数据读取
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
        uint8_t okRes[] = {0xDE, 0xDF, 0xEF, 0xD2, 0x00};

        int ret = 0;
        for(int i = 0; i < recvLength; i++){
            ret += recvBuffer[i] ^ okRes[i];
        }
        //        qDebug() << (ret?"使用zigbee通信，设置目标ID faild":
        //                         "使用zigbee通信，设置目标ID ok") ;
        return ret?false:true;
    }
    else    // 没有收到设备的反馈
    {
        //        qDebug("[serial] read data timeout!");
        return false;
    }
}


void delayUS(int time)
{

    int num = (time < 200)?200:(time-200)*4;

    int nn = 1000 * num;
    int flag = 0;
    /////////////////  start ////////////////
    while(nn--){
        if(nn%num == 0){/////////////////  end  ////////////// 200 = 50us
            ///////////////////// start //////////////
            flag = 1 - flag;
            GPIOset::gpio_set_value(UART2, flag);
            //////////////////// end ///////////////    time = 150us
        }
    }
    return;
}

/*****************************************************
 *  ttyS1  232 zigbee  need set targetId
 *  ttyS2  485 modbus  need control GPIO0_12
 *  ttyS4  485 modbus  need control GPIO0_13
 * ****************************************************/
//获取采集器数据
/**************************************************
 * @brief:  读取slaveId模块的数据到pRegs，如果读取错误，则将pRegs置-1
 *          根据useZigbee，判断通信方式，（modebus/zigbee）
 *          modebus：slaveId当做moduleID使用，不需要通信前的设置采集器的目标ID，并且通信协议的前1字符为目标moduleID
 *          zigbee： slaveId当做zigbeeID使用，需要在通信前设置采集器的目标ID，并且通信协议的前2字符为本地zigbeeID
 * @param：
 *      pRegs：用来存取数据的buf，大小为12个int32_t
 *      slaveId:模块的ID，zigbeeID/moduleID
 *      startreg：采集器开始的寄存器地址，该不同功能对应不同地址
 *      nRegisters：需要读取的采集器的寄存器数量
 * @return: true=ok false=failed
 **************************************************/
bool DeviceOperator::readDevRegister(int32_t* pRegs, int32_t moduleId, int32_t zigbeeId,
                                            uint16_t startreg, uint16_t nRegisters)
{
    if(port->portName().indexOf("ttyS1") != -1){
        QString str = port->portName();
        return readDevRegister_zigbee(pRegs, zigbeeId, startreg, nRegisters);
    }else{
        return readDevRegister_modbus(pRegs, moduleId, startreg, nRegisters);
    }
}

bool DeviceOperator::readDevRegister_modbus(int32_t* pRegs, int slaveId,
                                            uint16_t startreg, uint16_t nRegisters)
{
    if(!pRegs)
        return false;

    if(!port->isOpen()){
        return false;
    }

    char msgBuffer[32] = {};
    int  msgLen = 0;
    msgBuffer[msgLen++] = slaveId;
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
    // ----------------------------------------启动数据发送
    if(isArm){
        if(port->portName().indexOf("S2")){
            GPIOset::gpio_set_value(UART2, UART_WRIT);
        }else if(port->portName().indexOf("S4")){
            GPIOset::gpio_set_value(UART4, UART_WRIT);
        }
    }

    if(port->write(msgBuffer, msgLen) == -1){
        qDebug() << LABEL + "数据发送失败";
        return false;
    }else if(port->waitForBytesWritten(250) == false){
        qDebug() << LABEL + "数据write失败 timeout! ";
        return false;
    }

    //    int n = 6400*8;
    //    while(n--){
    //        if(n == 0){
    //            goto abc;
    //        }
    //    }
    //    abc:
    //usleep(7700);//8ms = 7700us+150us+150us

    //    while(n--){
    //        if(n%100 == 0){//200us
    //            flag = 1 - flag;
    //            GPIOset::gpio_set_value(UART2, flag);//50us
    //        }
    //    }

    emit    sendMsg(QByteArray(msgBuffer, msgLen));
    //    QString str1 =  QByteArray(msgBuffer, msgLen).toHex().toUpper();
    //    int len1 = str1.length()/2;
    //    for(int i = 0; i < len1; i++){
    //        str1.insert(2*i+i-1, " ");
    //    }
    //    qDebug() << "[send]" << str1;

    //----------------------------------------启动读取数据
    if(isArm){// enable 485 write
        if(port->portName().indexOf("S2")){
            GPIOset::gpio_set_value(UART2, UART_READ);
        }else if(port->portName().indexOf("S4")){
            GPIOset::gpio_set_value(UART4, UART_READ);
        }
    }

    if(port->waitForReadyRead(500) == false){
        qDebug() << LABEL + "数据read失败 timeout! ";
        return false;
    }

    uint8_t recvBuffer[256] = { 0 };
    int     recvLength = 0;
    do
    {
        recvLength += port->read((char *)(recvBuffer + recvLength), 256-recvLength);
    }
    while(port->waitForReadyRead(50));
    emit    recvMsg(QByteArray((char *)recvBuffer, recvLength));


//        QString str =  QByteArray((char *)recvBuffer, recvLength).toHex().toUpper();
//        int len = str.length()/2;
//        for(int i = 0; i < len; i++){
//            str.insert(2*i+i-1, " ");
//        }
//        qDebug() << "[recv]<<" << str;
//        qDebug() << "recvLength = "<<recvLength << "   nRegisters=" << nRegisters;

    /* 响应数据已经接收完，开始消息处理 */
    if(recvLength != nRegisters*2 + 5){
        return false;
    }
    // 检查消息CRC是否正确
    if(usMBCRC16(recvBuffer, recvLength)){
        return false;
    }
    int msgOffset = 0;

    // 检查发送方ID
    if(recvBuffer[msgOffset++] != slaveId){
        return false;
    }

    // 检查发送方反馈功能码
    if(recvBuffer[msgOffset++] != MB_CODE_FUNC_READ_REG){
        return false;
    }

    // 检查是否收到完整的数据
    if(recvBuffer[msgOffset++] != nRegisters * 2){
        return false;
    }

    // ok! 收到期待的反馈响应，提取接收到的寄存器值
    for(int regIndex = 0; regIndex < nRegisters; ++regIndex)
    {
        pRegs[regIndex] = 0;
        pRegs[regIndex]  = recvBuffer[msgOffset++] << 8U;
        pRegs[regIndex] |= recvBuffer[msgOffset++];
    }
    return true;
}

bool DeviceOperator::readDevRegister_zigbee(int32_t* pRegs, int slaveId,
                                            uint16_t startreg, uint16_t nRegisters)
{
    if(!pRegs)
        return false;

    if(!port->isOpen()){
        return false;
    }

    if(!setNetAddress(slaveId)){
        return false;
    }
    char msgBuffer[32] = {};
    int  msgLen = 0;
    msgBuffer[msgLen++] = HI_BYTE(hostId);
    msgBuffer[msgLen++] = LO_BYTE(hostId);
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
    // ----------------------------------------启动数据发送
    if(port->write(msgBuffer, msgLen) == -1){
        qDebug() << LABEL + "数据发送失败";
        for(int i = 0; i < 12; pRegs[i++] = -1);
        return false;
    }

    if(port->waitForBytesWritten(250) == false){
        qDebug() << LABEL + "数据write失败 timeout! ";
        return false;
    }

    emit    sendMsg(QByteArray(msgBuffer, msgLen));
//    QString str1 =  QByteArray(msgBuffer, msgLen).toHex().toUpper();
//    int len1 = str1.length()/2;
//    for(int i = 0; i < len1; i++){
//        str1.insert(2*i+i-1, " ");
//    }
//    qDebug() << "[send]" << str1;

    //----------------------------------------启动读取数据
    if(port->waitForReadyRead(500) == false){
        qDebug() << LABEL + "数据read失败 timeout! ";
        for(int i = 0; i < 12; pRegs[i++] = -1);
        return false;
    }

    uint8_t recvBuffer[256] = { 0 };
    int     recvLength = 0;
    do
    {
        recvLength += port->read((char *)(recvBuffer + recvLength), 256-recvLength);
    }
    while(port->waitForReadyRead(50));
    emit    recvMsg(QByteArray((char *)recvBuffer, recvLength));


//    QString str =  QByteArray((char *)recvBuffer, recvLength).toHex().toUpper();
//    int len = str.length()/2;
//    for(int i = 0; i < len; i++){
//        str.insert(2*i+i-1, " ");
//    }
//    qDebug() << "[recv]<<" << str;
//    qDebug() << "recvLength = "<<recvLength << "   nRegisters=" << nRegisters;

    /* 响应数据已经接收完，开始消息处理 */
    if(recvLength != nRegisters*2 + 6){
        return false;
    }
    // 检查消息CRC是否正确
    if(usMBCRC16(recvBuffer, recvLength)){
        return false;
    }
    int msgOffset = 0;

    // 检查发送方ID
    if(recvBuffer[msgOffset++] != HI_BYTE(slaveId) ||
            recvBuffer[msgOffset++] != LO_BYTE(slaveId)){
        return false;
    }

    // 检查发送方反馈功能码
    if(recvBuffer[msgOffset++] != MB_CODE_FUNC_READ_REG){
        return false;
    }

    // 检查是否收到完整的数据
    if(recvBuffer[msgOffset++] != nRegisters * 2){
        return false;
    }

    // ok! 收到期待的反馈响应，提取接收到的寄存器值
    for(int regIndex = 0; regIndex < nRegisters; ++regIndex)
    {
        pRegs[regIndex] = 0;
        pRegs[regIndex]  = recvBuffer[msgOffset++] << 8U;
        pRegs[regIndex] |= recvBuffer[msgOffset++];
    }
    return true;
}

bool DeviceOperator::writeDevRegister(int dev, uint16_t regAddr, uint16_t value)
{

    //////////////////////////////////////////
    if(!port->isOpen())
        return false;

    char msgBuffer[32];
    int  msgLen = 0;

//    if(useZigbee){
//        msgBuffer[msgLen++] = HI_BYTE(hostId);
//        msgBuffer[msgLen++] = LO_BYTE(hostId);
//    }else{
//        msgBuffer[msgLen++] = dev;
//    }
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

    // ----------------------------启动数据发送
    if(isArm){// enable 485 write
        if(port->portName().indexOf("S2")){
            GPIOset::gpio_set_value(UART2, UART_WRIT);
        }else if(port->portName().indexOf("S4")){
            GPIOset::gpio_set_value(UART4, UART_WRIT);
        }else{
            qDebug()<< LABEL + "error 没有找到对应串口";
            return false;
        }
    }

    if(port->write(msgBuffer, msgLen) == -1){
        qDebug() << LABEL + "数据发送失败";
    }
    if(port->waitForBytesWritten(250) == false){
        qDebug() << LABEL + "数据write失败 timeout! ";
        return false;
    }

    usleep(7700);//8ms = 770us+150us+150us
    port->clear();

    emit    sendMsg(QByteArray(msgBuffer, msgLen));

    // ------------------------启动数据读取
    if(isArm){// enable 485 read
        if(port->portName().indexOf("S2")){
            GPIOset::gpio_set_value(UART2, UART_READ);
        }else if(port->portName().indexOf("S4")){
            GPIOset::gpio_set_value(UART4, UART_READ);
        }else{
            qDebug()<< LABEL + "error 没有找到对应串口";
            return false;
        }
    }
    if(port->waitForReadyRead(250) == false){
        qDebug() << LABEL + "数据read失败 timeout! ";
        return false;
    }

    uint8_t recvBuffer[256] = { 0 };
    int recvLength = 0;

    do
    {
        recvLength += port->read((char *)(recvBuffer + recvLength), 256-recvLength);
    }
    while(port->waitForReadyRead(50));
    emit    recvMsg(QByteArray((char *)recvBuffer, recvLength));

    //------------------------------check data
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

/*
 * 从获取到的采集器设备寄存器中解析该设备的配置信息
 * devRegs指针指向从设备硬件版本寄存器开始的设备寄存器数组。
 */
bool DeviceOperator::parseDeviceConfig(DataGatherConfiguration& cfg, int32_t* devRegs, int nRegs)
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
