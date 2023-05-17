// crc.h

#ifndef __CRC_H__
#define __CRC_H__

// Includes
#include "LOUD_types.h"

// Compile Switches
#define CRC32_USE_TABLE 1   // 0 -> compute CRCs on the fly, 1 -> use precomputed table
                            // NOTE:  If CRC32_USE_TABLE is set, you must call CRC32_init() before calling any other function!

// Defines
#define CRC32_SEED         0xFFFFFFFF   // initial value
#define CRC32_POLYNOMIAL   0xEDB88320   // reversed
#define CRC32_TABLE_VALUES 256

// Prototypes
void   CRC32_init(void);
uint32 CRC32_byte(uint32, uint8);
uint32 CRC32_bytes(uint8 *, uint32);
uint32 CRC32_block_bytes(uint32, uint8 *, uint32);
uint32 CRC32_word(uint32, uint32);
uint32 CRC32_words(uint32 *, uint32);
uint32 CRC32_block_words(uint32, uint32 *, uint32);

#endif  // __CRC_H__
