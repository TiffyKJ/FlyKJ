#include "crc24.h"

/*
 * Extract MSB-first bit
 *
 * ADS-B bit ordering:
 *
 * byte0 bit7
 * byte0 bit6
 * ...
 * byte0 bit0
 *
 */
static uint8_t get_bit(
        const uint8_t *data,
        uint32_t index)
{
    uint32_t byte =
        index / 8;

    uint32_t bit =
        7 - (index % 8);


    return (data[byte] >> bit) & 1;
}



uint32_t crc24_syndrome(
        const uint8_t *msg,
        uint32_t bits)
{
    if(!msg || bits == 0)
        return 0;


    /*
     * ADS-B currently supports:
     *
     * Short Frame:
     *      56 bits
     *
     * Long Frame:
     *      112 bits
     *
     */
    if(bits > 112)
        return 0;



    uint32_t crc = 0;



    for(uint32_t i = 0;
        i < bits;
        i++)
    {

        uint32_t input =
            get_bit(msg,i);



        /*
         * Current MSB of CRC register
         */
        uint32_t msb =
            (crc >> 23) & 1;



        /*
         * Shift register
         */
        crc <<= 1;



        /*
         * Polynomial feedback
         */
        if(msb ^ input)
        {
            crc ^= CRC24_POLY;
        }



        /*
         * Keep 24 bits
         */
        crc &= 0xFFFFFF;
    }



    return crc;
}



int crc24_check(
        const uint8_t *msg,
        uint32_t bits)
{
    return crc24_syndrome(msg,bits)==0;
}
