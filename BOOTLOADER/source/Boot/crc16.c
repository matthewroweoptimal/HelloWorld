/*
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "NuMicro.h"
#include "crc16.h"
#include "spi_flash_nu.h"

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////


// **************************************************************
//	CRC Utilities
// **************************************************************

//---------------------------------------------------------------
// Called at start of calculation
//---------------------------------------------------------------
void Crc16Init(P_REGION_CRC rgnCrc)
{
     // initialize running crc and byte count
	rgnCrc->currentCrc = 0;
}

//---------------------------------------------------------------
// Accumulate a Byte from an image into the CRC calculation
//---------------------------------------------------------------
static void Crc16AccumulateByte( uint32_t* pCrc, uint8_t byte )
{
    *pCrc ^= byte << 8;
    for ( uint32_t i = 0; i < 8; ++i )
    {
        uint32_t temp = *pCrc << 1;
        if (*pCrc & 0x8000)
        {
            temp ^= 0x1021;
        }
        *pCrc = temp;
    }
}

//------------------------------------------------------------------
// CRC each byte within either the SPI flash area or the APROM area
//------------------------------------------------------------------
void Crc16Update(P_REGION_CRC rgnCrc, const uint8_t * pSrc, uint32_t uiBytes, enum CRC_SOURCE sourceType)
{
    uint32_t crc = rgnCrc->currentCrc;
   	uint32_t spiFlashOffset = (uint32_t)pSrc + sizeof(OLY_REGION);	// For SPI Flash region firmware starts after the OLY_REGION header
    uint32_t j;

    for ( j = 0; j < uiBytes; )
    {
        uint32_t apromByte;
        uint32_t spiByte;

        if ( sourceType == CRC_SPI_FLASH )
        {	// CRC the firmware area in SPI flash
        	spiByte = spi_flash_read_fw_byte(spiFlashOffset + j);
        	Crc16AccumulateByte( &crc, spiByte );
        	j++;
        }
        else
        {	// CRC the firmware area in APROM (have to read 32-bit word at a time)
        	uint32_t aprom4Bytes = FMC_Read( (uint32_t)&pSrc[j] );
        	apromByte = (uint8_t)(aprom4Bytes & 0xFF);
        	Crc16AccumulateByte( &crc, apromByte );

        	apromByte = (uint8_t)((aprom4Bytes >> 8) & 0xFF);
        	Crc16AccumulateByte( &crc, apromByte );

        	apromByte = (uint8_t)((aprom4Bytes >> 16) & 0xFF);
        	Crc16AccumulateByte( &crc, apromByte );

        	apromByte = (uint8_t)((aprom4Bytes >> 24) & 0xFF);
        	Crc16AccumulateByte( &crc, apromByte );
        	j += 4;
        }
    }

    rgnCrc->currentCrc = crc;
}

//---------------------------------------------------------------
// Called to complete CRC calculation
//---------------------------------------------------------------
void Crc16Finalize(P_REGION_CRC rgnCrc, uint16_t * pHash)
{
     *pHash = rgnCrc->currentCrc;
}


