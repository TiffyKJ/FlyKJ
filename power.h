#ifndef POWER_H
#define POWER_H

#include <stdint.h>
#include <stddef.h>

#include "iq_reader.h"


/*
 * Calculate signal power from one IQ sample.
 *
 * Input:
 *     unsigned 8-bit IQ sample from RTL-SDR
 *
 * Conversion:
 *     signed_I = I - 128
 *     signed_Q = Q - 128
 *
 * Output:
 *     Power = I^2 + Q^2
 */


uint32_t power_calc_sample(IQSample sample);



/*
 * Batch processing version.
 *
 * Convert an array of IQ samples into power envelope.
 *
 * in_samples:
 *     IQ input buffer
 *
 * out_power:
 *     output power array
 *
 * count:
 *     number of samples
 */


void power_calc_buffer(
        const IQSample *in_samples,
        uint32_t *out_power,
        size_t count);


#endif
