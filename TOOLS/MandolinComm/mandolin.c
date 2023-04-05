// mandolin.c - A parser for the MANDOLIN audio control protocol.
//              ASSUMES LITTLE-ENDIAN HOST BYTE ORDER!

#include "LOUD_types.h" // standard types - #define any compile switches either in the project options or above this line
#include <assert.h>

#if 0
#include "config.h"     // or whichever header #defines VERBOSE, SUCCINCT for the project
#else
#if !defined(VERBOSE)
#define VERBOSE  0
#endif
#if !defined(SUCCINCT)
#define SUCCINCT 0
#endif
#endif

#include "mandolin.h"

#if VERBOSE || SUCCINCT
#include <stdio.h>
#endif

#if DYNAMIC_MEMORY_ALLOCATION_AVAILABLE
#include <stdlib.h>     // needed for NULL, c/malloc(), free()
#else
#if !defined(NULL)
#define NULL 0
#endif
#endif

#if NO_LEGACY_PACK_AND_WRITE
#include <string.h>     // needed for memcpy()
#endif

#if ENABLE_MANDOLIN_TIMEOUT
// External Time Support
#include "tick.h"
extern uint32 tick_count;


uint32 MANDOLIN_FIFO_valid_stamp(mandolin_fifo *bfp)
{
    return bfp->valid_header;   // timestamp applied when the last payload byte was processed, or 0 if no valid header
}
#endif


//
// DEBUG FUNCTIONS
//

