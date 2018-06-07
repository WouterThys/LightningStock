#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "../Drivers/SYSTEM_Driver.h"
#include "../Drivers/SPI2_Driver.h"
#include "../Settings.h"
#include "NRF_Channels.h"
#include "NRF_Module.h"
#include "NRF_Controller.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define R_REGISTER      0x00
#define W_REGISTER      0x20
#define R_RX_PAYLOAD    0x61
#define W_TX_PAYLOAD    0xA0
#define W_NOP           0xFF

#define R_REGISTER_MSK  0x1F
#define W_REGISTER_MSK  0x1F

// Local defines
#define WFSM_SSetup     0x0001    
#define WFSM_SWrite     0x0002
#define WFSM_SWait      0x0003
#define WFSM_SCheck     0x0004

#define CRC_OK          0x01
#define CRC_NOK         0x00

#define RESEND_MAX      10
#define WAIT_MAX        10

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/
#define NRF_Reset       NRF_CE = 0; NRF_CE = 1; DelayMs(10); NRF_CE = 0  

#define tdSSCK              DelayUs(500) 
#define tSRD                DelayUs(500)
#define tSREADY             DelayUs(500)
#define tEND                DelayUs(100)
#define tSTARTUP            DelayMs(100)

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static nrfStatus_t status;
static bool writeBusy, readFlag, writeFlag;

// Finite state machine 
struct FSM {
    uint16_t     State; // Current state of the Finite State Machine
    uint16_t     NextState; // Next state of the Finite State Machine
}WFsm;

/* Event function pointers */
static void (*nrfInterrupt)(uint8_t source);

/*******************************************************************************
 *          LOCAL FUNCTIONS
*******************************************************************************/
static void    NRF_WriteRegister(uint8_t reg, uint8_t value); // Write to a register
static uint8_t NRF_ReadRegister(uint8_t reg); // Read from a register
static void    NRF_UpdateStatus();
static void    NRF_WritePayload(uint8_t * data, uint16_t length);
static void    NRF_Configure(void); // Configure the registers of the NRF module

void NRF_WriteRegister(uint8_t reg, uint8_t value) {
    NRF_CSN = 0; // Pull SCSN low
    
    spi2Write(W_REGISTER | (reg & W_REGISTER_MSK)); // Write command and address
    nrfSTATUS = spi2Write(value); // Write value

    NRF_CSN = 1; // Pull SCSN high
    DelayUs(1);
}

uint8_t NRF_ReadRegister(uint8_t reg) {
    uint8_t read;
    
    NRF_CSN = 0; // Pull SCSN low
    
    spi2Write(R_REGISTER | (reg & R_REGISTER_MSK)); // Read command and address
    read = spi2Write(0xFF); // Read value

    NRF_CSN = 1; // Pull SCSN high
    DelayUs(1);
    return read;
}

void NRF_UpdateStatus() {
    NRF_CSN = 0; // Pull SCSN low
    
    nrfSTATUS = spi2Write(W_NOP); // Write command and address

    NRF_CSN = 1; // Pull SCSN high
    DelayUs(1);
}

void NRF_WritePayload(uint8_t * data, uint16_t length) {
    NRF_CSN = 0; // Pull SCSN low
    
    spi2Write(W_TX_PAYLOAD); // Write address
    
    int c = 0;
    for(c = 0; c < length; c++) {
        spi2Write(*(data + c));
    }

    NRF_CSN = 1; // Pull SCSN high
    DelayUs(1);
}

void NRF_Configure() {
    // Initial port values
    //NRF_Reset; // Reset
    ////C_DEBUG_WriteMessage("NRF reset");
    //NRF_STALED_0 = 0;
    NRF_CSN = 1; // Active low SCSN pin
    tSTARTUP;
    
    // Write registers
//    nrfCONFIGbits.MASK_RX_DR = 0; // Reflect RX_DR as active low interrupt on IRQ
//    nrfCONFIGbits.MASK_TX_DS = 0; // Reflect TX_DS as active low interrupt on IRQ
//    nrfCONFIGbits.MASK_MAX_RT = 0; // Refect MAX_RT as active low interrupt on IRQ
//    nrfCONFIGbits.EN_CRC = 1; // Enable CRC
//    nrfCONFIGbits.CRCO = 0; // One byte CRC
//    nrfCONFIGbits.PWR_UP = 0; // Keep in StandBy-I mode
//    nrfCONFIGbits.PRIM_RX = 0; // PTX
//    
//    NRF_WriteRegister(nrfCONFIGbits.address, nrfCONFIG);
    
    
    // Test
    uint8_t config;// = NRF_ReadRegister(nrfCONFIGbits.address);
    //printf("After read: %d\n", config);
    
    nrfCONFIGbits.PRIM_RX = 0;
    nrfCONFIGbits.CRCO = 1;
    nrfCONFIGbits.EN_CRC = 1;
    NRF_WriteRegister(nrfCONFIGbits.address, nrfCONFIG);
    
    config = NRF_ReadRegister(nrfCONFIGbits.address);
    printf("After write: %d\n", config);
}

/*******************************************************************************
 *          CONTROLLER FUNCTIONS
*******************************************************************************/
void nrfInitMaster(void (*onInterrupt)(uint8_t source)) {
    
    // Event function pointer
    nrfInterrupt = onInterrupt;
    
    // Initialize ports
    NRF_CE_Dir = 0;
    NRF_CSN_Dir = 0; 
    
    // Initialize SPI module
    spi2Init();
    spi2Enable(true);
    
    // Initialize status
    status.STA = NRF_STATUS_Initialize;
    WFsm.NextState = WFSM_SSetup;
    WFsm.State = WFSM_SSetup;
    writeBusy = false;
    readFlag = false;
    writeFlag = false;
    
    // Enable the interrupts
    NRF_IRQ_Dir = 1;
    RPINR0bits.INT1R = 47; // Connect to RPI47 (RB15)
    INTCON2bits.INT1EP = 1; // Interrupt on negative edge
    _INT1IF = 0; // Clear flag
    _INT1IP = IP_INT1;
    _INT1IE = 1;
    
    // Configure NRF module
    NRF_Configure();
    //NRF_WaitForLink();
    
}

void __attribute__ ( (interrupt, no_auto_psv) ) _INT1Interrupt( void ) {
    (*nrfInterrupt)(NRF_ReadRegister(nrfSTATUSbits.address));
    _INT1IF = 0;        //Clear the INT1 interrupt flag or else
}