/*
 * ConfigManager.h
 *
 *  Created on: Jun 18, 2015
 *      Author: Kris.Simonsen
 */

#ifndef CONFIGMANAGER_H_
#define CONFIGMANAGER_H_

#include "MQX_To_FreeRTOS.h"

extern "C" {
#include "oly.h"
#include "timer.h"
#include "CurrentSense.h"
}

#if USE_OLY_UI
#include "RearPanelUI.h"
#endif
#if USE_CDD_UI
#include "CddlMembraneUI.h"
#endif


#include "SpiSharcPort.h"
#include "UartPort.h"
//#include "uart_voice.h"
//#include "uart_irda.h"
#include "NetworkPort.h"
//#include "IRDAUartPort.h"

enum{
	optimize_source_panel,
	optimize_source_irda,
	optimize_source_network
};


#define CONNECT_MANAGEMENT_PERIOD 100	//	Time period for strobe to connect management system

#define LCD_DIM_VALUE		25
#define LCD_BRIGHT_VALUE	100

#define FLIP_SCREEN_MIN		135
#define FLIP_SCREEN_MAX		225

#define USE_SCREEN_IDENTIFY	1

class Region;

extern const char c_DefaultDeviceName[];
extern const char c_DefaultCustom1Name[];

namespace oly {

class Config
{
protected:
	oly_params_t 	   olyParams;
	oly_flash_params_t olyStoredParams;
	oly_status_t 	olyStatus;
	float			olyMeterForUI;
	mandolin_parameter_value	olyCurrentSenseMeter[3];

	bool IsPhysicallyUpsideDown=0;

	oly_audio_source_t  ActiveInputSource;	// TODO: Move to Status parameter?
	bool GlobalMute=0;
	bool DanteMute=0;

	_timer_id		SubscriptionTimers[ePID_OLYspeaker1_STATUS_FENCE];
	MQX_TICK_STRUCT SubscriptionTicks[ePID_OLYspeaker1_STATUS_FENCE];

	_timer_id		identify_timer;
	MQX_TICK_STRUCT identify_ticks;

	_timer_id		dsp_error_timer;
	MQX_TICK_STRUCT dsp_error_ticks;

	float Audio_Level =0;
	float optHeightDeviceOffset=0;			//this device's distance in meters from the top of the array
	int8_t arrayOptSize;					//count of optimizable devices in array
	int8_t arrayOptPosition;				//position of device in optimizable array (0 = not in optimizable array)
	int8_t arrayOptSubSize;					//count of optimizable subs in array
	int8_t arrayOptSubPosition;				//position of device in sub array (0 = not in optimizable array)
	int8_t arrayOptOrientation;

	Region *m_pUpgrade = 0;
	bool m_bCycle;

	bool			previous_hf_test_value = false;
	bool			previous_hf_mute_value = false;
	bool			previous_hf_solo_value = false;

	bool			previous_mf_test_value = false;
	bool			previous_mf_mute_value = false;
	bool			previous_mf_solo_value = false;

	bool			previous_lf_test_value = false;
	bool			previous_lf_mute_value = false;
	bool			previous_lf_solo_value = false;

#ifdef	USE_DANTE_AUTO_STATIC_IP
	bool 			m_bdante_auto_ip_address_changed = false;
#endif
public:

	LOUD_test_mode	previous_self_test_mode = eTEST_MODE_DISABLED;

#if USE_OLY_UI
	RearPanelUI		olyUI;
#endif
#if USE_CDD_UI
	cddlMembraneUI		cddlUI;
#endif
	SpiSharcPort	olyDspPort;

#if !USES_FOUR_IRDA
	UartPort		olyVoicingPort;
#endif
	NetworkPort		olyNetworkPort;
//	IRDAUartPort	olyIrdaPortA;
//	IRDAUartPort	olyIrdaPortB;
//	IRDAUartPort	olyIrdaPortC;
//	IRDAUartPort	olyIrdaPortD;

	Config();

	/* Config Functions */
	void SetFanEnabled(bool fan_on);
	void SetIP();
	void SwitchToDHCP();
	bool GetDhcp();
	void Run(int nMs);

	oly_params_t *GetParamsDatabase();
	oly_flash_params_t *GetStoredParamsDatabase();
	oly_status_t *GetStatusParamsDatabase();
	void SetActivePreset(OLY_target group, uint32_t instance);
	
