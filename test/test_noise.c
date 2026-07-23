#include <stdio.h>
#include <assert.h>

#include "noise.h"


void test_steady_state()
{
    NoiseEstimator est;


    noise_init(
            &est,
            100);


    for(int i=0;i<10000;i++)
    {
        noise_update(
                &est,
                100);
    }

    uint32_t result =
        noise_get(&est);


    printf(
        "[Test 1] Steady State: %u\n",
        result);


    assert(result == 100);
}



void test_burst_immunity()
{
    NoiseEstimator est;


    noise_init(
            &est,
            100);


    /*
     * Background
     */
    for(int i=0;i<100;i++)
    {
        noise_update(
                &est,
                100);
    }



    /*
     * Simulated strong ADS-B burst
     *
     * Stress test:
     * continuous high power
     */
    for(int i=0;i<300;i++)
    {
        noise_update(
                &est,
                20000);
    }


    uint32_t peak =
        noise_get(&est);



    printf(
        "[Test 2] Burst Immunity: %u\n",
        peak);


    /*
     * Noise floor should rise,
     * but must stay controlled.
     */
    assert(peak < 6000);
    assert(peak > 100);


    /*
     * Recovery
     */
    for(int i=0;i<200;i++)
    {
        noise_update(
                &est,
                100);
    }

    uint32_t recovered =
        noise_get(&est);


    printf(
        "[Test 2] Recovery: %u\n",
        recovered);
}



void test_step_response()
{
    NoiseEstimator est;


    noise_init(
            &est,
            100);


    /*
     * Initial environment
     */
    for(int i=0;i<1000;i++)
    {
        noise_update(
                &est,
                100);
    }



    /*
     * Environment noise rises
     */
    for(int i=0;i<5000;i++)
    {
        noise_update(
                &est,
                500);
    }


    uint32_t result =
        noise_get(&est);


    printf(
        "[Test 3] Step Response: %u\n",
        result);


    /*
     * IIR convergence:
     * should approach 500
     */
    assert(result > 490);
    assert(result <= 500);
}



int main()
{
    printf(
        "=== Noise Estimator Tests ===\n");

    test_steady_state();

    test_burst_immunity();

    test_step_response();


    printf(
        "=== All Tests Passed ===\n");

    return 0;
}
