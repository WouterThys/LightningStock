// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef SETTINGS_H
#define	SETTINGS_H

#include <xc.h> // include processor files - each processor file is guarded.  

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
#define IP_CN       1

    
/**
 *  PIC
 */
 
#define LED1            PORTBbits.RB15
#define LED1_Dir        TRISBbits.TRISB15
#define LED2            PORTBbits.RB14
#define LED2_Dir        TRISBbits.TRISB14
    

    
    
/**
 * UART
 */    
#define UART1_BAUD      57600 /* 57600 */
#define UART1_ID        2   

#define UART1_RX_Dir    TRISBbits.TRISB6    /* RX Pin on RP38-RB6             */
#define UART1_TX_Dir    TRISBbits.TRISB7    /* TX Pin on RP39-RB7             */

#define UART1_RX_Map    0b0100110           /* RX on RP38                     */
#define UART1_TX_Map    RPOR2bits.RP39R     /* TX on RP39                     */
    
/**
 * SPI 1
 */    
    
#define SPI1_SCK_Pin    PORTBbits.RB11      /* SCK Pin on RP43-RB11           */
#define SPI1_SCK_Dir    TRISBbits.TRISB11
    
#define SPI1_SDO_Pin    PORTBbits.RB10      /* SDO Pin on RP42-RB10           */
#define SPI1_SDO_Dir    TRISBbits.TRISB10    
    
#define SPI1_SDI_Pin    PORTBbits.RB12      /* SDI Pin on RPI44-RB12          */
#define SPI1_SDI_Dir    TRISBbits.TRISB12      
    
#define SPI1_CS_Pin     PORTAbits.RA4       /* CS DAC Pin on I/O-RA4          */
#define SPI1_CS_Dir     TRISAbits.TRISA4    
    
#define SPI1_SDO_Map    0b001000
#define SPI1_SCK_Map    0b001001  
#define SPI1_SDI_Map    44



/**
 * SPI 2
 */    
    
#define SPI2_SCK_Pin    PORTBbits.RB11      /* SCK Pin on RP43-RB11           */
#define SPI2_SCK_Dir    TRISBbits.TRISB11
    
#define SPI2_SDO_Pin    PORTBbits.RB10      /* SDO Pin on RP42-RB10           */
#define SPI2_SDO_Dir    TRISBbits.TRISB10    
    
#define SPI2_SDI_Pin    PORTBbits.RB12      /* SDI Pin on RPI44-RB12          */
#define SPI2_SDI_Dir    TRISBbits.TRISB12      
    
#define SPI2_CS_Pin     PORTAbits.RA4       /* CS DAC Pin on I/O-RA4          */
#define SPI2_CS_Dir     TRISAbits.TRISA4    
    
#define SPI2_SDO_Map    0b001000
#define SPI2_SCK_Map    0b001001  
#define SPI2_SDI_Map    44

    

#endif	/* XC_HEADER_TEMPLATE_H */

