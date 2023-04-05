// CMWrap.c - A parser for the ConmonMandolinMWrap audio control protocol.
//              ASSUMES LITTLE-ENDIAN HOST BYTE ORDER!

#include "LOUD_types.h" // standard types - #define any compile switches either in the project options or above this line
#if 0
#  include "config.h"     // or whichever header #defines VERBOSE, SUCCINCT for the project
#else
#  ifndef VERBOSE
#    define VERBOSE  0
#  endif
#  define SUCCINCT 0
#endif

#include "CMWrap.h"

#if VERBOSE || SUCCINCT
#include <stdio.h>
#endif

#if DYNAMIC_MEMORY_ALLOCATION_AVAILABLE
#include <stdlib.h>     // needed for NULL, c/malloc(), free()
#else
#  ifndef NULL
#    define NULL 0
#  endif
#endif

#if ENABLE_CMWRAP_TIMEOUT
// External Time Support
#include "tick.h"
extern uint32 tick_count;


#define CMWRAP_MULTITHREAD_DEBUGTEST_ENABLED 1



#if(CMWRAP_MULTITHREAD_DEBUGTEST_ENABLED)

void CMWrap_MultiThreaad_DebugTestCheck(CMWrap_fifo *cmwfp)
{
	if (cmwfp->write_i >= cmwfp->buffer_bytes)
	   cmwfp->write_i = 0;
	if (cmwfp->read_i >= cmwfp->buffer_bytes)
	   cmwfp->read_i = 0;
}
#define CMWRAP_MULTITHREAD_DEBUGTEST(cmwfp)	 CMWrap_MultiThreaad_DebugTestCheck(cmwfp);

#else
#define CMWRAP_MULTITHREAD_DEBUGTEST(cmwfp)

#endif


//
// FIFO MANAGEMENT FUNCTIONS
//

uint32 CMWrap_FIFO_valid_stamp(CMWrap_fifo *cmwfp)
{
    return cmwfp->valid_header;   // timestamp applied when the last payload byte was processed, or 0 if no valid header
}
#endif


// Returns the maximum number of bytes that can be held in the FIFO.
// The current construction of the FIFO, which has no 'current' byte count,
// causes us to lose the use of one byte of the FIFO, but should gain read/write thread independence...
CMWrap_fifo_type CMWrap_FIFO_max_bytes(CMWrap_fifo *cmwfp)
{
    return cmwfp->buffer_bytes - 1;
}


// 'Thread safe' for reading.
bool32 CMWrap_FIFO_is_empty(CMWrap_fifo *cmwfp)
{
	CMWRAP_MULTITHREAD_DEBUGTEST(cmwfp)	

    return cmwfp->read_i==cmwfp->write_i;
}


// 'Thread safe' for writing.
bool32 CMWrap_FIFO_is_full(CMWrap_fifo *cmwfp)
{
	CMWRAP_MULTITHREAD_DEBUGTEST(cmwfp)	

    return (cmwfp->read_i)? cmwfp->read_i==cmwfp->write_i+1 : cmwfp->buffer_bytes==cmwfp->write_i+1;

    // r -> 0, but changes after test ==> OK, as false branch doesn't depend on r  (can get false full, which is OK)
    // r -> MFP-1, but wraps after test ==> OK, as r advancing can't cause a false !full
    // r -> MFP (due to interrupt of dequeue) ==> OK to put single byte in FIFO, as data has already been read
}


// 'Thread safe' for reading.
CMWrap_fifo_type CMWrap_FIFO_bytes_full(CMWrap_fifo *cmwfp)
{
    CMWrap_fifo_type bytes = cmwfp->write_i + cmwfp->buffer_bytes - cmwfp->read_i;  // latch indices

	CMWRAP_MULTITHREAD_DEBUGTEST(cmwfp)	

    
    return (bytes>=cmwfp->buffer_bytes)? bytes - cmwfp->buffer_bytes : bytes;       // test
}


// 'Thread safe' for writing.
CMWrap_fifo_type CMWrap_FIFO_bytes_empty(CMWrap_fifo *cmwfp)
{
    return cmwfp->buffer_bytes-1 - CMWrap_FIFO_bytes_full(cmwfp);
}


#if DEBUG_FIFO_BUFFER
// Read and reset the FIFO's high watermark.
CMWrap_fifo_type CMWrap_FIFO_high_watermark(CMWrap_fifo *cmwfp)
{
    CMWrap_fifo_type max_buffer_bytes = cmwfp->max_buffer_bytes;

    cmwfp->max_buffer_bytes = CMWrap_FIFO_bytes_full(cmwfp);

    return max_buffer_bytes;
}
#endif


