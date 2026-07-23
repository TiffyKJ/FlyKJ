#include "preamble.h"

static uint64_t integrate(
        const uint32_t *power,
        uint32_t start,
        uint32_t width)
{

    uint64_t sum = 0;


    for(uint32_t i=0;i<width;i++)
    {
        sum += power[start+i];
    }


    return sum;
}


static uint32_t time_to_samples(
        uint32_t sample_rate,
        uint32_t microseconds)
{
    return
        (sample_rate * microseconds)
        /1000000;
}



static int check_preamble(
        const uint32_t *power,
        uint32_t index,
        uint32_t pulse_width,
        uint32_t spacing,
        uint64_t pulse_threshold,
        uint64_t pause_threshold)
{

    /*
     * Pulse locations:
     *
     * 0us
     * 1us
     * 3.5us
     * 4.5us
     */


    uint32_t p0 =
        index;

    uint32_t p1 =
        index + spacing;

    uint32_t p2 =
        index + time_to_samples(
                1,
                0);   // placeholder


    (void)p2;


    uint64_t e0 =
        integrate(
            power,
            p0,
            pulse_width);


    uint64_t e1 =
        integrate(
            power,
            p1,
            pulse_width);


    if(e0 < pulse_threshold)
        return 0;


    if(e1 < pulse_threshold)
        return 0;


    /*
     * TODO:
     *
     * add pulse3/pulse4
     *
     * add pause penalty
     */


    return 1;
}

void preamble_init(
        PreambleDetector *detector,
        uint32_t sample_rate,
        uint32_t threshold_factor)
{

    if(!detector)
        return;

    detector->sample_rate =
        sample_rate;

    detector->threshold_factor =
        threshold_factor;

    detector->pulse_window =
        sample_rate / 2000000;
}




int preamble_detect(
        PreambleDetector *detector,
        const uint32_t *power,
        uint32_t length,
        uint32_t noise)
{

    if(!detector || !power)
        return -1;

    uint64_t pulse_threshold =
        (uint64_t)noise *
        detector->threshold_factor *
        detector->pulse_window;

    uint64_t pause_threshold =
        pulse_threshold / 4;


    uint32_t span =
        time_to_samples(
            detector->sample_rate,
            5);


    for(uint32_t i=0;
        i+span<length;
        i++)
    {

        if(check_preamble(
                power,
                i,
                detector->pulse_window,
                detector->pulse_window*2,
                pulse_threshold,
                pause_threshold))
        {
            return i;
        }

    }
    return -1;
}
