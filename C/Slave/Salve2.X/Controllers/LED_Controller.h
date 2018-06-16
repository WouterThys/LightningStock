#ifndef LED_CONTROLLER_H
#define	LED_CONTROLLER_H

#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
    
/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/ 

/**
 * LED information. Total space is 2 bytes:
 * 
 * Location:
 *  |R3|R2|R1|R0|C3|C2|C1|C0|
 *   Rx: 4-bit wide row
 *   Cx: 4-bit wide column
 * 
 * RGB brightness
 *  |R2|R1|R0|G2|G1|G0|B1|B0|
 *   Rx: 3-bit wide red LED brightness
 *   Gx: 3-bit wide green LED brightness
 *   Bx: 2-bit wide blue LED brightness
 */
typedef struct {
    uint8_t row:4;      // Location row
    uint8_t col:4;      // Location column
    uint8_t RGB;        // RGB LED brightness
} LedLocation_t;


/******************************************************************************/
/* Variables                                                                  */
/******************************************************************************/    

    
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/
void ledInit();

#endif	/* LED_CONTROLLER_H */