// Initialize a FIFO.
// fifo_bufptr should be a pointer to an array of bytes of length fifo_bytes.
bool32 CMWrap_FIFO_init(CMWrap_fifo *cmwfp, CMWrap_fifo_type fifo_bytes, CMWrap_byte_type *fifo_bufptr)
{
    cmwfp->write_i      = 0;
    cmwfp->read_i       = 0;
    cmwfp->buffer_bytes = fifo_bytes;
    cmwfp->valid_header = 0;

#if DEBUG_FIFO_BUFFER
    cmwfp->max_buffer_bytes = 0;
#endif

#if CMWrap_FIFO_IS_POWER_OF_TWO
    if(fifo_bytes && !(fifo_bytes & fifo_bytes-1)) {
        // fifo_bytes is not a power of two!
        return false;
    }
    else
        cmwfp->buffer_mask = fifo_bytes - 1;
#endif

    cmwfp->buffer = fifo_bufptr;

    return true;
}


// Reset a FIFO to its initial state (without freeing or reinitializing the buffer).
void CMWrap_FIFO_reset(CMWrap_fifo *cmwfp)
{
    cmwfp->write_i      = 0;
    cmwfp->read_i       = 0;
    cmwfp->valid_header = 0;
}


#if DYNAMIC_MEMORY_ALLOCATION_AVAILABLE
// Initialize a FIFO.
// The FIFO buffer will be dynamically allocated.
bool32 CMWrap_FIFO_alloc(CMWrap_fifo *cmwfp, CMWrap_fifo_type fifo_bytes)
{
    cmwfp->write_i      = 0;
    cmwfp->read_i       = 0;
    cmwfp->buffer_bytes = fifo_bytes;
    cmwfp->valid_header = 0;

#if DEBUG_FIFO_BUFFER
    cmwfp->max_buffer_bytes = 0;
#endif

#if CMWrap_FIFO_IS_POWER_OF_TWO
    if(fifo_bytes && !(fifo_bytes & fifo_bytes-1)) {
        // fifo_bytes is not a power of two!
        return false;
    }
    else
        cmwfp->buffer_mask = fifo_bytes - 1;
#endif

    // Zero initialize the buffer
    if(NULL==(cmwfp->buffer = (CMWrap_byte_type *) calloc(fifo_bytes,sizeof(CMWrap_byte_type))))
        return false;

    return true;
}


// Free a FIFO.
void CMWrap_FIFO_free(CMWrap_fifo *cmwfp)
{
    if(NULL!=cmwfp->buffer) {
        free(cmwfp->buffer);
        cmwfp->buffer = NULL;
    }
}
#endif


// Add a byte to the FIFO.
// Doesn't check for buffer fullness!
// 'Thread safe' for writing.
void CMWrap_FIFO_enqueue_byte(CMWrap_fifo *cmwfp, CMWrap_byte_type byte)
{


	CMWRAP_MULTITHREAD_DEBUGTEST(cmwfp)	

    cmwfp->buffer[cmwfp->write_i++] = byte; // write
                                        // increment index
#if CMWrap_FIFO_IS_POWER_OF_TWO
    cmwfp->write_i &= cmwfp->buffer_mask;   // wrap index
#else
    if(cmwfp->buffer_bytes==cmwfp->write_i) // wrap index
        cmwfp->write_i = 0;
#endif

#if DEBUG_FIFO_BUFFER
	if(CMWrap_FIFO_bytes_full(cmwfp)>cmwfp->max_buffer_bytes)
        cmwfp->max_buffer_bytes = CMWrap_FIFO_bytes_full(cmwfp);
#endif
}


// Add an array of n bytes to the FIFO.
// Doesn't check for buffer fullness!
void CMWrap_FIFO_enqueue_bytes(CMWrap_fifo *cmwfp, CMWrap_byte_type *bytep, CMWrap_fifo_type n)
{
    CMWrap_fifo_type bytes_til_wrap = cmwfp->buffer_bytes - cmwfp->write_i;


	CMWRAP_MULTITHREAD_DEBUGTEST(cmwfp)

    if(n>=bytes_til_wrap) {
        n -= bytes_til_wrap;

        while(bytes_til_wrap--)
            cmwfp->buffer[cmwfp->write_i++] = *bytep++;

        cmwfp->write_i = 0;
    }

    while(n--)
        cmwfp->buffer[cmwfp->write_i++] = *bytep++;

#if DEBUG_FIFO_BUFFER
	if(CMWrap_FIFO_bytes_full(cmwfp)>cmwfp->max_buffer_bytes)
        cmwfp->max_buffer_bytes = CMWrap_FIFO_bytes_full(cmwfp);
#endif
}


