/*
 * UartPort.h
 *
 *  Created on: Jul 1, 2015
 *      Author: Kris.Simonsen
 */

#ifndef SOURCES_UARTPORT_H_
#define SOURCES_UARTPORT_H_

#include <MandolinPort.h>
#include <CommMandolin.h>

namespace oly {

class UartPort: public MandolinPort
{
protected:
	comm_method					comm_type;
	uint8_t						hw_port;
	mandolin_mqx_message 		tmpMsg;
	uint8_t 					tmpBuffer[MAX_MANDOLIN_MSG_SIZE];
public:
	UartPort(mandolin_fifo * rx_fifo);
	void WriteMessage(mandolin_message * pMsg, bool bRequireConnection=false);	//	set bRequireConnection if mandolin connection process must have completed to send this message (only used for network port currently)
	void ProcessRxFifo(int nId);

#ifndef _SECONDARY_BOOT
	virtual void ConfigureMeters(int nAutoMetersListId, int nAutoMetersRate, int nAutoMetersFlags, bool bAutoMetersNoAck);
#endif	//	_SECONDARY_BOOT

};

} /* namespace oly */




#endif /* SOURCES_UARTPORT_H_ */
