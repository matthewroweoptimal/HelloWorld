/*
 * flash.c
 *
 *  Created on: Mar 20, 2015
 *      Author: Kris.Simonsen  - IQ Refashioned for Nuvoton 2022
 */

#include <stdio.h>
#include "flash_params.h"
//#include "SSD_FTFx.h"
//#include "mqx.h"
#include "crc16.h"	//we can change to the crc32 native to nuvoton?
#include "FreeRTOS.h"
#include "task.h"

//oly_flash_params_t	__attribute__ ((section(".flash_params"))) g_flash_params;		// Global NV params block - IQ this was commented out, even in the NXP version.


/* array to copy __Launch_Command func to RAM - QI, not sure how this fits in?*/
uint8_t __ram_for_launch_cmd[LAUNCH_CMD_SIZE];

//uint32_t gCallBackCnt; /* global counter in callback(). */
//pFLASHCOMMANDSEQUENCE pFlashCommandSequence;
uint8_t * source;

void* p_CurrentParamMemLoc = NULL;
uint32_t emptyMemorySlotIndex = 0;
uint32_t flashFileAge = 0;

void flash_driver_init()
{
	uint32_t result;
	uint32_t UserConfig[2];

	SYS_UnlockReg();
	//result = FlashInit(pFlashSSDConfig);
    FMC_Open();                        /* Enable FMC ISP function */

    //We should check the dataflash is enabled and the base address is correct.

    //check the dataflash is enabled is user_config0 and the address in user_config1 is correct
    result = FMC_Read(FMC_CONFIG_BASE);
    if ((result & CONFIG_HOST_FLASH_ENABLE_MSK)  || (FMC_Read(FMC_CONFIG_BASE+4) != DATA_FLASH_ADDR))
    {
    	printf("Data flash not enabled in Config0... Attempting to enable...");
    	UserConfig[0] =  result & ~CONFIG_HOST_FLASH_ENABLE_MSK;
    	UserConfig[1] = DATA_FLASH_ADDR;
    	result = FMC_WriteConfig(UserConfig,2);
    	if (result)
    	{
    		printf("Attempt to enable data flash failed");
    		while(1);
    	} else
    		printf("Data flash enabled in Config0/1");

    	        result = FMC_ReadDataFlashBaseAddr();
    	        printf("Data Flash Base Address ............... [0x%08x]\n", result);
    }

    //The address should also be available in FMC register. Bit field DFBA accessable from the following function
    result = FMC_ReadDataFlashBaseAddr();

    if (result != DATA_FLASH_ADDR)
    {
    	printf("Data Flash Base Address does not match expected : [0x%08x]\n", result);
    	while (1);
    } else
    {
    	printf("Data Flash Base Address as expected : [0x%08x]\n", result);
    }


    //we now need to copy critical command sequence to RAM. The following is how the original driver worked. I can't just use it but might be able to re-write!
	//IQ ?!? pFlashCommandSequence = (pFLASHCOMMANDSEQUENCE)RelocateFunction((uint32_t)__ram_for_launch_cmd , LAUNCH_CMD_SIZE ,(uint32_t)FlashCommandSequence);
}