// Remove and return a byte from the FIFO.
// Doesn't check for buffer emptiness!
// 'Thread safe' for reading.
CMWrap_byte_type CMWrap_FIFO_dequeue_byte(CMWrap_fifo *cmwfp)
{
    CMWrap_byte_type byte = cmwfp->buffer[cmwfp->read_i++];   // read

                                                             // increment index
#if CMWrap_FIFO_IS_POWER_OF_TWO
    cmwfp->read_i &= cmwfp->buffer_mask;                        // wrap index
#else
    if(cmwfp->buffer_bytes==cmwfp->read_i)                      // wrap index
        cmwfp->read_i = 0;
#endif

	CMWRAP_MULTITHREAD_DEBUGTEST(cmwfp)	

    return byte;
}


// Remove and return an array of n bytes from the FIFO.
// Caller should provide a buffer of length n.
// Returns the actual number of bytes dequeued.
// Note that all bytes will be in network (BIG-ENDIAN) order.
CMWrap_fifo_type CMWrap_FIFO_dequeue_bytes(CMWrap_fifo *cmwfp, CMWrap_byte_type *bytep, CMWrap_fifo_type n)
{
    CMWrap_fifo_type bytes_in_fifo    = CMWrap_FIFO_bytes_full(cmwfp);
    CMWrap_fifo_type bytes_to_dequeue = (n>bytes_in_fifo)? bytes_in_fifo : n;
    CMWrap_fifo_type bytes_til_wrap   = cmwfp->buffer_bytes - cmwfp->read_i;

	CMWRAP_MULTITHREAD_DEBUGTEST(cmwfp)	



    n = bytes_to_dequeue;

    if(n>=bytes_til_wrap) {
        n -= bytes_til_wrap;

        while(bytes_til_wrap--)
            *bytep++ = cmwfp->buffer[cmwfp->read_i++];

        cmwfp->read_i = 0;
    }

    while(n--)
        *bytep++ = cmwfp->buffer[cmwfp->read_i++];

    return bytes_to_dequeue;
}


// Return the value of the nth oldest FIFO bin.
// Return value is undefined if n > cmwfp->buffer_bytes.
CMWrap_byte_type CMWrap_FIFO_get_byte(CMWrap_fifo *cmwfp, CMWrap_fifo_type n)
{
    CMWrap_fifo_type byte_to_get = cmwfp->read_i + n;

	CMWRAP_MULTITHREAD_DEBUGTEST(cmwfp)	

    while(cmwfp->buffer_bytes<=byte_to_get)
        byte_to_get -= cmwfp->buffer_bytes;

    return cmwfp->buffer[byte_to_get];
}


// Remove up to the n oldest bytes from the FIFO.
// Returns the actual number of bytes dumped.
CMWrap_fifo_type CMWrap_FIFO_dump_bytes(CMWrap_fifo *cmwfp, CMWrap_fifo_type n)
{
    CMWrap_fifo_type bytes_in_fifo  = CMWrap_FIFO_bytes_full(cmwfp);
    CMWrap_fifo_type bytes_to_erase = (n>bytes_in_fifo)? bytes_in_fifo : n;

    cmwfp->read_i += bytes_to_erase;

    while(cmwfp->buffer_bytes<=cmwfp->read_i)
        cmwfp->read_i -= cmwfp->buffer_bytes;

    return bytes_to_erase;
}



// --------------------------------------------------------------------------------------------------
//
// PARSER
//

