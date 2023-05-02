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
#include "Upgrade.h"
#include <string.h>

//oly_flash_params_t	__attribute__ ((section(".flash_params"))) g_flash_params;		// Global NV params block - IQ this was commented out, even in the NXP version.


/* array to copy __Launch_Command func to RAM - QI, not sure how this fits in?*/
uint8_t __ram_for_launch_cmd[LAUNCH_CMD_SIZE];

//uint32_t gCallBackCnt; /* global counter in callback(). */
//pFLASHCOMMANDSEQUENCE pFlashCommandSequence;
uint8_t * source;

void* p_CurrentParamMemLoc = NULL;
uint32_t emptyMemorySlotIndex = 0;
uint32_t flashFileAge = 0;

static void flash_driver_init()
{
	uint32_t result;
	uint32_t UserConfig[2];

	SYS_UnlockReg();
    FMC_Open();                        /* Enable FMC ISP function */

    //We should check the dataflash is enabled and the base address is correct.

    //check the dataflash is enabled is user_config0 and the address in user_config1 is correct
    UserConfig[0] = FMC_Read(FMC_CONFIG_BASE);
    UserConfig[1] = FMC_Read(FMC_CONFIG_BASE + 4);
    if ((UserConfig[0] & CONFIG_HOST_FLASH_ENABLE_MSK)  || (UserConfig[1] != DATA_FLASH_ADDR))
    {
    	printf("Data flash not enabled in Config0... Attempting to enable...\n");
    	UserConfig[0] &=  ~CONFIG_HOST_FLASH_ENABLE_MSK;	// Clear DFEN bit to enable data flash area
    	UserConfig[1] = DATA_FLASH_ADDR;					// Set the Data Flash base address

    	result = FMC_WriteConfig(UserConfig,2);
    	if (result)
    	{
    		printf("Attempt to enable data flash failed\n");
    		while(1);
    	} else
    		printf("Data flash enabled in Config0/1\n");

    	    result = FMC_ReadDataFlashBaseAddr();
    	    printf("Data Flash Base Address ............... [0x%08x]\n", result);
    }

    // The address should also be available in FMC register. Bit field DFBA accessable from the following function
    result = FMC_ReadDataFlashBaseAddr();

    if (result != DATA_FLASH_ADDR)
    {
    	printf("Data Flash Base Address 0x%08x does not match expected : [0x%08x] (Try Reboot)\n", result, DATA_FLASH_ADDR);

        //---- Perform chip reset to make new User Config take effect ----
        SYS_ResetChip();
    	while (1);
    }
    else
    {
    	printf("Data Flash Base Address as expected : [0x%08x]\n", result);
    }

	FMC_Close();
	SYS_LockReg();
}

void flash_init()
{	// Initialise Data Flash
	flash_driver_init();
}

//-----------------------------------------------------------------------------
// Program the specified block of memory to Data Flash
//-----------------------------------------------------------------------------
bool data_flash_program( uint32_t dataFlashAddr, uint32_t *pu32Data, uint32_t numU32s )
{   
    if ( dataFlashAddr < DATA_FLASH_ADDR )
    {
        printf("Data Flash address %08X is below 0x%x\n", dataFlashAddr, DATA_FLASH_ADDR);
        return false;
    }

	SYS_UnlockReg();
    FMC_Open();                        /*--- Enable FMC ISP function ---*/

    bool bRet = true;
    if ( FMC_Erase(dataFlashAddr) != 0 )                // Erase 4kB page
    {
        printf("Data Flash Erase @ %08X failed!\n", dataFlashAddr);
        bRet = false;
    }
    else
    {
		for ( uint32_t addr = dataFlashAddr, i = 0; i < numU32s; addr += 4, i++ )
		{
			if ( FMC_Write(addr, pu32Data[i]) != 0 )        /* Program flash */
			{
				printf("Data Flash Write @ %08X failed!\n", addr);
				bRet = false;
			}
			else
			{
				uint32_t verification = FMC_Read(addr);
	            if (g_FMC_i32ErrCode != 0)
	            {
	                printf("Data Flash Verify Failed @ %08X\n", addr);
	                bRet = false;
	            }
	            else if ( verification != pu32Data[i] )
				{
					printf("Data Flash Verify Failed @ 0x%08X, wrote 0x%08X, read 0x%08X\n", addr, pu32Data[i], verification);
				}
			}
		}
    }

    FMC_Close();                       /*--- Disable FMC ISP function ---*/
	SYS_LockReg();
    return bRet;
}

//-----------------------------------------------------------------------------
// Read a block of data from Data Flash
//-----------------------------------------------------------------------------
bool data_flash_read( uint32_t dataFlashAddr, uint32_t *pu32Data, uint32_t numU32s )
{
    if ( dataFlashAddr < DATA_FLASH_ADDR )
    {
        printf("Data Flash address 0x%x is below 0x%x\n", dataFlashAddr, DATA_FLASH_ADDR);
        return false;
    }

	SYS_UnlockReg();
    FMC_Open();                        /*--- Enable FMC ISP function ---*/

    bool bRet = true;
	for ( uint32_t addr = dataFlashAddr, i = 0; i < numU32s; addr += 4, i++ )
	{
		pu32Data[i] = FMC_Read(addr);
        if (g_FMC_i32ErrCode != 0)
        {
            printf("Data Flash Read Failed @ %08X\n", addr);
            bRet = false;
            break;
        }
    }

    FMC_Close();                       /*--- Disable FMC ISP function ---*/
	SYS_LockReg();
    return bRet;
}

