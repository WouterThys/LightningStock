#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <string.h>

#include "Settings.h"
#include "utils.h"
#include "Drivers/SYSTEM_Driver.h"
#include "Drivers/UART_Driver.h"

#include "Controllers/NRF_Controller.h"



/*******************************************************************************
 *          DEFINES
 ******************************************************************************/

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/


/*******************************************************************************
 *          DEFINES
 ******************************************************************************/


/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/


/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();
static void uartReadDone(UartData_t data);

void initialize() {
    sysInterruptEnable(false);
    
    // Initialize system
    sysInitPll();
    sysInitOscillator();
    sysInitPorts();
    
    // Interrupts
    sysInitInterrupts();
    sysInterruptEnable(true);
}

void uartReadDone(UartData_t data) {
    
}


/*******************************************************************************
 *          MAIN PROGRAM
 ******************************************************************************/

int main(void) {
    initialize();
    
    // Initialize UART
    uartDriverInit(UART1_BAUD, &uartReadDone);
    uartDriverEnable(true);
    DelayMs(10);
    
    // Initialize nRF
    nrfInit();
    
    DelayMs(10);
    printf("panda");
    
    while(1) {
        
    }
}