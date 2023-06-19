// DO NOT EDIT THIS FILE
// Created by xmlerator on Friday, 12 December 2014 at 06:05:53 PM.
#define OLYmixer1_XML_VERSION 7

#ifndef __OLYmixer1_ARCH_H__
#define __OLYmixer1_ARCH_H__

typedef enum {
	eTYPE_INVALID = -1,
	eTYPE_NULL,
	eTYPE_INPUT,
	eTYPE_FXREVERB,
	eTYPE_FXDELAY,
	eTYPE_AUX,
	eTYPE_FXSEND,
	eTYPE_SOLO,
	eTYPE_MAINOUT,
	eTYPE_RECORDBUS,
	eTYPE_FENCE
} OLYmixer1_type;

typedef enum {
	eEFFECT_INVALID = -1,
	eEFFECT_NULL,
	eEFFECT_RTA,
	eEFFECT_HPF,
	eEFFECT_GATE,
	eEFFECT_COMP,
	eEFFECT_PEQ,
	eEFFECT_AUXSEND,
	eEFFECT_FXSEND,
	eEFFECT_REVERB,
	eEFFECT_DELAY,
	eEFFECT_GEQ,
	eEFFECT_LPF,
	eEFFECT_FENCE
} OLYmixer1_effect;

typedef enum {
	eQUAL_INVALID = -1,
	eQUAL_NULL,
	eQUAL_HIGH,
	eQUAL_HIMID,
	eQUAL_LOMID,
	eQUAL_LOW,
	eQUAL_BAND,
	eQUAL_FENCE
} OLYmixer1_qualifier;

typedef enum {
	ePARAM_INVALID = -1,
	ePARAM_NULL,
	ePARAM_INXSOLO,
	ePARAM_SOLO,
	ePARAM_ENABLE,
	ePARAM_INPUT,
	ePARAM_FADER,
	ePARAM_LAST_SNAPSHOT,
	ePARAM_SNAPSHOT_DIRTY,
	ePARAM_NAME,
	ePARAM_SAFE,
	ePARAM_CRC,
	ePARAM_NAMECRC,
	ePARAM_IMAGECRC,
	ePARAM_IMAGES,
	ePARAM_IMAGESCRC,
	ePARAM_SNAPCRC,
	ePARAM_SNAPS,
	ePARAM_NAMESCRC,
	ePARAM_SNAPSCRC,
	ePARAM_SHOWCRC,
	ePARAM_NOTESCRC,
	ePARAM_ROUTING,
	ePARAM_TRIM,
	ePARAM_THUMB,
	ePARAM_COLOR,
	ePARAM_INVERT,
	ePARAM_FREQ,
	ePARAM_SLOPE,
	ePARAM_MODEL,
	ePARAM_THRESH,
	ePARAM_ATTACK,
	ePARAM_RELEASE,
	ePARAM_RANGE,
	ePARAM_HOLD,
	ePARAM_GAIN,
	ePARAM_RATIO,
	ePARAM_KNEE,
	ePARAM_TYPE,
	ePARAM_Q,
	ePARAM_MUTE,
	ePARAM_PAN,
	ePARAM_LRASSIGN,
	ePARAM_STLINK,
	ePARAM_SOURCE,
	ePARAM_USELRMUTE,
	ePARAM_PREDELAY,
	ePARAM_DECAY,
	ePARAM_DIFFUSION,
	ePARAM_DAMPING,
	ePARAM_ROLLOFF,
	ePARAM_BAL,
	ePARAM_DELAY,
	ePARAM_DELAY2,
	ePARAM_FEEDBACK,
	ePARAM_RESPONSE,
	ePARAM_DISTANCE,
	ePARAM_FENCE
} OLYmixer1_parameter;

#endif	// __OLYmixer1_ARCH_H__