/*
 * Parameters.c
 *
 *  Created on: May 20, 2015
 *      Author: Kris.Simonsen
 */
#include <stdio.h>
#include "InputHandler.h"
//#include "ftm_encoder.h"
#include "pin.h"
#include "MQX_to_FreeRTOS.h"
//#include "lwevent.h"

LWEVENT_STRUCT 	user_event;
LWEVENT_STRUCT	sys_event;

#if 1
static oly_inputs_t oly_inputs;
static oly_inputs_t oly_inputs_last;
static oly_inputs_sw_timers_t oly_inputs_sw_timers = {0,0};

static uint8_t amp_fault = 0;
static uint8_t amp_clip = 0;

int16_t enc_pos = 0;
int16_t enc_last_pos = 0;

void PORTA_IRQHandler()
{
#ifdef SC_COMMENTED_OUT
	GPIO_DRV_ClearPinIntFlag(SHARC_SPI_READY);
//	if(GPIO_DRV_ReadPinInput(SHARC_SPI_READY))
		_lwevent_set(&sys_event, event_dsp_tx_ready);
//	else
//		_lwevent_clear(&sys_event, event_dsp_tx_ready);
#endif // SC_COMMENTED_OUT
}

uint8_t init_inputs(void){

	/* Read current state of all GPIO inputs */
	oly_inputs.Buttons.bits.SW2 = GPIO_DRV_ReadPinInput(SW2);
	//oly_inputs.Buttons.bits.EncoderSW = GPIO_DRV_ReadPinInput(UI_PB_1);
	oly_inputs.Logic.bits.amp1_ch1_clip = GPIO_DRV_ReadPinInput(AMP1_CH1_CLIP);
	oly_inputs.Logic.bits.amp1_ch2_clip = GPIO_DRV_ReadPinInput(AMP1_CH2_CLIP);
	oly_inputs.Logic.bits.amp2_ch1_clip = GPIO_DRV_ReadPinInput(AMP2_CH1_CLIP);
	oly_inputs.Logic.bits.amp2_ch2_clip = GPIO_DRV_ReadPinInput(AMP2_CH2_CLIP);
	oly_inputs.Logic.bits.amp1_fault = GPIO_DRV_ReadPinInput(AMP1_PROTECT);
	oly_inputs.Logic.bits.amp2_fault = GPIO_DRV_ReadPinInput(AMP2_PROTECT);
	oly_inputs.Logic.bits.dante_mute = GPIO_DRV_ReadPinInput(DANTE_MUTE);		//IQ - ULTIMO_PDP_UNLOCKED

	oly_inputs_last = oly_inputs;

	return oly_inputs.Buttons.reg;
}

void amp_fault_state_changed(void)
{
	amp_fault = !oly_inputs.Logic.bits.amp1_fault | !oly_inputs.Logic.bits.amp2_fault;
	_lwevent_set(&sys_event, event_amp_fault);
}

void amp_clip_state_changed(void)
{
	amp_clip = !oly_inputs_last.Logic.bits.amp1_ch1_clip | !oly_inputs_last.Logic.bits.amp1_ch2_clip | !oly_inputs_last.Logic.bits.amp2_ch1_clip | !oly_inputs_last.Logic.bits.amp2_ch2_clip;
	_lwevent_set(&sys_event, event_amp_clip);
}

#ifdef FREERTOS_CONFIG_H
	void TriggerIdentifyStop( _timer_id id )
#else
	void TriggerIdentifyStop( _timer_id id, void * data_ptr, MQX_TICK_STRUCT_PTR tick_ptr )
#endif
{
	_lwevent_set( &user_event, event_IdentifyStop );
}

void EngageSpeakerProfile( _timer_id id, void * data_ptr, MQX_TICK_STRUCT_PTR tick_ptr )
{
	_lwevent_set( &user_event, event_EngageProfile );
}

void OptimizingDone( _timer_id id, void * data_ptr, MQX_TICK_STRUCT_PTR tick_ptr )
{
	_lwevent_set( &user_event, event_OptimizingDone );
}

void UIHandleAlertScreen( _timer_id id, void * data_ptr, MQX_TICK_STRUCT_PTR tick_ptr )
{
	_lwevent_set( &sys_event, event_AlertScreenTimeout );
}

#ifdef FREERTOS_CONFIG_H
	void TriggerDSPError( _timer_id id )
#else
	void TriggerDSPError( _timer_id id, void * data_ptr, MQX_TICK_STRUCT_PTR tick_ptr )
