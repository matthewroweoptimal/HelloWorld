/*
 * oly.h
 *
 *  Created on: May 7, 2015
 *      Author: Kris.Simonsen
 */

#ifndef OLY_H_
#define OLY_H_

#ifndef PLATFORM_OLY
#define PLATFORM_OLY
#endif

#include <stdint.h>
#include "OLY_meta.h"
#include "OLYspeaker1_pids.h"
#include "LOUD_defines.h"
#include "LOUD_enums.h"
#include "LOUD_types.h"

#ifndef _WIN32
#include "Model_Info.h"
#endif


#ifdef DANTE_V4
 #define OLY_FW_VERSION		    FW_VERSION_CONST(2,2,43)
 #define MIN_FW_VERSION_MAJOR   2                           // Cannot upgrade FW to versions below this
 #define MIN_FW_VERSION_MINOR   2
 #define MIN_FW_VERSION_SUB     43

#else
 #define OLY_FW_VERSION		    FW_VERSION_CONST(1,1,35)
 #define MIN_FW_VERSION_MAJOR   1                           // Cannot upgrade FW to versions below this
 #define MIN_FW_VERSION_MINOR   1
 #define MIN_FW_VERSION_SUB     13
#endif

#define DANTE_OS_VERSION		FW_VERSION_CONST(2,2,3,1)

#ifndef BUILD_VERSION
 #define BUILD_VERSION 			0
#endif

/* Parameter Definitions */
#define PARAM_STORE_TIMEOUT_MS	1000	// Time after last parameter update before store to flash
#define METER_POLLING_WAIT_MS	5000	// Wait time to start periodic meter polling at boot (milliseconds)
#define DSP_METER_RATE_MS		20

#define FAST_CONNECT_PAYLOAD_LENGTH	8

#define STRING_PARAMETER_LENGTH	64

#define LIMITER_LED_THRESHOLD_DB	-12.0
#define FLOAT_NEG_INFINITY			-3.4028235E38
#define FLOAT_NEG_INFINITY_AS_HEX	0xFF7FFFFF		// Used for comparasin to above to avoid rounding errors
#define METER_WORD_FOR_LED			eMID_OLYspeaker1_INPUT_IN
#define METER_WORD_FOR_APP			eMID_OLYspeaker1_GROUP_SOFTLIMIT_OUT

typedef union {
    struct {
    uint8_t major;
    uint8_t minor;
    uint8_t sub;
    uint8_t build;      // Not used by application, build versioning only
    };
    uint32_t u32;
} oly_version_t;

#define FW_VERSION_CONST(major,minor,sub)   (((BUILD_VERSION & 0xFF)<<24)|((sub&0xFF)<<16)|((minor&0xFF)<<8)|major)

#define MAX_TCP_CLIENTS			1

typedef enum {
	ch_none = 0,
	ch_HF = 1,
	ch_MF = 2,
	ch_LF = 3,
	ch_All = 4,
} dsp_channels_t;

typedef enum {
	sPro2_amp1 = 1,
	sPro2_amp2 = 2
} amp_instance_t;

typedef enum {
	src_none = 0,
	src_analog,
	src_dante,
	src_pinknoise,
	src_sine,
} oly_audio_source_t;

typedef union {
	struct {
		uint32_t	 		na;	// Needed to align PID with index, also used to detect empty or incompatible flash
		/* User Settings */
		uint32_t		 	Active_User;
		uint32_t		 	Active_Profile;
		uint32_t		 	Active_Throw;
		uint32_t		 	Active_Voicing;
		uint32_t		 	Active_FIR;
		uint32_t		 	Active_Xover;
		int32				Logo_Mode;
		int32	 			Display_Mode;
		int32				Cardioid_Index;
		int32				Input_Mode;
		uint32_t			LCD_Brightness;

		/* Calibration Settings */
		float32				Fan_Threshold;
		float32				Fan_Hysteresis;
		float32				cs_Z_limit_low_ch1;
		float32				cs_Z_limit_high_ch1;
		float32				cs_Z_limit_low_ch2;
		float32				cs_Z_limit_high_ch2;
		float32				cs_Z_limit_low_ch3;
		float32				cs_Z_limit_high_ch3;
		float32				cs_Z_limit_low_ch4;
		float32				cs_Z_limit_high_ch4;

		/* Self-Test Options */
		bool32				LF_Mute;
		bool32				MF_Mute;
		bool32				HF_Mute;
		bool32				LF_Test;
		bool32				MF_Test;
		bool32				HF_Test;
		bool32				LF_Solo;
		bool32				MF_Solo;
		bool32				HF_Solo;

		/* System Metadata */
		uint32_t			RigID;
		uint32_t			GroupID;
		uint32_t			ArrayID;
		float32				Array_Height_m;
		float32				Array_Dist_Front_m;
		float32				Array_Dist_Rear_m;
		uint32_t			Array_Index_Last;
		uint32_t			Array_Size_Last;
		bool32				Array_Was_Optimized;
		bool32				Use_Metric;
		int32				Lockout;
	};
	uint32_t 		 Values[ePID_OLYspeaker1_DEVICE_FENCE];
}	device_params_t;

