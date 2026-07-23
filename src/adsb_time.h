#ifndef ADSB_TIME_H
#define ADSB_TIME_H


/*
 * ADS-B 1090ES Timing
 *
 * Unit:
 * 0.1 us
 *
 */

/*
 * Preamble
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
 * Payload
 *
 * Preamble:
 *
 * 0 ~ 5 us
 *
 * Protection silence:
 *
 * 5 ~ 8 us
 *
 * Data:
 *
 * starts at 8 us
 *
 */


#define ADSB_PAYLOAD_OFFSET_X10 80



/*
 * ADS-B Frame
 */

#define ADSB_FRAME_BITS 112



/*
 * PPM bit timing
 *
 * one bit:
 *
 * 1.0 us
 *
 * half bit:
 *
 * 0.5 us
 */

#define ADSB_BIT_HALF_X10 5

#define ADSB_BIT_FULL_X10 10



#endif
