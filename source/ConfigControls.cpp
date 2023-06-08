/*
 * Config_Controls.cpp
 *
 *  Created on: Mar 21, 2016
 *      Author: kris.simonsen
 */

#include "ConfigManager.h"
//#include "IRDA_Task.h"
//#include "IRDAManager.h"
#include "OLYspeaker1_map.h"


namespace oly {

/* Controls functions should only be called by the rear panel.  */

void Config::Controls_SetVolume(float level_db)
{
#ifdef SC_COMMENTED_OUT
	mandolin_parameter_value new_level;
	if (level_db < ParamGetMin(ePID_OLYspeaker1_USER_SPEAKER_FADER))
		new_level.f = ParamGetMin(ePID_OLYspeaker1_USER_SPEAKER_FADER);
	else if (level_db > ParamGetMax(ePID_OLYspeaker1_USER_SPEAKER_FADER))
		new_level.f = ParamGetMax(ePID_OLYspeaker1_USER_SPEAKER_FADER);
	else
		new_level.f = level_db;

	ParamSetUser(olyParams.Device->Active_User+1, ePID_OLYspeaker1_USER_SPEAKER_FADER, new_level);
	olyNetworkPort.WriteMessage(SetParameter(eTARGET_USER, 0, ePID_OLYspeaker1_USER_SPEAKER_FADER, new_level.u), true);

	if(level_db != ParamGetDefault(ePID_OLYspeaker1_USER_SPEAKER_FADER))
	{
		ParamSetDevice(ePID_OLYspeaker1_DEVICE_ARRAY_SIZE_LAST, (void*)&gOLYspeaker1DeviceParameterTable[ePID_OLYspeaker1_DEVICE_ARRAY_SIZE_LAST].maxValue);
		IRDATask_AutoOptimizeError(IRDA_Opti_Mismatch);
		_lwevent_set(&irda_lwevent, irda_event_array_status_update);	//notify task to update
	}
#endif // SC_COMMENTED_OUT
}

float Config::Controls_GetVolume()
{
	return ParamGetValue(olyParams.Device->Active_User, ePID_OLYspeaker1_USER_SPEAKER_FADER);
}

void Config::Controls_SetFuncGenVolume(float level_db)
{
	mandolin_parameter_value new_level;
	if (level_db < ParamGetMin(ePID_OLYspeaker1_USER_INPUT_FUNCGEN_FADER))
		new_level.f = ParamGetMin(ePID_OLYspeaker1_USER_INPUT_FUNCGEN_FADER);
	else if (level_db > ParamGetMax(ePID_OLYspeaker1_USER_INPUT_FUNCGEN_FADER))
		new_level.f = ParamGetMax(ePID_OLYspeaker1_USER_INPUT_FUNCGEN_FADER);
	else
		new_level.f = level_db;

	ParamSetUser(olyParams.Device->Active_User+1, ePID_OLYspeaker1_USER_INPUT_FUNCGEN_FADER, new_level);
	olyNetworkPort.WriteMessage(SetParameter(eTARGET_USER, 0, ePID_OLYspeaker1_USER_INPUT_FUNCGEN_FADER, new_level.u), true);
}

float Config::Controls_GetFuncGenVolume()
{
	return ParamGetValue(olyParams.Device->Active_User, ePID_OLYspeaker1_USER_INPUT_FUNCGEN_FADER);
}

void Config::Controls_SetSpeakerDelay(float delay_ms)
{
#ifdef SC_COMMENTED_OUT
	mandolin_parameter_value new_delay;
	if (delay_ms < ParamGetMin(ePID_OLYspeaker1_USER_SPEAKER_DELAY_TIME))
		new_delay.f = ParamGetMin(ePID_OLYspeaker1_USER_SPEAKER_DELAY_TIME);
	else if (delay_ms > ParamGetMax(ePID_OLYspeaker1_USER_SPEAKER_DELAY_TIME))
		new_delay.f = ParamGetMax(ePID_OLYspeaker1_USER_SPEAKER_DELAY_TIME);
	else
		new_delay.f = delay_ms;

	ParamSetUser(olyParams.Device->Active_User+1, ePID_OLYspeaker1_USER_SPEAKER_DELAY_TIME, new_delay);
	olyNetworkPort.WriteMessage(SetParameter(eTARGET_USER, 0, ePID_OLYspeaker1_USER_SPEAKER_DELAY_TIME, new_delay.u), true);

	if(delay_ms != ParamGetDefault(ePID_OLYspeaker1_USER_SPEAKER_DELAY_TIME)) {
		ParamSetDevice(ePID_OLYspeaker1_DEVICE_ARRAY_SIZE_LAST, (void*)&gOLYspeaker1DeviceParameterTable[ePID_OLYspeaker1_DEVICE_ARRAY_SIZE_LAST].maxValue);
		IRDATask_AutoOptimizeError(IRDA_Opti_Mismatch);
		_lwevent_set(&irda_lwevent, irda_event_array_status_update);	//notify task to update
	}
#endif // SC_COMMENTED_OUT
}

float Config::Controls_GetSpeakerDelay()
{
	return ParamGetValue(olyParams.Device->Active_User, ePID_OLYspeaker1_USER_SPEAKER_DELAY_TIME);
}

void Config::Controls_SetProfile(LOUD_oly_profiles profile)
{
#ifdef SC_COMMENTED_OUT
	array_opt_vals_t opt_vals;

	SetActivePreset(eTARGET_PROFILE, (uint32_t)profile);
	olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_ACTIVE_PROFILE, olyParams.Device->Active_Profile), true);

	IRDA_GetOptMsgValues(&opt_vals);
	if(*(uint32_t*)&profile != opt_vals.irda_Active_Profile_instance) {
		ParamSetDevice(ePID_OLYspeaker1_DEVICE_ARRAY_SIZE_LAST, (void*)&gOLYspeaker1DeviceParameterTable[ePID_OLYspeaker1_DEVICE_ARRAY_SIZE_LAST].maxValue);
		IRDATask_AutoOptimizeError(IRDA_Opti_Mismatch);
		_lwevent_set(&irda_lwevent, irda_event_array_status_update);	//notify task to update
	}