// Checks the FIFO for a valid CMWrap message.
// Returns the length in words of the entire message if a valid message is detected (the message will be located at the tail of the FIFO),
//         0 if no valid message is detected or on error (timeout or checksum mismatch).
CMWrap_fifo_type CMWrap_FIFO_parse(CMWrap_fifo *cmwfp, CMWrap_parse_error *error)
{
    uint8 sync_word;
    CMWrap_fifo_type message_length_bytes;

    CMWrap_fifo_type i;
	uint16 payload_length;
	uint32	header_checksum;
    uint32  checksum, payload_checksum;
#if ENABLE_CMWRAP_TIMEOUT
	uint32	ticks_elapsed;
#endif
	

	int bytes = CMWrap_FIFO_bytes_full(cmwfp);

	if (bytes<0)
	{
		//printf("Error on bytes full, negative amount %d\n", bytes);
		return CMWRAP_ERROR;
	}
    // Clear any error condition
    error->error = CMWRAP_NO_ERROR;

	sync_word = CMWRAP_SYNC_WORD & 0x0ff;

		// -----------------------------------
		// Test timeout	on the partial old message
		// -----------------------------------
#if ENABLE_CMWRAP_TIMEOUT
		if (cmwfp->valid_header) 
		{
            // Check watchdog
            ticks_elapsed = TICKS_ELAPSED(tick_count,cmwfp->rx_time);

            if(ticks_elapsed>CMWRAP_TIMEOUT) 
			{
                // Log timeout
                error->error     = CMWRAP_ERROR_TIMEOUT;
                error->transport = CMWrap_FIFO_get_byte(cmwfp,CMWRAP_BYTE_TRANSPORT);
#if SUCCINCT
                fprintf(stdout,"Error #%d occurred while parsing!\n",CMWRAP_ERROR_TIMEOUT);
#endif
                // Discard sync
                CMWrap_FIFO_dump_bytes(cmwfp,1);
				bytes -=1;

                // Invalidate header
                cmwfp->valid_header = 0;
				return 0;
                // Iterate
                //return -1; continue
            }
        }
#endif


	
    while(bytes>=CMWRAP_HEADER_BYTES)
	{

		// -----------------------------------
		// parse header
		// -----------------------------------
		if (!cmwfp->valid_header)
		{
            while (bytes>=1) //CMWRAP_HEADER_BYTES)
			{
				if ((CMWrap_FIFO_get_byte(cmwfp,0) & 0x0ff) == sync_word)
					break;

                CMWrap_FIFO_dequeue_byte(cmwfp);
				bytes--;

			}
			if(bytes<CMWRAP_HEADER_BYTES)			// not enough bytes
				 return 0;

            // Compute checksum of header
            checksum = 0;
            for(i=0; i<CMWRAP_HEADER_CHECKSUM_BYTES; i++)
                checksum += (uint16) (CMWrap_FIFO_get_byte(cmwfp,i)&0x0ff);
            checksum = (~checksum) & 0x0000ffff;

            // Assemble header checksum
            header_checksum = PACK_SHORT(CMWrap_FIFO_get_byte(cmwfp,CMWRAP_BYTE_HEADERCHECKSUM_HI),
            		CMWrap_FIFO_get_byte(cmwfp,CMWRAP_BYTE_HEADERCHECKSUM_LO)) & 0x0ffff;

            // Verify header checksum
            if(checksum!= header_checksum ) 
			{
	#if SUCCINCT
					fprintf(stdout,"ERROR: Mandolin Error #%d (mismatch header checksum %X != %X) occurred while parsing!\n",
							CMWRAP_ERROR_CHECKSUM_MISMATCH, checksum, header_checksum);
	#endif
			    //error->id = 	CMWRAP_MSG_INVALID;
			    //error->sequence = 	0;
			    //error->error = 	CMWRAP_ERROR_CHECKSUM_MISMATCH;
                // Discard sync
                CMWrap_FIFO_dump_bytes(cmwfp,1);
				bytes -= 1;
			}
			else
			{

		        cmwfp->valid_header = 1;
#if ENABLE_CMWRAP_TIMEOUT
				cmwfp->rx_time = tick_count;
#endif
			}

		}
	
		// -----------------------------------
		// parse the payload
		// -----------------------------------
		if (cmwfp->valid_header)
		{
			payload_length = PACK_SHORT(CMWrap_FIFO_get_byte(cmwfp,CMWRAP_BYTE_LENGTH_HI),CMWrap_FIFO_get_byte(cmwfp,CMWRAP_BYTE_LENGTH_LO));

			if (payload_length == 0)
			{
				cmwfp->valid_header = 0;		// changet the state back
				return 	CMWRAP_HEADER_WORDS;
			}
			else
			{

				message_length_bytes =  CMWRAP_HEADER_BYTES + CMWRAP_BYTES_PER_WORD*((CMWrap_fifo_type) payload_length);
				if (bytes< message_length_bytes) // not enougth 
						return 0;	


				// Compute checksum of payload
				checksum = 0;
				for(i=CMWRAP_HEADER_BYTES; i<message_length_bytes; i++)
					checksum += (uint32) CMWrap_FIFO_get_byte(cmwfp,i);
				checksum = (~checksum) & 0x0ffff;

				            // Assemble header checksum
	            payload_checksum = PACK_SHORT(CMWrap_FIFO_get_byte(cmwfp,CMWRAP_BYTE_PAYLOADCHECKSUM_HI),
		        		CMWrap_FIFO_get_byte(cmwfp,CMWRAP_BYTE_PAYLOADCHECKSUM_LO)) & 0x0ffff;

				// Verify payload checksum
				if(checksum==payload_checksum) {
					cmwfp->valid_header = 0;		// changet the state back
					return (CMWRAP_HEADER_WORDS + ((CMWrap_fifo_type) payload_length));
				}
				else
				{
					// Log mismatch
					error->error     = CMWRAP_ERROR_CHECKSUM_MISMATCH;
					error->transport = CMWrap_FIFO_get_byte(cmwfp,CMWRAP_BYTE_TRANSPORT);
	#if SUCCINCT
					fprintf(stdout,"Error: Mandolin Error #%d (mismatch checksum %X != %X) occurred while parsing!\n",
							CMWRAP_ERROR_CHECKSUM_MISMATCH, checksum, payload_checksum);
	#endif
					// Discard sync
					CMWrap_FIFO_dump_bytes(cmwfp,1);
					bytes--;

					// Invalidate header
					cmwfp->valid_header = 0;
					return 0;	// error

				}
			}

		}
    }
    
    return 0;
}



