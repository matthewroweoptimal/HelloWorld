/*
 * spi_flash.c
 *
 *  Created on: May 2, 2016
 *      Author: Ben.Huber - Adapted by Iain Quarmby for Nuvoton
 */

#include <stdio.h>
//#include "oly.h"
#include "flash_params.h"
#include "spim.h"
//#include "spi_settings.h"
#include "spi_flash_nu.h"
//#include "mqx.h"
#include "crc16.h"
//#include "spi_aux.h"
#include <string.h>     // needed for memcpy()

#define USE_DATA_FLASH     			0   // Not using data flash for params

#define FLASH_BLOCK_SIZE            (64*1024)    /* Flash block size. Depend on the physical flash. */
#define TEST_BLOCK_ADDR             0      /* Test block address on SPI flash. */
#define BUFFER_SIZE                 8
#define USE_4_BYTES_MODE            0            /* W25Q20 does not support 4-bytes address mode. */
uint8_t  g_buff[BUFFER_SIZE] __attribute__((aligned(4)));


/*FUNCTION*---------------------------------------------------------------
*
* Function Name : memory_check_for_flash_presence
* Comments  : This function checks if a spi flash is present
* Return:
*         Status of SPI transfer
*
*END*----------------------------------------------------------------------*/
uint32_t memory_check_for_flash_presence ()
{
	uint32_t result;

    result = SPIM_InitFlash(FLASH_WRITE_CLEAR_PROTECT);
    if(result != SPIM_INITFLASH_SUCCESS)
    	return 0;
    else
    	return 1;
}

spi_flash_status_t spi_flash_init(oly_flash_params_t * olyStoredParams)
{
	uint32_t i, result;

	uint32_t flashFileAddress;
	uint32_t latestFlashFileIndex = 0;
	uint32_t latestFlashFileAge = 0;
	uint32_t tempflashFileAge;
    uint8_t  idBuf[3];
    //dspi_status_t spiStatus;
    spi_flash_status_t ret;

    //fs - spiStatus = memory_set_protection (FSL_SPI_AUX, 0);

    result = SPIM_InitFlash(FLASH_WRITE_CLEAR_PROTECT);
    if(result != SPIM_INITFLASH_SUCCESS)
    	printf("SPIM_InitFlash failed return code: %i\n", result);

    SPIM_ReadJedecId(idBuf, sizeof (idBuf), 1);

    printf("SPIM get JEDEC ID=0x%02X, 0x%02X, 0x%02X\n", idBuf[0], idBuf[1], idBuf[2]);

    SPIM_WinbondUnlock(1);

	printf("SPiFlash check slot ");
	//	Look for the most recent save//
	for( i=0; i<SPI_NUM_MEMORY_SLOTS; i++)	{
		printf("%i", i+1);
		flashFileAddress = SPI_FLASH_FS_BASE+SPI_FLASH_SECTOR_SIZE*i;
		tempflashFileAge = spi_flash_check_for_valid_data(i, PARAMS_WRITE_SIZE, olyStoredParams);
		if(tempflashFileAge != 0) {
			if(latestFlashFileAge < tempflashFileAge) {
				latestFlashFileAge = tempflashFileAge;
				latestFlashFileIndex = i;
				flashFileAge =  latestFlashFileAge;
				printf("* ");
			}
			else {
				printf("o ");
			}
		}
		else {
			printf("x ");
		}
	}
	//point to next free slot - to be erased later
	if(flashFileAge != 0) {
		printf("latest in slot %i age %i\n ", latestFlashFileIndex+1, flashFileAge);

		emptyMemorySlotIndex = latestFlashFileIndex + 1;
		if(emptyMemorySlotIndex >= SPI_NUM_MEMORY_SLOTS) emptyMemorySlotIndex = 0;
		//now reload data from latest
		spi_flash_check_for_valid_data(latestFlashFileIndex, PARAMS_WRITE_SIZE, olyStoredParams);
		ret = SPI_FLASH_LOAD_OK;
	}
	else {
		emptyMemorySlotIndex = 0; //put at start for save
		printf("  Spi Memory CRC Error\n");
		spi_flash_param_reinit();
		ret = SPI_FLASH_LOAD_FAIL;
	}
	return ret;
}

