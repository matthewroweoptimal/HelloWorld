/*
 * ConfigManager.cpp
 *
 *  Created on: Jun 18, 2015
 *      Author: Kris.Simonsen
 */

#include "ConfigManager.h"
#include "Region.h"
//#include "IRDAUartPort.h"
//#include "IRDA_Task.h"

#include "CommMandolin.h"
#include "network.h"
#include "TimeKeeper.h"

extern "C" {
#include "flash_params.h"
//#include "Drivers/SpiFlash/spi_flash_fs.h"
//#include "pascal_spro2.h"
//#include "CS42526.h"
#ifndef _SECONDARY_BOOT
#include "Voicing_Info.h"
#endif	//	_SECONDARY_BOOT
//#include "SpeakerConfiguration.h"
//#include "AmpMonitor.h"
//#include "MMA8653FC.h"
//#include "ftm_lcdbacklight.h"
#include "OLYspeaker1_map.h"
#include "oly_logo.h"
//#include "UltimoPort.h"
//#include "IRDAManager.h"
//#include "uart_irda.h"
#include <math.h>
#if USE_CDD_UI
//#include "cddl_leds.h"
#endif
}

#include <stdio.h>
#include <string.h>

static char g_DiscoServiceName[64] = "\x0";

const char c_DefaultDeviceName[] = "Default Name";
const char c_DefaultGroupName[] = "Default Group";
const char c_DefaultDanteDiscoName[] = "Unknown";
const char c_Preset1Name[] = "Sapphire";
const char c_Preset2Name[] = "White";
const char c_Preset3Name[] = "Grey";
const char c_Preset4Name[] = "Blue";
const char c_DefaultCustom1Name[] = "User";

extern uint32_t g_CurrentIpAddress;
extern const char *g_pszModelName;

bool startSelfTest = false;
bool destroySelfTestTimer = false;
MQX_TICK_STRUCT selfTest_ticks;
static _timer_id selfTest_timer;
uint8_t g_nLastMeterSeq = 0;

//extern UltimoPort * g_pUltimoPort;
bool g_ScreenInTransition = false;

namespace oly {

#if 1

#ifndef _SECONDARY_BOOT

    void Config::EncoderL()
    {
    #if USE_OLY_UI
    	olyUI.EncoderL();
    #endif
    }
    
    void Config::EncoderR()
    {
    #if USE_OLY_UI
    	olyUI.EncoderR();
    #endif
    }
    
    void Config::EncoderSW()
    {
    #if USE_OLY_UI
    	olyUI.EncoderSW();
    #endif
    }
    
    void Config::Switch2()
    {
    #if USE_CDD_UI
    	cddlUI.SW2();
    #endif
    }
    
    void Config::SW2_long()
    {
    #if USE_CDD_UI && !MFG_TEST_MARTIN
    	cddlUI.SW2_long();
    #endif
    }
    
    void Config::SW2_long_long()
    {
    #if USE_CDD_UI && !MFG_TEST_MARTIN
    	cddlUI.SW2_long_long();
    #endif
    }

    void Config::OnParamSubscriptionEventVoicingList(int listId)
    {
    #ifndef USES_FOUR_IRDA
    #if 0
    	int flags = olyVoicingPort.GetParameterListFlags(listId);
    	int group = MANDOLIN_EDIT_TARGET(flags);
    
    	olyVoicingPort.ReportMeters(listId, (mandolin_parameter_value *)olyStatus.Values);
    
    	if (eTARGET_STATUS==group)
    	{
    		// Reset audio level peak after every push //
    		//	!!!Audio level needs to be peaked for each port
    		Audio_Level = FLOAT_NEG_INFINITY;
    	}
    #endif
    #endif
    }
    void Config::OnParamSubscriptionEventNetworkList(int listId)
    {
    	int flags = olyNetworkPort.GetParameterListFlags(listId);
    	int group = MANDOLIN_EDIT_TARGET(flags);
    
    	olyNetworkPort.ReportMeters(listId, (mandolin_parameter_value *)olyStatus.Values);
    
    	if (eTARGET_STATUS==group)
    	{
    		// Reset audio level peak after every push //
    		//	!!!Audio level needs to be peaked for each port
    		Audio_Level = FLOAT_NEG_INFINITY;
    	}
    }
    
    void Config::ParamSetDevice(OLYspeaker1_DEVICE_pid PID, void * pValue)
    {
    	switch (PID) {
    		case ePID_OLYspeaker1_DEVICE_LOGO_MODE:
    			IdentifyStop();
    			olyParams.Device->Logo_Mode = *(LOUD_logo_mode*)pValue;
    			RefreshLogoState();
    #if USE_OLY_UI
    			olyUI.UpdateLogoMode((LOUD_logo_mode)olyParams.Device->Logo_Mode);
    #endif	//	USE_OLY_UI
    			break;
    		case ePID_OLYspeaker1_DEVICE_DISPLAY_MODE:
    			SetDisplayUserMode(*(LOUD_disp_mode*)pValue);
    			break;
    		case ePID_OLYspeaker1_DEVICE_CARDIOID_IDX:
    			SetCardioidIndex(*(uint32_t*)pValue);
    			break;
    		case ePID_OLYspeaker1_DEVICE_INPUT_MODE:
    			SetInputSelectMode(*(LOUD_audio_mode*)pValue);
    			break;
    		case ePID_OLYspeaker1_DEVICE_ACTIVE_USER:
    			SetActivePreset(eTARGET_USER, *(uint32_t*)pValue);
    			break;
    		case ePID_OLYspeaker1_DEVICE_ACTIVE_PROFILE:
    			SetActivePreset(eTARGET_PROFILE, *(uint32_t*)pValue);
    			break;
    		case ePID_OLYspeaker1_DEVICE_ACTIVE_THROW:
    			SetActivePreset(eTARGET_THROW, *(uint32_t*)pValue);
    			break;
    		case ePID_OLYspeaker1_DEVICE_ACTIVE_VOICING:
    			SetActivePreset(eTARGET_VOICING, *(uint32_t*)pValue);
    			break;
    		case ePID_OLYspeaker1_DEVICE_ACTIVE_FIR:
    			SetActivePreset(eTARGET_FIR, *(uint32_t*)pValue);
    			break;
    		case ePID_OLYspeaker1_DEVICE_ACTIVE_XOVER:
    			SetActivePreset(eTARGET_XOVER, *(uint32_t*)pValue);
    			break;
    		case ePID_OLYspeaker1_DEVICE_LF_MUTE:
    		case ePID_OLYspeaker1_DEVICE_MF_MUTE:
    		case ePID_OLYspeaker1_DEVICE_HF_MUTE:
    			olyParams.Device->Values[PID] = *(uint32_t*)pValue;
    			OnMuteSoloChanged();
    			break;
    		case ePID_OLYspeaker1_DEVICE_TEST_LF:
    		case ePID_OLYspeaker1_DEVICE_TEST_MF:
    		case ePID_OLYspeaker1_DEVICE_TEST_HF:
    			olyParams.Device->Values[PID] = *(uint32_t*)pValue;
    			if (olyStatus.SelfTest_Mode == eTEST_MODE_ENABLED) {
    #if USE_OLY_UI
    				olyUI.UpdateSelfTestMode(eTEST_MODE_ENABLED);
    #endif
    				EvaluateChTestStates();
    			}
    			break;
    		case ePID_OLYspeaker1_DEVICE_SOLO_LF:
    		case ePID_OLYspeaker1_DEVICE_SOLO_MF:
    		case ePID_OLYspeaker1_DEVICE_SOLO_HF:
    			olyParams.Device->Values[PID] = *(uint32_t*)pValue;
    			OnMuteSoloChanged();
    			break;
    		case ePID_OLYspeaker1_DEVICE_LCD_BRIGHTNESS:
    			SetLcdBrightness(*(uint32_t*)pValue);
    			break;
    		case ePID_OLYspeaker1_DEVICE_FAN_THRESHOLD:
    			olyParams.Device->Fan_Threshold = *(float32*)pValue;
    			EvaluateFanControl();
    			break;
    		case ePID_OLYspeaker1_DEVICE_FAN_HYSTERESIS:
    			olyParams.Device->Fan_Hysteresis = *(float32*)pValue;
    			EvaluateFanControl();
    			break;
    #if USE_CDD_UI
    		//case ePID_OLYspeaker1_DEVICE_CS_Z_LIMIT_LOW_AMP1:
    		//case ePID_OLYspeaker1_DEVICE_CS_Z_LIMIT_LOW_AMP2:
    		//case ePID_OLYspeaker1_DEVICE_CS_Z_LIMIT_LOW_AMP3:
    		case ePID_OLYspeaker1_DEVICE_CS_Z_LIMIT_LOW_AMP4:
    		//case ePID_OLYspeaker1_DEVICE_CS_Z_LIMIT_HIGH_AMP1:
    		//case ePID_OLYspeaker1_DEVICE_CS_Z_LIMIT_HIGH_AMP2:
    		//case ePID_OLYspeaker1_DEVICE_CS_Z_LIMIT_HIGH_AMP3:
    		//case ePID_OLYspeaker1_DEVICE_CS_Z_LIMIT_HIGH_AMP4:
    			olyParams.Device->Values[PID] = *(uint32_t*)pValue;
    			break;
    #endif
    		case ePID_OLYspeaker1_DEVICE_ARRAY_HEIGHT_M:
    			olyParams.Device->Array_Height_m = *(float32*)pValue;
    #if USE_OLY_UI
    			olyUI.UpdateHeight(olyParams.Device->Array_Height_m);
    #endif	// USE_OLY_UI
    			break;
    		case ePID_OLYspeaker1_DEVICE_ARRAY_DIST_FRONT_M:
    			olyParams.Device->Array_Dist_Front_m = *(float32*)pValue;
    #if USE_OLY_UI
    			olyUI.UpdateFrontDistance(olyParams.Device->Array_Dist_Front_m);
    #endif	// USE_OLY_UI
    			break;
    		case ePID_OLYspeaker1_DEVICE_ARRAY_DIST_REAR_M:
    			olyParams.Device->Array_Dist_Rear_m = *(float32*)pValue;
    #if USE_OLY_UI
    			olyUI.UpdateRearDistance(olyParams.Device->Array_Dist_Rear_m);
    #endif	// USE_OLY_UI
    			break;
    		case ePID_OLYspeaker1_DEVICE_ARRAY_INDEX_LAST:
    			olyParams.Device->Array_Index_Last = *(uint32_t*)pValue;
    			break;
    		case ePID_OLYspeaker1_DEVICE_ARRAY_SIZE_LAST:
    			olyParams.Device->Array_Size_Last = *(uint32_t*)pValue;
    			break;
    		case ePID_OLYspeaker1_DEVICE_ARRAY_WAS_OPTIMIZED:
    			olyParams.Device->Array_Was_Optimized = *(bool32*)pValue;
    			break;
    		case ePID_OLYspeaker1_DEVICE_USE_METRIC:
    			olyParams.Device->Use_Metric = *(uint32_t*)pValue;
    #if USE_OLY_UI
    			olyUI.UpdateUnitType((bool)olyParams.Device->Use_Metric);
    #endif
    			break;
    
    		default:
    			//olyParams.Device->Values[i] = *(uint32_t*)pValue;
    			break;
    	}
    	StoreParams();
    }
    
    
    void Config::SetActivePreset(OLY_target group, uint32_t instance)
    {
#ifdef SC_COMMENTED_OUT
    	switch (group) {
    	case eTARGET_USER:
    		if (instance < USER_PRESETS) {
    #if USE_CDD_UI
    			//remap user presets to actual memory: F1, F2, F3, (F4), U1
    			uint32_t user_preset_remap;
    			user_preset_remap = cddlUI.cddlRemapUser(instance);
    			printf("Loading user preset %d internal %d\n", instance, user_preset_remap);
    			if(user_preset_remap != 1  || (GetUserParamsValidIndicator() != 0xffffffff)) {  //take care of unused user preset
    				olyParams.Device->Active_User = instance;
    				olyParams.Device->Active_Profile = instance;
    
    				//copy  into slot zero to allow dump using rugly - do not need to do in edit if always done here
    				memcpy(olyParams.User[0], olyParams.User[user_preset_remap], sizeof(olyStoredParams.Stored_User_Presets[0]));
    				memcpy(olyParams.Profile[0], olyParams.Profile[user_preset_remap], sizeof(olyStoredParams.Stored_Profile_Presets[0]));
    
    				//now dump the user parameters over to DSP
    //				olyDspPort.WriteMessage(GetSyncMessage(MANDOLIN_SE_SYNC_CURRENT_STATE));
    //				olyDspPort.WriteMessage(SetParameterBlock(eTARGET_USER, 0, 1, olyParams.User[user_preset_remap],OLYspeaker1_USER_PARAMETER_MAX-1));
    //				olyDspPort.WriteMessage(SetParameter(eTARGET_USER, 0, ePID_OLYspeaker1_USER_INPUT_ROUTING, AudioSrc2Routing(ActiveInputSource)));	// Override input source
    				//olyDspPort.WriteMessage(GetSyncMessage(MANDOLIN_SE_SYNC_OK));
    
    				//now dump the profile parameters over to DSP
    				//olyDspPort.WriteMessage(GetSyncMessage(MANDOLIN_SE_SYNC_CURRENT_STATE));
    //				olyDspPort.WriteMessage(SetParameterBlock(eTARGET_PROFILE, user_preset_remap, 1, olyParams.Profile[user_preset_remap],OLYspeaker1_PROFILE_PARAMETER_MAX-1));
    //				olyDspPort.WriteMessage(GetSyncMessage(MANDOLIN_SE_SYNC_OK));
    				cddlUI.UpdateUser(instance);
    			}
    			else {
    				printf("User preset does not exist\n");
    			}
    #else
    			olyParams.Device->Active_User = instance;
    			olyDspPort.WriteMessage(GetSyncMessage(MANDOLIN_SE_SYNC_CURRENT_STATE));
    			olyDspPort.WriteMessage(SetParameterBlock(eTARGET_USER, 0, 1, olyParams.User[instance],OLYspeaker1_USER_PARAMETER_MAX-1));
    			olyDspPort.WriteMessage(SetParameter(eTARGET_USER, 0, ePID_OLYspeaker1_USER_INPUT_ROUTING, AudioSrc2Routing(ActiveInputSource))); // Override input source
    			olyDspPort.WriteMessage(GetSyncMessage(MANDOLIN_SE_SYNC_OK));
    #endif
    
    #if USE_OLY_UI
    			olyUI.UpdateVolume(ParamGetValue(olyParams.Device->Active_User, ePID_OLYspeaker1_USER_SPEAKER_FADER));
    			olyUI.UpdateDelay(ParamGetValue(olyParams.Device->Active_User, ePID_OLYspeaker1_USER_SPEAKER_DELAY_TIME));
    #endif // USE_OLY_UI
    		}
    		break;
    	case eTARGET_PROFILE:
    		if (instance < PROFILE_PRESETS) {
    		//do not allow profile index change for CDD since handled with user preset
    #if USE_OLY_UI
    			olyParams.Device->Active_Profile = instance;
    			olyDspPort.WriteMessage(SetParameterBlock(eTARGET_PROFILE, instance, 1, olyParams.Profile[instance],OLYspeaker1_PROFILE_PARAMETER_MAX-1));
    			olyUI.UpdateActiveProfile((LOUD_oly_profiles)instance);
    #endif // USE_OLY_UI
    		}
    		break;
    	case eTARGET_THROW:
    		if (instance < THROW_PRESETS) {
    			olyParams.Device->Active_Throw = instance;
    
    			//	Sync DSP
    //			olyDspPort.WriteMessage(SetParameterBlock(eTARGET_THROW, instance, 1, olyParams.Throw[instance],OLYspeaker1_THROW_PARAMETER_MAX-1));
    		}
    		break;
    	case eTARGET_XOVER:
    		if (instance < XOVER_PRESETS) {
    			olyParams.Device->Active_Xover = instance;
    //			olyDspPort.WriteMessage(SetParameterBlock(eTARGET_XOVER, instance, 1, olyParams.Xover[instance],OLYspeaker1_XOVER_PARAMETER_MAX-1));
    #if USE_OLY_UI
    			olyUI.UpdateActiveXover((LOUD_xover_type)olyParams.Device->Active_Xover);
    #endif
    		}
    		break;
    	case eTARGET_VOICING:
    		if (instance < VOICING_PRESETS) {
    			olyParams.Device->Active_Voicing = instance;
    //			olyDspPort.WriteMessage(GetSyncMessage(MANDOLIN_SE_SYNC_CURRENT_STATE));
    //			olyDspPort.WriteMessage(SetParameterBlock(eTARGET_VOICING, instance, 1, olyParams.Voicing[instance],OLYspeaker1_VOICING_PARAMETER_MAX-1));
    //			olyDspPort.WriteMessage(GetSyncMessage(MANDOLIN_SE_SYNC_OK));
    		}
    		break;
    	case eTARGET_FIR:
    		if (instance < FIR_PRESETS) {
    			olyParams.Device->Active_FIR = instance;
    //			olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, instance, ePID_OLYspeaker1_FIR_HF_FIR_ENABLE, olyParams.FIR[olyParams.Device->Active_FIR],302));
    			_time_delay(10);
    //			olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, instance, ePID_OLYspeaker1_FIR_HF_FIR_COEF300_VALUE, olyParams.FIR[olyParams.Device->Active_FIR],300));
    			_time_delay(10);
    //			olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, instance, ePID_OLYspeaker1_FIR_MF_FIR_ENABLE, olyParams.FIR[olyParams.Device->Active_FIR],302));
    			_time_delay(10);
    //			olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, instance, ePID_OLYspeaker1_FIR_MF_FIR_COEF300_VALUE, olyParams.FIR[olyParams.Device->Active_FIR],300));
    			_time_delay(10);
    //			olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, instance, ePID_OLYspeaker1_FIR_LF_FIR_ENABLE, olyParams.FIR[olyParams.Device->Active_FIR],302));
    			_time_delay(10);
    //			olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, instance, ePID_OLYspeaker1_FIR_LF_FIR_COEF300_VALUE, olyParams.FIR[olyParams.Device->Active_FIR],300));
    			_time_delay(10);
    		}
    		break;
    	default:
    		printf("ERROR: SET preset failed, invalid GROUP.\n");
    		break;
    	}
    	StoreParams();
#endif // SC_COMMENTED_OUT
    }
    
    #if 1
    void Config::SetInputSelectMode(LOUD_audio_mode mode)
    {
    	olyParams.Device->Input_Mode = mode;
    
    	/* Only update current source if not in Self Test mode */
    	if (olyStatus.SelfTest_Mode == eTEST_MODE_DISABLED)
    	{
    		if (mode == eAUDIO_MODE_AUTO) {
    //			SetAudioSource((oly_audio_source_t)GetDanteAudioSource());
    		}
    		else if (mode == eAUDIO_MODE_ANALOG){
    			SetAudioSource(src_analog);
    			printf("Man: Analog Source Selected\n");
    		}
    		else if (mode == eAUDIO_MODE_DANTE){
    			SetAudioSource(src_dante);
    			printf("Man: Dante Source Selected\n");
    		}
    	}
    	StoreParams();
    }
    
    uint8_t Config::GetInputSelectMode( void )
    {
    	uint8_t temp = (uint8_t) olyParams.Device->Input_Mode;
    	return temp;
    }
    #endif // 0
    