#endif // SC_COMMENTED_OUT
}

void Config::Controls_SetThrow(uint32_t throwNum)
{
	uint32_t tempThrowNum;

	if(throwNum < 0)	tempThrowNum= 0;
	else if(throwNum > THROW_PRESETS-1)	tempThrowNum= THROW_PRESETS-1;
	else	tempThrowNum = throwNum;
//	ParamSetDevice(ePID_OLYspeaker1_DEVICE_ACTIVE_THROW, &tempThrowNum);
	SetActivePreset(eTARGET_THROW, tempThrowNum);
	olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_ACTIVE_THROW, olyParams.Device->Active_Throw), true);
}

void Config::Controls_SetCardioid(LOUD_cardioid cardioid, uint32_t indexNum)
{
	/* Set the voicing */
	SetActivePreset(eTARGET_VOICING, (uint32_t)cardioid);
	olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_ACTIVE_VOICING, olyParams.Device->Active_Voicing), true);

	/* Set the graphical index */
	ParamSetDevice(ePID_OLYspeaker1_DEVICE_CARDIOID_IDX, &indexNum);
	olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_CARDIOID_IDX, olyParams.Device->Cardioid_Index), true);
}

void Config::Controls_SetXoverMode(LOUD_xover_type mode)
{
	mandolin_parameter_value val;
	SetActivePreset(eTARGET_XOVER, (uint32_t)mode);
	olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_ACTIVE_XOVER, olyParams.Device->Active_Xover), true);

	if (mode == eXOVER_TYPE_VARIABLE){
		// Re-enable the crossover if disabled by Mosaic
		val.u = olyParams.Xover[eXOVER_TYPE_VARIABLE][ePID_OLYspeaker1_XOVER_SPEAKER_XOVER_ENABLE];
		if (!val.b) {
			val.b = true;
			ParamSetXover(eXOVER_TYPE_VARIABLE+1,ePID_OLYspeaker1_XOVER_SPEAKER_XOVER_ENABLE,val);
			olyNetworkPort.WriteMessage(SetParameter(eTARGET_XOVER, eXOVER_TYPE_VARIABLE, ePID_OLYspeaker1_XOVER_SPEAKER_XOVER_ENABLE, val.u), true);
		}
	}
}

