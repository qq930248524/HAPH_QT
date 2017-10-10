#ifndef _MODBUSCRC_H
#define _MODBUSCRC_H

#include <stdint.h>

uint16_t usMBCRC16(const uint8_t * pucFrame, uint16_t usLen);

#endif
