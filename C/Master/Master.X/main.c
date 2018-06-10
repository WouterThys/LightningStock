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
#define NRF_ADDRESS  0xAA
#define FIFO_MAX     100

typedef enum {
    S0_FindLink = 0,
    S1_StandBy = 1,
            
    S2_SLI_CreatePayload = 2,   // Create payload and send command to RX
    S3_SLI_WaitForIRQ = 3,      // Wait for IRQ before sending payload
    S4_SLI_SendPayload = 4,     // Send payload to RX
    S5_SLI_WaitForIRQ = 5       // Wait for IRQ before going back
} fsmState_t;

typedef struct {
    bool       doFsm;           // Flag to indicate if FSM should be executed
    bool       onIrq;           // Flag to indicate FSM is set by IRQ
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

/**
 * LED information. Total space is 2 bytes:
 * 
 * Location:
 *  |R3|R2|R1|R0|C3|C2|C1|C0|
 *   Rx: 4-bit wide row
 *   Cx: 4-bit wide column
 * 
 * RGB brightness
 *  |R2|R1|R0|G2|G1|G0|B1|B0|
 *   Rx: 3-bit wide red LED brightness
 *   Gx: 3-bit wide green LED brightness
 *   Bx: 2-bit wide blue LED brightness
 */
typedef struct {
    uint8_t row:4;      // Location row
    uint8_t col:4;      // Location column
    uint8_t RGB;        // RGB LED brightness
} LedLocation_t;

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
static bool nrfLinkOk = false;
static nrfIrq_t nrfIrq;
static uint8_t nrfPayload[32]; // NRF can send max 32 bytes at once
static uint8_t nrfPayloadCnt = -1;

// FSM
static bool  fsmFirst = true;
static fsm_t masterFsm;

// FIFO
static LedLocation_t ledLocationFifo[FIFO_MAX];
static int16_t fifoLength = -1;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();
static void uartReadDone(UartData_t data);
static void nrfInterrupt(nrfIrq_t irqState);

static void fsmTransition(fsm_t * fsm);
static void fsmExecuteState(fsm_t fsm);

static void nrfSendCommand(nrfHeader_t header);
static void nrfFindLink();
static void nrfCreatePayload();
static void nrfSendPayload();

static bool fifoInsert(uint8_t row, uint8_t column, uint8_t rgb);
static bool fifoTake(LedLocation_t * ledLocation);


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
    // Put data in LED buffer
    fifoInsert(1, 2, 0x12);
    fifoInsert(5, 7, 0x34);
    fifoInsert(3, 8, 0x56);
    fifoInsert(1, 1, 0x78);
    masterFsm.doFsm = true;
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
    DelayMs(100);
    
    // FSM
    masterFsm.doFsm = true;
    masterFsm.currentState = S0_FindLink;
    masterFsm.nextState = S0_FindLink;
    
    // Initialize nRF
    nrfInit(NRF_ADDRESS, &nrfInterrupt);
    NRF_LINK_LED = 0;
    NRF_ERR_LED = 0;
    DelayMs(10);
    
    printf("MASTER INIT");
    DelayMs(10);
    
    while(1) {
        
        // NRF interrupt
        if (nrfIrqFlag) {
            if (nrfIrq.maxRetry) {
                nrfLinkOk = false;
                NRF_LINK_LED = 0;
                NRF_ERR_LED = 1;
                printf("IRQ: Max retry..\n");
            }
            if (nrfIrq.sendReady) {
                nrfLinkOk = true;
                NRF_LINK_LED = 1;
                NRF_ERR_LED = 0;
                printf("IRQ: Send ready..\n");
            }
            if (nrfIrq.readReady) {
                nrfLinkOk = true;
                NRF_LINK_LED = 1;
                NRF_ERR_LED = 0;
                printf("IRQ: Read ready..\n");
            }
            nrfIrqFlag = false;
            masterFsm.doFsm = true;
            masterFsm.onIrq = true;
        }
        
        // FSM
        if (masterFsm.doFsm) {
            masterFsm.doFsm = false;
            
            // Find next state
            fsmTransition(&masterFsm);
            
            // Execute state
            fsmExecuteState(masterFsm);
            
            if (masterFsm.currentState != masterFsm.nextState) {
                printf("FSM: S%d -> S%d\n", masterFsm.currentState, masterFsm.nextState);
            }
            
            // Done
            masterFsm.currentState = masterFsm.nextState; 
            masterFsm.onIrq = false;
        }
    }
}

