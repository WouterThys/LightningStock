#ifndef SPI1_DRIVER_H
#define	SPI1_DRIVER_H

#include "../Settings.h"    

    
extern uint16_t SPI1_ReadData;
    
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/    
    
/**
 *  Initializes the SPI2 module on with SPI ports defined in settings.h. SPI2
 *  will be a 16-bit module. SPI2 mode is Mode 0,0.
 */
void spi1Init();

/**
 * 
 * @param enable
 */
void spi1Enable(bool enable);

/**
 * 
 * @param data
 */
void spi1Write(uint16_t data);



#endif