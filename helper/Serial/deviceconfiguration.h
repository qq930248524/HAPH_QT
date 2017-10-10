#ifndef DEVICECONFIGURATION_H
#define DEVICECONFIGURATION_H

#include <stdint.h>

#include "deviceregistermap.h"

enum ModbusParityMode {
    MB_NO_PARITY = 0,
    MB_ODD_PARITY,
    MB_EVEN_PARITY,
    MB_INVALID_PARITY
};

struct DataGatherConfiguration {
    uint16_t hardwareVersion;
    uint16_t AcSensorSpan;
    uint16_t DcSensorSpan;
    uint16_t firmwareVersion;

    uint16_t zigbeePanID;
    uint16_t zigbeeLocalAddress;
    uint16_t zigbeeTargetAddress;
    uint16_t zigbeeChannel;
    bool zigbeeExist;

    uint16_t devID;             // 采集器的Modbus地址
    uint16_t modbusBaudrate;    // 采集器使用的波特率索引
    ModbusParityMode parity;    // 采集器使用的校验方式

    uint16_t modbusQuery;
    uint16_t inputType;
    uint16_t inputMode;
    uint16_t adcStatus;
    uint16_t adcRefVoltage;
    uint8_t  serialID[MB_REG_SERIAL_LENGTH];
    uint8_t  productName[MB_REG_NAME_LENGTH];
};

#endif // DEVICECONFIGURATION_H
