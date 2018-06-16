#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "../Drivers/SYSTEM_Driver.h"
#include "../Drivers/SPI1_Driver.h"
#include "../Settings.h"
#include "MCP_Module.h"
#include "MCP_Controller.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define MCP_R_REGISTER      0x01
#define MCP_W_REGISTER      0x00

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/
#define McpWrite(d)     spi1Write(d); 

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/


/*******************************************************************************
 *          LOCAL FUNCTIONS
*******************************************************************************/
static void    MCP_WriteRegister(uint8_t reg, uint8_t value); // Write to a register
static uint8_t MCP_ReadRegister(uint8_t reg); // Read from a register
static void    MCP_Configure();



void MCP_WriteRegister(uint8_t reg, uint8_t value) {
    MCP_CS = 0; // Pull CS low
    
    McpWrite(MCP_ADDR + MCP_W_REGISTER); // Control byte 
    McpWrite(reg);                       // Address byte
    McpWrite(value);                     // Write data byte

    MCP_CS = 1; // Pull CS high
}

uint8_t MCP_ReadRegister(uint8_t reg) {
    uint8_t read = 0;
    
    MCP_CS = 0; // Pull CS low
    
    McpWrite(MCP_ADDR + MCP_R_REGISTER); // Control byte
    McpWrite(reg);                       // Address byte
    read = McpWrite(0x00);               // Read data byte

    MCP_CS = 1; // Pull CS high
    
    return read;
}


void MCP_Configure() {
    
    // Set all output
    mcpIODIRA = 0x00;
    mcpIODIRB = 0x00;
    MCP_WriteRegister(mcpIODIRAbits.address, mcpIODIRA);
    MCP_WriteRegister(mcpIODIRBbits.address, mcpIODIRB);
    
}

/*******************************************************************************
 *          CONTROLLER FUNCTIONS
*******************************************************************************/
void mcpInit() {

    
    // Initialize ports
    MCP_CS = 1;
    MCP_CS_Dir = 0;
    
    // Initialize SPI module
    spi1Init();
    spi1Enable(true);
    
    // Configure MCP module
    MCP_Configure();   
}

void mcpSetPORTA(uint8_t value) {
    mcpGPIOA = value;
    MCP_WriteRegister(mcpGPIOAbits.address, value);
}

void mcpSetPORTB(uint8_t value) {
    mcpGPIOB = value;
    MCP_WriteRegister(mcpGPIOBbits.address, value);
}

uint8_t mcpGetPORTA() {
    mcpOLATA = MCP_ReadRegister(mcpOLATAbits.address);
    return mcpOLATA;
}

uint8_t mcpGetPORTB() {
    mcpOLATB = MCP_ReadRegister(mcpOLATBbits.address);
    return mcpOLATB;
}
