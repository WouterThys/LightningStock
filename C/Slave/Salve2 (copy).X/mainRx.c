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
#define ADDRESS  0xAA

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/


/*******************************************************************************
 *          DEFINES
 ******************************************************************************/


/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static bool nrfIrqFlag = false;
static nrfIrq_t nrfIrq;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();
static void uartReadDone(UartData_t data);
static void nrfInterrupt(nrfIrq_t irqState);

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

void nrfInterrupt(nrfIrq_t irqState) {
    nrfIrq = irqState;
    nrfIrqFlag = true;
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
    nrfInit(ADDRESS, &nrfInterrupt);
    
    DelayMs(10);
    printf("SLAVE INIT\n");
    nrfPrepareRead(ADDRESS, 1);
    printf("Waiting for data..\n");
    
    while(1) {
        
        if (nrfIrqFlag) {
            if (nrfIrq.maxRetry) {
                printf("IRQ: Max retry..\n");
            }
            if (nrfIrq.sendReady) {
                printf("IRQ: Send ready..\n");
            }
            if (nrfIrq.readReady) {
                printf("IRQ: Read ready..\n");
            }
            nrfIrqFlag = false;
           
            uint8_t data[1];
            nrfRead(data, 1);
            
            int i;
            for(i = 0; i < 1; i++) {
                printf("%d, ", data[i]);
            }
        }
    }
}