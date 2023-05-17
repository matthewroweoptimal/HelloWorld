/**
* File     : uart_transport.c
* Created  : October 2014
* Updated  : 2014/10/27
* Author   : Chamira Perera
* Synopsis : HostCPU Transport (physical) Interface PC UART implementation
*
* This is an implementation of the SPI / UART peripheral interface on the Host CPU
*
* Copyright 2005-2014, Audinate Pty Ltd and/or its licensors
* Confidential
* Audinate Copyright Header Version 1
*/

#include <unistd.h>
#include <errno.h>
#include <hostcpu_transport.h>
#include <uhip_structures.h>
#include <sys/time.h>
//#include "fsl_uart_hal.h"
//#include "fsl_os_abstraction.h"
#include "uart_ultimo.h"
#define PC_COM_PORT 1


static int g_nUltimoPort = -1;


void hostcpu_transport_cleanup()
{
	g_nUltimoPort = -1;
}

aud_bool_t hostcpu_transport_init(int nPort)
{
	g_nUltimoPort = nPort;
	return ((-1!=g_nUltimoPort)?AUD_TRUE:AUD_FALSE);
}

/**
* Write bytes to the transport interface
* @param buffer [in] Pointer to the buffer to write
* @param num_bytes [in] The number of bytes to write
* @return the number of bytes written
*/
uint16_t hostcpu_transport_write(uint8_t const * buffer, uint16_t num_bytes)
{
	uint32_t i;
	uint32_t chunks = num_bytes / UHIP_CHUNK_SIZE;
//	uint32_t num_bytes_written;
	int nByte;

	if (!num_bytes) {
		return 0;
	}

	if (-1==g_nUltimoPort) {
		return 0;
	}

	if (chunks)
	{
		for (i = 0; i < chunks; ++i)
		{
#ifdef DANTE_DEBUG_FINE
			printf("UHIP:Write COBs chunk (x%x bytes):\n", num_bytes);
			for(nByte=0;nByte<UHIP_CHUNK_SIZE;nByte++)
			{
				printf("%02X,", (unsigned int)buffer[(i * UHIP_CHUNK_SIZE)+nByte]);
			}
			printf("\n");
#endif	//	 DANTE_DEBUG_FINE

#ifdef ULTIMO_UART_EDMA
			UART_DRV_EdmaSendDataBlocking(g_nUltimoPort, &buffer[i * UHIP_CHUNK_SIZE], UHIP_CHUNK_SIZE, OSA_WAIT_FOREVER);
#else	//	ULTIMO_UART_EDMA
		//	UART_DRV_SendDataBlocking(g_nUltimoPort, &buffer[i * UHIP_CHUNK_SIZE], UHIP_CHUNK_SIZE, OSA_WAIT_FOREVER);
#endif	//	ULTIMO_UART_EDMA

			//IQ - attempt to direct code the sending of one chunck out the nuvoton uart.

			for(nByte=0;nByte<UHIP_CHUNK_SIZE;nByte++)
			{
            while(UART_IS_TX_FULL(UART1));  /* Wait Tx is not full to transmit data */
            UART_WRITE(UART1, buffer[(i * UHIP_CHUNK_SIZE)+nByte]);
			}



//			if (num_bytes_written>UHIP_CHUNK_SIZE)
//			{
//				printf("Unable to write data\n");
//				return 0;
//			}
//			if (num_bytes_written == 0)
//			{
//				printf("Did not write any data to the serial port\n");
//				return 0;
//			}
		}
	}
	else
	{
#ifdef DANTE_DEBUG_FINE
		printf("UHIP:Write COBs packet (x%x bytes):\n", num_bytes);
		for(nByte=0;nByte<UHIP_CHUNK_SIZE;nByte++)
		{
			printf("%02X,", (unsigned int)buffer[(i * UHIP_CHUNK_SIZE)+nByte]);
		}
		printf("\n");
#endif	//	 DANTE_DEBUG_FINE
//		num_bytes_written = write(g_nUltimoPort, buffer, num_bytes);
		//UART_DRV_SendDataBlocking(g_nUltimoPort, buffer, num_bytes, OSA_WAIT_FOREVER);
//		if (num_bytes_written>UHIP_CHUNK_SIZE)
//		{
//			printf("Unable to write data, errno = %d\n", errno);
//			return 0;
//		}
//		if (num_bytes_written == 0)
//		{
//			printf("Did not write any data to the serial port\n");
//			return 0;
//		}
	}


	return num_bytes;
}

