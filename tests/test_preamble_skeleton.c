/*
 * ============================================================
 * REFERENCE TEST NOTICE
 *
 * Commit: 006-1
 *
 * Test:
 *     Preamble Skeleton Test
 *
 *
 * Purpose:
 *
 *     - Verify API
 *     - Verify timing offsets
 *     - Verify build system
 *
 *
 * This does NOT test real ADS-B reception.
 *
 * Production tests:
 *
 *     Commit 006-2
 *
 * ============================================================
 */


#include <stdio.h>
#include <assert.h>

#include "preamble_skeleton.h"



void test_fake_preamble(void)
{

    uint32_t power[100];


    for(int i=0;i<100;i++)
    {
        power[i]=100;
    }


    /*
     * Sample rate:
     *
     * 2 MHz
     *
     *
     * spacing_05us:
     *
     * 1 sample
     *
     *
     * Base:
     *
     * index 20
     *
     *
     * Pulse positions:
     *
     * 0.0us -> 20
     *
     * 1.0us -> 22
     *
     * 3.5us -> 27
     *
     * 4.5us -> 29
     */


    power[20]=20000;
    power[22]=20000;
    power[27]=20000;
    power[29]=20000;


    PreambleSkeleton detector;



    preamble_skeleton_init(
            &detector,
            2000000,
            5);



    int index =
        preamble_skeleton_detect(
                &detector,
                power,
                100,
                100);



    printf(
        "Detected index = %d (Expected:20)\n",
        index);



    assert(index == 20);
}


int main(void)
{
    printf(
        "=== Preamble Skeleton Test ===\n");

    test_fake_preamble();

    printf(
        "=== Passed ===\n");

    return 0;
}
