#ifndef IQ_READER_H
#define IQ_READER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef struct
{
    uint8_t i;
    uint8_t q;

}
IQSample;

typedef struct
{
    FILE* file;

    uint64_t samples_read;

}
IQReader;

bool iq_reader_open(
        IQReader* reader,
        const char* filepath);


size_t iq_reader_read(
        IQReader* reader,
        IQSample* buffer,
        size_t max_samples);


void iq_reader_close(
        IQReader* reader);


#endif
