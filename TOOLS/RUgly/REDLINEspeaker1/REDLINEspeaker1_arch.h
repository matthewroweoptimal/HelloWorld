// DO NOT EDIT THIS FILE
// Created by xmlerator on Friday, 05 June 2015 at 11:36:56 AM.
#define REDLINEspeaker1_XML_VERSION 7

#ifndef __REDLINEspeaker1_ARCH_H__
#define __REDLINEspeaker1_ARCH_H__

typedef enum {
	eTYPE_INVALID = -1,
	eTYPE_NULL,
	eTYPE_INPUT,
	eTYPE_HF,
	eTYPE_MF,
	eTYPE_FENCE
} REDLINEspeaker1_type;

typedef enum {
	eEFFECT_INVALID = -1,
	eEFFECT_NULL,
	eEFFECT_HPF,
	eEFFECT_LPF,
	eEFFECT_PEQ,
	eEFFECT_SOFTLIMIT,
	eEFFECT_PEQ_POST,
	eEFFECT_DELAY,
	eEFFECT_DYNEQ,
	eEFFECT_FIR,
	eEFFECT_FENCE
} REDLINEspeaker1_effect;

typedef enum {
	eQUAL_INVALID = -1,
	eQUAL_NULL,
	eQUAL_BAND,
	eQUAL_COEF,
	eQUAL_FENCE
} REDLINEspeaker1_qualifier;

typedef enum {
	ePARAM_INVALID = -1,
	ePARAM_NULL,
	ePARAM_ENABLE,
	ePARAM_FREQ,
	ePARAM_SLOPE,
	ePARAM_RESPONSE,
	ePARAM_TYPE,
	ePARAM_GAIN,
	ePARAM_Q,
	ePARAM_FADER,
	ePARAM_MUTE,
	ePARAM_INVERT,
	ePARAM_THRESH,
	ePARAM_ATTACK,
	ePARAM_RELEASE,
	ePARAM_RATIO,
	ePARAM_KNEE,
	ePARAM_TIME,
	ePARAM_DETECTEN,
	ePARAM_DETECTTYPE,
	ePARAM_DETECTGAIN,
	ePARAM_DETECTFREQ,
	ePARAM_DETECTQ,
	ePARAM_AGCENABLE,
	ePARAM_AGCATTACK,
	ePARAM_AGCDECAY,
	ePARAM_AGCACTIVE,
	ePARAM_AGCTHRESH,
	ePARAM_AGCRATIO,
	ePARAM_RESUMEN,
	ePARAM_RESUMTYPE,
	ePARAM_RESUMGAIN,
	ePARAM_RESUMFREQ,
	ePARAM_RESUMQ,
	ePARAM_FADER_POST,
	ePARAM_MUTE_POST,
	ePARAM_VALUE,
	ePARAM_FENCE
} REDLINEspeaker1_parameter;

#endif	// __REDLINEspeaker1_ARCH_H__