//
// MESSAGE MANAGEMENT FUNCTIONS
//

// Return the length of a CMWrap message in bytes.
CMWrap_fifo_type CMWrap_MSG_bytes(CMWrap_message *bmp)
{
    return (CMWRAP_BYTES_PER_WORD*(CMWRAP_HEADER_WORDS + (bmp->length)));
}


// Erase a valid CMWrap message from the tail of the FIFO.
// NOTE:  This function is mildly dangerous - if there is no valid message, the results of the call are unpredictable!
void CMWrap_MSG_erase(CMWrap_fifo *cmwfp)
{
    // Read in message length
    uint16 payload_length = PACK_SHORT(CMWrap_FIFO_get_byte(cmwfp,CMWRAP_BYTE_LENGTH_HI),CMWrap_FIFO_get_byte(cmwfp,CMWRAP_BYTE_LENGTH_LO));

    // Erase entire message
    CMWrap_FIFO_dump_bytes(cmwfp,CMWRAP_HEADER_BYTES + CMWRAP_BYTES_PER_WORD*(CMWrap_fifo_type) payload_length);
}


CMWrap_fifo_type CMWrap_MSG_readbuffer(CMWrap_byte_type **pBuffer, uint16* nLength, CMWrap_message* bmp, CMWrap_byte_type* pMsgPayload)
{
	CMWrap_byte_type byte;
    CMWrap_byte_type byte2;
    CMWrap_byte_type byte3;
    CMWrap_byte_type byte4;
	uint32 checksum;

	CMWrap_byte_type *buffer;

	if (*nLength == 0)
	{
		return 0;
	}
	buffer = *pBuffer;

    // Assemble length
    byte = *buffer++;
	*nLength = *nLength-1;
	while (((byte & 0x0ff) != (CMWRAP_SYNC_WORD & 0x0ff)) && (nLength > 0))
	{
	    byte = *buffer++;
		*nLength = *nLength-1;;

	}
	bmp->sync = byte;
	if (*nLength < 1) return 0;

  	// Read transport flags
    bmp->transport = *buffer++;
	*nLength = *nLength-1;

	byte = *buffer++;
	*nLength = *nLength-1;
    bmp->length = PACK_SHORT(byte,*buffer++);
	*nLength = *nLength-1;

	if (( bmp->length*CMWRAP_BYTES_PER_WORD+12) > *nLength)
		return 0;

	// read source id
	byte = *buffer++;
	byte2 = *buffer++;
	byte3 = *buffer++;
	byte4 = *buffer++;
	bmp->source_id_hi = PACK_LONG(byte, byte2, byte3, byte4);

	byte =*buffer++;
	byte2 = *buffer++;
	byte3 = *buffer++;
	byte4 = *buffer++;
	bmp->source_id_lo = PACK_LONG(byte, byte2, byte3, byte4);

    // read checksums
    byte = *buffer++;
    bmp->payload_checksum = PACK_SHORT(byte,*buffer++);
    byte = *buffer++;
    bmp->header_checksum = PACK_SHORT(byte,*buffer++);
	*nLength = *nLength-12;

	// verify header checksum
	checksum = bmp->sync + bmp->transport + (bmp->length & 0x0ff) + ((bmp->length >> 8) & 0x0ff) +
			(bmp->source_id_hi & 0x0ff) + ((bmp->source_id_hi>>8) & 0x0ff) + ((bmp->source_id_hi>>16) & 0x0ff) + ((bmp->source_id_hi>>24) & 0x0ff) +
			(bmp->source_id_lo & 0x0ff) + ((bmp->source_id_lo>>8) & 0x0ff) + ((bmp->source_id_lo>>16) & 0x0ff) + ((bmp->source_id_lo>>24) & 0x0ff) +
			 (bmp->payload_checksum & 0x0ff) + ((bmp->payload_checksum >> 8) & 0x0ff);
	checksum = (~checksum) & 0x0ffff;
	if (checksum !=  (bmp->header_checksum & 0x0ffff))
		return 0;

    if(bmp->length) {
        CMWrap_fifo_type i;
		CMWrap_fifo_type payload_bytes = CMWRAP_BYTES_PER_WORD*((CMWrap_fifo_type) bmp->length);

        // Assign payload
        bmp->payload = pMsgPayload;

        // Read in payload + checksum one byte at a time, unscrambling network byte order into host byte order
		checksum = 0;
        for(i=0; i<payload_bytes; i+=4) {
            pMsgPayload[i] = *buffer++;
            pMsgPayload[i+1] = *buffer++;
            pMsgPayload[i+2] = *buffer++;
            pMsgPayload[i+3] = *buffer++;
			checksum += pMsgPayload[i]+pMsgPayload[i+1]+pMsgPayload[i+2]+pMsgPayload[i+3];
        }
		*nLength = *nLength-payload_bytes;
		checksum = (~checksum) & 0x0ffff;
		// need to check the payload?
		if (checksum !=  (bmp->payload_checksum & 0x0ffff))
			return 0;

    }
    else
        bmp->payload = NULL;

	return CMWrap_MSG_bytes(bmp);

}