	void SetInputSelectMode(LOUD_audio_mode mode);
	uint8_t GetInputSelectMode();
	void SetDisplayUserMode(LOUD_disp_mode display_mode);
	void RefreshLogoState();
	void SetLogoMeter(float db);
	LOUD_disp_mode GetDisplayUserMode(void);
	void SetCardioidIndex(uint32_t indexNum);
	dsp_channels_t GetSequenceStart();
	void SetArrayWasOptimized(bool on);
	void SetArrayMismatch(bool mismatch);
	void UpdateNeighbourHardwareStatus(void);
	void UpdateArrayStatus(
			int8_t array_size, int8_t device_index, float deviceHeightOffset,
			int8_t opt_array_size, int8_t opt_device_index, int8_t opt_sub_size, int8_t opt_sub_index, int8_t opt_orientation,
			int8_t arrayMismatch, bool arrayReady);

	uint32_t ReverseIpValues(uint32_t ipAddress);

	/* Status Functions */
	void SetSelfTestMode(LOUD_test_mode test_mode);
	LOUD_test_mode GetSelfTestMode(void);

	void ResetSelfTestTimer();
	void DestroySelfTestTimer();
	void SelfTestSequence();
	void SetDeviceHfTest(bool isActive);
	bool32 GetDeviceHfTest(void);
	void SetDeviceMfTest(bool isActive);
	bool32 GetDeviceMfTest(void);
	void SetDeviceLfTest(bool isActive);
	bool32 GetDeviceLfTest(void);
	void EvaluateChTestStates(void);
	void SetGlobalMute(bool mute);
	void SetHFMute(bool mute);
	void SetHFSolo(bool solo);
	void SetMFMute(bool mute);
	void SetMFSolo(bool solo);
	void SetLFMute(bool mute);
	void SetLFSolo(bool solo);
	float GetCurrentAmpTemp(amp_instance_t amp);
	void UpdateCurrentTiltAngle(void);
	
	void OnIPAddressChanged(uint32_t ip);
	void OnAmpFaultStatusChanged(bool fault);
	void OnLimiterStatusChanged(bool limiting);

	void OnArrayConfigurationChanged(void);
	void OnFanStatusChanged(bool fan_on);
	void OnAmpTempChanged(amp_instance_t amp, float32 temp);
	void OnParamSubscriptionEventVoicingList(int listId);
	void OnParamSubscriptionEventNetworkList(int listId);
	void SaveTestValues(void);
	void RestoreTestValues(void);
	void SaveDriverValues(void);
	void RestoreDriverValues(void);
	void OnSelfTestModeChanged(LOUD_test_mode mode);
	void SetSelfTestSequence(dsp_channels_t sequence);
	void OnMuteSoloChanged();

	void KillAllSubscriptions(void);
	
	void GetCurrentMeter();
	float GetCurrentUIMeter();
	bool MeterRangeCheck(float min, float max);
	float GetDeviceHeightOffset(void);
	int8_t GetOptimzeDeviceArrayIndex(void);
	int8_t GetOptimizeDeviceArraySize(void);
	int8_t GetOptimizeDeviceSubIndex(void);
	int8_t GetOptimizeDeviceSubSize(void);
	int8_t GetOptimizeDeviceOrientation(void);
	bool GetAmpFaultStatus(void);
	void OnDanteMuteChanged();
	void CheckOrientation();
	void CheckInitialOrientation();

	float CurrentSenseGetOutputMeter(uint32_t channelNumber);
	void CurrentSenseGetImpedanceLimits(current_sense_Z_limits_t * current_sense_Z_limits);
	void OnCurrentSenseImpedanceChanged(uint32_t channelNumber);

