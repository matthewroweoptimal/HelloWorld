/*
 * SpiSharcPort.cpp
 *
 *  Created on: Jul 1, 2015
 *      Author: Kris.Simonsen
 */

#include <SpiSharcPort.h>
#include <stdio.h>

extern "C" {
//#include "gpio_pins.h"
}

#include "spi_sharc.h"	// TODO : SPI Driver comms to SHARC to be done
#include "os_tasks.h"

namespace oly {

SpiSharcPort::SpiSharcPort(mandolin_fifo * rx_fifo) : MandolinPort(rx_fifo)
{
	int i;
	int j;
	int nSize;

	port_qid = _msgq_open(DSP_QUEUE, 0);
	m_bPortOpen = true;	// Port is always open to DSP

	// debug for now
	nSize = sizeof(tmpTxBuffer)/ sizeof(uint8_t);
	for (i = 0; i< nSize; i++){
		tmpTxBuffer[i] = i & 0x0ff;
	}
	nSize = sizeof(tmpRxBuffer)/ sizeof(uint8_t);
	for (i = 0; i< nSize; i++){
		tmpRxBuffer[i] = i & 0x0ff;
	}

	tmpRxBuffer[MAX_MANDOLIN_MSG_SIZE-12] = 0x55; // fill with a specific value that never gets writen to
	//end of debug

	_mutatr_init(&spi_dsp_mutex_attr);
	//_mutatr_set_sched_protocol(&spi_dsp_mutex_attr, (MUTEX_PRIO_INHERIT | MUTEX_PRIO_PROTECT));
	//_mutatr_set_wait_protocol(&spi_dsp_mutex_attr, MUTEX_QUEUEING);
	_mutex_init(spi_dsp_mutex, &spi_dsp_mutex_attr);



#ifdef USE_SPISHARC_MANDOLIN_FIFO
	MANDOLIN_FIFO_init(&SPISharcRxFifo,SPISHARC_MANDOLIN_FIFO_SIZE, SPISharcRxFifo_bytes);
#endif

}

void SpiSharcPort::WriteMessage(mandolin_message * pMsg, bool bRequireConnection) {

	if (pMsg) {
		if (pMsg->transport == MANDOLIN_TRANSPORT_NULL) {
			pMsg->transport = MANDOLIN_TRANSPORT_NOACK;	// Don't need ACKs on DSP messages
			MANDOLIN_MSG_pack(pMsg,pMsg->sequence);	// recalc CRC
		}

		if (MANDOLIN_MSG_bytes(pMsg) > SHARC_MAX_MESSAGE_SIZE){
			printf("ERR: DSP Write Error: message too large for DSP");
			return;
		}

		_mutex_lock(spi_dsp_mutex);
		// Need message + payload to be in contiguous mem
		uint16_t message_length_bytes = MANDOLIN_MSG_write((uint8_t*)&tmpTxBuffer,pMsg);  // tmpTxBuffer not thread safe, it is a class variable that is different if different tasks use WriteMessage
		dspWriteData(tmpTxBuffer, message_length_bytes);
		_mutex_unlock(spi_dsp_mutex);
	}
}

int gdspWriteDataErrorCount = 0;
bool SpiSharcPort::dspWriteData(uint8_t * pTxData, uint32_t length_bytes) {
	// this is called from 2 different tasks  (config and sysevent and meter)  (this is mutexed around this) and needs to be mutex since they are sharing a class variable tmpRxBuffer and tmpTxBuffer that can be trashed with other task
	int i;
	uint32 nBytesTransferred;
	uint32 nAvailable;


#ifndef USE_SPISHARC_MANDOLIN_FIFO
	uint16_t	bytes_to_process = 0;
	uint8_t*	byte_ptr,end_ptr;
	dspi_status_t result;
	uint8_t*	pBytes;
	uint16_t 	uiHeaderChecksum;

	mandolin_mqx_message_ptr rx_msg = NULL;
#endif

	if (length_bytes == 0)
	{
		return false;
	}

	if (length_bytes > SHARC_MAX_MESSAGE_SIZE){
		printf("ERR: DSP Write Error: message too large for DSP");
		gdspWriteDataErrorCount++;		// fill with a specific value that never gets writen to
		return false;
	}

	TASKDEBUG_POS(TASK_SPISHARC,0x77)

	nBytesTransferred = CallMasterTransferBlocking(FSL_SPI_SHARC,pTxData,tmpRxBuffer,length_bytes,500);
	if (nBytesTransferred == 0) // check if transfer worked
	{
		gdspWriteDataErrorCount++;
		return false;   // transferred did not work.. so now what?
	}

#ifdef USE_SPISHARC_MANDOLIN_FIFO
	nAvailable = MANDOLIN_FIFO_bytes_empty(&SPISharcRxFifo);
	if (nAvailable > SPISharcRxFifo.buffer_bytes)
	{
		gdspWriteDataErrorCount++; // should not happen!!!!
		return false;
	}
	if (nAvailable > nBytesTransferred)
	{
		MANDOLIN_FIFO_enqueue_bytes(&SPISharcRxFifo, tmpRxBuffer, nBytesTransferred);
	}
	else
	{
		printf("Not enough space in Network Mandolin Rx FIFO\n");
	}
	if (SPISharcRxFifo.write_i >= SPISharcRxFifo.buffer_bytes)
	{
		SPISharcRxFifo.write_i  = 0; // should NOT happen
		gdspWriteDataErrorCount++;
		return false;   // transferred did not work.. so now what?

	}


	// Attempt to parse data in FIFO
	ParseMandolinMQX(port_qid, COMM_SPI, &SPISharcRxFifo, 2, TASK_SPISHARC);	// TASK_SYSEVENT for the ReadPendingMessage, WriteMessage TASK_SPKR_CNFG for now put in a temp "task" for debug

#else


	// Check for any returned messages from DSP
	for (i=0;i<length_bytes;i++){
		if (tmpRxBuffer[i] == MANDOLIN_SYNC_WORD){		// look for possible mandolin message
			byte_ptr = &tmpRxBuffer[i];
			bytes_to_process = length_bytes - i;

			break;
		}
	}

	while (bytes_to_process) {
		// Check that received data is a valid mandolin message
		if (*byte_ptr == MANDOLIN_SYNC_WORD) {		//	Probably a mandolin message
			rx_msg = (mandolin_mqx_message_ptr) MSG_ALLOC(message_pool);
			if (!rx_msg)
			{
				return false;

			}

				if (bytes_to_process < MANDOLIN_HEADER_BYTES) { 					// 	Not enough for complete header

					if ((i+bytes_to_process+MANDOLIN_HEADER_BYTES) >= MAX_MANDOLIN_MSG_SIZE)
					{
						nDebugErrorCount++;		// fill with a specific value that never gets writen to

					}
					CallMasterTransferBlocking(FSL_SPI_SHARC,NULL,(byte_ptr+bytes_to_process),MANDOLIN_HEADER_BYTES,500);

					bytes_to_process = MANDOLIN_HEADER_BYTES;
				}
				uint8_t byte;
				// Read in header one byte at a time
				// Read sync
			rx_msg->message.sync = *byte_ptr++;

			if (rx_msg->message.sync != MANDOLIN_SYNC_WORD)
			{

				return false; // something bad happened here
			}
			// Read sequence
			rx_msg->message.sequence = *byte_ptr++;

			// Assemble length
			byte = *byte_ptr++;
			rx_msg->message.length = PACK_SHORT(byte,*byte_ptr++);

			// Read transport flags
			rx_msg->message.transport = *byte_ptr++;

			// Read message ID
			rx_msg->message.id = *byte_ptr++;

			// Assemble header checksum
			byte = *byte_ptr++;;
			rx_msg->message.checksum = PACK_SHORT(byte,*byte_ptr++);
			bytes_to_process -= MANDOLIN_HEADER_BYTES;

			//TODO: Check header checksum for valid length
			uiHeaderChecksum = 0;
			pBytes = &rx_msg->message.sync;
	        for(i=0; i<(MANDOLIN_HEADER_BYTES-2); i++)
	        	  uiHeaderChecksum += *pBytes++;		// add all 12 bytes.. .must be
	        uiHeaderChecksum = ~uiHeaderChecksum;
	        if (uiHeaderChecksum != rx_msg->message.checksum)
	        {
	        	nDebugErrorCount++;
	        	return false;	// end loop
	        }

			if(rx_msg->message.length > 0)						// message contains payload
			{
				rx_msg->message.payload = rx_msg->payload;
				uint16_t payload_bytes = MANDOLIN_BYTES_PER_WORD * (rx_msg->message.length + 1);		// Extra word for checksum
				uint8_t *pPayload = rx_msg->payload;

				if (payload_bytes < bytes_to_process){
					for (i = 0; i< (payload_bytes>>2); i++){
						*pPayload++ = byte_ptr[3];
						*pPayload++ = byte_ptr[2];
						*pPayload++ = byte_ptr[1];
						*pPayload++ = byte_ptr[0];
						byte_ptr+=MANDOLIN_BYTES_PER_WORD;
						bytes_to_process -= MANDOLIN_BYTES_PER_WORD;
					}
				}
				else if (payload_bytes > bytes_to_process) {
					uint16_t bytes_rem = payload_bytes - bytes_to_process;

					if (bytes_rem >= MAX_MANDOLIN_MSG_SIZE)
					{
						nDebugErrorCount++;		// fill with a specific value that never gets writen to
					}

					for (i = 0; i< (bytes_to_process>>2); i++){
						*pPayload++ = byte_ptr[3];
						*pPayload++ = byte_ptr[2];
						*pPayload++ = byte_ptr[1];
						*pPayload++ = byte_ptr[0];
						byte_ptr+=MANDOLIN_BYTES_PER_WORD;
					}

					if(bytes_to_process & 0x3)	{
						pPayload[3] = *byte_ptr++;
						pPayload[2] = *byte_ptr++;
					}

					// Pull the rest of the payload into the message
					CallMasterTransferBlocking(FSL_SPI_SHARC,NULL,byte_ptr,bytes_rem,500);

					if(bytes_to_process & 0x3)	{
						pPayload[1] = *byte_ptr++;
						pPayload[0] = *byte_ptr++;
						pPayload+=4;
					}

					for (i = 0; i< (bytes_rem>>2); i++){
						*pPayload++ = byte_ptr[3];
						*pPayload++ = byte_ptr[2];
						*pPayload++ = byte_ptr[1];
						*pPayload++ = byte_ptr[0];
						byte_ptr+=MANDOLIN_BYTES_PER_WORD;
					}

					bytes_to_process = 0;
				}
			}
			else {
				rx_msg->message.payload = NULL;
			}

			// Presumably we now have a valid mandolin message
			rx_msg->mqx_header.SOURCE_QID = port_qid;
			rx_msg->mqx_header.TARGET_QID = _msgq_get_id(0,CONFIG_QUEUE);
			rx_msg->mqx_header.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(comm_method) + 8 + MANDOLIN_MSG_bytes(&rx_msg->message);
			rx_msg->sender_type = COMM_SPI;

			//TODO: Do we need to _free on error
			//TODO: Do we need to check for free pool msgs
			if (!_msgq_send(rx_msg)) {
				printf("DSP MSGQ error: 0x%lx\n", _task_errno);
				//_task_set_error(MQX_OK); // Reset error so not blocked when overfull
			}

		}else {
			bytes_to_process--;
			byte_ptr++;
		}
	}

#endif
	return true;
}

void SpiSharcPort::ReadPendingMessage(uint32 uiBytesToRead)
{
	_mutex_lock(spi_dsp_mutex);

	TASKDEBUG_POS(TASK_SYSEVENT,0x66)
	dspWriteData(NULL, uiBytesToRead); //SHARC_BYTES_TO_READ);
	TASKDEBUG_POS(TASK_SYSEVENT,0x67)
	_mutex_unlock(spi_dsp_mutex);
}

int gMaxTransferCount = 0;
uint32 SpiSharcPort::CallMasterTransferBlocking(uint32_t instance,
        const uint8_t * sendBuffer,
        uint8_t * receiveBuffer,
        size_t transferByteCount,
        uint32_t timeout)
{
	dspi_status_t result;
	static int nDebugErrorCount = 0;

	if (transferByteCount > MAX_MANDOLIN_MSG_SIZE)
	{
		nDebugErrorCount++;		// fill with a specific value that never gets writen to

	}
	if (transferByteCount > gMaxTransferCount )
	{
		gMaxTransferCount = transferByteCount;
	}

	result = DSPI_DRV_MasterTransferBlocking(instance, sendBuffer, receiveBuffer, transferByteCount, timeout);
#ifndef _SECONDARY_BOOT
	if(kStatus_DSPI_Success != result)	{
		printf("ERR: DSP Write Error: kStatus: %d ... Resetting Driver \n", result);
		dsp_sharc_SetMode_Host();
	}
#endif	//	_SECONDARY_BOOT
	if (kStatus_DSPI_Success == result)
	{
		return transferByteCount;
	}
	else
	{
		return 0;
	}
}

} /* namespace oly */