void flash_init()
{
	uint32_t i;
	uint32_t sectors = FLASH_PARAMS_SIZE / P_SECTOR_SIZE;
	uint32_t paramsWriteSize = (((sizeof(oly_flash_params_t) + 7)/8) * 8);				// Round up to nearest 64-bit boundary (8 byte address boundary)
	uint32_t fileWriteSize = paramsWriteSize + CRC_BLOCK_SIZE + HEADER_BLOCK_SIZE;		// Round up to nearest 64-bit boundary (8 byte address boundary)
	uint32_t sectorsPerSlot = (fileWriteSize + (P_SECTOR_SIZE-1))/P_SECTOR_SIZE;
	uint32_t numMemorySlots = sectors / sectorsPerSlot;
	uint32_t latestFlashFileIndex = 0;
	uint32_t latestFlashFileAge = 0;
	uint32_t flashFileAddress;
	uint32_t tempflashFileAge;

	printf("Flash Sectors=%i, WriteSize=%i, Slots=%i\n", sectors, fileWriteSize, numMemorySlots);

	flash_driver_init();

	printf("Flash check slot ");
	//Need to initialize pointer even if no flash data is valid
	p_CurrentParamMemLoc = (void *)(FLASH_PARAMS_ADDR+HEADER_BLOCK_SIZE);
	//	Look for the most recent save//
	for( i=0; i<numMemorySlots; i++)	{
		printf("%i", i+1);
		flashFileAddress = FLASH_PARAMS_ADDR+(sectorsPerSlot*P_SECTOR_SIZE)*i;
		tempflashFileAge = flash_check_for_valid_data(flashFileAddress, fileWriteSize);
		if(tempflashFileAge != 0) {
			if(latestFlashFileAge < tempflashFileAge) {
				latestFlashFileAge = tempflashFileAge;
				latestFlashFileIndex = i;
				p_CurrentParamMemLoc = (void *)(flashFileAddress + HEADER_BLOCK_SIZE); // also keep address to avoid recalc
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
		if(emptyMemorySlotIndex >= numMemorySlots) emptyMemorySlotIndex = 0;
	}
	else {
		emptyMemorySlotIndex = 0; //put at start for save
		printf("Param Memory CRC Error\n");
		flash_param_reinit();
	}

}

uint32_t flash_check_for_valid_data(uint32_t startAddress, uint32_t size)
{
	crc16_data_t flash_Crc_Desc;
	uint16_t crcBlock= 0;
	uint32_t flashSigAddress = startAddress;
	uint32_t flashAgeAddress = startAddress + 4;
	uint32_t flashCRCAddress = startAddress + size - CRC_BLOCK_SIZE;
	uint32_t *p_flashSig =  (uint32_t *)flashSigAddress;
	uint32_t *p_flashAge =  (uint32_t *)flashAgeAddress;
	uint16_t *p_flashCRC =  (uint16_t *)flashCRCAddress;

	printf("started check len= %08x add = %08x  sig = %08x  age = %08x\n", size, startAddress, *p_flashSig, *p_flashAge);

	// Check CRC //
	if(*p_flashSig != HEADER_SIGNATURE) {
		return 0;													//bad copy
	}

	crc16_init(&flash_Crc_Desc);
	crc16_update(&flash_Crc_Desc, (uint8_t*) startAddress, size - CRC_BLOCK_SIZE);     //calc crc excluding crc value
	crc16_finalize(&flash_Crc_Desc, &crcBlock);

	if(*p_flashCRC == crcBlock) {
		return *p_flashAge;
	}
	else {
		printf("\n Block found with bad CRC %08x  %08x", *p_flashCRC, crcBlock);
		return 0;  // If CRC doesn't match return 0 (invalid age)
	}
}


/* Erases the whole olyParam block and reset the p_CurrentParamMemLoc */
uint32_t flash_param_reinit(void)
{
	uint32_t i, result;
	uint32_t sectors = FLASH_PARAMS_SIZE / P_SECTOR_SIZE;					//16 sectors, 4k each


	for (i=0; i<sectors; i++) {
		//_int_disable();
		result = FMC_Erase(FLASH_PARAMS_ADDR + (i * P_SECTOR_SIZE));		//erases 4kbytes
		//_int_enable();
		if (result != FMC_RESULT_OK) {
			printf("Param Flash Erase error = %x\n", result);
			return result;
		}

		result = FMC_CheckAllOne((FLASH_PARAMS_ADDR + (i * P_SECTOR_SIZE)), P_SECTOR_SIZE);

		if (result != READ_ALLONE_YES) {
			printf("Param Flash Verify error = %x\n", result);
			return result;
		}
	}

	emptyMemorySlotIndex = 0;
	p_CurrentParamMemLoc = (void*) FLASH_PARAMS_ADDR + HEADER_BLOCK_SIZE; //this now points to 0xff ?? not best way to do this? IQ - I do not get this, not 0xff
	printf("Param Memory Reinitialized\n");
	flashFileAge = 0;
	return 0;
}

/* Erases then writes once to single sector */
uint32_t write_oly_params(oly_flash_params_t * pParams)
{
	uint32_t i, j, result;

	uint32_t sectors = FLASH_PARAMS_SIZE / P_SECTOR_SIZE;
	uint32_t paramsWriteSize = (((sizeof(oly_flash_params_t) + 7)/8) * 8);		// Round up to nearest 64-bit boundary (8 byte address boundary)
	uint32_t fileWriteSize = paramsWriteSize + CRC_BLOCK_SIZE + HEADER_BLOCK_SIZE;		// Round up to nearest 64-bit boundary (8 byte address boundary)
	uint32_t sectorsPerSlot = (fileWriteSize + (P_SECTOR_SIZE-1))/P_SECTOR_SIZE;
	uint32_t numMemorySlots = sectors / sectorsPerSlot;
	flashFileAge++;

	crc16_data_t flash_Crc_Desc;
	uint32_t flashDestination = FLASH_PARAMS_ADDR + ((sectorsPerSlot*emptyMemorySlotIndex) * P_SECTOR_SIZE);
	uint32_t dest, src, count, flashBlocksize, blocksToWrite;

	uint16_t crcBlock[CRC_BLOCK_SIZE/2] = {0};



	//	Erase current empty slot	//
	for (i=0; i< sectorsPerSlot; i++) {
		//_int_disable(); can the values be changed by an interrupt?
		result = FMC_Erase(FLASH_PARAMS_ADDR + ((i + (sectorsPerSlot*emptyMemorySlotIndex)) * P_SECTOR_SIZE));
		//_int_enable();
		if (result != FMC_RESULT_OK) {
			printf("Param Flash Erase error = %x\n", result);
			return result;
		}

		//_disable();
		result = FMC_CheckAllOne(FLASH_PARAMS_ADDR + ((i + (sectorsPerSlot*emptyMemorySlotIndex)) * P_SECTOR_SIZE), P_SECTOR_SIZE);
		//_int_enable();

		if (result != READ_ALLONE_YES) {
			printf("Param Flash Verify error = %x\n", result);
			return result;
		}
	}

	//	write signature and age (write cycle count)	//
	//	FMC_WriteMultiple has querks. Will not write less than 16 bytes and the number of bytes to program needs 8 taking from it!
	//  Must use different function if less than 16 bytes.
	//_int_disable();
    //result = FMC_WriteMultiple(flashDestination, header, HEADER_BLOCK_SIZE);
    //result = FMC_Write8Bytes(flashDestination, HEADER_SIGNATURE, flashFileAge);

	//_int_enable();
//	if (result != FMC_RESULT_OK) {
//		printf("Param Flash Program error = %x\n", result);
//		return result;
//	}

	//Write the parameter data
	//break into blocks to avoid long interrupt disable - IQ why do we disable interrupts?
	blocksToWrite = (paramsWriteSize + (FLASH_WRITE_BLOCK_SIZE-1))/FLASH_WRITE_BLOCK_SIZE;
	dest = flashDestination + HEADER_BLOCK_SIZE;
	src = (uint32_t) pParams;
	count = paramsWriteSize;
	for(i=0; i< blocksToWrite; i++){
		if(count >= FLASH_WRITE_BLOCK_SIZE) flashBlocksize = FLASH_WRITE_BLOCK_SIZE;
		else flashBlocksize = count;

		taskDISABLE_INTERRUPTS();
		//FMC_WriteMultiple will write 8 more bytes than you ask it to!
	    result = FMC_WriteMultiple(dest, (uint32_t *) src, flashBlocksize);
	    taskENABLE_INTERRUPTS();

		dest = dest + FLASH_WRITE_BLOCK_SIZE;
		src = src + FLASH_WRITE_BLOCK_SIZE;
		count = count - FLASH_WRITE_BLOCK_SIZE;


		if (result != flashBlocksize) {
			printf("Param Flash Program error = %x\n", result);
			return result;
		}
	}

	// Calculate and write CRC //
	crc16_init(&flash_Crc_Desc);
	crc16_update(&flash_Crc_Desc, (uint8_t *) flashDestination, fileWriteSize - CRC_BLOCK_SIZE);
	crc16_finalize(&flash_Crc_Desc, crcBlock);
	//_int_disable();
    result = FMC_WriteMultiple(flashDestination + fileWriteSize - CRC_BLOCK_SIZE, (uint32_t*) &crcBlock, CRC_BLOCK_SIZE-8);
	//_int_enable();
	if (result != FMC_RESULT_OK) {
		printf("Param CRC Flash Program error = %x\n", result);
		return result;
	}

	//	Update the static pointer to the most recent param memory location	//
	p_CurrentParamMemLoc = (void*) FLASH_PARAMS_ADDR + HEADER_BLOCK_SIZE + ((sectorsPerSlot*emptyMemorySlotIndex) * P_SECTOR_SIZE);

	if(!result)	printf("Param Flash Write OK to Slot %i of %i age %i\n", emptyMemorySlotIndex+1, numMemorySlots, flashFileAge);
	//get next free slot
	emptyMemorySlotIndex++;
	if(emptyMemorySlotIndex >= numMemorySlots) emptyMemorySlotIndex = 0;

	return(result);
}


/* Erases then writes once to single sector */
uint32_t write_sector(uint32_t uiFlashAddress, uint8 *pData)
{
	uint32_t result;
	uint32_t i, dest, src, count, flashBlocksize, blocksToWrite;
	//_int_disable();
	//result = FlashEraseSector(&FlashSSDConfig, uiFlashAddress, P_SECTOR_SIZE, pFlashCommandSequence);
	result = FMC_Erase(uiFlashAddress);
	//_int_enable();
	if (result != FMC_RESULT_OK)
	{
		printf("Region Flash Erase error = %x\n", result);
		return result;
	}


	//_int_disable();
	//result = FlashVerifySection(&FlashSSDConfig, uiFlashAddress, FTFx_PSECTOR_SIZE/FSL_FEATURE_FLASH_PFLASH_SECTION_CMD_ADDRESS_ALIGMENT, 0, pFlashCommandSequence);
	result = FMC_CheckAllOne(uiFlashAddress, P_SECTOR_SIZE);
	//_int_enable();
	if (result != READ_ALLONE_YES)
	{
		printf("Region Flash Verify error = %x\n", result);
		return result;
	}

	//_int_disable();
	//result =  FlashProgram(&FlashSSDConfig, uiFlashAddress,P_SECTOR_SIZE, pData, pFlashCommandSequence);

	blocksToWrite = P_SECTOR_SIZE/FLASH_WRITE_BLOCK_SIZE;
	dest = uiFlashAddress;
	src = (uint32_t) pData;
	count = P_SECTOR_SIZE;
	for(i=0; i< blocksToWrite; i++)
	{
		if(count >= FLASH_WRITE_BLOCK_SIZE) flashBlocksize = FLASH_WRITE_BLOCK_SIZE;
		else flashBlocksize = count;

		//_int_disable();
	    result = FMC_WriteMultiple(dest, &src, flashBlocksize-8);
		//_int_enable();

		dest = dest + FLASH_WRITE_BLOCK_SIZE;
		src = src + FLASH_WRITE_BLOCK_SIZE;
		count = count - FLASH_WRITE_BLOCK_SIZE;


		if (result != FMC_RESULT_OK)
		{
			printf("Param Flash Program error = %x\n", result);
			return result;
		}
	if(!result)	printf("Region Flash Write OK\n");
	}
	return(result);

}

/* Special case of write sector where interrupts are not enabled between sub operations as vector
 * table is being overwritten */
uint32_t write_sector_zero(uint8 *pData)
{
	uint32_t result;

	//_int_disable();
	//result = FlashEraseSector(&FlashSSDConfig, 0, P_SECTOR_SIZE, pFlashCommandSequence);
	result = FMC_Erase(0);

	if (result != FMC_RESULT_OK) {
		//_int_enable();
		printf("Region Flash Erase error = %x\n", result);
		return result;
	}


	//result = FlashVerifySection(&FlashSSDConfig, 0, FTFx_PSECTOR_SIZE/FSL_FEATURE_FLASH_PFLASH_SECTION_CMD_ADDRESS_ALIGMENT, 0, pFlashCommandSequence);


	if (result != FMC_RESULT_OK) {
		//_int_enable();
		printf("Region Flash Verify error = %x\n", result);
		return result;
	}

	//result =  FlashProgram(&FlashSSDConfig, 0,P_SECTOR_SIZE, pData, pFlashCommandSequence);
	//_int_enable();
	if (result != FMC_RESULT_OK) {
		printf("Region Flash Program error = %x\n", result);
		return result;
	}

	return(result);
}
