#ifndef POWER_H
#define POWER_H


#include <stdint.h>
#include <stddef.h>

#include "iq_reader.h"



/*
 * Calculate power from one IQ sample.
 *
 * raw IQ:
 *
 *      0 -------- 128 --------255
 *                    ^
 *                DC center
 *
 *
 * signed:
 *
 * I = raw_I -128
 * Q = raw_Q -128
 *
 *
 * Power:
 *
 * P = I² + Q²
 *
 */
uint32_t power_calc_sample(
        IQSample sample);


/*
 * Batch processing.
 */
void power_calc_buffer(
        const IQSample *input,
        uint32_t *output,
        size_t count);

#endif
