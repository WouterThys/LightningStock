#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "../Settings.h"
#include "SPI2_Driver.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static uint8_t readData;
static bool readDone;

/*******************************************************************************
 *          BASIC FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/
void spi2Init() {
    /* Disable SPI */
    spi2Enable(false);

    /* SPI2CON1 Register */
    SPI2CON1bits.DISSCK = 0;        // Internal SPI clock is enabled
    SPI2CON1bits.DISSDO = 0;        // SD02 pin is controlled by the module
    SPI2CON1bits.MODE16 = 0;        // Communication is byte-wide (8-bit)
    SPI2CON1bits.CKE = 1;           // Serial output data changes on transition from Idle clock state to active clock state
    SPI2CON1bits.CKP = 0;           // Idle state for clock is a low level; active state is a high level mode
    SPI2CON1bits.MSTEN = 1;         // Master mode
    SPI2CON1bits.SPRE = 0b000;       // Secondary pre-scale 8:1
    SPI2CON1bits.PPRE = 0b00;       // Primary pre-scale 64:1

    /* SPI2CON2 Register */
    SPI2CON2bits.FRMEN = 0;         // Framed SPI2 support is enabled (SSx pin used as frame sync pule input/output)
    SPI2CON2bits.SPIBEN = 0;        // Enhanced Buffer is disabled

    /* Interrupts */
    _SPI2IF = 0; // Clear flag
    _SPI2IP = SPI2_IP; // Priority
    _SPI2IE = 1; // Enable interrupt
}

void spi2Enable(bool enable) {
    if (enable) {
        // Ports
        SPI2_SDO_Dir = 0;           // SDO output   (RB11)
        SPI2_SCK_Dir = 0;           // SCK output   (RB10)
        SPI2_SDI_Dir = 1;           // SDI input    (RB12)
        
        // Registers
        RPOR4bits.RP42R = SPI2_SCK_Map;
        RPOR4bits.RP43R = SPI2_SDO_Map;
        RPINR22bits.SDI2R = SPI2_SDI_Map;
        
        SPI2STATbits.SPIEN = 1;     // Enable SPI2
    } else {
        SPI2STATbits.SPIEN = 0;     // Disable 
    }
}

uint8_t spi2Write(uint8_t data) {
    readDone = false;
    SPI2BUF = data;
    while(!readDone);
    return readData;
}


/*******************************************************************************
 *          INTERRUPTS
 ******************************************************************************/

// SPI TX done
void __attribute__ ( (interrupt, no_auto_psv) ) _SPI2Interrupt(void) {
    if (_SPI2IF) {
        readData = SPI2BUF;
        readDone = true;
        _SPI2IF = 0;
    }
}

