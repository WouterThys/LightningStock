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
#define NRF_WRITE_REG   0x00
#define NRF_READ_REG    0x80   

union TXSTAU TXSTAu;
#define TXSTAbits       TXSTAu.TXSTAb
#define TXSTA           TXSTAu.TXSTAr 
union TXMODU TXMODu;
#define TXMODbits       TXMODu.TXMODb
#define TXMOD           TXMODu.TXMODr
union I2SCNF_INU I2SCNF_INu;
#define I2SCNF_INbits   I2SCNF_INu.I2SCNF_INb
#define I2SCNF_IN       I2SCNF_INu.I2SCNF_INr
union RXMODS RXMODs;
#define RXMODbits       RXMODs.RXMODb
#define RXMOD           RXMODs.RXMODr
union I2SCNF_OUTU I2SCNF_OUTu;
#define I2SCNF_OUTbits  I2SCNF_OUTu.I2SCNF_OUTb
#define I2SCNF_OUT      I2SCNF_OUTu.I2SCNF_OUTr
union LNKSTAU LNKSTAu;
#define LNKSTAbits      LNKSTAu.LNKSTAb
#define LNKSTA          LNKSTAu.LNKSTAr
union LNKMODU LNKMODu;
#define LNKMODbits      LNKMODu.LNKMODb
#define LNKMOD          LNKMODu.LNKMODr
union INTSTAU INTSTAu;
#define INTSTAbits      INTSTAu.INTSTAb
#define INTSTA          INTSTAu.INTSTAr
union INTCFU INTCFu;
#define INTCFbits       INTCFu.INTCFb
#define INTCF           INTCFu.INTCFr
union TXRESOU TXRESOu;
#define TXRESObits      TXRESOu.TXRESOb
#define TXRESO          TXRESOu.TXRESOr
union TESTCHU TESTCHu;
#define TESTCHbits      TESTCHu.TESTCHb
#define TESTCH          TESTCHu.TESTCHr

uint8_t TXCOUNTr;
#define TXCOUNT TXCOUNTr
uint8_t RXCOUNTr;
#define RXCOUNT RXCOUNTr
uint8_t DTXSTAr;
#define DTXSTA DTXSTAr
uint8_t BCHDr;
#define BCHD BCHDr
uint8_t NBCHr;
#define NBCH NBCHr
uint8_t NACHr;
#define NACH NACHr
uint8_t NLCHr;
#define NLCH NLCHr
uint8_t MDURr;
#define MDUR MDURr
uint8_t TXLATr;
#define TXLAT TXLATr
uint8_t TXPWRr;
#define TXPWR TXPWRr
uint8_t RXPWRr;
#define RXPWR RXPWRr
uint8_t LNKCSTATEr;
#define LNKCSTATE LNKCSTATEr   
uint8_t TESTREGr;
#define TESTREG TESTREGr

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
#define SPI_Write(data) spi2Write(data)
#define NRF_Reset       NRF_RES = 0; NRF_RES = 1; DelayMs(10); NRF_RES = 0  

#define tdSSCK              DelayUs(500) 
#define tSRD                DelayUs(500)
#define tSREADY             DelayUs(500)
#define tEND                DelayUs(100)
#define tSTARTUP            DelayMs(100)

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static nrfStatus_t status;
static nrfData_t writeData;
static nrfData_t readData;
static bool writeBusy, readFlag, writeFlag;
static uint16_t waitCnt, resendCnt; 

// Finite state machine 
struct FSM {
    uint16_t     State; // Current state of the Finite State Machine
    uint16_t     NextState; // Next state of the Finite State Machine
}WFsm;



/*******************************************************************************
 *          LOCAL FUNCTIONS
*******************************************************************************/
static void    NRF_WriteRegister(uint8_t reg, uint8_t value); // Write to a register
static uint8_t NRF_ReadRegister(uint8_t reg); // Read from a register
static void    NRF_WriteData(void); // Write data
static void    NRF_ReadData(void); // Read data
static void    NRF_Configure(void); // Configure the registers of the NRF module
static void    NRF_WaitForLink(void); // Wait for a link between the modules

void NRF_WriteRegister(uint8_t reg, uint8_t value) {
    NRF_SCSN = 0; // Pull SCSN low
    
    tdSSCK;
    SPI_Write(NRF_WRITE_REG | reg); // Write address
    tSRD;
    SPI_Write(value); // Write value
    tSREADY;

    NRF_SCSN = 1; // Pull SCSN high
    tEND;
}

