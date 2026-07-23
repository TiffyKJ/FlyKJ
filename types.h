#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>


/*
 * Raw Mode-S message
 * 112 bits = 14 bytes
 */
typedef struct
{
    uint8_t data[14];

    // microseconds since receiver start
    uint64_t timestamp;

}
ModeSMessage;



/*
 * Result of decoding one ADS-B message
 */
typedef struct
{
    uint32_t icao;


    bool has_callsign;
    char callsign[9];


    bool has_position;
    double lat;
    double lon;


    bool has_velocity;
    float velocity;
    float heading;


    int altitude;
}
AdsbDecodedData;



/*
 * CPR even/odd frame cache
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
}
CPRState;



/*
 * Tracked aircraft
 */
typedef struct
{
    uint32_t icao;
    AdsbDecodedData data;
    CPRState cpr;
    uint64_t last_seen;
}
Aircraft;
#endif
