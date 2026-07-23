#ifndef PHASE_H
#define PHASE_H


#include <stdint.h>

typedef struct
{

    /*
     * Refined P0 start index
     */
    uint32_t refined_index;


    /*
     * Matching score:
     *
     * Epulse - Egap
     */
    uint64_t score;


    /*
     * Total pulse energy
     */
    uint64_t pulse_energy;


    /*
     * Relative offset:
     *
     * -1
     *  0
     * +1
     */
    int32_t phase_offset;

} PhaseSearchResult;


int phase_search_refine(
        const uint32_t *power,
        uint32_t length,
        uint32_t rough_index,
        uint32_t sample_rate,
        PhaseSearchResult *result);

#endif