// Read a valid CMWrap message from the tail of the FIFO and into its own structure.
// Caller should provide both a message (header) structure and a (payload) buffer of appropriate length.
// Returns the actual number of bytes dequeued.
// NOTE:  This function is mildly dangerous - if there is no valid message, the results of the call are unpredictable!
CMWrap_fifo_type CMWrap_MSG_read(CMWrap_fifo *cmwfp, CMWrap_message *bmp, CMWrap_byte_type *payload_ptr)
{
    CMWrap_byte_type byte;
    CMWrap_byte_type byte2;
    CMWrap_byte_type byte3;
    CMWrap_byte_type byte4;

    // Read in header one byte at a time

    // Read sync
    bmp->sync = CMWrap_FIFO_dequeue_byte(cmwfp);

	// Read transport flags
    bmp->transport = CMWrap_FIFO_dequeue_byte(cmwfp);

    // Assemble length
    byte = CMWrap_FIFO_dequeue_byte(cmwfp);
    bmp->length = PACK_SHORT(byte,CMWrap_FIFO_dequeue_byte(cmwfp));

	// read source id
	byte = CMWrap_FIFO_dequeue_byte(cmwfp);
	byte2 = CMWrap_FIFO_dequeue_byte(cmwfp);
	byte3 = CMWrap_FIFO_dequeue_byte(cmwfp);
	byte4 = CMWrap_FIFO_dequeue_byte(cmwfp);
	bmp->source_id_hi = PACK_LONG(byte, byte2, byte3, byte4);

	byte = CMWrap_FIFO_dequeue_byte(cmwfp);
	byte2 = CMWrap_FIFO_dequeue_byte(cmwfp);
	byte3 = CMWrap_FIFO_dequeue_byte(cmwfp);
	byte4 = CMWrap_FIFO_dequeue_byte(cmwfp);
	bmp->source_id_lo = PACK_LONG(byte, byte2, byte3, byte4);


    // read checksums
    byte = CMWrap_FIFO_dequeue_byte(cmwfp);
    bmp->payload_checksum = PACK_SHORT(byte,CMWrap_FIFO_dequeue_byte(cmwfp));
    byte = CMWrap_FIFO_dequeue_byte(cmwfp);
    bmp->header_checksum = PACK_SHORT(byte,CMWrap_FIFO_dequeue_byte(cmwfp));

    if(bmp->length) {
        CMWrap_fifo_type i;
		CMWrap_fifo_type payload_bytes = CMWRAP_BYTES_PER_WORD*((CMWrap_fifo_type) bmp->length);

        // Assign payload
        bmp->payload = payload_ptr;

        // Read in payload + checksum one byte at a time, unscrambling network byte order into host byte order
        for(i=0; i<payload_bytes; i+=4) {
            payload_ptr[i] = CMWrap_FIFO_dequeue_byte(cmwfp);
            payload_ptr[i+1] = CMWrap_FIFO_dequeue_byte(cmwfp);
            payload_ptr[i+2] = CMWrap_FIFO_dequeue_byte(cmwfp);
            payload_ptr[i+3] = CMWrap_FIFO_dequeue_byte(cmwfp);
        }
    }
    else
        bmp->payload = NULL;

    return CMWrap_MSG_bytes(bmp);
}