uint32_t spi_flash_check_for_valid_data(uint32_t page_num, uint32 size, oly_flash_params_t * olyStoredParams)
{
	uint32_t nBytes, ret;
	crc16_data_t flash_Crc_Desc;
	uint16_t crcBlock= 0;
	uint32_t header_data[2];
	uint16 crc_in_flash;
    //dspi_status_t spiStatus;
    uint8_t* dest;
    uint32_t bytes_left_count;

	//printf("started check len= %08x add = %08x  sig = %08x  age = %08x\n", size, startAddress, *p_flashSig, *p_flashAge);
	uint32_t work_address = SPI_FLASH_FS_BASE+SPI_FLASH_SECTOR_SIZE*page_num;
	//spiStatus = memory_read_data(FSL_SPI_AUX, work_address, SPI_HEADER_BLOCK_SIZE, (uint8_t*)&header_data, &nBytes);
	SPIM_IO_Read(work_address,USE_4_BYTE_ADDR_MODE, SPI_HEADER_BLOCK_SIZE, (uint8_t*)&header_data, OPCODE_FAST_READ, 1,1,1,1);
	work_address = work_address + 8;
	if(header_data[0] == SPI_HEADER_SIGNATURE){
		dest = (uint8_t*)olyStoredParams;
		bytes_left_count = PARAMS_WRITE_SIZE;
		while(1){
			//spiStatus = memory_read_data(FSL_SPI_AUX, work_address, SPI_MEMORY_PAGE_SIZE, dest, &nBytes);
			SPIM_IO_Read(work_address,USE_4_BYTE_ADDR_MODE, SPI_MEMORY_PAGE_SIZE, dest, OPCODE_FAST_READ, 1,1,1,1);
			work_address = work_address + SPI_MEMORY_PAGE_SIZE;
			dest = dest + SPI_MEMORY_PAGE_SIZE;
			bytes_left_count = bytes_left_count - SPI_MEMORY_PAGE_SIZE;
			if(bytes_left_count < SPI_MEMORY_PAGE_SIZE) break;
		}
		if(bytes_left_count > 0){
			//spiStatus = memory_read_data(FSL_SPI_AUX, work_address, bytes_left_count, dest, &nBytes);
			SPIM_IO_Read(work_address,USE_4_BYTE_ADDR_MODE, bytes_left_count, dest, OPCODE_FAST_READ, 1,1,1,1);
		}
		work_address = work_address + bytes_left_count;
		///spiStatus = memory_read_data(FSL_SPI_AUX, work_address, SPI_CRC_BLOCK_SIZE,  (uint8_t*)&crc_in_flash, &nBytes);
		SPIM_IO_Read(work_address,USE_4_BYTE_ADDR_MODE, SPI_CRC_BLOCK_SIZE, (uint8_t*)&crc_in_flash, OPCODE_FAST_READ, 1,1,1,1);

		crc16_init(&flash_Crc_Desc);
		crc16_update(&flash_Crc_Desc, (uint8_t*)&header_data, SPI_HEADER_BLOCK_SIZE);		//calc crc excluding crc value
		crc16_update(&flash_Crc_Desc, (uint8_t*)olyStoredParams, PARAMS_WRITE_SIZE);		//calc crc excluding crc value
		crc16_finalize(&flash_Crc_Desc, &crcBlock);

		if(crc_in_flash == crcBlock) {
				ret = header_data[1];
		}
		else {
			printf("\nSpi Block found with bad CRC %08x  %08x\n", crc_in_flash, crcBlock);
			ret = 0;  // If CRC doesn't match return 0 (invalid age)
		}
	}
	else
		ret = 0;  // bad sig

	return ret;
}


