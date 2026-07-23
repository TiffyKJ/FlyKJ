#ifndef CRC24_H
#define CRC24_H

#include <stdint.h>

#define CRC24_POLY 0xFFF409

/*
 * Calculate Mode-S CRC24 syndrome
 *
 * input:
 *   msg:
 *       ADS-B message bytes
 *
 *   bits:
 *       message length in bits
 *
 * return:
 *       24-bit syndrome
 *
 *
 * For valid DF17:
 *
 *       syndrome == 0
 */
uint32_t crc24_syndrome(
        const uint8_t *msg,
        uint32_t bits);


/*
 * Simple validity check
 */
int crc24_check(
        const uint8_t *msg,
        uint32_t bits);


#endif
