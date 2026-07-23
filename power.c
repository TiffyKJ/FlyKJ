#include "power.h"


uint32_t power_calc_sample(IQSample sample)
{
    /*
     * RTL-SDR unsigned 8-bit IQ:
     *
     * 0 -------- 128 -------- 255
     *             ^
     *          zero point
     *
     * Remove DC offset.
     */
    //despcription giving by ai
    
    int16_t i_signed = (int16_t)sample.i - 128;
    int16_t q_signed = (int16_t)sample.q - 128;


    /*
     * Signal power:
     *
     * P = I^2 + Q^2
     *
     * No sqrt needed because
     * relative energy is enough.
     */

    
    int32_t power =
        (int32_t)i_signed * i_signed + 
        (int32_t)q_signed * q_signed;

    return (uint32_t)power;
}



void power_calc_buffer(
        const IQSample *in_samples,
        uint32_t *out_power,
        size_t count)
{
    if (!in_samples || !out_power)
        return;
    
    for (size_t i = 0; i < count; i++)
    {
        out_power[i] = power_calc_sample(in_samples[i]);
    }
}