/* Erases all spi olyParam blocks and reset the p_CurrentParamMemLoc */
uint32_t spi_flash_param_reinit()
{
	uint32_t i, result;
    //dspi_status_t spiStatus;

	for (i=0; i<SPI_NUM_MEMORY_SLOTS; i++) {
		//spiStatus = memory_erase_32Kbyte (FSL_SPI_AUX, (i * SPI_FLASH_SECTOR_SIZE));
		//result = memory_is_erased(FSL_SPI_AUX, (i * SPI_FLASH_SECTOR_SIZE), SPI_FLASH_SECTOR_SIZE);
		SPIM_EraseBlock((i * SPI_FLASH_SECTOR_SIZE),USE_4_BYTE_ADDR_MODE, OPCODE_BE_32K, NBIT_ONE, IS_BLOCK);
		result = spi_flash_check_block_erased((i * SPI_FLASH_SECTOR_SIZE), SPI_FLASH_SECTOR_SIZE);

		if (result != 1) {
			break;
		}
	}
	if(result == 1){
		printf("Spi Param Memory Reinitialized\n");
	}
	else{
		printf("Spi Param Memory Reinit failed\n");
	}
	emptyMemorySlotIndex = 0;
	flashFileAge = 0;
	return result;
}

/* Erases then writes once to single sector - why outside the function */
uint8_t bufferFlashWrite[SPI_MEMORY_PAGE_SIZE];

uint32_t spi_flash_write_oly_params(oly_flash_params_t * olyStoredParams)
{
	uint32_t result;
	uint32_t nBytes, src;
	uint32_t fail = 0;
	uint32 header_data[2];
	uint32_t destination_address = (SPI_FLASH_FS_BASE+emptyMemorySlotIndex*SPI_FLASH_SECTOR_SIZE);
	uint16_t crcBlock= 0;
	crc16_data_t flash_Crc_Desc;
	//dspi_status_t spiStatus;
	uint32_t work_size;

	//	Erase current empty slot	//
	//spiStatus = memory_erase_32Kbyte (FSL_SPI_AUX, destination_address);
	//result = memory_is_erased (FSL_SPI_AUX, (uint32_t)destination_address, SPI_FLASH_SECTOR_SIZE);
	SPIM_EraseBlock(destination_address,USE_4_BYTE_ADDR_MODE, OPCODE_BE_32K, NBIT_ONE, IS_BLOCK);
	result = spi_flash_check_block_erased(destination_address, SPI_FLASH_SECTOR_SIZE);

	if(result != 1) fail = fail | 1;
	flashFileAge++;
	crc16_init(&flash_Crc_Desc);

	//write header
	header_data[0] = SPI_HEADER_SIGNATURE;
	header_data[1] = flashFileAge;

	//spiStatus = memory_write_data (FSL_SPI_AUX, destination_address, SPI_HEADER_BLOCK_SIZE, (uint8_t*)header_data, &nBytes);
    SPIM_IO_Write(destination_address, USE_4_BYTE_ADDR_MODE, SPI_HEADER_BLOCK_SIZE, (uint8_t*)&header_data, OPCODE_PP,1,1,1);
	//if(spiStatus != kStatus_DSPI_Success) fail = fail | 2;

	uint8_t check_buff[SPI_HEADER_BLOCK_SIZE] = {0};

	SPIM_IO_Read(destination_address, USE_4_BYTE_ADDR_MODE, SPI_HEADER_BLOCK_SIZE, check_buff, OPCODE_FAST_READ, 1, 1, 1, 1);

	crc16_update(&flash_Crc_Desc, (uint8_t *) header_data, SPI_HEADER_BLOCK_SIZE);
	destination_address = destination_address + SPI_HEADER_BLOCK_SIZE;
	//spiStatus = memory_write_data (FSL_SPI_AUX, destination_address, PARAMS_WRITE_SIZE, (uint8_t*)olyStoredParams, &nBytes);
	SPIM_IO_Write(destination_address, USE_4_BYTE_ADDR_MODE, PARAMS_WRITE_SIZE, (uint8_t*)olyStoredParams, OPCODE_PP,1,1,1);
	//if(spiStatus != kStatus_DSPI_Success) fail = fail | 4;

	crc16_update(&flash_Crc_Desc, (uint8_t *) olyStoredParams, PARAMS_WRITE_SIZE);
	destination_address = destination_address + PARAMS_WRITE_SIZE;

	//write final crc
	crc16_finalize(&flash_Crc_Desc, &crcBlock);
	//spiStatus = memory_write_data (FSL_SPI_AUX, destination_address, SPI_CRC_BLOCK_SIZE, (uint8_t*)&crcBlock, &nBytes);
	SPIM_IO_Write(destination_address, USE_4_BYTE_ADDR_MODE, SPI_CRC_BLOCK_SIZE, (uint8_t*)&crcBlock, OPCODE_PP,1,1,1);


	printf("SPI Param Flash Write OK  to Slot %i of %i age %i\n", emptyMemorySlotIndex+1, SPI_NUM_MEMORY_SLOTS, flashFileAge);

	//get next free slot
	emptyMemorySlotIndex++;
	if(emptyMemorySlotIndex >= SPI_NUM_MEMORY_SLOTS) emptyMemorySlotIndex = 0;

	result = 1;
	return(result);
}

