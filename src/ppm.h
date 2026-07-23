#ifndef PPM_H
#define PPM_H


#include <stdint.h>


/*
 * ADS-B 112-bit message
 *
 * 112 / 8 = 14 bytes
 *
 */

typedef struct
{

    uint8_t bytes[14];


} PPMDecoderResult;



/*
 * Decode ADS-B PPM payload
 *
 * Input:
 *
 * power:
 *      Power detector output
 *
 * length:
 *      number of samples
 *
 * refined_index:
 *      Preamble P0 start index
 *
 * sample_rate:
 *      samples per second
 *
 *
 * Output:
 *
 * bytes[14]
 *
 *
 * Return:
 *
 * 0 success
 * -1 error
 *
 */

int ppm_decode_frame(
        const uint32_t *power,
        uint32_t length,
        uint32_t refined_index,
        uint32_t sample_rate,
        PPMDecoderResult *result);



#endif
