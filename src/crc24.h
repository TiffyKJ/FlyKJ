#ifndef CRC24_H
#define CRC24_H

#include <stdint.h>


/*
 * ADS-B / Mode-S CRC-24 Polynomial
 *
 * x^24 + x^23 + ... + x^3 + 1
 *
 * Hex:
 * 0xFFF409
 */
#define CRC24_POLY 0xFFF409



/*
 * Calculate Mode-S CRC24 Syndrome
 *
 * Input:
 *      msg  : ADS-B frame bytes
 *      bits : number of valid bits
 *
 * Example:
 *
 *      DF17 long frame:
 *
 *      bits = 112
 *
 *
 * Return:
 *
 *      0
 *          CRC valid
 *
 *      non-zero
 *          CRC remainder
 *
 */
uint32_t crc24_syndrome(
        const uint8_t *msg,
        uint32_t bits);



/*
 * CRC check helper
 *
 * return:
 *
 *      1 : valid
 *      0 : invalid
 *
 */
int crc24_check(
        const uint8_t *msg,
        uint32_t bits);


#endif /* CRC24_H */
