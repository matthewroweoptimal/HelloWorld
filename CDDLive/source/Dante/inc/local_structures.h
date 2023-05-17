/*
 * File     : local_structures.h
 * Created  : May 2014
 * Updated  : Date: 2014/05/16
 * Author   : Michael Ung <michael.ung@audinate.com>
 * Synopsis : All local DDP messages structure definitions.
 *
 * Copyright 2005-2014, Audinate Pty Ltd and/or its licensors
	Confidential
 * Audinate Copyright Header Version 1
 */
#ifndef _DDP_LOCAL_STRUCTURES_H
#define _DDP_LOCAL_STRUCTURES_H

#include "ddp.h"

/**
 * @struct ddp_local_audio_format_header
 * @brief Structure format for the "local audio format" message
 */
typedef struct ddp_local_audio_format_header
{
	ddp_message_header_t header;				/*!< Message header */
	ddp_local_subheader_t subheader;			/*!< Local event message subheader */
	struct
	{
		dante_samplerate_t current_samplerate;	/*!< Current sample rate */
		dante_samplerate_t reboot_samplerate;	/*!< Reboot sample rate */
	} payload; /*!< fixed payload */
} ddp_local_audio_format_header_t;

/**
 * @struct ddp_local_clock_pullup_header
 * @brief Structure format for the "local clock pullup" message
 */
typedef struct ddp_local_clock_pullup_header
{
	ddp_message_header_t header;			/*!< Message header */
	ddp_local_subheader_t subheader;		/*!< Local event message subheader */
	struct
	{
		uint16_t current_pullup;	/*!< Current pullup @see clock_pullup */
		uint16_t reboot_pullup;		/*!< Reboot pullup @see clock_pullup */
	} payload; /*!< fixed payload */
} ddp_local_clock_pullup_header_t;

#endif