	/* Parameter Controls - UI only */
	void  Controls_SetVolume(float level_db);
	float Controls_GetVolume();
	void Controls_SetFuncGenVolume(float level_db);
	float Controls_GetFuncGenVolume();
	void Controls_SetSpeakerDelay(float delay_ms);
	float Controls_GetSpeakerDelay();
	void Controls_SetThrow(uint32_t throwNum);
	void Controls_SetProfile(LOUD_oly_profiles profile);
	void Controls_SetCardioid(LOUD_cardioid cardioid, uint32_t indexNum);
	void Controls_SetXoverMode(LOUD_xover_type mode);
	void Controls_SetXoverFreq(float center_freq);
	float Controls_GetXoverFreq();
	void Controls_SetPolarityInvert(bool invert);
	bool Controls_GetPolarityInvert(void);
	void Controls_AutoOptimize(float dMax, float dMin, float height, uint8_t source, uint32_t active_profile);
	void Controls_ApplyAutoOptimize(void);
	void Controls_SetMaxDist(float max_distance_meters);
	float Controls_GetMaxDist(void);
	void Controls_SetMinDist(float min_distance_meters);
	float Controls_GetMinDist(void);
	void Controls_SetHeight(float value);
	float Controls_GetHeight(void);
	void Controls_SetDisplayMode(LOUD_disp_mode mode);
	void Controls_SetArrayIndexFromStatus(void);
	uint32_t Controls_GetArrayIndex(void);
	void Controls_SetArraySizeFromStatus(void);
	uint32_t Controls_GetArraySize(void);

	void Controls_RestoreDefaults(void);
	void Controls_RestoreIPAddresses(void);

	void Controls_SetLogoMode(LOUD_logo_mode mode);
	void Controls_SetUnitType(bool type);
	bool Controls_GetUnitType();

	/* UI Functions */
	void EncoderL();
	void EncoderR();
	void EncoderSW();
	void Switch2();
	void SW2_long();
	void SW2_long_long();

	/* System Functions */
	void LoadAllFromFlash();
	
	void StoreParams();			// Starts the countdown before writing to flash
	
	void WriteParamsToFlash();	// Actually writes to flash
	
	void RestoreDefaults(bool startup);
	
	void RestoreIPAddresses();
	void RestoreUserEQ(int instance);
	
	void ClearOptimization();
	float ParamGetMin(OLYspeaker1_USER_pid PID);
	float ParamGetMax(OLYspeaker1_USER_pid PID);
	float ParamGetDefault(OLYspeaker1_USER_pid PID);
	float ParamGetValue(uint32_t instance, OLYspeaker1_USER_pid PID);
	float ParamXoverGetMin(OLYspeaker1_XOVER_pid PID);
	float ParamXoverGetMax(OLYspeaker1_XOVER_pid PID);
	float ParamXoverGetValue(uint32_t instance, OLYspeaker1_XOVER_pid PID);
	mandolin_error_code ParamValidate(OLY_target group, uint32_t PID, mandolin_parameter_value * pValue);
	
	void SetAudioSource(oly_audio_source_t source);
	oly_audio_source_t GetAudioSource(void);
	void EvaluateFanControl();
	void SetAmpRouting(uint32_t amp1, uint32_t amp2, uint32_t amp3, uint32_t amp4);
	bool GetAmpRoutingHasChanged(void);
	void SetModel(LOUD_brand brand, uint32_t model_id);

	/* Error Functions */
	void DSPErrorTimer_Cancel(void);

	void DSPErrorTimer_Reset(void); 
	
	void SendGetHardwareInfo(MandolinPort * srcPort);

	void SetLcdBrightness(int level);
	void RampLcdBrightness(bool set_ramp, uint32_t level);
	uint32_t GetStoredBrightness(void);
	uint32_t GetActiveBrightness(void);
	void DimLcdBrightness(bool isDim);
	bool GetIsPhysicallyUpsideDown();
	int32 GetTiltAngle();
	void UpdateOrientation(bool initial, bool flipped);
	void InitUI();	
	void InitStatusParams();	
	char * GetDiscoServiceName();
	void OnNetworkConnectionChanged(bool connected);
	
	void OpenNetworkPort(bool bOpen, int nPort);
	bool GetForceNetworkPortClose(int nPort);
	

	void IdentifyStart(uint32_t ms, LOUD_identify_mode mode);
	void IdentifyStop();
	LOUD_identify_mode GetIdentifyMode(void);

	bool ValidateMAC(uint8 mac[6]);
	void MandolinProcess(int nTaskId);
	void MandolinHandle(MandolinPort * srcPort, mandolin_mqx_message * msg_ptr);
	bool IsInitialized();
	void getIpSettings( uint32_t &ip, uint32_t &subnet, uint32_t &gw );
	uint32_t GetUserParamsValidIndicator();
	bool32 ComputeAutoOptimize(void);
	void OptiCalcAndSetAirloss(uint32_t compEQ);