// Fail here if bit width assumptions are wrong!
static void MANDOLIN_check_types(void)
{
#if defined(__ADSP21000__)
    assert(1==sizeof(int32));
    assert(1==sizeof(uint32));
    assert(1==sizeof(float32));
    assert(1==sizeof(bool32));
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


char *MANDOLIN_MSG_id_string(mandolin_message_id msgid)
{
    switch(msgid) {
        case MANDOLIN_MSG_INVALID:                          return "INVALID ID";

        // Core
        case MANDOLIN_MSG_PING:                             return "PING";
        case MANDOLIN_MSG_ADVERTIZE_HARDWARE_INFO:          return "ADVERTIZE HARD INFO";
        case MANDOLIN_MSG_GET_HARDWARE_INFO:                return "GET HARD INFO";
        case MANDOLIN_MSG_GET_SOFTWARE_INFO:                return "GET SOFT INFO";
        case MANDOLIN_MSG_REBOOT:                           return "REBOOT";

        // Event
        case MANDOLIN_MSG_TRIGGER_SYSTEM_EVENT:             return "TRIGGER SYSTEM EVENT";
        case MANDOLIN_MSG_TRIGGER_APPLICATION_EVENT:        return "TRIGGER APP EVENT";

        // Test
        case MANDOLIN_MSG_TEST:                             return "TEST";

        // Info
        case MANDOLIN_MSG_GET_INFO:                         return "GET INFO";

        // File
        case MANDOLIN_MSG_FILE_OPEN:                        return "FILE OPEN";
        case MANDOLIN_MSG_FILE_CLOSE:                       return "FILE CLOSE";
        case MANDOLIN_MSG_GET_FILE:                         return "FILE GET";
        case MANDOLIN_MSG_POST_FILE:                        return "FILE POST";

        // State
        case MANDOLIN_MSG_GET_SYSTEM_STATUS:                return "GET SYSTEM STATUS";
        case MANDOLIN_MSG_SET_SYSTEM_STATE:                 return "SET SYSTEM STATE";
        case MANDOLIN_MSG_RESET_SYSTEM_STATE:               return "RESET SYSTEM STATE";
        case MANDOLIN_MSG_SUBSCRIBE_SYSTEM_STATE:           return "SUBSCRIBE SYSTEM STATE";

        // Parameter
        case MANDOLIN_MSG_GET_APPLICATION_PARAMETER:        return "GET APP PARAMETER";
        case MANDOLIN_MSG_SET_APPLICATION_PARAMETER:        return "SET APP PARAMETER";
        case MANDOLIN_MSG_RESET_APPLICATION_PARAMETER:      return "RESET APP PARAMETER";
        case MANDOLIN_MSG_SUBSCRIBE_APPLICATION_PARAMETER:  return "SUBSCRIBE APP PARAMETER";
        case MANDOLIN_MSG_CREATE_PARAMETER_LIST:            return "CREATE PARAMETER LIST";

        // String
        case MANDOLIN_MSG_GET_APPLICATION_STRING:           return "GET APP STRING";
        case MANDOLIN_MSG_SET_APPLICATION_STRING:           return "SET APP STRING";
        case MANDOLIN_MSG_RESET_APPLICATION_STRING:         return "RESET APP STRING";

        default:                                            return "UNRECOGNIZED ID";
    }
}


// Print a MANDOLIN message to the standard output.
void MANDOLIN_MSG_print(mandolin_message *bmp)
{
#if VERBOSE
    mandolin_fifo_type i;

    // Print header
    fprintf(stdout,"header:  ");
    fprintf(stdout,"0x%02x ",bmp->sync);
    fprintf(stdout,"0x%02x ",bmp->sequence);
    fprintf(stdout,"0x%02x ",SHORT_HI(bmp->length));
    fprintf(stdout,"0x%02x ",SHORT_LO(bmp->length));
    fprintf(stdout,"0x%02x ",bmp->transport);
    fprintf(stdout,"0x%02x ",bmp->id);
    fprintf(stdout,"0x%02x ",SHORT_HI(bmp->checksum));
    fprintf(stdout,"0x%02x ",SHORT_LO(bmp->checksum));

    // Print payload (from low byte index to high - if words on the wire are network ordered, this is big-endian)
    fprintf(stdout,"\npayload:  ");
    if(bmp->length) {
        mandolin_byte_type *payload_ptr = (mandolin_byte_type *) bmp->payload;

        for(i=0; i<MANDOLIN_BYTES_PER_WORD*(1 + (mandolin_fifo_type) bmp->length); i++)
            fprintf(stdout,"0x%02x ",payload_ptr[i]);
    }
    fprintf(stdout,"\n");
#endif
}


//
// FIFO MANAGEMENT FUNCTIONS
//

// Returns the maximum number of bytes that can be held in the FIFO.
// The current construction of the FIFO, which has no 'current' byte count,
// causes us to lose the use of one byte of the FIFO, but should gain read/write thread independence...
mandolin_fifo_type MANDOLIN_FIFO_max_bytes(mandolin_fifo *bfp)
{
    return bfp->buffer_bytes - 1;
}


// 'Thread safe' for reading.
bool MANDOLIN_FIFO_is_empty(mandolin_fifo *bfp)
{
    return bfp->read_i==bfp->write_i;
}


// 'Thread safe' for writing.
bool MANDOLIN_FIFO_is_full(mandolin_fifo *bfp)
{
    return (bfp->read_i)? bfp->read_i==bfp->write_i+1 : bfp->buffer_bytes==bfp->write_i+1;

    // r -> 0, but changes after test ==> OK, as false branch doesn't depend on r  (can get false full, which is OK)
    // r -> MFP-1, but wraps after test ==> OK, as r advancing can't cause a false !full
    // r -> MFP (due to interrupt of dequeue) ==> OK to put single byte in FIFO, as data has already been read
}


// 'Thread safe' for reading.
mandolin_fifo_type MANDOLIN_FIFO_bytes_full(mandolin_fifo *bfp)
{
    mandolin_fifo_type bytes = bfp->write_i + bfp->buffer_bytes - bfp->read_i;  // latch indices

    return (bytes>=bfp->buffer_bytes)? bytes - bfp->buffer_bytes : bytes;       // test
}


// 'Thread safe' for writing.
mandolin_fifo_type MANDOLIN_FIFO_bytes_empty(mandolin_fifo *bfp)
{
    return bfp->buffer_bytes-1 - MANDOLIN_FIFO_bytes_full(bfp);
}


#if DEBUG_FIFO_BUFFER
// Read and reset the FIFO's high watermark.
mandolin_fifo_type MANDOLIN_FIFO_high_watermark(mandolin_fifo *bfp)
{
    mandolin_fifo_type max_buffer_bytes = bfp->max_buffer_bytes;

    bfp->max_buffer_bytes = MANDOLIN_FIFO_bytes_full(bfp);

    return max_buffer_bytes;
}
#endif


// Initialize a FIFO.
// fifo_bufptr should be a pointer to an array of bytes of length fifo_bytes.
bool MANDOLIN_FIFO_init(mandolin_fifo *bfp, mandolin_fifo_type fifo_bytes, mandolin_byte_type *fifo_bufptr)
{
    // Sanity check
    MANDOLIN_check_types();

    bfp->write_i      = 0;
    bfp->read_i       = 0;
    bfp->buffer_bytes = fifo_bytes;
    bfp->valid_header = 0;

#if DEBUG_FIFO_BUFFER
    bfp->max_buffer_bytes = 0;
#endif

#if MANDOLIN_FIFO_IS_POWER_OF_TWO
    if(fifo_bytes && !(fifo_bytes & fifo_bytes-1))
        bfp->buffer_mask = fifo_bytes - 1;
    else {
        // fifo_bytes is not a power of two!
        return false;
    }
#endif

    bfp->buffer = fifo_bufptr;

    return true;
}


// Reset a FIFO to its initial state (without freeing or reinitializing the buffer).
void MANDOLIN_FIFO_reset(mandolin_fifo *bfp)
{
    bfp->write_i      = 0;
    bfp->read_i       = 0;
    bfp->valid_header = 0;
}


#if DYNAMIC_MEMORY_ALLOCATION_AVAILABLE
// Initialize a FIFO.
// The FIFO buffer will be dynamically allocated.
bool MANDOLIN_FIFO_alloc(mandolin_fifo *bfp, mandolin_fifo_type fifo_bytes)
{
    // Sanity check
    MANDOLIN_check_types();

    bfp->write_i      = 0;
    bfp->read_i       = 0;
    bfp->buffer_bytes = fifo_bytes;
    bfp->valid_header = 0;

#if DEBUG_FIFO_BUFFER
    bfp->max_buffer_bytes = 0;
#endif

#if MANDOLIN_FIFO_IS_POWER_OF_TWO
    if(fifo_bytes && !(fifo_bytes & fifo_bytes-1))
        bfp->buffer_mask = fifo_bytes - 1;
    else {
        // fifo_bytes is not a power of two!
        return false;
    }
#endif

    // Zero initialize the buffer
    if(NULL==(bfp->buffer = (mandolin_byte_type *) calloc(fifo_bytes,sizeof(mandolin_byte_type))))
        return false;

    return true;
}


// Free a FIFO.
void MANDOLIN_FIFO_free(mandolin_fifo *bfp)
{
    if(NULL!=bfp->buffer) {
        free(bfp->buffer);
        bfp->buffer = NULL;
    }
}
#endif


// Add a byte to the FIFO.
// Doesn't check for buffer fullness!
// 'Thread safe' for writing.
void MANDOLIN_FIFO_enqueue_byte(mandolin_fifo *bfp, mandolin_byte_type byte)
{
    bfp->buffer[bfp->write_i++] = byte; // write
                                        // increment index
#if MANDOLIN_FIFO_IS_POWER_OF_TWO
    bfp->write_i &= bfp->buffer_mask;   // wrap index
#else
    if(bfp->buffer_bytes==bfp->write_i) // wrap index
        bfp->write_i = 0;
#endif

#if DEBUG_FIFO_BUFFER
    if(MANDOLIN_FIFO_bytes_full(bfp)>bfp->max_buffer_bytes)
        bfp->max_buffer_bytes = MANDOLIN_FIFO_bytes_full(bfp);
#endif
}


// Add an array of n bytes to the FIFO.
// Doesn't check for buffer fullness!
void MANDOLIN_FIFO_enqueue_bytes(mandolin_fifo *bfp, mandolin_byte_type *bytep, mandolin_fifo_type n)
{
    mandolin_fifo_type bytes_til_wrap = bfp->buffer_bytes - bfp->write_i;

    if(n>=bytes_til_wrap) {
        n -= bytes_til_wrap;

        while(bytes_til_wrap--)
            bfp->buffer[bfp->write_i++] = *bytep++;

        bfp->write_i = 0;
    }

    while(n--)
        bfp->buffer[bfp->write_i++] = *bytep++;

#if DEBUG_FIFO_BUFFER
    if(MANDOLIN_FIFO_bytes_full(bfp)>bfp->max_buffer_bytes)
        bfp->max_buffer_bytes = MANDOLIN_FIFO_bytes_full(bfp);
#endif
}


// Remove and return a byte from the FIFO.
// Doesn't check for buffer emptiness!
// 'Thread safe' for reading.
mandolin_byte_type MANDOLIN_FIFO_dequeue_byte(mandolin_fifo *bfp)
{
    mandolin_byte_type byte = bfp->buffer[bfp->read_i++];   // read
                                                            // increment index
#if MANDOLIN_FIFO_IS_POWER_OF_TWO
    bfp->read_i &= bfp->buffer_mask;                        // wrap index
#else
    if(bfp->buffer_bytes==bfp->read_i)                      // wrap index
        bfp->read_i = 0;
#endif

    return byte;
}


// Remove and return an array of n bytes from the FIFO.
// Caller should provide a buffer of length n.
// Returns the actual number of bytes dequeued.
// Note that all bytes will be in network (BIG-ENDIAN) order.
mandolin_fifo_type MANDOLIN_FIFO_dequeue_bytes(mandolin_fifo *bfp, mandolin_byte_type *bytep, mandolin_fifo_type n)
{
    mandolin_fifo_type bytes_in_fifo    = MANDOLIN_FIFO_bytes_full(bfp);
    mandolin_fifo_type bytes_to_dequeue = (n>bytes_in_fifo)? bytes_in_fifo : n;
    mandolin_fifo_type bytes_til_wrap   = bfp->buffer_bytes - bfp->read_i;

    n = bytes_to_dequeue;

    if(n>=bytes_til_wrap) {
        n -= bytes_til_wrap;

        while(bytes_til_wrap--)
            *bytep++ = bfp->buffer[bfp->read_i++];

        bfp->read_i = 0;
    }

    while(n--)
        *bytep++ = bfp->buffer[bfp->read_i++];

    return bytes_to_dequeue;
}


// Return the value of the nth oldest FIFO bin.
// Return value is undefined if n > bfp->buffer_bytes.
mandolin_byte_type MANDOLIN_FIFO_get_byte(mandolin_fifo *bfp, mandolin_fifo_type n)
{
    mandolin_fifo_type byte_to_get = bfp->read_i + n;

    while(bfp->buffer_bytes<=byte_to_get)
        byte_to_get -= bfp->buffer_bytes;

    return bfp->buffer[byte_to_get];
}


// Remove up to the n oldest bytes from the FIFO.
// Returns the actual number of bytes dumped.
mandolin_fifo_type MANDOLIN_FIFO_dump_bytes(mandolin_fifo *bfp, mandolin_fifo_type n)
{
    mandolin_fifo_type bytes_in_fifo  = MANDOLIN_FIFO_bytes_full(bfp);
    mandolin_fifo_type bytes_to_erase = (n>bytes_in_fifo)? bytes_in_fifo : n;

    bfp->read_i += bytes_to_erase;

    while(bfp->buffer_bytes<=bfp->read_i)
        bfp->read_i -= bfp->buffer_bytes;

    return bytes_to_erase;
}


//
// PARSER
//

// Assumes that a valid header has already been located and is at the tail of the FIFO.
// Returns the length of the valid message (including header) on success,
//         0 if more bytes are needed, or
//        -1 on error (timeout or checksum mismatch).
static int32 parse_payload(mandolin_fifo *bfp, mandolin_parse_error *error)
{
    // Look for valid payload
#if defined(__ADSP21000__)
    uint32 payload_length = PACK_SHORT(MANDOLIN_FIFO_get_byte(bfp,MANDOLIN_BYTE_LENGTH_HI),MANDOLIN_FIFO_get_byte(bfp,MANDOLIN_BYTE_LENGTH_LO));
#else
    uint16 payload_length = PACK_SHORT(MANDOLIN_FIFO_get_byte(bfp,MANDOLIN_BYTE_LENGTH_HI),MANDOLIN_FIFO_get_byte(bfp,MANDOLIN_BYTE_LENGTH_LO));
#endif

    if(payload_length) {
        mandolin_fifo_type message_length_bytes;

#if ENABLE_MANDOLIN_TIMEOUT
        // Latch current (non-zero) timestamp
        uint32 current_count = tick_count;

        if(bfp->valid_header) {
            // Check watchdog
            uint32 ticks_elapsed = TICKS_ELAPSED(current_count,bfp->valid_header);

            if(ticks_elapsed>MANDOLIN_TIMEOUT) {
                // Log timeout
                error->error     = MANDOLIN_ERROR_TIMEOUT;
                error->sequence  = MANDOLIN_FIFO_get_byte(bfp,MANDOLIN_BYTE_SEQUENCE);
                error->transport = MANDOLIN_FIFO_get_byte(bfp,MANDOLIN_BYTE_TRANSPORT);
                error->id        = MANDOLIN_FIFO_get_byte(bfp,MANDOLIN_BYTE_MSG_ID);
#if SUCCINCT
                fprintf(stdout,"Error #%d occurred while parsing!\n",MANDOLIN_ERROR_TIMEOUT);
#endif
                // Discard sync
                MANDOLIN_FIFO_dump_bytes(bfp,1);

                // Invalidate header
                bfp->valid_header = 0;

                // Iterate
                return -1;
            }
        }

        // Stuff flag with current timestamp
        bfp->valid_header = current_count;
#else
        // Validate header
        bfp->valid_header = 1;
#endif

        // Look for valid payload checksum
        message_length_bytes = MANDOLIN_HEADER_BYTES + MANDOLIN_BYTES_PER_WORD*(1 + (mandolin_fifo_type) payload_length);
        if(MANDOLIN_FIFO_bytes_full(bfp)<message_length_bytes)
            return 0;
        else {
            mandolin_fifo_type i;
            uint32             checksum,payload_checksum;

            // Compute checksum of payload
            checksum = 0;
            for(i=MANDOLIN_HEADER_BYTES; i<message_length_bytes-4; i++)
                checksum += (uint32) MANDOLIN_FIFO_get_byte(bfp,i);
            checksum = ~checksum;

            // Assemble payload checksum
            if(IS_MANDOLIN_NETWORK_ENDIAN(MANDOLIN_FIFO_get_byte(bfp,MANDOLIN_BYTE_TRANSPORT)))
                payload_checksum = PACK_LONG(MANDOLIN_FIFO_get_byte(bfp,message_length_bytes-4),MANDOLIN_FIFO_get_byte(bfp,message_length_bytes-3),MANDOLIN_FIFO_get_byte(bfp,message_length_bytes-2),MANDOLIN_FIFO_get_byte(bfp,message_length_bytes-1));
            else
                payload_checksum = PACK_LONG(MANDOLIN_FIFO_get_byte(bfp,message_length_bytes-1),MANDOLIN_FIFO_get_byte(bfp,message_length_bytes-2),MANDOLIN_FIFO_get_byte(bfp,message_length_bytes-3),MANDOLIN_FIFO_get_byte(bfp,message_length_bytes-4));

            // Verify payload checksum
            if(checksum==payload_checksum) {
                // Invalidate header
                bfp->valid_header = 0;

                // Return length of message
                return MANDOLIN_HEADER_WORDS + 1 + (mandolin_fifo_type) payload_length;
            }
            else {
                // Log mismatch
                error->error     = MANDOLIN_ERROR_CHECKSUM_MISMATCH;
                error->sequence  = MANDOLIN_FIFO_get_byte(bfp,MANDOLIN_BYTE_SEQUENCE);
                error->transport = MANDOLIN_FIFO_get_byte(bfp,MANDOLIN_BYTE_TRANSPORT);
                error->id        = MANDOLIN_FIFO_get_byte(bfp,MANDOLIN_BYTE_MSG_ID);
#if SUCCINCT
                fprintf(stdout,"Error #%d occurred while parsing!\n",MANDOLIN_ERROR_CHECKSUM_MISMATCH);
#endif
                // Discard sync
                MANDOLIN_FIFO_dump_bytes(bfp,1);

                // Invalidate header
                bfp->valid_header = 0;

                // Iterate
                return -1;
            }
        }
    }
    else {
        // Return length of message
        return MANDOLIN_HEADER_WORDS;
    }
}

// Checks the FIFO for a valid MANDOLIN message.
// Returns the length in words of the entire message if a valid message is detected (the message will be located at the tail of the FIFO),
//         0 if no valid message is detected or on error (timeout or checksum mismatch).
mandolin_fifo_type MANDOLIN_FIFO_parse(mandolin_fifo *bfp, mandolin_parse_error *error)
{
    // Clear any error condition
    error->error = MANDOLIN_NO_ERROR;

    do {
        if(bfp->valid_header) { // This case, while not strictly necessary, removes a lot of potentially unnecessary header checksum computation
            // Look for valid payload
            int32 message_words = parse_payload(bfp,error);

            if(message_words<0)
                continue;
            else
                return message_words;
        }
        else if(MANDOLIN_FIFO_bytes_full(bfp)<MANDOLIN_HEADER_BYTES)
            return 0;
        else {
            // Look for sync word
#if defined(__ADSP21000__)
            uint32 sync_word = MANDOLIN_FIFO_get_byte(bfp,MANDOLIN_BYTE_SYNC);
#else
            uint8 sync_word = MANDOLIN_FIFO_get_byte(bfp,MANDOLIN_BYTE_SYNC);
#endif

            while(MANDOLIN_SYNC_WORD!=sync_word) {
                MANDOLIN_FIFO_dequeue_byte(bfp);

                if(MANDOLIN_FIFO_is_empty(bfp))
                    return 0;
                else
                    sync_word = MANDOLIN_FIFO_get_byte(bfp,MANDOLIN_BYTE_SYNC);
            }

            // Look for valid header checksum
            if(MANDOLIN_FIFO_bytes_full(bfp)<MANDOLIN_HEADER_BYTES)
                return 0;
            else {
                mandolin_fifo_type i;
#if defined(__ADSP21000__)
                uint32          checksum16,header_checksum;
#else
                uint16          checksum16,header_checksum;
#endif

                // Compute checksum of header
                checksum16 = 0;
                for(i=0; i<MANDOLIN_HEADER_CHECKSUM_BYTES; i++)
                    checksum16 += MANDOLIN_FIFO_get_byte(bfp,i);
#if defined(__ADSP21000__)
                checksum16 = (~checksum16) & 0x0000FFFF;
#else
                checksum16 = ~checksum16;
#endif

                // Assemble header checksum
                header_checksum = PACK_SHORT(MANDOLIN_FIFO_get_byte(bfp,MANDOLIN_BYTE_CHECKSUM_HI),MANDOLIN_FIFO_get_byte(bfp,MANDOLIN_BYTE_CHECKSUM_LO));

                // Verify header checksum
                if(checksum16==header_checksum) {
                    // Look for valid payload
                    int32 message_words = parse_payload(bfp,error);

                    if(message_words<0)
                        continue;
                    else
                        return message_words;
                }
                else {
                    // Discard sync
                    MANDOLIN_FIFO_dump_bytes(bfp,1);

                    // Iterate
                    continue;
                }
            }
        }
    }
    while(MANDOLIN_NO_ERROR==error->error);

    return 0;
}


//
// MESSAGE MANAGEMENT FUNCTIONS
//

// Return the length of a MANDOLIN message in bytes.
mandolin_fifo_type MANDOLIN_MSG_bytes(mandolin_message *bmp)
{
    return MANDOLIN_BYTES_PER_WORD*(MANDOLIN_HEADER_WORDS + ((bmp->length)? 1 + (mandolin_fifo_type) bmp->length : 0));
}


// Erase a valid MANDOLIN message from the tail of the FIFO.
// NOTE:  This function is mildly dangerous - if there is no valid message, the results of the call are unpredictable!
void MANDOLIN_MSG_erase(mandolin_fifo *bfp)
{
    // Read in message length
#if defined(__ADSP21000__)
    uint32 payload_length = PACK_SHORT(MANDOLIN_FIFO_get_byte(bfp,MANDOLIN_BYTE_LENGTH_HI),MANDOLIN_FIFO_get_byte(bfp,MANDOLIN_BYTE_LENGTH_LO));
#else
    uint16 payload_length = PACK_SHORT(MANDOLIN_FIFO_get_byte(bfp,MANDOLIN_BYTE_LENGTH_HI),MANDOLIN_FIFO_get_byte(bfp,MANDOLIN_BYTE_LENGTH_LO));
#endif

    // Add any payload checksum
    payload_length += payload_length? 1 : 0;

    // Erase entire message
    MANDOLIN_FIFO_dump_bytes(bfp,MANDOLIN_HEADER_BYTES + MANDOLIN_BYTES_PER_WORD*(mandolin_fifo_type) payload_length);
}


mandolin_fifo_type MANDOLIN_MSG_readbuffer(mandolin_byte_type **pBuffer, uint16* nByteLength, mandolin_message *bmp, mandolin_byte_type *payload_ptr)
{
	 mandolin_byte_type byte;

	 mandolin_byte_type *buffer;

	 if (*nByteLength < MANDOLIN_HEADER_BYTES)
	 {
		 return 0;
	 }
	 buffer = *pBuffer;

    // Read in header one byte at a time

    // Read sync
    bmp->sync = *buffer++;
	if (bmp->sync != MANDOLIN_SYNC_WORD) return 0;


    // Read sequence
    bmp->sequence = *buffer++;

    // Assemble length
    byte = *buffer++;
    bmp->length = PACK_SHORT(byte,*buffer++);
	if (bmp->length > MANDOLIN_MAX_PAYLOAD_DATA_WORDS) 
		return 0;

    // Read transport flags
    bmp->transport = *buffer++;

    // Read message ID
    bmp->id = *buffer++;

    // Assemble header checksum
    byte = *buffer++;
    bmp->checksum = PACK_SHORT(byte,*buffer++);

	// validity check
	if ( (bmp->checksum & 0x0ffff) != 
		((~( (bmp->sync+bmp->sequence+
		  ((bmp->length>>8) & 0x0ff) + (bmp->length & 0x0ff) +
		   (bmp->transport) + (bmp->id) ) ) & 0x0ffff)) )
	{
		return 0;
	}

    if(bmp->length) 
	{
        mandolin_fifo_type i;
		mandolin_fifo_type payload_bytes = MANDOLIN_BYTES_PER_WORD*(1 + (mandolin_fifo_type) bmp->length);
		int32 checksum = 0;
		int32 checksum_calc = 0;

        // Assign payload
        bmp->payload = payload_ptr;

        // Read in payload + checksum one byte at a time, unscrambling network byte order into host byte order
        for(i=0; i<(payload_bytes-4); i+=4) 
		{
            payload_ptr[i+3] = *buffer++;
            payload_ptr[i+2] = *buffer++;
            payload_ptr[i+1] = *buffer++;
            payload_ptr[i+0] = *buffer++;

			checksum += (payload_ptr[i+3] + payload_ptr[i+2] + payload_ptr[i+1] + payload_ptr[i+0]);
        }
		// Mandolin payload
        payload_ptr[i+3] = *buffer++;
        payload_ptr[i+2] = *buffer++;
        payload_ptr[i+1] = *buffer++;
        payload_ptr[i+0] = *buffer++;

		checksum_calc = ((payload_ptr[i+3] & 0x0ff)<<24) + ((payload_ptr[i+2] & 0x0ff)<<16) + 
			((payload_ptr[i+1] & 0x0ff)<<8) + ((payload_ptr[i+0] & 0x0ff)<<0);
		checksum_calc = ~checksum_calc;
		if (checksum_calc  != checksum )
		{
			return 0;
		}
    }
    else
        bmp->payload = NULL;

	if (*nByteLength < (buffer - *pBuffer))
	{
		return 0;
	}
	*nByteLength -= (buffer - *pBuffer); 
	*pBuffer = buffer;
    return MANDOLIN_MSG_bytes(bmp);
}

// Read a valid MANDOLIN message from the tail of the FIFO and into its own structure.
// Caller should provide both a message (header) structure and a (payload) buffer of appropriate length.
// Returns the actual number of bytes dequeued.
// NOTE:  This function is mildly dangerous - if there is no valid message, the results of the call are unpredictable!
mandolin_fifo_type MANDOLIN_MSG_read(mandolin_fifo *bfp, mandolin_message *bmp, mandolin_byte_type *payload_ptr)
{
    mandolin_byte_type byte;

    // Read in header one byte at a time

    // Read sync
    bmp->sync = MANDOLIN_FIFO_dequeue_byte(bfp);

    // Read sequence
    bmp->sequence = MANDOLIN_FIFO_dequeue_byte(bfp);

    // Assemble length
    byte = MANDOLIN_FIFO_dequeue_byte(bfp);
    bmp->length = PACK_SHORT(byte,MANDOLIN_FIFO_dequeue_byte(bfp));

    // Read transport flags
    bmp->transport = MANDOLIN_FIFO_dequeue_byte(bfp);

    // Read message ID
    bmp->id = MANDOLIN_FIFO_dequeue_byte(bfp);

    // Assemble header checksum
    byte = MANDOLIN_FIFO_dequeue_byte(bfp);
    bmp->checksum = PACK_SHORT(byte,MANDOLIN_FIFO_dequeue_byte(bfp));

    if(bmp->length) {
        mandolin_fifo_type i,payload_bytes = MANDOLIN_BYTES_PER_WORD*(1 + (mandolin_fifo_type) bmp->length);

        // Assign payload
        bmp->payload = payload_ptr;

        if(IS_MANDOLIN_NETWORK_ENDIAN(bmp->transport)) {
            // Read in payload + checksum one byte at a time, unscrambling network byte order into host byte order
            for(i=0; i<payload_bytes; i+=4) {
                payload_ptr[i+3] = MANDOLIN_FIFO_dequeue_byte(bfp);
                payload_ptr[i+2] = MANDOLIN_FIFO_dequeue_byte(bfp);
                payload_ptr[i+1] = MANDOLIN_FIFO_dequeue_byte(bfp);
                payload_ptr[i+0] = MANDOLIN_FIFO_dequeue_byte(bfp);
            }
        }
        else {
            // Read in payload + checksum in one go
            MANDOLIN_FIFO_dequeue_bytes(bfp,payload_ptr,payload_bytes);
        }
    }
    else
        bmp->payload = NULL;

    return MANDOLIN_MSG_bytes(bmp);
}


#if DYNAMIC_MEMORY_ALLOCATION_AVAILABLE
// Read a valid MANDOLIN message from the tail of the FIFO and into its own structure.
// Dynamically allocates memory for the message.
// Returns false on failure (of memory allocation), true otherwise.
bool MANDOLIN_MSG_copy(mandolin_fifo *bfp, mandolin_message **bmpp)
{
    mandolin_byte_type byte;

    mandolin_message *bmp = NULL;

    // Allocate header
    if(NULL==(bmp = (mandolin_message *) malloc(sizeof(mandolin_message))))
        return false;

    // Read in header one byte at a time

    // Read sync
    bmp->sync = MANDOLIN_FIFO_dequeue_byte(bfp);

    // Read sequence
    bmp->sequence = MANDOLIN_FIFO_dequeue_byte(bfp);

    // Assemble length
    byte = MANDOLIN_FIFO_dequeue_byte(bfp);
    bmp->length = PACK_SHORT(byte,MANDOLIN_FIFO_dequeue_byte(bfp));

    // Read transport flags
    bmp->transport = MANDOLIN_FIFO_dequeue_byte(bfp);

    // Read message ID
    bmp->id = MANDOLIN_FIFO_dequeue_byte(bfp);

    // Assemble header checksum
    byte = MANDOLIN_FIFO_dequeue_byte(bfp);
    bmp->checksum = PACK_SHORT(byte,MANDOLIN_FIFO_dequeue_byte(bfp));

    if(bmp->length) {
        mandolin_byte_type *payload_ptr;
        mandolin_fifo_type  i,payload_bytes = MANDOLIN_BYTES_PER_WORD*(1 + (mandolin_fifo_type) bmp->length);

        // Allocate payload 
        if(NULL==(bmp->payload = (void *) malloc((size_t) payload_bytes))) {
            free(bmp);
            bmp = NULL;

            return false;
        }
        payload_ptr = (mandolin_byte_type *) bmp->payload;

        if(IS_MANDOLIN_NETWORK_ENDIAN(bmp->transport)) {
            // Read in payload + checksum one byte at a time, unscrambling network byte order into host byte order
            for(i=0; i<payload_bytes; i+=4) {
                payload_ptr[i+3] = MANDOLIN_FIFO_dequeue_byte(bfp);
                payload_ptr[i+2] = MANDOLIN_FIFO_dequeue_byte(bfp);
                payload_ptr[i+1] = MANDOLIN_FIFO_dequeue_byte(bfp);
                payload_ptr[i+0] = MANDOLIN_FIFO_dequeue_byte(bfp);
            }
        }
        else {
            // Read in payload + checksum in one go
            MANDOLIN_FIFO_dequeue_bytes(bfp,payload_ptr,payload_bytes);
        }
    }
    else
        bmp->payload = NULL;

    // Assign header
    *bmpp = bmp;

    return true;
}


// Allocate and return an empty MANDOLIN message with n payload words.
// Returns false on failure (of memory allocation), true otherwise.
bool MANDOLIN_MSG_alloc(mandolin_message **bmpp, mandolin_fifo_type n)
{
    mandolin_message *bmp = NULL;

    // Allocate header
    if(NULL==(bmp = (mandolin_message *) malloc(sizeof(mandolin_message))))
        return false;

    if(n) {
        // Allocate payload 
        if(NULL==(bmp->payload = (void *) malloc((size_t) MANDOLIN_BYTES_PER_WORD*(n+1)))) {
            free(bmp);
            bmp = NULL;

            return false;
        }
    }
    else
        bmp->payload = NULL;

    // Assign header
    *bmpp = bmp;

    return true;
}


// Destroy a MANDOLIN message, freeing any dynamically allocated memory.
void MANDOLIN_MSG_free(mandolin_message *bmp)
{
    if(NULL!=bmp) {
        // Free payload
        if(NULL!=bmp->payload) {
            free(bmp->payload);
            bmp->payload = NULL;
        }

        // Free header
        free(bmp);
        bmp = NULL;
    }
}
#endif


// Prepare a MANDOLIN message for transmission by adding a sync and computing any checksums.
// The length, transport flags, message ID, and payload (if any) should already be initialized.
// Word-length fields should be in host (little-endian) format for now.
// Note that the sequence number can be updated later using MANDOLIN_MSG_stuff_sequence(),
// and a placeholder value used now, if necessary.
void MANDOLIN_MSG_pack(mandolin_message *bmp, mandolin_byte_type sequence_number)
{
#if defined(__ADSP21000__)
    uint32 checksum16;
#else
    uint16 checksum16;
#endif

    // Pack sync
    bmp->sync = MANDOLIN_SYNC_WORD;

    // Insert sequence
    bmp->sequence = sequence_number;

    // Compute header checksum
    checksum16 =          bmp->sync      +          bmp->sequence +
                 SHORT_HI(bmp->length)   + SHORT_LO(bmp->length) +
                          bmp->transport +          bmp->id;

    // Pack header checksum
#if defined(__ADSP21000__)
    bmp->checksum = (~checksum16) & 0x0000FFFF;
#else
    bmp->checksum = ~checksum16;
#endif

    if(bmp->length) {
        mandolin_byte_type *payload_ptr = (mandolin_byte_type *) bmp->payload;

        mandolin_fifo_type  i;

        // Compute payload checksum - don't swap byte order
        uint32 checksum32 = 0;
        for(i=0; i<MANDOLIN_BYTES_PER_WORD*(mandolin_fifo_type) bmp->length; i++)
            checksum32 += payload_ptr[i];
        checksum32 = ~checksum32;

        // Pack payload checksum (in host order)
        payload_ptr[i+0] =  checksum32      & 0x000000FF;
        payload_ptr[i+1] = (checksum32>> 8) & 0x000000FF;
        payload_ptr[i+2] = (checksum32>>16) & 0x000000FF;
        payload_ptr[i+3] = (checksum32>>24) & 0x000000FF;
    }
}


// Stuff a sequence number into a packed MANDOLIN message, updating its header checksum.
void MANDOLIN_MSG_stuff_sequence(mandolin_message *bmp, mandolin_byte_type sequence_number)
{
    mandolin_fifo_type  i;
    mandolin_byte_type *bytes = (mandolin_byte_type *) bmp;

#if defined(__ADSP21000__)
    uint32 checksum16 = 0;
#else
    uint16 checksum16 = 0;
#endif

    // Insert sequence number
    bytes[MANDOLIN_BYTE_SEQUENCE] = sequence_number;

    // Recompute header checksum
    for(i=0; i<=MANDOLIN_BYTE_MSG_ID; i++)
        checksum16 += bytes[i];
    checksum16 = ~checksum16;

    // Insert new header checksum
    bytes[MANDOLIN_BYTE_CHECKSUM_HI] = (checksum16>>8) & 0x000000FF;
    bytes[MANDOLIN_BYTE_CHECKSUM_LO] =  checksum16     & 0x000000FF;
}


// Write a MANDOLIN message to the specified buffer.
// Returns the number of bytes written (which will always be all of the bytes specified in the message header).
mandolin_fifo_type MANDOLIN_MSG_write(mandolin_byte_type *buffer, mandolin_message *bmp)
{
    mandolin_fifo_type i;

    // Write header in network byte order
    *buffer++ =          bmp->sync;
    *buffer++ =          bmp->sequence;
    *buffer++ = SHORT_HI(bmp->length);
    *buffer++ = SHORT_LO(bmp->length);
    *buffer++ =          bmp->transport;
    *buffer++ =          bmp->id;
    *buffer++ = SHORT_HI(bmp->checksum);
    *buffer++ = SHORT_LO(bmp->checksum);

    if(bmp->length) {
        mandolin_byte_type *payload_ptr = (mandolin_byte_type *) bmp->payload;

#if NO_LEGACY_PACK_AND_WRITE
        if(IS_MANDOLIN_HOST_ENDIAN(bmp->transport)) {
            // Copy the payload + checksum, keeping host byte order
            memcpy(buffer,payload_ptr,MANDOLIN_BYTES_PER_WORD*(1 + (mandolin_fifo_type) bmp->length));
        }
        else {
#endif
            // Write payload + checksum, scrambling host byte order into network byte order
            for(i=0; i<MANDOLIN_BYTES_PER_WORD*(1 + (mandolin_fifo_type) bmp->length); i+=4) {
                *buffer++ = payload_ptr[i+3];
                *buffer++ = payload_ptr[i+2];
                *buffer++ = payload_ptr[i+1];
                *buffer++ = payload_ptr[i+0];
            }
#if NO_LEGACY_PACK_AND_WRITE
        }
#endif
    }

    return MANDOLIN_MSG_bytes(bmp);
}


// Write a MANDOLIN message to the specified FIFO.
// Returns false on failure, true otherwise.
bool MANDOLIN_MSG_enqueue(bool (*fifo_enqueue)(mandolin_byte_type), mandolin_message *bmp)
{
    mandolin_fifo_type i;

    // Enqueue header
    if(!fifo_enqueue(bmp->sync))               return false;
    if(!fifo_enqueue(bmp->sequence))           return false;
    if(!fifo_enqueue(SHORT_HI(bmp->length)))   return false;
    if(!fifo_enqueue(SHORT_LO(bmp->length)))   return false;
    if(!fifo_enqueue(bmp->transport))          return false;
    if(!fifo_enqueue(bmp->id))                 return false;
    if(!fifo_enqueue(SHORT_HI(bmp->checksum))) return false;
    if(!fifo_enqueue(SHORT_LO(bmp->checksum))) return false;

    if(bmp->length) {
        mandolin_byte_type *payload_ptr = (mandolin_byte_type *) bmp->payload;

#if NO_LEGACY_PACK_AND_WRITE
        if(IS_MANDOLIN_HOST_ENDIAN(bmp->transport)) {
            // Enqueue payload + checksum, keeping host byte order
            for(i=0; i<MANDOLIN_BYTES_PER_WORD*(1 + (mandolin_fifo_type) bmp->length); i+=4) {
                if(!fifo_enqueue(payload_ptr[i+0])) return false;
                if(!fifo_enqueue(payload_ptr[i+1])) return false;
                if(!fifo_enqueue(payload_ptr[i+2])) return false;
                if(!fifo_enqueue(payload_ptr[i+3])) return false;
            }
        }
        else {
#endif
            // Enqueue payload + checksum, scrambling host byte order into network byte order
            for(i=0; i<MANDOLIN_BYTES_PER_WORD*(1 + (mandolin_fifo_type) bmp->length); i+=4) {
                if(!fifo_enqueue(payload_ptr[i+3])) return false;
                if(!fifo_enqueue(payload_ptr[i+2])) return false;
                if(!fifo_enqueue(payload_ptr[i+1])) return false;
                if(!fifo_enqueue(payload_ptr[i+0])) return false;
            }
#if NO_LEGACY_PACK_AND_WRITE
        }
#endif
    }

    return true;
}
