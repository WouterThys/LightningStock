#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <string.h>

#include "Settings.h"
#include "utils.h"
#include "Controllers/NRF_Controller.h"
#include "Drivers/SYSTEM_Driver.h"
#include "Drivers/UART_Driver.h"





/*******************************************************************************
 *          DEFINES
 ******************************************************************************/

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/


/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
uint8_t ADR1[] = {0xA1, 0xB2, 0xC3};

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static bool nrfIrqFlag = false;
static nrfIrq_t nrfIrq;

static nrfDevice_t device1 = { 1, {0xA1, 0xB2, 0xC3}, false, false };

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
    LED1 = 1;
    
    // Initialize UART
    uartDriverInit(UART1_BAUD, &uartReadDone);
    uartDriverEnable(true);
    DelayMs(100);
    
    // Initialize nRF
    nrfInit(&nrfInterrupt);
    DelayMs(10);
    
    // Test
    uint8_t data[10] = {1,2,3, 4, 5, 6, 7, 8, 9, 10};
    nrfWrite(&device1, data, 10);
    // Test
    
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
        }
        
        
    }
}