/*******************************************************************************
 *          FSM Functions
 ******************************************************************************/
void fsmTransition(fsm_t * fsm) {
    switch(fsm->currentState) {
        case S0_FindLink:
            if (nrfLinkOk) {
                fsm->nextState = S1_StandBy;
            }
            break;
            
        case S1_StandBy:
            if (fifoLength >= 0) {
                fsm->nextState = S2_SLI_CreatePayload;
            }
            break;
            
        case S2_SLI_CreatePayload:
            fsm->nextState = S3_SLI_WaitForIRQ;
            break;
            
        case S3_SLI_WaitForIRQ:
            if (fsm->onIrq) {
                if (nrfLinkOk) {
                    fsm->nextState = S4_SLI_SendPayload;
                } else {
                    fsm->nextState = S0_FindLink;
                }
                masterFsm.doFsm = true;
            }
            break;
            
        case S4_SLI_SendPayload:
            fsm ->nextState = S5_SLI_WaitForIRQ;
            break;
            
        case S5_SLI_WaitForIRQ:
            if (fsm->onIrq) {
                fsm->nextState = S0_FindLink;
                masterFsm.doFsm = true;
            }
            break;
    }
}

void fsmExecuteState(fsm_t fsm) {
    switch(fsm.currentState) {
        case S0_FindLink:
            if (!nrfLinkOk) {
                if (!fsmFirst) {
                    DelayMs(500);
                }
                fsmFirst = false;
                nrfFindLink();
            }
            break;
            
        case S1_StandBy:
            // Do nothing
            break;
            
        case S2_SLI_CreatePayload:
            nrfCreatePayload();
            nrfHeader_t h = {LedInfo, nrfPayloadCnt};
            nrfSendCommand(h);
            break;
            
        case S3_SLI_WaitForIRQ:
            // Do wait
            break;
            
        case S4_SLI_SendPayload:
            nrfSendPayload();
            break;
            
        case S5_SLI_WaitForIRQ:
            // Do wait
            break;
    }
}


/*******************************************************************************
 *          NRF Functions
 ******************************************************************************/
void nrfSendCommand(nrfHeader_t header) {
    if (header.msgLength <= 32) {
        uint8_t commandByte = header.msgLength + (header.command << 5);
        nrfWrite(NRF_ADDRESS, &commandByte, 1);
    } else {
        printf("NRF: payload %d > 32\n", header.msgLength);
    }
}

void nrfSendPayload() {
    DelayMs(10);
    nrfWrite(NRF_ADDRESS, nrfPayload, nrfPayloadCnt);
}



void nrfFindLink() {
    nrfHeader_t command;
    command.command = FindLink;
    command.msgLength = 0;
    nrfSendCommand(command);
}

void nrfCreatePayload() {
    if (fifoLength >= 0) {
        nrfPayloadCnt = 0;
        
        LedLocation_t loc;
        while ((fifoTake(&loc)) && (nrfPayloadCnt < 32)) {
            
            uint8_t location = (loc.col & 0x0F) + ((loc.row << 4) & 0xF0 );
            nrfPayload[nrfPayloadCnt] = location;
            nrfPayloadCnt++;
            
            nrfPayload[nrfPayloadCnt] = loc.RGB;
            nrfPayloadCnt++;
            
        }
    }
}



/*******************************************************************************
 *          FIFO Functions
 ******************************************************************************/
bool fifoInsert(uint8_t row, uint8_t column, uint8_t rgb) {
    if (fifoLength >= FIFO_MAX) {
        printf("FIFO: buffer overflow\n");
        return false;
    }
    
    // Create
    LedLocation_t loc = {row, column, rgb};
    
    // Add to FIFO
    fifoLength++;
    ledLocationFifo[fifoLength] = loc;
    return true;
}

bool fifoTake(LedLocation_t * ledLocation) {
    if (fifoLength < 0) {
        printf("FIFO: buffer underflow\n");
        return false;
    }
    
    // Take
    *ledLocation = ledLocationFifo[0];
    
    // Shift all
    uint16_t i;
    for (i = 1; i <= fifoLength; i++) {
        ledLocationFifo[i-1] = ledLocationFifo[i];
    }
    fifoLength--;
    return true;
}