	// For handling Dante changes //
	void OnDanteChange_OSVersion(uint8_t * osVersion);
	void OnDanteChange_CapVersion(uint8_t * capVersion);
	void OnDanteChange_LinkSpeed(uint32 linkSpeed);
	void OnDanteChange_LinkFlags(uint16_t linkFlags);
	void OnDanteChange_MacAddress(uint8_t * macAddress);
	void OnDanteChange_ConfigIpAddress(uint32_t ipAddress, uint32_t ipGateway, uint32_t ipMask);
	void OnDanteChange_IpAddress(uint32_t ipAddress);
	void OnDanteChange_ModelName(char * modelName);
	void OnDanteChange_ModelID(uint8_t * modelID);
	void OnDanteChange_FriendlyName(char * friendlyName);
	void OnDanteChange_DeviceID(uint8_t * deviceID);
	void OnDanteChange_ChannelStatus(uint16_t channelStatus);
	void OnDanteChange_DeviceStatus(uint32_t deviceStatus);
	void OnDanteChange_IdentityStatus(uint16_t identityStatus);
	void OnDanteChange_Reboot(void);
	void OnDanteChange_Upgrade(void);
	void OnDanteChange_CodecReset(void);

private:
	void HandleGetSoftwareInfo(MandolinPort * srcPort, mandolin_message * pMsg);
	void SelfTestEnableChannel(int channel);
	void ParamSetDevice(OLYspeaker1_DEVICE_pid PID, void * Value);
	void ParamSetUser(uint32_t instance, OLYspeaker1_USER_pid PID, mandolin_parameter_value Value);
	void ParamSetVoicing(uint32_t instance, OLYspeaker1_VOICING_pid PID, uint32_t Value);
	void ParamSetFIR(uint32_t instance,OLYspeaker1_FIR_pid PID, uint32_t Value);
	void ParamSetThrow(uint32_t instance,OLYspeaker1_THROW_pid PID, uint32_t Value);
	void ParamSetProfile(uint32_t instance,OLYspeaker1_PROFILE_pid PID, uint32_t Value);
	void ParamSetXover(uint32_t instance,OLYspeaker1_XOVER_pid PID, mandolin_parameter_value Value);
	void ParamSetAirloss(OLYspeaker1_AIRLOSS_pid PID, mandolin_parameter_value Value);
	void ParamSetNoiseGate(OLYspeaker1_NOISEGATE_pid PID, mandolin_parameter_value Value);
	void HandleSetApplicationParameter(MandolinPort * srcPort, mandolin_message * pMsg);
	void HandleGetApplicationParameters(MandolinPort * srcPort, mandolin_message * pMsg);
	void HandleSetApplicationString(MandolinPort * srcPort, mandolin_message * pMsg);
	void HandleGetApplicationString(MandolinPort * srcPort, mandolin_message * pMsg);
	void HandleCreateParameterList(MandolinPort * srcPort, mandolin_message * pMsg);
	void HandleSubscribeParameter(MandolinPort * srcPort, mandolin_message * pMsg);
	void HandleMetersResponse(mandolin_message * pMsg);
	void HandleFastConnect(MandolinPort * srcPort, mandolin_message * pMsg);

	/* Test Functions */
	void HandleTestCommand(MandolinPort * srcPort, mandolin_message * pMsg);
	void HandleGetSystemStatus(MandolinPort * srcPort, mandolin_message * pMsg);
	void HandleSystemEvent(MandolinPort * srcPort, mandolin_message *pMsg);
	void SoftwareReset(MandolinPort * srcPort, mandolin_message * pMsg);
	void HandleFileMessage(MandolinPort * srcPort, mandolin_message * pMsg);
#if MFG_TEST_EAW || MFG_TEST_MARTIN
	bool Mfg_SetOutputPinValue(uint32_t pinID, uint32_t value);
	bool Mfg_GetInputPinValue(uint32_t pinID);
	void Mfg_SetAudioTestSource(oly_audio_source_t source);
	void Mfg_SetFunctionGenerator(mandolin_parameter_value fader, mandolin_parameter_value frequency);
#endif // MFG_TEST_EAW || MFG_TEST_MARTIN
};

} /* namespace oly */

#endif /* CONFIGMANAGER_H_ */
