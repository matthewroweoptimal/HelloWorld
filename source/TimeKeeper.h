/*
 * TimeKeeper.h
 *
 *  Created on: Oct 20, 2015
 *      Author: Althaf.Ismail
 */

#ifndef SOURCES_TIMEKEEPER_H_
#define SOURCES_TIMEKEEPER_H_

#include "MQX_To_FreeRTOS.h"
//#include "lwevent.h"
#include <timer.h>

#define TIMEOUT_HEARTBEAT		500		// in milliseconds
#define TIMEOUT_AMPMONITORING	2000	// in milliseconds
#define TIMEOUT_FLASHWRITE		1
#define TIMEOUT_SELFTEST		2000	// in milliseconds

typedef enum	{
	event_timer_flashwrite_start			= 0x00000001,
	event_timer_flashwrite_expired			= 0x00000002,
	event_timer_amp_temp_update  			= 0x00000004,
	event_timer_subscription_Voicing1		= 0x00000008,
	event_timer_subscription_Voicing2		= 0x00000010,
	event_timer_subscription_Network1		= 0x00000020,	//	Leave space for 10 network ports
	event_timer_subscription_Network2		= 0x00000040,
//	event_timer_subscription_STATUS_FENCE	= 0x20000000,	//	Leave space for 10 network ports
	event_timer_self_test_switch			= 0x40000000,
	event_timer_backpanel_ui_timeout		= 0x80000000,
} oly_timer_events_t;

extern LWEVENT_STRUCT timer_event;

void DeferredReboot(int nMs);
void HeartbeatTimer( _timer_id id, void * data_ptr, MQX_TICK_STRUCT_PTR tick_ptr );
void CheckAmps( _timer_id id, void * data_ptr, MQX_TICK_STRUCT_PTR tick_ptr );
void TriggerFlashWrite( _timer_id id, void * data_ptr, MQX_TICK_STRUCT_PTR tick_ptr );
void NetworkSubscriptionUpdater( _timer_id id, void * data_ptr, MQX_TICK_STRUCT_PTR tick_ptr );
void VoicingSubscriptionUpdater( _timer_id id, void * data_ptr, MQX_TICK_STRUCT_PTR tick_ptr );
#ifdef FREERTOS_CONFIG_H
	void TriggerSelfTestSwitch( _timer_id id );
#else
	void TriggerSelfTestSwitch( _timer_id id, void * data_ptr, MQX_TICK_STRUCT_PTR tick_ptr );
#endif
void UIBackToDefaultScreen( _timer_id id, void * data_ptr, MQX_TICK_STRUCT_PTR tick_ptr );

#endif /* SOURCES_TIMEKEEPER_H_ */
