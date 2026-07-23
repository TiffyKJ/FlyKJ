#include "power.h"


uint32_t power_calc_sample(
        IQSample sample)
{

    int16_t i_signed =
        (int16_t)sample.i - 128;


    int16_t q_signed =
        (int16_t)sample.q - 128;



    int32_t power =
        (int32_t)i_signed * i_signed
        +
        (int32_t)q_signed * q_signed;


    return (uint32_t)power;
}

void power_calc_buffer(
        const IQSample *input,
        uint32_t *output,
        size_t count)
{
    if(!input || !output)
        return;

    for(size_t i=0;i<count;i++)
    {
        output[i] =
            power_calc_sample(input[i]);
    }
}