void Config::Controls_SetXoverFreq(float center_freq)
{
#if 1 //TODO:
	mandolin_parameter_value new_freq;
	if (center_freq < ParamXoverGetMin(ePID_OLYspeaker1_XOVER_SPEAKER_XOVER_FREQ))
		new_freq.f = ParamXoverGetMin(ePID_OLYspeaker1_XOVER_SPEAKER_XOVER_FREQ);
	else if (center_freq > ParamXoverGetMax(ePID_OLYspeaker1_XOVER_SPEAKER_XOVER_FREQ))
		new_freq.f = ParamXoverGetMax(ePID_OLYspeaker1_XOVER_SPEAKER_XOVER_FREQ);
	else
		new_freq.f = center_freq;

	ParamSetXover(eXOVER_TYPE_VARIABLE+1, ePID_OLYspeaker1_XOVER_SPEAKER_XOVER_FREQ, new_freq);
	olyNetworkPort.WriteMessage(SetParameter(eTARGET_XOVER, eXOVER_TYPE_VARIABLE, ePID_OLYspeaker1_XOVER_SPEAKER_XOVER_FREQ, new_freq.u), true);
#endif
}

float Config::Controls_GetXoverFreq()
{
	return ParamXoverGetValue(eXOVER_TYPE_VARIABLE, ePID_OLYspeaker1_XOVER_SPEAKER_XOVER_FREQ);
}

void Config::Controls_SetPolarityInvert(bool invert)
{
	mandolin_parameter_value polarity_invert;
	polarity_invert.b = invert;

	ParamSetUser(olyParams.Device->Active_User+1, ePID_OLYspeaker1_USER_SPEAKER_INVERT, polarity_invert);
	olyNetworkPort.WriteMessage(SetParameter(eTARGET_USER, 0, ePID_OLYspeaker1_USER_SPEAKER_INVERT, polarity_invert.u), true);
}

bool Config::Controls_GetPolarityInvert()
{
	return 0;
}

//void __attribute__((optimize("O0")))Config::Controls_AutoOptimize(float dMax, float dMin, float height, uint8_t source)
void Config::Controls_AutoOptimize(float dMax, float dMin, float height, uint8_t source, uint32_t active_profile)
{
#ifdef SC_COMMENTED_OUT
	bool32 optResult;
	Controls_SetArrayIndexFromStatus();
	Controls_SetArraySizeFromStatus();

	switch(source)
	{
	case optimize_source_panel:
		Controls_SetMaxDist(dMax);	//set olyParms.Device value, update network
		Controls_SetMinDist(dMin); //set olyParms.Device value, update network
		Controls_SetHeight(height); //set olyParms.Device value, update network
		IRDATask_AutoOptimizeStart(dMax, dMin, height, olyParams.Device->Active_Profile);
		//GetCurrentTiltAngle();	// Should already be updated
		break;
	case optimize_source_irda:
		Controls_SetMaxDist(dMax);
		Controls_SetMinDist(dMin);
		Controls_SetHeight(height);
		if(active_profile < PROFILE_PRESETS)
		{
			SetActivePreset(eTARGET_PROFILE, active_profile);
			olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_ACTIVE_PROFILE, olyParams.Device->Values[ePID_OLYspeaker1_DEVICE_ACTIVE_PROFILE]), true);
		}

		break;
	case optimize_source_network:
		Controls_SetMaxDist(dMax);	//set olyParms.Device value, update network
		Controls_SetMinDist(dMin); //set olyParms.Device value, update network
		Controls_SetHeight(height); //set olyParms.Device value, update network
		IRDATask_AutoOptimizeStart(dMax, dMin, height, active_profile);
		break;
	}

	SetArrayMismatch(false);
	IRDATask_OnArrayMismatchChange(false);

	optResult = ComputeAutoOptimize();

	ParamSetDevice(ePID_OLYspeaker1_DEVICE_ARRAY_WAS_OPTIMIZED, &optResult);
	olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_ARRAY_WAS_OPTIMIZED, olyParams.Device->Values[ePID_OLYspeaker1_DEVICE_ARRAY_WAS_OPTIMIZED]), true);

	_lwevent_set(&irda_lwevent, irda_event_array_status_update);	//notify task to update
#endif // SC_COMMENTED_OUT
}


void Config::Controls_SetMaxDist(float max_distance_meters)
{
	float32 new_max = max_distance_meters;
	ParamSetDevice(ePID_OLYspeaker1_DEVICE_ARRAY_DIST_REAR_M, &new_max);
	olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_ARRAY_DIST_REAR_M, olyParams.Device->Values[ePID_OLYspeaker1_DEVICE_ARRAY_DIST_REAR_M]), true);
}

