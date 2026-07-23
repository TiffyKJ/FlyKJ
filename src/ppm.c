#include <string.h>

#include "ppm.h"
#include "adsb_time.h"

static inline uint32_t us10_to_samples(
        uint32_t sample_rate,
        uint32_t us_x10)
{

    return (uint32_t)
        (((uint64_t)sample_rate * us_x10)
        / 10000000ULL);

}


static uint64_t integrate_region(
        const uint32_t *power,
        uint32_t length,
        uint32_t start,
        uint32_t end)
{

    if(start >= length)
        return 0;

    if(end > length)
        end = length;

    uint64_t sum = 0;

    for(uint32_t i=start;i<end;i++)
    {
        sum += power[i];
    }


    return sum;

}


int ppm_decode_frame(
        const uint32_t *power,
        uint32_t length,
        uint32_t refined_index,
        uint32_t sample_rate,
        PPMDecoderResult *result)
{

    if(!power ||
       !result ||
       length == 0)
    {
        return -1;
    }

    /*
     * Preamble start
     *
     * +
     *
     * 8us
     *
     */

    uint32_t payload_start =
        refined_index +
        us10_to_samples(
            sample_rate,
            ADSB_PAYLOAD_OFFSET_X10);

    /*
     * 112 bits
     *
     * each bit = 1us
     *
     */

    uint32_t frame_samples =
        us10_to_samples(
            sample_rate,
            ADSB_FRAME_BITS *
            ADSB_BIT_FULL_X10);



    /*
     * boundary check
     */

    if((uint64_t)payload_start +
       frame_samples >
       length)
    {
        return -1;
    }

    memset(
        result->bytes,
        0,
        sizeof(result->bytes));




    for(uint32_t bit_index=0;
        bit_index<ADSB_FRAME_BITS;
        bit_index++)
    {



        uint32_t start =
            payload_start +
            us10_to_samples(
                sample_rate,
                bit_index *
                ADSB_BIT_FULL_X10);



        uint32_t middle =
            payload_start +
            us10_to_samples(
                sample_rate,
                bit_index *
                ADSB_BIT_FULL_X10
                +
                ADSB_BIT_HALF_X10);



        uint32_t end =
            payload_start +
            us10_to_samples(
                sample_rate,
                (bit_index+1) *
                ADSB_BIT_FULL_X10);


        /*
         * Differential energy
         */

        uint64_t first_energy =
            integrate_region(
                power,
                length,
                start,
                middle);



        uint64_t second_energy =
            integrate_region(
                power,
                length,
                middle,
                end);


        /*
         * PPM:
         *
         * 1:
         *
         * first half high
         *
         *
         * 0:
         *
         * second half high
         *
         */


        uint8_t bit;


        if(first_energy > second_energy)
        {
            bit = 1;
        }
        else
        {
            bit = 0;
        }


        /*
         * MSB First
         *
         */

        if(bit)
        {

            result->bytes[bit_index / 8]
                |=
                (1 <<
                (7-(bit_index%8)));

        }

    }

    return 0;

}
