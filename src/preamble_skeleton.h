/*
 * ============================================================
 * REFERENCE IMPLEMENTATION NOTICE
 *
 * Commit: 006-1
 *
 * Module:
 *     ADS-B Preamble Detector Skeleton
 *
 * Status:
 *     Temporary reference implementation
 *
 *
 * Purpose:
 *     - Validate module interface
 *     - Validate ADS-B timing model
 *     - Validate test pipeline
 *
 *
 * This file will be replaced by:
 *
 *     preamble.h
 *
 * in Commit 006-2:
 *
 *     Production Preamble Matcher
 *
 *
 * Do not use this implementation
 * for real RF performance evaluation.
 *
 * ============================================================
 */



#ifndef PREAMBLE_SKELETON_H
#define PREAMBLE_SKELETON_H


#include <stdint.h>



typedef struct
{
    /*
     * Threshold multiplier
     *
     * threshold =
     * noise * threshold_factor
     */
    uint32_t threshold_factor;



    /*
     * IQ sample rate
     */
    uint32_t sample_rate;


} PreambleSkeleton;




/*
 * Initialize skeleton detector
 */
void preamble_skeleton_init(
        PreambleSkeleton *detector,
        uint32_t sample_rate,
        uint32_t threshold_factor);





/*
 * Search ADS-B preamble skeleton
 *
 * return:
 *
 * >=0:
 *      frame start index
 *
 * -1:
 *      not found
 */
int preamble_skeleton_detect(
        PreambleSkeleton *detector,
        const uint32_t *power,
        uint32_t length,
        uint32_t noise);



#endif
