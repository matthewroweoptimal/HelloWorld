/*
 * flash_params.h
 *
 *  Created on: Mar 20, 2015
 *      Author: Kris.Simonsen
 */

#ifndef FLASH_PARAMS_H_
#define FLASH_PARAMS_H_

#include <stdint.h>
#include "LOUD_types.h"

/* Starting address of Program Flash */
#define P_FLASH_BASE            0x00000000
//#define FTFx_REG_BASE           0x40020000

/* Program Flash block information */
#define P_FLASH_SIZE            (256* ONE_KB * P_BLOCK_NUM)		/* 2 banks of 256 for the M487 */
#define P_BLOCK_NUM             2
#define P_SECTOR_SIZE           0x1000							/* 4k for M487 */
#define P_SECTOR_SIZE_U32       (P_SECTOR_SIZE/sizeof(uint32_t))


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
bool flash_writeChunk( uint32_t flashAddress, uint32_t pu32Data[], uint32_t numU32s );


#endif /* FLASH_PARAMS_H_ */
