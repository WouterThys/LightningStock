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
#include "Drivers/SPI2_Driver.h"
#include "Controllers/NRF_Controller.h"




/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define MY_ADDRESS  0xAA


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
    LED1 = 1;
    
    // Initialize UART
    uartDriverInit(UART1_BAUD, &uartReadDone);
    uartDriverEnable(true);
    DelayMs(100);
    
    // Initialize nRF
    nrfInit(MY_ADDRESS, &nrfInterrupt);
    DelayMs(10);
    printf("MASTER INIT");
    DelayMs(20);
    
    // Test
    uint8_t data[10] = {1,2,3, 4, 5, 6, 7, 8, 9, 10};
    nrfWrite(MY_ADDRESS, data, 10);
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
            
            DelayMs(2000);
            uint8_t data[10] = {1,2,3, 4, 5, 6, 7, 8, 9, 10};
            nrfWrite(MY_ADDRESS, data, 10);
        }
        
        
    }
}