    void Config::SetAudioSource(oly_audio_source_t source)
    {
    	int i;
    
    	LOUD_routingsrc routing = AudioSrc2Routing(source);
    
    	if (ActiveInputSource != source) {
    		ActiveInputSource = source;
    
    		switch (ActiveInputSource) {
    			case(src_analog):
    				printf("Audio: Analog Source Selected\n");
    				break;
    			case(src_dante):
    				printf("Audio: Dante Source Selected\n");
    				break;
    			case(src_pinknoise):
    				printf("Audio: Internal Pink Noise Source Selected\n");
    				break;
    			case(src_sine):
    				printf("Audio: Internal Sine Source Selected\n");
    				break;
    		}
    
    		OnDanteMuteChanged();	// Evaluate mute state
    		olyParams.User[0][ePID_OLYspeaker1_USER_INPUT_ROUTING] = routing; // force user 0 routing to selected value for backward compatibility
    //		olyDspPort.WriteMessage(SetParameter(eTARGET_USER,0,ePID_OLYspeaker1_USER_INPUT_ROUTING, (uint32_t)routing));	// This is the only case where the DSP receives this parameter
    		olyNetworkPort.WriteMessage(SetParameter(eTARGET_USER, 0, ePID_OLYspeaker1_USER_INPUT_ROUTING, (uint32_t)routing));
    	}
    
    #if USE_OLY_UI
    	olyUI.UpdateInputSource(source);
    #endif
    #if USE_CDD_UI
    	cddlUI.UpdateInputSource(source);
    #endif
    }
    
    oly_audio_source_t Config::GetAudioSource( void )
    {
    
    #if 0	// orig
    	uint32_t temp = olyParams.User[olyParams.Device->Active_User][ePID_OLYspeaker1_USER_INPUT_ROUTING];
    	switch( temp ) 	{
    		case eROUTINGSRC_NONE:		return src_none;
    		case eROUTINGSRC_MICPRE:	return src_analog;
    		case eROUTINGSRC_DANTE:		return src_dante;
    		case eROUTINGSRC_GENERATOR:
    			if (olyParams.User[olyParams.Device->Active_User][ePID_OLYspeaker1_USER_INPUT_FUNCGEN_TYPE] == eFUNCTION_SINE)
    				return src_sine;
    			else
    				return src_pinknoise;
    		default:					return src_analog;	}
    #elif 1
    	return ActiveInputSource;
    #else	// Use this when status parameter is available
    	return olyStatus.ActiveInputSource;
    #endif
    
    }
    
    void Config::GetCurrentMeter()
    {
    //	olyDspPort.WriteMessage(RequestMeter(eTARGET_METERS, eMID_OLYspeaker1_INPUT_IN, 0x7f, ++g_nLastMeterSeq));
    }
    
    void Config::SetLogoMeter(float db)
    {
    	float range, threshold;
    	uint32_t ground_index = olyStatus.Array_Size - olyStatus.Array_Index;	// index 0 = bottom speaker
    
    	if (olyParams.Device->Logo_Mode == eLOGO_MODE_SPECIAL)
    	{
    #if USE_OLY_UI
    		range = 60 / olyStatus.Array_Size;
    		threshold = -60 + (range * ground_index);
    		if (db > threshold)
    			LogoSetState(LogoMeterState(olyStatus.Array_Size, ground_index));
    		else
    			LogoSetState(LOGO_OFF);
    #endif
    	}
    }
    
    
    float Config::GetCurrentUIMeter()
    {
    	float temp = olyMeterForUI;
    	olyMeterForUI = FLOAT_NEG_INFINITY;
    #if USE_OLY_UI
    	SetLogoMeter(temp);
    #elif USE_CDD_UI
    #if 0
    	if(olyStatus.Limiting)
    		cddl_show_meter(METER_THRESH_LIMIT);		//force limiter to come on
    	else
    		cddl_show_meter(temp);
    #endif
    #if 0
    	if(olyParams.Device->cs_Z_limit_low_ch4 <= 2.0)  // take care of bad initial state
    		olyParams.Device->cs_Z_limit_low_ch4 = -METER_THRESHOLD_STANDBY;
    	if((temp >= -olyParams.Device->cs_Z_limit_low_ch4)||(olyParams.Device->cs_Z_limit_low_ch4 > -METER_THRESHOLD_STANDBY_DISABLE)){
    		cddl_standby_meter_trigger();
    	}
    #endif
    #endif
    	return temp;
    }
    
    bool Config::MeterRangeCheck(float min, float max) {
    	float meterValue = GetCurrentUIMeter();
    	return ((meterValue > min) && (meterValue < max)) ? true : false;
    }
    
    bool Config::GetAmpFaultStatus(void)
    {
    	return (bool) olyStatus.Amp_Fault;
    }
    
