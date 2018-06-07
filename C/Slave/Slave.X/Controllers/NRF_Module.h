/* 
 * File:   User input functions
 * Author: wouter
 */
#ifndef NRF_MODULE_H
#define	NRF_MODULE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
    
/******************************************************************************/
/* Registers: nRF2460                                                         */
/******************************************************************************/   
// ATX register addresses
#define NRF_TXSTA_Adr       0x01
#define NRF_INTSTA_Adr      0x02    
#define NRF_TXMOD_Adr       0x5A
#define NRF_TXLAT_Adr       0x52
#define NRF_INTCF_Adr       0x53
#define NRF_I2SCNF_IN_Adr   0x54   
#define NRF_TXPWR_Adr       0x56
#define NRF_TXRESO_Adr      0x50
    
// LINK status register addresses
#define NRF_LNKSTA_Adr      0x03

// LINK control register addresses
#define NRF_CH0_Adr         0x0C
#define NRF_CH1_Adr         0x0D
#define NRF_CH2_Adr         0x0E
#define NRF_CH3_Adr         0x0F
#define NRF_CH4_Adr         0x10
#define NRF_CH5_Adr         0x11
#define NRF_CH6_Adr         0x12
#define NRF_CH7_Adr         0x13
#define NRF_CH8_Adr         0x14
#define NRF_CH9_Adr         0x15
#define NRF_CH10_Adr        0x16
#define NRF_CH11_Adr        0x17
#define NRF_CH12_Adr        0x18
#define NRF_CH13_Adr        0x19
#define NRF_CH14_Adr        0x1A
#define NRF_CH15_Adr        0x1B
#define NRF_CH16_Adr        0x1C
#define NRF_CH17_Adr        0x1D
#define NRF_CH18_Adr        0x1E
#define NRF_CH19_Adr        0x1F
#define NRF_CH20_Adr        0x20
#define NRF_CH21_Adr        0x21
#define NRF_CH22_Adr        0x22
#define NRF_CH23_Adr        0x23
#define NRF_CH24_Adr        0x24
#define NRF_CH25_Adr        0x25
#define NRF_CH26_Adr        0x26
#define NRF_CH27_Adr        0x27
#define NRF_CH28_Adr        0x28
#define NRF_CH29_Adr        0x29
#define NRF_CH30_Adr        0x2A
#define NRF_CH31_Adr        0x2B
#define NRF_CH32_Adr        0x2C
#define NRF_CH33_Adr        0x2D
#define NRF_CH34_Adr        0x2E
#define NRF_CH35_Adr        0x2F
#define NRF_CH36_Adr        0x30
#define NRF_CH37_Adr        0x31
#define NRF_BCHD_Adr        0x32
#define NRF_NBCH_Adr        0x33
#define NRF_NACH_Adr        0x34
#define NRF_NLCH_Adr        0x35
#define NRF_LNKMOD_Adr      0x36
#define NRF_MDUR_Adr        0x0B
#define NRF_ADDR0_Adr       0x39
#define NRF_ADDR1_Adr       0x3A
#define NRF_ADDR2_Adr       0x3B
#define NRF_ADDR3_Adr       0x3C
#define NRF_ADDR4_Adr       0x3D    
#define NRF_LNKCSTATE_Adr   0x3E

// DATA channel    
#define NRF_DTXSTA_Adr      0x4E    
#define NRF_RXCOUNT_Adr     0x5B
#define NRF_TXCOUNT_Adr     0x5C
#define NRF_RXBUF0_Adr      0x5D
#define NRF_RXBUF1_Adr      0x5E
#define NRF_RXBUF2_Adr      0x5F    
#define NRF_TXBUF0_Adr      0x65   
#define NRF_TXBUF1_Adr      0x66    
#define NRF_TXBUF2_Adr      0x67        

// ARX control register addresses
#define NRF_RXMOD_Adr       0x4A
#define NRF_I2SCNF_OUT_Adr  0x44
#define NRF_RXPWR_Adr       0x49
#define NRF_SYNCDL_Adr      0x37

// Test register addresses
#define NRF_TESTREG_Adr     0x7E
#define NRF_TESTCH_Adr      0x7F
#define NRF_REVBYT_Adr      0x7D    
    
