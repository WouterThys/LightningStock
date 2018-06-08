#ifndef NRF_MODULE_H
#define	NRF_MODULE_H

#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

    
/******************************************************************************/
/* Register values                                                            */
/******************************************************************************/


/************************* CONFIG *********************************************/   
struct CONFIG_s {
    union {
        struct {
            unsigned PRIM_RX:1;     // RX/TX control
            unsigned PWR_UP:1;      // 0=Power up, 1=Power down
            unsigned CRCO:1;        // CRC encoding scheme
            unsigned EN_CRC:1;      // Enable CRC
            unsigned MASK_MAX_RT:1; // Mask interrupt caused by MAX_RT
            unsigned MASK_TX_DS:1;  // Mask interrupt caused by TX_DS
            unsigned MASK_RX_DR:1;  // Mask interrupt caused by RX_DR
            unsigned :1;            // Reserved
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct CONFIG_s CONFIGBITS;
volatile CONFIGBITS nrfCONFIGbits = {{.value = 0x08}, .address = 0x00};
#define nrfCONFIG   nrfCONFIGbits.value



/************************* EN_AA **********************************************/   
struct EN_AA_s {
    union {
        struct {
            unsigned ENAA_P0:1;     // Enable auto acknowledgement data pipe 0
            unsigned ENAA_P1:1;     // Enable auto acknowledgement data pipe 1
            unsigned ENAA_P2:1;     // Enable auto acknowledgement data pipe 2
            unsigned ENAA_P3:1;     // Enable auto acknowledgement data pipe 3
            unsigned ENAA_P4:1;     // Enable auto acknowledgement data pipe 4
            unsigned ENAA_P5:1;     // Enable auto acknowledgement data pipe 5
            unsigned :2;            // Reserved
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct EN_AA_s EN_AABITS;
volatile EN_AABITS nrfEN_AAbits = {{.value = 0x3F}, .address = 0x01};
#define nrfEN_AA   nrfEN_AAbits.value



/************************* EN_RXADDR ******************************************/   
struct EN_RXADDR_s {
    union {
        struct {
            unsigned ERX_P0:1;     // Enable data pipe 0
            unsigned ERX_P1:1;     // Enable data pipe 1
            unsigned ERX_P2:1;     // Enable data pipe 2
            unsigned ERX_P3:1;     // Enable data pipe 3
            unsigned ERX_P4:1;     // Enable data pipe 4
            unsigned ERX_P5:1;     // Enable data pipe 5
            unsigned :2;            // Reserved
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct EN_RXADDR_s EN_RXADDRBITS;
volatile EN_RXADDRBITS nrfEN_RXADDRbits = {{.value = 0x03}, .address = 0x02};
#define nrfEN_RXADDR   nrfEN_RXADDRbits.value



/************************* SETUP_AW *******************************************/   
struct SETUP_AW_s {
    union {
        struct {
            unsigned AW:2;          // RX/TX Address field width
            unsigned :6;            // Reserved
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct SETUP_AW_s SETUP_AWBITS;
volatile SETUP_AWBITS nrfSETUP_AWbits = {{.value = 0x03}, .address = 0x03};
#define nrfSETUP_AW   nrfSETUP_AWbits.value



/************************* SETUP_RETR *****************************************/   
struct SETUP_RETR_s {
    union {
        struct {
            unsigned ARC:4;             // Auto Retransmit Count
            unsigned ADR:4;             // Auto Retransmit Delay
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct SETUP_RETR_s SETUP_RETRBITS;
volatile SETUP_RETRBITS nrfSETUP_RETRbits = {{.value = 0x03}, .address = 0x04};
#define nrfSETUP_RETR   nrfSETUP_RETRbits.value



/************************* RF_CH **********************************************/   
struct RF_CH_s {
    union {
        struct {
            unsigned RF_CH:7;            // Frequency channel the nRF operates on
            unsigned :1;                 // Reserved
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct RF_CH_s RF_CHBITS;
volatile RF_CHBITS nrfRF_CHbits = {{.value = 0x02}, .address = 0x05};
#define nrfRF_CH   nrfRF_CHbits.value



/************************* RF_SETUP *******************************************/   
struct RF_SETUP_s {
    union {
        struct {
            unsigned :1;                 // Reserved
            unsigned RF_PXR:2;           // Set RF output power in TX mode
            unsigned RF_DR_HIGH:1;       // Select between high speed data rates
            unsigned PLL_LOCK:1;         // Force PLL lock signal. Only used in test
            unsigned RF_DR_LOW:1;        // Set RF Data Rate to 250kbps.
            unsigned :1;                 // Reserved
            unsigned CONT_WAVE:1;        // Enables continuous carrier transmit when high
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct RF_SETUP_s RF_SETUPBITS;
volatile RF_SETUPBITS nrfRF_SETUPbits = {{.value = 0x07}, .address = 0x06};
#define nrfRF_SETUP   nrfRF_SETUPbits.value



/************************* STATUS *******************************************/   
struct STATUS_s {
    union {
        struct {
            unsigned TX_FULL:1;          // TX FIFO full flag
            unsigned RX_P_NO:3;          // Data pipe number for the payload available for reading from RX_FIFO
            unsigned MAX_RT:1;           // Maximum number of TX transmits interrupt.
            unsigned TX_DS:1;            // Data Sent TX FIFO interrupt.
            unsigned RX_DR:1;            // Data Ready RX FIFO interrupt.
            unsigned :1;                 // Reserved
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct STATUS_s STATUSBITS;
volatile STATUSBITS nrfSTATUSbits = {{.value = 0x00}, .address = 0x07};
#define nrfSTATUS   nrfSTATUSbits.value



/************************* OBSERVE_TX *****************************************/   
struct OBSERVE_TX_s {
    union {
        struct {
            unsigned ARC_CNT:4;          // Count retransmitted packages.
            unsigned PLOS_CNT:4;         // Count lost packets.
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct OBSERVE_TX_s OBSERVE_TXBITS;
volatile OBSERVE_TXBITS nrfOBSERVE_TXbits = {{.value = 0x00}, .address = 0x08};
#define nrfOBSERVE_TX   nrfOBSERVE_TXbits.value



/************************* RPD *****************************************/   
struct RPD_s {
    union {
        struct {
            unsigned RPD:1;              // Received Power Detector.
            unsigned PLOS_CNT:7;         // Reserved.
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct RPD_s RPDBITS;
volatile RPDBITS nrfRPDbits = {{.value = 0x00}, .address = 0x09};
#define nrfRPD   nrfRPDbits.value



/************************* RX_ADDR_P0 *****************************************/   
struct RX_ADDR_P0_s {
    union {
//        struct {
//            unsigned RX_ADDR_P0:40;      // Receive address data pipe0.
//        };
        double value;
    };
    uint8_t address;
};
typedef struct RX_ADDR_P0_s RX_ADDR_P0BITS;
volatile RX_ADDR_P0BITS nrfRX_ADDR_P0bits = {{.value = 0xE7E7E7E7E7}, .address = 0x0A};
#define nrfRX_ADDR_P0   nrfRX_ADDR_P0bits.value



/************************* RX_ADDR_P1 *****************************************/   
struct RX_ADDR_P1_s {
    union {
//        struct {
//            unsigned RX_ADDR_P1:40;      // Receive address data pipe 1.
//        };
        double value;
    };
    uint8_t address;
};
typedef struct RX_ADDR_P1_s RX_ADDR_P1BITS;
volatile RX_ADDR_P1BITS nrfRX_ADDR_P1bits = {{.value = 0xC2C2C2C2C2}, .address = 0x0B};
#define nrfRX_ADDR_P1   nrfRX_ADDR_P1bits.value



/************************* RX_ADDR_P2 *****************************************/   
struct RX_ADDR_P2_s {
    union {
        struct {
            unsigned RX_ADDR_P2:8;        // Receive address data pipe 2.
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct RX_ADDR_P2_s RX_ADDR_P2BITS;
volatile RX_ADDR_P2BITS nrfRX_ADDR_P2bits = {{.value = 0xC3}, .address = 0x0C};
#define nrfRX_ADDR_P2   nrfRX_ADDR_P2bits.value



/************************* RX_ADDR_P3 *****************************************/   
struct RX_ADDR_P3_s {
    union {
        struct {
            unsigned RX_ADDR_P3:8;        // Receive address data pipe 3.
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct RX_ADDR_P3_s RX_ADDR_P3BITS;
volatile RX_ADDR_P3BITS nrfRX_ADDR_P3bits = {{.value = 0xC4}, .address = 0x0D};
#define nrfRX_ADDR_P3   nrfRX_ADDR_P3bits.value



/************************* RX_ADDR_P4 *****************************************/   
struct RX_ADDR_P4_s {
    union {
        struct {
            unsigned RX_ADDR_P4:8;        // Receive address data pipe 4.
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct RX_ADDR_P4_s RX_ADDR_P4BITS;
volatile RX_ADDR_P4BITS nrfRX_ADDR_P4bits = {{.value = 0xC5}, .address = 0x0E};
#define nrfRX_ADDR_P4   nrfRX_ADDR_P4bits.value



/************************* RX_ADDR_P5 *****************************************/   
struct RX_ADDR_P5_s {
    union {
        struct {
            unsigned RX_ADDR_P5:8;        // Receive address data pipe 5.
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct RX_ADDR_P5_s RX_ADDR_P5BITS;
volatile RX_ADDR_P5BITS nrfRX_ADDR_P5bits = {{.value = 0xC6}, .address = 0x0F};
#define nrfRX_ADDR_P5   nrfRX_ADDR_P5bits.value



/************************* TX_ADDR ********************************************/   
struct TX_ADDR_s {
    union {
//        struct {
//            unsigned TX_ADDR:40;      // Transmit Address
//        };
        double value;
    };
    uint8_t address;
};
typedef struct TX_ADDR_s TX_ADDRBITS;
volatile TX_ADDRBITS nrfTX_ADDRbits = {{.value = 0xE7E7E7E7E7}, .address = 0x10};
#define nrfTX_ADDR   nrfTX_ADDRbits.value



/************************* RX_PW_P0 *******************************************/   
struct RX_PW_P0_s {
    union {
        struct {
            unsigned RX_PW_P0:6;      // Number of bytes in RX payload in data pipe 0
            unsigned :2;              // Reserved
        };
        long value;
    };
    uint8_t address;
};
typedef struct RX_PW_P0_s RX_PW_P0BITS;
volatile RX_PW_P0BITS nrfRX_PW_P0bits = {{.value = 0x00}, .address = 0x11};
#define nrfRX_PW_P0   nrfRX_PW_P0bits.value



/************************* RX_PW_P1 *******************************************/   
struct RX_PW_P1_s {
    union {
        struct {
            unsigned RX_PW_P1:6;      // Number of bytes in RX payload in data pipe 0
            unsigned :2;              // Reserved
        };
        long value;
    };
    uint8_t address;
};
typedef struct RX_PW_P1_s RX_PW_P1BITS;
volatile RX_PW_P1BITS nrfRX_PW_P1bits = {{.value = 0x00}, .address = 0x12};
#define nrfRX_PW_P1   nrfRX_PW_P1bits.value



/************************* RX_PW_P2 *******************************************/   
struct RX_PW_P2_s {
    union {
        struct {
            unsigned RX_PW_P2:6;      // Number of bytes in RX payload in data pipe 0
            unsigned :2;              // Reserved
        };
        long value;
    };
    uint8_t address;
};
typedef struct RX_PW_P2_s RX_PW_P2BITS;
volatile RX_PW_P2BITS nrfRX_PW_P2bits = {{.value = 0x00}, .address = 0x13};
#define nrfRX_PW_P2   nrfRX_PW_P2bits.value



/************************* RX_PW_P3 *******************************************/   
struct RX_PW_P3_s {
    union {
        struct {
            unsigned RX_PW_P3:6;      // Number of bytes in RX payload in data pipe 0
            unsigned :2;              // Reserved
        };
        long value;
    };
    uint8_t address;
};
typedef struct RX_PW_P3_s RX_PW_P3BITS;
volatile RX_PW_P3BITS nrfRX_PW_P3bits = {{.value = 0x00}, .address = 0x14};
#define nrfRX_PW_P3   nrfRX_PW_P3bits.value



/************************* RX_PW_P4 *******************************************/   
struct RX_PW_P4_s {
    union {
        struct {
            unsigned RX_PW_P4:6;      // Number of bytes in RX payload in data pipe 0
            unsigned :2;              // Reserved
        };
        long value;
    };
    uint8_t address;
};
typedef struct RX_PW_P4_s RX_PW_P4BITS;
volatile RX_PW_P4BITS nrfRX_PW_P4bits = {{.value = 0x00}, .address = 0x15};
#define nrfRX_PW_P4   nrfRX_PW_P4bits.value



/************************* RX_PW_P5 *******************************************/   
struct RX_PW_P5_s {
    union {
        struct {
            unsigned RX_PW_P5:6;      // Number of bytes in RX payload in data pipe 0
            unsigned :2;              // Reserved
        };
        long value;
    };
    uint8_t address;
};
typedef struct RX_PW_P5_s RX_PW_P5BITS;
volatile RX_PW_P5BITS nrfRX_PW_P5bits = {{.value = 0x00}, .address = 0x16};
#define nrfRX_PW_P5   nrfRX_PW_P5bits.value



/************************* FIFO_STATUS ****************************************/   
struct FIFO_STATUS_s {
    union {
        struct {
            unsigned RX_EMPTY:1;         // RX FIFO empty flag
            unsigned RX_FULL:1;          // RX FIFO full flag
            unsigned :2;                 // Reserved
            unsigned TX_EMPTY:1;         // TX FIFO empty flag
            unsigned TX_FULL:1;          // TX FIFO full flag
            unsigned DPL_P5:1;           // Enable dynamic payload
            unsigned :2;                 // Reserved
        };
        long value;
    };
    uint8_t address;
};
typedef struct FIFO_STATUS_s FIFO_STATUSBITS;
volatile FIFO_STATUSBITS nrfFIFO_STATUSbits = {{.value = 0x11}, .address = 0x17};
#define nrfFIFO_STATUS   nrfFIFO_STATUSbits.value



/************************* DYNPD *******************************************/   
struct DYNPD_s {
    union {
        struct {
            unsigned DPL_P0:1;           // Enable dynamic payload length data pipe 0
            unsigned DPL_P1:1;           // Enable dynamic payload length data pipe 1
            unsigned DPL_P2:1;           // Enable dynamic payload length data pipe 2
            unsigned DPL_P3:1;           // Enable dynamic payload length data pipe 3
            unsigned DPL_P4:1;           // Enable dynamic payload length data pipe 4
            unsigned DPL_P5:1;           // Enable dynamic payload length data pipe 5
            unsigned :2;                 // Reserved
        };
        long value;
    };
    uint8_t address;
};
typedef struct DYNPD_s DYNPDBITS;
volatile DYNPDBITS nrfDYNPDbits = {{.value = 0x00}, .address = 0x1C};
#define nrfDYNPD   nrfDYNPDbits.value



/************************* FEATURE *******************************************/   
struct FEATURE_s {
    union {
        struct {
            unsigned EN_DYN_ACK:1;       // 
            unsigned EN_ACK_PAY:1;       // Enable payload with ACK
            unsigned EN_DPL:1;           // Enable dynamic payload length
            unsigned :5;                 // Reserved
        };
        long value;
    };
    uint8_t address;
};
typedef struct FEATURE_s FEATUREBITS;
volatile FEATUREBITS nrfFEATUREbits = {{.value = 0x00}, .address = 0x16};
#define nrfFEATURE   nrfFEATUREbits.value













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