/*
 * File     : audio_client.h
 * Created  : August 2014
 * Updated  : Date: 2014/08/22
 * Author   : Michael Ung <michael.ung@audinate.com>
 * Synopsis : All audio DDP client helper functions.
 *
 * Copyright 2005-2014, Audinate Pty Ltd and/or its licensors
	Confidential
 * Audinate Copyright Header Version 1
 */
#ifndef _DDP_AUDIO_CLIENT_H
#define _DDP_AUDIO_CLIENT_H

#include "packet.h"
#include "audio_structures.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @file audio_client.h
* @brief All audio DDP client helper functions.
*/

/**
 * \defgroup Audio DDP Audio
 * @{
 */

/**
 * \defgroup AudioFuncs DDP Audio Functions
 * @{
 */
 
/**
* Creates a DDP audio basic request message. This message can only be used to query the current state of the device.
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param request_id [in] A non-zero sequence number which is populated in this message
* @return AUD_SUCCESS if the DDP audio basic request message was successfully created, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_add_audio_basic_request
(
	ddp_packet_write_info_t * packet_info,
	ddp_request_id_t request_id
);

/**
* Reads data fields from a DDP audio basic response message
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param offset [in] Starting point in the packet buffer to read this message
* @param out_request_id [out optional] Pointer to the sequence number of this message
* @param out_status [out optional] Pointer to the status of this message
* @param out_capability_flags [out optional] Bitwise OR'd capability flags of this response @see audio_basic_capabilty_flags
* @param out_default_sample_rate [out optional] The default sample rate of the device 
* @param out_default_encoding [out optional] The default encoding of the device
* @param out_rx_chans [out optional] The number of receive audio channels of the device
* @param out_tx_chans [out optional] The number of transmit audio channels of the device
* @param out_change_flags [out optional] Bitwise OR'd change flags of this response @see audio_basic_change_flags
* @return AUD_SUCCESS if the DDP audio basic response message was successfully read, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_read_audio_basic_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t * out_request_id,
	uint16_t * out_status,
	uint32_t * out_capability_flags,
	dante_samplerate_t * out_default_sample_rate,
	uint16_t * out_default_encoding,
	uint16_t * out_rx_chans,
	uint16_t * out_tx_chans,
	uint8_t * out_change_flags
);

/**
* Creates a DDP audio sample rate config request message. This message can be used as a command to change the state of the device and as a request to query the current state of the device.
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param request_id [in] A non-zero sequence number which is populated in this message
* @param control_flags [in] Bitwise OR'd control flags @see audio_srate_config_control_flags, set to zero to query the current sample rate of the device
* @param sample_rate [in optional] The new sample rate which needs to be applied
* @return AUD_SUCCESS if a DDP audio sample rate config request message was successfully created, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_add_audio_sample_rate_config_request
(
	ddp_packet_write_info_t * packet_info,
	ddp_request_id_t request_id,
	uint16_t control_flags,
	dante_samplerate_t sample_rate
);

/**
* Reads data fields from a DDP audio sample rate config response message
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param offset [in] Starting point in the packet buffer to read this message
* @param out_message_info [in out] Pointer to a structure which contains information about the payload and heap for this DDP packet (memory should be allocated for this structure by the caller of this function)
* @param out_request_id [out optional] Pointer to the sequence number of this message
* @param out_status [out optional] Pointer to the status of this message
* @param out_current_samplerate [out optional] Pointer to the sample rate used by the device
* @param out_reboot_srate [out optional] Pointer to the sample rate used after a reboot by the device
* @param out_num_supported_srates [out optional] Pointer to the number of supported sample rates of the device
* @return AUD_SUCCESS if the DDP audio sample rate config response message was successfully read, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_read_audio_sample_rate_config_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_message_read_info_t * out_message_info,
	ddp_request_id_t * out_request_id,
	uint16_t * out_status,	
	dante_samplerate_t * out_current_samplerate,
	dante_samplerate_t * out_reboot_srate,
	uint16_t * out_num_supported_srates
);

/**
* Reads a supported sample rate field from a DDP audio sample rate config response message. N.B. This function should only be called after calling the ddp_read_audio_sample_rate_config_response function @see ddp_read_audio_sample_rate_config_response
* @param message_info [in] Pointer to a structure which has information about the DDP packet buffer, use the out_message_info parameter after calling the ddp_read_audio_sample_rate_config_response function
* @param srate_idx [in] Index into the supported sample rates block of information
* @param out_supported_srate [out optional] Pointer to a supported sample rate index by srate_idx
* @return AUD_SUCCESS if the DDP audio sample rate config response message was successfully read, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_read_audio_sample_rate_config_supported_srate
(
	const ddp_message_read_info_t * message_info,
	uint16_t srate_idx,
	dante_samplerate_t * out_supported_srate
);

/**
* Creates a DDP audio encoding config request message. This message can be used as a command to change the state of the device and as a request to query the current state of the device.
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param request_id [in] A non-zero sequence number which is populated in this message
* @param control_flags [in optional] Bitwise OR'd control flags @see audio_encoding_config_control_flags, use zero to query the current encoding of the device
* @param encoding [in optional] The encoding value which needs to be set @see audio_supported_encoding
* @return AUD_SUCCESS if a DDP audio encoding request message was successfully created, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_add_audio_encoding_config_request
(
	ddp_packet_write_info_t * packet_info,
	ddp_request_id_t request_id,
	uint16_t control_flags,
	uint16_t encoding
);

/**
* Reads data fields from a DDP audio encoding config response message
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param offset [in] Starting point in the packet buffer to read this message
* @param out_message_info [in out] Pointer to a structure which contains information about the payload and heap for this DDP packet (memory should be allocated for this structure by the caller of this function)
* @param out_request_id [out optional] Pointer to the sequence number of this response
* @param out_status [out optional] Pointer to the status of this response
* @param out_current_encoding [out optional] Pointer to the encoding used by the device
* @param out_reboot_encoding [out optional] Pointer to the encoding used by the device after a reboot
* @param out_num_supported_encodings [out optional] Pointer to the number of supported encodings used by the device
* @return AUD_SUCCESS if a DDP audio encoding config response message was successfully created, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_read_audio_encoding_config_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_message_read_info_t * out_message_info,
	ddp_request_id_t * out_request_id,
	uint16_t * out_status,
	uint16_t * out_current_encoding,
	uint16_t * out_reboot_encoding,
	uint16_t * out_num_supported_encodings
);


/**
* Reads a supported encoding field from a DDP audio encoding config response message. N.B. This function should only be called after calling the ddp_read_audio_encoding_config_response function @see ddp_read_audio_encoding_config_response
* @param message_info [in] Pointer to a structure which has information about the DDP packet buffer, use the out_message_info parameter after calling the ddp_read_audio_encoding_config_response function
* @param enc_idx [in] Index into the supported sample rates block of information
* @param out_supported_enc [out optional] Pointer to a supported encoding index by enc_idx
* @return AUD_SUCCESS if the DDP audio encoding config response message was successfully read, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_read_audio_encoding_config_supported_encoding
(
	const ddp_message_read_info_t * message_info,
	uint16_t enc_idx,
	uint16_t * out_supported_enc
);

/**@}*/
/**@}*/

#ifdef __cplusplus
}
#endif

#endif // _DDP_AUDIO_CLIENT_H