/******************************************************************************/
/* Useful defines                                                            */
/******************************************************************************/      
#define NRF_0DBM        0x03
#define NRF_5DBM        0x02
#define NRF_10DBM       0x01
#define NRF_20DBM       0x00    
    
    
/******************************************************************************/
/* Register values                                                            */
/******************************************************************************/     
union TXSTAU {
    struct {
        unsigned :3;
        unsigned RATE:2; // ATX audio input rate register: 10 = 32 kHz
        unsigned :3;
    }TXSTAb;
    uint8_t TXSTAr;
};

union TXMODU {
    struct {
        unsigned MCLKOC:2; // MCLK output control: 00 = MCLK off, 11 = output 256x32 kSPS.
        unsigned :4;
        unsigned ATPWD:1; // Audio transmitter power down.
        unsigned RFTEN:1; // RF transceiver enable.
    }TXMODb;
    uint8_t TXMODr;
};

union I2SCNF_INU {
    struct {
        unsigned :1; 
        unsigned WSD:1; // WS to MSB delay: 0 = 0 clock cycles, 1 = 1 clock cycle.
        unsigned :1; 
        unsigned WSP:1; // WS polarity: 0 = left sample, 1 = right sample.
        unsigned MSL:1; // Mono sample location: 0 = left channel samples, 1 = right channel samples.
        unsigned :2; 
        unsigned I2SCM:1; // I2S audio clock mode: 0 = Slave mode, 1 = master mode.
    }I2SCNF_INb;
    uint8_t I2SCNF_INr;
};

union RXMODS {
    struct {
        unsigned :5;
        unsigned RFTEN:1; // RF transceiver enable.
        unsigned :1;
        unsigned ATPWD:1; // Audio transmitter power down.
    }RXMODb;
    uint8_t RXMODr;
};

union I2SCNF_OUTU {
    struct {
        unsigned :1; 
        unsigned WSD:1; // WS to MSB delay: 0 = 0 clock cycles, 1 = 1 clock cycle.
        unsigned DBCR:1; // Data to Bit Clock relation, data valid at: 0 = rising edge, 1 = falling edge.
        unsigned WSP:1; // WS polarity: 0 = left sample, 1 = right sample.
        unsigned :2; 
        unsigned MUTE:1; // Mute sound output
        unsigned :1; 
    }I2SCNF_OUTb;
    uint8_t I2SCNF_OUTr;
};

union LNKSTAU {
    struct {
        unsigned LE:1; // Link established.
        unsigned :7;
    }LNKSTAb;
    uint8_t LNKSTAr;
};

union LNKMODU {
    struct {
        unsigned MUTEN:1; // Enables use of Mute duration feature.
        unsigned :1;
        unsigned DAFH:1; // Disable adaptive frequency hopping.
        unsigned :1;
        unsigned FRC:1; // Force reconfiguration with new configuration data.
        unsigned :1; // Reserved, MBZ.
        unsigned RES:1; // ATX and ARX reset to initial EEPROM register contents if no counterpart is found on the next link initialization.
        unsigned :1; // Reserved, MBZ.
    }LNKMODb;
    uint8_t LNKMODr;
};

union INTSTAU {
    struct {
        unsigned :1;
        unsigned :1; 
        unsigned :1; 
        unsigned DRF:1; // Data received, RXCOUNT bytes available in RXBUF[0:2].
        unsigned RMTDF:1; // Remote transfer done status flag.
        unsigned :1; 
        unsigned LBF:1; // Link broken status flag.
        unsigned :1;
    }INTSTAb;
    uint8_t INTSTAr;
};

union INTCFU {
    struct {
        unsigned :1;
        unsigned :1; 
        unsigned :1; 
        unsigned DREN:1; // Enable data received interrupt.
        unsigned RTDEN:1; // Enable remote transfer done interrupt.
        unsigned :1; 
        unsigned LBEN:1; // Enable link broken interrupt.
        unsigned IP:1; // Interrupt polarity: 0 = active low, 1 = active high.
    }INTCFb;
    uint8_t INTCFr;
};

union TXRESOU {
    struct {
        unsigned ROP:1; // Reset output polarity: 0 = active low, 1 = active high.
        unsigned ROS:2; // Reset output select: 0 = no reset, 4 = reset on RESO pin.
        unsigned :5;
    }TXRESOb;
    uint8_t TXRESOr;
};

union TESTCHU {
    struct {
        unsigned CHN:7; // Channel number.
        unsigned MODE:1; // Initializes the mode described in TESTREG: 0 = RX, 1 = TX.
    }TESTCHb;
    uint8_t TESTCHr;
};

#endif