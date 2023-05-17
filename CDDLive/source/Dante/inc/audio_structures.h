/*
 * File     : audio_structures.h
 * Created  : May 2014
 * Updated  : Date: 2014/05/16
 * Author   : Michael Ung <michael.ung@audinate.com>
 * Synopsis : All audio ddp messages structure definitions.
 *
 * Copyright 2005-2014, Audinate Pty Ltd and/or its licensors
	Confidential
 * Audinate Copyright Header Version 1
 */
#ifndef _DDP_AUDIO_STRUCTURES_H
#define _DDP_AUDIO_STRUCTURES_H

#include "ddp.h"

/**
* @file audio_structures.h
* @brief All audio DDP messages structure definitions.
*/

 /** \addtogroup Audio
 *  @{
 */

/**
 * \defgroup AudioEnums DDP Audio Enums
 * @{
 */

/**
* @enum audio_basic_capabilty_flags 
* @brief Audio basic capability_flags values for ddp_audio_basic_response_t
*/
enum audio_basic_capabilty_flags
{
	CAPABILITY_FLAG_SUPPORT_MULTIPLE_SAMPLE_RATES 	= 0x00000001, /*!< Supports multiple sample rates */
	CAPABILITY_FLAG_SUPPORT_MULTIPLE_ENCODINGS		= 0x00000002, /*!< Supports multiple encodings */
	CAPABILITY_FLAG_AUDIO_MUTE_ON_ERROR				= 0x00000004, /*!< Audio mute on error enabled  */
};

/**
* @enum audio_basic_change_flags 
* @brief Audio basic change_flag values for ddp_audio_basic_response_t
*/
enum audio_basic_change_flags
{
	CHANGE_FLAG_SUPPORT_DYNAMIC_SAMPLE_RATE_CHANGE	= 0x01, 	/*!< Supports dynamic sample rate change */
	CHANGE_FLAG_SUPPORT_DYNAMIC_ENCODING_CHANGE		= 0x02, 	/*!< Supports dynamic encoding change */
};

/**
* @enum audio_srate_config_control_flags
* @brief Audio sample rate config control_flags for ddp_audio_srate_config_request_t
*/
enum audio_srate_config_control_flags
{
	SET_SRATE_FLAGS = 0x0001	/*!< Set or change the sample rate */
};

/**
* @enum audio_srate_config_valid_flags
* @brief Audio sample rate config valid_flags for ddp_audio_srate_config_response_t
*/
enum audio_srate_config_valid_flags
{
	CURR_SAMPLE_RATE_VALID_FLAGS = 0x0001,	/*!< current_samplerate field is valid */
	REBOOT_SAMPLE_RATE_VALID_FLAGS = 0x0002,	/*!< reboot_samplerate field is valid */
	SUPPORTED_SAMPLE_RATES_VALID_FLAGS = 0x0004		/*!< num_supported_srates, supported_srates_size, supported_srates_offset field is valid */
};

/**
* @enum audio_encoding_config_control_flags
* @brief Audio encoding config control_flags for ddp_audio_enc_config_request_payload_t
*/
enum audio_encoding_config_control_flags
{
	AUDIO_ENC_CONFIG__SET_ENCODING_FLAGS = 0x0001	/*!<  Set or change the audio encoding */
};

/**
* @enum audio_encoding_config_valid_flags
* @brief Audio encoding config control_flags for ddp_audio_enc_config_request_payload_t
*/
enum audio_encoding_config_valid_flags
{
	AUDIO_ENC_CONFIG__CURRENT_ENC_VALID = 0x0001,	/*!< current_encoding is valid */
	AUDIO_ENC_CONFIG__REBOOT_ENC_VALID = 0x0002,	/*!< reboot_encoding is valid */
	AUDIO_ENC_CONFIG__SUPPORTED_ENC_VALID = 0x0004	/*!< num_supported_encs, supported_encs_size, and supported_encs_offset is valid */
};

/**
* @enum audio_supported_encoding
* @brief Audio encoding config control_flags for ddp_audio_enc_config_request_payload_t
*/
enum audio_supported_encoding
{
	PCM16_SUPPORTED_ENCODING = 16,	/*!< PCM16 encoding */
	PCM24_SUPPORTED_ENCODING = 24,	/*!< PCM24 encoding */
	PCM32_SUPPORTED_ENCODING = 32	/*!< PCM32 encoding */
};

/**@}*/

/**
* \defgroup AudioStructs DDP Audio Structures
* @{
*/

/**
* @struct ddp_audio_basic_request
* @brief Structure format for the "audio basic request" message
*/
struct ddp_audio_basic_request
{
	ddp_message_header_t header;			/*!< message header */
	ddp_request_subheader_t subheader;		/*!< request message subheader */
};

/**
* @struct ddp_audio_basic_response
* @brief Structure format for the "audio basic response" message
*/
struct ddp_audio_basic_response
{
	ddp_message_header_t header;					/*!< message header */
	ddp_response_subheader_t subheader;				/*!< response message subheader */

	struct
	{
		uint32_t capability_flags;					/*!< Capability flags @see audio_basic_capabilty_flags */
		dante_samplerate_t default_sample_rate;		/*!< Default sample rate */
		uint16_t default_encoding;					/*!< Default audio encoding */
		uint16_t rx_chans;							/*!< Number of receive channels */
		uint16_t tx_chans;							/*!< Number of transmit channels */
		uint8_t change_flags;						/*!< Change flags @see audio_basic_change_flags */
		uint8_t pad0;								/*!< Pad for alignment - must be '0' */
	} payload; /*!< fixed payload */
};

/**
* @struct ddp_audio_srate_config_request
* @brief Structure format for the "audio sample rate config request" message
*/
struct ddp_audio_srate_config_request
{
	ddp_message_header_t header;			/*!< message header */
	ddp_request_subheader_t subheader;		/*!< request message subheader */