    #if MFG_TEST_EAW || MFG_TEST_MARTIN
    bool Config::Mfg_SetOutputPinValue(uint32_t pinID, uint32_t value) {
    	bool valid = false;
    	switch (pinID) {
    #if MFG_TEST_EAW
    	case OUTPUT_EAW_AMP1_DISABLE_MUTE:
    		GPIO_DRV_WritePinOutput(AMP1_DISABLE_MUTE_CNTRL, value);
    		printf("EAW Mfg: Setting Amp 1 mute to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_EAW_AMP1_BTL4R:
    		GPIO_DRV_WritePinOutput(BTL4R_CONTROL, value);
    		printf("EAW Mfg: Setting Amp 1 BTL4R to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_EAW_AMP1_TEMP_VAC_SEL_CONTROL:
    		GPIO_DRV_WritePinOutput(AMP1_TEMP_VAC_SEL, value);
    		printf("EAW Mfg: Setting Amp 1 Temp Vac Sel to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_EAW_AMP1_STANDBY:
    		GPIO_DRV_WritePinOutput(AMP1_2_STANDBY_CNTRL, value);
    		printf("EAW Mfg: Setting Amp 1 & 2's Standby values to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_EAW_AMP2_DISABLE_MUTE:
    		GPIO_DRV_WritePinOutput(AMP2_DISABLE_MUTE_CNTRL, value);
    		printf("EAW Mfg: Setting Amp 2 mute to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_EAW_AMP2_BTL4R:
    		printf("EAW Mfg: CANNOT SET AMP2 TO BRIDGE MODE!\n");
    		valid = false;
    		break;
    	case OUTPUT_EAW_AMP2_TEMP_VAC_SEL_CONTROL:
    		GPIO_DRV_WritePinOutput(AMP2_TEMP_VAC_SEL, value);
    		printf("EAW Mfg: Setting Amp 2 Temp Vac Sel to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_EAW_AMP2_STANDBY:
    		GPIO_DRV_WritePinOutput(AMP1_2_STANDBY_CNTRL, value);
    		printf("EAW Mfg: Setting Amp 1 & 2's Standby values to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_EAW_DISPLAY_D0:
    		GPIO_DRV_WritePinOutput(DISPLAY_D0, value);
    		printf("EAW Mfg: Setting D0 to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_EAW_DISPLAY_D1:
    		GPIO_DRV_WritePinOutput(DISPLAY_D1, value);
    		printf("EAW Mfg: Setting D1 to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_EAW_DISPLAY_D2:
    		GPIO_DRV_WritePinOutput(DISPLAY_D2, value);
    		printf("EAW Mfg: Setting D2 to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_EAW_DISPLAY_D3:
    		GPIO_DRV_WritePinOutput(DISPLAY_D3, value);
    		printf("EAW Mfg: Setting D3 to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_EAW_DISPLAY_D4:
    		GPIO_DRV_WritePinOutput(DISPLAY_D4, value);
    		printf("EAW Mfg: Setting D4 to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_EAW_DISPLAY_D5:
    		GPIO_DRV_WritePinOutput(DISPLAY_D5, value);
    		printf("EAW Mfg: Setting D5 to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_EAW_DISPLAY_D6:
    		GPIO_DRV_WritePinOutput(DISPLAY_D6, value);
    		printf("EAW Mfg: Setting D6 to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_EAW_FAN_CONTROL:
    		GPIO_DRV_WritePinOutput(FAN_CONTROL, value);
    		printf("EAW Mfg: Setting Fan to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_EAW_LOGO_LED_AMBER:
    		GPIO_DRV_WritePinOutput(LOGO_LED_AMBER, value);
    		printf("EAW Mfg: Setting Amber LED to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_EAW_LOGO_LED_WHITE:
    		GPIO_DRV_WritePinOutput(LOGO_LED_WHITE, value);
    		printf("EAW Mfg: Setting White LED to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_EAW_LOGO_LED_RED:
    		GPIO_DRV_WritePinOutput(LOGO_LED_RED, value);
    		printf("EAW Mfg: Setting Red LED to %d\n", value);
    		valid = true;
    		break;
    #elif MFG_TEST_MARTIN
    	case OUTPUT_MARTIN_AMP1_DISABLE_MUTE:
    		GPIO_DRV_WritePinOutput(AMP1_DISABLE_MUTE_CNTRL, value);
    		printf("Martin Mfg: Setting Amp 1 mute to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_MARTIN_AMP1_BTL4R:
    		GPIO_DRV_WritePinOutput(BTL4R_CONTROL, value);
    		printf("Martin Mfg: Setting Amp 1 BTL4R to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_MARTIN_AMP1_TEMP_VAC_SEL_CONTROL:
    		GPIO_DRV_WritePinOutput(AMP1_TEMP_VAC_SEL, value);
    		printf("Martin Mfg: Setting Amp 1 Temp Vac Sel to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_MARTIN_AMP1_STANDBY:
    		GPIO_DRV_WritePinOutput(AMP1_2_STANDBY_CNTRL, value);
    		printf("Martin Mfg: Setting Amp 1 & 2's Standby values to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_MARTIN_AMP2_DISABLE_MUTE:
    		GPIO_DRV_WritePinOutput(AMP2_DISABLE_MUTE_CNTRL, value);
    		printf("Martin Mfg: Setting Amp 2 mute to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_MARTIN_AMP2_BTL4R:
    		printf("Martin Mfg: CANNOT SET AMP2 TO BRIDGE MODE!\n");
    		valid = true;
    		break;
    	case OUTPUT_MARTIN_AMP2_TEMP_VAC_SEL_CONTROL:
    		GPIO_DRV_WritePinOutput(AMP2_TEMP_VAC_SEL, value);
    		printf("Martin Mfg: Setting Amp 2 Temp Vac Sel to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_MARTIN_AMP2_STANDBY:
    		GPIO_DRV_WritePinOutput(AMP1_2_STANDBY_CNTRL, value);
    		printf("Martin Mfg: Setting Amp 1 & 2's Standby values to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_MARTIN_DISPLAY_D2:
    		GPIO_DRV_WritePinOutput(DISPLAY_D2, value);
    		printf("Martin Mfg: Setting D2 to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_MARTIN_DISPLAY_D3:
    		GPIO_DRV_WritePinOutput(DISPLAY_D3, value);
    		printf("Martin Mfg: Setting D3 to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_MARTIN_DISPLAY_D4:
    		GPIO_DRV_WritePinOutput(DISPLAY_D4, value);
    		printf("Martin Mfg: Setting D4 to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_MARTIN_DISPLAY_D5:
    		GPIO_DRV_WritePinOutput(DISPLAY_D5, value);
    		printf("Martin Mfg: Setting D5 to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_MARTIN_DISPLAY_D6:
    		GPIO_DRV_WritePinOutput(DISPLAY_D6, value);
    		printf("Martin Mfg: Setting D6 to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_MARTIN_FAN_CONTROL:
    		GPIO_DRV_WritePinOutput(FAN_CONTROL, value);
    		printf("Martin Mfg: Setting Fan to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_MARTIN_LOGO_LED_AMBER:
    		GPIO_DRV_WritePinOutput(LOGO_LED_AMBER, value);
    		printf("Martin Mfg: Setting Amber LED to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_MARTIN_LOGO_LED_WHITE:
    		GPIO_DRV_WritePinOutput(LOGO_LED_WHITE, value);
    		printf("Martin Mfg: Setting White LED to %d\n", value);
    		valid = true;
    		break;
    	case OUTPUT_MARTIN_LOGO_LED_RED:
    		GPIO_DRV_WritePinOutput(LOGO_LED_RED, value);
    		printf("Martin Mfg: Setting Red LED to %d\n", value);
    		valid = true;
    		break;
    #endif // MFG_TEST_EAW
    	}
    return valid;
    }
    
    bool Config::Mfg_GetInputPinValue(uint32_t pinID) {
    	bool pinValue = false;
    	printf("Manufacturing Test: Getting value of input pin %d.\n", pinID);
    	switch (pinID) {
    #if MFG_TEST_EAW
    	case INPUT_EAW_AMP1_CH1_CLIP:
    		pinValue = GPIO_DRV_ReadPinInput(AMP1_CH1_CLIP);
    		break;
    	case INPUT_EAW_AMP1_CH2_CLIP:
    		pinValue = GPIO_DRV_ReadPinInput(AMP1_CH2_CLIP);
    		break;
    	case INPUT_EAW_AMP1_PROTECT:
    		pinValue = GPIO_DRV_ReadPinInput(AMP1_PROTECT);
    		break;
    	case INPUT_EAW_AMP2_CH1_CLIP:
    		pinValue = GPIO_DRV_ReadPinInput(AMP2_CH1_CLIP);
    		break;
    	case INPUT_EAW_AMP2_CH2_CLIP:
    		pinValue = GPIO_DRV_ReadPinInput(AMP2_CH2_CLIP);
    		break;
    	case INPUT_EAW_AMP2_PROTECT:
    		pinValue = GPIO_DRV_ReadPinInput(AMP2_PROTECT);
    		break;
    #elif MFG_TEST_MARTIN
    	case INPUT_MARTIN_AMP1_CH1_CLIP:
    		pinValue = GPIO_DRV_ReadPinInput(AMP1_CH1_CLIP);
    		break;
    	case INPUT_MARTIN_AMP1_CH2_CLIP:
    		pinValue = GPIO_DRV_ReadPinInput(AMP1_CH2_CLIP);
    		break;
    	case INPUT_MARTIN_AMP1_PROTECT:
    		pinValue = GPIO_DRV_ReadPinInput(AMP1_PROTECT);
    		break;
    	case INPUT_MARTIN_AMP2_CH1_CLIP:
    		pinValue = GPIO_DRV_ReadPinInput(AMP2_CH1_CLIP);
    		break;
    	case INPUT_MARTIN_AMP2_CH2_CLIP:
    		pinValue = GPIO_DRV_ReadPinInput(AMP2_CH2_CLIP);
    		break;
    	case INPUT_MARTIN_AMP2_PROTECT:
    		pinValue = GPIO_DRV_ReadPinInput(AMP2_PROTECT);
    		break;
    	case INPUT_MARTIN_DISPLAY_D0:
    		// "SW2" actually corresponds to the D0 pin for Martin
    		pinValue = GPIO_DRV_ReadPinInput(SW2);
    		break;
    	case INPUT_MARTIN_DISPLAY_D1:
    		// "UI_PB_1" actually corresponds to the D1 pin for Martin
    		pinValue = GPIO_DRV_ReadPinInput(UI_PB_1);
    		break;
    #endif // MFG_TEST_EAW
    	}
    	return pinValue;
    }
    
    void Config::Mfg_SetAudioTestSource(oly_audio_source_t source)
    {
    	mandolin_parameter_value routing, gen_type;
    	if(source == src_pinknoise)	{
    		routing.i = eROUTINGSRC_GENERATOR;
    		gen_type.i = eFUNCTION_PINKNOISE;
    		printf("Audio: Internal Pink Noise Source Selected\n");
    	}
    	else if(source == src_sine)	{
    		routing.i = eROUTINGSRC_GENERATOR;
    		gen_type.i = eFUNCTION_SINE;
    		printf("Audio: Internal Sine Wave Source Selected\n");
    	}
    
    	for(int i=0;i<USER_PRESETS;i++) {
    		ParamSetUser(i+1, ePID_OLYspeaker1_USER_INPUT_ROUTING, routing);
    		ParamSetUser(i+1, ePID_OLYspeaker1_USER_INPUT_FUNCGEN_TYPE, gen_type);
    	}
    
    #if USE_OLY_UI
    	olyUI.UpdateInputSource(src_sine);
    #endif
    #if USE_CDD_UI
    	cddlUI.UpdateInputSource(src_sine);
    #endif
    
    }
    
    void Config::Mfg_SetFunctionGenerator(mandolin_parameter_value fader, mandolin_parameter_value frequency)
    {
    	for(int i=0;i<USER_PRESETS;i++) {
    		ParamSetUser(i+1, ePID_OLYspeaker1_USER_INPUT_FUNCGEN_FADER, fader);
    		ParamSetUser(i+1, ePID_OLYspeaker1_USER_INPUT_FUNCGEN_FREQ, frequency);
    	}
    }
    
    #endif // MFG_TEST_EAW || MFG_TEST_MARTIN
    
    //hardcode dac channel for now
    float Config::CurrentSenseGetOutputMeter(uint32_t channelNumber)
    {
    	float ret;
    	if ((channelNumber>=0) && (channelNumber<=2))
    	{
    		ret = olyCurrentSenseMeter[channelNumber].f;
    		olyCurrentSenseMeter[channelNumber].f = FLOAT_NEG_INFINITY;
    	}
    	else
    		ret = -100.0;
    
    	return ret;
    }
    
    
    
    void Config::CurrentSenseGetImpedanceLimits(current_sense_Z_limits_t * current_sense_Z_limits)
    {
    	current_sense_Z_limits[0].openLowerLimit = olyParams.Device->cs_Z_limit_low_ch1;
    	current_sense_Z_limits[1].openLowerLimit = olyParams.Device->cs_Z_limit_low_ch2;
    	current_sense_Z_limits[2].openLowerLimit = olyParams.Device->cs_Z_limit_low_ch3;
    
    	current_sense_Z_limits[0].shortUpperLimit = olyParams.Device->cs_Z_limit_high_ch1;
    	current_sense_Z_limits[1].shortUpperLimit = olyParams.Device->cs_Z_limit_high_ch2;
    	current_sense_Z_limits[2].shortUpperLimit = olyParams.Device->cs_Z_limit_high_ch3;
    }
    
    
    void Config::OnCurrentSenseImpedanceChanged(uint32_t channelNumber)
    {
    	if(channelNumber == 0) {
    		olyStatus.cs_Z_ch1 = current_sense_Z_result[channelNumber].impedanceAveraged;
    		olyStatus.cs_I_ch1 = current_sense_Z_result[channelNumber].current;
    		olyStatus.cs_V_ch1 = current_sense_Z_result[channelNumber].voltage;
    
    		if (olyStatus.cs_fault_ch1 != current_sense_Z_result[channelNumber].fault_status)
    		{
    			olyStatus.cs_fault_ch1 = current_sense_Z_result[channelNumber].fault_status;
    			olyNetworkPort.WriteMessage(SetParameter(eTARGET_STATUS, 0, ePID_OLYspeaker1_STATUS_CS_FAULT_CH1, olyStatus.Values[ePID_OLYspeaker1_STATUS_CS_FAULT_CH1]), true);
    		}
    	}
    	else if(channelNumber == 1) {
    		olyStatus.cs_Z_ch2 = current_sense_Z_result[channelNumber].impedanceAveraged;
    		olyStatus.cs_I_ch2 = current_sense_Z_result[channelNumber].current;
    		olyStatus.cs_V_ch2 = current_sense_Z_result[channelNumber].voltage;
    		if (olyStatus.cs_fault_ch2 != current_sense_Z_result[channelNumber].fault_status)
    		{
    			olyStatus.cs_fault_ch2 = current_sense_Z_result[channelNumber].fault_status;
    			olyNetworkPort.WriteMessage(SetParameter(eTARGET_STATUS, 0, ePID_OLYspeaker1_STATUS_CS_FAULT_CH2, olyStatus.Values[ePID_OLYspeaker1_STATUS_CS_FAULT_CH2]), true);
    		}
    	}
    	else if(channelNumber == 2) {
    		olyStatus.cs_Z_ch3 = current_sense_Z_result[channelNumber].impedanceAveraged;
    		olyStatus.cs_I_ch3 = current_sense_Z_result[channelNumber].current;
    		olyStatus.cs_V_ch3 = current_sense_Z_result[channelNumber].voltage;
    		if (olyStatus.cs_fault_ch3 != current_sense_Z_result[channelNumber].fault_status)
    		{
    			olyStatus.cs_fault_ch3 = current_sense_Z_result[channelNumber].fault_status;
    			olyNetworkPort.WriteMessage(SetParameter(eTARGET_STATUS, 0, ePID_OLYspeaker1_STATUS_CS_FAULT_CH3, olyStatus.Values[ePID_OLYspeaker1_STATUS_CS_FAULT_CH3]), true);
    		}
    	}
    }
    
    void Config::OnAmpFaultStatusChanged(bool fault)
    {
    	if (fault != olyStatus.Amp_Fault)
    	{
    		olyStatus.Amp_Fault = fault;
    		olyNetworkPort.WriteMessage(SetParameter(eTARGET_STATUS, 0, ePID_OLYspeaker1_STATUS_AMP_FAULT, olyStatus.Values[ePID_OLYspeaker1_STATUS_AMP_FAULT]), true);
    	}
    
    	RefreshLogoState();
    
    #if USE_OLY_UI
    	olyUI.UpdateFaultIndicator(fault);
    	if(fault)	olyUI.IdleTimer_Reset();
    #endif
    }
    
    
    void Config::OnLimiterStatusChanged(bool limiting)
    {
    	if (limiting != olyStatus.Limiting)
    	{
    		olyStatus.Limiting = limiting;
    		olyNetworkPort.WriteMessage(SetParameter(eTARGET_STATUS, 0, ePID_OLYspeaker1_STATUS_LIMITING, olyStatus.Values[ePID_OLYspeaker1_STATUS_LIMITING]), true);
    	}
    
    	RefreshLogoState();
    }
    
    
    void Config::OnAmpTempChanged(amp_instance_t amp, float32 temp)
    {
    	if (amp == sPro2_amp1) {
    		if (temp != olyStatus.Amp1_Temp)
    		{
    			olyStatus.Amp1_Temp = temp;
    			olyNetworkPort.WriteMessage(SetParameter(eTARGET_STATUS, 0, ePID_OLYspeaker1_STATUS_AMP1_TEMP, olyStatus.Values[ePID_OLYspeaker1_STATUS_AMP1_TEMP]), true);
    		}
    	}
    
    	else if (amp == sPro2_amp2) {
    		if (temp != olyStatus.Amp2_Temp)
    		{
    			olyStatus.Amp2_Temp = temp;
    #ifndef USE_OLY_UI
    			olyNetworkPort.WriteMessage(SetParameter(eTARGET_STATUS, 0, ePID_OLYspeaker1_STATUS_AMP2_TEMP, olyStatus.Values[ePID_OLYspeaker1_STATUS_AMP2_TEMP]), true);
    #endif
    		}
    	}
    
    	EvaluateFanControl();
    
    #if USE_OLY_UI
    	if (amp == sPro2_amp2) {
    		if(olyStatus.Amp2_Temp >= olyStatus.Amp1_Temp) {
    			olyUI.UpdateTemperature(olyStatus.Amp2_Temp);
    		}
    		else {
    			olyUI.UpdateTemperature(olyStatus.Amp1_Temp);
    		}
    	}
    #endif
    }
    
    
    void Config::EvaluateFanControl()
    {
    	float tempLimit_upper = olyParams.Device->Fan_Threshold + (olyParams.Device->Fan_Hysteresis / 2);
    	float tempLimit_lower = olyParams.Device->Fan_Threshold - (olyParams.Device->Fan_Hysteresis / 2);
    
    	if( !olyStatus.Fan_Enabled && ((olyStatus.Amp1_Temp > tempLimit_upper) || (olyStatus.Amp1_Temp > tempLimit_upper) ) )	{
    		SetFanEnabled(true);
    	}
    
    	if( olyStatus.Fan_Enabled && ( (olyStatus.Amp1_Temp < tempLimit_lower) && (olyStatus.Amp2_Temp < tempLimit_lower) ) )	{
    		SetFanEnabled(false);
    	}
    }
    
    void Config::SetCardioidIndex(uint32_t indexNum)
    {
    	olyParams.Device->Cardioid_Index = (int32)indexNum;
    #if USE_OLY_UI
    	olyUI.UpdateCardioidIndex(olyParams.Device->Cardioid_Index);
    #endif
    }
    
    void Config::SetDisplayUserMode(LOUD_disp_mode display_mode)
    {
    	olyParams.Device->Display_Mode = (int32) display_mode;
    #if USE_OLY_UI
    	olyUI.UpdateDisplayMode((LOUD_disp_mode)olyParams.Device->Display_Mode);
    #endif	//	USE_OLY_UI
    }
    
    LOUD_disp_mode Config::GetDisplayUserMode(void)
    {
    	return (LOUD_disp_mode)olyParams.Device->Display_Mode;
    }
    
    void Config::RefreshLogoState() {
#ifdef SC_COMMENTED_OUT
    #if !MFG_TEST_EAW && !MFG_TEST_MARTIN
    #if USE_OLY_UI
    	if (olyStatus.Amp_Fault)
    	{
    		olyStatus.Logo_State = LOGO_RUN_STATE_FAULT;
    	}
    	else if (olyStatus.Identify)
    	{
    		if (olyStatus.Identify == eIDENTIFY_MODE_ADVERTISE)
    			olyStatus.Logo_State = LOGO_RUN_STATE_ADVERTISE;
    		else if (olyStatus.Identify == eIDENTIFY_MODE_IDENTIFY)
    			olyStatus.Logo_State = LOGO_RUN_STATE_IDENTIFY;
    		else if (olyStatus.Identify == eIDENTIFY_MODE_CONNECTED)
    			olyStatus.Logo_State = LOGO_RUN_STATE_IDENTIFY_ALT;
    		else
    			olyStatus.Logo_State = LOGO_RUN_STATE_IDENTIFY;
    	}
    	else if (olyStatus.Limiting)
    	{
    		olyStatus.Logo_State = LOGO_RUN_STATE_LIMITING;
    	}
    	else
    	{
    		olyStatus.Logo_State = LOGO_RUN_STATE_NORMAL;
    	}
    #endif
    	LogoUpdateRunState((oly_Logo_Run_State_t)olyStatus.Logo_State, (LOUD_logo_mode)olyParams.Device->Logo_Mode);
    #elif MFG_TEST_MARTIN
    	LogoSetState(LOGO_OFF);
    #endif // !MFG_TEST_EAW && !MFG_TEST_MARTIN
#endif //  SC_COMMENTED_OUT
    }
    
    void Config::DSPErrorTimer_Cancel()
    {
    	_timer_cancel( dsp_error_timer );
    }
    
    void Config::DSPErrorTimer_Reset()
    {
    	if( dsp_error_timer )	_timer_cancel( dsp_error_timer );
    	dsp_error_timer = _timer_start_oneshot_after_ticks(TriggerDSPError, 0, TIMER_ELAPSED_TIME_MODE, &dsp_error_ticks);
    }
    
    void Config::IdentifyStart(uint32_t ms, LOUD_identify_mode mode) {
    
    	if (mode!=olyStatus.Identify)
    	{
    		olyStatus.Identify = mode;
    		olyNetworkPort.WriteMessage(SetParameter(eTARGET_STATUS, 0, ePID_OLYspeaker1_STATUS_IDENTIFY, olyStatus.Values[ePID_OLYspeaker1_STATUS_IDENTIFY]), true);
    	}
    
    	RefreshLogoState();
    
    	if (identify_timer)
    		_timer_cancel(identify_timer);
    
    	if (ms != IDENTIFY_FOREVER)
    	{
    		printf("Identify type %d started for %d ms\n",mode, ms);
    		_time_init_ticks(&identify_ticks, 0);
    		_time_add_msec_to_ticks(&identify_ticks, ms);
    		identify_timer = _timer_start_oneshot_after_ticks(TriggerIdentifyStop, 0, TIMER_ELAPSED_TIME_MODE, &identify_ticks);
    	}else {
    		printf("Identify type %d started for FOREVER\n",mode);
    	}
    
    	_lwevent_clear(&user_event, event_IdentifyStop);
    }
    
    void Config::IdentifyStop() {
    
    	if (olyStatus.Identify)
    	{
    		olyStatus.Identify = 0;
    		olyNetworkPort.WriteMessage(SetParameter(eTARGET_STATUS, 0, ePID_OLYspeaker1_STATUS_IDENTIFY, olyStatus.Values[ePID_OLYspeaker1_STATUS_IDENTIFY]), true);
    	}
    
    	printf("Identify stopped\n");
    	if ( identify_timer )
    		_timer_cancel( identify_timer );
    
    	RefreshLogoState();
    }
    
    LOUD_identify_mode Config::GetIdentifyMode()
    {
    	return (LOUD_identify_mode) olyStatus.Identify;
    }
    
    void Config::SetSelfTestSequence(dsp_channels_t sequence) {
    
    	if (sequence != olyStatus.SelfTest_Seq)
    	{
    		olyStatus.SelfTest_Seq = sequence;
    		olyNetworkPort.WriteMessage(SetParameter(eTARGET_STATUS, 0, ePID_OLYspeaker1_STATUS_SELFTEST_SEQ, olyStatus.Values[ePID_OLYspeaker1_STATUS_SELFTEST_SEQ]), true);
    	}
    
    #if USE_OLY_UI
    	olyUI.UpdateSelfTestSequence(sequence);
    #endif
    }
    
    void Config::SaveTestValues()
    {
    	previous_hf_test_value = olyParams.Device->HF_Test;
    	previous_mf_test_value = olyParams.Device->MF_Test;
    	previous_lf_test_value = olyParams.Device->LF_Test;
    }
    
    void Config::RestoreTestValues()
    {
    	SetDeviceHfTest(previous_hf_test_value);
    	SetDeviceMfTest(previous_mf_test_value);
    	SetDeviceLfTest(previous_lf_test_value);
    }
    
    void Config::SaveDriverValues()
    {
    	previous_hf_mute_value = olyParams.Device->HF_Mute;
    	previous_hf_solo_value = olyParams.Device->HF_Solo;
    
    	previous_mf_mute_value = olyParams.Device->MF_Mute;
    	previous_mf_solo_value = olyParams.Device->MF_Solo;
    
    	previous_lf_mute_value = olyParams.Device->LF_Mute;
    	previous_lf_solo_value = olyParams.Device->LF_Solo;
    }
    
    void Config::RestoreDriverValues()
    {
    	printf("Restoring driver values.\n");
    	SetHFMute(previous_hf_mute_value);
    	SetHFSolo(previous_hf_solo_value);
    
    	SetMFMute(previous_mf_mute_value);
    	SetMFSolo(previous_mf_solo_value);
    
    	SetLFMute(previous_lf_mute_value);
    	SetLFSolo(previous_lf_solo_value);
    
    	OnMuteSoloChanged();
    }
    
    void Config::OnSelfTestModeChanged(LOUD_test_mode mode) {
    	if (mode != olyStatus.SelfTest_Mode) {
    
    		// if not jumping out of sequence and not jumping out of disabled, save test values
    		if (olyStatus.SelfTest_Mode != eTEST_MODE_SEQUENCE && olyStatus.SelfTest_Mode != eTEST_MODE_DISABLED)
    			SaveTestValues();
    
    		// if jumping out of disabled, save driver values
    		if (olyStatus.SelfTest_Mode == eTEST_MODE_DISABLED)
    			SaveDriverValues();
    
    		// if jumping out of sequence, restore test values
    		if (olyStatus.SelfTest_Mode == eTEST_MODE_SEQUENCE)
    			RestoreTestValues();
    
    		// if jumping into disabled, restore driver values
    		if (mode == eTEST_MODE_DISABLED)
    			RestoreDriverValues();
    
    		// if jumping into sequence, save the current test mode
    		if (mode == eTEST_MODE_SEQUENCE)
    			previous_self_test_mode = (LOUD_test_mode) olyStatus.SelfTest_Mode;
    
    		printf("Setting self test mode to %d\n", mode);
    		olyStatus.SelfTest_Mode = mode;
    		olyNetworkPort.WriteMessage(SetParameter(eTARGET_STATUS, 0, ePID_OLYspeaker1_STATUS_SELFTEST_MODE, olyStatus.Values[ePID_OLYspeaker1_STATUS_SELFTEST_MODE]), true);
    	}
    #if USE_OLY_UI
    	olyUI.UpdateSelfTestMode(mode);
    #endif
    }
    
    dsp_channels_t Config::GetSequenceStart() {
    	if (HAS_HF)			return ch_HF;
    	else if (HAS_MF)	return ch_MF;
    	else				return ch_LF;
    }
    
    LOUD_test_mode Config::GetSelfTestMode(void)
    {
    	return (LOUD_test_mode) olyStatus.SelfTest_Mode;
    }
    
    void Config::SetSelfTestMode(LOUD_test_mode test_mode)
    {
    	if (test_mode < eTEST_MODE_FENCE)
    		OnSelfTestModeChanged(test_mode);
    
    	switch(test_mode) {
    		case(eTEST_MODE_DISABLED):
    			startSelfTest = false;
    			destroySelfTestTimer = true;
    			SetGlobalMute(false);
    			//olyParams.Voicing[i] = olyStoredParams.Stored_Voicing_Presets[i];
    			SetInputSelectMode((LOUD_audio_mode)olyParams.Device->Input_Mode);	// Restore stored input
    			break;
    		case(eTEST_MODE_ENABLED):
    			startSelfTest = false;
    			destroySelfTestTimer = true;
    			EvaluateChTestStates();
    			SetAudioSource(src_pinknoise);
    			break;
    		case(eTEST_MODE_MUTE_FROM_SOLO):
    			startSelfTest = false;
    			destroySelfTestTimer = true;
    			// disconnecting master rig should restore speaker state (it shouldn't get stuck in mute)
    			// use global mute, or individual driver mutes? does it matter?
    			//SelfTestEnableChannel(ch_none);
    			SetGlobalMute(true);
    			break;
    		case(eTEST_MODE_SEQUENCE):
    			m_bCycle = false;
    			SelfTestEnableChannel(ch_none);	// mute all channels
    			SetAudioSource(src_pinknoise);
    			startSelfTest = true;
    			destroySelfTestTimer = false;
    			SetSelfTestSequence(GetSequenceStart());
    			SelfTestSequence();
    			break;
    		case(eTEST_MODE_CYCLE):
    			m_bCycle = true;
    			SelfTestEnableChannel(ch_none);	// mute all channels
    			SetAudioSource(src_pinknoise);
    			startSelfTest = true;
    			SetSelfTestSequence(GetSequenceStart());
    			SelfTestSequence();
    			break;
    		default:
    			break;
    	}
    	StoreParams();
    }
    
    void Config::ResetSelfTestTimer()
    {
    	if(selfTest_timer)	DestroySelfTestTimer();
    	selfTest_timer = _timer_start_periodic_every_ticks(TriggerSelfTestSwitch, 0, TIMER_ELAPSED_TIME_MODE, &selfTest_ticks);
    }
    
    void Config::DestroySelfTestTimer()
    {
    	_timer_cancel( selfTest_timer );
    }
    
    void Config::SelfTestSequence()
    {
    	if (startSelfTest) {
    		ResetSelfTestTimer();
    		startSelfTest = false;
    	}
    	else if (destroySelfTestTimer) {
    		// destroy timer, restore self test mode, restore test values, unmute channels
    		DestroySelfTestTimer();
    		SetSelfTestMode(previous_self_test_mode);
    		destroySelfTestTimer = false;
    	}
    	else {
    		// every time we enter the sequence, mute channels and set test bools to false
    		SelfTestEnableChannel(ch_none);
    		SetDeviceHfTest(false);
    		SetDeviceMfTest(false);
    		SetDeviceLfTest(false);
    		switch (olyStatus.SelfTest_Seq) {
    		case ch_none:	// all drivers off
    			SetSelfTestSequence(GetSequenceStart());
    			SelfTestEnableChannel(ch_none);
    			break;
    		case ch_HF:
    			SetSelfTestSequence(ch_HF);
    			if (HAS_MF) {
    				olyStatus.SelfTest_Seq = ch_MF;
    			} else {
    				olyStatus.SelfTest_Seq = ch_LF;
    			}
    			SelfTestEnableChannel(ch_HF);
    			SetDeviceHfTest(true);
    			break;
    		case ch_MF:
    			SetSelfTestSequence(ch_MF);
    			if (HAS_LF) {
    				olyStatus.SelfTest_Seq = ch_LF;
    			} else {
    				if (m_bCycle) {
    					olyStatus.SelfTest_Seq = ch_none;
    				} else {
    					olyStatus.SelfTest_Seq = ch_All;
    				}
    			}
    			SelfTestEnableChannel(ch_MF);
    			SetDeviceMfTest(true);
    			break;
    		case ch_LF:
    			SetSelfTestSequence(ch_LF);
    			if (!m_bCycle) {
    				if (!HAS_HF && !HAS_MF) {
    					startSelfTest = false;
    					destroySelfTestTimer = true;
    				} else {
    					olyStatus.SelfTest_Seq = ch_All;
    				}
    			} else {
    				olyStatus.SelfTest_Seq = ch_none;
    			}
    			SelfTestEnableChannel(ch_LF);
    			SetDeviceLfTest(true);
    			break;
    		case ch_All:	// final full range test
    			SetSelfTestSequence(ch_All);
    			SelfTestEnableChannel(ch_All);	// un-mute all channels
    			SetDeviceHfTest(true);
    			SetDeviceMfTest(true);
    			SetDeviceLfTest(true);
    			startSelfTest = false;
    			destroySelfTestTimer = true;
    			break;
    		default:
    			break;
    		}
    	}
    }
    
    void Config::SetDeviceHfTest(bool isActive)
    {
    	if (isActive != olyParams.Device->HF_Test)
    	{
    		printf("Setting HF test to %d\n", isActive);
    		olyParams.Device->HF_Test = isActive;
    		olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_TEST_HF, olyParams.Device->Values[ePID_OLYspeaker1_DEVICE_TEST_HF]), true);
    	}
    }
    
    bool32 Config::GetDeviceHfTest(void)
    {
    	return olyParams.Device->HF_Test;
    }
    
    void Config::SetDeviceMfTest(bool isActive)
    {
    	if (isActive != olyParams.Device->MF_Test)
    	{
    		printf("Setting MF test to %d\n", isActive);
    		olyParams.Device->MF_Test = isActive;
    		olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_TEST_MF, olyParams.Device->Values[ePID_OLYspeaker1_DEVICE_TEST_MF]), true);
    	}
    }
    
    bool32 Config::GetDeviceMfTest(void)
    {
    	return olyParams.Device->MF_Test;
    }
    
    void Config::SetDeviceLfTest(bool isActive)
    {
    	if (isActive != olyParams.Device->LF_Test)
    	{
    		printf("Setting LF test to %d\n", isActive);
    		olyParams.Device->LF_Test = isActive;
    		olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_TEST_LF, olyParams.Device->Values[ePID_OLYspeaker1_DEVICE_TEST_LF]), true);
    	}
    }
    
    bool32 Config::GetDeviceLfTest(void)
    {
    	return olyParams.Device->LF_Test;
    }
    
    void Config::EvaluateChTestStates()
    {
    	SelfTestEnableChannel(ch_none);			// mute all channels
    
    	if (olyParams.Device->HF_Solo || olyParams.Device->MF_Solo || olyParams.Device->LF_Solo) {
    		if (olyParams.Device->HF_Test && olyParams.Device->HF_Solo) SelfTestEnableChannel(ch_HF);
    		if (olyParams.Device->MF_Test && olyParams.Device->MF_Solo) SelfTestEnableChannel(ch_MF);
    		if (olyParams.Device->LF_Test && olyParams.Device->LF_Solo) SelfTestEnableChannel(ch_LF);
    	} else {
    		if (olyParams.Device->HF_Test && !olyParams.Device->HF_Mute) SelfTestEnableChannel(ch_HF);
    		if (olyParams.Device->MF_Test && !olyParams.Device->HF_Mute) SelfTestEnableChannel(ch_MF);
    		if (olyParams.Device->LF_Test && !olyParams.Device->HF_Mute) SelfTestEnableChannel(ch_LF);
    	}
    }
    
    void Config::SelfTestEnableChannel(int channel)
    {
    	switch (channel) {
    	case ch_none:
    		olyDspPort.WriteMessage(SetParameter(eTARGET_VOICING, 0, ePID_OLYspeaker1_VOICING_HF_MUTE,1));
    		olyDspPort.WriteMessage(SetParameter(eTARGET_VOICING, 0, ePID_OLYspeaker1_VOICING_MF_MUTE,1));
    		olyDspPort.WriteMessage(SetParameter(eTARGET_VOICING, 0, ePID_OLYspeaker1_VOICING_LF_MUTE,1));
    		break;
    	case ch_HF:
    		olyDspPort.WriteMessage(SetParameter(eTARGET_VOICING, 0, ePID_OLYspeaker1_VOICING_HF_MUTE,0));
    		break;
    	case ch_MF:
    		olyDspPort.WriteMessage(SetParameter(eTARGET_VOICING, 0, ePID_OLYspeaker1_VOICING_MF_MUTE,0));
    		break;
    	case ch_LF:
    		olyDspPort.WriteMessage(SetParameter(eTARGET_VOICING, 0, ePID_OLYspeaker1_VOICING_LF_MUTE,0));
    		break;
    	case ch_All:
    		olyDspPort.WriteMessage(SetParameter(eTARGET_VOICING, 0, ePID_OLYspeaker1_VOICING_HF_MUTE,0));
    		olyDspPort.WriteMessage(SetParameter(eTARGET_VOICING, 0, ePID_OLYspeaker1_VOICING_MF_MUTE,0));
    		olyDspPort.WriteMessage(SetParameter(eTARGET_VOICING, 0, ePID_OLYspeaker1_VOICING_LF_MUTE,0));
    		break;
    	}
    }
    
    void Config::SetHFMute(bool mute)
    {
    	if (mute != olyParams.Device->HF_Mute)
    	{
    		olyParams.Device->HF_Mute = mute;
    		olyDspPort.WriteMessage(SetParameter(eTARGET_VOICING, 0, ePID_OLYspeaker1_VOICING_HF_MUTE,olyParams.Device->Values[ePID_OLYspeaker1_DEVICE_HF_MUTE]));
    		olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_HF_MUTE, olyParams.Device->Values[ePID_OLYspeaker1_DEVICE_HF_MUTE]), true);
    	}
    }
    
    void Config::SetMFMute(bool mute)
    {
    	if (mute != olyParams.Device->MF_Mute)
    	{
    		olyParams.Device->MF_Mute = mute;
    		olyDspPort.WriteMessage(SetParameter(eTARGET_VOICING, 0, ePID_OLYspeaker1_VOICING_MF_MUTE,olyParams.Device->Values[ePID_OLYspeaker1_DEVICE_MF_MUTE]));
    		olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_MF_MUTE, olyParams.Device->Values[ePID_OLYspeaker1_DEVICE_MF_MUTE]), true);
    	}
    }
    
    void Config::SetLFMute(bool mute)
    {
    	if (mute != olyParams.Device->LF_Mute)
    	{
    		olyParams.Device->LF_Mute = mute;
    		olyDspPort.WriteMessage(SetParameter(eTARGET_VOICING, 0, ePID_OLYspeaker1_VOICING_LF_MUTE,olyParams.Device->Values[ePID_OLYspeaker1_DEVICE_LF_MUTE]));
    		olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_LF_MUTE, olyParams.Device->Values[ePID_OLYspeaker1_DEVICE_LF_MUTE]), true);
    	}
    }
    
    void Config::SetHFSolo(bool solo)
    {
    	if (solo != olyParams.Device->HF_Solo)
    	{
    		olyParams.Device->HF_Solo = solo;
    		olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_SOLO_HF, olyParams.Device->Values[ePID_OLYspeaker1_DEVICE_SOLO_HF]), true);
    	}
    }
    
    void Config::SetMFSolo(bool solo)
    {
    	if (solo != olyParams.Device->MF_Solo)
    	{
    		olyParams.Device->MF_Solo = solo;
    		olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_SOLO_MF, olyParams.Device->Values[ePID_OLYspeaker1_DEVICE_SOLO_MF]), true);
    	}
    }
    
    void Config::SetLFSolo(bool solo)
    {
    	if (solo != olyParams.Device->LF_Solo)
    	{
    		olyParams.Device->LF_Solo = solo;
    		olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_SOLO_LF, olyParams.Device->Values[ePID_OLYspeaker1_DEVICE_SOLO_LF]), true);
    	}
    }
    
    void Config::OnMuteSoloChanged() {
    	if (olyParams.Device->HF_Solo || olyParams.Device->MF_Solo || olyParams.Device->LF_Solo) {
    		olyDspPort.WriteMessage(SetParameter(eTARGET_VOICING, 0, ePID_OLYspeaker1_VOICING_HF_MUTE, !olyParams.Device->HF_Solo));
    		olyDspPort.WriteMessage(SetParameter(eTARGET_VOICING, 0, ePID_OLYspeaker1_VOICING_MF_MUTE, !olyParams.Device->MF_Solo));
    		olyDspPort.WriteMessage(SetParameter(eTARGET_VOICING, 0, ePID_OLYspeaker1_VOICING_LF_MUTE, !olyParams.Device->LF_Solo));
    	} else {
    		olyDspPort.WriteMessage(SetParameter(eTARGET_VOICING, 0, ePID_OLYspeaker1_VOICING_HF_MUTE, olyParams.Device->HF_Mute));
    		olyDspPort.WriteMessage(SetParameter(eTARGET_VOICING, 0, ePID_OLYspeaker1_VOICING_MF_MUTE, olyParams.Device->MF_Mute));
    		olyDspPort.WriteMessage(SetParameter(eTARGET_VOICING, 0, ePID_OLYspeaker1_VOICING_LF_MUTE, olyParams.Device->LF_Mute));
    	}
    }
    
    void Config::ParamSetUser(uint32_t instance, OLYspeaker1_USER_pid PID, mandolin_parameter_value Value)
    {
    	if (instance == 0)
    		instance = olyParams.Device->Active_User;
    	else
    		instance--;
    
    	if (instance < USER_PRESETS_STORED)
    		olyParams.User[instance][PID] = Value.u;
    
    	if ((instance == olyParams.Device->Active_User) && (PID != ePID_OLYspeaker1_USER_INPUT_ROUTING)) // INPUT_ROUTING should only be set by SetAudioSource
    		olyDspPort.WriteMessage(SetParameter(eTARGET_USER,0,PID,Value.u));
    
    	StoreParams();
    
    #if USE_OLY_UI
    	/* Perform any UI-related Updates */
    	switch (PID){
    		case(ePID_OLYspeaker1_USER_SPEAKER_FADER):
    			olyUI.UpdateVolume(Value.f);
    			break;
    		case(ePID_OLYspeaker1_USER_SPEAKER_DELAY_TIME):
    			olyUI.UpdateDelay(Value.f);
    			break;
    		case(ePID_OLYspeaker1_USER_SPEAKER_INVERT):
    			olyUI.UpdatePolarity(Value.b);
    			break;
    	}
    #endif
    }
    
    void Config::ParamSetVoicing(uint32_t instance, OLYspeaker1_VOICING_pid PID, uint32_t Value)
    {
    	if (instance == 0)
    		instance = olyParams.Device->Active_Voicing;
    	else
    		instance--;
    
    	if (instance < VOICING_PRESETS_STORED)
    		olyParams.Voicing[instance][PID] = Value;
    
    	if (instance == olyParams.Device->Active_Voicing)
    		olyDspPort.WriteMessage(SetParameter(eTARGET_VOICING,0,PID,Value));
    
    	StoreParams();
    }
    
    void Config::ParamSetFIR(uint32_t instance, OLYspeaker1_FIR_pid PID, uint32_t Value)
    {
    	if (instance == 0)
    		instance = olyParams.Device->Active_FIR;
    	else
    		instance--;
    
    	if (instance < FIR_PRESETS_STORED)
    	olyParams.FIR[instance][PID] = Value;
    
    	if (instance == olyParams.Device->Active_FIR)
    		olyDspPort.WriteMessage(SetParameter(eTARGET_FIR,0,PID,Value));
    
    	StoreParams();
    }
    
    void Config::ParamSetThrow(uint32_t instance, OLYspeaker1_THROW_pid PID, uint32_t Value)
    {
    	if (instance == 0)
    		instance = olyParams.Device->Active_Throw;
    	else
    		instance--;
    
    	if (instance < THROW_PRESETS_STORED)
    	olyParams.Throw[instance][PID] = Value;
    
    	if (instance == olyParams.Device->Active_Throw)
    		olyDspPort.WriteMessage(SetParameter(eTARGET_THROW,0,PID,Value));
    
    	StoreParams();
    }
    
    void Config::ParamSetProfile(uint32_t instance, OLYspeaker1_PROFILE_pid PID, uint32_t Value)
    {
    	if (instance == 0)
    		instance = olyParams.Device->Active_Profile;
    	else
    		instance--;
    
    	if (instance < PROFILE_PRESETS_STORED)
    	olyParams.Profile[instance][PID] = Value;
    
    	if (instance == olyParams.Device->Active_Profile)
    		olyDspPort.WriteMessage(SetParameter(eTARGET_PROFILE,0,PID,Value));
    
    	StoreParams();
    }
    
    void Config::ParamSetXover(uint32_t instance, OLYspeaker1_XOVER_pid PID, mandolin_parameter_value Value)
    {
    	if (instance == 0)
    		instance = olyParams.Device->Active_Xover;
    	else
    		instance--;
    
    	if (instance < XOVER_PRESETS_STORED)
    	olyParams.Xover[instance][PID] = Value.u;
    
    	if (instance == olyParams.Device->Active_Xover)
    		olyDspPort.WriteMessage(SetParameter(eTARGET_XOVER,0,PID,Value.u));
    
    #if USE_OLY_UI
    	/* Perform any UI-related Updates */
    	switch (PID){
    		case(ePID_OLYspeaker1_XOVER_SPEAKER_XOVER_FREQ):
    			if (instance == eXOVER_TYPE_VARIABLE)
    			olyUI.UpdateXoverFreq(Value.f);
    			break;
    	}
    #endif
    
    	StoreParams();
    }
    
    void Config::ParamSetAirloss(OLYspeaker1_AIRLOSS_pid PID, mandolin_parameter_value Value)
    {
    	olyParams.Airloss[PID] = Value.u;
    
    	olyDspPort.WriteMessage(SetParameter(eTARGET_AIRLOSS,0,PID,Value.u));
    
    	StoreParams();
    }
    
    void Config::ParamSetNoiseGate(OLYspeaker1_NOISEGATE_pid PID, mandolin_parameter_value Value)
    {
    	olyParams.NoiseGate[PID] = Value.u;
    
    	olyDspPort.WriteMessage(SetParameter(eTARGET_NOISEGATE,0,PID,Value.u));
    
    	StoreParams();
    }
    
    
    void Config::SetModel(LOUD_brand brand, uint32_t model_id)
    {
    	//	For debug use only!!
    	Region::SetSystemModel(model_id);
    	Region::SetSystemBrand(brand);
    	g_pszModelName = Region::GetMandolinModelName(Region::GetSystemBrand(), Region::GetSystemModel());
        RestoreDefaults(false);
    }
    
    void Config::ClearOptimization(void)
    {
    	uint32_t i = 0;
    #if USE_OLY_UI
    	/* Invalidate variable Array_Was_Optimized, array status, last index and last size */
    	ParamSetDevice(ePID_OLYspeaker1_DEVICE_ARRAY_WAS_OPTIMIZED, &i);
    	ParamSetDevice(ePID_OLYspeaker1_DEVICE_ARRAY_INDEX_LAST, &i);
    	ParamSetDevice(ePID_OLYspeaker1_DEVICE_ARRAY_SIZE_LAST, &i);
    
    	olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_ARRAY_WAS_OPTIMIZED, olyParams.Device->Array_Was_Optimized), true);
    	olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_ARRAY_INDEX_LAST, olyParams.Device->Array_Index_Last), true);
    	olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_ARRAY_SIZE_LAST, olyParams.Device->Array_Size_Last), true);
    
    	/* Reset compensation EQ */
    	Controls_SetThrow(eCOMPENSATION_EQ_FLAT);
    
    	/* Initialize Airloss Parameters */
    	for (i=1;i<OLYspeaker1_AIRLOSS_PARAMETER_MAX; i++){
    		olyParams.Airloss[i] = gOLYspeaker1AirlossParameterTable[i].defValue;
    	}
    
    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_THROW, olyParams.Device->Active_Throw, 1, olyParams.Throw[olyParams.Device->Active_Throw],OLYspeaker1_THROW_PARAMETER_MAX-1));
    	_time_delay(10);
    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_AIRLOSS, 0, 1, olyParams.Airloss,OLYspeaker1_AIRLOSS_PARAMETER_MAX-1));
    	_time_delay(10);
    
    	olyNetworkPort.WriteMessage(SetParameterBlock(eTARGET_THROW, olyParams.Device->Active_Throw, 1, olyParams.Throw[olyParams.Device->Active_Throw],OLYspeaker1_THROW_PARAMETER_MAX-1));
    	olyNetworkPort.WriteMessage(SetParameterBlock(eTARGET_AIRLOSS, 0, 1, olyParams.Airloss,OLYspeaker1_AIRLOSS_PARAMETER_MAX-1));
    
    	_lwevent_set(&irda_lwevent, irda_event_array_status_update);	//notify task to update
    #endif
    }
    
    void Config::RestoreUserEQ(int instance)
    {
    	mandolin_parameter_value tmp_val;
    	int j;
    
    	if (instance > USER_PRESETS)
    		return;
    
    	if (User_Presets[instance]) {
    		// An external define for preset values exists, use it
    		memcpy (olyStoredParams.Stored_User_Presets[instance], User_Presets[instance],sizeof(olyStoredParams.Stored_User_Presets[instance]));
    	} else {
    		// Otherwise copy defaults from map file
    		for (j=0;j<OLYspeaker1_USER_PARAMETER_MAX;j++){
    			olyStoredParams.Stored_User_Presets[instance][j] = gOLYspeaker1UserParameterTable[j].defValue;
    		}
    
    	/*  Override default EQ center frequencies to match Master Rig UI */
    	#ifdef 	EQ_DEFAULT_FREQ_BAND1
    				tmp_val.f = EQ_DEFAULT_FREQ_BAND1;
    				olyStoredParams.Stored_User_Presets[instance][ePID_OLYspeaker1_USER_SPEAKER_PEQ_BAND1_FREQ] = tmp_val.u;
    	#endif
    	#ifdef 	EQ_DEFAULT_FREQ_BAND2
    				tmp_val.f = EQ_DEFAULT_FREQ_BAND2;
    				olyStoredParams.Stored_User_Presets[instance][ePID_OLYspeaker1_USER_SPEAKER_PEQ_BAND2_FREQ] = tmp_val.u;
    	#endif
    	#ifdef 	EQ_DEFAULT_FREQ_BAND3
    				tmp_val.f = EQ_DEFAULT_FREQ_BAND3;
    				olyStoredParams.Stored_User_Presets[instance][ePID_OLYspeaker1_USER_SPEAKER_PEQ_BAND3_FREQ] = tmp_val.u;
    	#endif
    	#ifdef 	EQ_DEFAULT_FREQ_BAND4
    				tmp_val.f = EQ_DEFAULT_FREQ_BAND4;
    				olyStoredParams.Stored_User_Presets[instance][ePID_OLYspeaker1_USER_SPEAKER_PEQ_BAND4_FREQ] = tmp_val.u;
    	#endif
    	#ifdef 	EQ_DEFAULT_FREQ_BAND5
    				tmp_val.f = EQ_DEFAULT_FREQ_BAND5;
    				olyStoredParams.Stored_User_Presets[instance][ePID_OLYspeaker1_USER_SPEAKER_PEQ_BAND5_FREQ] = tmp_val.u;
    	#endif
    	#ifdef 	EQ_DEFAULT_FREQ_BAND6
    				tmp_val.f = EQ_DEFAULT_FREQ_BAND6;
    				olyStoredParams.Stored_User_Presets[instance][ePID_OLYspeaker1_USER_SPEAKER_PEQ_BAND6_FREQ] = tmp_val.u;
    	#endif
    	#ifdef 	EQ_DEFAULT_FREQ_BAND7
    				tmp_val.f = EQ_DEFAULT_FREQ_BAND7;
    				olyStoredParams.Stored_User_Presets[instance][ePID_OLYspeaker1_USER_SPEAKER_PEQ_BAND7_FREQ] = tmp_val.u;
    	#endif
    
        #ifdef EQ_GROUP_FREQ_BAND1
    				tmp_val.f = EQ_GROUP_FREQ_BAND1;
    				olyStoredParams.Stored_User_Presets[instance][ePID_OLYspeaker1_USER_GROUP_PEQ_BAND1_FREQ] = tmp_val.u;
        #endif
    	#ifdef EQ_GROUP_FREQ_BAND2
    				tmp_val.f = EQ_GROUP_FREQ_BAND2;
    				olyStoredParams.Stored_User_Presets[instance][ePID_OLYspeaker1_USER_GROUP_PEQ_BAND2_FREQ] = tmp_val.u;
    	#endif
    	#ifdef EQ_GROUP_FREQ_BAND3
    				tmp_val.f = EQ_GROUP_FREQ_BAND3;
    				olyStoredParams.Stored_User_Presets[instance][ePID_OLYspeaker1_USER_GROUP_PEQ_BAND3_FREQ] = tmp_val.u;
    	#endif
    	#ifdef EQ_GROUP_FREQ_BAND4
    				tmp_val.f = EQ_GROUP_FREQ_BAND4;
    				olyStoredParams.Stored_User_Presets[instance][ePID_OLYspeaker1_USER_GROUP_PEQ_BAND4_FREQ] = tmp_val.u;
    	#endif
    	#ifdef EQ_GROUP_FREQ_BAND5
    				tmp_val.f = EQ_GROUP_FREQ_BAND5;
    				olyStoredParams.Stored_User_Presets[instance][ePID_OLYspeaker1_USER_GROUP_PEQ_BAND5_FREQ] = tmp_val.u;
    	#endif
    	#ifdef EQ_GROUP_FREQ_BAND6
    				tmp_val.f = EQ_GROUP_FREQ_BAND6;
    				olyStoredParams.Stored_User_Presets[instance][ePID_OLYspeaker1_USER_GROUP_PEQ_BAND6_FREQ] = tmp_val.u;
    	#endif
    	#ifdef EQ_GROUP_FREQ_BAND7
    				tmp_val.f = EQ_GROUP_FREQ_BAND7;
    				olyStoredParams.Stored_User_Presets[instance][ePID_OLYspeaker1_USER_GROUP_PEQ_BAND7_FREQ] = tmp_val.u;
    	#endif
    
    
    	}
    
    	ActiveInputSource = src_none; // Needed to spoof the compare in SetInputSource
    	SetInputSelectMode((LOUD_audio_mode)olyParams.Device->Input_Mode);
    
    	if (olyParams.Device->Active_User == instance)
    	{
    		olyDspPort.WriteMessage(GetSyncMessage(MANDOLIN_SE_SYNC_CURRENT_STATE));
    		olyDspPort.WriteMessage(SetParameterBlock(eTARGET_USER, olyParams.Device->Active_User, 1, olyParams.User[olyParams.Device->Active_User],OLYspeaker1_USER_PARAMETER_MAX-1));
    		_time_delay(10);
    		olyDspPort.WriteMessage(GetSyncMessage(MANDOLIN_SE_SYNC_OK));
    	}
    
    }
    
    void Config::RestoreDefaults(bool startup){
    	int i,j;
    	mandolin_parameter_value tmp_val;
    
    	printf("Restoring parameters to factory defaults.\n");
    	GlobalMute = 0;

// TODO : SC Commented out
//    	amp_init();	// Mutes output
    
    #if OLY_UI_MODE_RSX18 && OLY_UI_MODE_HIDDEN_MENU
    	bool use_revA_woofer = GetAmpRoutingHasChanged();
    #endif
    
    	/* Initialize Device Parameters */
    	for (i=1;i<OLYspeaker1_DEVICE_PARAMETER_MAX; i++){
    		olyParams.Device->Values[i] = gOLYspeaker1DeviceParameterTable[i].defValue;
    	}
    	/* Initialize Status Parameters */
    	for (i=1;i<OLYspeaker1_STATUS_PARAMETER_MAX; i++){
    		olyStatus.Values[i] = gOLYspeaker1StatusParameterTable[i].defValue;
    	}
    
    #if USE_CDD_UI
    #define CDDL_FAN_DEFAULT_TEMP 50.0
    	olyParams.Device->Fan_Threshold = CDDL_FAN_DEFAULT_TEMP;
    	olyParams.Device->Active_User = 1; //force active user to first factory preset
    	olyParams.Device->Active_Profile = 1;
    #endif
    
    // Extra validation for preset counts
    if (olyParams.Device->Active_Profile >= PROFILE_PRESETS)
    	olyParams.Device->Active_Profile = 0;
    
    if (olyParams.Device->Active_User >= USER_PRESETS)
    	olyParams.Device->Active_User = 0;
    
    if (olyParams.Device->Active_Throw >= THROW_PRESETS)
    	olyParams.Device->Active_Throw = 0;
    
    if (olyParams.Device->Active_Voicing >= VOICING_PRESETS)
    	olyParams.Device->Active_Voicing = 0;
    
    if (olyParams.Device->Active_Xover >= XOVER_PRESETS)
    	olyParams.Device->Active_Xover = 0;
    
    #if OLY_UI_MODE_RSX12 || OLY_UI_MODE_RSX18
    	olyParams.Device->Active_Xover = eXOVER_TYPE_RSX;
    #endif
    
    	strcpy(olyStoredParams.Stored_Labels[eLID_OLYspeaker1_DEVICE_DEVICE_NAME], c_DefaultDeviceName);
    	strcpy(olyStoredParams.Stored_Labels[eLID_OLYspeaker1_DEVICE_GROUP_NAME], c_DefaultGroupName);
    	strcpy(olyStoredParams.Stored_Labels[eLID_OLYspeaker1_DEVICE_PROFILE1_NAME], c_DefaultCustom1Name);
    	strcpy(olyStoredParams.Stored_Labels[eLID_OLYspeaker1_DEVICE_PROFILE2_NAME], c_DefaultCustom1Name);
    	strcpy(olyStoredParams.Stored_Labels[eLID_OLYspeaker1_DEVICE_DANTE_DISCO_NAME], c_DefaultDanteDiscoName);
    
    
    	SetInputSelectMode((LOUD_audio_mode)olyParams.Device->Input_Mode);
    	SetSelfTestMode(eTEST_MODE_DISABLED);
    	OnDanteMuteChanged();
    	RefreshLogoState();
    
    #if USER_PRESETS_STORED
    	/* Reset Editable User parameters */
    	for (i=0; i< USER_PRESETS_STORED; i++) {
    		RestoreUserEQ(i);
    	}
    
    #if USE_CDD_UI
    		//until it is written to, user 1 should not be loaded
    		//Store 0xFFFFFFFF in the null first word to mark as unuseable
    		olyStoredParams.Stored_User_Presets[eCDD_PRESETS_USER][0] = 0xFFFFFFFF;
    #endif
    
    #endif
    
    #if PROFILE_PRESETS_STORED
    	/* Reset Editable Profile parameters */
    	for (i=0; i< PROFILE_PRESETS_STORED; i++){
    		if (Profile_Presets[i]) {
    			// An external define for preset values exists, use it
    			memcpy (olyStoredParams.Stored_Profile_Presets[i], Profile_Presets[i],sizeof(olyStoredParams.Stored_Profile_Presets[i]));
    		} else {
    			// Otherwise copy defaults from map file
    			for (j=0;j<OLYspeaker1_PROFILE_PARAMETER_MAX;j++){
    				olyStoredParams.Stored_Profile_Presets[i][j] = gOLYspeaker1ProfileParameterTable[j].defValue;
    			}
    		}
    	}
    #endif
    
    // Override with Default "User" Profile EQ bands
    #ifdef 	EQ_DEFAULT_FREQ_BAND1
    			tmp_val.f = EQ_DEFAULT_FREQ_BAND1;
    			olyStoredParams.Stored_Profile_Presets[eOLY_PROFILES_CUSTOM1][ePID_OLYspeaker1_PROFILE_PROFILE_PEQ_BAND1_FREQ] = tmp_val.u;
    #endif
    #ifdef 	EQ_DEFAULT_FREQ_BAND2
    			tmp_val.f = EQ_DEFAULT_FREQ_BAND2;
    			olyStoredParams.Stored_Profile_Presets[eOLY_PROFILES_CUSTOM1][ePID_OLYspeaker1_PROFILE_PROFILE_PEQ_BAND2_FREQ] = tmp_val.u;
    #endif
    #ifdef 	EQ_DEFAULT_FREQ_BAND3
    			tmp_val.f = EQ_DEFAULT_FREQ_BAND3;
    			olyStoredParams.Stored_Profile_Presets[eOLY_PROFILES_CUSTOM1][ePID_OLYspeaker1_PROFILE_PROFILE_PEQ_BAND3_FREQ] = tmp_val.u;
    #endif
    #ifdef 	EQ_DEFAULT_FREQ_BAND4
    			tmp_val.f = EQ_DEFAULT_FREQ_BAND4;
    			olyStoredParams.Stored_Profile_Presets[eOLY_PROFILES_CUSTOM1][ePID_OLYspeaker1_PROFILE_PROFILE_PEQ_BAND4_FREQ] = tmp_val.u;
    #endif
    #ifdef 	EQ_DEFAULT_FREQ_BAND5
    			tmp_val.f = EQ_DEFAULT_FREQ_BAND5;
    			olyStoredParams.Stored_Profile_Presets[eOLY_PROFILES_CUSTOM1][ePID_OLYspeaker1_PROFILE_PROFILE_PEQ_BAND5_FREQ] = tmp_val.u;
    #endif
    #ifdef 	EQ_DEFAULT_FREQ_BAND6
    			tmp_val.f = EQ_DEFAULT_FREQ_BAND6;
    			olyStoredParams.Stored_Profile_Presets[eOLY_PROFILES_CUSTOM1][ePID_OLYspeaker1_PROFILE_PROFILE_PEQ_BAND6_FREQ] = tmp_val.u;
    #endif
    #ifdef 	EQ_DEFAULT_FREQ_BAND7
    			tmp_val.f = EQ_DEFAULT_FREQ_BAND7;
    			olyStoredParams.Stored_Profile_Presets[eOLY_PROFILES_CUSTOM1][ePID_OLYspeaker1_PROFILE_PROFILE_PEQ_BAND7_FREQ] = tmp_val.u;
    #endif
    
    
    #if THROW_PRESETS_STORED
    	/* Reset Editable Throw parameters */
    	for (i=0; i< THROW_PRESETS_STORED; i++){
    		if (Throw_Presets[i]) {
    			// An external define for preset values exists, use it
    			memcpy (olyStoredParams.Stored_Throw_Presets[i], Throw_Presets[i],sizeof(olyStoredParams.Stored_Throw_Presets[i]));
    		} else {
    			// Otherwise copy defaults from map file
    			for (j=0;j<OLYspeaker1_THROW_PARAMETER_MAX;j++){
    				olyStoredParams.Stored_Throw_Presets[i][j] = gOLYspeaker1ThrowParameterTable[j].defValue;
    			}
    		}
    	}
    #endif
    
    #if VOICING_PRESETS_STORED
    	/* Reset Editable Voicing parameters */
    	for (i=0; i< VOICING_PRESETS_STORED; i++){
    		if (Voicing_Presets[i]) {
    			// An external define for preset values exists, use it
    			memcpy(olyStoredParams.Stored_Voicing_Presets[i], Voicing_Presets[i],sizeof(olyStoredParams.Stored_Voicing_Presets[i]));
    		} else {
    			// Otherwise copy defaults from map file
    			for (j=0;j<OLYspeaker1_VOICING_PARAMETER_MAX;j++){
    				olyStoredParams.Stored_Voicing_Presets[i][j] = gOLYspeaker1VoicingParameterTable[j].defValue;
    			}
    		}
    	}
    #endif
    
    #if FIR_PRESETS_STORED
    	/* Reset Editable FIR parameters */
    	for (i=0; i< FIR_PRESETS_STORED; i++){
    		if (FIR_Presets[i]) {
    			// An external define for preset values exists, use it
    			memcpy (olyStoredParams.Stored_FIR_Presets[i], FIR_Presets[i],sizeof(olyStoredParams.Stored_FIR_Presets[i]));
    		} else {
    			 // Would load FIR defaults from _map.c but not enough memory,
    			 // clear out the FIR coefs
    			for (j=0;j<OLYspeaker1_FIR_PARAMETER_MAX;j++){
    				olyStoredParams.Stored_FIR_Presets[i][j] = 0;
    			}
    		}
    	}
    #endif
    
    #if XOVER_PRESETS_STORED
    	/* Reset Editable Xover parameters */
    	for (i=0; i< XOVER_PRESETS_STORED; i++){
    		if (Xover_Presets[i]) {
    			// An external define for preset values exists, use it
    			memcpy(olyStoredParams.Stored_Xover_Presets[i], Xover_Presets[i],sizeof(olyStoredParams.Stored_Xover_Presets[i]));
    		} else {
    			// Otherwise copy defaults from map file
    			for (j=0;j<OLYspeaker1_XOVER_PARAMETER_MAX;j++){
    				olyStoredParams.Stored_Xover_Presets[i][j] = gOLYspeaker1XoverParameterTable[j].defValue;
    
    			}
    		}
    	}
    #endif
    
    #ifdef XOVER_DEFAULT_FREQ
    	// Override default freq with model-specific value
    	tmp_val.f = XOVER_DEFAULT_FREQ;
    	olyStoredParams.Stored_Xover_Presets[eXOVER_TYPE_VARIABLE][ePID_OLYspeaker1_XOVER_SPEAKER_XOVER_FREQ] = tmp_val.u;
    #endif
    
    	/* Initialize Airloss Parameters */
    	for (i=1;i<OLYspeaker1_AIRLOSS_PARAMETER_MAX; i++){
    		olyParams.Airloss[i] = gOLYspeaker1AirlossParameterTable[i].defValue;
    	}
    
    
    	/* Initialize NoiseGate Parameters */
    
    #if USE_CDD_NOISEGATE
    	memcpy(olyStoredParams.Stored_NoiseGate, NOISEGATE_Preset_CDDL,sizeof(olyStoredParams.Stored_NoiseGate));
    #else
    	for (i=1;i<OLYspeaker1_NOISEGATE_PARAMETER_MAX; i++){
    		olyParams.NoiseGate[i] = gOLYspeaker1NoisegateParameterTable[i].defValue;
    	}
    #endif
    
    	/* Send all parameters to DSP */
    
    	// send a sync current state message to DSP
    	olyDspPort.WriteMessage(GetSyncMessage(MANDOLIN_SE_SYNC_CURRENT_STATE));
    
    #if USE_CDD_UI
    	SetActivePreset(eTARGET_USER,  olyParams.Device->Active_User);
    #else
    	//these blocks are loaded as presets on CDDL
    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_USER, olyParams.Device->Active_User, 1, olyParams.User[olyParams.Device->Active_User],OLYspeaker1_USER_PARAMETER_MAX-1));
    	_time_delay(10);
        olyDspPort.WriteMessage(SetParameterBlock(eTARGET_PROFILE, olyParams.Device->Active_Profile, 1, olyParams.Profile[olyParams.Device->Active_Profile],OLYspeaker1_PROFILE_PARAMETER_MAX-1));
    #endif
        _time_delay(10);
    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_VOICING, olyParams.Device->Active_Voicing, ePID_OLYspeaker1_VOICING_GLOBAL_PEQ_ENABLE, olyParams.Voicing[olyParams.Device->Active_Voicing],128));
        _time_delay(10);
    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_VOICING, olyParams.Device->Active_Voicing, ePID_OLYspeaker1_VOICING_HF_PEQ_POST_BAND2_Q, olyParams.Voicing[olyParams.Device->Active_Voicing],128));
        _time_delay(10);
    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_VOICING, olyParams.Device->Active_Voicing, ePID_OLYspeaker1_VOICING_MF_PEQ_POST_BAND4_GAIN, olyParams.Voicing[olyParams.Device->Active_Voicing],128));
        _time_delay(10);
    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_VOICING, olyParams.Device->Active_Voicing, ePID_OLYspeaker1_VOICING_LF_DYNEQ1_DETECTFREQ, olyParams.Voicing[olyParams.Device->Active_Voicing],OLYspeaker1_VOICING_PARAMETER_MAX-ePID_OLYspeaker1_VOICING_LF_DYNEQ1_DETECTFREQ));

        _time_delay(10);
    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_THROW, olyParams.Device->Active_Throw, 1, olyParams.Throw[olyParams.Device->Active_Throw],OLYspeaker1_THROW_PARAMETER_MAX-1));
    	_time_delay(10);
    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_XOVER, olyParams.Device->Active_Xover, 1, olyParams.Xover[olyParams.Device->Active_Xover],OLYspeaker1_XOVER_PARAMETER_MAX-1));
    	_time_delay(10);
    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_AIRLOSS, 0, 1, olyParams.Airloss,OLYspeaker1_AIRLOSS_PARAMETER_MAX-1));
    	_time_delay(10);
    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_NOISEGATE, 0, 1, olyParams.NoiseGate,OLYspeaker1_NOISEGATE_PARAMETER_MAX-1));
    	_time_delay(10);
    
    #if FIR_PRESETS
    if (FIR_Presets[olyParams.Device->Active_FIR]){
    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_HF_FIR_ENABLE, olyParams.FIR[olyParams.Device->Active_FIR],128));
    	_time_delay(10);
    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_HF_FIR_COEF127_VALUE, olyParams.FIR[olyParams.Device->Active_FIR],128));
    	_time_delay(10);
    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_HF_FIR_COEF255_VALUE, olyParams.FIR[olyParams.Device->Active_FIR],128));
    	_time_delay(10);
    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_HF_FIR_COEF383_VALUE, olyParams.FIR[olyParams.Device->Active_FIR],128));
    	_time_delay(10);
    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_HF_FIR_COEF511_VALUE, olyParams.FIR[olyParams.Device->Active_FIR],ePID_OLYspeaker1_FIR_MF_FIR_ENABLE-ePID_OLYspeaker1_FIR_HF_FIR_COEF511_VALUE));
    	_time_delay(10);

    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_MF_FIR_ENABLE, olyParams.FIR[olyParams.Device->Active_FIR],128));
    	_time_delay(10);
    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_MF_FIR_COEF127_VALUE, olyParams.FIR[olyParams.Device->Active_FIR],128));
    	_time_delay(10);
    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_MF_FIR_COEF255_VALUE, olyParams.FIR[olyParams.Device->Active_FIR],128));
    	_time_delay(10);
    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_MF_FIR_COEF383_VALUE, olyParams.FIR[olyParams.Device->Active_FIR],128));
    	_time_delay(10);
    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_MF_FIR_COEF511_VALUE, olyParams.FIR[olyParams.Device->Active_FIR],ePID_OLYspeaker1_FIR_LF_FIR_ENABLE-ePID_OLYspeaker1_FIR_MF_FIR_COEF511_VALUE));
    	_time_delay(10);

    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_LF_FIR_ENABLE, olyParams.FIR[olyParams.Device->Active_FIR],128));
    	_time_delay(10);
    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_LF_FIR_COEF127_VALUE, olyParams.FIR[olyParams.Device->Active_FIR],128));
    	_time_delay(10);
    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_LF_FIR_COEF255_VALUE, olyParams.FIR[olyParams.Device->Active_FIR],128));
    	_time_delay(10);
    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_LF_FIR_COEF383_VALUE, olyParams.FIR[olyParams.Device->Active_FIR],128));
    	_time_delay(10);
    	olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_LF_FIR_COEF511_VALUE, olyParams.FIR[olyParams.Device->Active_FIR],ePID_OLYspeaker1_FIR_FENCE-ePID_OLYspeaker1_FIR_LF_FIR_COEF511_VALUE));
    	_time_delay(10);


    } else {
    	// No FIR preset file, disable blocks in DSP
    	mandolin_parameter_value tmp;
    	tmp.f = 1.0;
    	ParamSetFIR(olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_HF_FIR_ENABLE, 0);
    	ParamSetFIR(olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_HF_FIR_MUTE, 0);
    	ParamSetFIR(olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_HF_FIR_COEF1_VALUE, tmp.u);
    	ParamSetFIR(olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_MF_FIR_ENABLE, 0);
    	ParamSetFIR(olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_MF_FIR_MUTE, 0);
    	ParamSetFIR(olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_MF_FIR_COEF1_VALUE, tmp.u);
    	ParamSetFIR(olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_LF_FIR_ENABLE, 0);
    	ParamSetFIR(olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_LF_FIR_MUTE, 0);
    	ParamSetFIR(olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_LF_FIR_COEF1_VALUE, tmp.u);
    }
    #else
    	// No FIR presets used, disable blocks in DSP
    	mandolin_parameter_value tmp;
    	tmp.f = 1.0;
    	ParamSetFIR(olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_HF_FIR_ENABLE, 0);
    	ParamSetFIR(olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_HF_FIR_MUTE, 0);
    	ParamSetFIR(olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_HF_FIR_COEF1_VALUE, tmp.u);
    	ParamSetFIR(olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_MF_FIR_ENABLE, 0);
    	ParamSetFIR(olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_MF_FIR_MUTE, 0);
    	ParamSetFIR(olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_MF_FIR_COEF1_VALUE, tmp.u);
    	ParamSetFIR(olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_LF_FIR_ENABLE, 0);
    	ParamSetFIR(olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_LF_FIR_MUTE, 0);
    	ParamSetFIR(olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_LF_FIR_COEF1_VALUE, tmp.u);
    #endif
    
    	// send a sync ok message to indicate the end of sync current state
    	olyDspPort.WriteMessage(GetSyncMessage(MANDOLIN_SE_SYNC_OK));
    
    	// may want to wait a bit here while all the messages process on the DSP before unmuting.....
    	_time_delay(50);
    
    	//Sort the default routing *AMP1 - 3. AMP2 - 1 for full range speakers. AMP1 - 3. AMP2 - 3 for subs. Done in the VOICING_data_factory_preset1.h should never change!
    	//SetAmpRouting(3, 1, NULL, NULL);


    #if OLY_UI_MODE_RSX18 && OLY_UI_MODE_HIDDEN_MENU
    	if (use_revA_woofer)
    		SetAmpRouting(AMP1_ROUTING_REV_A, AMP2_ROUTING_REV_A, AMP3_ROUTING_REV_A, AMP4_ROUTING_REV_A);
    #endif
    
    
    	SetSelfTestMode(eTEST_MODE_DISABLED);
    	OnDanteMuteChanged();

