#ifndef _CRC_H_
#define _CRC_H_

#include <stdint.h>

#define X25_INIT_CRC 0xffff
#define X25_VALIDATE_CRC 0xf0b8

uint16_t crc_accumulate(uint8_t data, uint16_t crcAccum);


#endif
