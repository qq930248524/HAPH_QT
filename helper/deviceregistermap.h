#ifndef DEVICEREGISTERMAP_H
#define DEVICEREGISTERMAP_H

#define MB_MAX_DEVICE_ADDR              247
#define MB_MIN_DEVICE_ADDR              1

#define MB_BROADCAST_ADDR               0

#define MB_CODE_FUNC_WRITE_REG			0x06
#define MB_CODE_FUNC_READ_REG			0x03

#define REG_HOLDING_START				(0x08)
#define REG_HOLDING_NREGS				(36)

#define MB_REG_HARDVER_ADDR             0x08
#define MB_REG_AC_SENSORSPAN_ADDR       0x09
#define MB_REG_DC_SENSORSPAN_ADDR       0x0A

#define MB_REG_ZG_PANID_ADDR			0x0C
#define MB_REG_ZG_LOCAL_ADDR			0x0D
#define MB_REG_ZG_TARGET_ADDR           0x0E
#define MB_REG_ZG_CNTL_ADDR             0x0F
#define MB_REG_CFG_ADDR					0x10
#define MB_REG_QUERY_ADDR				0x11
#define MB_REG_INTYPE_ADDR				0x12
#define MB_REG_INMODE_ADDR              0x13
#define MB_REG_ADC_ST_ADDR              0x14
#define MB_REG_ADC_REF_ADDR             0x15
#define MB_REG_SOFTVER_ADDR             0x16
#define MB_REG_SERIAL_ADDR				0x17
#define MB_REG_SERIAL_LENGTH			12
#define MB_REG_NAME_ADDR                0x1D
#define MB_REG_NAME_LENGTH              4
#define MB_REG_CNTR_ADDR				0x1F
#define MB_REG_ADCRES_ADDR				0x20
#define MB_REG_ADCRES_LENGTH            12

#define MB_HW_VER_OFFSET				(MB_REG_HARDVER_ADDR - REG_HOLDING_START)
#define MB_AC_SENSOR_OFFSET				(MB_REG_AC_SENSORSPAN_ADDR - REG_HOLDING_START)
#define MB_DC_SENSOR_OFFSET             (MB_REG_DC_SENSORSPAN_ADDR - REG_HOLDING_START)


#define ZG_REG_PANID_OFFSET				(MB_REG_ZG_PANID_ADDR - REG_HOLDING_START)
#define ZG_REG_LADDR_OFFSET				(MB_REG_ZG_LOCAL_ADDR - REG_HOLDING_START)
#define ZG_REG_TADDR_OFFSET				(MB_REG_ZG_TARGET_ADDR - REG_HOLDING_START)
#define ZG_REG_CNTL_OFFSET				(MB_REG_ZG_CNTL_ADDR - REG_HOLDING_START)
//
#define MB_REG_CFG_OFFSET               (MB_REG_CFG_ADDR - REG_HOLDING_START)
#define MB_REG_QUERY_OFFSET             (MB_REG_QUERY_ADDR - REG_HOLDING_START)
#define MB_REG_INTYPE_OFFSET            (MB_REG_INTYPE_ADDR - REG_HOLDING_START)
#define MB_REG_INMODE_OFFSET            (MB_REG_INMODE_ADDR - REG_HOLDING_START)
#define MB_REG_ADCST_OFFSET             (MB_REG_ADC_ST_ADDR - REG_HOLDING_START)
#define MB_REG_ADCREF_OFFSET			(MB_REG_ADC_REF_ADDR - REG_HOLDING_START)
#define MB_REG_SW_VER_OFFSET			(MB_REG_SOFTVER_ADDR - REG_HOLDING_START)
#define MB_REG_SID_OFFSET				(MB_REG_SERIAL_ADDR - REG_HOLDING_START)
#define MB_REG_PROD_OFFSET				(MB_REG_NAME_ADDR - REG_HOLDING_START)
#define MB_REG_CTRL_OFFSET				(MB_REG_CNTR_ADDR - REG_HOLDING_START)
#define MB_REG_ADCRES_OFFSET			(MB_REG_ADCRES_ADDR - REG_HOLDING_START)

// ModBus配置寄存器位Offset
#define MB_REG_CFG_ADDR_OFF				(8U)
#define MB_REG_CFG_LOCK_OFF				(7U)
#define MB_REG_CFG_MODE_OFF				(6U)
#define MB_REG_CFG_PARITY_OFF			(4U)
#define MB_REG_CFG_BAUD_OFF				(0U)

#define ZG_REG_CNTL_CHANNEL_MSK         0x00FFU
#define ZG_REG_CNTL_STATE_MSK           0x0100U

#define MB_REG_CFG_ADDR_MSK				0xFF00U
#define MB_REG_CFG_LOCK_MSK				0x0080U
#define MB_REG_CFG_MODE_MSK				0x0040U
#define MB_REG_CFG_PARITY_MSK			0x0030U
#define MB_REG_CFG_BAUD_MSK				0x000FU

#define MB_CTRL_ACCESS_KEY				0x5A00U

#define MB_REG_QUERY_MSK				0x03FFU

#define ZG_REG_STATE_ACTIVE_MSK         0x0100U

#define MB_CFG_NBAUDRATE				11

#define HI_BYTE(W)                      (((W) & 0xFF00U) >> 8)
#define LO_BYTE(W)                      ((W) & 0xFFU)

#endif // DEVICEREGISTERMAP_H

