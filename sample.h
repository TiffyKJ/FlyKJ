#ifndef SAMPLE_H
#define SAMPLE_H

#include <stdint.h>

/*
 * Raw 8-bit Unsigned IQ sample from SDR hardware
 */
typedef struct {
    uint8_t i;
    uint8_t q;
} IQSample;

#endif 