uint8_t NRF_ReadRegister(uint8_t reg) {
    uint8_t read;
    
    NRF_SCSN = 0; // Pull SCSN low
    
    tdSSCK;
    SPI_Write(NRF_READ_REG | reg); // Write address
    tSRD;
    read = SPI_Write(0x00); // Read value
    tSREADY;
    
    NRF_SCSN = 1; // Pull SCSN high
    tEND;
    return read;
}

void NRF_WriteData() {
    NRF_WriteRegister(NRF_TXBUF0_Adr, writeData.command);
    NRF_WriteRegister(NRF_TXBUF1_Adr, writeData.data);
    NRF_WriteRegister(NRF_TXBUF2_Adr, writeData.command + writeData.data);
    
    // Send data
    NRF_WriteRegister(NRF_TXCOUNT_Adr,3);
}

void NRF_ReadData() {
    readData.command = NRF_ReadRegister(NRF_RXBUF0_Adr);
    readData.data = NRF_ReadRegister(NRF_RXBUF1_Adr);
    readData.crc = NRF_ReadRegister(NRF_RXBUF2_Adr);
}

void NRF_WaitForLink() {
    NRF_STALED_0 = 0;
    ////C_DEBUG_WriteMessage("Connecting...");
    while(!NRF_ReadRegister(NRF_LNKSTA_Adr)) {
        DelayMs(10);
    }
    ////C_DEBUG_WriteMessage("Link established!");
    NRF_STALED_0 = 1;
}

void NRF_Configure() {
    // Initial port values
    NRF_Reset; // Reset
    ////C_DEBUG_WriteMessage("NRF reset");
    NRF_STALED_0 = 0;
    NRF_SCSN = 1; // Active low SCSN pin
    tSTARTUP;
    
    
    // TEST
    CONFIGbits.CRCO = 1;
    
    CONFIG = 0x55;
    
    NRF_WriteRegister(CONFIGbits.address, CONFIGbits.value);
    // TEST
    
    // Write registers
    NRF_WriteRegister(NRF_TXSTA_Adr, 0x50); // 32kHz
    NRF_WriteRegister(NRF_TXLAT_Adr, 0x06); // Latency settings
    NRF_WriteRegister(NRF_I2SCNF_IN_Adr, 0x80); // I2S Master mode
    NRF_WriteRegister(NRF_TXPWR_Adr,0x03); // 0dBm output power
    NRF_WriteRegister(NRF_CH0_Adr, 31); // Randomly generated
    NRF_WriteRegister(NRF_CH1_Adr, 59); // Randomly generated
    NRF_WriteRegister(NRF_CH2_Adr, 17); // Randomly generated
    NRF_WriteRegister(NRF_CH3_Adr, 3); // Randomly generated
    NRF_WriteRegister(NRF_CH4_Adr, 45); // Randomly generated
    NRF_WriteRegister(NRF_CH5_Adr, 73); // Randomly generated
    NRF_WriteRegister(NRF_NBCH_Adr, 0x02); // Maximum number of channels that are not used at any time
    NRF_WriteRegister(NRF_NACH_Adr, 0x06); // Maximum number of channels that are used during streaming at any time
    NRF_WriteRegister(NRF_NLCH_Adr, 0x06); // Maximum number of channels that are used during link up phase
    
    NRF_WriteRegister(NRF_ADDR0_Adr, 0x15); // Radio address byte #1
    NRF_WriteRegister(NRF_ADDR1_Adr, 0x36); // Radio address byte #2
    NRF_WriteRegister(NRF_ADDR2_Adr, 0x57); // Radio address byte #3
    NRF_WriteRegister(NRF_ADDR3_Adr, 0x78); // Radio address byte #4
    NRF_WriteRegister(NRF_ADDR4_Adr, 0x99); // Radio address byte #5
    
    NRF_WriteRegister(NRF_INTCF_Adr, 0x58); // Enable all interrupts
    NRF_WriteRegister(NRF_I2SCNF_OUT_Adr, 0x00); // I2S mode
    NRF_WriteRegister(NRF_RXPWR_Adr, 0x03); // 0dBm output power
    NRF_WriteRegister(NRF_RXMOD_Adr, 0x20); // Enable RF
    NRF_WriteRegister(NRF_TXMOD_Adr, 0x83); // Enable RF and MCLK
}

