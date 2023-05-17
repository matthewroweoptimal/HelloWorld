/*
 * File     : clocking_client.h
 * Created  : August 2014
 * Updated  : Date: 2014/08/22
 * Author   : Michael Ung <michael.ung@audinate.com>
 * Synopsis : All clocking DDP client helper functions.
 *
 * Copyright 2005-2014, Audinate Pty Ltd and/or its licensors
	Confidential
 * Audinate Copyright Header Version 1
 */
#ifndef _DDP_CLOCKING_CLIENT_H
#define _DDP_CLOCKING_CLIENT_H

#include "packet.h"
#include "clocking_structures.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @file clocking_client.h
* @brief All clocking DDP client helper functions.
*/

/**
* \defgroup Clocking DDP Clocking
* @{
*/

/**
* \defgroup ClockingStructs DDP Clocking Structures
* @{
*/

/**
* @struct clock_config_control_fields
* @brief Structure used to set the clock config related information
*/
struct clock_config_control_fields
{
	uint16_t 	valid_flags;		/*!< Bitwise OR'd valid flags @see clock_config_control_flags, set this to zero to query the clock config related information */
	uint8_t 	preferred;			/*!< Preferred clock master state which needs to be applied */
	uint8_t 	word_clock_sync;	/*!< External word clock sycn state which needs to be applied */
	uint8_t 	logging;			/*!< Clock logging state which needs to be applied */
};

/** @cond */
typedef struct clock_config_control_fields clock_config_control_fields_t;
/** @endcond */

/**
* @struct clock_pullup_control_fields
* @brief Structure used to set clock pullup related information
*/
struct clock_pullup_control_fields
{
	uint16_t 			valid_flags;		/*!< Bitwise OR'd valid flags @see clock_pullup_control_flags, set this to zero to query the clock pullup related information */
	uint8_t 			pullup;				/*!< Pullup value which needs to be applied, valid only if CLOCK_PULLUP_SET_PULLUP is set in the valid_flags field */
	const uint8_t * 	subdomain;			/*!< Subdomain string. This value cannot be directly set in the ddp_add_clock_pullup_request function but can be used to read the subdomain string in the request message */
	uint16_t 			subdomain_length;	/*!< Length of the subdomain. This value cannot be directly set in the ddp_add_clock_pullup_request function but can be used to read the subdomain string length in the request message */
};

/** @cond */
typedef struct clock_pullup_control_fields clock_pullup_control_fields_t;
/** @endcond */

/**
* @struct clock_supported_pullup
* @brief Structure format of the supported pullups
*/
struct clock_supported_pullup
{
	uint8_t pullup;		/*!< A supported pullup value @see clock_pullup */
	uint8_t pad0;		/*!< Pad for alignment - must be '0' */
	uint16_t pad1;		/*!< Pad for alignment - must be '0' */
};

/** @cond */
typedef struct clock_supported_pullup clock_supported_pullup_t;
/** @endcond */

/**@}*/

/**
* \defgroup ClockFuncs DDP Clocking Functions
* @{
*/

