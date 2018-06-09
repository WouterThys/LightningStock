#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "SPI1_Driver.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
uint16_t SPI1_ReadData;

/*******************************************************************************
 *          BASIC FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/
void spi1Init() {
    /* Disable SPI */
    spi1Enable(false);

    /* SPI1CON1 Register */
    SPI1CON1bits.DISSCK = 0;        // Internal SPI clock is enabled
    SPI1CON1bits.DISSDO = 0;        // SD01 pin is controlled by the module
    SPI1CON1bits.MODE16 = 1;        // Communication is word-wide (16-bit)
    SPI1CON1bits.CKE = 1;           // Serial output data changes on transition from Idle clock state to active clock state
    SPI1CON1bits.CKP = 0;           // Idle state for clock is a low level; active state is a high level mode
    SPI1CON1bits.MSTEN = 1;         // Master mode
    SPI1CON1bits.SPRE = 0b00;       // Secondary pre-scale 8:1
    SPI1CON1bits.PPRE = 0b00;       // Primary pre-scale 64:1

    /* SPI1CON2 Register */
    SPI1CON2bits.FRMEN = 0;         // Framed SPI1 support is enabled (SSx pin used as frame sync pule input/output)
    SPI1CON2bits.SPIBEN = 0;        // Enhanced Buffer is disabled

    /* Interrupts */
    _SPI1IF = 0; // Clear flag
    _SPI1IP = SPI1_IP; // Priority
    _SPI1IE = 1; // Enable interrupt
}

void spi1Enable(bool enable) {
    if (enable) {
        // Ports
        SPI1_SDO_Dir = 0;           // SDO output   (RB10)
        SPI1_SCK_Dir = 0;           // SCK output   (RB11)
        
        // Registers
        RPOR4bits.RP43R = SPI1_SCK_Map;
        RPOR4bits.RP42R = SPI1_SDO_Map;
        RPINR22bits.SDI2R = SPI1_SDI_Map;
        
        SPI1STATbits.SPIEN = 1;     // Enable SPI1
    } else {
        SPI1STATbits.SPIEN = 0;     // Disable 
    }
}

void spi1Write(uint16_t data) {
    SPI1BUF = data;
}


/*******************************************************************************
 *          INTERRUPTS
 ******************************************************************************/

// SPI TX done
void __attribute__ ( (interrupt, no_auto_psv) ) _SPI1Interrupt(void) {
    if (_SPI1IF) {
        SPI1_ReadData = SPI1BUF;
        _SPI1IF = 0;
    }
}

