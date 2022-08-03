/*
 * MandolinHandle.cpp
 *
 *  Created on: Nov 11, 2015
 *      Author: Kris.Simonsen
 *
 *  Description:
 *	These are Mandolin commands specific to OLY / CDDP.
 *	In production builds test functions may be disabled.
 */
#include "CommMandolin.h"

extern "C" {
#include "flash_params.h"
//#include "TimeKeeper.h"
//#include "lwevent.h"
//#include <timer.h>
#ifndef _SECONDARY_BOOT
//#include "CurrentSense.h"
//#include "AUX_Spi_task.h"
#include "OLYspeaker1_map.h"
#include "Model_Info.h"
#endif	//	_SECONDARY_BOOT
}


#include <stdio.h>
#include <string.h>
#include "ConfigManager.h"
//#include "UltimoPort.h"

#include "Region.h"
#include "oly_logo.h"
#ifndef _SECONDARY_BOOT
//#include "CurrentSense.h"
//#include "IRDA_Task.h"
#endif

extern uint32_t g_CurrentIpAddress;
extern uint8_t g_nLastMeterSeq;
extern bool g_bMeterComplete;

//extern UltimoPort * g_pUltimoPort;

bool b_tx_response_flag = false;

namespace oly {

void Config::MandolinHandle(MandolinPort * srcPort, mandolin_mqx_message * msg_ptr) {

	mandolin_message * pMsg = &msg_ptr->message;
	mandolin_parameter_value * pPayload = (mandolin_parameter_value *)pMsg->payload;

	int eventID = pPayload[0].i;

	srcPort->KickDog();

	if (IS_MANDOLIN_REPLY(pMsg->transport)){
		//TODO: Update ACK tracker

		switch (pMsg->id) {
#ifndef _SECONDARY_BOOT
			case MANDOLIN_MSG_GET_APPLICATION_PARAMETER:
				if (MANDOLIN_EDIT_TARGET(ReadParameterFlags(pMsg)) == eTARGET_METERS) {
					HandleMetersResponse(pMsg);
				} else if (MANDOLIN_EDIT_TARGET(ReadParameterFlags(pMsg)) == eTARGET_FIR) {
					// Enable Rugly etc. to read back FIR settings
					olyNetworkPort.WriteMessage(pMsg, false);
#if !USES_FOUR_IRDA
//					olyVoicingPort.WriteMessage(pMsg);
#endif
				}
				break;
			case MANDOLIN_MSG_SET_APPLICATION_PARAMETER:
			case MANDOLIN_MSG_SET_APPLICATION_STRING:
				srcPort->SetParameterReply(pMsg);
				break;
#endif	//	_SECONDARY_BOOT
			case MANDOLIN_MSG_GET_HARDWARE_INFO:
				srcPort->GetHardwareInfoReply(pMsg);
				break;
			case MANDOLIN_MSG_GET_SYSTEM_STATUS:
				switch(pPayload[0].i) {
				case MANDOLIN_SS_MAX_MSG_WORDS:
					srcPort->GetMaxMsgWordsReply(pMsg);
					break;
				default:
					break;
				}
				break;
			default:
				break;
		}

	} else {

		switch (pMsg->id) {

			case MANDOLIN_MSG_PING:
				srcPort->Ping(pMsg);
				break;
			case MANDOLIN_MSG_GET_SOFTWARE_INFO:
				HandleGetSoftwareInfo(srcPort, pMsg);
				break;
			case MANDOLIN_MSG_GET_HARDWARE_INFO:
				srcPort->WriteMessage(GetHardwareInfoResponse(Region::GetSystemBrand(), Region::GetSystemModel(), Region::GetSystemMAC(), pMsg->sequence));
				break;
			case MANDOLIN_MSG_REBOOT:
				SoftwareReset(srcPort, pMsg);
				break;
			case MANDOLIN_MSG_TRIGGER_SYSTEM_EVENT:
				HandleSystemEvent(srcPort, pMsg);
				break;
			case MANDOLIN_MSG_TEST:
				HandleTestCommand(srcPort, pMsg);
				break;
			case MANDOLIN_MSG_GET_SYSTEM_STATUS:
				HandleGetSystemStatus(srcPort, pMsg);
				break;
#ifndef _SECONDARY_BOOT
			case MANDOLIN_MSG_TRIGGER_APPLICATION_EVENT:
				switch(eventID){
					case MANDOLIN_AE_LOAD_SNAPSHOT:
						SetActivePreset((OLY_target)MANDOLIN_EDIT_TARGET(pPayload[1].u), MANDOLIN_WILD_MAP(pPayload[1].u));
						break;
					case MANDOLIN_AE_SET_AVAILABLE:
						if (pPayload[1].i > 0)
							IdentifyStart(pPayload[1].i, eIDENTIFY_MODE_ADVERTISE);
						else
							IdentifyStop();
						break;
#if USE_CDD_UI
					case MANDOLIN_AE_STORE_SNAPSHOT:
					case MANDOLIN_AE_STORE_USER_EDIT:
						if (olyParams.Device->Active_User == 0) { // sanity check
							memcpy(olyParams.User[1], olyParams.User[0], sizeof(olyStoredParams.Stored_User_Presets[0]));
							memcpy(olyParams.Profile[1], olyParams.Profile[0], sizeof(olyStoredParams.Stored_Profile_Presets[0]));
							//force UI to UP
							SetActivePreset(eTARGET_USER, USER_PRESETS-1);
						}
						break;
#endif
					case MANDOLIN_AE_SET_SELF_TEST_MODE:
						SetSelfTestMode((LOUD_test_mode)pPayload[1].i);
						break;
					case MANDOLIN_AE_START_AUTO_OPTIMIZE:
#if 0
						array_opt_vals_t opt_vals;

						opt_vals.OptMax = pPayload[1].f;
						opt_vals.OptMin = pPayload[2].f;
						opt_vals.OptHeight = pPayload[3].f;
						opt_vals.irda_Active_Profile_instance = olyParams.Device->Active_Profile;

						Controls_AutoOptimize(opt_vals.OptMax, opt_vals.OptMin, opt_vals.OptHeight, optimize_source_network, opt_vals.irda_Active_Profile_instance);
#endif // 0
						break;
					case MANDOLIN_AE_RESET_AUTO_OPTIMIZE:
//						IRDATask_AutoOptimizeError(IRDA_Opti_Clear);
						ClearOptimization();
						break;
					case MANDOLIN_AE_RESTORE_DEFAULTS:
						Controls_RestoreDefaults();
						break;
				}
				srcPort->WriteMessage(GetAckResponseWithData(pMsg, (uint8_t*)&eventID, sizeof(eventID)));
				break;
			case MANDOLIN_MSG_CREATE_PARAMETER_LIST:
				HandleCreateParameterList(srcPort, pMsg);
				break;
			case MANDOLIN_MSG_SUBSCRIBE_APPLICATION_PARAMETER:
				HandleSubscribeParameter(srcPort, pMsg);
				break;
			case MANDOLIN_MSG_SET_APPLICATION_PARAMETER:
				HandleSetApplicationParameter(srcPort, pMsg);
				break;
			case MANDOLIN_MSG_GET_APPLICATION_PARAMETER:
				HandleGetApplicationParameters(srcPort, pMsg);
				break;
			case MANDOLIN_MSG_SET_APPLICATION_STRING:
				HandleSetApplicationString(srcPort, pMsg);
				break;
			case MANDOLIN_MSG_GET_APPLICATION_STRING:
				HandleGetApplicationString(srcPort, pMsg);
				break;
#endif
			case MANDOLIN_MSG_FILE_OPEN:
			case MANDOLIN_MSG_FILE_CLOSE:
			case MANDOLIN_MSG_POST_FILE:
				HandleFileMessage(srcPort, pMsg);
				break;
			default:
				srcPort->WriteMessage(GetErrorResponse(MANDOLIN_ERROR_UNIMPLEMENTED_MSGID,pMsg));
				//olyDspPort.WriteMessage(pMsg);
				break;
		}
	}
}

#ifndef _SECONDARY_BOOT


void Config::HandleMetersResponse(mandolin_message * pMsg)
{
	int i;
	float * pPayload = (float*) pMsg->payload;
	bool limiting_indicator;

	if (g_nLastMeterSeq != pMsg->sequence)
	{
//		printf("Meter response Seq=%d, expecting %d!\n", pMsg->sequence, g_nLastMeterSeq);
	}
	else
	{
		// got a valid meter, reset the DSP error timer
		// if this timer expires, we haven't gotten a meter in 'TIMEOUT_DSP_ERROR' seconds and a DSP error is assumed
		DSPErrorTimer_Reset();
		g_bMeterComplete = true;
	}

	olyNetworkPort.HandleMetersResponse((mandolin_parameter_value *)&pPayload[1]);
#if !USES_FOUR_IRDA
//	olyVoicingPort.HandleMetersResponse((mandolin_parameter_value *)&pPayload[1]);
#endif
//	_lwevent_set( &aux_spi_event, current_sense_new_meter );

	// Keep below printf for debugging //
//	printf("%i.%idB\r", (int)(pPayload[METER_WORD_FOR_LED+1]), ((pPayload[METER_WORD_FOR_LED+1]<0) ? ((int)(-10*(pPayload[METER_WORD_FOR_LED+1]-(int)pPayload[METER_WORD_FOR_LED+1]))) : ((int)(10*(pPayload[METER_WORD_FOR_LED+1]-(int)pPayload[METER_WORD_FOR_LED+1]))) ) );

	//	Update the meter variable used to feed the backpanel UI. It needs peak recalculation after every push to App	//
	if( pPayload[METER_WORD_FOR_LED+1] > olyMeterForUI )	{
		olyMeterForUI = pPayload[METER_WORD_FOR_LED+1];
	}

//	limiting_indicator = (pPayload[METER_WORD_FOR_LED+1] > LIMITER_LED_THRESHOLD_DB)
//			| (pPayload[eMID_OLYspeaker1_HF_KLIMITER_OUT+1] > LIMITER_LED_THRESHOLD_DB)
//			| (pPayload[eMID_OLYspeaker1_MF_KLIMITER_OUT+1] > LIMITER_LED_THRESHOLD_DB)
//			| (pPayload[eMID_OLYspeaker1_LF_KLIMITER_OUT+1] > LIMITER_LED_THRESHOLD_DB);

	limiting_indicator = (pPayload[eMID_OLYspeaker1_GROUP_SOFTLIMIT_GAINREDUCE+1] != 0)
			| (pPayload[eMID_OLYspeaker1_HF_KLIMITER_GAINREDUCE+1] != 0)
			| (pPayload[eMID_OLYspeaker1_MF_KLIMITER_GAINREDUCE+1] != 0)
			| (pPayload[eMID_OLYspeaker1_LF_KLIMITER_GAINREDUCE+1] != 0);

	if (olyStatus.Limiting != limiting_indicator) {
		OnLimiterStatusChanged(limiting_indicator);
	}

	//	Update the meter variable used to feed the Apps. It needs peak recalculation after every push to App	//
	if( pPayload[METER_WORD_FOR_APP+1] > Audio_Level )	{
		Audio_Level = pPayload[METER_WORD_FOR_APP+1];
	}

	//	Update the current Sense meter variables
	if( pPayload[eMID_OLYspeaker1_HF_OUT+1] > olyCurrentSenseMeter[2].f )	{
		olyCurrentSenseMeter[2].f = pPayload[eMID_OLYspeaker1_HF_OUT+1];
	}
	if( pPayload[eMID_OLYspeaker1_MF_OUT+1] > olyCurrentSenseMeter[1].f )	{
		olyCurrentSenseMeter[1].f = pPayload[eMID_OLYspeaker1_MF_OUT+1];
	}
	if( pPayload[eMID_OLYspeaker1_LF_OUT+1] > olyCurrentSenseMeter[0].f )	{
		olyCurrentSenseMeter[0].f = pPayload[eMID_OLYspeaker1_LF_OUT+1];
	}
}

void Config::HandleSetApplicationParameter(MandolinPort * srcPort, mandolin_message * pMsg)
{
	mandolin_parameter_value *	pPID;
	mandolin_parameter_value * 	pValue;
	uint32_t	i, pid_count, flags, group, instance;
	mandolin_error_code error;

	flags    = ReadParameterFlags(pMsg);
	group    = MANDOLIN_EDIT_TARGET(flags);
	instance = MANDOLIN_WILD_MAP(flags);

	pPID   = (mandolin_parameter_value*)pMsg->payload;
	pValue = (mandolin_parameter_value*)pMsg->payload + 2;

	if (IS_MANDOLIN_MULTIPARAMETER(flags)) {
		pid_count = (pMsg->length - 1) >> 1;
	} else {
		pid_count = (pMsg->length - 2);
	}

	/* Check values are within range */
	for(i=0; i<pid_count; i++){

		if (IS_MANDOLIN_MULTIPARAMETER(flags)) {
			if (i == MANDOLIN_FLAG_WORD_OFFSET) { pPID++; }	// Flag word offset

			error = ParamValidate((OLY_target)group, pPID[i*2].i, &pValue[i*2]);
		} else {
			error = ParamValidate((OLY_target)group, pPID[0].i + i, &pValue[i]);
		}

		if (error) {
			srcPort->WriteMessage(GetErrorResponse(error, pMsg));
			printf("SET APPLICATION PARAM message received with error ID: %d\n", error);
			return;
		}
	}

	pPID   = (mandolin_parameter_value*)pMsg->payload;

	switch(group){
		case eTARGET_DEVICE:
			for(i=0; i<pid_count; i++) {
				if (IS_MANDOLIN_MULTIPARAMETER(flags))
				{
					if (i == MANDOLIN_FLAG_WORD_OFFSET) { pPID++; }	// Flag word offset

					printf("Parameter set request for PID: %d Value: %d\n",pPID[i*2], pValue[i*2]);
					ParamSetDevice((OLYspeaker1_DEVICE_pid)pPID[i*2].i, &pValue[i*2]);
				} else {
					ParamSetDevice((OLYspeaker1_DEVICE_pid)(pPID[0].i + i), &pValue[i]);
				}

			}
			srcPort->WriteMessage(GetAckResponse(pMsg));
			#if USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			olyUI.DebugOut((char*)"NetRX: Set App Prm Device");
			#endif	// USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			break;
		case eTARGET_USER:
#if USE_CDD_UI
			if (olyParams.Device->Active_User != 0) {
				SetActivePreset(eTARGET_USER, 0);  //this is done to also handle the UI
			}
			instance = 0; //force instance to internal 0 preset
#endif

			for(i=0; i<pid_count; i++) {
				if (IS_MANDOLIN_MULTIPARAMETER(flags))
				{
					if (i == MANDOLIN_FLAG_WORD_OFFSET) { pPID++; }	// Flag word offset
					ParamSetUser(instance+1,(OLYspeaker1_USER_pid)pPID[i*2].i, pValue[i*2]);
				} else {
					ParamSetUser(instance+1,(OLYspeaker1_USER_pid)(pPID[0].i + i), pValue[i]);
				}
			}
			srcPort->WriteMessage(GetAckResponse(pMsg));
			#if USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			olyUI.DebugOut((char*)"NetRX: Set App Prm User");
			#endif	// USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG

			break;
		case eTARGET_VOICING:
			for(i=0; i<pid_count; i++) {
				if (IS_MANDOLIN_MULTIPARAMETER(flags)) {
					if (i == MANDOLIN_FLAG_WORD_OFFSET) { pPID++; }	// Flag word offset
					ParamSetVoicing(instance,(OLYspeaker1_VOICING_pid)pPID[i*2].i, pValue[i*2].u);
				} else {
					ParamSetVoicing(instance,(OLYspeaker1_VOICING_pid)(pPID[0].i + i), pValue[i].u);
				}
			}
			srcPort->WriteMessage(GetAckResponse(pMsg));
			#if USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			olyUI.DebugOut((char*)"NetRX: Set App Prm Voice");
			#endif	// USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			break;
		case eTARGET_PROFILE:
#if USE_CDD_UI
			if (olyParams.Device->Active_User != 0) {
				SetActivePreset(eTARGET_USER, 0);  //this is done to also handle the UI
			}
			instance = 0; //force instance to internal 0 preset
#endif
			for(i=0; i<pid_count; i++) {
				if (IS_MANDOLIN_MULTIPARAMETER(flags)) {
					if (i == MANDOLIN_FLAG_WORD_OFFSET) { pPID++; }	// Flag word offset
					ParamSetProfile(instance,(OLYspeaker1_PROFILE_pid)pPID[i*2].i, pValue[i*2].u);
				} else {
					ParamSetProfile(instance,(OLYspeaker1_PROFILE_pid)(pPID[0].i + i), pValue[i].u);
				}
			}
			srcPort->WriteMessage(GetAckResponse(pMsg));
			#if USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			olyUI.DebugOut((char*)"NetRX: Set App Prm Profile");
			#endif	// USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			break;
		case eTARGET_THROW:
			for(i=0; i<pid_count; i++) {
				if (IS_MANDOLIN_MULTIPARAMETER(flags)) {
					if (i == MANDOLIN_FLAG_WORD_OFFSET) { pPID++; }	// Flag word offset
					ParamSetThrow(instance,(OLYspeaker1_THROW_pid)pPID[i*2].i, pValue[i*2].u);
				} else {
					ParamSetThrow(instance,(OLYspeaker1_THROW_pid)(pPID[0].i + i), pValue[i].u);
				}
			}
			srcPort->WriteMessage(GetAckResponse(pMsg));
			#if USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			olyUI.DebugOut((char*)"NetRX: Set App Prm Throw");
			#endif	// USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			break;
		case eTARGET_XOVER:
			for(i=0; i<pid_count; i++) {
				if (IS_MANDOLIN_MULTIPARAMETER(flags)) {
					if (i == MANDOLIN_FLAG_WORD_OFFSET) { pPID++; }	// Flag word offset
					ParamSetXover(instance,(OLYspeaker1_XOVER_pid)pPID[i*2].i, pValue[i*2]);
				} else {
					ParamSetXover(instance,(OLYspeaker1_XOVER_pid)(pPID[0].i + i), pValue[i]);
				}
			}
			srcPort->WriteMessage(GetAckResponse(pMsg));
			#if USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			olyUI.DebugOut((char*)"NetRX: Set App Prm Xover");
			#endif	// USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			break;
		case eTARGET_FIR:
			for(i=0; i<pid_count; i++) {
				if (IS_MANDOLIN_MULTIPARAMETER(flags))
				{
					if (i == MANDOLIN_FLAG_WORD_OFFSET) { pPID++; }	// Flag word offset
					ParamSetFIR(instance,(OLYspeaker1_FIR_pid)pPID[i*2].i, pValue[i*2].u);
				} else {
					ParamSetFIR(instance,(OLYspeaker1_FIR_pid)(pPID[0].i + i), pValue[i].u);
				}
			}
			srcPort->WriteMessage(GetAckResponse(pMsg));
			#if USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			olyUI.DebugOut((char*)"NetRX: Set App Prm FIR");
			#endif	// USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			return;
		case eTARGET_AIRLOSS:
			for(i=0; i<pid_count; i++) {
				if (IS_MANDOLIN_MULTIPARAMETER(flags)) {
					if (i == MANDOLIN_FLAG_WORD_OFFSET) { pPID++; }	// Flag word offset
					ParamSetAirloss((OLYspeaker1_AIRLOSS_pid)pPID[i*2].i, pValue[i*2]);
				} else {
					ParamSetAirloss((OLYspeaker1_AIRLOSS_pid)(pPID[0].i + i), pValue[i]);
				}
			}
			srcPort->WriteMessage(GetAckResponse(pMsg));
			#if USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			olyUI.DebugOut((char*)"NetRX: Set App Prm Airloss");
			#endif	// USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			break;
		case eTARGET_NOISEGATE:
			for(i=0; i<pid_count; i++) {
				if (IS_MANDOLIN_MULTIPARAMETER(flags)) {
					if (i == MANDOLIN_FLAG_WORD_OFFSET) { pPID++; }	// Flag word offset
					ParamSetNoiseGate((OLYspeaker1_NOISEGATE_pid)pPID[i*2].i, pValue[i*2]);
				} else {
					ParamSetNoiseGate((OLYspeaker1_NOISEGATE_pid)(pPID[0].i + i), pValue[i]);
				}
			}
			srcPort->WriteMessage(GetAckResponse(pMsg));
			#if USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			olyUI.DebugOut((char*)"NetRX: Set App Prm NoiseGate");
			#endif	// USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			break;
		default:
			break;
	}

}

void Config::HandleGetApplicationParameters(MandolinPort * srcPort, mandolin_message * pMsg)
{
	uint32		profile;
	uint32		group;
	uint32_t *	pPIDs;
	uint32_t * 	pValue;
	uint32_t	i, pid_count, flags, instance;
	uint32_t 		idx = 0;

	mandolin_error_code error = MANDOLIN_NO_ERROR;
	mandolin_message * txMsg;

	flags    = ReadParameterFlags(pMsg);
	group    = MANDOLIN_EDIT_TARGET(flags);
	instance = MANDOLIN_WILD_MAP(flags);

	pPIDs   = (uint32_t*)pMsg->payload;

	uint32_t * pLocalValues = 0;

	switch (group){
		case(eTARGET_DEVICE):
			pLocalValues = olyParams.Device->Values;
			#if USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			olyUI.DebugOut((char*)"NetRX: Get App Prm Device");
			#endif	// USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			break;
		case(eTARGET_USER):
#if USE_CDD_UI
			instance = 0;
#else
			if (instance == 0)
				instance = olyParams.Device->Active_User;
			else
				instance--;
#endif
			if (instance < USER_PRESETS)
				pLocalValues = olyParams.User[instance];
				#if USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
				olyUI.DebugOut((char*)"NetRX: Get App Prm User");
				#endif	// USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			break;
		case(eTARGET_VOICING):
#if USE_CDD_UI
			instance = 0;
#else
			if (instance == 0)
				instance = olyParams.Device->Active_Voicing;
			else
				instance--;
#endif
			if (instance < VOICING_PRESETS)
				pLocalValues = olyParams.Voicing[instance];
				#if USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
				olyUI.DebugOut((char*)"NetRX: Get App Prm Voice");
				#endif	// USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			break;
		case(eTARGET_THROW):
#if USE_CDD_UI
			instance = 0;
#else
			if (instance == 0)
				instance = olyParams.Device->Active_Throw;
			else
				instance--;
#endif
			if (instance < THROW_PRESETS)
				pLocalValues = olyParams.Throw[instance];
				#if USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
				olyUI.DebugOut((char*)"NetRX: Get App Prm Throw");
				#endif	// USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			break;
		case(eTARGET_PROFILE):
#if USE_CDD_UI
			instance = 0;
#else
			if (instance == 0)
				instance = olyParams.Device->Active_Profile;
			else
				instance--;
#endif
			if (instance < PROFILE_PRESETS)
				pLocalValues = olyParams.Profile[instance];
				#if USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
				olyUI.DebugOut((char*)"NetRX: Get App Prm Profile");
				#endif	// USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			break;
		case(eTARGET_XOVER):
#if USE_CDD_UI
			instance = 0;
#else
			if (instance == 0)
				instance = olyParams.Device->Active_Xover;
			else
				instance--;
#endif
			if (instance < XOVER_PRESETS)
				pLocalValues = olyParams.Xover[instance];
				#if USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
				olyUI.DebugOut((char*)"NetRX: Get App Prm Xover");
				#endif	// USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			break;
		case(eTARGET_STATUS):
			pLocalValues = olyStatus.Values;
			#if USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			olyUI.DebugOut((char*)"NetRX: Get App Prm Status");
			#endif	// USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			break;
		case(eTARGET_METERS):
			srcPort->GetMeterResponse(pMsg);
			#if USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			olyUI.DebugOut((char*)"NetRX: Get App Prm Meter");
			#endif	// USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			return;
		case(eTARGET_FIR):
			// TODO: Update this if we ever have a product with more than one FIR
//			olyDspPort.WriteMessage(pMsg);
			#if USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			olyUI.DebugOut((char*)"NetRX: Get App Prm FIR");
			#endif	// USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			return;
		case(eTARGET_AIRLOSS):
			pLocalValues = olyParams.Airloss;
			#if USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			olyUI.DebugOut((char*)"NetRX: Get App Prm Airloss");
			#endif	// USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			break;
		case(eTARGET_NOISEGATE):
			pLocalValues = olyParams.NoiseGate;
			#if USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			olyUI.DebugOut((char*)"NetRX: Get App Prm NoiseGate");
			#endif	// USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			break;
		default:
			printf("Get Parameter Group Range error\n");
			error = MANDOLIN_ERROR_INVALID_FLAGS;
			#if USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			olyUI.DebugOut((char*)"NetRX: Get App Prm Invalid");
			#endif	// USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			break;
	}

	if (!pLocalValues)
		error = MANDOLIN_ERROR_INVALID_FLAGS;		// Preset (map) flag out of range

	if (error) {
		srcPort->WriteMessage(GetErrorResponse(error, pMsg));
		return;
	}

	if (IS_MANDOLIN_MULTIPARAMETER(flags)) {

		pid_count = pMsg->length - 1;

		txMsg = GetBlankMessage();
		uint32* pPayload = (uint32*)txMsg->payload;

		txMsg->id = MANDOLIN_MSG_GET_APPLICATION_PARAMETER;
		txMsg->transport = MANDOLIN_TRANSPORT_REPLY;
		txMsg->length = 1 + (pid_count * 2);

		pPayload[idx++] = pPIDs[0];
		pPayload[idx++] = flags;
		pPayload[idx++] = pLocalValues[pPIDs[0]];

		pPIDs++;	// flag offset

		for (i=1; i<pid_count;i++) {
			pPayload[idx++] = pPIDs[i];
			pPayload[idx++] = pLocalValues[pPIDs[i]];   // Value
		}

		MANDOLIN_MSG_pack(txMsg, pMsg->sequence);
		srcPort->WriteMessage(txMsg);
	} else if(group != eTARGET_METERS)	{
		srcPort->WriteMessage(GetParameterBlockResponse((OLY_target)group, instance, pPIDs[0],pLocalValues,MANDOLIN_WILD_COUNT(flags),pMsg->sequence));
	}
}

void Config::HandleSetApplicationString(MandolinPort * srcPort, mandolin_message * pMsg)
{
	OLYspeaker1_DEVICE_lid string_id = *(OLYspeaker1_DEVICE_lid*)pMsg->payload;

	char * pStr = (char *)pMsg->payload + 8;

	if (strlen(pStr) < (STRING_PARAMETER_LENGTH-1))
	{
		if (string_id < eLID_OLYspeaker1_DEVICE_FENCE) {
			strcpy(olyStoredParams.Stored_Labels[string_id], pStr);
			srcPort->WriteMessage(GetAckResponse(pMsg));
			StoreParams();
		} else {
			srcPort->WriteMessage(GetErrorResponse(MANDOLIN_ERROR_INVALID_ID, pMsg));
		}
	} else {
		srcPort->WriteMessage(GetErrorResponse(MANDOLIN_ERROR_INVALID_DATA, pMsg));
	}

#if USE_OLY_UI
	switch(string_id)	{
	case eLID_OLYspeaker1_DEVICE_DEVICE_NAME:
		if (strcmp(olyStoredParams.Stored_Labels[eLID_OLYspeaker1_DEVICE_DEVICE_NAME], c_DefaultDeviceName))
			olyUI.UpdateTitleBarLabel(olyStoredParams.Stored_Labels[eLID_OLYspeaker1_DEVICE_DEVICE_NAME]);
		else
			olyUI.UpdateTitleBarLabel((char*)Region::GetMandolinModelName(Region::GetSystemBrand(), Region::GetSystemModel()));
		break;
	case eLID_OLYspeaker1_DEVICE_PROFILE1_NAME:
		if (strcmp(olyStoredParams.Stored_Labels[eLID_OLYspeaker1_DEVICE_PROFILE1_NAME], c_DefaultDeviceName))
			olyUI.UpdateProfileName(eOLY_PROFILES_CUSTOM1, olyStoredParams.Stored_Labels[eLID_OLYspeaker1_DEVICE_PROFILE1_NAME]);
		else
			olyUI.UpdateProfileName(eOLY_PROFILES_CUSTOM1, (char*)c_DefaultCustom1Name);
		break;
	}
#endif

}

void Config::HandleGetApplicationString(MandolinPort * srcPort, mandolin_message * pMsg)
{
	uint32_t * 	pPayload = (uint32_t*)pMsg->payload;
	char * pData = 0;
	OLYspeaker1_DEVICE_lid  string_id;

	string_id = (OLYspeaker1_DEVICE_lid)pPayload[0];

	pData = olyStoredParams.Stored_Labels[string_id];

	if (pData) {
		srcPort->WriteMessage(GetApplicationStringResponse(string_id, pData, strlen(pData), pMsg->sequence));
	} else {
		srcPort->WriteMessage(GetErrorResponse(MANDOLIN_ERROR_INVALID_ID, pMsg));
	}
}

void Config::HandleGetSoftwareInfo(MandolinPort * srcPort, mandolin_message * pMsg)
{
	uint32 dante_ver, cap_ver, cap_build;

//	g_pUltimoPort->GetCapVersion(&cap_ver, &cap_build);
//	g_pUltimoPort->GetOsVersion(&dante_ver);

	srcPort->WriteMessage(GetSoftwareInfoResponse(dante_ver, cap_ver, cap_build, pMsg->sequence));
}

void Config::HandleCreateParameterList(MandolinPort * srcPort, mandolin_message * pMsg)
{
	uint32_t *				payload = (uint32_t*)pMsg->payload;
	int						listId;
	int 					param;
	int						parameterId;
	int						msgLength;
	mandolin_error_code		err = MANDOLIN_NO_ERROR;

	listId = payload[0];

	if ((1!=listId) && (2!=listId))
		err = MANDOLIN_ERROR_INVALID_DATA;

	msgLength = pMsg->length;

	srcPort->SetParameterListNum(listId, 0);

	param = 1;
	parameterId = -1;
	if (msgLength > 1)
	{
		parameterId = payload[param]; // get the first id to see if empty list
	}
	if (parameterId != 0) 	  // if not empty //	if (( nParameterId != 0xffffffff) && (nParameterId != 0) ) 	  // if not empty
	{
		while (param<msgLength)
		{
			if (param<PORT_PARAMETERLIST_MAX)
			{
				parameterId = payload[param];
				srcPort->SetParameterListParameterId(listId, param-1, parameterId);
				param++;
			}
			else
			{
				printf("Parameter List Count %d exceeds maximum of %d\n", msgLength-1, PORT_PARAMETERLIST_MAX);
				break;
			}
		}
	}
	srcPort->SetParameterListNum(listId, param-1);
	if (MANDOLIN_NO_ERROR==err)
	{
		mandolin_list_op op;
		if (1==param)
		{
			op = MANDOLIN_LIST_DESTROY;
			srcPort->WriteMessage(GetAckResponseWithData(pMsg, (uint8_t *)&op, sizeof(err)));
		}
		else
		{
			op = MANDOLIN_LIST_CREATE;
			srcPort->WriteMessage(GetAckResponseWithData(pMsg, (uint8_t *)&op, sizeof(err)));
		}
	}
	else
		srcPort->WriteMessage(GetErrorResponse(err, pMsg));

}

void Config::HandleSubscribeParameter(MandolinPort * srcPort, mandolin_message * pMsg)
{
	int 					listId;
	uint32_t 					flags;
	uint32_t *				payload = (uint32_t*)pMsg->payload;
	int 					schedule;
	int 					period_ms;
	mandolin_error_code		err = MANDOLIN_NO_ERROR;

	// Sanity check length
	if(pMsg->length != 3)
		err = MANDOLIN_ERROR_LENGTH_MISMATCH;

	listId =		payload[0];
	flags =			payload[1];	//	(eTARGET_STATUS<<MANDOLIN_EDIT_TARGET_SHIFT); // For testing status
	schedule = 		(int)payload[2];	//	payload[2]?((-1000)<<16):0;	//	test sending only changed parameters

	if(MANDOLIN_NO_ERROR==err)
	{
		if (MANDOLIN_WILD_TYPE(flags) !=  MANDOLIN_TYPE_LIST)
		{
			err = MANDOLIN_ERROR_INVALID_DATA;
		}
		if ((1!=listId) && (2!=listId))
			err = MANDOLIN_ERROR_INVALID_DATA;
	}

	if(MANDOLIN_NO_ERROR==err)
	{
		period_ms = (schedule>>16);	//	sign extension for signed period
		srcPort->ConfigureMeters(listId,
				period_ms,
				flags,
				(((schedule>>MANDOLIN_SCHEDULE_NOACK_SHIFT) & MANDOLIN_SCHEDULE_NOACK_MASK) != 0));
	}

	if(MANDOLIN_NO_ERROR==err)
		srcPort->WriteMessage(GetAckResponse(pMsg));
	else
		srcPort->WriteMessage(GetErrorResponse(err, pMsg));
}

void Config::KillAllSubscriptions(void)
{
	uint8_t i = 0;

	for(i=0; i<ePID_OLYspeaker1_STATUS_FENCE; i++)	_timer_cancel(SubscriptionTimers[i]);
}

#endif	//	_SECONDARY_BOOT

void Config::HandleTestCommand(MandolinPort * srcPort, mandolin_message * pMsg)
{
	oly_test_command 	cmdID;
	uint32_t *			pPayload = (uint32_t*)pMsg->payload;
	uint32_t uiLength = pMsg->length;

	cmdID = (oly_test_command)pPayload[0];

	uint8_t * Mac;

	uint32_t TestValueID, TestValue1, TestValue2;
	mandolin_parameter_value test_value, test_value2, test_value3;

	switch (cmdID)
	{
		//printf("Test command received: ID %d", cmdID);

		case(TEST_CMD_SET_STATIC_IP):
			m_pUpgrade->SetStaticIp(pPayload[1]);
			srcPort->WriteMessage(GetAckResponse(pMsg));
			break;
		case(TEST_CMD_SET_STATIC_ALL):
			m_pUpgrade->SetIpSettings(pPayload[1], pPayload[2], pPayload[3]);
			srcPort->WriteMessage(GetAckResponse(pMsg));
			break;
		case(TEST_CMD_GET_STATIC_ALL):
			srcPort->WriteMessage(GetStaticAllResponse(m_pUpgrade->GetStaticIp(), m_pUpgrade->GetStaticGateway(), m_pUpgrade->GetStaticMask(), pMsg->sequence));
			break;
		case(TEST_CMD_SET_IDENTITY):
			if (uiLength>=2)
			{
				int32_t nBrand = 0xFFFFFFFF;
				int32_t nModel = 0xFFFFFFFF;
				uint16_t hardwareRev = 0xFFFF;
				uint32_t uiSerialNumber = 0xFFFFFFFF;
				uint8_t mac[6];
				mac[0] = (pPayload[1] >> 8) & 0xFF;
				mac[1] = (pPayload[1] >> 0) & 0xFF;
				mac[2] = (pPayload[2] >> 24) & 0xFF;
				mac[3] = (pPayload[2] >> 16) & 0xFF;
				mac[4] = (pPayload[2] >> 8) & 0xFF;
				mac[5] = (pPayload[2] >> 0) & 0xFF;
				if (uiLength>=4)
					nBrand = pPayload[3];
				if (uiLength>=5)
					nModel = pPayload[4];
				if (uiLength>=6)
					hardwareRev = (uint16_t)(pPayload[5]&0xFFFF);
				if (uiLength>=7)
					uiSerialNumber = pPayload[6];
				m_pUpgrade->WriteIdentity(mac, nBrand, nModel, hardwareRev, uiSerialNumber);
			}
			srcPort->WriteMessage(GetAckResponse(pMsg));
			break;
		case(TEST_CMD_SET_MODEL):
			if (uiLength>=3)
			{
				int32_t nBrand = 0xFFFFFFFF;
				int32_t nModel = 0xFFFFFFFF;
				uint16_t hardwareRev = 0xFFFF;
				uint8_t mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
				uint32_t uiSerialNumber = 0xFFFFFFFF;

				nBrand = pPayload[1];
				nModel = pPayload[2];

				m_pUpgrade->WriteIdentity(mac, nBrand, nModel, hardwareRev, uiSerialNumber);
			}
			srcPort->WriteMessage(GetAckResponse(pMsg));
			break;
		case(TEST_CMD_SET_SERIAL_NUMBER):
			if (uiLength>=2)
			{
				int32_t nBrand = 0xFFFFFFFF;
				int32_t nModel = 0xFFFFFFFF;
				uint16_t hardwareRev = 0xFFFF;
				uint8_t mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
				uint32_t uiSerialNumber = pPayload[1];

				m_pUpgrade->WriteIdentity(mac, nBrand, nModel, hardwareRev, uiSerialNumber);
			}
			srcPort->WriteMessage(GetAckResponse(pMsg));
			break;
		case(TEST_CMD_GET_SERIAL_NUMBER):
			srcPort->WriteMessage(GetSerialNumberResponse(m_pUpgrade->GetSerialNumber(), pMsg->sequence));
			break;
		case(TEST_CMD_PRINT_MESSAGE):
			printf("MANDOLIN TEST MESSAGE: %s\n", (char*)pMsg->payload + 4);
			srcPort->WriteMessage(GetAckResponse(pMsg));
			break;
		case(TEST_CMD_GET_HW_VER):
			TestValue1 = Region::GetHardwareRevision();
			srcPort->WriteMessage(GetAckResponseWithData(pMsg, (uint8_t*)&TestValue1, sizeof(TestValue1)));
			break;
#ifndef _SECONDARY_BOOT
		case(TEST_CMD_RESTORE_DEFAULTS):
			RestoreDefaults(false);
			srcPort->WriteMessage(GetAckResponse(pMsg));
			break;
		case(TEST_CMD_GET_STATUS_ALL):
			srcPort->WriteMessage(GetStatusAllResponse(olyStatus.Values, pMsg->sequence));
			break;
#endif	//	_SECONDARY_BOOT

#if MFG_TEST_EAW || MFG_TEST_MARTIN
		case TEST_CMD_SET_INPUT:
			// Sets the input channel.
			TestValue1 = pPayload[1];
			switch (TestValue1) {
			case TEST_INPUT_ANALOG:
				SetAudioSource(src_analog);
				break;
			case TEST_INPUT_DANTE:
				SetAudioSource(src_dante);
				break;
			case TEST_INPUT_PINK:
				Mfg_SetAudioTestSource(src_pinknoise);
				SetAudioSource(src_pinknoise);
				break;
			case TEST_INPUT_SINE:
				Mfg_SetAudioTestSource(src_sine);
				SetAudioSource(src_pinknoise);
				break;
			}
			srcPort->WriteMessage(GetAckResponse(pMsg));
			break;
		case TEST_CMD_SET_OUTPUT:
			// Sets the output channel.
			TestValue1 = pPayload[1];
			SelfTestEnableChannel(ch_none);
			SelfTestEnableChannel(TestValue1);
			srcPort->WriteMessage(GetAckResponse(pMsg));
			break;
		case TEST_CMD_SET_FUNC_GEN:
			// It sets parameters for the function generator.
			test_value.u = pPayload[2];				// fader
			test_value2.u = pPayload[3];			// frequency
			Mfg_SetFunctionGenerator(test_value, test_value2);
			srcPort->WriteMessage(GetAckResponse(pMsg));
			break;
		case TEST_CMD_GET_RMS_METER_LEVEL:
			// This message returns RMS Meter Level of the input source selected by the previous message.
			test_value.f = GetCurrentUIMeter();
			srcPort->WriteMessage(Mfg_TestValueResponse(TEST_CMD_GET_RMS_METER_LEVEL, test_value.u, pMsg->sequence));
			break;
		case TEST_CMD_GET_RMS_METER_RANGE:
			// This message test RMS levels against specified min and max values.
			test_value.u = pPayload[1];				// min
			test_value2.u = pPayload[2];			// max
			test_value3.b = MeterRangeCheck(test_value.f, test_value2.f);
			srcPort->WriteMessage(Mfg_TestValueResponse(TEST_CMD_GET_RMS_METER_RANGE, test_value3.u, pMsg->sequence));
			break;
		case TEST_CMD_WRITE_IO:
			// This message set IO pin to 1 or 0 and checks the associated peripheral device state accordingly.
			TestValueID = pPayload[1];
			TestValue1 = pPayload[2];
			srcPort->WriteMessage(Mfg_TestValueIndexedResponse(TEST_CMD_WRITE_IO, TestValueID, (uint32_t) Mfg_SetOutputPinValue(TestValueID, TestValue1), pMsg->sequence));
			break;
		case TEST_CMD_READ_IO:
			// This message read the status of an IO pin.
			TestValueID = pPayload[1];
			srcPort->WriteMessage(Mfg_TestValueIndexedResponse(TEST_CMD_READ_IO, TestValueID, (uint32_t) Mfg_GetInputPinValue(TestValueID), pMsg->sequence));
			break;
		case TEST_CMD_READ_AMP_TEMPERATURE:
			// This message reads voltage value corresponding to amp current temperature.
			TestValueID = pPayload[1];
			test_value.f = GetCurrentAmpTemp((amp_instance_t) TestValueID);
			srcPort->WriteMessage(Mfg_TestValueIndexedResponse(TEST_CMD_READ_AMP_TEMPERATURE, TestValueID, test_value.u, pMsg->sequence));
			break;
		case TEST_CMD_VALIDATE_SPI_FLASH:
			test_value.u = memory_check_for_flash_presence(FSL_SPI_AUX);
			srcPort->WriteMessage(Mfg_TestValueResponse(TEST_CMD_VALIDATE_SPI_FLASH, test_value.u, pMsg->sequence));
			break;
		case TEST_CMD_GET_IMON_VALUE:
			TestValueID = pPayload[1];
			test_value.u = (uint32_t) CurrentSenseRawRead((uint8_t) TestValueID);
			//test_value.u = (uint32_t) CurrentSenseRawRead(current_sense_config[TestValueID].adc_channel);
			srcPort->WriteMessage(Mfg_TestValueIndexedResponse(TEST_CMD_GET_IMON_VALUE, TestValueID, test_value.u, pMsg->sequence));
			break;
#endif // MFG_TEST_EAW || MFG_TEST_MARTIN
		case TEST_DSP_ERROR:
			// forward error message out Network and UART
			olyNetworkPort.WriteMessage(pMsg, false);
#if !USES_FOUR_IRDA
//			olyVoicingPort.WriteMessage(pMsg);
#endif
			break;
		default:
			srcPort->WriteMessage(GetErrorResponse(MANDOLIN_ERROR_UNIMPLEMENTED_MSGID, pMsg));
			break;
	}
}

void Config::SoftwareReset(MandolinPort * srcPort, mandolin_message * pMsg)
{
	uint32_t uiRebootType;
	uint32_t *pPayload = (uint32_t *)pMsg->payload;
	mandolin_error_code error = MANDOLIN_ERROR;

	if (!pMsg->payload)
	{
		error = MANDOLIN_ERROR_INVALID_DATA;
	}
	else
	{
		uiRebootType =	pPayload[0];

		if ((uiRebootType!=OLY_APPID_BOOTLOADER) && (uiRebootType!=OLY_APPID_MAIN))	//	 update for real macros when present
		{
			//	Invalid reboot mode
			error = MANDOLIN_ERROR_INVALID_DATA;
		}
		else if (srcPort->IsPortOpen())
		{
			//	Send Disconnect messages to all other ports - don't know if should send to calling port (Alex?)
//			srcPort->WriteMessage(Disconnect(MANDOLIN_DISCONNECT_UPDATING, broadcastExcludeNoSHARC);	//	Not currently supported

			//	Change launch type in flash
			if (m_pUpgrade)
			{
#ifdef SC_COMMENTED_OUT
				bool bSuccess = m_pUpgrade->SetLaunchType((uiRebootType==OLY_APPID_BOOTLOADER)?OLY_REGION_SECONDARY_BOOT:OLY_REGION_APPLICATION);
#endif //  SC_COMMENTED_OUT
				error =  MANDOLIN_ERROR_INVALID_DATA; // SC : bSuccess?MANDOLIN_NO_ERROR:MANDOLIN_ERROR_INVALID_DATA;
			}

			//	 only schedule reset if everything worked
//			if (MANDOLIN_NO_ERROR==error)
//				DeferredReboot(500);	//	wait 500 milliseconds to reboot
		}
		else
			error = MANDOLIN_ERROR;
	}

	// is response needed
	if (ISNT_MANDOLIN_NOACK(pMsg->transport))
	{
		mandolin_message *pM;
		if (MANDOLIN_NO_ERROR==error)
			pM =  GetAckResponseWithData(pMsg,(uint8_t*)&uiRebootType,4);
		else if (MANDOLIN_NO_ERROR_NO_FURTHER_REPLY!=error)
			pM =  GetErrorResponse(error, pMsg);
		srcPort->WriteMessage(pM);
	}


	// Response is ACK + any payload - ignoring NOACK
   // MANDOLIN_reply_with_payload(pMsgWrap,(uint16)return_words,&MANDOLIN_ReplyMsgPayload);
   // return MANDOLIN_NO_ERROR_NO_FURTHER_REPLY;
	return;
}

void Config::HandleGetSystemStatus(MandolinPort * srcPort, mandolin_message * pMsg)
{
	uint32_t	uiItem;
	int			nPayloadLength = 0;
	uint32_t	uiValue[8];
	uint32_t * 	puiPayload = (uint32_t *)pMsg->payload;
	mandolin_error_code error = MANDOLIN_ERROR;

	uiItem = puiPayload[0];

	switch (uiItem) {
	case MANDOLIN_SS_SYSTEM_IP_ADDRESS:
		uiValue[nPayloadLength++] = MANDOLIN_SS_SYSTEM_IP_ADDRESS;
	    uiValue[nPayloadLength++] = (((g_CurrentIpAddress >> 24) & 0x0ff) << 0) + (((g_CurrentIpAddress >> 16) & 0x0ff) << 8)
                + (((g_CurrentIpAddress >> 8) & 0x0ff) << 16) + (((g_CurrentIpAddress >> 0) & 0x0ff) << 24);
		error = MANDOLIN_NO_ERROR;
		break;
	case MANDOLIN_SS_STATIC_IP_CONFIG:
		uiValue[nPayloadLength++] = MANDOLIN_SS_STATIC_IP_CONFIG;
		uiValue[nPayloadLength++] = m_pUpgrade->GetStaticIp()?1:0;
		uiValue[nPayloadLength++] = m_pUpgrade->GetStaticIp();
		uiValue[nPayloadLength++] = m_pUpgrade->GetStaticMask();
		uiValue[nPayloadLength++] = m_pUpgrade->GetStaticGateway();
		error = MANDOLIN_NO_ERROR;
		break;
	case MANDOLIN_SS_MAX_MSG_WORDS:
		uiValue[nPayloadLength++] = MANDOLIN_SS_MAX_MSG_WORDS;
		uiValue[nPayloadLength++] = MANDOLIN_MAX_PAYLOAD_DATA_WORDS + MANDOLIN_HEADER_WORDS;
		error = MANDOLIN_NO_ERROR;
		srcPort->GetMaxMsgWords(pMsg);
		break;
	default:
		error = MANDOLIN_ERROR_INVALID_DATA;
		break;
	}

	// is response needed
	if (ISNT_MANDOLIN_NOACK(pMsg->transport))
	{
		mandolin_message *pM;
		if (MANDOLIN_NO_ERROR==error)
			pM =  GetAckResponseWithData(pMsg,(uint8_t*)&uiValue, nPayloadLength * sizeof(uint32_t));
		else if (MANDOLIN_NO_ERROR_NO_FURTHER_REPLY!=error)
			pM =  GetErrorResponse(error, pMsg);
		srcPort->WriteMessage(pM);
	}
}

void Config::HandleSystemEvent(MandolinPort * srcPort, mandolin_message * pMsg)
{
	mandolin_error_code error = MANDOLIN_ERROR;
	uint32_t *pPayload = (uint32_t *)pMsg->payload;
	uint32_t uiEventType;
	uint32_t uiConnectParams[6];

	if ((!pMsg->payload) || (pMsg->length<1))
	{
		error = MANDOLIN_ERROR_INVALID_DATA;
	}
	else
	{
		uiEventType = pPayload[0];

		switch (uiEventType)
		{
			//	Not doing anything but acknowledging Update commands for now.
			case MANDOLIN_SE_BEGIN_UPDATE:
				error = MANDOLIN_NO_ERROR;
				break;
			case MANDOLIN_SE_END_UPDATE:
				error = MANDOLIN_NO_ERROR;
				break;
			case MANDOLIN_SE_CONNECT:
				if (pPayload[1]){
#ifdef _SECONDARY_BOOT
					error = MANDOLIN_ERROR_INVALID_ID;
#else
					HandleFastConnect(srcPort, pMsg);
					return;
#endif	// _SECONDARY_BOOT
				} else {
					IdentifyStop();
					error = srcPort->Connect(pMsg);
				}
				break;
			case MANDOLIN_SE_IDENTIFY:
				 error = MANDOLIN_NO_ERROR;
				 if (pPayload[1] > 0) {
					 if (pMsg->length > 2) {
						 IdentifyStart(pPayload[1], (LOUD_identify_mode)pPayload[2]);
					 } else {
						 IdentifyStart(pPayload[1], eIDENTIFY_MODE_IDENTIFY);
					 }
				 } else {
				     IdentifyStop();
				 }
				break;
#ifndef  _SECONDARY_BOOT
			case MANDOLIN_SE_SYNC_CURRENT_STATE:
				srcPort->Sync(pMsg);
				error = MANDOLIN_NO_ERROR;
				break;
#endif	//	_SECONDARY_BOOT
			default:
				error = MANDOLIN_ERROR_UNIMPLEMENTED_MSGID;
				break;
		}
	}

	// is response needed
	if (ISNT_MANDOLIN_NOACK(pMsg->transport))
	{
		mandolin_message *pM;
		if (MANDOLIN_NO_ERROR==error)
			pM =  GetAckResponseWithData(pMsg,(uint8_t*)&uiEventType,sizeof(uiEventType));
		else if (MANDOLIN_NO_ERROR_NO_FURTHER_REPLY!=error)
			pM =  GetErrorResponse(error, pMsg);
		srcPort->WriteMessage(pM);
	}

	return;
}

void Config::HandleFastConnect(MandolinPort * srcPort, mandolin_message * pMsg)
{
	mandolin_error_code error = MANDOLIN_ERROR;
	uint32_t *pPayload = (uint32_t *)pMsg->payload;

	uint32_t version = ((MANDOLIN_MAJOR_VERSION & 0x0ff) << 24) | ((MANDOLIN_MINOR_VERSION & 0x0ff) << 16) | ((OLYspeaker1_XML_VERSION & 0x0ffff) << 0);
    volatile oly_version_t oly_version;

    LOUD_identify_mode ident_mode = eIDENTIFY_MODE_OFF;
    	// FAST CONNECT
    if (pPayload[2] == 0) {
		IdentifyStop();
    } else {
    	if (pPayload[1] == 1) {			// Fast
    		ident_mode = eIDENTIFY_MODE_ADVERTISE; // Amber solid
    	} else if (pPayload[1] == 2) {	// Furious
    		ident_mode = eIDENTIFY_MODE_CONNECTED;	// Amber blink
    	} else if (pPayload[1] == 3) { 	// Furiouser
    		ident_mode = eIDENTIFY_MODE_IDENTIFY;	// White blink
    	}
		IdentifyStart(pPayload[2], ident_mode);
    }

	uint32_t * response = new uint32_t[FAST_CONNECT_PAYLOAD_LENGTH];

	response[0] = version;
#ifdef _SECONDARY_BOOT
	oly_version.u32 = OLY_BOOT_VERSION;
	response[1] = OLY_APPID_BOOTLOADER;
	response[2] = (oly_version.major << 24) | (oly_version.minor << 16) | (oly_version.sub << 8) | (oly_version.build);
	response[3] = 0;
	response[4] = 0;
	response[5] = 0;
	response[6] = 0;
	response[7] = 0;
#else
	oly_version.u32 = OLY_FW_VERSION;
	response[1] = OLY_APPID_MAIN;
	response[2] = (oly_version.major << 24) | (oly_version.minor << 16) | (oly_version.sub << 8) | (oly_version.build);
	response[3] = olyStatus.Angle;
	response[4] = olyStatus.Array_Size;
	response[5] = olyStatus.Array_Index;
	response[6] = olyStatus.Array_ID_Above;
	response[7] = olyStatus.Array_ID_Below;

#endif	//	_SECONDARY_BOOT

	srcPort->WriteMessage(GetSysEventResponseWithData(pMsg, (uint8_t*)response, FAST_CONNECT_PAYLOAD_LENGTH * 4), false);

	delete response;

	srcPort->SetForceClose();
}

void Config::HandleFileMessage(MandolinPort * srcPort, mandolin_message * pMsg)
{
	mandolin_error_code error = MANDOLIN_ERROR;
	uint32_t *pPayload = (uint32_t *)pMsg->payload;

	P_OLY_REGION pUpgradeRgn;
	uint8_t *pResponse = 0;
	uint32_t uiResponseBytes = 0;
	uint32_t uiResponse;

	if (!pMsg->payload)
	{
		error = MANDOLIN_ERROR_INVALID_DATA;
	}
	else
	{
		switch (pMsg->id) {
		case MANDOLIN_MSG_FILE_OPEN:
			pUpgradeRgn = (P_OLY_REGION)pPayload;

			//	Change launch type in flash
			if (m_pUpgrade)
			{
				if (!m_pUpgrade->StartUpgrade(pUpgradeRgn))
				{
					error =  MANDOLIN_ERROR;
				}
				else
				{
					error = MANDOLIN_NO_ERROR;
					//	return chunksize
					uiResponse =  P_SECTOR_SIZE;
					uiResponseBytes = sizeof(uiResponse);
					pResponse = (uint8_t *)&uiResponse;
				}
			}
			break;
		case MANDOLIN_MSG_FILE_CLOSE:
			if (m_pUpgrade)
			{
				if (!m_pUpgrade->EndUpgrade())
					error =  MANDOLIN_ERROR;
				else
					error = MANDOLIN_NO_ERROR;
			}
			break;
		case MANDOLIN_MSG_POST_FILE:
			//	Change launch type in flash
			if (m_pUpgrade)
			{
				uint32_t uiBytes = *(pPayload+3);
				if (!m_pUpgrade->UpgradeChunk((uint8_t*)(pPayload+4), uiBytes))
					error =  MANDOLIN_ERROR_INVALID_DATA;
				else
				{
					error = MANDOLIN_NO_ERROR;
					//	return code
					uiResponse =  1;
					uiResponseBytes = sizeof(uiResponse);
					pResponse = (uint8_t *)&uiResponse;
				}
			}
			break;
		default:
			error = MANDOLIN_ERROR_UNIMPLEMENTED_MSGID;
			break;
		}
	}

	// is response needed
	if (ISNT_MANDOLIN_NOACK(pMsg->transport))
	{
		mandolin_message *pM;
		if (MANDOLIN_NO_ERROR==error)
		{
			if (pResponse)
				pM =  GetAckResponseWithData(pMsg, pResponse, uiResponseBytes);
			else
				pM =  GetAckResponse(pMsg);
		}
		else if (MANDOLIN_NO_ERROR_NO_FURTHER_REPLY!=error)
			pM =  GetErrorResponse(error, pMsg);
		srcPort->WriteMessage(pM);
	}
}

#ifndef _SECONDARY_BOOT

mandolin_error_code Config::ParamValidate(OLY_target group, uint32_t PID, mandolin_parameter_value * pValue)
{
	mandolin_parameter_value  val, min, max;
	mandolin_error_code error = MANDOLIN_NO_ERROR;
	const OLYspeaker1_parameter_descriptor * pTable;
	OLY_format format;

	val = *pValue;

	switch(group)
	{
	case(eTARGET_USER):
		if (PID >= ePID_OLYspeaker1_USER_FENCE)
			error = MANDOLIN_ERROR_INVALID_ID;
		pTable = gOLYspeaker1UserParameterTable;
		break;
	case(eTARGET_PROFILE):
		if (PID >= ePID_OLYspeaker1_PROFILE_FENCE)
			error = MANDOLIN_ERROR_INVALID_ID;
		pTable = gOLYspeaker1ProfileParameterTable;
		break;
	case(eTARGET_THROW):
		if (PID >= ePID_OLYspeaker1_THROW_FENCE)
			error = MANDOLIN_ERROR_INVALID_ID;
		pTable = gOLYspeaker1ThrowParameterTable;
		break;
	case(eTARGET_VOICING):
		if (PID >= ePID_OLYspeaker1_VOICING_FENCE)
			error = MANDOLIN_ERROR_INVALID_ID;
		pTable = gOLYspeaker1VoicingParameterTable;
		break;
	case(eTARGET_DEVICE):
		if (PID >= ePID_OLYspeaker1_DEVICE_FENCE)
			error = MANDOLIN_ERROR_INVALID_ID;
		pTable = gOLYspeaker1DeviceParameterTable;
		break;
	case(eTARGET_XOVER):
		if (PID >= ePID_OLYspeaker1_XOVER_FENCE)
			error = MANDOLIN_ERROR_INVALID_ID;
		pTable = gOLYspeaker1XoverParameterTable;
		break;
	case(eTARGET_FIR):
		error = MANDOLIN_NO_ERROR;
		if(PID >= ePID_OLYspeaker1_FIR_FENCE)
			error = MANDOLIN_ERROR_INVALID_ID;
		if ((PID == ePID_OLYspeaker1_FIR_HF_FIR_ENABLE) || (PID == ePID_OLYspeaker1_FIR_HF_FIR_MUTE) ||
			(PID == ePID_OLYspeaker1_FIR_MF_FIR_ENABLE) || (PID == ePID_OLYspeaker1_FIR_MF_FIR_MUTE) ||
			(PID == ePID_OLYspeaker1_FIR_LF_FIR_ENABLE) || (PID == ePID_OLYspeaker1_FIR_LF_FIR_MUTE)) {
			if (val.i > 1) {
				printf("Parameter Set Range error Val:%d > Max:%d\n", val.i, 1);
				error = MANDOLIN_ERROR_INVALID_DATA;
			}else if (val.i < 0) {
				printf("Parameter Set Range error Val:%d < Min:%d\n", val.i, 0);
				error = MANDOLIN_ERROR_INVALID_DATA;
			}
		} else {	// its a coef
			if (val.f > 2.0) {
				printf("Parameter Set Range error Val:%f > Max:%f\n", val.f, 2.0);
				error = MANDOLIN_ERROR_INVALID_DATA;
			}else if (val.f < -2.0) {
				printf("Parameter Set Range error Val:%f < Min:%f\n", val.f, -2.0);
				error = MANDOLIN_ERROR_INVALID_DATA;
			}

		}
		return error;
	case(eTARGET_AIRLOSS):
		if (PID >= ePID_OLYspeaker1_AIRLOSS_FENCE)
			error = MANDOLIN_ERROR_INVALID_ID;
		pTable = gOLYspeaker1AirlossParameterTable;
		break;
	case(eTARGET_NOISEGATE):
		if (PID >= ePID_OLYspeaker1_NOISEGATE_FENCE)
			error = MANDOLIN_ERROR_INVALID_ID;
		pTable = gOLYspeaker1NoisegateParameterTable;
		break;
	case(eTARGET_STATUS):
		return MANDOLIN_NO_ERROR;	// No need to validate status as they are read-only
		break;
	default:
		if (group >= eTARGET_FENCE)
			return MANDOLIN_ERROR_INVALID_FLAGS;
		else	// what's left?  status?  should there be an error for this?
			return MANDOLIN_ERROR_UNIMPLEMENTED_MSGID;
		break;
	}

	if (error)
		return error;

	format = (OLY_format)pTable[PID].format;

//	printf("Group = %d, PID = %d, format = %d\n", group, PID, format);

	switch(format)
	{
	case(eFORMAT_FLOAT):
		min.f = *(float32*)&pTable[PID].minValue;
		max.f = *(float32*)&pTable[PID].maxValue;
		if (val.f > max.f) {
			printf("Parameter Set Range error Val:%f > Max:%f\n",val.f, max.f);
			error = MANDOLIN_ERROR_INVALID_DATA;
		}else if (val.f < min.f) {
			printf("Parameter Set Range error Val:%f < Min:%f\n",val.f, min.f);
			error = MANDOLIN_ERROR_INVALID_DATA;
		}
		break;
	case(eFORMAT_UNSIGNED):
		if (val.u > pTable[PID].maxValue)
		{
			printf("Parameter Set Range error Val:%d > Max:%d\n",val.u, pTable[PID].maxValue);
			error = MANDOLIN_ERROR_INVALID_DATA;
		}else if (val.u < pTable[PID].minValue){
			printf("Parameter Set Range error Val:%d < Min:%d\n",val.u, pTable[PID].minValue);
			error = MANDOLIN_ERROR_INVALID_DATA;
		}
		break;
	case(eFORMAT_BOOL):
	case(eFORMAT_SIGNED):
		min.i = *(int32*)&pTable[PID].minValue;
		max.i = *(int32*)&pTable[PID].maxValue;
		if (val.i > max.i) {
			printf("Parameter Set Range error Val:%d > Max:%d\n",val.i, max.i);
			error = MANDOLIN_ERROR_INVALID_DATA;
		}else if (val.f < min.f) {
			printf("Parameter Set Range error Val:%d < Min:%d\n",val.i, min.i);
			error = MANDOLIN_ERROR_INVALID_DATA;
		}
	default:
		if (format >= eFORMAT_FENCE)
			error = MANDOLIN_ERROR_INVALID_FLAGS;
		else
			error = MANDOLIN_NO_ERROR;
		break;
	}
	return error;
}

float Config::ParamGetMin(OLYspeaker1_USER_pid PID)
{
	void * pMin = (void*)&gOLYspeaker1UserParameterTable[PID].minValue;
	return *(float*)pMin;
}

float Config::ParamGetMax(OLYspeaker1_USER_pid PID)
{
	void * pMax = (void*)&gOLYspeaker1UserParameterTable[PID].maxValue;
	return *(float*)pMax;
}

float Config::ParamGetDefault(OLYspeaker1_USER_pid PID)
{
	void * pMin = (void*)&gOLYspeaker1UserParameterTable[PID].defValue;
	return *(float*)pMin;
}

float Config::ParamGetValue(uint32_t instance, OLYspeaker1_USER_pid PID)
{
	void * pValue = (void*)&olyParams.User[instance][PID];
	return *(float*)pValue;
}

float Config::ParamXoverGetMin(OLYspeaker1_XOVER_pid PID)
{
	void * pMin = (void*)&gOLYspeaker1XoverParameterTable[PID].minValue;
	return *(float*)pMin;
}

float Config::ParamXoverGetMax(OLYspeaker1_XOVER_pid PID)
{
	void * pMax = (void*)&gOLYspeaker1XoverParameterTable[PID].maxValue;
	return *(float*)pMax;
}

float Config::ParamXoverGetValue(uint32_t instance, OLYspeaker1_XOVER_pid PID)
{
	void * pValue = (void*)&olyParams.Xover[instance][PID];
	return *(float*)pValue;
}

//	This is messy.  Need a better solution to get param database into Sync logic in MandolinPort.cpp.
oly_params_t *Config::GetParamsDatabase()
{
	return (&olyParams);
}

//	This is messy.  Need a better solution to get param database into Sync logic in MandolinPort.cpp.
oly_flash_params_t *Config::GetStoredParamsDatabase()
{
	return (&olyStoredParams);
}

//	This is messy.  Need a better solution to get param database into Sync logic in MandolinPort.cpp.
oly_status_t *Config::GetStatusParamsDatabase()
{
	return (&olyStatus);
}

#endif	//	_SECONDARY_BOOT

}	/* namespace oly */

