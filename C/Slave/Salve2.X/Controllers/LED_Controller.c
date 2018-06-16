#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "../Drivers/SYSTEM_Driver.h"
#include "../Settings.h"
#include "LED_Controller.h"

// MCP activates rows
// NXP activates RGB columns

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/


/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/


/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static LedLocation_t ledLocations [LED_ROWS] [LED_COLS];

/*******************************************************************************
 *          LOCAL FUNCTIONS
*******************************************************************************/


/*******************************************************************************
 *          CONTROLLER FUNCTIONS
*******************************************************************************/
void ledInit() {
    
    
    // Create LED location array
    uint8_t r,c;
    for (r = 0; r < LED_ROWS; r++) {
        for (c = 0; c < LED_COLS; c++) {
            ledLocations[r][c].row = r;
            ledLocations[r][c].col = c;
            ledLocations[r][c].RGB = 0x00;
        }
    }
    
}