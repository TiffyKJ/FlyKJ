/*
 * ============================================================
 * REFERENCE IMPLEMENTATION NOTICE
 *
 * Commit: 006-1
 *
 * Module:
 *     ADS-B Preamble Detector Skeleton
 *
 *
 * Current implementation:
 *
 *     - Four pulse timing check
 *     - Threshold comparison
 *     - Sliding window search
 *
 *
 * Missing:
 *
 *     - Pulse energy integration
 *     - Pause region suppression
 *     - Correlation scoring
 *     - Phase search
 *     - Real SDR robustness
 *
 *
 * Replacement:
 *
 *     Commit 006-2
 *     Production Preamble Matcher
 *
 * ============================================================
 */


#include "preamble_skeleton.h"





static int check_preamble(
        const uint32_t *power,
        uint32_t index,
        uint32_t spacing_05us,
        uint64_t threshold)
{

    /*
     * ADS-B 1090ES preamble:
     *
     *
     * Pulse 0:
     *     0.0 us
     *
     * Pulse 1:
     *     1.0 us
     *
     * Pulse 2:
     *     3.5 us
     *
     * Pulse 3:
     *     4.5 us
     *
     *
     * Skeleton:
     *
     * only check four high energy points
     */


    uint32_t p0 =
        power[index];


    uint32_t p1 =
        power[index + spacing_05us * 2];


    uint32_t p2 =
        power[index + spacing_05us * 7];


    uint32_t p3 =
        power[index + spacing_05us * 9];



    if((uint64_t)p0 < threshold)
        return 0;


    if((uint64_t)p1 < threshold)
        return 0;


    if((uint64_t)p2 < threshold)
        return 0;


    if((uint64_t)p3 < threshold)
        return 0;



    return 1;
}



void preamble_skeleton_init(
        PreambleSkeleton *detector,
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



int preamble_skeleton_detect(
        PreambleSkeleton *detector,
        const uint32_t *power,
        uint32_t length,
        uint32_t noise)
{

    if(!detector || !power)
        return -1;



    /*
     * Prevent overflow:
     *
     * noise * factor
     */
    uint64_t threshold =
        (uint64_t)noise *
        detector->threshold_factor;





    /*
     * Skeleton timing model:
     *
     * 0.5us/sample approximation
     *
     *
     * NOTE:
     *
     * Commit 006-2
     * will replace this with:
     *
     * sample_rate * time / 1e6
     */
    uint32_t spacing_05us =
        detector->sample_rate / 2000000;



    if(spacing_05us == 0)
        return -1;





    /*
     * Preamble span:
     *
     * 4.5us pulse position
     *
     * +0.5us pulse width
     *
     * =5us
     */
    uint32_t span =
        spacing_05us * 10;


    for(uint32_t i=0;
        i + span <= length;
        i++)
    {

        if(check_preamble(
                power,
                i,
                spacing_05us,
                threshold))
        {
            return (int)i;
        }

    }

    return -1;
}
