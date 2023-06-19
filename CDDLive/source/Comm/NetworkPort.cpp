/*
 * NetworkPort.cpp
 *
 *  Created on: Oct 1, 2015
 *      Author: Kris.Simonsen
 */

#ifndef _NETWORKPORT_CPP_
#define _NETWORKPORT_CPP_

#include <NetworkPort.h>

#include "network.h"
#include "CommMandolin.h"

extern "C" {
//#include "gpio_pins.h"
//#include "uart_voice.h"
}

#include "timekeeper.h"

#include <stdio.h>


#include "os_tasks.h"

namespace oly {

NetworkPort::NetworkPort(): MandolinPort(NULL) {
	port_qid = _msgq_open((_queue_number)NET_TX_QUEUE, 0);
}

void NetworkPort::WriteMessage(mandolin_message * pMsg, bool bRequireConnection) {
	if (pMsg && IsPortOpen() && ((!bRequireConnection) || GetConnected())) {

		mandolin_mqx_msg_packed_ptr 	mqx_msg_ptr;


		mqx_msg_ptr = (mandolin_mqx_msg_packed_ptr) MSG_ALLOC(message_pool);


		if (mqx_msg_ptr)
		{
			mqx_msg_ptr->mqx_header.SOURCE_QID = port_qid;
			mqx_msg_ptr->mqx_header.TARGET_QID = _msgq_get_id(0,TCP_QUEUE);
			mqx_msg_ptr->mqx_header.SIZE = sizeof(comm_method) + 8 + MANDOLIN_MSG_bytes(pMsg);
			mqx_msg_ptr->sender_type = COMM_ETHERNET_TCP;
//			printf("WriteMessage( Id %d )\n", pMsg->id);

			if (pMsg->length > MAX_MANDOLIN_MSG_SIZE)
			{
				MSG_FREE(_msgq_get_id(0,TCP_QUEUE), mqx_msg_ptr);

			}
			else
			{
				mqx_msg_ptr->length = MANDOLIN_MSG_write(mqx_msg_ptr->data, pMsg);

				if (!MSGQ_SEND(_msgq_get_id(0,TCP_QUEUE), mqx_msg_ptr)) {
					printf("Network MSGQ error: 0x%lx\n", _task_errno);
					_task_set_error(MQX_OK); // Reset error so not blocked when overfull

					 MSG_FREE(_msgq_get_id(0,TCP_QUEUE), mqx_msg_ptr);

				}
			}
		}
		else
		{
			_mqx_uint err = _task_get_error();
//			printf("!Network Output:_msg_alloc failed with error 0x%X, dumping message!\n", (unsigned int)err);
		}

	} else {
		//printf("Network WRITE error: Attempted to write while not connected\n");
	}
}

void NetworkPort::Run(int nMs)
{
	MandolinPort::Run(nMs);

//	if (mcsDisconnected!=m_MandolinConnectState)
//		printf("lps:%d,to=%d\r\n", m_uiStateTimeElapsed, m_uiStateTimeoutMs);
	//	Only do time out processing for network - don't want to break anything
	if ((mcsDisconnected!=m_MandolinConnectState) && (mcsConnected!=m_MandolinConnectState) && (m_uiStateTimeElapsed>m_uiStateTimeoutMs))
	{
		printf("Connection: State change timeout!\n");
		SetForceClose();	//	Disconnect
	}
#if !MFG_TEST_EAW && !MFG_TEST_MARTIN
	// Enforce Mandolin Connection
	if (((mcsDisconnected==m_MandolinConnectState) || (mcsConnected==m_MandolinConnectState)) && (m_uiIdleTimeElapsed>m_uiIdleTimeoutMs))
	{
		printf("Connection: Idle timeout!\n");
		SetForceClose();	//	Disconnect
	}
#endif
}

//	Set up meters
void NetworkPort::ConfigureMeters(int nAutoMetersListId, int nAutoMetersRate, int nAutoMetersFlags, bool bAutoMetersNoAck)
{
	MandolinPort::ConfigureMeters(nAutoMetersListId, nAutoMetersRate, nAutoMetersFlags, bAutoMetersNoAck);

	int nList = nAutoMetersListId - 1;
	// Timer already running for this Parameter, kill it first
	if (m_SubscriptionTimer[nList])
	{
		_timer_cancel(m_SubscriptionTimer[nList]);
		m_SubscriptionTimer[nList] = 0;
	}

	// If period_ms is 0, don't start timer
	if( m_nAutoMetersRate[nList] )
	{
		_time_init_ticks(&m_SubscriptionTicks[nList], 0);
		_time_add_msec_to_ticks(&m_SubscriptionTicks[nList], m_nAutoMetersRate[nList]);
		m_SubscriptionTimer[nList] = _timer_start_periodic_every_ticks(NetworkSubscriptionUpdater, m_SubscriptionTimer, TIMER_ELAPSED_TIME_MODE, &m_SubscriptionTicks[nList]);
	}
}

} /* namespace oly */

#endif /* _NETWORKPORT_CPP_ */
