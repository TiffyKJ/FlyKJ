#include <stdio.h>
#include <assert.h>

#include "phase.h"


static void setup_pulse(
        uint32_t *power,
        uint32_t base,
        uint32_t sr,
        uint32_t start_x10,
        uint32_t duration_x10,
        uint32_t value)
{

    uint32_t start =
        base +
        ((uint64_t)sr*start_x10)/10000000ULL;

    uint32_t len =
        ((uint64_t)sr*duration_x10)/10000000ULL;

    if(len==0)
        len=1;

    for(uint32_t i=0;i<len;i++)
    {
        power[start+i]=value;
    }
}



void test_phase_gradient(void)
{
    uint32_t power[200];

    for(int i=0;i<200;i++)
        power[i]=100;

    uint32_t sr=2400000;

    uint32_t ideal=41;

    setup_pulse(power,ideal,sr,0,5,10000);
    setup_pulse(power,ideal,sr,10,5,10000);
    setup_pulse(power,ideal,sr,35,5,10000);
    setup_pulse(power,ideal,sr,45,5,10000);

    /*
     * fake wrong phase energy
     */
    power[40]=3000;

    PhaseSearchResult result;

    int ret =
        phase_search_refine(
            power,
            200,
            40,
            sr,
            &result);

    assert(ret==0);


    printf(
        "rough=%d refined=%d offset=%d score=%llu\n",
        40,
        result.refined_index,
        result.phase_offset,
        result.score);

    assert(result.refined_index==41);
    assert(result.phase_offset==1);
}


int main(void)
{
    printf(
        "=== Phase Search Test ===\n");

    test_phase_gradient();
  
    printf(
        "=== Passed ===\n");

    return 0;
}
