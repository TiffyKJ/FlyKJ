#include <stdio.h>
#include <assert.h>

#include "preamble.h"

static void setup_pulse(
        uint32_t *power,
        uint32_t base_idx,
        uint32_t sample_rate,
        uint32_t start_us_x10,
        uint32_t duration_us_x10,
        uint32_t level)
{

    uint32_t start =
        base_idx +
        ((uint64_t)sample_rate *
        start_us_x10)
        /
        10000000ULL;

    uint32_t length =
        ((uint64_t)sample_rate *
        duration_us_x10)
        /
        10000000ULL;


    if(length == 0)
        length = 1;

    for(uint32_t i=0;i<length;i++)
    {
        power[start+i]=level;
    }
}



void test_standard_preamble(void)
{

    uint32_t power[200];
    for(int i=0;i<200;i++)
        power[i]=100;


    uint32_t base=30;
    uint32_t sr=2400000;

    setup_pulse(power,base,sr,0,5,10000);
    setup_pulse(power,base,sr,10,5,10000);
    setup_pulse(power,base,sr,35,5,10000);
    setup_pulse(power,base,sr,45,5,10000);


    PreambleDetector detector;

    preamble_init(
        &detector,
        sr,
        5);


    int idx =
        preamble_detect(
            &detector,
            power,
            200,
            100);

    printf(
        "[Strong] index=%d\n",
        idx);

    assert(idx==30);
}





void test_noise_rejection(void)
{
    uint32_t power[200];

    for(int i=0;i<200;i++)
        power[i]=100;

    uint32_t base=30;


    /*
     * continuous burst
     */
    for(int i=0;i<20;i++)
    {
        power[base+i]=10000;
    }


    PreambleDetector detector;

    preamble_init(
        &detector,
        2400000,
        5);


    int idx =
        preamble_detect(
            &detector,
            power,
            200,
            100);


    printf(
        "[Noise] index=%d\n",
        idx);

    assert(idx==-1);
}



void test_weak_signal(void)
{

    uint32_t power[200];

    for(int i=0;i<200;i++)
        power[i]=100;

    uint32_t base=40;

    uint32_t sr=2400000;

    setup_pulse(power,base,sr,0,5,600);
    setup_pulse(power,base,sr,10,5,600);
    setup_pulse(power,base,sr,35,5,600);
    setup_pulse(power,base,sr,45,5,600);


    PreambleDetector detector;


    preamble_init(
        &detector,
        sr,
        5);

    int idx =
        preamble_detect(
            &detector,
            power,
            200,
            100);

    printf(
        "[Weak] index=%d\n",
        idx);


    assert(idx==40);
}



int main(void)
{

    printf(
        "=== Preamble Test ===\n");

    test_standard_preamble();

    test_noise_rejection();

    test_weak_signal();

    printf(
        "=== Passed ===\n");

    return 0;
}