/*******************************************************************************
 *          CONTROLLER FUNCTIONS
*******************************************************************************/
void nrfInit() {
    // Initialise ports
    NRF_RES_Dir = 0;
    NRF_SCSN_Dir = 0; 
    NRF_IRQ_Dir  = 1; /** set as input when used!!! Map to interrupt port**/ 
    
    // Initialise SPI module
    spi2Init();
    
    // Initialise status
    status.STA = NRF_STATUS_Initialize;
    WFsm.NextState = WFSM_SSetup;
    WFsm.State = WFSM_SSetup;
    writeBusy = false;
    readFlag = false;
    writeFlag = false;
    
    // Enable the interrupts
    // TODO: create interrupt controller
    
    // Configure NRF module
    NRF_Configure();
    NRF_WaitForLink();
    
}

void nrfCheckInterrupts() {
    uint8_t interruptSource = 0;
    interruptSource = NRF_ReadRegister(NRF_INTSTA_Adr);
    NRF_WriteRegister(NRF_INTSTA_Adr, NRF_STATUS_IntClear); // Clear all flags
    
    switch(interruptSource) {
        case NRF_STATUS_IntLB:
            //C_DEBUG_WriteMessage("Link broken");
            NRF_WaitForLink();
            break;

        case NRF_STATUS_IntTD:
            writeFlag = true;
            break;

        case NRF_STATUS_IntDR:
            NRF_ReadData();
            if(writeBusy){
                readFlag = true;
            } else {
                if(readData.crc == (readData.command + readData.data)) {
                    writeData.command = CRC_OK;
                    //C_DEBUG_WriteRegister(REG_CUSTOM, NRF_DataRead.data);
                } else {
                    writeData.command = CRC_NOK;
                }
                NRF_WriteData();
            }
            
            break;

        default:
            break;
    }

    
//    while(NRF_IRQ == 0) {
//        
//    }
}

void nrfWriteData(nrfData_t data) {
    
    if(writeBusy) {
        return;
    } else {
        writeBusy = true;
    }
    
    while(writeBusy) {
        // Transitions
        
        //C_DEBUG_WriteRegister(NRF_TXBUF0_Adr, WFsm.State);
        
        switch(WFsm.State) {
            case WFSM_SSetup:
                WFsm.NextState = WFSM_SWrite;
                break;
                
            case WFSM_SWrite:
                WFsm.NextState = WFSM_SWait;
                break;
                
            case WFSM_SWait:
                if(readFlag) {
                    readFlag = false;
                    WFsm.NextState = WFSM_SCheck;
                }
                if(waitCnt > WAIT_MAX) {
                    //C_DEBUG_WriteMessage("Wait exceeded");
                    writeBusy = false;
                    readFlag = false;
                    WFsm.State = WFSM_SSetup;
                    return;
                }
                break;
                
            case WFSM_SCheck:
                if(readData.command == CRC_OK) {
                    WFsm.State = WFSM_SSetup;
                    writeBusy = false;
                    readFlag = false;
                } else {
                    if(resendCnt > RESEND_MAX) {
                        //C_DEBUG_WriteMessage("Resend exceeded");
                        writeBusy = false;
                        readFlag = false;
                        WFsm.NextState = WFSM_SSetup;
                    } else {
                        WFsm.NextState = WFSM_SWrite;
                    }
                }
                break;
                
            default:
                break;
        }
        
        // Do work
        switch(WFsm.State) {
            case WFSM_SSetup:
                waitCnt = 0;
                resendCnt = 0;
                writeData.command = data.command;
                writeData.data = data.data; 
                break;
                
            case WFSM_SWrite:
                NRF_WriteData();
                resendCnt++;
                break;
                
            case WFSM_SWait:
                waitCnt++;
                break;
                
            case WFSM_SCheck:
                
                break;
                
            default:
                break;
        }        
        // Clock
        WFsm.State = WFsm.NextState;
    }
}

void nrfReadData(nrfData_t data) {
    // Get the data
//    data.command = NRF_ReadRegister(NRF_RXBUF0_Adr);
//    data.data = NRF_ReadRegister(NRF_RXBUF1_Adr);
//    data.crc = NRF_ReadRegister(NRF_RXBUF2_Adr);
}
