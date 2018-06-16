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
#include "Controllers/MCP_Controller.h"
#include "Controllers/LED_Controller.h"



/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define NRF_ADDRESS  0xAA

typedef enum {
    S0_PrepareCommand = 0,      // Prepare for command
    S1_WaitForCommand = 1,      // Wait for command
    S2_ReadCommand = 2,
    S3_CheckCommand = 3,
    S4_ReadPayload = 4
} fsmState_t;

typedef struct {
    bool       doFsm;           // Flag to indicate if FSM should be executed
    bool       onIrq;
    fsmState_t currentState;    // Current state of the FSM
    fsmState_t nextState;       // Next state of the FSM
} fsm_t ;

typedef enum {
    FindLink = 0b001,
    LedInfo = 0b010
} nrfCommand_t;

/**
 * Header with command and message length
 * |C2|C1|C0|L5|L4|L3|L2|L1|L0|
 * Cx: 3-bit wide command
 * Lx: 5-bit wide message length
 */
typedef struct {
    nrfCommand_t command;     // Command to send to the receiver
    uint8_t      msgLength;   // Number of bytes that will follow after this command
} nrfHeader_t;



/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/


/*******************************************************************************
 *          DEFINES
 ******************************************************************************/


/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
// NRF
static bool nrfIrqFlag = false;
static bool nrfHasData = false;
static bool nrfLinkOk = false;
static nrfIrq_t nrfIrq;
static uint8_t nrfPayload[32]; // NRF can send max 32 bytes at once
static uint8_t nrfPayloadCnt = -1;
static nrfCommand_t nrfCommand;

// FSM
static fsm_t slaveFsm;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();
static void uartReadDone(UartData_t data);
static void nrfInterrupt(nrfIrq_t irqState);

static void fsmTransition(fsm_t * fsm);
static void fsmExecuteState(fsm_t * fsm);

static void nrfDecode(uint8_t * payload, uint8_t length);

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
    
    // FSM
    slaveFsm.doFsm = true;
    slaveFsm.currentState = S0_PrepareCommand;
    slaveFsm.nextState = S0_PrepareCommand;
    
    // Initialize controllers
    nrfInit(NRF_ADDRESS, &nrfInterrupt);
    mcpInit();
    ledInit();
    
    DelayMs(10);
    printf("SLAVE INIT\n");
    
    uint8_t p = 0;
    
    while(1) {
        
        //
        mcpSetPORTB(p);
        p++;
        DelayMs(500);
        //
        
        if (nrfIrqFlag) {
            if (nrfIrq.maxRetry) {
                nrfLinkOk = false;
                NRF_LINK_LED = nrfLinkOk;
                NRF_ERR_LED = 1;
                printf("IRQ: Max retry..\n");
            }
            if (nrfIrq.sendReady) {
                nrfLinkOk = true;
                NRF_LINK_LED = nrfLinkOk;
                NRF_ERR_LED = 0;
                printf("IRQ: Send ready..\n");
                nrfHasData = true;
            }
            if (nrfIrq.readReady) {
                nrfLinkOk = true;
                nrfHasData = true;
                NRF_LINK_LED = nrfLinkOk;
                NRF_ERR_LED = 0;
                printf("IRQ: Read ready..\n");
            }
            nrfIrqFlag = false;
            slaveFsm.doFsm = true;
            slaveFsm.onIrq = true;
        }
        
        
        // FSM
        if (slaveFsm.doFsm) {
            slaveFsm.doFsm = false;
            
            // Find next state
            fsmTransition(&slaveFsm);
            
            // Execute state
            fsmExecuteState(&slaveFsm);
            
            if (slaveFsm.currentState != slaveFsm.nextState) {
                printf("FSM: S%d -> S%d\n", slaveFsm.currentState, slaveFsm.nextState);
            }
            
            // Done
            slaveFsm.currentState = slaveFsm.nextState;    
            slaveFsm.onIrq = false;
        }
    }
}


/*******************************************************************************
 *          FSM Functions
 ******************************************************************************/
void fsmTransition(fsm_t * fsm) {
    switch(fsm->currentState) {
        case S0_PrepareCommand:
            fsm->nextState = S1_WaitForCommand;
            break;
            
        case S1_WaitForCommand:
            if (nrfHasData) {
                fsm->nextState = S2_ReadCommand;
                nrfHasData = false;
                slaveFsm.doFsm = true;
            }
            break;
            
        case S2_ReadCommand:
            fsm->nextState = S4_ReadPayload;
            break;
            
//        case S3_CheckCommand:
//            if (nrfPayloadCnt > 0) {
//                fsm->nextState = S4_ReadPayload;
//            } else {
//                fsm->nextState = S0_PrepareCommand;
//            }
//            fsm->doFsm = true;
//            break;
            
        case S4_ReadPayload:
            fsm->nextState = S0_PrepareCommand;
            fsm->doFsm = true;
            break;
        
    }
}

void fsmExecuteState(fsm_t * fsm) {
    uint8_t command;
    switch(fsm->currentState) {
        case S0_PrepareCommand:
            nrfPayloadCnt = 0;
            nrfPrepareRead(NRF_ADDRESS, 1);
            break;
            
        case S1_WaitForCommand:
            // Do wait
            break;
            
        case S2_ReadCommand:
            nrfRead(&command, 1);
            
            nrfCommand = ((command >> 5) & 0x07);
            nrfPayloadCnt = command & 0x1F;
            printf("FSM: expecting %d byes\n", nrfPayloadCnt);
            if (nrfPayloadCnt > 0) {
                nrfPrepareRead(NRF_ADDRESS, nrfPayloadCnt);
            } else {
                fsm->nextState = S0_PrepareCommand;
                fsm->doFsm = true;
            }
            break;
            
        case S4_ReadPayload:
            if (nrfPayloadCnt > 0) {
                nrfRead(nrfPayload, nrfPayloadCnt);
            
                nrfDecode(nrfPayload, nrfPayloadCnt);
                nrfPayloadCnt = 0;
            } 
            break;
    }
}


/*******************************************************************************
 *          NRF Functions
 ******************************************************************************/

void nrfDecode(uint8_t * payload, uint8_t length) {
    uint8_t l = 0;
    while(l < length) {
        uint8_t location = payload[l];
        l++;
        uint8_t rgb = payload[l];
        l++;
        
        LedLocation_t loc;
        loc.row = ((location >> 4) & 0x0F);
        loc.col = (location & 0x0F);
        loc.RGB = rgb;
        
        printf(" - r:%d, c=%d, rbg:%d\n", loc.row, loc.col, loc.RGB);
    }
}