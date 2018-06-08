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
#define FLUSH_TX        0xE1
#define FLUSH_RX        0xE2
#define W_NOP           0xFF

#define R_REGISTER_MSK  0x1F
#define W_REGISTER_MSK  0x1F

#define CRC_OK          0x01
#define CRC_NOK         0x00

#define RESEND_MAX      10
#define WAIT_MAX        10

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/
#define NRF_Reset       NRF_CE = 0; NRF_CE = 1; DelayMs(10); NRF_CE = 0  

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static nrfIrq_t nrfIrqState;
static nrfDevice_t * lastTxDevice;

/* Event function pointers */
static void (*nrfInterrupt)(nrfIrq_t irqState);

/*******************************************************************************
 *          LOCAL FUNCTIONS
*******************************************************************************/
static void    NRF_WriteRegister(uint8_t reg, uint8_t value); // Write to a register
static void    NRF_ReadRegister(uint8_t reg, uint8_t * value); // Read from a register
static void    NRF_WriteAddress(uint8_t reg, uint8_t * address, uint16_t length);
static void    NRF_UpdateStatus();
static void    NRF_WritePayload(uint8_t * data, uint16_t length);
static void    NRF_ReadPayload();
static void    NRF_FlushTx();
static void    NRF_FlushRx();
static void    NRF_Configure(void); // Configure the registers of the NRF module

static void    NRF_PrepareWrite(nrfDevice_t * device);

void NRF_WriteRegister(uint8_t reg, uint8_t value) {
    NRF_CSN = 0; // Pull SCSN low
    
    spi2Write(W_REGISTER | (reg & W_REGISTER_MSK)); // Write command and address
    nrfSTATUS = spi2Write(value); // Write value

    NRF_CSN = 1; // Pull SCSN high
    DelayUs(1);
}

void NRF_ReadRegister(uint8_t reg, uint8_t * value) {
    NRF_CSN = 0; // Pull SCSN low
    
    spi2Write(R_REGISTER | (reg & R_REGISTER_MSK)); // Read command and address
    *value = spi2Write(0xFF); // Read value

    NRF_CSN = 1; // Pull SCSN high
    DelayUs(1);
}

void NRF_WriteAddress(uint8_t reg, uint8_t * address, uint16_t length) {
    NRF_CSN = 0; // Pull SCSN low
    
    spi2Write(W_REGISTER | (reg & W_REGISTER_MSK)); // Write command and address
    
    // Send data to address register while kept CSN low
    int l = 0;
    while (l < length) {
        spi2Write(*(address + l));
        l++;
    }

    NRF_CSN = 1; // Pull SCSN high
    
    DelayUs(1);
}

void NRF_UpdateStatus() {
    NRF_CSN = 0; // Pull SCSN low
    
    nrfSTATUS = spi2Write(W_NOP); // Write command and address

    NRF_CSN = 1; // Pull SCSN high
    DelayUs(1);
}

void NRF_WritePayload(uint8_t * data, uint16_t length) {
    NRF_CSN = 0; // Pull SCSN low
    NRF_CE = 0;
    
    spi2Write(W_TX_PAYLOAD); // Write to TX-FIFO command
    
    // Send data to TX-FIFO while kept CSN low
    int l = 0;
    while (l < length) {
        spi2Write(*(data + l));
        l++;
    }

    NRF_CSN = 1; // Pull SCSN high
    DelayUs(1);
    
    // Send payload
    NRF_CE = 1;
    DelayUs(10);
    NRF_CE = 0;
}

void NRF_ReadPayload() {
    NRF_CSN = 0; // Pull SCSN low
    
    spi2Write(R_RX_PAYLOAD); // Write to RX-FIFO command
    
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
    
    nrfSTATUS = spi2Write(FLUSH_TX); // Write command

    NRF_CSN = 1; // Pull SCSN high
    DelayUs(1);
}

void NRF_FlushRx() {
    NRF_CSN = 0; // Pull SCSN low
    
    nrfSTATUS = spi2Write(FLUSH_RX); // Write command

    NRF_CSN = 1; // Pull SCSN high
    DelayUs(1);
}

void NRF_PrepareWrite(nrfDevice_t  * device) {
    
    // Clear 
    NRF_CE = 0;
    NRF_FlushTx();
    
    // Set as PTX
    nrfCONFIGbits.PRIM_RX = 0;
    NRF_WriteRegister(nrfCONFIGbits.address, nrfCONFIG);
    
    // Write addresses
    if (lastTxDevice->id != device->id) {
        NRF_WriteAddress(nrfTX_ADDRbits.address, device->address, ADDRESS_LENGTH);
        NRF_WriteAddress(nrfRX_ADDR_P0bits.address, device->address, ADDRESS_LENGTH);
    }
    
    // Device
    device->writeBusy = true;
    lastTxDevice = device;
}





void NRF_Configure() {
    NRF_CSN = 1; // Active low SCSN pin
    DelayMs(10);
    
    
    // TODO: write address width
    // TODO: write power
    // TODO: write channels?
    // TODO: setup CRC, ART, ...
    
    // For PTX
    // Auto ack: ENAA_P0 = 1;
    // NO_ACK = 0
    // Enable auto retransmit
    // ARC_CNT: retransmit count
}

/*******************************************************************************
 *          CONTROLLER FUNCTIONS
*******************************************************************************/
void nrfInitMaster(void (*onInterrupt)(nrfIrq_t irqState)) {
    
    // Event function pointer
    nrfInterrupt = onInterrupt;
    
    // Initialize ports
    NRF_CE_Dir = 0;
    NRF_CSN_Dir = 0; 
    
    // Initialize SPI module
    spi2Init();
    spi2Enable(true);
    
    // Last device
    lastTxDevice->id = -1;
    
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

void nrfWrite(nrfDevice_t  * device, uint8_t * data, uint16_t length) {
    if (device->writeBusy) return;
    NRF_PrepareWrite(device);
    NRF_WritePayload(data, length);
}

void __attribute__ ( (interrupt, no_auto_psv) ) _INT1Interrupt( void ) {
    
    // Read STATUS register to determine IRQ source
    NRF_UpdateStatus();
    
    // Maximum retry interrupt
    if (nrfSTATUSbits.MAX_RT) {
        nrfIrqState.maxRetry = true;
        nrfSTATUSbits.MAX_RT = 1; // To clear 
    } else {
        nrfIrqState.maxRetry = false;
    }
    
    // Read ready interrupt
    if (nrfSTATUSbits.RX_DR) {
        nrfIrqState.readReady = true;
        nrfSTATUSbits.RX_DR = 1; // To clear 
        if (lastTxDevice->id > 0) {
            lastTxDevice->readBusy = false;
        }
    } else {
        nrfIrqState.readReady = false;
    }
    
    // Send ready interrupt
    if (nrfSTATUSbits.TX_DS) {
        nrfIrqState.sendReady = true;
        nrfSTATUSbits.TX_DS = 1; // To clear 
        if (lastTxDevice->id > 0) {
            lastTxDevice->writeBusy = false;
        }
    } else {
        nrfIrqState.sendReady = false;
    }
    
    nrfIrqState.rxPipeNo = nrfSTATUSbits.RX_P_NO;
    nrfIrqState.txFull = nrfSTATUSbits.TX_FULL;
    
    // Write to clear interrupts
    NRF_WriteRegister(nrfSTATUSbits.address, nrfSTATUS);
    
    (*nrfInterrupt)(nrfIrqState);
    _INT1IF = 0;        //Clear the INT1 interrupt flag or else
}