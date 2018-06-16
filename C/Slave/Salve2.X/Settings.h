// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef SETTINGS_H
#define	SETTINGS_H

#define DEBUG       1
    
    
/**
 * Interrupt priorities
 */

#define MI2C_IP     7
#define T3_IP       6    
#define DMA1_IP     5

#define IP_U1RX     3
#define SPI2_IP     2
#define SPI1_IP     4
#define IP_INT1     1


/**
 * LED LOCATIONS
 */
#define LED_ROWS    12
#define LED_COLS    5


    
/**
 *  PIC
 */
 
#define NRF_LINK_LED    LATAbits.LATA0
#define NRF_ERR_LED     LATAbits.LATA1
    

    
/**
 * NRF
 */

#define NRF_CSN         PORTBbits.RB14 
#define NRF_CE          PORTBbits.RB13
#define NRF_IRQ         PORTBbits.RB15

#define NRF_CSN_Dir     TRISBbits.TRISB14
#define NRF_CE_Dir      TRISBbits.TRISB13
#define NRF_IRQ_Dir     TRISBbits.TRISB15


/**
 * MCP23S17
 */
#define MCP_CS          PORTBbits.RB4
#define MCP_CS_Dir      TRISBbits.TRISB4
#define MCP_ADDR        0b01000000


    
/**
 * UART
 */    
#define UART1_BAUD      57600 /* 57600 */
#define UART1_ID        2   

#define UART1_RX_Dir    TRISBbits.TRISB6    /* RX Pin on RP38-RB6             */
#define UART1_TX_Dir    TRISBbits.TRISB5    /* TX Pin on RP37-RB5             */

#define UART1_RX_Map    0b0100110           /* RX on RP38                     */
#define UART1_TX_Map    RPOR1bits.RP37R     /* TX on RP37                     */
    
/**
 * SPI 1
 */    
    
#define SPI1_SCK_Pin    PORTBbits.RB7      /* SCK Pin on RP39-RB7             */
#define SPI1_SCK_Dir    TRISBbits.TRISB7
    
#define SPI1_SDO_Pin    PORTBbits.RB8      /* SDO Pin on RP40-RB8             */
#define SPI1_SDO_Dir    TRISBbits.TRISB8    
    
#define SPI1_SDI_Pin    PORTBbits.RB9      /* SDI Pin on RPI41-RB9            */
#define SPI1_SDI_Dir    TRISBbits.TRISB9        
    
//#define SPI1_SDO_Map    0b001000
//#define SPI1_SCK_Map    0b001001  
//#define SPI1_SDI_Map    44



/**
 * SPI 2
 */    
    
#define SPI2_SCK_Pin    PORTBbits.RB10      /* SCK Pin on RP42-RB10           */
#define SPI2_SCK_Dir    TRISBbits.TRISB10
    
#define SPI2_SDO_Pin    PORTBbits.RB11      /* SDO Pin on RP43-RB11           */
#define SPI2_SDO_Dir    TRISBbits.TRISB11    
    
#define SPI2_SDI_Pin    PORTBbits.RB12      /* SDI Pin on RPI44-RB12          */
#define SPI2_SDI_Dir    TRISBbits.TRISB12       
    
#define SPI2_SDO_Map    0b001000 
#define SPI2_SCK_Map    0b001001  
#define SPI2_SDI_Map    44

    

#endif	/* XC_HEADER_TEMPLATE_H */

