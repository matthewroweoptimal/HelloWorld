/*
 * flash_params.h
 *
 *  Created on: Mar 20, 2015
 *      Author: Kris.Simonsen
 */

#ifndef FLASH_PARAMS_H_
#define FLASH_PARAMS_H_

#include "board.h"
//#include "SSD_FTFx.h"
#include "oly.h"



#define CONFIG_HOST_FLASH_ENABLE_MSK	0x00000001
#define DATA_FLASH_ADDR					0x0007E000	// Allows 8kB at top of Flash for region which will include :
													//     0x0007E000 : OLY_BLOCK			(See : Upgrade.h)
													//     0x0007F000 : OLY_IDENTITY


/* Starting address of Program Flash */
#define P_FLASH_BASE            0x00000000
#define FTFx_REG_BASE           0x40020000

/* Program Flash block information */
#define P_FLASH_SIZE            (256* ONE_KB * P_BLOCK_NUM)		/* 2 banks of 256 for the M487 */
#define P_BLOCK_NUM             2
#define P_SECTOR_SIZE           0x1000							/* 4k for M487 */

/* Data Flash block information */
#define FLEXNVM_BASE            DATA_FLASH_ADDR
#define FLEXNVM_SECTOR_SIZE     4*ONE_KB
#define FLEXNVM_BLOCK_SIZE      4*ONE_KB
#define FLEXNVM_BLOCK_NUM       16
/* Flex Ram block information */
//#define EERAM_BASE              FSL_FEATURE_FLASH_FLEX_RAM_START_ADDRESS
//#define EERAM_SIZE              FSL_FEATURE_FLASH_FLEX_RAM_SIZE

#define	FLASH_WRITE_BLOCKSIZE	32

/* Can be written to : Flash Parameter Storage is now in SPI Flash for Nuvoton System */
//#define FLASH_PARAMS_ADDR		0x00070000
//#define FLASH_PARAMS_SIZE		0x10000

#define FMC_RESULT_OK					0

/* These defines relate to a callback function which must be copied to RAM during runtime */
#define LAUNCH_CMD_SIZE         0x100

/* Size of function used for callback.  Change this depending on the size of your function */
#define CALLBACK_SIZE           0x80


/* Other defines */
#define CRC_BLOCK_SIZE			8
#define HEADER_BLOCK_SIZE		8
#define HEADER_SIGNATURE		0xAA55A55A
#define FLASH_WRITE_BLOCK_SIZE  0x100						//512 bytes
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


void flash_init();
bool data_flash_program( uint32_t dataFlashAddr, uint32_t *pu32Data, uint32_t numU32s );
bool data_flash_read( uint32_t dataFlashAddr, uint32_t *pu32Data, uint32_t numU32s );

extern void* p_CurrentParamMemLoc;


#endif /* FLASH_PARAMS_H_ */