float Config::Controls_GetMaxDist(void)
{
	return (float) olyParams.Device->Array_Dist_Rear_m;
}

void Config::Controls_SetMinDist(float min_distance_meters)
{
	float32 new_min = (float32) min_distance_meters;
	ParamSetDevice(ePID_OLYspeaker1_DEVICE_ARRAY_DIST_FRONT_M, &new_min);
	olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_ARRAY_DIST_FRONT_M, olyParams.Device->Values[ePID_OLYspeaker1_DEVICE_ARRAY_DIST_FRONT_M]), true);
}

float Config::Controls_GetMinDist(void)
{
	return (float) olyParams.Device->Array_Dist_Front_m;
}

void Config::Controls_SetHeight(float value)
{
	float32 temp = (float32) value;
	ParamSetDevice(ePID_OLYspeaker1_DEVICE_ARRAY_HEIGHT_M, &temp);
	olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_ARRAY_HEIGHT_M, olyParams.Device->Values[ePID_OLYspeaker1_DEVICE_ARRAY_HEIGHT_M]), true);
}

float Config::Controls_GetHeight(void)
{
	return (float) olyParams.Device->Array_Height_m;
}

float Config::GetDeviceHeightOffset(void)
{
	return optHeightDeviceOffset;
}

int8_t Config::GetOptimzeDeviceArrayIndex(void)
{
	return arrayOptPosition;
}
int8_t Config::GetOptimizeDeviceArraySize(void)
{
	return arrayOptSize;
}

int8_t Config::GetOptimizeDeviceSubIndex(void)
{
	return arrayOptSubPosition;
}
int8_t Config::GetOptimizeDeviceSubSize(void)
{
	return arrayOptSubSize;
}
int8_t Config::GetOptimizeDeviceOrientation(void)
{
	return arrayOptOrientation;
}
void Config::Controls_SetDisplayMode(LOUD_disp_mode mode)
{
	// TODO: notify network of LCD brightness?
	uint32_t temp = (uint32_t) mode;
	ParamSetDevice(ePID_OLYspeaker1_DEVICE_DISPLAY_MODE, &temp);
	olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_DISPLAY_MODE, olyParams.Device->Display_Mode), true);
}

void Config::Controls_SetArrayIndexFromStatus(void)
{
	uint32_t array_index = olyStatus.Array_Index;
	ParamSetDevice(ePID_OLYspeaker1_DEVICE_ARRAY_INDEX_LAST, &array_index);
	olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_ARRAY_INDEX_LAST, olyParams.Device->Array_Index_Last), true);
}

uint32_t Config::Controls_GetArrayIndex(void)
{
	return (uint32_t) olyParams.Device->Array_Index_Last;
}

void Config::Controls_SetArraySizeFromStatus(void)
{
	uint32_t array_size = olyStatus.Array_Size;
	ParamSetDevice(ePID_OLYspeaker1_DEVICE_ARRAY_SIZE_LAST, &array_size);
	olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_ARRAY_SIZE_LAST, olyParams.Device->Array_Size_Last), true);
}

uint32_t Config::Controls_GetArraySize(void)
{
	return (uint32_t) olyParams.Device->Array_Size_Last;
}

void Config::Controls_RestoreDefaults(void)
{
	RestoreDefaults(false);
	olyNetworkPort.WriteMessage(GetSyncMessage(MANDOLIN_SE_SYNC_CURRENT_STATE), true);
}

void Config::Controls_RestoreIPAddresses(void)
{
	RestoreIPAddresses();
}

void Config::Controls_SetLogoMode(LOUD_logo_mode mode)
{
	uint32_t temp = (uint32_t) mode;
	ParamSetDevice(ePID_OLYspeaker1_DEVICE_LOGO_MODE, &temp);
	olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_LOGO_MODE, olyParams.Device->Logo_Mode), true);
}

void Config::Controls_SetUnitType(bool type)
{
	uint32_t temp = (uint32_t) type;
	ParamSetDevice(ePID_OLYspeaker1_DEVICE_USE_METRIC, &temp);
	olyNetworkPort.WriteMessage(SetParameter(eTARGET_DEVICE, 0, ePID_OLYspeaker1_DEVICE_USE_METRIC, olyParams.Device->Use_Metric), true);
}

bool Config::Controls_GetUnitType()
{
	return olyParams.Device->Use_Metric;
}

} // namespace oly

