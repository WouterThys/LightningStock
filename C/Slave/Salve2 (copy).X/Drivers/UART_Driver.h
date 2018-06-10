#ifndef UART_DRIVER_H
#define	UART_DRIVER_H

#include "../Settings.h"
#include "SYSTEM_Driver.h"


#ifndef FCY
 #define SYS_FREQ        140000000L
 #define FCY             SYS_FREQ/2
#endif

#ifndef IP_U1RX
 #define IP_U1RX         1
#endif

#ifndef UART1_RX_Dir
 #define UART1_RX_Dir     TRISBbits.TRISB6
 #define UART1_RX_Map     0b0100110           /* RX on RP38                     */
#endif

#ifndef UART1_TX_Dir
 #define UART1_TX_Dir    TRISBbits.TRISB7
 #define UART1_TX_Map    RPOR2bits.RP39R     /* TX on RP39                    */
#endif

#ifndef UART1_BAUD
 #define UART1_BAUD      57600
#endif


/**
 * Select the message type, only one of these types should be selected!
 */
#define TYPE_TEXT
//#define TYPE_SIMPLE_SHORT
//#define TYPE_SIMPLE_LONG
//#define TYPE_VARIABLE


#define START_CHAR 0x24 /* $ */
#define STOP_CHAR  0x26 /* & */
#define SEP_CHAR   0x3A /* : */
#define ACK_CHAR   0x40 /* @ */
#define MES_CHAR   0x4D /* M */

#define STA_OK              0x00  /* Message converted successfull            */
#define STA_NOK             0x01  /* Message not converted                    */
#define STA_INVALID_START   0x02  /* Invalid start character                  */
#define STA_INVALID_STOP    0x04  /* Invalid stop character                   */
#define STA_INVALID_SEP     0x08  /* Invalid separator character              */
#define STA_INVALID_LENGTH  0x10  /* Total message length invalid             */
#define STA_INVALID_COMMAND 0x20  /* Command length invalid                   */
#define STA_INVALID_MESSAGE 0x40  /* Message length invalid                   */



/**
 * Simple text message: any message that ends with '\0' character can be 
 * received. The maximum length of the message is defined as MESSAGE_LENGTH
 */
#ifdef TYPE_TEXT
#define MESSAGE_LENGTH 100
#endif

/**
 * Short formatted message: "$C:M&" 
 *  - 1 command byte - C
 *  - 1 message byte - M
 *  - No acknowledge
 */
#ifdef TYPE_SIMPLE_SHORT
#define MESSAGE_LENGTH 1
#endif

/**
 * Long formatted message: "$C:M0M1:A&" 
 *  - 1 command bytes - C
 *  - 2 message bytes - M0 and M1
 *  - With acknowledge
 */
#ifdef TYPE_SIMPLE_LONG
#define MESSAGE_LENGTH 2
#define ACKNOWLEDGE    
#endif

/**
 * Variable formatted message: "$C:L:M0M1...ML:A&" 
 *  - 1 command byte - C
 *  - L message bytes - M0 -> ML
 *  - With acknowledge
 */
#ifdef TYPE_VARIABLE
#define MESSAGE_LENGTH 100
#define ACKNOWLEDGE    
#endif

typedef struct {
    uint8_t sender; // Sender id
    uint8_t ack;    // Acknowledge
    uint8_t messageLength; // Length of the message
    uint8_t status; // Status - has one of the defined statuses
    uint8_t command; // Command
    char message[MESSAGE_LENGTH]; // Message
} UartData_t;


/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/
/**
 * Initialize the UART module, select which module to use. The module is enabled
 * when it is initialized.
 * @param baud: Baud rate of the UART 
 */
void uartDriverInit(uint16_t baud, void (*onReadDone)(UartData_t data));

/**
 * Enable the UART module
 * @param enable Enable or disable UART.
 */
void uartDriverEnable(bool enable);

/**
 * 
 * @param data
 */
void uartDriverWriteByte(uint8_t data);

/**
 * Write data to the TX pin of UART module. 
 * @param command: Command
 * @param data: Data string to write
 */
void uartDriverWrite(char command, char* data);

/**
 * Write data to the TX pin of UART module. 
 * @param command: Command
 * @param data: Data integer to write
 */
void uartDriverWriteInt(char command, int data);


#endif