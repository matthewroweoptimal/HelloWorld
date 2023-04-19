/*
 * SpeakerConfiguration.h
 *
 *  Created on: Nov 16, 2015
 *      Author: Althaf.Ismail
 */

#ifndef SOURCES_SPEAKERCONFIGURATION_H_
#define SOURCES_SPEAKERCONFIGURATION_H_

#include "MQX_To_FreeRTOS.h"
#include <stdio.h>

//#include "mqx.h"
//#include "event.h"

typedef enum	{
	event_select_audio_src_none	= 0x01,
	event_select_audio_src_anlg	= 0x02,
	event_select_audio_src_dnte	= 0x04,
	event_select_audio_src_pink	= 0x08,
	event_spkr_cnfg_5			= 0x10,
	event_spkr_cnfg_6			= 0x20,
	event_spkr_cnfg_7			= 0x40,
	event_spkr_cnfg_8			= 0x80,
} spkr_cnfg_event_t;

void OverrideAudioSource( uint8_t chStatus );
void AutoSelectAudioSource( uint8_t chStatus );
uint8_t GetDanteAudioSource( void );

extern void * event_spkr_cnfg_p;

#endif /* SOURCES_SPEAKERCONFIGURATION_H_ */