#endif
{
	_lwevent_set( &sys_event, event_DSPError );
}
void poll_inputs(void){

	/* Read current state of all GPIO inputs */
	oly_inputs.Buttons.bits.SW2 = GPIO_DRV_ReadPinInput(SW2);
	//oly_inputs.Buttons.bits.EncoderSW = GPIO_DRV_ReadPinInput(UI_PB_1);
	oly_inputs.Logic.bits.amp1_ch1_clip = GPIO_DRV_ReadPinInput(AMP1_CH1_CLIP);
	oly_inputs.Logic.bits.amp1_ch2_clip = GPIO_DRV_ReadPinInput(AMP1_CH2_CLIP);
	oly_inputs.Logic.bits.amp2_ch1_clip = GPIO_DRV_ReadPinInput(AMP2_CH1_CLIP);
	oly_inputs.Logic.bits.amp2_ch2_clip = GPIO_DRV_ReadPinInput(AMP2_CH2_CLIP);
	oly_inputs.Logic.bits.amp1_fault = GPIO_DRV_ReadPinInput(AMP1_PROTECT);
	oly_inputs.Logic.bits.amp2_fault = GPIO_DRV_ReadPinInput(AMP2_PROTECT);
	oly_inputs.Logic.bits.dsp_tx_ready = GPIO_DRV_ReadPinInput(SHARC_SPI_READY);
	oly_inputs.Logic.bits.dante_mute = GPIO_DRV_ReadPinInput(DANTE_MUTE);


	/* Create events for any new changes */
#if USE_CDD_UI
	/* keep track of long button presses for SW2 for CDDL*/
	if(oly_inputs.Buttons.bits.SW2 == 0)
		oly_inputs_sw_timers.SW2++;  						//don't worry about rollover -it is years away
	if(oly_inputs_sw_timers.SW2 == LONG_PRESS_TIMEOUT){		//activate long press without release
		_lwevent_set(&user_event, event_SW2_LONG);
	}
	if(oly_inputs_sw_timers.SW2 == LONG_LONG_PRESS_TIMEOUT){		//activate long long press without release
		_lwevent_set(&user_event, event_SW2_LONG_LONG);
	}

	if (oly_inputs.Buttons.bits.SW2 != oly_inputs_last.Buttons.bits.SW2){
		oly_inputs_last.Buttons.bits.SW2 = oly_inputs.Buttons.bits.SW2;
		if (oly_inputs_last.Buttons.bits.SW2 == 1){			// Only interested in short release, not press
			if((oly_inputs_sw_timers.SW2 < LONG_PRESS_TIMEOUT) || (oly_inputs_sw_timers.SW2 >= LONG_LONG_PRESS_TIMEOUT))
				_lwevent_set(&user_event, event_SW2);
			oly_inputs_sw_timers.SW2 = 0;
		}
	}
#else
	//short press only for normal UI
	if (oly_inputs.Buttons.bits.SW2 != oly_inputs_last.Buttons.bits.SW2){
		oly_inputs_last.Buttons.bits.SW2 = oly_inputs.Buttons.bits.SW2;
		if (oly_inputs_last.Buttons.bits.SW2 == 0){			// Only interested in press, not release
			amp_fault = !amp_fault;
			_lwevent_set(&user_event, event_SW2);
		}
	}
#endif

	if (oly_inputs.Buttons.bits.EncoderSW != oly_inputs_last.Buttons.bits.EncoderSW){
		oly_inputs_last.Buttons.bits.EncoderSW = oly_inputs.Buttons.bits.EncoderSW;
		if (oly_inputs_last.Buttons.bits.EncoderSW == 0){			// Only interested in press, not release
			_lwevent_set(&user_event, event_EncoderSW);
		}
	}

	if (oly_inputs.Logic.bits.amp1_ch1_clip != oly_inputs_last.Logic.bits.amp1_ch1_clip){
		oly_inputs_last.Logic.bits.amp1_ch1_clip = oly_inputs.Logic.bits.amp1_ch1_clip;
		amp_clip_state_changed();
	}

	if (oly_inputs.Logic.bits.amp1_ch2_clip != oly_inputs_last.Logic.bits.amp1_ch2_clip){
		oly_inputs_last.Logic.bits.amp1_ch2_clip = oly_inputs.Logic.bits.amp1_ch2_clip;
		amp_clip_state_changed();
	}

	if (oly_inputs.Logic.bits.amp2_ch1_clip != oly_inputs_last.Logic.bits.amp2_ch1_clip){
		oly_inputs_last.Logic.bits.amp2_ch1_clip = oly_inputs.Logic.bits.amp2_ch1_clip;
		amp_clip_state_changed();
	}

	if (oly_inputs.Logic.bits.amp2_ch2_clip != oly_inputs_last.Logic.bits.amp2_ch2_clip){
		oly_inputs_last.Logic.bits.amp2_ch2_clip = oly_inputs.Logic.bits.amp2_ch2_clip;
		amp_clip_state_changed();
	}

	if (oly_inputs.Logic.bits.amp1_fault != oly_inputs_last.Logic.bits.amp1_fault){
		oly_inputs_last.Logic.bits.amp1_fault = oly_inputs.Logic.bits.amp1_fault;
		amp_fault_state_changed();
	}

#ifdef USE_CDD_UI
	if (oly_inputs.Logic.bits.amp2_fault != oly_inputs_last.Logic.bits.amp2_fault){
		oly_inputs_last.Logic.bits.amp2_fault = oly_inputs.Logic.bits.amp2_fault;
		amp_fault_state_changed();
	}
#endif

	if (oly_inputs.Logic.bits.dante_mute != oly_inputs_last.Logic.bits.dante_mute){
		oly_inputs_last.Logic.bits.dante_mute = oly_inputs.Logic.bits.dante_mute;
		//printf("Mute pin state = %d\n", oly_inputs.Logic.bits.dante_mute);
		_lwevent_set(&sys_event, event_dsp_mute);
	}
}

void poll_encoder(void)
{
#ifdef SC_COMMENTED_OUT
	enc_pos = FTM_DRV_CounterRead(FSL_ENCODER_FTM2);

	if ( (enc_pos > enc_last_pos) && (enc_pos - enc_last_pos > 2) ){
		if (enc_pos & 1 && 1){
			_lwevent_set(&user_event, event_EncoderR);
			enc_last_pos = enc_pos;
		}
	}
	else if ( (enc_pos < enc_last_pos) && (enc_last_pos - enc_pos > 2) ){
		if (enc_pos & 1 && 1){
			_lwevent_set(&user_event, event_EncoderL);
			enc_last_pos = enc_pos;
		}
	}
#endif // SC_COMMENTED_OUT
}


uint8_t get_amp_fault_state(void)
{
	return amp_fault;
}

uint8_t get_amp_clip_state(void)
{
	return amp_clip;
}

uint8_t get_dante_mute_state(void)
{
	return oly_inputs.Logic.bits.dante_mute;
}

#endif // 0