/**
* Creates a DDP clock basic request message. This message can only be used to query the current state of the device. 
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param request_id [in] A non-zero sequence number which is populated in this message
* @return AUD_SUCCESS if the DDP clock basic request message was successfully created, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_add_clock_basic_request
(
	ddp_packet_write_info_t * packet_info,
	ddp_request_id_t request_id
);

/**
* Reads data fields from a DDP clock basic response message
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param offset [in] Starting point in the packet buffer to read this message
* @param out_request_id [out optional] Pointer to the sequence number for this request message
* @param out_status [out optional] Pointer to the status of this message
* @param out_capability_flags [out optional] Bitwise OR'd capability flags of this response @see clock_basic_capability_flags
* @param out_clock_state [out optional] Pointer to the clock state of the device
* @param out_mute_state [out optional] Pointer to the mute state of the device
* @param out_ext_wc_state [out optional] Pointer to the external mute state of the device
* @param out_preferred [out optional] Pointer to the preferred clock master state of the device
* @param out_drift [out optional] Pointer to the clock drift value of the device in PPM
* @return AUD_SUCCESS if the DDP clock basic response message was successfully read, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_read_clock_basic_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t * out_request_id,
	ddp_status_t * out_status,
	uint32_t* out_capability_flags,
	uint8_t* out_clock_state,
	uint8_t* out_mute_state,
	uint8_t* out_ext_wc_state,
	uint8_t* out_preferred,
	uint32_t* out_drift
);

/**
* Creates a DDP clock config request message. This message can be used as a command to change the state of the device and as a request to query the current state of the device.
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param request_id [in] A non-zero sequence number which is populated in this message
* @param control_fields [in] Clock config related information that needs to be applied on the device @see clock_config_control_fields
* @return AUD_SUCCESS if the DDP clock config request message was successfully created, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_add_clock_config_request
(
	ddp_packet_write_info_t * packet_info,
	ddp_request_id_t request_id,
	clock_config_control_fields_t * control_fields
);

/**
* Reads data fields from a DDP clock config response message
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param offset [in] Starting point in the packet buffer to read this message
* @param out_request_id [out optional] Pointer to the sequence number for this request message
* @param out_status [out optional] Pointer to the status of this message
* @param out_preferred [out optional] Pointer to the preferred master state of the device
* @param out_word_clock_sync [out optional] Pointer to the external word clock sync state of the device
* @param out_logging [out optional] Pointer to the clock logging state of the device
* @return AUD_SUCCESS if the DDP clock config response message was successfully read, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_read_clock_config_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t * out_request_id,
	ddp_status_t * out_status,
	uint8_t * out_preferred,
	uint8_t * out_word_clock_sync,
	uint8_t * out_logging
);

/**
* Creates a DDP clock pullup request message. This message can be used as a command to change the state of the device and as a request to query the current state of the device.
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param request_id [in] A non-zero sequence number which is populated in this message
* @param control_fields [in] The clock pullup related information that needs to be applied on the device @see clock_pullup_control_fields
* @return AUD_SUCCESS if the DDP clock pullup request message was successfully created, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_add_clock_pullup_request
(
	ddp_packet_write_info_t * packet_info,
	ddp_request_id_t request_id,
	clock_pullup_control_fields_t * control_fields
);

/**
* Reads data fields from a DDP clock pullup response message
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param offset [in] Starting point in the packet buffer to read this message
* @param out_request_id [out optional] Pointer to the sequence number for this request message
* @param out_status [out optional] Pointer to the status of this message
* @param out_current_pullup [out optional] Pointer to the current pullup value of the device @see clock_pullup
* @param out_reboot_pullup [out optional] Pointer to the pullup value used after the device reboots @see clock_pullup
* @param out_current_subdomain [out optional] Pointer to a pointer to the current subdomain string of the device
* @param out_current_subdomain_length [out optional] Pointer to the length of the current subdomain string 
* @param out_reboot_subdomain [out optional] Pointer to a pointer to the subdomain string used after the device reboots
* @param out_reboot_subdomain_length [out optional] Pointer to the length of the reboot subdomain string 
* @param out_supported_pullups [out optional] Pointer to a pointer to an array of structures which has information about the supported pullups of the device @see clock_supported_pullup
* @param out_num_supported_pullups [out optional] Pointer to the number of pullups supported by the device
* @return AUD_SUCCESS if the DDP clock pullup response message was successfully read, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_read_clock_pullup_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t * out_request_id,
	ddp_status_t * out_status,
	uint8_t * out_current_pullup,
	uint8_t * out_reboot_pullup,
	const uint8_t ** out_current_subdomain,
	uint16_t * out_current_subdomain_length,
	const uint8_t ** out_reboot_subdomain,
	uint16_t* out_reboot_subdomain_length,
	const clock_supported_pullup_t ** out_supported_pullups,
	uint16_t * out_num_supported_pullups
);

/**@}*/
/**@}*/

#ifdef __cplusplus
}
#endif


#endif // _DDP_CLOCKING_CLIENT_H