// TODO : SC Commented out
#ifdef SC_COMENTED_OUT
    	_lwevent_set(&irda_lwevent, irda_event_array_status_update);	//notify IR task of changes

    	init_codec(CODEC_CH1_INVERT, CODEC_CH2_INVERT, CODEC_CH3_INVERT, CODEC_CH4_INVERT);
    
    	if (!startup)
    		amp_mute(false);
#endif // SC_COMENTED_OUT
    
    	olyStoredParams.Version.u32 = OLY_FW_VERSION;
    	olyStoredParams.XML_Version = OLYspeaker1_XML_VERSION;
    
    	WriteParamsToFlash();
    	InitStatusParams();
    	InitUI();
    }
    
    void Config::RestoreIPAddresses()
    {
// TODO : SC Commented out
//    	if(g_pUltimoPort) // Send Dante IP Reset message (Ultimo switch to DHCP and reboot)
//    		g_pUltimoPort->TriggerIpReset();
    
    	SwitchToDHCP(); // Switch Network IP to DHCP
    	if(m_pUpgrade->SetLaunchType(OLY_REGION_APPLICATION)) // Make sure that the launch type is application
    		DeferredReboot(1000); // Reboot to allow the switch to take affect. Wait for 1000 ms to give the user enough time to stop pressing the enc
    }
    
    void Config::LoadAllFromFlash()
    {
    	int i;
    	printf("OLY Params size = %d\n", sizeof(oly_flash_params_t));

// TODO : SC Commented out
#if SC_COMENTED_OUT
    	system_flash_init(&olyStoredParams);
    	//memcpy(&olyStoredParams, p_CurrentParamMemLoc, sizeof(oly_flash_params_t));
    	printf("Load Params From Slot#%i\n", emptyMemorySlotIndex+1);
    
    	oly_version_t version;
    	version.u32 = OLY_FW_VERSION;
    
    	// If XML version or Major/Minor rev, clear flash to defaults.  Sub rev ignored
    	if  ((olyStoredParams.XML_Version != OLYspeaker1_XML_VERSION) || ((olyStoredParams.Version.u32 & 0xFF00) != (version.u32 & 0xFF00) )){
    
    		printf("Parameter Version incompatible: XML stored= %i new= %i\n", olyStoredParams.XML_Version, OLYspeaker1_XML_VERSION);
    		printf("Version stored = %d.%d.%d, new = %d.%d.%d\n", olyStoredParams.Version.major,olyStoredParams.Version.minor, olyStoredParams.Version.sub, version.major, version.minor, version.sub);
    		/* Load default device parameters */
    		system_flash_param_reinit();
    		RestoreDefaults(true);
    
    	}else{
    		printf("XML version=%i\n", olyStoredParams.XML_Version);
    		/* Initialize Status Parameters */
    		for (int i=1;i<OLYspeaker1_STATUS_PARAMETER_MAX; i++){
    			olyStatus.Values[i] = gOLYspeaker1StatusParameterTable[i].defValue;
    		}
    
    		olyDspPort.WriteMessage(GetSyncMessage(MANDOLIN_SE_SYNC_CURRENT_STATE));
    
    #if	0	//	Use if message size is not big enough to hold largest command ~450 parameters, other places in the code need this too.
    		olyDspPort.SyncTarget(eTARGET_USER, olyParams.Device->Active_User, 1, olyParams.User[olyParams.Device->Active_User], OLYspeaker1_USER_PARAMETER_MAX-1);
    		olyDspPort.SyncTarget(eTARGET_USER, olyParams.Device->Active_User, 1, olyParams.User[olyParams.Device->Active_User],OLYspeaker1_USER_PARAMETER_MAX-1);
    		olyDspPort.SyncTarget(eTARGET_PROFILE, olyParams.Device->Active_Profile, 1, olyParams.Profile[olyParams.Device->Active_Profile],OLYspeaker1_PROFILE_PARAMETER_MAX-1);
    	    olyDspPort.SyncTarget(eTARGET_VOICING, olyParams.Device->Active_Voicing, 1, olyParams.Voicing[olyParams.Device->Active_Voicing],OLYspeaker1_VOICING_PARAMETER_MAX-1);
    		olyDspPort.SyncTarget(eTARGET_THROW, olyParams.Device->Active_Throw, 1, olyParams.Throw[olyParams.Device->Active_Throw],OLYspeaker1_THROW_PARAMETER_MAX-1);
    		olyDspPort.SyncTarget(eTARGET_AIRLOSS, 0, 1, olyParams.Airloss,OLYspeaker1_AIRLOSS_PARAMETER_MAX-1);
    		olyDspPort.SyncTarget(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_HF_FIR_ENABLE, olyParams.FIR[olyParams.Device->Active_FIR],302);
    		olyDspPort.SyncTarget(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_HF_FIR_COEF300_VALUE, olyParams.FIR[olyParams.Device->Active_FIR],300);
    		olyDspPort.SyncTarget(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_MF_FIR_ENABLE, olyParams.FIR[olyParams.Device->Active_FIR],302);
    		olyDspPort.SyncTarget(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_MF_FIR_COEF300_VALUE, olyParams.FIR[olyParams.Device->Active_FIR],300);
    		olyDspPort.SyncTarget(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_LF_FIR_ENABLE, olyParams.FIR[olyParams.Device->Active_FIR],302);
    		olyDspPort.SyncTarget(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_LF_FIR_COEF300_VALUE, olyParams.FIR[olyParams.Device->Active_FIR],300);
    #else
    
    #if USE_CDD_UI
    		SetActivePreset(eTARGET_USER,  olyParams.Device->Active_User);		// note that this causes the MANDOLIN_SE_SYNC_OK message to be sent!
    #else
    		olyDspPort.WriteMessage(SetParameterBlock(eTARGET_USER, olyParams.Device->Active_User, 1, olyParams.User[olyParams.Device->Active_User],OLYspeaker1_USER_PARAMETER_MAX-1));
    		_time_delay(10);
    	    olyDspPort.WriteMessage(SetParameterBlock(eTARGET_PROFILE, olyParams.Device->Active_Profile, 1, olyParams.Profile[olyParams.Device->Active_Profile],OLYspeaker1_PROFILE_PARAMETER_MAX-1));
    #endif
    	    _time_delay(10);
    		olyDspPort.WriteMessage(SetParameterBlock(eTARGET_VOICING, olyParams.Device->Active_Voicing, 1, olyParams.Voicing[olyParams.Device->Active_Voicing],OLYspeaker1_VOICING_PARAMETER_MAX-1));
    		_time_delay(10);
    		olyDspPort.WriteMessage(SetParameterBlock(eTARGET_THROW, olyParams.Device->Active_Throw, 1, olyParams.Throw[olyParams.Device->Active_Throw],OLYspeaker1_THROW_PARAMETER_MAX-1));
    		_time_delay(10);
    		olyDspPort.WriteMessage(SetParameterBlock(eTARGET_XOVER, olyParams.Device->Active_Xover, 1, olyParams.Xover[olyParams.Device->Active_Xover],OLYspeaker1_XOVER_PARAMETER_MAX-1));
    		_time_delay(10);
    		olyDspPort.WriteMessage(SetParameterBlock(eTARGET_AIRLOSS, 0, 1, olyParams.Airloss,OLYspeaker1_AIRLOSS_PARAMETER_MAX-1));
    		_time_delay(10);
    		olyDspPort.WriteMessage(SetParameterBlock(eTARGET_NOISEGATE, 0, 1, olyParams.NoiseGate,OLYspeaker1_NOISEGATE_PARAMETER_MAX-1));
    		_time_delay(10);
    
    		olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_HF_FIR_ENABLE, olyParams.FIR[olyParams.Device->Active_FIR],302));
    		_time_delay(10);
    		olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_HF_FIR_COEF300_VALUE, olyParams.FIR[olyParams.Device->Active_FIR],300));
    		_time_delay(10);
    		olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_MF_FIR_ENABLE, olyParams.FIR[olyParams.Device->Active_FIR],302));
    		_time_delay(10);
    		olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_MF_FIR_COEF300_VALUE, olyParams.FIR[olyParams.Device->Active_FIR],300));
    		_time_delay(10);
    		olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_LF_FIR_ENABLE, olyParams.FIR[olyParams.Device->Active_FIR],302));
    		_time_delay(10);
    		olyDspPort.WriteMessage(SetParameterBlock(eTARGET_FIR, olyParams.Device->Active_FIR, ePID_OLYspeaker1_FIR_LF_FIR_COEF300_VALUE, olyParams.FIR[olyParams.Device->Active_FIR],300));
    		_time_delay(10);
    #endif
    
    		// send a sync ok message to indicate the end of sync current state
     		olyDspPort.WriteMessage(GetSyncMessage(MANDOLIN_SE_SYNC_OK));
    		// may want to wait a bit here while all the messages process on the DSP before unmuting.....
    		_time_delay(50);
    
    		SetInputSelectMode((LOUD_audio_mode)olyParams.Device->Input_Mode);
    		SetSelfTestMode(eTEST_MODE_DISABLED);
    
    		RefreshLogoState();
    
    		init_codec(CODEC_CH1_INVERT, CODEC_CH2_INVERT, CODEC_CH3_INVERT, CODEC_CH4_INVERT);
    //		amp_mute(false);
    	}
    #if USE_CDD_UI
    	//FORCE LED on for CDD at startup - on time depends on sysEvents task startup for now
    	LogoSetState(LOGO_ON_RED);
    	olyParams.Device->Logo_Mode = eLOGO_MODE_OFF;
    #endif
