#ifndef NRF_CONTROLLER_H
#define	NRF_CONTROLLER_H

#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
    
/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/ 
// Link defines    
#define NRF_NPPF        8
#define NRF_LNK_WTRH    10
#define NRF_LNK_ETRH    50
#define NRF_LNK_OK      0x0000
#define NRF_LNK_WRN     0x0001
#define NRF_LNK_ERR     0x0002    

// Status defines    
#define NRF_STATUS_Initialize       0x00
#define NRF_STATUS_Running          0x01    
#define NRF_STATUS_LinkEstablihed   0x01
#define NRF_STATUS_IntNone          0x00
#define NRF_STATUS_IntTD            0x10
#define NRF_STATUS_IntDR            0x08
#define NRF_STATUS_IntLB            0x40
#define NRF_STATUS_IntClear         0x58   
      
typedef struct NRF_STATUS {
    uint8_t STA; // Status: Initialising, Running, .............
    uint8_t LNK; // Established link or not.
    uint8_t INT; // Interrupt.
    uint8_t PWR; // Transceiver output power.
} nrfStatus_t;   

typedef struct NRF_DATA {
    uint8_t command; // Command
    uint8_t data; // Data
    uint8_t crc; // Control
} nrfData_t;
    
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
void nrfInitMaster(void (*onInterrupt)(uint8_t source));
/**
 * Check interrupts, should be called when IRQ pin is changed
 */
void nrfCheckInterrupts(void); 
/**
 * Write data to the other chip
 * @param data: stuct with command and data.
 */
void nrfWriteData(nrfData_t data);
/**
 * Read data to the other chip
 * @param data: stuct with command and data.
 */
void nrfReadData(nrfData_t data);


#endif