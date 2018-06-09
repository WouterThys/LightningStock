#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "../Drivers/SYSTEM_Driver.h"
#include "../Drivers/SPI2_Driver.h"
#include "../Settings.h"
#include "NRF_Module.h"
#include "NRF_Controller.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define R_REGISTER      0x00
#define W_REGISTER      0x20
#define R_RX_PAYLOAD    0x61
#define W_TX_PAYLOAD    0xA0
#define FLUSH_TX        0xE1
#define FLUSH_RX        0xE2
#define W_NOP           0xFF

#define R_REGISTER_MSK  0x1F
#define W_REGISTER_MSK  0x1F

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/
#define NRF_Reset       NRF_CE = 0; NRF_CE = 1; DelayMs(10); NRF_CE = 0  
#define NrfWrite(d)     spi2Write(d); 

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static nrfIrq_t nrfIrqState;
static uint8_t lastAddress;
static uint8_t thisAddress;

/* Event function pointers */
static void (*nrfInterrupt)(nrfIrq_t irqState);

/*******************************************************************************
 *          LOCAL FUNCTIONS
*******************************************************************************/
static void    NRF_WriteRegister(uint8_t reg, uint8_t value); // Write to a register
static uint8_t NRF_ReadRegister(uint8_t reg); // Read from a register
static void    NRF_UpdateStatus();
static void    NRF_WritePayload(uint8_t * data, uint16_t length);
static void    NRF_ReadPayload();
static void    NRF_FlushTx();
static void    NRF_FlushRx();
static void    NRF_ClearInterrupts();
static void    NRF_Configure(void); // Configure the registers of the NRF module

static void    NRF_PrepareWrite(uint8_t address);
static void    NRF_PrepareRead(uint8_t address);

void NRF_WriteRegister(uint8_t reg, uint8_t value) {
    NRF_CSN = 0; // Pull SCSN low
    
    NrfWrite(W_REGISTER | (reg & W_REGISTER_MSK)); // Write command and address
    nrfSTATUS = spi2Write(value); // Write value

    NRF_CSN = 1; // Pull SCSN high
    
}

uint8_t NRF_ReadRegister(uint8_t reg) {
    uint8_t read = 0;
    NRF_CSN = 0; // Pull SCSN low
    
    NrfWrite(R_REGISTER | (reg & R_REGISTER_MSK)); // Read command and address
    read = NrfWrite(0xFF); // Read value

    NRF_CSN = 1; // Pull SCSN high
    DelayUs(1);
    
    return read;
}

void NRF_UpdateStatus() {
    NRF_CSN = 0; // Pull SCSN low
    
    nrfSTATUS = NrfWrite(W_NOP); // Write command and address

    NRF_CSN = 1; // Pull SCSN high
    DelayUs(1);
}

void NRF_WritePayload(uint8_t * data, uint16_t length) {
    
    NRF_CE = 0;
    NRF_CSN = 0; // Pull SCSN low
    
    NrfWrite(W_TX_PAYLOAD); // Write to TX-FIFO command
    
    // Send data to TX-FIFO while kept CSN low
    uint16_t l = 0;
    while (l < length) {
        NrfWrite(data[l]);
        l++;
    }
    
    NRF_CSN = 1; // Pull SCSN high
    DelayUs(1);
    
    // Send payload
    NRF_CE = 1;
    DelayUs(12);
    NRF_CE = 0;
    DelayUs(1);
}

void NRF_ReadPayload() {
    NRF_CSN = 0; // Pull SCSN low
    
    NrfWrite(R_RX_PAYLOAD); // Write to RX-FIFO command
    
//    // Send data to TX-FIFO while kept CSN low
//    int l = 0;
//    while (l < length) {
//        spi2Write(*(data + l));
//        l++;
//    }
//
//    NRF_CSN = 1; // Pull SCSN high
//    
//    // Send payload
//    NRF_CE = 1;
//    DelayUs(10);
//    NRF_CE = 0;
    
    DelayUs(1);
}

void NRF_FlushTx() {
     NRF_CSN = 0; // Pull SCSN low
    
    nrfSTATUS = NrfWrite(FLUSH_TX); // Write command

    NRF_CSN = 1; // Pull SCSN high
    DelayUs(1);
}

void NRF_FlushRx() {
    NRF_CSN = 0; // Pull SCSN low
    
    nrfSTATUS = NrfWrite(FLUSH_RX); // Write command

    NRF_CSN = 1; // Pull SCSN high
    DelayUs(1);
}

void NRF_ClearInterrupts() {
    nrfSTATUS = 0x70;
    NRF_WriteRegister(nrfSTATUSbits.address, nrfSTATUS);
}

