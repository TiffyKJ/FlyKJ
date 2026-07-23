#include <stdio.h>
#include <stdint.h>
#include "types.h"
#include "iq_reader.h"
#include "power.h"

int main(int argc, char *argv[]) {
    const char *filepath = (argc > 1) ? argv[1] : "sample.iq";

    IQReader reader;
    if (!iq_reader_open(&reader, filepath)) {
        fprintf(stderr, "Failed to open IQ file: %s\n", filepath);
        return 1;
    }

    #define TEST_SAMPLES 10
    IQSample samples[TEST_SAMPLES];
    size_t count = iq_reader_read(&reader, samples, TEST_SAMPLES);

    if (count != TEST_SAMPLES) {
        printf("Warning: file shorter than expected (%zu samples read)\n", count);
    }

    uint32_t power[TEST_SAMPLES];
    power_calc_buffer(samples, power, count);

    printf("Successfully read %zu samples from '%s':\n", count, filepath);
    for (size_t i = 0; i < count; i++) {
        printf("Sample [%02zu] -> I: %3u, Q: %3u | Power: %u\n", 
               i, samples[i].i, samples[i].q, power[i]);
    }

    iq_reader_close(&reader);
    return 0;
}
