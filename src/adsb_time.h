#ifndef ADSB_TIME_H
#define ADSB_TIME_H


/*
 * ADS-B 1090ES Preamble Timing
 *
 * Unit:
 *
 * 0.1 us
 *
 *
 * Single Source of Truth
 *
 * Used by:
 *
 * preamble.c
 * phase.c
 */


#define ADSB_P0_START_X10 0
#define ADSB_P0_END_X10   5

#define ADSB_G0_START_X10 5
#define ADSB_G0_END_X10   10

#define ADSB_P1_START_X10 10
#define ADSB_P1_END_X10   15

#define ADSB_G1_START_X10 15
#define ADSB_G1_END_X10   35

#define ADSB_P2_START_X10 35
#define ADSB_P2_END_X10   40

#define ADSB_G2_START_X10 40
#define ADSB_G2_END_X10   45

#define ADSB_P3_START_X10 45
#define ADSB_P3_END_X10   50


/*
 * Whole preamble evaluation window
 *
 * 0 ~ 5.0us
 */
#define ADSB_PREAMBLE_END_X10 50


#endif
