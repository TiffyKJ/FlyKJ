#include <stdio.h>
#include <assert.h>

#include "ppm.h"

static void fill_region(
        uint32_t *power,
        uint32_t start,
        uint32_t end,
        uint32_t value)
{

    for(uint32_t i=start;i<end;i++)
    {
        power[i]=value;
    }

}


static void setup_bit(
        uint32_t *power,
        uint32_t payload_start,
        uint32_t sample_rate,
        uint32_t bit_index,
        uint8_t value)
{

    uint32_t start =
        payload_start +
        ((uint64_t)sample_rate *
        bit_index *
        10)
        /
        10000000ULL;



    uint32_t middle =
        payload_start +
        ((uint64_t)sample_rate *
        (bit_index*10+5))
        /
        10000000ULL;



    uint32_t end =
        payload_start +
        ((uint64_t)sample_rate *
        (bit_index*10+10))
        /
        10000000ULL;


    if(value)
    {

        fill_region(
            power,
            start,
            middle,
            10000);


        fill_region(
            power,
            middle,
            end,
            100);

    }

    else
    {

        fill_region(
            power,
            start,
            middle,
            100);


        fill_region(
            power,
            middle,
            end,
            10000);

    }

}


void test_ppm_decode()
{

    uint32_t power[400];


    for(int i=0;i<400;i++)
    {
        power[i]=100;
    }

    uint32_t sample_rate =
        2400000;



    uint32_t refined_index =
        10;

    uint32_t payload_start =
        refined_index +
        ((uint64_t)sample_rate*80)
        /
        10000000ULL;


    /*
     * First byte:
     *
     * 10100001
     *
     */

    uint8_t pattern[8]=
    {
        1,0,1,0,
        0,0,0,1
    };


    for(int i=0;i<8;i++)
    {

        setup_bit(
            power,
            payload_start,
            sample_rate,
            i,
            pattern[i]);

    }



    /*
     * remaining bits = 0
     */

    for(int i=8;i<112;i++)
    {

        setup_bit(
            power,
            payload_start,
            sample_rate,
            i,
            0);

    }


    PPMDecoderResult result;

    int ret =
        ppm_decode_frame(
            power,
            400,
            refined_index,
            sample_rate,
            &result);

    assert(ret==0);

    printf(
        "[PPM Test] Byte0 = 0x%02X\n",
        result.bytes[0]);

    assert(result.bytes[0]==0xA1);

    for(int i=1;i<14;i++)
    {
        assert(result.bytes[i]==0);
    }


}



int main()
{

    printf(
      "=== PPM Decoder Test ===\n");

    test_ppm_decode();

    printf(
      "=== Passed ===\n");

    return 0;

}
