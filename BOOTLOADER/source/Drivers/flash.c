/*
 * flash.c
 *
 *  Created on: April 19, 2023
 *      Author: Stuart Campbell
 */

#include "NuMicro.h"
#include "flash_params.h"
#include "crc16.h"	//we can change to the crc32 native to nuvoton?
#include "Upgrade.h"


// Define 'NO_DEBUG_MSG_OUTPUT' in 'flash_params.h' to control serial information messages (code size reduction)
#if NO_DEBUG_MSG_OUTPUT
	#define PutString(x)	// To save space, take away all serial information messages
#else
	extern void PutString(char *str);
#endif

extern void PutChar(char ch);


static uint32_t verifyFailCount = 0;
static uint32_t lastFlashAddr = 0;

//-----------------------------------------------------------------------------
// Program the specified block of memory to APROM Flash
// @param flashAddress  The actual APROM Flash address
// @param pu32Data      Holds the sector of data to be written to the APROM area
// @param numU32s       The number of U32s to write to APROM
//-----------------------------------------------------------------------------
bool flash_writeChunk( uint32_t flashAddress, uint32_t pu32Data[], uint32_t numU32s )
{
    if ( (flashAddress & 0xFFF) != 0 )
    {   // Not at a 4kB sector boundary
        PutString("Must be at 4kB boundary for Flash write\n");
        return false;
    }
    
    if ( FMC_Erase(flashAddress) != 0 )                /* Erase 4kB page */
    {
       	PutString("Flash Erase failed\n");
        return false;
    }  

    for ( volatile uint32_t addr = flashAddress, i = 0; i < numU32s; addr += 4, i++ )
    {
    	lastFlashAddr = addr;
        if ( FMC_Write(addr, pu32Data[i]) != 0 )       /* Program flash */
        {
        	PutString("Flash Write failed\n");
            return false;
        }

#if 1	// Found enough space for this Verifiction step to go in.
        // Could be removed if more space needed for further code (would then rely on CRC check at end)
		volatile uint32_t verification = FMC_Read(addr);
		if ( verification != pu32Data[i] )
		{
			verifyFailCount++;
			PutChar('V');
			PutString("Verify Failed\n");
		}
#endif
    }  
    
    return true;
}

