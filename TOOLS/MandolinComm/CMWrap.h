// CMWrap.h - A parser for the CMWRAP audio control protocol.
//              ASSUMES LITTLE-ENDIAN HOST BYTE ORDER!

#ifndef __CMWRAP_H__
#define __CMWRAP_H__

// Compile Time Switches - SET AS NEEDED!

// System & Language Switches
#define DYNAMIC_MEMORY_ALLOCATION_AVAILABLE 0 // 0 -> all memory must be passed in, 1 -> dynamically allocate memory
#define DEBUG_FIFO_BUFFER                   0 // 0 -> don't track maximum FIFO buffer fullness, 1 -> do track fullness

// FIFO Timeout Switches
#define ENABLE_CMWRAP_TIMEOUT 1 // 0 -> wait (potentially infinitely) for the rest of any message with a valid header,
                                  // 1 -> time out the FIFO if messages are not received in a timely manner
                                  //      [Actually, this only invalidates the sync of any message whose payload takes too long to arrive -
                                  //      and can only do that when the parser is called after time expires.  It is in no way proactive.]
                                  //      NOTE:  Enabling timeouts requires time support as follows:
                                  //      1) an external uint32 'tick_count' in milliseconds,
                                  //      2) a 'TICKS_ELAPSED()' function or macro to compute elapsed time, and
                                  //      3) assurance that tick_count will never be zero.
#if ENABLE_CMWRAP_TIMEOUT
#define CMWRAP_TIMEOUT               3000  // make it big for now
                                  
#endif

// FIFO Size Switches
#define CMWRAP_FIFO_IS_POWER_OF_TWO   0      // 0 -> any FIFO size is OK, 1 -> all FIFO sizes must be powers of two
#define CMWRAP_MAX_PAYLOAD_DATA_WORDS (1024*2) //65535u // 2^16 - 1 data words - the maximum words the current protocol supports
                                               // You may want to shrink this (significantly) if you don't anticipate using 16-bits of length
                                               // or if you have set CMWRAP_FIFO_IS_POWER_OF_TWO, and want to use CMWRAP_FIFO_BYTES in your code.
#define CMWRAP_BUFFER_SLOP_BYTES      48u    // The current structure of the FIFO means that we lose the use of one byte,
                                               // so this should always be at least one.
                                               // Just rounding it up to a round 262200 bytes for now...

// THE DEFINES BELOW ARE PROTOCOL DEFINED - DON'T CHANGE THEM!
#define CMWRAP_HEADER_WORDS          4u
#define CMWRAP_MAX_MESSAGE_WORDS     (CMWRAP_HEADER_WORDS + CMWRAP_MAX_PAYLOAD_DATA_WORDS)
#define CMWRAP_BYTES_PER_WORD        4u
#define CMWRAP_HEADER_BYTES          (CMWRAP_BYTES_PER_WORD*CMWRAP_HEADER_WORDS)
#define CMWRAP_MAX_MESSAGE_BYTES     (CMWRAP_BYTES_PER_WORD*CMWRAP_MAX_MESSAGE_WORDS)
#define CMWRAP_FIFO_BYTES            (CMWRAP_MAX_MESSAGE_BYTES*4 + CMWRAP_BUFFER_SLOP_BYTES)    // MAB - Should be smaller than FIFO_TYPE_MAX/2 to avoid overflow!
// THE DEFINES ABOVE ARE PROTOCOL DEFINED - DON'T CHANGE THEM!

// DON'T CHANGE ANYTHING BELOW THIS LINE!


// Defines
#define CMWRAP_MAJOR_VERSION 1
#define CMWRAP_MINOR_VERSION 1
//      CMWRAP BUILD # 1          // This is only intended for development use!

#define CMWRAP_FILL_BYTE        0x00


// Enums

// Header

// Sync
#if defined(__ADSP21000__)
#define CMWRAP_SYNC_WORD ((uint32) 0x000000AC)
#else
#define CMWRAP_SYNC_WORD ((uint8) 0xAC)
#endif


