/*
 * TimeKeeper.c
 *
 *  Created on: Nov 11, 2015
 *      Author: Althaf.Ismail
 */

#include <stdio.h>
#include "TimeKeeper.h"
#include "AmpMonitor.h"
//#include "gpio_pins.h"
#include "Board.h"
//#include "fsl_wdog_driver.h"
#include "OLYspeaker1_pids.h"
#if	USE_CDD_UI
#include "cddl_leds.h"
#endif

LWEVENT_STRUCT timer_event;

int g_nDeferredRebootDowncount = 0;

//	perform background timer-based actions
void HeartbeatTimer(_timer_id id, void * data_ptr, MQX_TICK_STRUCT_PTR tick_ptr)
{
	//	Toggle LEDs every tick (500 ms)
#ifdef SC_COMMENTED_OUT
	GPIO_DRV_TogglePinOutput(DEBUG_LED_A);
#endif // SC_COMMENTED_OUT

	//	Check if needs to be rebooted
	if(g_nDeferredRebootDowncount)
	{
		g_nDeferredRebootDowncount -= TIMEOUT_HEARTBEAT;
		if (g_nDeferredRebootDowncount<=0)
		{
			g_nDeferredRebootDowncount = 0;

#ifdef SC_COMMENTED_OUT
			//	Soft Reset Now
			WDOG_DRV_ResetSystem();
#endif // SC_COMMENTED_OUT
		}
	}
#if	USE_CDD_UI
	//cddl_standby_500ms_timer_callback();
#endif
}

//	Schedule reboot
void DeferredReboot(int nMs)
{
	g_nDeferredRebootDowncount = nMs;
}



void CheckAmps( _timer_id id, void * data_ptr, MQX_TICK_STRUCT_PTR tick_ptr )
{
	_lwevent_set( &timer_event, event_timer_amp_temp_update );
}



void TriggerFlashWrite( _timer_id id, void * data_ptr, MQX_TICK_STRUCT_PTR tick_ptr )
{
	_lwevent_set( &timer_event, event_timer_flashwrite_expired );
}


void NetworkSubscriptionUpdater( _timer_id id, void * data_ptr, MQX_TICK_STRUCT_PTR tick_ptr )
{
	//	eventually needs to be multi-port, now just multi-list

	uint32_t tmpID = *(uint32_t*)&id;
	uint32_t * pSubsciptionArray = (uint32_t*)data_ptr;
	uint8_t ix;

	int nLists = 2;

	// Check which subscription timer ID has expired //
	for(ix=0; ix<nLists; ix++) {
		if( *pSubsciptionArray++ == tmpID ) {
			break;
		}
	}
	_lwevent_set( &timer_event, event_timer_subscription_Network1<<ix );
}

void VoicingSubscriptionUpdater( _timer_id id, void * data_ptr, MQX_TICK_STRUCT_PTR tick_ptr )
{

	uint32_t tmpID = *(uint32_t*)&id;
	uint32_t * pSubsciptionArray = (uint32_t*)data_ptr;
	uint8_t ix;

	int nLists = 2;

	// Check which subscription timer ID has expired //
	for(ix=0; ix<nLists; ix++) {
		if( *pSubsciptionArray++ == tmpID ) {
			break;
		}
	}

	_lwevent_set( &timer_event, event_timer_subscription_Voicing1<<ix );
}

#ifdef FREERTOS_CONFIG_H
	void TriggerSelfTestSwitch( _timer_id id )
#else
	void TriggerSelfTestSwitch( _timer_id id, void * data_ptr, MQX_TICK_STRUCT_PTR tick_ptr )
#endif
{
	_lwevent_set( &timer_event, event_timer_self_test_switch );
}

void UIBackToDefaultScreen( _timer_id id, void * data_ptr, MQX_TICK_STRUCT_PTR tick_ptr )
{
	_lwevent_set( &timer_event, event_timer_backpanel_ui_timeout );
}