#if DYNAMIC_MEMORY_ALLOCATION_AVAILABLE
// Read a valid CMWrap message from the tail of the FIFO and into its own structure.
// Dynamically allocates memory for the message.
// Returns false on failure (of memory allocation), true otherwise.
bool32 CMWrap_MSG_copy(CMWrap_fifo *cmwfp, CMWrap_message **bmpp)
{
    CMWrap_byte_type byte;

    CMWrap_message *bmp = NULL;

    // Allocate header
    if(NULL==(bmp = (CMWrap_message *) malloc(sizeof(CMWrap_message))))
        return false;

    // Read in header one byte at a time

    // Read sync
    bmp->sync = CMWrap_FIFO_dequeue_byte(cmwfp);

    // Read sequence
    bmp->sequence = CMWrap_FIFO_dequeue_byte(cmwfp);

    // Assemble length
    byte = CMWrap_FIFO_dequeue_byte(cmwfp);
    bmp->length = PACK_SHORT(byte,CMWrap_FIFO_dequeue_byte(cmwfp));

    // Read transport flags
    bmp->transport = CMWrap_FIFO_dequeue_byte(cmwfp);

    // Read message ID
    bmp->id = CMWrap_FIFO_dequeue_byte(cmwfp);

    // Assemble header checksum
    byte = CMWrap_FIFO_dequeue_byte(cmwfp);
    bmp->checksum = PACK_SHORT(byte,CMWrap_FIFO_dequeue_byte(cmwfp));

    if(bmp->length) {
        CMWrap_byte_type *payload_ptr;
        CMWrap_fifo_type  i,payload_bytes = CMWRAP_BYTES_PER_WORD*(1 + (CMWrap_fifo_type) bmp->length);

        // Allocate payload 
        if(NULL==(bmp->payload = (void *) malloc((size_t) payload_bytes))) {
            free(bmp);
            bmp = NULL;

            return false;
        }
        payload_ptr = (CMWrap_byte_type *) bmp->payload;

#if 1
        // Read in payload + checksum one byte at a time, unscrambling network byte order into host byte order
        for(i=0; i<payload_bytes; i+=4) {
            payload_ptr[i+3] = CMWrap_FIFO_dequeue_byte(cmwfp);
            payload_ptr[i+2] = CMWrap_FIFO_dequeue_byte(cmwfp);
            payload_ptr[i+1] = CMWrap_FIFO_dequeue_byte(cmwfp);
            payload_ptr[i+0] = CMWrap_FIFO_dequeue_byte(cmwfp);
        }
#else
        // Read in payload + checksum in one go
        CMWrap_FIFO_dequeue_bytes(cmwfp,payload_ptr,payload_bytes);
#endif
    }
    else
        bmp->payload = NULL;

    // Assign header
    *bmpp = bmp;

    return true;
}