//flag to indicate which flash to use
uint32_t use_spi_flash=0;
uint32_t system_flash_write_counter=0;
uint32_t system_flash_init(oly_flash_params_t * olyStoredParams)
{
	/*
	 * we assume SPI flash is always present
	 */

	spi_flash_status_t spi_flash_status;
	//if(memory_check_for_flash_presence(FSL_SPI_AUX)){
	//	printf("FLASH INIT found external SPI flash\n");
		use_spi_flash = 1;
		//use_spi_flash = 0;
	//}
	//else
	//	printf("FLASH INIT using internal flash\n");

	if(use_spi_flash){
		spi_flash_status = spi_flash_init(olyStoredParams);
		if(spi_flash_status == SPI_FLASH_LOAD_OK){
			printf("SPI Flash Load OK -  Params size = %d\n", sizeof(oly_flash_params_t));
		}
		else {
			//this would force defaults to be loaded - IQ this is commented out in the NXP code
			//memset(olyStoredParams, 0, sizeof(oly_flash_params_t));

			//this will copy over internal flash if a backup copy is found there
			//if backup copy is corrupted, flash was erased to 0xFF which forces a re-init later
			memcpy(olyStoredParams, p_CurrentParamMemLoc, sizeof(oly_flash_params_t));
			spi_flash_status = SPI_FLASH_LOAD_OK;
		}
	}
	else{
		//this will copy over internal flash even if all 0xff
		//if this ever happens, defaults will be loaded
		memcpy(olyStoredParams, p_CurrentParamMemLoc, sizeof(oly_flash_params_t));
		spi_flash_status = SPI_FLASH_LOAD_OK;
	}
	return spi_flash_status;
}

void system_flash_write_oly_params(oly_flash_params_t * olyStoredParams)
{
	if(use_spi_flash)
	{
		spi_flash_write_oly_params(olyStoredParams);
    #if USE_DATA_FLASH
		//make sure we always have one good copy in internal flash
		if (system_flash_write_counter == 0)
		{
			write_oly_params(olyStoredParams);
			system_flash_write_counter++;
        }
    #endif
	}
#if USE_DATA_FLASH
	else
	{
		write_oly_params(olyStoredParams);
		system_flash_write_counter++;
	}
#endif
}

void system_flash_param_reinit()
{
	if (use_spi_flash)
		spi_flash_param_reinit();
		
#if USE_DATA_FLASH
	//always re-init internal flash	- need to look into - WHAT!!
	flash_param_reinit();
	system_flash_write_counter=0;
#endif
}


/** spi_flash_check_block_erased
  * @brief      Check if the block is erased
  * @param      u32Addr         Block to erase which contains the u32Addr.
  * @param      u32BlockSize	size of block
  * @return     uint32_t		result = 0 if not erased, 1 if erased.
  */
