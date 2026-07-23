#include "noise.h"


void noise_init(
        NoiseEstimator *estimator,
        uint32_t initial)
{
    if(!estimator)
        return;


    estimator->floor = initial;


    estimator->attack_shift =
        NOISE_DEFAULT_ATTACK_SHIFT;


    estimator->decay_shift =
        NOISE_DEFAULT_DECAY_SHIFT;
}



void noise_update(
        NoiseEstimator *estimator,
        uint32_t power)
{
    if(!estimator)
        return;

    uint32_t noise =
        estimator->floor;


    if(power > noise)
    {
        /*
         * Slow attack
         *
         * noise += (power-noise)/1024
         *
         * Prevent strong ADS-B pulse
         * from lifting noise floor.
         */

        uint32_t diff =
            power - noise;

        uint32_t step =
            diff >> estimator->attack_shift;

        if(step > 0)
            noise += step;

    }
    else
    {
        /*
         * Fast decay
         *
         * noise -= (noise-power)/16
         *
         * Quickly return to background level.
         */

        uint32_t diff =
            noise - power;

        uint32_t step =
            diff >> estimator->decay_shift;

        /*
         * Avoid fixed-point dead zone
         */
        if(step > 0)
            noise -= step;
    }


    estimator->floor = noise;
}


uint32_t noise_get(
        const NoiseEstimator *estimator)
{
    if(!estimator)
        return 0;

    return estimator->floor;
}
