// crc.c

#include <assert.h>     // Unfortunately, this is only needed if CRC32_USE_TABLE is defined...  Otherwise, width assumptions won't be tested!
#include "crc.h"


// Fail here if bit width assumptions are wrong!
static void CRC32_check_types(void)
{
#if defined(__ADSP21000__)
    assert(4==sizeof(int32));
    assert(4==sizeof(uint32));
    assert(4==sizeof(float32));
    assert(4==sizeof(bool32));
#else
    assert(4==sizeof(int32));
    assert(2==sizeof(int16));
    assert(1==sizeof(int8));

    assert(4==sizeof(uint32));
    assert(2==sizeof(uint16));
    assert(1==sizeof(uint8));

    assert(4==sizeof(bool32));

    assert(8==sizeof(float64));
    assert(4==sizeof(float32));
#endif
}


#if CRC32_USE_TABLE
static bool   CRC32_table_inited = false;
static uint32 CRC32_table[CRC32_TABLE_VALUES];
#endif

void CRC32_init(void)
{
#if CRC32_USE_TABLE
    uint32 i,j,crc,mask;

    CRC32_check_types();

    if(!CRC32_table_inited) {
        for(j=0; j<CRC32_TABLE_VALUES; j++) {
            crc = j;

            for(i=0; i<8u; i++) {
                mask = -(crc &  1);
                crc  =  (crc >> 1) ^ (CRC32_POLYNOMIAL & mask);
            }

            CRC32_table[j] = crc;
        }
        CRC32_table_inited = true;
    }
#endif
}


// Pass a single byte through the CRC-32 polynomial with initial state crc.
// Taken from a sample chapter from Henry S. Warren, Jr.'s 'Hacker's Delight'.
uint32 CRC32_byte(uint32 crc, uint8 byte)
{
#if CRC32_USE_TABLE
    return (crc >> 8) ^ CRC32_table[(crc ^ byte) & 0x000000FF];

#else

    uint32 i,mask;

    crc = crc ^ byte;

    for(i=0; i<8u; i++) {
        mask = -(crc &  1);
        crc  =  (crc >> 1) ^ (CRC32_POLYNOMIAL & mask);
    }

    return crc;
#endif
}


// Compute the CRC of a complete stream of bytes.
uint32 CRC32_bytes(uint8 *byte, uint32 count)
{
    uint32 i,crc = CRC32_SEED;
    for(i=0; i<count; i++)
        crc = CRC32_byte(crc,byte[i]);
    return ~crc;
}


// Compute the CRC of a block of bytes - used when the bytes are not contiguous and have to be done in blocks.
// You should initialize crc to CRC32_SEED before processing the first block,
// and take the one's complement of the returned CRC value after processing the last block.
uint32 CRC32_block_bytes(uint32 crc, uint8 *byte, uint32 count)
{
    uint32 i;
    for(i=0; i<count; i++)
        crc = CRC32_byte(crc,byte[i]);
    return crc;
}


uint32 CRC32_word(uint32 crc, uint32 word)
{
    crc =  CRC32_byte(crc, (uint8) ((word    ) & 0x000000FF));
    crc =  CRC32_byte(crc, (uint8) ((word>> 8) & 0x000000FF));
    crc =  CRC32_byte(crc, (uint8) ((word>>16) & 0x000000FF));
    return CRC32_byte(crc, (uint8) ((word>>24) & 0x000000FF));
}


// Compute the CRC of a complete stream of 32-bit words.
uint32 CRC32_words(uint32 *word, uint32 count)
{
    uint32 i,crc = CRC32_SEED;
    for(i=0; i<count; i++)
        crc = CRC32_word(crc,word[i]);
    return ~crc;
}


// Compute the CRC of a block of 32-bit words - used when the words are not contiguous and have to be done in blocks.
// You should initialize crc to CRC32_SEED before processing the first block,
// and take the one's complement of the returned CRC value after processing the last block.
uint32 CRC32_block_words(uint32 crc, uint32 *word, uint32 count)
{
    uint32 i;
    for(i=0; i<count; i++)
        crc = CRC32_word(crc,word[i]);
    return crc;
}