	struct
	{
		uint16_t control_flags;				/*!< Control flags for this message @see audio_srate_config_control_flags */
		uint16_t pad1;						/*!< Pad for alignment - must be '0' */
		dante_samplerate_t sample_rate;		/*!< Sample rate set on the device, only valid if the SET_SRATE_FLAGS is set in the control_flags field */
	} payload; /*!< fixed payload */
};

/**
* @struct ddp_audio_srate_config_response
* @brief Structure format for the "audio sample rate config response" message
*/
struct ddp_audio_srate_config_response
{
	ddp_message_header_t header;				/*!< message header */
	ddp_response_subheader_t subheader;			/*!< response message subheader */

	struct
	{
		uint16_t valid_flags;						/*!< Valid flags - which fields in this message is valid @see audio_srate_config_valid_flags */
		uint16_t pad0;								/*!< Pad for alignment - must be '0' */
		dante_samplerate_t current_samplerate;		/*!< Current sample rate, only valid if CURR_SAMPLE_RATE_VALID_FLAGS is set in the valid_flags */
		dante_samplerate_t reboot_samplerate;		/*!< Reboot sample rate, only valid if REBOOT_SAMPLE_RATE_VALID_FLAGS is set in the valid_flags */
		uint16_t num_supported_srates;				/*!< Number of supported sample rates, only valid if SUPPORTED_SAMPLE_RATES_VALID_FLAGS is set in the valid_flags */
		uint16_t supported_srates_size;				/*!< Size of each sample rate in the supported sample rate arary, only valid if SUPPORTED_SAMPLE_RATES_VALID_FLAGS is set in the valid_flags */
		ddp_raw_offset_t supported_srates_offset;	/*!< Offset to the supported sample rate arary, only valid if SUPPORTED_SAMPLE_RATES_VALID_FLAGS is set in the valid_flags */
		uint16_t pad1;								/*!< Pad for alignment - must be '0' */
	} payload; /*!< fixed payload */

	// Heap goes here
};

/**
* @struct ddp_audio_encoding_config_request
* @brief Structure format for the "audio encoding config response" message
*/
struct ddp_audio_encoding_config_request
{
	ddp_message_header_t header;			/*!< message header */
	ddp_request_subheader_t subheader;		/*!< request message subheader */

	struct
	{
		uint16_t control_flags;				/*!< Control flags for this message @see audio_encoding_config_control_flags */
		uint16_t encoding;					/*!< Audio encoding set set on the device, only valid if the AUDIO_ENC_CONFIG__SET_ENCODING_FLAGS is set in the control_flags field */
	} payload; /*!< fixed payload */
};

/**
* @struct ddp_supported_enc_array
* @brief Structure format for a supported encoding element for the "audio encoding config response" message
*/
struct ddp_supported_enc_array
{
	uint16_t encoding;	/*!< Supported encoding value @see audio_supported_encoding */
	uint16_t pad0;		/*!< Pad for alignment - must be '0' */
};

/**
* @struct ddp_custom_enc_array
* @brief Structure format for a custom non-PCM encoding
*/
struct ddp_custom_enc_array
{
	uint16_t custom_encoding;		/*!< Custom non-PCM encoding value */
	uint16_t pad0;					/*!< Pad for alignment - must be '0' */
};

/**
* @struct ddp_audio_encoding_config_response
* @brief Structure format for the "audio encoding config response" message
*/
struct ddp_audio_encoding_config_response
{
	ddp_message_header_t header;			/*!< message header */
	ddp_response_subheader_t subheader;		/*!< response message subheader */

	struct
	{
		uint16_t valid_flags;						/*!< Valid flags - which fields in this message are valid @see audio_encoding_config_valid_flags */
		uint16_t current_encoding;					/*!< Current encoding used by the device, only valid if the AUDIO_ENC_CONFIG__CURRENT_ENC_VALID is set in the valid_flags field */
		uint16_t reboot_encoding;					/*!< Encoding used by the device after a reboot, only valid if the AUDIO_ENC_CONFIG__REBOOT_ENC_VALID is set in the valid_flags field */
		uint16_t num_supported_encs;				/*!< Number of supported encodings  @see ddp_supported_enc_array, only valid if the AUDIO_ENC_CONFIG__SUPPORTED_ENC_VALID is set in the valid_flags field */
		uint16_t supported_encs_size;				/*!< Size of each supported encodings field @see ddp_supported_enc_array, only valid if the AUDIO_ENC_CONFIG__SUPPORTED_ENC_VALID is set in the valid_flags field */
		ddp_raw_offset_t supported_encs_offset;		/*!< Offset from the start of the TLV0 header to the supported encodings array, only valid if the AUDIO_ENC_CONFIG__SUPPORTED_ENC_VALID is set in the valid_flags field */
	} payload; /*!< fixed payload */

	// Heap goes here
};

//! @cond Doxygen_Suppress
typedef struct ddp_audio_basic_request ddp_audio_basic_request_t;
typedef struct ddp_audio_basic_response ddp_audio_basic_response_t;
typedef struct ddp_audio_srate_config_request ddp_audio_srate_config_request_t;
typedef struct ddp_audio_srate_config_response ddp_audio_srate_config_response_t;
typedef struct ddp_audio_encoding_config_request ddp_audio_encoding_config_request_t;
typedef struct ddp_supported_enc_array ddp_supported_enc_array_t;
typedef struct ddp_custom_enc_array ddp_custom_enc_array_t;
typedef struct ddp_audio_encoding_config_response ddp_audio_encoding_config_response_t;
//! @endcond

/**@}*/
/**@}*/


#endif // _DDP_AUDIO_STRUCTURES_H
