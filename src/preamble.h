#ifndef PREAMBLE_H
#define PREAMBLE_H

#include <stdint.h>


/*
 * ADS-B 1090ES Preamble Detector
 *
 * Commit 006-2
 *
 * Function:
 *
 * Power Envelope
 *       |
 *       v
 * Preamble Candidate Index
 *
 *
 * Detection:
 *
 * 1. Pulse Energy Gate
 *
 * 2. Gap/Pulse Ratio Gate
 */

typedef struct
{
    /*
     * SDR sample rate
     *
     * Example:
     *
     * 2000000
     * 2400000
     */
    uint32_t sample_rate;


    /*
     * Pulse threshold multiplier
     *
     * threshold =
     *
     * noise * factor
     */
    uint32_t threshold_factor;


} PreambleDetector;


void preamble_init(
        PreambleDetector *detector,
        uint32_t sample_rate,
        uint32_t threshold_factor);



/*
 * Search ADS-B preamble
 *
 *
 * return:
 *
 * >=0:
 *      rough frame start index
 *
 * -1:
 *      not found
 */
int preamble_detect(
        PreambleDetector *detector,
        const uint32_t *power,
        uint32_t length,
        uint32_t noise);


#endif
