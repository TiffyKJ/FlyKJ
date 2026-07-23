#ifndef TYPES_H
#define TYPES_H


#include <stdint.h>
#include <stdbool.h>


/*
 * Raw Mode-S message
 *
 * ADS-B extended squitter:
 *
 * 112 bits = 14 bytes
 *
 * This structure contains only
 * raw received frame data.
 */
typedef struct
{
    uint8_t data[14];

    /*
     * Microseconds since receiver start
     */
    uint64_t timestamp;

} ModeSMessage;




/*
 * Decoded ADS-B message result
 */
typedef struct
{
    uint32_t icao;


    bool has_callsign;

    char callsign[9];


    bool has_position;

    double lat;

    double lon;

    int altitude;


    bool has_velocity;

    float velocity;

    float heading;


} AdsbDecodedData;



/*
 * CPR even/odd frame cache
 *
 * Used for global position decoding.
 */
typedef struct
{

    uint32_t even_lat;

    uint32_t even_lon;


    uint32_t odd_lat;

    uint32_t odd_lon;


    uint64_t even_time;

    uint64_t odd_time;


    bool has_even;

    bool has_odd;

} CPRState;




/*
 * Aircraft tracking object
 *
 * Stored in aircraft database.
 */
typedef struct
{
    uint32_t icao;

    AdsbDecodedData data;

    CPRState cpr;

    /*
     * Microseconds since receiver start
     */
    uint64_t last_seen;

} Aircraft;


#endif