uint32_t spi_flash_check_block_erased(uint32_t u32Addr, uint32_t u32BlockSize)
{
	uint32_t result=0;
	uint8_t  g_buff[BUFFER_CHECK_SIZE];
	uint32_t *pData, offset, i;

	for (offset = 0; offset < u32BlockSize; offset += BUFFER_CHECK_SIZE)
	{
		memset(g_buff, 0, BUFFER_CHECK_SIZE);
		SPIM_IO_Read(u32Addr+offset, USE_4_BYTE_ADDR_MODE, BUFFER_CHECK_SIZE, g_buff, OPCODE_FAST_READ, 1, 1, 1, 1);

		pData = (uint32_t *)g_buff;
		for (i = 0; i < BUFFER_CHECK_SIZE; i += 4, pData++)
		{
			if (*pData != 0xFFFFFFFF)
			{
				return result;
			}
		}
	}

    result = 1;
    return result;
}


//-----------------------------------------------------------------------------
// Erases the whole firmware allocated region within the SPI flash
//-----------------------------------------------------------------------------
uint32_t spi_flash_fw_erase()
{
	uint32_t sector, result;

	for ( sector = 0; sector < SPI_FW_SECTORS; sector++ )
	{
		SPIM_EraseBlock( SPI_FLASH_FW_BASE + sector * SPI_FLASH_FW_SECTOR_SIZE,
		                 USE_4_BYTE_ADDR_MODE, OPCODE_BE_32K, NBIT_ONE, IS_BLOCK );
		result = spi_flash_check_block_erased( SPI_FLASH_FW_BASE + sector * SPI_FLASH_FW_SECTOR_SIZE,
		                                       SPI_FLASH_FW_SECTOR_SIZE );
		if ( result != 1 )
		{
			break;
		}
	}
	
	if ( result == 1 )
	{
		printf("SPI Flash FW Erased\n");
	}
	else
	{
		printf("SPI Flash FW Erase failed\n");
	}
	return result;  // 0 = fail, non-zero : success
}

//-----------------------------------------------------------------------------
// Write something to the firmware flash area (the whole FW area is already erased)
//-----------------------------------------------------------------------------
bool spi_flash_write_fw( uint8_t* pData, uint32_t offset, uint32_t dataLength )
{
	uint32_t destAddress = (SPI_FLASH_FW_BASE + offset);

    SPIM_IO_Write( destAddress,
                   USE_4_BYTE_ADDR_MODE, dataLength, pData, OPCODE_PP, 1,1,1 );

	printf("\t\tSPI FW Flash Write : Addr 0x%08X, Len %d\n", destAddress, dataLength);
	return( true );	// Always SUCCESS, confirmed on flash verification
}

//-----------------------------------------------------------------------------
// Verify content of FW SPI Flash matches buffer
//-----------------------------------------------------------------------------
bool spi_flash_verify_fw_chunk( uint32_t uiFlashOffset, uint8_t* pChunk )
{
	uint8_t  flashContent[FW_UPGRADE_CHUNK_SIZE];
	
	uint32_t uiFlashPhyAddress = SPI_FLASH_FW_BASE + uiFlashOffset; // Actual SPI Flash address

	memset(flashContent, 0, FW_UPGRADE_CHUNK_SIZE);
	SPIM_IO_Read( uiFlashPhyAddress, USE_4_BYTE_ADDR_MODE, FW_UPGRADE_CHUNK_SIZE, flashContent, OPCODE_FAST_READ, 1, 1, 1, 1);

	uint32_t *pFlashWord = (uint32_t *)flashContent;
	uint32_t *pChunkWord = (uint32_t *)pChunk;
	for ( uint32_t i = 0; i < FW_UPGRADE_CHUNK_SIZE; i += 4, pFlashWord++, pChunkWord++ )
	{
		if ( *pFlashWord != *pChunkWord )
		{
			return false;
		}
	}

    return true;
}

//-----------------------------------------------------------------------------
// Verify content of FW SPI Flash matches buffer
//-----------------------------------------------------------------------------
uint8_t spi_flash_read_fw_byte( uint32_t uiFlashOffset )
{	// Firmware content is after the OLY_REGION header.
	uint32_t uiFlashFwAddress = SPI_FLASH_FW_BASE + uiFlashOffset; // Actual FW SPI Flash address
	uint8_t flashContent;
	
	SPIM_IO_Read( uiFlashFwAddress, USE_4_BYTE_ADDR_MODE, 1, &flashContent, OPCODE_FAST_READ, 1, 1, 1, 1);

    return flashContent;
}
