// mandolin_config.h - A parser for the MANDOLIN audio control protocol.
//                     ASSUMES LITTLE-ENDIAN HOST BYTE ORDER!

#ifndef __MANDOLIN_CONFIG_H__
#define __MANDOLIN_CONFIG_H__

// Compile Time Switches - SET AS NEEDED!

// System & Language Switches
#define DYNAMIC_MEMORY_ALLOCATION_AVAILABLE 0 // 0 -> all memory must be passed in, 1 -> dynamically allocate memory
#define DEBUG_FIFO_BUFFER                   0 // 0 -> don't track maximum FIFO buffer fullness, 1 -> do track fullness
#define NO_LEGACY_PACK_AND_WRITE            1 // 1 -> use the new, improved packing tools, 0 -> use the same old crap

// FIFO Timeout Switches
#define ENABLE_MANDOLIN_TIMEOUT 0 // 0 -> wait (potentially infinitely) for the rest of any message with a valid header,
                                  // 1 -> time out the FIFO if messages are not received in a timely manner
                                  //      [Actually, this only invalidates the sync of any message whose payload takes too long to arrive -
                                  //      and can only do that when the parser is called after time expires.  It is in no way proactive.]
                                  //      NOTE:  Enabling timeouts requires time support as follows:
                                  //      1) an external uint32 'tick_count' in milliseconds,
                                  //      2) a 'TICKS_ELAPSED()' function or macro to compute elapsed time, and
                                  //      3) assurance that tick_count will never be zero.
#if ENABLE_MANDOLIN_TIMEOUT
#define MANDOLIN_TIMEOUT        1000 // Even at 9600 baud, it should only take a little more than 1 ms to receive each byte.
                                  // Hiccups of more than 2 ms mean something is wrong!
#endif

// FIFO Size Switches
#define MANDOLIN_FIFO_IS_POWER_OF_TWO   0      // 0 -> any FIFO size is OK, 1 -> all FIFO sizes must be powers of two
#define MANDOLIN_MAX_PAYLOAD_DATA_WORDS 512u    // 2^16 - 1 data words - the maximum words the current protocol supports
                                               // You may want to shrink this (significantly) if you don't anticipate using 16-bits of length
                                               // or if you have set MANDOLIN_FIFO_IS_POWER_OF_TWO, and want to use MANDOLIN_FIFO_BYTES in your code.
#define MANDOLIN_BUFFER_SLOP_BYTES      8u    // The current structure of the FIFO means that we lose the use of one byte,
                                               // so this should always be at least one.
                                               // Just rounding it up to a round 262200 bytes for now...

#endif // __MANDOLIN_CONFIG_H__
