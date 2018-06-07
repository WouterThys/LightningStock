#ifndef SPI2_DRIVER_H
#define	SPI2_DRIVER_H

#ifdef	__cplusplus
extern "C" {
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