#ifndef PREAMBLE_H
#define PREAMBLE_H

#include <stdint.h>

typedef struct
{
    uint32_t sample_rate;
    uint32_t threshold_factor;
    uint32_t pulse_window;

} PreambleDetector;

void preamble_init(
        PreambleDetector *detector,
        uint32_t sample_rate,
        uint32_t threshold_factor);

int preamble_detect(
        PreambleDetector *detector,
        const uint32_t *power,
        uint32_t length,
        uint32_t noise);

#endif