#endif // SC_COMENTED_OUT
    }
    
    void Config::OnDanteMuteChanged()
    {
// TODO : SC Commented out
#ifdef SC_COMENTED_OUT
    	uint16_t tempInt;
    
    	DanteMute = get_dante_mute_state();
    
    	if (!GlobalMute)
    	{
    		if (ActiveInputSource == src_dante)
    		{
    			printf("Sending Dante Mute state to DSP: %d\n", DanteMute);
    			if (DanteMute)
    				olyDspPort.WriteMessage(SetParameter(eTARGET_USER,0,ePID_OLYspeaker1_USER_SPEAKER_MUTE,1));
    			else
    				olyDspPort.WriteMessage(SetParameter(eTARGET_USER,0,ePID_OLYspeaker1_USER_SPEAKER_MUTE,olyParams.User[olyParams.Device->Active_User][ePID_OLYspeaker1_USER_SPEAKER_MUTE]));
    
    #if USE_BACKPANEL_DEBUG && USE_DEBUG_ULTIMO
    			{
    				char tempString[32] = {0};
    				if(DanteMute)
    					sprintf(tempString, "DSP Muted");
    				else
    					sprintf(tempString, "DSP Unmuted");
    				olyUI.DebugLineOut(1, tempString);
    			}
    #endif
    
    		}
    	}
    
    	if (g_pUltimoPort) {
    		g_pUltimoPort->GetLinkFlags(&tempInt);
    		OnDanteChange_LinkFlags(tempInt);		// Update Network Indicator
    	}
#endif // SC_COMENTED_OUT
    }
    
    void Config::SetGlobalMute(bool mute)
    {
    	if (mute !=GlobalMute)
    	{
    		GlobalMute = mute;
    		printf("Global Mute set to %d\n", GlobalMute);
    
    		if (GlobalMute)
    			olyDspPort.WriteMessage(SetParameter(eTARGET_USER,0,ePID_OLYspeaker1_USER_SPEAKER_MUTE,1));
    		else
    			olyDspPort.WriteMessage(SetParameter(eTARGET_USER,0,ePID_OLYspeaker1_USER_SPEAKER_MUTE,olyParams.User[olyParams.Device->Active_User][ePID_OLYspeaker1_USER_SPEAKER_MUTE]));
    
    #if USE_BACKPANEL_DEBUG && USE_DEBUG_ULTIMO
    				{
    					char tempString[32] = {0};
    					if(GlobalMute)
    						sprintf(tempString, "DSP Muted");
    					else
    						sprintf(tempString, "DSP Unmuted");
    					olyUI.DebugLineOut(1, tempString);
    				}
    #endif
    
    	}
    }
    
    void Config::StoreParams()
    {
    	// Starts the countdown before writing to flash
    	_lwevent_set( &timer_event, event_timer_flashwrite_start );
    }
    
    void Config::WriteParamsToFlash()
    {
#ifdef SC_COMENTED_OUT
    	// Actually writes to flash
    	system_flash_write_oly_params(&olyStoredParams);
#endif // SC_COMENTED_OUT
    }
    
    float Config::GetCurrentAmpTemp(amp_instance_t amp)
    {
#ifdef SC_COMENTED_OUT
    	int32_t rounded = ROUND(ReadAmpTemperature(amp));
    	return (float)rounded;
#endif //  SC_COMENTED_OUT
    }
    
    void Config::UpdateCurrentTiltAngle(void)
    {
#ifdef SC_COMENTED_OUT
    	int32 tempVar = (int32) GetRawTilt();
    
    	if (olyStatus.Angle != tempVar)
    	{
    		olyStatus.Angle = (int32) tempVar;
    		CheckOrientation();
    #if USES_IRDA	// TODO: Enable for FRs once angle and axis logic is sorted
    		olyNetworkPort.WriteMessage(SetParameter(eTARGET_STATUS, 0, ePID_OLYspeaker1_STATUS_ANGLE, olyStatus.Values[ePID_OLYspeaker1_STATUS_ANGLE]), true);
    #endif
    #if USE_OLY_UI
    		olyUI.UpdateAutoTilt(CalcCorrectedTilt(tempVar));
    #endif	// USE_OLY_UI
    	}
#endif //  SC_COMENTED_OUT
    }
    
    #endif // _SECONDARY_BOOT
        
    void Config::SetLcdBrightness(int level)
    {
    		if( level < 0 )	level = 0;
    		if( level > 100) level = 100;
    #ifndef _SECONDARY_BOOT
    		olyParams.Device->LCD_Brightness = level;
    #endif	//	_SECONDARY_BOOT
    		RampLcdBrightness(false, (uint32_t) level);
    #ifndef _SECONDARY_BOOT
    		StoreParams();
    #endif	//	_SECONDARY_BOOT
    }
    
    void Config::RampLcdBrightness(bool set_ramp, uint32_t level)
    {
#ifdef SC_COMENTED_OUT
    	static uint32_t target;
    	static uint32_t count = 0;
    
    	if(set_ramp) {
    		if( count++ == 1200 ) {
    			count = 0;
    			if(GetAppliedBrightnessLevel() > target)
    				UpdateBrightnessLevel(GetAppliedBrightnessLevel() - 1);
    			else if(GetAppliedBrightnessLevel() < target)
    				UpdateBrightnessLevel(GetAppliedBrightnessLevel() + 1);
    			else
    				_lwevent_clear(&user_event, event_brightness_change);
    		}
    	}
    	else {
    		target = level;
    		if( target != GetAppliedBrightnessLevel() )
    			_lwevent_set(&user_event, event_brightness_change);
    	}
#endif // SC_COMENTED_OUT
    }
    
    uint32_t Config::GetStoredBrightness( void )
    {
    	return olyParams.Device->LCD_Brightness;
    }
    
    uint32_t Config::GetActiveBrightness( void )
    {
    	// SC_COMMENTED_OUT
    	return 0;
    	//return GetAppliedBrightnessLevel();
    }
    
    void Config::DimLcdBrightness(bool isDim)
    {
    	if(isDim && (olyParams.Device->Display_Mode == eDISP_MODE_OFF))
    		RampLcdBrightness(false, 0);
    	else if(isDim && (olyParams.Device->Display_Mode == eDISP_MODE_DIM))
    		RampLcdBrightness(false, LCD_DIM_VALUE);
    	else
    		RampLcdBrightness(false, olyParams.Device->LCD_Brightness);
    }
    
    bool Config::GetIsPhysicallyUpsideDown()
    {
    	return ((olyStatus.Angle > FLIP_SCREEN_MIN) && (olyStatus.Angle < FLIP_SCREEN_MAX)) ? true:false;
    }
    
    int32 Config::GetTiltAngle()
    {
    	return (olyStatus.Angle);
    }
    
    void Config::CheckOrientation()
    {
    #if USES_IRDA	// Only flip screen for array speakers
    	bool orientation = GetIsPhysicallyUpsideDown();
    	if (orientation != IsPhysicallyUpsideDown)
    	{
    		IsPhysicallyUpsideDown= orientation;
    		UpdateOrientation(false, orientation);
    	}
    #endif
    }
    
    void Config::CheckInitialOrientation()
    {
    #if USES_IRDA	// Only flip screen for array speakers
    	bool orientation = GetIsPhysicallyUpsideDown();
    	if (orientation != IsPhysicallyUpsideDown)
    	{
    		IsPhysicallyUpsideDown= orientation;
    		UpdateOrientation(true, orientation);
    	}
    #endif
    }
    
    void Config::UpdateOrientation(bool initial, bool flipped)
    {
    #if USES_IRDA	// Only flip screen for array speakers
    #if USE_OLY_UI
    	if(initial)
    		olyUI.SetScreenOrientation(flipped);
    #endif
    
    	IRDA_Mgr_InvertedPorts(flipped);
    #endif
    }
        
    void Config::InitUI()
    {
    	oly_version_t FwVer;
    	FwVer.u32 = OLY_FW_VERSION;
    
    	uint32_t uiStaticIp = m_pUpgrade->GetStaticIp();
    
    #if USE_OLY_UI
    	OnArrayConfigurationChanged();
    
    	CheckInitialOrientation();
    
    	if (strcmp(olyStoredParams.Stored_Labels[eLID_OLYspeaker1_DEVICE_DEVICE_NAME], c_DefaultDeviceName))
    		olyUI.UpdateTitleBarLabel(olyStoredParams.Stored_Labels[eLID_OLYspeaker1_DEVICE_DEVICE_NAME]);
    	else
    		olyUI.UpdateTitleBarLabel((char*)Region::GetMandolinModelName(Region::GetSystemBrand(), Region::GetSystemModel()));
    
    	olyUI.UpdateFwVersion(Region::GetSystemBrand(), Region::GetSystemModel(), FwVer, Region::GetHardwareRevision());
    	olyUI.UpdateMacAddress(Region::GetSystemMAC(), false);
    	olyUI.UpdateIPaddress(g_CurrentIpAddress, 0==uiStaticIp);
    #ifdef _SECONDARY_BOOT
    	olyUI.UpdateVolume(0.f);
    	olyUI.UpdateInputSource(src_none);
    #else	//	_SECONDARY_BOOT
    	olyUI.UpdateVolume(ParamGetValue(olyParams.Device->Active_User, ePID_OLYspeaker1_USER_SPEAKER_FADER));
    	olyUI.UpdateInputSource(GetAudioSource());
    	olyUI.UpdateProfileName(eOLY_PROFILES_PRESET1, (char*)c_Preset1Name);
    	olyUI.UpdateProfileName(eOLY_PROFILES_PRESET2, (char*)c_Preset2Name);
    	olyUI.UpdateProfileName(eOLY_PROFILES_PRESET3, (char*)c_Preset3Name);
    	olyUI.UpdateProfileName(eOLY_PROFILES_PRESET4, (char*)c_Preset4Name);
    	olyUI.UpdateProfileName(eOLY_PROFILES_CUSTOM1, olyStoredParams.Stored_Labels[eLID_OLYspeaker1_DEVICE_PROFILE1_NAME]);
    	olyUI.UpdateActiveProfile((LOUD_oly_profiles)olyParams.Device->Active_Profile);
    	olyUI.UpdateDelay(ParamGetValue(olyParams.Device->Active_User, ePID_OLYspeaker1_USER_SPEAKER_DELAY_TIME));
    	olyUI.UpdateXoverFreq(ParamXoverGetValue(eXOVER_TYPE_VARIABLE, ePID_OLYspeaker1_XOVER_SPEAKER_XOVER_FREQ));
    	olyUI.UpdateActiveXover((LOUD_xover_type)olyParams.Device->Active_Xover);
    	olyUI.UpdatePolarity((bool) olyParams.User[olyParams.Device->Active_User][ePID_OLYspeaker1_USER_SPEAKER_INVERT]);
    	olyUI.UpdateStatusIndicator(true); // turn light green for now
    	if (g_pUltimoPort) {
    		uint16_t tempInt;
    		g_pUltimoPort->GetLinkFlags(&tempInt);
    		OnDanteChange_LinkFlags(tempInt);		// Update Network Indicator
    	}
    	else
    		olyUI.UpdateNetworkIndicator(false);
    	olyUI.UpdateRearDistance(olyParams.Device->Array_Dist_Rear_m);
    	olyUI.UpdateFrontDistance(olyParams.Device->Array_Dist_Front_m);
    	olyUI.UpdateHeight(olyParams.Device->Array_Height_m);
    	olyUI.UpdateAutoStatus(OPTIMIZE_Status_Not_Ready);
    	olyUI.UpdateAutoPosition(olyStatus.Array_Index, olyStatus.Array_Size);
    	olyUI.UpdateCardioidIndex(olyParams.Device->Cardioid_Index);
    	olyUI.UpdateLogoMode((LOUD_logo_mode)olyParams.Device->Logo_Mode);
    	olyUI.UpdateDisplayMode((LOUD_disp_mode)olyParams.Device->Display_Mode);
    	olyUI.UpdateUnitType((bool)olyParams.Device->Use_Metric);
    	olyUI.UpdateStatusLine0(NULL);
    	olyUI.UpdateStatusLine1(NULL);
    	olyUI.UpdateStatusLine2(NULL);
    	olyUI.UpdateStatusLine8(NULL);
    	olyUI.UpdateStatusLine9(NULL);
        #if USE_BACKPANEL_DEBUG && USE_DEBUG_ULTIMO
    				{
    					char tempString[32] = {0};
    					if(DanteMute || GlobalMute)
    						sprintf(tempString, "DSP Muted");
    					else
    						sprintf(tempString, "DSP Unmuted");
    					olyUI.DebugLineOut(1, tempString);
    				}
        #endif
    #endif	//	_SECONDARY_BOOT
    
    #endif // USE_OLY_UI
    
    }
    
    void Config::InitStatusParams() {
    	// SC_COMMENTED_OUT
    	//olyStatus.Angle = 0; // SC : GetRawTilt();
    	olyStatus.Amp1_Temp = GetCurrentAmpTemp(sPro2_amp1);
    	olyStatus.Amp2_Temp = GetCurrentAmpTemp(sPro2_amp2);
    	//olyStatus.Identify = eIDENTIFY_MODE_OFF;
    	olyStatus.SelfTest_Seq = GetSequenceStart();
    	olyStatus.Amp_Fault = get_amp_fault_state();
    	// olyStatus.Logo_State = LogoGetState(); This loads the wrong value!!!
    	olyStatus.Logo_State = LOGO_RUN_STATE_NORMAL;
    	olyStatus.Array_Size = olyParams.Device->Array_Size_Last;
    	olyStatus.Array_Index = olyParams.Device->Array_Index_Last;
    }
       
    void Config::OnIPAddressChanged(uint32_t ip)
    {
    #if USE_OLY_UI
    	olyUI.UpdateIPaddress(ip, m_pUpgrade->GetStaticIp()==0);
    #endif
    }
    
    bool Config::ValidateMAC(uint8 mac[6])
    {
    	uint32_t upper,lower;
    
    	upper = ((mac[0]&0xFF) << 8) | (mac[1] & 0xFF);
    	lower = ((mac[2]&0xFF) << 24) | ((mac[3]&0xFF) << 16) | ((mac[4]&0xFF) << 8) | (mac[5] & 0xFF);
    	if (upper != 0x0000000F)
    		return false;
    	if ((lower < 0xF2080000) | (lower > 0xF208FFFF))
    		return false;
    
    	return true;
    }
    
    
    void Config::MandolinProcess(int nTaskId)
    {
    #ifndef USES_FOUR_IRDA
    	// Check for pending messages from Voicing UART
    	olyVoicingPort.ProcessRxFifo(nTaskId);
    #endif
    }
    
    void Config::SetFanEnabled(bool fan_on)
    {
#ifdef SC_COMMENTED_OUT
    #if !MFG_TEST_EAW && !MFG_TEST_MARTIN
    
    	if (fan_on != olyStatus.Fan_Enabled)
    	{
    		olyStatus.Fan_Enabled = fan_on;
    		GPIO_DRV_WritePinOutput(FAN_CONTROL, olyStatus.Fan_Enabled);
    		olyNetworkPort.WriteMessage(SetParameter(eTARGET_STATUS, 0, ePID_OLYspeaker1_STATUS_FAN_ENABLED, olyStatus.Values[ePID_OLYspeaker1_STATUS_FAN_ENABLED]), true);
    	}
    
    	printf("Fan State = %d\n", olyStatus.Fan_Enabled);
    #elif MFG_TEST_MARTIN
    	GPIO_DRV_WritePinOutput(FAN_CONTROL, fan_on);
    	olyStatus.Fan_Enabled = fan_on;
    #endif // !MFG_TEST_EAW && !MFG_TEST_MARTIN
#endif //  SC_COMMENTED_OUT
    }
    
    void Config::UpdateNeighbourHardwareStatus(void)
    {
#ifdef SC_COMMENTED_OUT
    	int8_t ports;
    	position_hw_data_t *pPortData = 0;
    	uint32_t array_ids[e_num_irda_ports];
    	for(ports = 0; ports < e_num_irda_ports; ports++)
    	{
    		pPortData = GetPortNeighbourHardware(ports);
    		e_irda_hw nPort = (e_irda_hw)ports;
    		array_ids[nPort] = 0;
    		if(pPortData)
    		{
    			switch(nPort)
    			{
    				case e_irda_A_top:
    				case e_irda_B_bottom:
    				case e_irda_C_top_rear:
    				case e_irda_D_bot_rear:
    					array_ids[nPort] = pPortData->hw_data_hi;
    					break;
    				default:
    					break;
    			}
    		}
    		pPortData = 0;
    	}
    
    	uint32_t newId;
    
    	if (!array_ids[e_irda_A_top] && array_ids[e_irda_C_top_rear])
    		newId = array_ids[e_irda_C_top_rear];
    	else
    		newId = array_ids[e_irda_A_top];
    
    	bool configChanged = false;
    	if (olyStatus.Array_ID_Above != newId)
    	{
    		olyStatus.Array_ID_Above = newId;
    		configChanged = true;
    		olyNetworkPort.WriteMessage(SetParameter(eTARGET_STATUS, 0, ePID_OLYspeaker1_STATUS_ARRAY_ID_ABOVE, olyStatus.Values[ePID_OLYspeaker1_STATUS_ARRAY_ID_ABOVE]), true);
    	}
    
    
    	if (!array_ids[e_irda_B_bottom] && array_ids[e_irda_D_bot_rear])
    		newId = array_ids[e_irda_D_bot_rear];
    	else
    		newId = array_ids[e_irda_B_bottom];
    
    	if (olyStatus.Array_ID_Below != newId) {
    		olyStatus.Array_ID_Below = newId;
    		configChanged = true;
    		olyNetworkPort.WriteMessage(SetParameter(eTARGET_STATUS, 0, ePID_OLYspeaker1_STATUS_ARRAY_ID_BELOW, olyStatus.Values[ePID_OLYspeaker1_STATUS_ARRAY_ID_BELOW]), true);
    	}
    
    	if(configChanged)
    		OnArrayConfigurationChanged();
    
    
    #if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
    	{
    		char temp[25];
    		sprintf(temp,"T:%04x B:%04x", olyStatus.Array_ID_Above, olyStatus.Array_ID_Below);
    		olyUI.DebugLineOut(0,temp);
    	}
    #endif	// USE_BACKPANEL_DEBUG
#endif //  SC_COMMENTED_OUT
    }
    
    
    void Config::UpdateArrayStatus(
    		int8_t array_size, int8_t device_index, float deviceHeightOffset,
    		int8_t opt_array_size, int8_t opt_device_index, int8_t opt_sub_size, int8_t opt_sub_index, int8_t opt_orientation,
    		int8_t arrayMismatch, bool arrayReady)
    {
    	if (array_size != olyStatus.Array_Size)
    	{
    		olyStatus.Array_Size = array_size;
    		olyNetworkPort.WriteMessage(SetParameter(eTARGET_STATUS, 0, ePID_OLYspeaker1_STATUS_ARRAY_SIZE, olyStatus.Values[ePID_OLYspeaker1_STATUS_ARRAY_SIZE]), true);
    	}
    
    	if (device_index != olyStatus.Array_Index)
    	{
    		olyStatus.Array_Index = device_index;
    		olyNetworkPort.WriteMessage(SetParameter(eTARGET_STATUS, 0, ePID_OLYspeaker1_STATUS_ARRAY_INDEX, olyStatus.Values[ePID_OLYspeaker1_STATUS_ARRAY_INDEX]), true);
    	}
    
    	optHeightDeviceOffset = deviceHeightOffset;
    	arrayOptSize = opt_array_size;
    	arrayOptPosition = opt_device_index;
    	arrayOptSubSize = opt_sub_size;
    	arrayOptSubPosition = opt_sub_index;
    	arrayOptOrientation = opt_orientation;
    
    	//check for mismatch
    	if((olyParams.Device->Array_Size_Last != array_size) || (olyParams.Device->Array_Index_Last != device_index) || (arrayMismatch != 0))
    	{
    		SetArrayMismatch(true);
    	} else {
    		//TODO: Has boot timeout finished
    		SetArrayMismatch(false);
    	}
    
    #if USE_OLY_UI
    	{
    		olyUI.UpdateAutoPosition(device_index, array_size);
    	}
    #endif

#ifdef SC_COMMENTED_OUT
    	IRDATask_OnArrayMismatchChange(((olyParams.Device->Array_Size_Last == array_size) && (olyParams.Device->Array_Index_Last == device_index)) ? false : true);
#endif //  SC_COMMENTED_OUT

    #if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
    	{
    		char temp[25];
    //		sprintf(temp,"Di:%i Ds:%i M:%i Si:%i Ss:%i", olyParams.Device->Array_Index_Last, olyParams.Device->Array_Size_Last, *(uint32_t*)&olyStatus.Array_Mismatch & 1, olyStatus.Array_Index, olyStatus.Array_Size);
    		sprintf(temp,"Po:%i M:%i Si:%i Ss:%i", olyParams.Device->Array_Was_Optimized & 1, *(uint32_t*)&olyStatus.Array_Mismatch & 1, olyStatus.Array_Index, olyStatus.Array_Size);
    		olyUI.DebugLineOut(1,temp);
    	}
    #endif	// USE_BACKPANEL_DEBUG
    }
    
    // Returns a reversed ip value (i.e. 123.223.323.423 -> 423.323.223.123)
    uint32_t Config::ReverseIpValues(uint32_t ipAddress)
    {
    	return IPREVERSEBYTES(ipAddress);
    }
    
    void Config::SetIP()
    {
    	g_CurrentIpAddress = m_pUpgrade->GetStaticIp();
    	uint32_t uiStaticGateway = m_pUpgrade->GetStaticGateway();
    	uint32_t uiStaticMask = m_pUpgrade->GetStaticMask();
    
    	if (0!=g_CurrentIpAddress)
    	{
    		network_UseStaticIP(&g_CurrentIpAddress, &uiStaticGateway, &uiStaticMask);
    	}else{
    		network_UseDHCP();
    	}
    }
    
    void Config::SwitchToDHCP()
    {
    	if(!GetDhcp())
    	{ // Don't bother if it currently is DHCP
    		m_pUpgrade->SetIpSettings(0,0,0);  //force static IP to zero to make it dhcp permanently
    	}
    }
