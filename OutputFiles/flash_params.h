/*
 * flash_params.h
 *
 *  Created on: Mar 20, 2015
 *      Author: Kris.Simonsen
 */

#ifndef FLASH_PARAMS_H_
#define FLASH_PARAMS_H_

#include "board.h"
#include "SSD_FTFx.h"
#include "oly.h"


/* Starting address of Program Flash */
#define P_FLASH_BASE            0x00000000
#define FTFx_REG_BASE           0x40020000

/* Program Flash block information */
#define P_FLASH_SIZE            (FSL_FEATURE_FLASH_PFLASH_BLOCK_SIZE * FSL_FEATURE_FLASH_PFLASH_BLOCK_COUNT)
#define P_BLOCK_NUM             FSL_FEATURE_FLASH_PFLASH_BLOCK_COUNT
#define P_SECTOR_SIZE           FSL_FEATURE_FLASH_PFLASH_BLOCK_SECTOR_SIZE	// 4k for K64
/* Data Flash block information */
#define FLEXNVM_BASE            FSL_FEATURE_FLASH_FLEX_NVM_START_ADDRESS
#define FLEXNVM_SECTOR_SIZE     FSL_FEATURE_FLASH_FLEX_NVM_BLOCK_SECTOR_SIZE
#define FLEXNVM_BLOCK_SIZE      FSL_FEATURE_FLASH_FLEX_NVM_BLOCK_SIZE
#define FLEXNVM_BLOCK_NUM       FSL_FEATURE_FLASH_FLEX_NVM_BLOCK_COUNT
/* Flex Ram block information */
#define EERAM_BASE              FSL_FEATURE_FLASH_FLEX_RAM_START_ADDRESS
#define EERAM_SIZE              FSL_FEATURE_FLASH_FLEX_RAM_SIZE

#define	FLASH_WRITE_BLOCKSIZE	FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE

/* Can be written to */
#define FLASH_PARAMS_ADDR		0x000FB000
#define FLASH_PARAMS_SIZE		0x4000

/* These defines relate to a callback function which must be copied to RAM during runtime */
#define LAUNCH_CMD_SIZE         0x100

/* Size of function used for callback.  Change this depending on the size of your function */
#define CALLBACK_SIZE           0x80


/* Other defines */
#define CRC_BLOCK_SIZE			8
#define HEADER_BLOCK_SIZE		8
#define HEADER_SIGNATURE		0xAA55A55A
#define FLASH_WRITE_BLOCK_SIZE  0x100
#define ONE_KB                  1024                        //0x400:  10 zeros
#define TWO_KB                  (2*ONE_KB)
#define THREE_KB                (3*ONE_KB)
#define FOUR_KB                 (4*ONE_KB)
#define FIVE_KB                 (5*ONE_KB)
#define SIX_KB                  (6*ONE_KB)
#define SEVEN_KB                (7*ONE_KB)
#define EIGHT_KB                (8*ONE_KB)
#define NINE_KB                 (9*ONE_KB)
#define TEN_KB                  (10*ONE_KB)
#define ONE_MB                  (ONE_KB*ONE_KB)             //0x100000:     20 zeros
#define ONE_GB                  (ONE_KB*ONE_KB*ONE_KB)      //0x40000000:   30 zeros

#define EnableInterrupts asm(" CPSIE i");
#define DisableInterrupts asm(" CPSID i");

void flash_init();
void flash_driver_init();
uint32_t flash_check_for_valid_data(uint32_t p_startAddress, uint32 size);
uint32_t flash_param_reinit(void);
uint32_t write_oly_params(oly_flash_params_t * pParams);
uint32_t write_sector(uint32_t uiFlashAddress, uint8 *pData);
uint32_t write_sector_zero(uint8 *pData);

extern void* p_CurrentParamMemLoc;
extern uint32_t emptyMemorySlotIndex;

#endif /* FLASH_PARAMS_H_ */
