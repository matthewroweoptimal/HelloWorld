/*
 * uarts.h
 *
 *  Created on: Aug 7, 2015
 *      Author: Kris.Simonsen
 */

#ifndef _UARTS_H_
#define _UARTS_H_

//	TW@@@ - Removed for initial compile
//#include "Events.h"

#define BUFFER_SIZE				1024		// power of 2 req.

#define FIFO_MASK				(BUFFER_SIZE-1u)
#define FIFO_RESET(fifo)		{fifo.write_idx = fifo.read_idx = 0;}
#define FIFO_IS_EMPTY(fifo)		(fifo.write_idx == fifo.read_idx)
#define FIFO_IS_FULL(fifo)		((FIFO_MASK & fifo.read_idx) == (FIFO_MASK & (fif.write_idx+1))
#define FIFO_WRITE(fifo, byte)	{fifo.data[FIFO_MASK & fifo.write_idx++] = byte;}
#define FIFO_READ(fifo, byte)	{fifo.read_idx++; byte = fifo.data[FIFO_MASK & (fifo.read_idx-1)];}
#define FIFO_COUNT(fifo)		(FIFO_MASK & (fifo.write_idx - fifo.read_idx))

typedef struct {
	uint8_t 	data[BUFFER_SIZE];
	uint32_t	write_idx;
	uint32_t	read_idx;
} ring_buffer_t;

extern ring_buffer_t ultimo_rx_fifo;

#endif /* _UARTS_H_ */