#endif // 0
    
    //	Messy having this in Config object, won't be necessary when Network.cpp combined with NetworkPort.cpp
    bool Config::GetForceNetworkPortClose(int nPort)
    {
    	return(olyNetworkPort.GetForceClose());
    }

    
//	Messy having this in Config object, won't be necessary when Network.cpp combined with NetworkPort.cpp
void Config::OpenNetworkPort(bool bOpen, int nPort)
{
	olyNetworkPort.OpenPort(bOpen);
}
  
#if 1
    void Config::SetArrayMismatch(bool mismatch)
    {
    	if (olyStatus.Array_Mismatch != mismatch) {
    		olyStatus.Array_Mismatch = mismatch;
    		olyNetworkPort.WriteMessage(SetParameter(eTARGET_STATUS, 0, ePID_OLYspeaker1_STATUS_ARRAY_MISMATCH, olyStatus.Values[ePID_OLYspeaker1_STATUS_ARRAY_MISMATCH]), true);
    	}
    	OnArrayConfigurationChanged();
    }
    
    void Config::SetArrayWasOptimized(bool was_optimized)
    {
    	if (olyParams.Device->Array_Was_Optimized != was_optimized) {
    		olyParams.Device->Array_Was_Optimized = was_optimized;
    		olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_ARRAY_WAS_OPTIMIZED, olyParams.Device->Values[ePID_OLYspeaker1_DEVICE_ARRAY_WAS_OPTIMIZED]), true);
    		OnArrayConfigurationChanged();
    	}
    }
    
    void Config::OnArrayConfigurationChanged()
    {
    #if	USE_OLY_UI
    
    #if !USES_IRDA
    	olyUI.UpdateArrayIndicator(array_none, indicator_gray);
    	return;
    #endif
    
    	indicator_color_t indicator;
    	optimize_status_t status;
    
    	// Was optimized and mismatch
    	if (olyParams.Device->Array_Was_Optimized && olyStatus.Array_Mismatch)
    	{
    		// Mismatch, UI shows "Ready"
    		status = OPTIMIZE_Status_Ready;
    		indicator = indicator_amber;
    	}
    
    	// Was NOT optimized and mismatch
    	else if (olyStatus.Array_Mismatch) {
    		// Ready to Optimize, UI Shows "Ready"
    		status = OPTIMIZE_Status_NotOptimized;
    		indicator = indicator_red; // Red is white
    	}
    
    	// Was optimized and NOT mismatch
    	else if (olyParams.Device->Array_Was_Optimized) {
    		// Sync, UI shows "Optimized"
    		status = OPTIMIZE_Status_Optimized;
    		indicator = indicator_green;
    	}
    
    	// Was NOT optimized and NOT mismatch
    	else {
    		// Not Ready, UI shows "Not Ready"
    		status = OPTIMIZE_Status_Not_Ready;
    		indicator = indicator_amber;
    	}
    
    	if ((olyStatus.Array_ID_Above) && (olyStatus.Array_ID_Below)) {
    		olyUI.UpdateArrayIndicator(array_both, indicator);
    	} else if (olyStatus.Array_ID_Above) {
    		olyUI.UpdateArrayIndicator(array_above, indicator);
    	} else if (olyStatus.Array_ID_Below) {
    		olyUI.UpdateArrayIndicator(array_below, indicator);
    	} else {
    		if (olyParams.Device->Array_Was_Optimized)
    			// Not in array so no indicator, but has optimization.
    			olyUI.UpdateArrayIndicator(array_none, indicator_dirty);
    		else
    			olyUI.UpdateArrayIndicator(array_none, indicator);
    	}
    
    	olyUI.UpdateAutoStatus(status);
    #endif
    }
    
    void Config::OnNetworkConnectionChanged(bool connected)
    {
    	OpenNetworkPort(connected, 0);
    
    #ifndef _SECONDARY_BOOT
    
    	RefreshLogoState();
    
    		//if(!connected)
    		//SetSelfTestMode(eTEST_MODE_DISABLED);
    #endif	//	_SECONDARY_BOOT
    }
    
    char * Config::GetDiscoServiceName()
    {
    	unsigned char *pSystemMAC = Region::GetSystemMAC();
    
    	sprintf(g_DiscoServiceName, "%s\xE2\x80\xA2\%02X%02X%02X",
    				g_pszModelName,
    				*(pSystemMAC+3),
    				*(pSystemMAC+4),
    				*(pSystemMAC+5));
    
    	return g_DiscoServiceName;
    }
    
    bool Config::GetDhcp()
    {
    	return(0==m_pUpgrade->GetStaticIp());
    }
    
    uint32_t Config::GetUserParamsValidIndicator()
    {
    #if USE_CDD_UI
    	uint32_t retval = olyStoredParams.Stored_User_Presets[1][0];
    	return retval;
    #else
    	return 0;
    #endif
    }
    
    void Config::OnDanteChange_OSVersion(uint8_t * osVersion)
    {
    #ifdef USE_OLY_UI
    	olyUI.UpdateDanteVersion((ULTIMO_VERSION *)osVersion);
    #endif
    }
    
    void Config::OnDanteChange_CapVersion(uint8_t * capVersion)
    {
    #ifdef USE_OLY_UI
    	olyUI.UpdateDanteCapVersion((ULTIMO_VERSION *)capVersion);
    #endif
    }
    
    void Config::OnDanteChange_LinkSpeed(uint32 linkSpeed)
    {
    #ifdef USE_OLY_UI
    	char tempString[25];	// Fix: Unallocated data
    	if(linkSpeed > 1000) linkSpeed = 0;
    	sprintf(tempString, "%i Mbps", linkSpeed);
    	olyUI.UpdateStatusLine5(tempString);
    #endif
    }
    
    void Config::OnDanteChange_LinkFlags(uint16_t linkFlags)
    {
    #ifdef USE_OLY_UI
    	char tempString[25];	// Fix: Unallocated data
    	if(linkFlags&1)	{
    		sprintf(tempString, "Connected");
    		if (DanteMute && (ActiveInputSource == src_dante))
    			olyUI.UpdateNetworkIndicator(indicator_red);
    		else
    			olyUI.UpdateNetworkIndicator(indicator_amber);
    	}
    	else {
    		sprintf(tempString, "Disconnected");
    		olyUI.UpdateNetworkIndicator(indicator_gray);
    	}
    	olyUI.UpdateStatusLine4(tempString);
    #endif
    }
    
    void Config::OnDanteChange_MacAddress(uint8_t * macAddress)
    {
    #ifdef USE_OLY_UI
    	olyUI.UpdateDanteMacAddress(macAddress, false);
    #endif
    }
    
    // Note: The Dante Config IP Address is not necessarily the same as the Dante current IP Address
    // i.e. if the dante config IP address is 0.0.0.0 (Dante is DHCP) the current Dante IP address will be something else (DHCP assigned).
    void Config::OnDanteChange_ConfigIpAddress(uint32_t ipAddress, uint32_t ipGateway, uint32_t ipMask)
    {
#ifdef SC_COMMENTED_OUT
    	if(!g_pUltimoPort)
    		return;
    
    	bool bIsDanteStatic;
    
    	switch(g_pUltimoPort->GetDanteDHCP())
    	{
    	case 0:
    		bIsDanteStatic = true;
    		break;
    	case 1:
    		bIsDanteStatic = false;
    		break;
    	case -1:
    	default:
    		// IP is invalid for Dante, disregard this call.
    		return;
    	}
    #ifdef USE_OLY_UI
    	char tempString[25];
    	sprintf(tempString, "Dante IP is %s", bIsDanteStatic?"Static":"DHCP" );
    	olyUI.UpdateStatusLine8(tempString);
    	// Dante IP/Gate/Mask is not reversed so don't use IPBYTES.
    	sprintf(tempString, "Dante IP: %d.%d.%d.%d", (ipAddress>>24)&0x0ff,  (ipAddress>>16)&0x0ff, (ipAddress>>8)&0x0ff, (ipAddress)&0x0ff);
    	olyUI.UpdateStatusLine8(tempString);
    	sprintf(tempString, "Dante Gate: %d.%d.%d.%d", (ipGateway>>24)&0x0ff,  (ipGateway>>16)&0x0ff, (ipGateway>>8)&0x0ff, (ipGateway)&0x0ff);
    	olyUI.UpdateStatusLine8(tempString);
    	sprintf(tempString, "Dante Mask: %d.%d.%d.%d", (ipMask>>24)&0x0ff,  (ipMask>>16)&0x0ff, (ipMask>>8)&0x0ff, (ipMask)&0x0ff);
    	olyUI.UpdateStatusLine8(tempString);
    #endif
    #ifdef	USE_DANTE_AUTO_STATIC_IP
    	// This is a temporary solution to setting the static IP for the Network control based on setting the Ultimo IP from Dante Controller.
    	if(!bIsDanteStatic)
    	{ // Dante is not static so Network IP Should also not be static (DHCP)
    		if(!GetDhcp())
    		{// Change network IP address back to DHCP
    			SwitchToDHCP();
    			m_bdante_auto_ip_address_changed = true; // Setting this to true will reboot the k64 host when the ultimo sends its reboot message.
    		}
    	}
    	else
    	{ // Dante is static so the Network IP should be the Dante static IP +1
    		uint32_t tempIp;
    		// Reverse the IP values since the stored value for the K64 is expected to be reversed.
    		if((ipAddress&0x0ff) == 0x0ff) // If the ip address is x.x.x.255 set the static to 254 instead
    			tempIp = ReverseIpValues(ipAddress-1);
    		else
    			tempIp = ReverseIpValues(ipAddress+1);
    
    		if(GetDhcp() || ( tempIp != g_CurrentIpAddress ))
    		{ // If the Network is currently DHCP or is not a static address that is the Dante IP+1, set it to the new value and reboot when the ultimo reboots.
    			//m_pUpgrade expects the ip/gate/mask to be in the reverse byte order that dante sends it in, so reverse them here.
    			uint32_t tempGateway 	= ReverseIpValues(ipGateway);
    			uint32_t tempMask		= ReverseIpValues(ipMask);
    
    			m_pUpgrade->SetIpSettings(tempIp, tempGateway, tempMask);
    			m_bdante_auto_ip_address_changed = true; // Setting this to true will reboot the k64 host when the ultimo sends its reboot message.
    		}
    	}
    #endif
#endif // SC_COMMENTED_OUT
    }
    
    void Config::OnDanteChange_IpAddress(uint32_t ipAddress)
    {
    #ifdef USE_OLY_UI
    	olyUI.UpdateDanteIPAddress(ipAddress);
    #endif
    }
    
    void Config::OnDanteChange_ModelName(char * modelName)
    {
    
    }
    
    void Config::OnDanteChange_ModelID(uint8_t * modelID)
    {
    
    }
    
    void Config::OnDanteChange_FriendlyName(char * friendlyName)
    {
    	if (strcmp(olyStoredParams.Stored_Labels[eLID_OLYspeaker1_DEVICE_DANTE_DISCO_NAME], friendlyName))
    	{
    		if (strlen(friendlyName) < STRING_PARAMETER_LENGTH-1) {
    			// TODO: Is there other validation necessary here
    			strcpy(olyStoredParams.Stored_Labels[eLID_OLYspeaker1_DEVICE_DANTE_DISCO_NAME], friendlyName);
    			StoreParams();	// This might also not be necessary
    			olyNetworkPort.WriteMessage(SetApplicationString(eLID_OLYspeaker1_DEVICE_DANTE_DISCO_NAME, olyStoredParams.Stored_Labels[eLID_OLYspeaker1_DEVICE_DANTE_DISCO_NAME]), true);
    		}
    	}
    
    #ifdef USE_OLY_UI
    	olyUI.UpdateStatusLine3(friendlyName);
    #endif
    }
    
    void Config::OnDanteChange_DeviceID(uint8_t * deviceID)
    {
    
    }
    
    void Config::OnDanteChange_ChannelStatus(uint16_t channelStatus)
    {
    #ifdef USE_OLY_UI
    	char tempString[25];
    	switch(channelStatus)	{
    	case DDP_RX_CHAN_STATUS_NONE:	sprintf(tempString, "None");	break;
    	case DDP_RX_CHAN_STATUS_UNRESOLVED:	sprintf(tempString, "Unresolved");	break;
    	case DDP_RX_CHAN_STATUS_RESOLVED:	sprintf(tempString, "Resolved");	break;
    	case DDP_RX_CHAN_STATUS_RESOLVE_FAIL:	sprintf(tempString, "Resolve Fail");	break;
    	case DDP_RX_CHAN_STATUS_SUBSCRIBE_SELF:	sprintf(tempString, "Self-Subscribed");	break;
    	case DDP_RX_CHAN_STATUS_IDLE:	sprintf(tempString, "Idle");	break;
    	case DDP_RX_CHAN_STATUS_IN_PROGRESS:	sprintf(tempString, "In Progress");	break;
    	case DDP_RX_CHAN_STATUS_DYNAMIC:	sprintf(tempString, "Dynamic");	break;
    	case DDP_RX_CHAN_STATUS_STATIC:	sprintf(tempString, "Static");	break;
    	case DDP_RX_CHAN_STATUS_MANUAL:	sprintf(tempString, "Manual");	break;
    	case DDP_RX_CHAN_STATUS_NO_CONNECTION:	sprintf(tempString, "No Connection");	break;
    	case DDP_RX_CHAN_STATUS_CHANNEL_FORMAT:	sprintf(tempString, "Format Mismatch");	break;
    	case DDP_RX_CHAN_STATUS_BUNDLE_FORMAT:	sprintf(tempString, "Flow Mismatch");	break;
    	case DDP_RX_CHAN_STATUS_NO_RX:	sprintf(tempString, "No RX");	break;
    	case DDP_RX_CHAN_STATUS_RX_FAIL:	sprintf(tempString, "RX Failed");	break;
    	case DDP_RX_CHAN_STATUS_TX_FAIL:	sprintf(tempString, "TX Failed");	break;
    	case DDP_RX_CHAN_STATUS_QOS_FAIL_RX:	sprintf(tempString, "RX QoS Failed");	break;
    	case DDP_RX_CHAN_STATUS_QOS_FAIL_TX:	sprintf(tempString, "TX QoS Failed");	break;
    	case DDP_RX_CHAN_STATUS_TX_REJECTED_ADDR:	sprintf(tempString, "Addr Err");	break;
    	case DDP_RX_CHAN_STATUS_INVALID_MSG:	sprintf(tempString, "Invalid Msg");	break;
    	case DDP_RX_CHAN_STATUS_CHANNEL_LATENCY:	sprintf(tempString, "Latency Err");	break;
    	case DDP_RX_CHAN_STATUS_CLOCK_DOMAIN:	sprintf(tempString, "Clock Err");	break;
    	case DDP_RX_CHAN_STATUS_UNSUPPORTED:	sprintf(tempString, "Unsupported");	break;
    	case DDP_RX_CHAN_STATUS_RX_LINK_DOWN:	sprintf(tempString, "RX Link Down");	break;
    	case DDP_RX_CHAN_STATUS_TX_LINK_DOWN:	sprintf(tempString, "TX Link Down");	break;
    	case DDP_RX_CHAN_STATUS_DYNAMIC_PROTOCOL:	sprintf(tempString, "Protocol Err");	break;
    	case DDP_RX_CHAN_STATUS_INVALID_CHANNEL:	sprintf(tempString, "Invalid Ch");	break;
    	case DDP_RX_CHAN_STATUS_TX_SCHEDULER_FAILURE:	sprintf(tempString, "Schedule Err");	break;
    	case DDP_RX_CHAN_STATUS_TEMPLATE_MISMATCH_DEVICE:	sprintf(tempString, "Dvc Mismatch");	break;
    	case DDP_RX_CHAN_STATUS_TEMPLATE_MISMATCH_FORMAT:	sprintf(tempString, "Frmt Mismatch");	break;
    	case DDP_RX_CHAN_STATUS_TEMPLATE_MISSING_CHANNEL:	sprintf(tempString, "Ch Missing");	break;
    	case DDP_RX_CHAN_STATUS_TEMPLATE_MISMATCH_CONFIG:	sprintf(tempString, "Cfg Mismatch");	break;
    	case DDP_RX_CHAN_STATUS_TEMPLATE_FULL:	sprintf(tempString, "Template Full");	break;
    	case DDP_RX_CHAN_STATUS_SYSTEM_FAIL:	sprintf(tempString, "System Fail");	break;
    	default:	sprintf(tempString, "None");	break;
    	}
    	olyUI.UpdateStatusLine6(tempString);
    #endif
    }
    
    void Config::OnDanteChange_DeviceStatus(uint32_t deviceStatus)
    {
    
    }
    
    void Config::OnDanteChange_IdentityStatus(uint16_t identityStatus)
    {
    
    }
    
    void Config::OnDanteChange_Reboot(void)
    {
    #ifdef USE_OLY_UI
    	char tempString[25];
    	sprintf(tempString, "Ultimo Rebooting...");
    	olyUI.UpdateStatusLine8(tempString);
    #endif
    
    #ifdef	USE_DANTE_AUTO_STATIC_IP
    	if(m_bdante_auto_ip_address_changed)
    	{
    		if(m_pUpgrade)
    		{
    			m_pUpgrade->SetLaunchType(OLY_REGION_APPLICATION);
    			DeferredReboot(500);
    		}
    	}
    #endif
    }
    
    void Config::OnDanteChange_Upgrade(void)
    {
    
    }
    
    void Config::OnDanteChange_CodecReset(void)
    {
    
    }
    
    bool Config::GetAmpRoutingHasChanged(void)
    {
    #if OLY_UI_MODE_RSX18 && OLY_UI_MODE_HIDDEN_MENU
    	if (olyParams.Voicing[0][ePID_OLYspeaker1_VOICING_AMP1_ROUTING] != AMP1_ROUTING_REV_B) {
    		return true;
    	}
    	if (olyParams.Voicing[0][ePID_OLYspeaker1_VOICING_AMP2_ROUTING] != AMP2_ROUTING_REV_B) {
    		return true;
    	}
    	if (olyParams.Voicing[0][ePID_OLYspeaker1_VOICING_AMP3_ROUTING] != AMP3_ROUTING_REV_B) {
    		return true;
    	}
    	if (olyParams.Voicing[0][ePID_OLYspeaker1_VOICING_AMP4_ROUTING] != AMP4_ROUTING_REV_B) {
    		return true;
    	}
    #endif
    	return false;
    }
    
    void Config::SetAmpRouting(uint32_t amp1, uint32_t amp2, uint32_t amp3, uint32_t amp4)
    {
    	ParamSetVoicing(0, ePID_OLYspeaker1_VOICING_AMP1_ROUTING, amp1);
    	ParamSetVoicing(0, ePID_OLYspeaker1_VOICING_AMP2_ROUTING, amp2);

#if 0
    	ParamSetVoicing(eCARDIOID_CARDIOID_FRONT+1, ePID_OLYspeaker1_VOICING_AMP1_ROUTING, amp1);
    	ParamSetVoicing(eCARDIOID_CARDIOID_FRONT+1, ePID_OLYspeaker1_VOICING_AMP2_ROUTING, amp2);
    	ParamSetVoicing(eCARDIOID_CARDIOID_FRONT+1, ePID_OLYspeaker1_VOICING_AMP3_ROUTING, amp3);
    	ParamSetVoicing(eCARDIOID_CARDIOID_FRONT+1, ePID_OLYspeaker1_VOICING_AMP4_ROUTING, amp4);
    
    	ParamSetVoicing(eCARDIOID_CARDIOID_REAR_2_STACK+1, ePID_OLYspeaker1_VOICING_AMP1_ROUTING, amp1);
    	ParamSetVoicing(eCARDIOID_CARDIOID_REAR_2_STACK+1, ePID_OLYspeaker1_VOICING_AMP2_ROUTING, amp2);
    	ParamSetVoicing(eCARDIOID_CARDIOID_REAR_2_STACK+1, ePID_OLYspeaker1_VOICING_AMP3_ROUTING, amp3);
    	ParamSetVoicing(eCARDIOID_CARDIOID_REAR_2_STACK+1, ePID_OLYspeaker1_VOICING_AMP4_ROUTING, amp4);
    
    	ParamSetVoicing(eCARDIOID_CARDIOID_REAR_3_STACK+1, ePID_OLYspeaker1_VOICING_AMP1_ROUTING, amp1);
    	ParamSetVoicing(eCARDIOID_CARDIOID_REAR_3_STACK+1, ePID_OLYspeaker1_VOICING_AMP2_ROUTING, amp2);
    	ParamSetVoicing(eCARDIOID_CARDIOID_REAR_3_STACK+1, ePID_OLYspeaker1_VOICING_AMP3_ROUTING, amp3);
    	ParamSetVoicing(eCARDIOID_CARDIOID_REAR_3_STACK+1, ePID_OLYspeaker1_VOICING_AMP4_ROUTING, amp4);
#endif
    }
    
    //	Background Processing for connection and sync maintenance
    void Config::Run(int nMs)
    {
    	if (olyNetworkPort.IsPortOpen())
    		olyNetworkPort.Run(nMs);
    //	if (olyVoicingPort.IsPortOpen())
    //		olyVoicingPort.Run(nMs);
    //	if (olyDspPort.IsPortOpen())
    //		olyDspPort.Run(nMs);
    //	if (olyUI.IsPortOpen())
    //		olyUI.Run(nMs);
    }
    
    #ifdef _SECONDARY_BOOT
    bool Config::IsInitialized() { return true; }
    #else //	_SECONDARY_BOOT
    bool Config::IsInitialized() { return olyStoredParams.XML_Version == OLYspeaker1_XML_VERSION; }
    #endif	//	_SECONDARY_BOOT
    
    
    Config::Config() :
    #if USE_OLY_UI
    olyUI(this),
    #endif
    #if USE_CDD_UI
    cddlUI(this),
    #endif
    
    olyDspPort(NULL),
    #if !USES_FOUR_IRDA
    olyVoicingPort(&UartVoiceRxFifo) // SC_COMMENTED_OUT : ,
    #endif
