#include "phase.h"
#include "adsb_time.h"

#define GAP_WEIGHT 1ULL

static inline uint32_t us10_to_samples(
        uint32_t sample_rate,
        uint32_t us_x10)
{
    return
        (uint32_t)
        (((uint64_t)sample_rate * us_x10)
        /10000000ULL);
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

    if(start == end)
        return power[start];

    for(uint32_t i=start;i<end;i++)
    {
        sum += power[i];
    }


    return sum;
}



static void calculate_phase_score(
        const uint32_t *power,
        uint32_t length,
        uint32_t index,
        uint32_t sample_rate,
        uint64_t *score,
        uint64_t *pulse_energy)
{

#define TIME(x) us10_to_samples(sample_rate,x)

    uint32_t p0s=index+TIME(ADSB_P0_START_X10);
    uint32_t p0e=index+TIME(ADSB_P0_END_X10);

    uint32_t g0s=index+TIME(ADSB_G0_START_X10);
    uint32_t g0e=index+TIME(ADSB_G0_END_X10);

    uint32_t p1s=index+TIME(ADSB_P1_START_X10);
    uint32_t p1e=index+TIME(ADSB_P1_END_X10);

    uint32_t g1s=index+TIME(ADSB_G1_START_X10);
    uint32_t g1e=index+TIME(ADSB_G1_END_X10);


    uint32_t p2s=index+TIME(ADSB_P2_START_X10);
    uint32_t p2e=index+TIME(ADSB_P2_END_X10);

    uint32_t g2s=index+TIME(ADSB_G2_START_X10);
    uint32_t g2e=index+TIME(ADSB_G2_END_X10);


    uint32_t p3s=index+TIME(ADSB_P3_START_X10);
    uint32_t p3e=index+TIME(ADSB_P3_END_X10);


    uint64_t ep =
        integrate_region(power,length,p0s,p0e)
       +integrate_region(power,length,p1s,p1e)
       +integrate_region(power,length,p2s,p2e)
       +integrate_region(power,length,p3s,p3e);


    uint64_t eg =
        integrate_region(power,length,g0s,g0e)
       +integrate_region(power,length,g1s,g1e)
       +integrate_region(power,length,g2s,g2e);


    *pulse_energy = ep;

    if(ep >= eg * GAP_WEIGHT)
        *score = ep - eg * GAP_WEIGHT;
    else
        *score = 0;

#undef TIME
}


int phase_search_refine(
        const uint32_t *power,
        uint32_t length,
        uint32_t rough_index,
        uint32_t sample_rate,
        PhaseSearchResult *result)
{

    if(!power || !result || length==0)
        return -1;

    uint32_t span_samples =
        us10_to_samples(
                sample_rate,
                ADSB_PREAMBLE_END_X10);

    int32_t best_offset=0;


    uint64_t best_score=0;

    uint64_t best_energy=0;



    for(int32_t offset=-1;
        offset<=1;
        offset++)
    {

        int32_t candidate =
            (int32_t)rough_index + offset;


        if(candidate < 0)
            continue;

        if((uint32_t)candidate + span_samples > length)
            continue;

        uint64_t score;
        uint64_t energy;

        calculate_phase_score(
                power,
                length,
                (uint32_t)candidate,
                sample_rate,
                &score,
                &energy);


        if(score > best_score)
        {
            best_score = score;
            best_energy = energy;
            best_offset = offset;
        }

    }

    int32_t final_index =
        (int32_t)rough_index + best_offset;

    if(final_index < 0)
        return -1;

    result->refined_index =
        (uint32_t)final_index;

    result->score =
        best_score;

    result->pulse_energy =
        best_energy;

    result->phase_offset =
        best_offset;

    return 0;
}
