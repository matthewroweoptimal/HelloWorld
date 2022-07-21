/*
 * SpiSharcPort.h
 *
 *  Created on: Jul 1, 2015
 *      Author: Kris.Simonsen
 */

#ifndef SOURCES_SPISHARCPORT_H_
#define SOURCES_SPISHARCPORT_H_

#include <MandolinPort.h>

extern "C" {
#include "CommMandolin.h"
#include "MQX_To_FreeRTOS.h"
}

// DSP has different size limit in mandolin_config.h
#define SHARC_MAX_MESSAGE_SIZE		(MANDOLIN_HEADER_CHECKSUM_BYTES + (512u * MANDOLIN_BYTES_PER_WORD))
#define	SPISHARC_MANDOLIN_FIFO_SIZE   (SHARC_MAX_MESSAGE_SIZE+100)


// Number of bytes to write when pulling message from SPI
#define SHARC_BYTES_TO_READ_FIRST		160
#define SHARC_BYTES_TO_READ_NEXT		100

#define USE_SPISHARC_MANDOLIN_FIFO

namespace oly {


class SpiSharcPort: public MandolinPort
{
protected:
	//mandolin_mqx_message		tmpMsg;
	uint8_t 					tmpTxBuffer[SHARC_MAX_MESSAGE_SIZE];
	uint8_t 					tmpRxBuffer[2 * SHARC_MAX_MESSAGE_SIZE];

#ifdef USE_SPISHARC_MANDOLIN_FIFO
	mandolin_fifo				SPISharcRxFifo;
	uint8_t						SPISharcRxFifo_bytes[SPISHARC_MANDOLIN_FIFO_SIZE+10];
#endif

	bool dspWriteData(uint8_t * pTxData, uint32_t length_bytes);
	MUTEX_STRUCT_PTR spi_dsp_mutex;
	MUTEX_ATTR_STRUCT spi_dsp_mutex_attr;
	uint32 CallMasterTransferBlocking(uint32_t instance,
            const uint8_t * sendBuffer,
            uint8_t * receiveBuffer,
            size_t transferByteCount,
            uint32_t timeout);
public:
	SpiSharcPort(mandolin_fifo * rx_fifo);
	void WriteMessage(mandolin_message * pMsg, bool bRequireConnection=false);	//	set bRequireConnection if mandolin connection process must have completed to send this message (only used for network port currently)
	void ReadPendingMessage(uint32 uiBytesToRead);
};

} /* namespace oly */

#endif /* SOURCES_SPISHARCPORT_H_ */