#ifdef SC_COMMENTED_OUT
    olyIrdaPortA(COMM_IRDA_A, irdaTopPortTask), olyIrdaPortB(COMM_IRDA_B, irdaBottomPortTask),
    olyIrdaPortC(COMM_IRDA_C, irdaTopRearPortTask),  olyIrdaPortD(COMM_IRDA_D, irdaBottomRearPortTask)
#endif // SC_COMMENTED_OUT
    {
    	int i;
    	m_pUpgrade = new Region;
    
    	olyParams.Device = &olyStoredParams.Device;
    
    	_time_init_ticks(&selfTest_ticks, 0);
    	_time_add_msec_to_ticks(&selfTest_ticks, TIMEOUT_SELFTEST);
    
    	_time_init_ticks(&dsp_error_ticks, 0);
    	_time_add_sec_to_ticks(&dsp_error_ticks, TIMEOUT_DSP_ERROR);
    
    	/* Initialize Preset Pointers */
    	for (i=0; i< USER_PRESETS; i++) {
    #if USER_PRESETS_STORED
    		if (i<USER_PRESETS_STORED)
    			olyParams.User[i] = olyStoredParams.Stored_User_Presets[i];
    		else
    #endif
    			olyParams.User[i] = (uint32_t*)User_Presets[i];
    	}
    
    	for (i=0; i< PROFILE_PRESETS; i++) {
    #if PROFILE_PRESETS_STORED
    		if (i<PROFILE_PRESETS_STORED)
    			olyParams.Profile[i] = olyStoredParams.Stored_Profile_Presets[i];
    		else
    #endif
    			olyParams.Profile[i] = (uint32_t*)Profile_Presets[i];
    	}
    
    	for (i=0; i< THROW_PRESETS; i++) {
    #if THROW_PRESETS_STORED
    		if (i<THROW_PRESETS_STORED)
    			olyParams.Throw[i] = olyStoredParams.Stored_Throw_Presets[i];
    		else
    #endif
    			olyParams.Throw[i] = (uint32_t*)Throw_Presets[i];
    	}
    
    	for (i=0; i< VOICING_PRESETS; i++) {
    #if VOICING_PRESETS_STORED
    		if (i<VOICING_PRESETS_STORED)
    			olyParams.Voicing[i] = olyStoredParams.Stored_Voicing_Presets[i];
    		else
    #endif
    			olyParams.Voicing[i] = (uint32_t*)Voicing_Presets[i];
    	}
    
    	for (i=0; i< FIR_PRESETS; i++) {
    #if FIR_PRESETS_STORED
    		if (i<FIR_PRESETS_STORED)
    			olyParams.FIR[i] = olyStoredParams.Stored_FIR_Presets[i];
    		else
    #endif
    			olyParams.FIR[i] = (uint32_t*)(void*)FIR_Presets[i];
    	}
    
    	for (i=0; i< XOVER_PRESETS; i++) {
    #if XOVER_PRESETS_STORED
    		if (i<XOVER_PRESETS_STORED)
    			olyParams.Xover[i] = olyStoredParams.Stored_Xover_Presets[i];
    		else
    #endif
    			olyParams.Xover[i] = (uint32_t*)Xover_Presets[i];
    	}
    
    	olyParams.Airloss = olyStoredParams.Stored_Airloss;
    	olyParams.NoiseGate = olyStoredParams.Stored_NoiseGate;
    
    	UpdateNeighbourHardwareStatus();
    
    }
#endif // 0

} /* namespace oly */

