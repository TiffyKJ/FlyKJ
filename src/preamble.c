#include "preamble.h"

/*
 * ADS-B preamble timing
 *
 * Unit:
 *
 * 0.1 us
 */

#define P0_START_X10 0
#define P0_END_X10   5

#define G0_START_X10 5
#define G0_END_X10   10

#define P1_START_X10 10
#define P1_END_X10   15

#define G1_START_X10 15
#define G1_END_X10   35

#define P2_START_X10 35
#define P2_END_X10   40

#define G2_START_X10 40
#define G2_END_X10   45

#define P3_START_X10 45
#define P3_END_X10   50

/*
 * Gap ratio:
 *
 * Eg / Ep < 0.3
 *
 * Eg * 10 < Ep * 3
 */
#define GAP_RATIO_NUM 3
#define GAP_RATIO_DEN 10



/*
 * Convert 0.1us time unit
 * to sample index
 *
 * No floating point
 */
static inline uint32_t us10_to_samples(
        uint32_t sample_rate,
        uint32_t us_x10)
{
    return
        (uint32_t)
        (((uint64_t)sample_rate * us_x10)
        /
        10000000ULL);
}


/*
 * Safe energy integration
 *
 * [start_idx,end_idx)
 *
 */
static uint64_t integrate_region(
        const uint32_t *power,
        uint32_t length,
        uint32_t start_idx,
        uint32_t end_idx)
{

    if(start_idx >= length)
    {
        return 0;
    }


    if(end_idx > length)
    {
        end_idx = length;
    }


    uint64_t sum = 0;


    /*
     * Extremely low sample rate case
     *
     * fallback single sample
     */
    if(start_idx == end_idx)
    {
        return power[start_idx];
    }


    for(uint32_t i=start_idx;
        i<end_idx;
        i++)
    {
        sum += power[i];
    }

    return sum;
}


static int check_preamble_at(
        const uint32_t *power,
        uint32_t length,
        uint32_t index,
        uint32_t sample_rate,
        uint64_t min_pulse_energy)
{


    /*
     * Convert physical timing
     */

    uint32_t p0_s =
        index +
        us10_to_samples(
            sample_rate,
            P0_START_X10);

    uint32_t p0_e =
        index +
        us10_to_samples(
            sample_rate,
            P0_END_X10);

    uint32_t g0_s =
        index +
        us10_to_samples(
            sample_rate,
            G0_START_X10);

    uint32_t g0_e =
        index +
        us10_to_samples(
            sample_rate,
            G0_END_X10);


    uint32_t p1_s =
        index +
        us10_to_samples(
            sample_rate,
            P1_START_X10);

    uint32_t p1_e =
        index +
        us10_to_samples(
            sample_rate,
            P1_END_X10);


    uint32_t g1_s =
        index +
        us10_to_samples(
            sample_rate,
            G1_START_X10);

    uint32_t g1_e =
        index +
        us10_to_samples(
            sample_rate,
            G1_END_X10);


    uint32_t p2_s =
        index +
        us10_to_samples(
            sample_rate,
            P2_START_X10);

    uint32_t p2_e =
        index +
        us10_to_samples(
            sample_rate,
            P2_END_X10);


    uint32_t g2_s =
        index +
        us10_to_samples(
            sample_rate,
            G2_START_X10);

    uint32_t g2_e =
        index +
        us10_to_samples(
            sample_rate,
            G2_END_X10);

    uint32_t p3_s =
        index +
        us10_to_samples(
            sample_rate,
            P3_START_X10);

    uint32_t p3_e =
        index +
        us10_to_samples(
            sample_rate,
            P3_END_X10);

    /*
     * Pulse energy
     */

    uint64_t ep0 =
        integrate_region(
            power,
            length,
            p0_s,
            p0_e);

    uint64_t ep1 =
        integrate_region(
            power,
            length,
            p1_s,
            p1_e);

    uint64_t ep2 =
        integrate_region(
            power,
            length,
            p2_s,
            p2_e);

    uint64_t ep3 =
        integrate_region(
            power,
            length,
            p3_s,
            p3_e);


    uint64_t e_pulse =
        ep0 +
        ep1 +
        ep2 +
        ep3;


    /*
     * Gate 1:
     *
     * Total pulse energy
     */
    if(e_pulse < min_pulse_energy)
    {
        return 0;
    }



    /*
     * Each pulse must exist
     */
    uint64_t min_single =
        min_pulse_energy / 4;

    if(ep0 < min_single ||
       ep1 < min_single ||
       ep2 < min_single ||
       ep3 < min_single)
    {
        return 0;
    }


    /*
     * Gap energy
     */

    uint64_t eg0 =
        integrate_region(
            power,
            length,
            g0_s,
            g0_e);

    uint64_t eg1 =
        integrate_region(
            power,
            length,
            g1_s,
            g1_e);

    uint64_t eg2 =
        integrate_region(
            power,
            length,
            g2_s,
            g2_e);

    uint64_t e_gap =
        eg0 +
        eg1 +
        eg2;


    /*
     * Gate 2:
     *
     * Gap suppression
     *
     * Eg/Ep < 0.3
     */
    if((e_gap * GAP_RATIO_DEN)
       >=
       (e_pulse * GAP_RATIO_NUM))
    {
        return 0;
    }

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
}





int preamble_detect(
        PreambleDetector *detector,
        const uint32_t *power,
        uint32_t length,
        uint32_t noise)
{

    if(!detector || !power)
        return -1;



    /*
     * 5us preamble window
     */
    uint32_t span_samples =
        us10_to_samples(
            detector->sample_rate,
            50);


    if(span_samples == 0 ||
       length < span_samples)
    {
        return -1;
    }



    /*
     * Total pulse duration:
     *
     * 4 * 0.5us = 2us
     */
    uint32_t pulse_samples =
        us10_to_samples(
            detector->sample_rate,
            20);


    /*
     * Avoid zero threshold
     */
    if(pulse_samples == 0)
    {
        pulse_samples = 1;
    }



    /*
     * Minimum pulse energy
     */
    uint64_t min_pulse_energy =
        (uint64_t)noise *
        detector->threshold_factor *
        pulse_samples;



    for(uint32_t i=0;
        i+span_samples <= length;
        i++)
    {

        if(check_preamble_at(
                power,
                length,
                i,
                detector->sample_rate,
                min_pulse_energy))
        {
            return (int)i;
        }

    }



    return -1;
}
