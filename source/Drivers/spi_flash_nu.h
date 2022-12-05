/*
 * spi_flash_fs.h
 *
 *  Created on: may 2, 2016
 *      Author: ben.huber
 */

#ifndef SPI_FLASH_NU_H_
#define SPI_FLASH_NU_H_

//#include "board.h" is c file
//#include "oly.h"
extern uint32_t emptyMemorySlotIndex;
extern uint32_t flashFileAge;

/* Starting address in Flash */
#define SPI_FLASH_FS_BASE		0x00000000
#define SPI_FLASH_FS_SIZE		0x20000				//128KByte
#define SPI_FLASH_SECTOR_SIZE	0x8000				//32KByte
#define SPI_NUM_MEMORY_SLOTS	(SPI_FLASH_FS_SIZE/0x8000)	//4 slots

#define OPCODE_BE_32K           		0x52U   /* Erase 64KB block */
#define FLASH_WRITE_CLEAR_PROTECT 		1
#define FLASH_WRITE_PROTECT				0
#define FLASH_NUM_BITS_4				4
#define SPIM_INITFLASH_SUCCESS			0
#define USE_4_BYTE_ADDR_MODE			0	//Nu flash is 3 byte address
#define IS_BLOCK						1	//??
#define NBIT_ONE						1	//??
#define SPI_MEMORY_PAGE_SIZE           	0x0100
#define BUFFER_CHECK_SIZE				0x400	//buffer size to read when checking erase


#define PARAMS_WRITE_SIZE sizeof(oly_flash_params_t)





/* Other defines */
#define SPI_CRC_BLOCK_SIZE			2
#define SPI_HEADER_BLOCK_SIZE		8
#define SPI_HEADER_SIGNATURE		0xAA55A55A

typedef enum {
	SPI_FLASH_LOAD_OK = 0x00,
	SPI_FLASH_LOAD_FAIL = 0x01,
	SPI_FLASH_NOT_FOUND = 0x02,
	SPI_FLASH_DRIVER_ERROR = 0x3,
} spi_flash_status_t;


spi_flash_status_t spi_flash_init(oly_flash_params_t * olyStoredParams);
uint32_t spi_flash_check_for_valid_data(uint32_t page_num, uint32 size, oly_flash_params_t * olyStoredParams);
uint32_t spi_flash_param_reinit();
uint32_t spi_flash_write_oly_params(oly_flash_params_t * pParams);
uint32_t system_flash_init(oly_flash_params_t * olyStoredParams);
void system_flash_write_oly_params(oly_flash_params_t * olyStoredParams);
void system_flash_param_reinit();
uint32_t spi_flash_check_block_erased(uint32_t u32Addr, uint32_t u32BlockSize);

extern uint32_t emptyMemorySlotIndex;


#endif /* SPI_FLASH_H_ */