// Allocate and return an empty CMWrap message with n payload words.
// Returns false on failure (of memory allocation), true otherwise.
bool32 CMWrap_MSG_alloc(CMWrap_message **bmpp, CMWrap_fifo_type n)
{
    CMWrap_message *bmp = NULL;

    // Allocate header
    if(NULL==(bmp = (CMWrap_message *) malloc(sizeof(CMWrap_message))))
        return false;

    if(n) {
        // Allocate payload 
        if(NULL==(bmp->payload = (void *) malloc((size_t) CMWRAP_BYTES_PER_WORD*(n+1)))) {
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


// Destroy a CMWrap message, freeing any dynamically allocated memory.
void CMWrap_MSG_free(CMWrap_message *bmp)
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


// Prepare a CMWrap message for transmission by adding a sync and computing any checksums.
// The length, transport flags, message ID, and payload (if any) should already be initialized.
// Word-length fields should be in host (little-endian) format for now.
void CMWrap_MSG_pack(CMWrap_message *bmp)
{
    uint16 checksum16;
	CMWrap_byte_type *payload_ptr;
    CMWrap_fifo_type  i;


    // Pack sync
    bmp->sync = CMWRAP_SYNC_WORD;

	checksum16 = 0;
    if(bmp->length) 
	{
        payload_ptr = (CMWrap_byte_type *) bmp->payload;
        // Compute payload checksum
        for(i=0; i<CMWRAP_BYTES_PER_WORD*(CMWrap_fifo_type) bmp->length; i++)
            checksum16 += payload_ptr[i];
    }
    bmp->payload_checksum = (~checksum16) & 0x0000FFFF;



   // Compute header checksum
    checksum16 =          bmp->sync      + 
						  bmp->transport +
						  ((bmp->length >>  8) & 0x0ff) + 
						  ((bmp->length >>  0) & 0x0ff) + 
						  ((bmp->source_id_hi >> 24) & 0x0ff) + 
 						  ((bmp->source_id_hi >> 16) & 0x0ff) + 
						  ((bmp->source_id_hi >>  8) & 0x0ff) + 
						  ((bmp->source_id_hi >>  0) & 0x0ff) + 
						  ((bmp->source_id_lo >> 24) & 0x0ff) + 
 						  ((bmp->source_id_lo >> 16) & 0x0ff) + 
						  ((bmp->source_id_lo >>  8) & 0x0ff) + 
						  ((bmp->source_id_lo >>  0) & 0x0ff) + 
   						  ((bmp->payload_checksum >>  8) & 0x0ff) + 
						  ((bmp->payload_checksum >>  0) & 0x0ff); 
    bmp->header_checksum = (~checksum16) & 0x0000FFFF;

}


// Write a CMWrap message to the specified buffer.
// Returns the number of bytes written (which will always be all of the bytes specified in the message header).
CMWrap_fifo_type CMWrap_MSG_write(CMWrap_byte_type *buffer, CMWrap_message *bmp)
{
    CMWrap_fifo_type i;

    // Write header in network byte order
    *buffer++ =          bmp->sync;
    *buffer++ =          bmp->transport;
    *buffer++ = SHORT_HI(bmp->length);
    *buffer++ = SHORT_LO(bmp->length);

	*buffer++ = (bmp->source_id_hi >> 24) & 0x0ff;
	*buffer++ = (bmp->source_id_hi >> 16) & 0x0ff;
	*buffer++ = (bmp->source_id_hi >>  8) & 0x0ff;
	*buffer++ = (bmp->source_id_hi >>  0) & 0x0ff;
	*buffer++ = (bmp->source_id_lo >> 24) & 0x0ff;
	*buffer++ = (bmp->source_id_lo >> 16) & 0x0ff;
	*buffer++ = (bmp->source_id_lo >>  8) & 0x0ff;
	*buffer++ = (bmp->source_id_lo >>  0) & 0x0ff;
	*buffer++ = (bmp->payload_checksum >>  8) & 0x0ff;
	*buffer++ = (bmp->payload_checksum >>  0) & 0x0ff;
	*buffer++ = (bmp->header_checksum >>  8) & 0x0ff;
	*buffer++ = (bmp->header_checksum >>  0) & 0x0ff;

    if(bmp->length) {
        CMWrap_byte_type *payload_ptr = (CMWrap_byte_type *) bmp->payload;

        // Write payload + checksum, scrambling host byte order into network byte order
        for(i=0; i<CMWRAP_BYTES_PER_WORD*((CMWrap_fifo_type) bmp->length); i+=4) {
            *buffer++ = payload_ptr[i+0];
            *buffer++ = payload_ptr[i+1];
            *buffer++ = payload_ptr[i+2];
            *buffer++ = payload_ptr[i+3];
        }
    }

    return CMWrap_MSG_bytes(bmp);
}


// Write a CMWrap message to the specified FIFO.
// Returns false on failure, true otherwise.
bool32 CMWrap_MSG_enqueue(bool32 (*fifo_enqueue)(CMWrap_byte_type), CMWrap_message *bmp)
{
    CMWrap_fifo_type i;

    if(!fifo_enqueue( bmp->sync))						return false;
    if(!fifo_enqueue( bmp->transport))					return false;
    if(!fifo_enqueue( SHORT_HI(bmp->length)))			return false;
    if(!fifo_enqueue( SHORT_LO(bmp->length)))			return false;

	if(!fifo_enqueue( (bmp->source_id_hi >> 24) & 0x0ff)) return false;
	if(!fifo_enqueue( (bmp->source_id_hi >> 16) & 0x0ff)) return false;
	if(!fifo_enqueue( (bmp->source_id_hi >>  8) & 0x0ff)) return false;
	if(!fifo_enqueue( (bmp->source_id_hi >>  0) & 0x0ff)) return false;
	if(!fifo_enqueue( (bmp->source_id_lo >> 24) & 0x0ff)) return false;
	if(!fifo_enqueue( (bmp->source_id_lo >> 16) & 0x0ff)) return false;
	if(!fifo_enqueue( (bmp->source_id_lo >>  8) & 0x0ff)) return false;
	if(!fifo_enqueue( (bmp->source_id_lo >>  0) & 0x0ff)) return false;
	if(!fifo_enqueue( (bmp->payload_checksum >>  8) & 0x0ff)) return false;
	if(!fifo_enqueue( (bmp->payload_checksum >>  0) & 0x0ff)) return false;


    if(bmp->length) {
        CMWrap_byte_type *payload_ptr = (CMWrap_byte_type *) bmp->payload;

        // Write payload + checksum, scrambling host byte order into network byte order
        for(i=0; i<CMWRAP_BYTES_PER_WORD*(1 + (CMWrap_fifo_type) bmp->length); i+=4) {
            if(!fifo_enqueue(payload_ptr[i+0])) return false;
            if(!fifo_enqueue(payload_ptr[i+1])) return false;
            if(!fifo_enqueue(payload_ptr[i+2])) return false;
            if(!fifo_enqueue(payload_ptr[i+3])) return false;
        }
    }

    return true;
}


// Print a CMWrap message to the standard output.
void CMWrap_MSG_print(CMWrap_message *bmp)
{
#if VERBOSE
    CMWrap_fifo_type i;

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

    // Print payload (in host byte order)
    fprintf(stdout,"\npayload:  ");
    if(bmp->length) {
        CMWrap_byte_type *payload_ptr = (CMWrap_byte_type *) bmp->payload;

        for(i=0; i<CMWRAP_BYTES_PER_WORD*(1 + (CMWrap_fifo_type) bmp->length); i++)
            fprintf(stdout,"0x%02x ",payload_ptr[i]);
    }
    fprintf(stdout,"\n");
#endif
}
