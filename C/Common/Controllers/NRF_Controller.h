#ifndef NRF_CONTROLLER_H
#define	NRF_CONTROLLER_H

#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
    
/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/ 

#ifndef ADDRESS_LENGTH 
#define ADDRESS_LENGTH  3
#endif

typedef struct {
    unsigned readReady:1; // Data Ready RX-FIFO interrupt set
    unsigned sendReady:1;  // Data Sent TX-FIFO interrupt set
    unsigned maxRetry:1;  // Maximum number TX retransmits interrupt set
    unsigned rxPipeNo:3;  // Data pipe number for payloayd available on pipe
    unsigned txFull:1;    // TX-FIFO full flag
} nrfIrq_t;

typedef struct {
    uint8_t id;
    uint8_t address[ADDRESS_LENGTH];
    bool    writeBusy;
    bool    readBusy;
} nrfDevice_t;
    
/******************************************************************************/
/* Variables                                                                  */
/******************************************************************************/    

    
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/
/**
 * Initialize the NRF module as TX
 * Initializes pins and interrupt to communicate with the NRF module
 * Should be called in the early initializing phase at startup.
 */
void nrfInit(void (*onInterrupt)(nrfIrq_t irqState));

void nrfWrite(nrfDevice_t * device, uint8_t * data, uint16_t length);

#endif