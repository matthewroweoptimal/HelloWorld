/*
 * spi_flash_nu.c
 *
 *  Created on: April 19, 2023
 *      Author: Stuart Campbell
 */
#include <stdio.h>
#include "flash_params.h"
#include "spim.h"
#include "spi_flash_nu.h"


extern void PutString(char *str);



//-----------------------------------------------------------------------------
// Initialise the SPI Flash
//-----------------------------------------------------------------------------
spi_flash_status_t spi_flash_init(void)
{
    uint32_t result = SPIM_InitFlash(FLASH_WRITE_CLEAR_PROTECT);
    if ( result != SPIM_INITFLASH_SUCCESS )
    {
    	PutString("SPI Flash fail\n");
    	return SPI_FLASH_DRIVER_ERROR;
    }

    SPIM_WinbondUnlock(1);
	return SPI_FLASH_LOAD_OK;
}

//-----------------------------------------------------------------------------
// Read Region Header from Start of SPI Flash Firmware area into provided buffer
// @param pHeader   Storage for the OLY_REGION header which is read
//-----------------------------------------------------------------------------
void spi_flash_readFwHeader( OLY_REGION* pHeader )
{
	SPIM_IO_Read( SPI_FLASH_FW_BASE, USE_4_BYTE_ADDR_MODE, sizeof(OLY_REGION), (uint8_t*)pHeader, OPCODE_FAST_READ, 1, 1, 1, 1);
}

//-----------------------------------------------------------------------------
// Read Chunk from actual firmware area in SPI flash
// @param uiFlashOffset     This is the byte offset with the actual APP image (i.e. after the OLY_REGION header)
//-----------------------------------------------------------------------------
void spi_flash_readFwSector( uint32_t uiFlashOffset, uint32_t* pSector )
{
    uint32_t uiFlashFwAddress = SPI_FLASH_FW_BASE + sizeof(OLY_REGION) + uiFlashOffset; // Actual FW SPI Flash address
	SPIM_IO_Read( uiFlashFwAddress, USE_4_BYTE_ADDR_MODE, P_SECTOR_SIZE, (uint8_t*)pSector, OPCODE_FAST_READ, 1, 1, 1, 1);
}

//-----------------------------------------------------------------------------
// Read a single byte from actual firmware area in SPI flash
// @param uiFlashOffset     This is the byte offset from SPI_FLASH_FW_BASE
//-----------------------------------------------------------------------------
uint8_t spi_flash_read_fw_byte( uint32_t uiFlashOffset )
{	// Firmware content is after the OLY_BLOCK header.
	uint32_t uiFlashFwAddress = SPI_FLASH_FW_BASE + uiFlashOffset; // Actual FW SPI Flash address
	uint8_t flashContent;
	
	SPIM_IO_Read( uiFlashFwAddress, USE_4_BYTE_ADDR_MODE, 1, &flashContent, OPCODE_FAST_READ, 1, 1, 1, 1);
    return flashContent;
}

