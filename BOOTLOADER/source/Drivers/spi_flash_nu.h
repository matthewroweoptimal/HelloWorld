/*
 * spi_flash_fs.h
 *
 *  Created on: may 2, 2016
 *      Author: ben.huber
 */

#ifndef SPI_FLASH_NU_H_
#define SPI_FLASH_NU_H_

#include "Upgrade.h"


/* Firmware Image Area in SPI Flash */
#define SPI_FLASH_FW_BASE		    0x00100000          // Second MiB is reserved for firmware image storage
#define SPI_FLASH_FW_SIZE		    0x80000				// 512 KByte reserved for firmware image storage
#define SPI_FLASH_FW_SECTOR_SIZE    0x8000				// 32 KByte sectors
#define SPI_FW_SECTORS      	    (SPI_FLASH_FW_SIZE/SPI_FLASH_FW_SECTOR_SIZE)	// 16 sectors

#define OPCODE_BE_32K           		0x52U   /* Erase 32KB block */
#define FLASH_WRITE_CLEAR_PROTECT 		1
#define FLASH_WRITE_PROTECT				0
#define FLASH_NUM_BITS_4				4
#define SPIM_INITFLASH_SUCCESS			0
#define USE_4_BYTE_ADDR_MODE			0	//Nu flash is 3 byte address
#define IS_BLOCK						1	//??
#define NBIT_ONE						1	//??
#define SPI_MEMORY_PAGE_SIZE           	0x0100

typedef enum {
	SPI_FLASH_LOAD_OK = 0x00,
	SPI_FLASH_LOAD_FAIL = 0x01,
	SPI_FLASH_NOT_FOUND = 0x02,
	SPI_FLASH_DRIVER_ERROR = 0x3,
} spi_flash_status_t;



spi_flash_status_t spi_flash_init(void);
void	spi_flash_readFwHeader( OLY_REGION* pHeader );
void    spi_flash_readFwSector( uint32_t uiFlashOffset, uint32_t* pSector );
uint8_t	spi_flash_read_fw_byte( uint32_t uiFlashOffset );


#endif /* SPI_FLASH_H_ */