// Mask & shift vs. transport flags byte
#define CMWRAP_TRANSPORT_NULL         0x00
#define CMWRAP_TRANSPORT_REPLY        0x01
#define CMWRAP_TRANSPORT_REPLY_MASK   ((uint8) CMWRAP_TRANSPORT_REPLY)
#define CMWRAP_TRANSPORT_REPLY_SHIFT  0
#define CMWRAP_TRANSPORT_STATUSCONTROL        0x02   // status = 0, control = 1
#define CMWRAP_TRANSPORT_STATUSCONTROL_MASK    ((uint8) CMWRAP_TRANSPORT_STATUSCONTROL)
#define CMWRAP_TRANSPORT_STATUSCONTROL_SHIFT   1
#define CMWRAP_TRANSPORT_METER        0x04   // not meter = 0, meter = 1
#define CMWRAP_TRANSPORT_METER_MASK    ((uint8) CMWRAP_TRANSPORT_METER)
#define CMWRAP_TRANSPORT_METER_SHIFT   2

#define IS_CMWRAP_REPLY(transport_flags)   (0!=((transport_flags) & (CMWRAP_TRANSPORT_REPLY_MASK)))
#define IS_CMWRAP_CONTROL(transport_flags)  (0!=((transport_flags) & (CMWRAP_TRANSPORT_STATUSCONTROL_MASK)))
#define IS_CMWRAP_METER(transport_flags)  (0!=((transport_flags) & (CMWRAP_TRANSPORT_METER_MASK)))

// Checksum
#define CMWRAP_HEADER_CHECKSUM_BYTES 14u

typedef enum {
    CMWRAP_BYTE_SYNC,        // 0
    CMWRAP_BYTE_TRANSPORT,   // 1
    CMWRAP_BYTE_LENGTH_HI,   // 2
    CMWRAP_BYTE_LENGTH_LO,   // 3

    CMWRAP_BYTE_SOURCE1_ID,    // 4
    CMWRAP_BYTE_SOURCE2_ID,    // 5
    CMWRAP_BYTE_SOURCE3_ID,    // 6
    CMWRAP_BYTE_SOURCE4_ID,    // 7
    CMWRAP_BYTE_SOURCE5_ID,    // 8
    CMWRAP_BYTE_SOURCE6_ID,    // 9
    CMWRAP_BYTE_SOURCE7_ID,    // 10
    CMWRAP_BYTE_SOURCE8_ID,    // 11

	CMWRAP_BYTE_PAYLOADCHECKSUM_HI, // 12
    CMWRAP_BYTE_PAYLOADCHECKSUM_LO, // 13
    CMWRAP_BYTE_HEADERCHECKSUM_HI, // 14
    CMWRAP_BYTE_HEADERCHECKSUM_LO, // 15
    CMWRAP_BYTE_PAYLOAD      // 16
} CMWrap_message_byte;

typedef struct {
    uint8  sync;
    uint8  transport;
    uint16 length;
    uint32 source_id_hi;
    uint32 source_id_lo;
    uint16 payload_checksum;
    uint16 header_checksum;
    void  *payload;
} CMWrap_message;

// Error
typedef enum {
    CMWRAP_NO_ERROR                  =  0,

    CMWRAP_ERROR                     =  1,

    CMWRAP_ERROR_BUSY,
    CMWRAP_ERROR_LOCKED,
    CMWRAP_ERROR_OUT_OF_MEMORY,
    CMWRAP_ERROR_TIMEOUT,
    CMWRAP_ERROR_CHECKSUM_MISMATCH,
    CMWRAP_ERROR_LENGTH_MISMATCH,

    CMWRAP_ERRORS
} CMWrap_error_code;


// Parser

// Error
typedef struct {
	uint8 transport;
    CMWrap_error_code error;
} CMWrap_parse_error;

// FIFO Types
#if defined(__ADSP21000__)
typedef uint32 CMWrap_byte_type;          // In this case it is up to the user to insure 8-bit bytes!
#else
typedef uint8  CMWrap_byte_type;
#endif
typedef uint32 CMWrap_fifo_type;          // All indices and buffer sizes must use the same data type.
#define CMWRAP_FIFO_TYPE_MAX 4294967295   // max 32-bit unsigned number

