/*
 * SpeakerConfiguration.c
 *
 *  Created on: Nov 17, 2015
 *      Author: Althaf.Ismail
 */

#include "SpeakerConfiguration.h"
#include "oly.h"
#include <stdio.h>
#include "MQX_To_FreeRTOS.h"

void * event_spkr_cnfg_p;
static uint8_t danteSource = 0;

void OverrideAudioSource(uint8_t chStatus) {
	switch (chStatus) {
	case 0:
//		if (_event_set(event_spkr_cnfg_p, event_select_audio_src_none) != MQX_OK)
			printf("\nEvent Set Error\n");
		break;
	case 1:
//		if (_event_set(event_spkr_cnfg_p, event_select_audio_src_anlg) != MQX_OK)
			printf("\nEvent Set Error\n");
		break;
	case 2:
//		if (_event_set(event_spkr_cnfg_p, event_select_audio_src_dnte) != MQX_OK)
			printf("\nEvent Set Error\n");
		break;
	case 3:
//		if (_event_set(event_spkr_cnfg_p, event_select_audio_src_pink) != MQX_OK)
			printf("\nEvent Set Error\n");
		break;
	default:
//		if (_event_set(event_spkr_cnfg_p, event_select_audio_src_anlg) != MQX_OK)
			printf("\nEvent Set Error\n");
		break;
	}
}

void AutoSelectAudioSource(uint8_t chStatus) {
	if (danteSource != chStatus)
		danteSource = chStatus;

	if (chStatus == 0x9 || chStatus == 0xa) {
//		if (_event_set(event_spkr_cnfg_p, event_select_audio_src_dnte) != MQX_OK)
			printf("\nEvent Set Error\n");
	} else {
//		if (_event_set(event_spkr_cnfg_p, event_select_audio_src_anlg) != MQX_OK)
			printf("\nEvent Set Error\n");
	}
}

uint8_t GetDanteAudioSource(void) {

	if (danteSource == 0x9 || danteSource == 0xa)
		return src_dante;
	else
		return src_analog;
}
