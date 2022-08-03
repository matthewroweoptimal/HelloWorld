/*
 * Parameters.h
 *
 *  Created on: May 20, 2015
 *      Author: Kris.Simonsen
 */

#ifndef SOURCES_INPUTHANDLER_H_
#define SOURCES_INPUTHANDLER_H_

#define EVENTS_ALL 		0xFF

#include "MQX_To_FreeRTOS.h"

#ifdef _cplusplus
extern "C" {
#endif

#include "oly.h"
//#include "lwevent.h"
#include <timer.h>
#ifdef _cplusplus
}
#endif

#define TIMEOUT_DSP_ERROR		30		// in seconds

typedef enum {			 // bit mask
	event_SW2 			= 0x01,
	event_EncoderSW 	= 0x02,
	event_EncoderL		= 0x04,
	event_EncoderR		= 0x08,
	event_IdentifyStop	= 0x10,
	event_SW2_LONG		= 0x20,
	event_EngageProfile	= 0x40,
	event_OptimizingDone	= 0x80,
	event_brightness_change = 0x100,
	event_SW2_LONG_LONG		= 0x200,

} oly_user_events_t;

typedef enum {			 // bit mask
    event_amp_clip				= 0x01,
    event_amp_fault				= 0x02,
	event_dsp_tx_ready			= 0x04,
	event_net_connect			= 0x08,
	event_net_disconnect		= 0x10,
	event_AlertScreenTimeout	= 0x20,
	event_DSPError				= 0x40,
	event_dsp_mute				= 0x80,
} oly_system_events_t;

typedef struct {
	union {
		struct {
			uint8_t 	SW2:1;
			uint8_t		EncoderSW:1;
			uint8_t		nc:6;
		} bits;
		uint8_t reg;
	} Buttons;
	union {
		struct{
			uint8_t		amp1_ch1_clip:1;
			uint8_t		amp1_ch2_clip:1;
			uint8_t		amp2_ch1_clip:1;
			uint8_t		amp2_ch2_clip:1;
			uint8_t		amp1_fault:1;
			uint8_t		amp2_fault:1;
			uint8_t		dsp_tx_ready:1;
			uint8_t		dante_mute:1;
		} bits;
		uint8_t	reg;
	} Logic;
} oly_inputs_t;

typedef struct {
	uint32_t 	SW2;
	uint32_t	EncoderSW;
} oly_inputs_sw_timers_t;
//the polling sleep is 20 this seems to be 2ms per tick?
#define LONG_PRESS_TIMEOUT (2000/20)
#define LONG_LONG_PRESS_TIMEOUT (10000/20)

uint8_t init_inputs(void);
void poll_inputs(void);
void poll_encoder(void);

#ifdef FREERTOS_CONFIG_H
	void TriggerIdentifyStop( _timer_id id );
#else
	void TriggerIdentifyStop( _timer_id id, void * data_ptr, MQX_TICK_STRUCT_PTR tick_ptr );
#endif
void EngageSpeakerProfile( _timer_id id, void * data_ptr, MQX_TICK_STRUCT_PTR tick_ptr );
void OptimizingDone( _timer_id id, void * data_ptr, MQX_TICK_STRUCT_PTR tick_ptr );
void UIHandleAlertScreen( _timer_id id, void * data_ptr, MQX_TICK_STRUCT_PTR tick_ptr );
#ifdef FREERTOS_CONFIG_H
	void TriggerDSPError( _timer_id id );
#else
	void TriggerDSPError( _timer_id id, void * data_ptr, MQX_TICK_STRUCT_PTR tick_ptr );
#endif

uint8_t get_amp_fault_state();
uint8_t get_amp_clip_state();

uint8_t get_dante_mute_state();

extern LWEVENT_STRUCT user_event;
extern LWEVENT_STRUCT sys_event;

#endif /* SOURCES_INPUTHANDLER_H_ */