void NRF_PrepareWrite(uint8_t address) {
    
    // Clear 
    NRF_CE = 0;
    NRF_FlushTx();
    NRF_ClearInterrupts();
    
    // Set as PTX
    nrfCONFIGbits.PRIM_RX = 0;
    NRF_WriteRegister(nrfCONFIGbits.address, nrfCONFIG);
    
    // No ack
    nrfEN_AAbits.ENAA_P0 = 1;
    NRF_WriteRegister(nrfEN_AAbits.address, nrfEN_AA);
    
    
    // Write addresses
    //if (lastAddress != address) {
    //    NRF_WriteRegister(nrfTX_ADDRbits.address, address);
    //    NRF_WriteRegister(nrfRX_ADDR_P0bits.address, address);
    //    NRF_WriteRegister(nrfRX_ADDR_P3bits.address, address);
    //}
    
    lastAddress = address;
}

void NRF_PrepareRead(uint8_t address) {
    
}




void NRF_Configure() {
    NRF_CSN = 1; // Active low SCSN pin
    DelayMs(10);
    
    NRF_FlushRx();
    NRF_FlushTx();
    NRF_ClearInterrupts();
    
    // CONFIG
    nrfCONFIGbits.PWR_UP = 1;  // Power up
    NRF_WriteRegister(nrfCONFIGbits.address, nrfCONFIG);
    
    // SETUP_AW
    //nrfSETUP_AWbits.AW = 0b01; // 3 bytes address length
    //NRF_WriteRegister(nrfSETUP_AWbits.address, nrfSETUP_AW);
    
    // SETUP_RETR
    //nrfSETUP_RETRbits.ADR = 0b0000; // Wait 250µs 
    //nrfSETUP_RETRbits.ARC = 5; // Retry 5 times
    //NRF_WriteRegister(nrfSETUP_RETRbits.address, nrfSETUP_RETR);
    
    // RF_SETUP
    //nrfRF_SETUPbits.RF_DR_LOW = 0;
    //nrfRF_SETUPbits.RF_DR_HIGH = 1; // 1Mbps
    //nrfRF_SETUPbits.RF_PWR = 0b00; // 0dBm
    //NRF_WriteRegister(nrfRF_SETUPbits.address, nrfSETUP_RETR);
    
    // Wait to power up
    DelayMs(100);
}

/*******************************************************************************
 *          CONTROLLER FUNCTIONS
*******************************************************************************/
void nrfInit(uint8_t address, void (*onInterrupt)(nrfIrq_t irqState)) {
    
    // Event function pointer
    nrfInterrupt = onInterrupt;
    
    // Address
    thisAddress = address;
    
    // Initialize ports
    NRF_CE_Dir = 0;
    NRF_CSN_Dir = 0; 
    NRF_CE = 0;
    
    // Initialize SPI module
    spi2Init();
    spi2Enable(true);
    
    // Last device
    lastAddress = 0xFF;
    
    // IRQ state
    nrfIrqState.maxRetry = false;
    nrfIrqState.readReady = false;
    nrfIrqState.sendReady = false;
    nrfIrqState.rxPipeNo = 0b111; // RX FIFO empty
    nrfIrqState.txFull = false;
    
    // Enable the interrupts
    NRF_IRQ_Dir = 1;
    RPINR0bits.INT1R = 47; // Connect to RPI47 (RB15)
    INTCON2bits.INT1EP = 1; // Interrupt on negative edge
    _INT1IF = 0; // Clear flag
    _INT1IP = IP_INT1;
    _INT1IE = 1;
    
    // Configure NRF module
    NRF_Configure();    
}

void nrfWrite(uint8_t address, uint8_t * data, uint16_t length) {
    NRF_PrepareWrite(address);
    NRF_WritePayload(data, length);
}

void __attribute__ ( (interrupt, no_auto_psv) ) _INT1Interrupt( void ) {
    
    // Read STATUS register to determine IRQ source
    NRF_UpdateStatus();
    
    // Maximum retry interrupt
    if (nrfSTATUSbits.MAX_RT) {
        nrfIrqState.maxRetry = true;
    } else {
        nrfIrqState.maxRetry = false;
    }
    
    // Read ready interrupt
    if (nrfSTATUSbits.RX_DR) {
        nrfIrqState.readReady = true;
    } else {
        nrfIrqState.readReady = false;
    }
    
    // Send ready interrupt
    if (nrfSTATUSbits.TX_DS) {
        nrfIrqState.sendReady = true;
    } else {
        nrfIrqState.sendReady = false;
    }
    
    nrfIrqState.rxPipeNo = nrfSTATUSbits.RX_P_NO;
    nrfIrqState.txFull = nrfSTATUSbits.TX_FULL;
    
    // Write to clear interrupts
    NRF_ClearInterrupts();
    
    (*nrfInterrupt)(nrfIrqState);
    _INT1IF = 0;        //Clear the INT1 interrupt flag or else
}