// FIFO
typedef struct {
    CMWrap_fifo_type  write_i;
    CMWrap_fifo_type  read_i;
    CMWrap_fifo_type  buffer_bytes;
#if CMWRAP_FIFO_IS_POWER_OF_TWO
    CMWrap_fifo_type  buffer_mask;
#endif
#if DEBUG_FIFO_BUFFER
    CMWrap_fifo_type  max_buffer_bytes;
#endif
    uint32              valid_header;
    uint32             rx_time;

    CMWrap_byte_type *buffer;
} CMWrap_fifo;


// Macros

#define PACK_SHORT(hi,lo) ((((uint16) (hi)) << 8) | (uint16) (lo))
#define PACK_LONG(hihi,hilo,lohi,lolo) ((((uint32) (hihi)) << 24) | (((uint32) (hilo)) << 16) | (((uint32) (lohi)) << 8) | ((uint32) (lolo)))

#define SHORT_LO(n) ((uint8) ( (n)       & 0x00FF))
#define SHORT_HI(n) ((uint8) (((n) >> 8) & 0x00FF))


// Prototypes
#if ENABLE_CMWRAP_TIMEOUT
uint32             CMWrap_FIFO_valid_stamp(CMWrap_fifo *);
#endif
CMWrap_fifo_type CMWrap_FIFO_max_bytes(CMWrap_fifo *);
bool32             CMWrap_FIFO_is_empty(CMWrap_fifo *);
bool32             CMWrap_FIFO_is_full(CMWrap_fifo *);
CMWrap_fifo_type CMWrap_FIFO_bytes_full(CMWrap_fifo *);
CMWrap_fifo_type CMWrap_FIFO_bytes_empty(CMWrap_fifo *);
#if DEBUG_FIFO_BUFFER
CMWrap_fifo_type CMWrap_FIFO_high_watermark(CMWrap_fifo *);
#endif
bool32             CMWrap_FIFO_init(CMWrap_fifo *, CMWrap_fifo_type, CMWrap_byte_type *);
void               CMWrap_FIFO_reset(CMWrap_fifo *);
#if DYNAMIC_MEMORY_ALLOCATION_AVAILABLE
bool32             CMWrap_FIFO_alloc(CMWrap_fifo *, CMWrap_fifo_type);
void               CMWrap_FIFO_free(CMWrap_fifo *);
#endif
void               CMWrap_FIFO_enqueue_byte(CMWrap_fifo *, CMWrap_byte_type);
void               CMWrap_FIFO_enqueue_bytes(CMWrap_fifo *, CMWrap_byte_type *, CMWrap_fifo_type);
CMWrap_byte_type CMWrap_FIFO_dequeue_byte(CMWrap_fifo *);
CMWrap_fifo_type CMWrap_FIFO_dequeue_bytes(CMWrap_fifo *, CMWrap_byte_type *, CMWrap_fifo_type);
CMWrap_byte_type CMWrap_FIFO_get_byte(CMWrap_fifo *, CMWrap_fifo_type);
CMWrap_fifo_type CMWrap_FIFO_dump_bytes(CMWrap_fifo *, CMWrap_fifo_type);

CMWrap_fifo_type CMWrap_FIFO_parse(CMWrap_fifo *, CMWrap_parse_error *);

CMWrap_fifo_type CMWrap_MSG_bytes(CMWrap_message *);
void               CMWrap_MSG_erase(CMWrap_fifo *);
CMWrap_fifo_type CMWrap_MSG_readbuffer(CMWrap_byte_type **pBuffer, uint16* nLength, CMWrap_message*, CMWrap_byte_type*);
CMWrap_fifo_type CMWrap_MSG_read(CMWrap_fifo *, CMWrap_message *, CMWrap_byte_type *);
#if DYNAMIC_MEMORY_ALLOCATION_AVAILABLE
bool32             CMWrap_MSG_copy(CMWrap_fifo *, CMWrap_message **);
bool32             CMWrap_MSG_alloc(CMWrap_message **, CMWrap_fifo_type);
void               CMWrap_MSG_free(CMWrap_message *);
#endif
void               CMWrap_MSG_pack(CMWrap_message *);
CMWrap_fifo_type CMWrap_MSG_write(CMWrap_byte_type *, CMWrap_message *);
bool32             CMWrap_MSG_enqueue(bool32 (*fifo_enqueue)(CMWrap_byte_type), CMWrap_message *);
void               CMWrap_MSG_print(CMWrap_message *);

#endif // __CMWRAP_H__