typedef union {
	struct {
		uint32_t			Online;			// Dummy param for PID alignment and test
		int32				Angle;
		float32				Amp1_Temp;
		float32				Amp2_Temp;
		bool32				Amp_Fault;
		bool32				Limiting;
		int32				Identify;
		bool32				Fan_Enabled;
		int32				SelfTest_Mode;
		int32	 			SelfTest_Seq;
		int32				Logo_State;
		float32				cs_I_ch1;
		float32				cs_I_ch2;
		float32				cs_I_ch3;
		float32				cs_I_ch4;
		float32				cs_V_ch1;
		float32				cs_V_ch2;
		float32				cs_V_ch3;
		float32				cs_V_ch4;
		float32				cs_Z_ch1;
		float32				cs_Z_ch2;
		float32				cs_Z_ch3;
		float32				cs_Z_ch4;
		int32				cs_fault_ch1;
		int32				cs_fault_ch2;
		int32				cs_fault_ch3;
		int32				cs_fault_ch4;
		bool32				Array_Mismatch;
		uint32_t			Array_Size;
		uint32_t			Array_Index;
		uint32_t			Array_ID_Above;
		uint32_t			Array_ID_Below;
//		uint32_t			ActiveInputSource;
	};
	uint32_t 		 Values[ePID_OLYspeaker1_STATUS_FENCE];
} oly_status_t;

typedef struct {
//	OLY_target	Group;
	uint32_t	PID;
	uint8_t		enabled;
	uint32_t	period_ms;
	void * 		pValue;
} oly_param_subscription;


typedef struct {
	oly_version_t	Version;
	uint32_t		XML_Version;

	/* Device params */
	device_params_t	Device;

#if USER_PRESETS_STORED
	uint32_t		Stored_User_Presets[USER_PRESETS_STORED][ePID_OLYspeaker1_USER_FENCE];
#endif
#if PROFILE_PRESETS_STORED
	uint32_t		Stored_Profile_Presets[PROFILE_PRESETS_STORED][ePID_OLYspeaker1_PROFILE_FENCE];
#endif
#if THROW_PRESETS_STORED
	uint32_t		Stored_Throw_Presets[THROW_PRESETS_STORED][ePID_OLYspeaker1_THROW_FENCE];
#endif
#if XOVER_PRESETS_STORED
	uint32_t		Stored_Xover_Presets[XOVER_PRESETS_STORED][ePID_OLYspeaker1_XOVER_FENCE];
#endif
#if VOICING_PRESETS_STORED
	uint32_t		Stored_Voicing_Presets[VOICING_PRESETS_STORED][ePID_OLYspeaker1_VOICING_FENCE];
#endif
#if FIR_PRESETS_STORED
	uint32_t		Stored_FIR_Presets[FIR_PRESETS_STORED][ePID_OLYspeaker1_FIR_FENCE];
#endif
	uint32_t		Stored_Airloss[ePID_OLYspeaker1_AIRLOSS_FENCE];
	uint32_t		Stored_NoiseGate[ePID_OLYspeaker1_NOISEGATE_FENCE];
	/* String Parameters */
	char  			Stored_Labels[eLID_OLYspeaker1_DEVICE_FENCE][STRING_PARAMETER_LENGTH];

} oly_flash_params_t;

#ifndef _WIN32
typedef struct {
	/* Pointers to Parameter Preset values */
	device_params_t * Device;
	uint32_t *		  User[USER_PRESETS];
	uint32_t *		  Profile[PROFILE_PRESETS];
	uint32_t *		  Throw[THROW_PRESETS];
	uint32_t *		  Xover[XOVER_PRESETS];
	uint32_t *		  Voicing[VOICING_PRESETS];
	uint32_t *		  FIR[FIR_PRESETS];
	uint32_t *		  Airloss;
	uint32_t *        NoiseGate;
} oly_params_t;
#endif

#endif /* OLY_H_ */
