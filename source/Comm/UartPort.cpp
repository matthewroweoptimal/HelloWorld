/*
 * UartPort.cpp
 *
 *  Created on: Jul 1, 2015
 *      Author: Kris.Simonsen
 */

#ifndef SOURCES_UARTPORT_CPP_
#define SOURCES_UARTPORT_CPP_


#include <UartPort.h>

extern "C" {
//#include "gpio_pins.h"
//#include "uart_voice.h"
//#include "timekeeper.h"
}

extern mandolin_fifo UartVoiceRxFifo;

#include <stdio.h>

namespace oly {

UartPort::UartPort(mandolin_fifo * rx_fifo) : MandolinPort(rx_fifo)
{
	tmpMsg.message.payload = tmpMsg.payload;
	port_qid = _msgq_open(UART_QUEUE, 0);
	m_bPortOpen = true;	// Port is always open to UART
}

void UartPort::WriteMessage(mandolin_message * pMsg, bool bRequireConnection) {
	if (pMsg) {
		uint16_t message_length_bytes = MANDOLIN_MSG_write((uint8_t*)&tmpBuffer,pMsg);	// Uses extra memory but needed to correct byte order for serial port.
//		printf("Writing COMM msg of length %d\n", message_length_bytes);
//		UART_DRV_SendDataBlocking(FSL_UART_VOICE, tmpBuffer,message_length_bytes,OSA_WAIT_FOREVER);
	}
}

void UartPort::ProcessRxFifo(int nTaskId)
{
	ParseMandolinMQX(port_qid, COMM_RS232_UART, RxFifo, 2, nTaskId);  // called in GUI_task
}

#ifndef _SECONDARY_BOOT
//	Set up meters
void UartPort::ConfigureMeters(int nAutoMetersListId, int nAutoMetersRate, int nAutoMetersFlags, bool bAutoMetersNoAck)
{
	MandolinPort::ConfigureMeters(nAutoMetersListId, nAutoMetersRate, nAutoMetersFlags, bAutoMetersNoAck);

	int nList = nAutoMetersListId - 1;
	// Timer already running for this Parameter, kill it first
	if (m_SubscriptionTimer[nList])
	{
		_timer_cancel(m_SubscriptionTimer[nList]);
	}

	// If period_ms is 0, don't start timer
	if( m_nAutoMetersRate[nList])
	{
		_time_init_ticks(&m_SubscriptionTicks[nList], 0);
		_time_add_msec_to_ticks(&m_SubscriptionTicks[nList], m_nAutoMetersRate[nList]);
//		m_SubscriptionTimer[nList] = _timer_start_periodic_every_ticks(VoicingSubscriptionUpdater, m_SubscriptionTimer, TIMER_ELAPSED_TIME_MODE, &m_SubscriptionTicks[nList]);
	}
}
#endif	//	_SECONDARY_BOOT


} /* namespace oly */

#endif /* SOURCES_UARTPORT_CPP_ */
