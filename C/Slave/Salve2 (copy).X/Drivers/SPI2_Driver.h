#ifndef SPI2_DRIVER_H
#define	SPI2_DRIVER_H

#include "../Settings.h"

#ifndef SPI2_SCK_Pin
    #define SPI2_SCK_Pin    PORTBbits.RB10      /* SCK Pin on RP42-RB10           */
    #define SPI2_SCK_Dir    TRISBbits.TRISB10
    #define SPI2_SCK_Map    0b001001 
#endif

#ifndef SPI2_SDO_Pin
    #define SPI2_SDO_Pin    PORTBbits.RB11      /* SDO Pin on RP43-RB11           */
    #define SPI2_SDO_Dir    TRISBbits.TRISB11  
    #define SPI2_SDO_Map    0b001000 
#endif
  
#ifndef SPI2_SDI_Pin
    #define SPI2_SDI_Pin    PORTBbits.RB12      /* SDI Pin on RPI44-RB12          */
    #define SPI2_SDI_Dir    TRISBbits.TRISB12  
    #define SPI2_SDI_Map    44
#endif

#ifndef SPI2_IP
    #define SPI2_IP         1
#endif


extern uint16_t SPI2_ReadData;
    
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/    
    
/**
 *  Initializes the SPI2 module on with SPI ports defined in settings.h. SPI2
 *  will be a 16-bit module. SPI2 mode is Mode 0,0.
 */
void spi2Init();

/**
 * 
 * @param enable
 */
void spi2Enable(bool enable);

/**
 * 
 * @param data
 */
uint8_t spi2Write(uint8_t data);



#endif