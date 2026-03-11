#ifndef CRC16_H
#define CRC16_H

#include <stdint.h>

uint16_t Modbus_CRC16(uint8_t *buf, uint16_t len);

#endif
