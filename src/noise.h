#ifndef NOISE_H
#define NOISE_H

#include <stdint.h>


/*
 * Noise tracker parameters
 *
 * Attack:
 *   slow increase
 *   prevent ADS-B burst from contaminating noise floor
 *
 * Decay:
 *   fast decrease
 *   recover after signal disappears
 */

#define NOISE_DEFAULT_ATTACK_SHIFT 10
#define NOISE_DEFAULT_DECAY_SHIFT   4



typedef struct
{
    uint32_t floor;

    uint8_t attack_shift;
    uint8_t decay_shift;

} NoiseEstimator;



/*
 * Initialize noise estimator
 *
 * initial:
 *     initial noise floor estimate
 */
void noise_init(
        NoiseEstimator *estimator,
        uint32_t initial);



/*
 * Feed one power sample
 *
 * power:
 *     current signal power
 */
void noise_update(
        NoiseEstimator *estimator,
        uint32_t power);



/*
 * Get current noise floor
 */
uint32_t noise_get(
        const NoiseEstimator *estimator);



#endif
