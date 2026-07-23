#include <stdio.h>
#include <assert.h>

#include "crc24.h"

/*
 * Set bits MSB first
 */
static void set_bits(
        uint8_t *msg,
        uint32_t start,
        uint32_t value,
        uint32_t length)
{

    for(int i = length - 1;
        i >= 0;
        i--)
    {

        uint8_t bit =
            (value >> i) & 1;


        uint32_t index =
            start + (length - 1 - i);


        uint32_t byte =
            index / 8;

      
        uint32_t pos =
            7 - (index % 8);

      
        if(bit)
        {
            msg[byte] |=
                (1 << pos);
        }

    }
}


void test_crc_valid(void)
{
    uint8_t frame[14]={0};



    /*
     * Fake DF17 payload
     *
     * first 88 bits
     */
    set_bits(
        frame,
        0,
        0x123456789ABCDE,
        56);


    /*
     * Calculate CRC parity
     *
     * Using payload only
     */
    uint32_t crc =
        crc24_syndrome(
            frame,
            88);

    /*
     * Append parity
     *
     * bits 88-111
     */
    frame[11] =
        (crc >> 16) & 0xff;

    frame[12] =
        (crc >> 8) & 0xff;

    frame[13] =
        crc & 0xff;



    uint32_t result =
        crc24_syndrome(
            frame,
            112);



    printf(
        "[CRC Valid Test] Syndrome = 0x%06X\n",
        result);



    assert(result == 0);
}



void test_crc_invalid(void)
{
    uint8_t frame[14]={0};

    frame[0]=0x8D;
    frame[1]=0xAA;

  
    /*
     * random wrong frame
     */
    assert(
        crc24_check(
            frame,
            112)==0);
}



int main(void)
{

    printf(
        "=== CRC24 Test ===\n");

    test_crc_valid();
    test_crc_invalid();

    printf(
        "=== CRC24 Passed ===\n");


    return 0;
}
