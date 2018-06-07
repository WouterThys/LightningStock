#ifndef NRF_CHANNELS_H
#define	NRF_CHANNELS_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <stdbool.h>
    
/******************************************************************************/
/* Channels                                                                   */
/******************************************************************************/      
const uint8_t hoppingSequence [] = {
    64,     94,     100,    76,     88,     46,     70,     1,      25,     67,
    16,     91,     109,    34,     7,      85,     10,     61,     13,     73,
    82,     58,     28,     55,     49,     22,     31,     37,     112,    19,
    43,     103,    40,     52,     79,     97,     106,    4,
};  

const unsigned char address [] = {
    'P','a','n','d','a',     
};
    
#ifdef	__cplusplus
}
#endif

#endif	