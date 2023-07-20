#include <stdint.h>
#include "crc.h"


uint16_t crc_accumulate(uint8_t data, uint16_t crcAccum)
{
        /*Accumulate one byte of data into the CRC*/
        uint8_t tmp;

        tmp = data ^ (uint8_t)(crcAccum &0xff);
        tmp ^= (tmp<<4);
        crcAccum = (crcAccum>>8) ^ (tmp<<8) ^ (tmp <<3) ^ (tmp>>4);

        return crcAccum;
}
