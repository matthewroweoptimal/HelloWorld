/*
 * File     : clocking_structures.h
 * Created  : May 2014
 * Updated  : Date: 2014/05/16
 * Author   : Michael Ung <michael.ung@audinate.com>
 * Synopsis : All clocking ddp messages structure definitions.
 *
 * Copyright 2005-2014, Audinate Pty Ltd and/or its licensors
	Confidential
 * Audinate Copyright Header Version 1
 */
#ifndef _DDP_CLOCKING_STRUCTURES_H
#define _DDP_CLOCKING_STRUCTURES_H

#include "ddp.h"

/**
* @file clocking_structures.h
* @brief All clocking ddp messages structure definitions.
*/

/** \addtogroup Clocking
*  @{
*/

/**
* \defgroup ClockingEnums DDP Clocking Enums
* @{
*/

/**
* @enum clock_basic_capability_flags
* @brief Clock basic capability_flags values for ddp_clock_basic_response_t
*/
enum clock_basic_capability_flags
{
	CAPABILITY_SUPPORT_PULLUP_SUBDOMAINS = 0x00000001,	/*!< Support for clock pullup/subdomains is available */
	CAPABILITY_SUPPORT_UNICAST_PTPV1 = 0x00000002,	/*!< Support for clock unicast PTP version 1 ports is available */
	CAPABILITY_SUPPORT_DYNAMIC_CLOCK_LOGGING = 0x00000004,	/*!< Support for dynamic clock logging is available */
	CAPABILITY_SUPPORT_EXTERNAL_WC = 0x00000008,	/*!< Support for using an external word clock is available */
	CAPABILITY_SLAVE_ONLY = 0x00000010,	/*!< Support for slave only operation is available */
};

/**
* @enum clock_state
* @brief Clock basic clock state values for ddp_clock_basic_response_t
*/
enum clock_state
{
	CLOCK_STATE_DISABLED = 0,	/*!< PTP clock is in a disabled state  */
	CLOCK_STATE_MASTER_INTERNAL_CLOCK = 1,	/*!< PTP clock is in master state and the device is using an internal clock source */
	CLOCK_STATE_MASTER_EXTERNAL_CLOCK = 2,	/*!< PTP clock is in master state and the device is using an external clock source */
	CLOCK_STATE_SLAVE_IN_SYNC = 3,	/*!< PTP clock is in slave state and it is in sync */
	CLOCK_STATE_SLAVE_NO_SYNC = 4,	/*!< PTP clock is in slave state and it is not in sync */
};

/**
* @enum clock_mute_state
* @brief Clock basic clock mute state values for ddp_clock_basic_response_t
*/
enum clock_mute_state
{
	MUTE_STATE_NOT_MUTED = 0,	/*!< PTP clocking is operating normally and audio is not muted */
	MUTE_STATE_MUTED_EXTERNAL_CLOCK = 1,	/*!< PTP clocking is not operating normally, the device is using an external clock source, and audio is muted */
	MUTE_STATE_MUTED_INTERNAL_CLOCK = 2,	/*!< PTP clocking is not operating normally, the device is using an internal clock source, and audio is muted */
	MUTE_STATE_MUTED_USER = 3,	/*!< user requested audio mute */
};

/**
* @enum clock_ext_wc_state
* @brief Clock basic external word clock values for ddp_clock_basic_response_t
*/
enum clock_ext_wc_state
{
	EXT_WC_STATE_NOT_SUPPORTED = 0,	/*!< An external word clocksource is not supported */
	EXT_WC_STATE_UNKNOWN = 1,	/*!< External word clock is in an unknown state */
	EXT_WC_STATE_NONE = 2,	/*!< External word clock state is none */
	EXT_WC_STATE_INVALID = 3,	/*!< External word clock state is invalid */
	EXT_WC_STATE_VALID = 4,	/*!< External word clock state is valid */
	EXT_WC_STATE_MISSING = 5,	/*!< External word clock state is missing */
};

/**
* @enum clock_preferred
* @brief Clock basic preferred state values for ddp_clock_basic_response_t
*/
enum clock_preferred
{
	CLOCK_NOT_PREFERRED = 0,	/*!< PTP clock is not set to preferred master */
	CLOCK_PREFERRED = 1,	/*!< PTP clock is set to preferred master */
};

/**
* @enum clock_config_control_flags
* @brief Clock config control_flags values for ddp_clock_config_request_t
*/
enum clock_config_control_flags
{
	CONTROL_FLAG_SET_PREFERRED = 0x0001,	/*!< Set device to be a preferred master */
	CONTROL_FLAG_SET_SLAVE_TO_WC = 0x0002,	/*!< Set slave device to use an external word clock */
	CONTROL_FLAGS_SET_LOGGING = 0x0004	/*!< Enable PTP logging */
};

/**
* @enum clock_ext_wc_sync
* @brief Clock config ext_word_clock_sync values for ddp_clock_config_request_t
*/
enum clock_ext_wc_sync
{
	EXT_WC_SYNC_DISABLED = 0,	/*!< External word clock syncing disabled */
	EXT_WC_SYNC_ENABLED = 1,	/*!< External word clock syncing enabled */
};

/**
* @enum clock_logging
* @brief Clock config logging values for ddp_clock_config_request_t
*/
enum clock_logging
{
	LOGGING_DISABLED = 0,	/*!< PTP logging disabled */
	LOGGING_ENABLED = 1,	/*!< PTP logging enabled */
};

/**
* @enum clock_config_response_valid_flags
* @brief Clock config valid_flags values for ddp_clock_config_response
*/
enum clock_config_response_valid_flags
{
	CLOCK_CONFIG_RSP_PREFERRED_VALID = 0x0001,	/*!< preferred field is valid */
	CLOCK_CONFIG_RSP_WC_SYNC_VALID = 0x0002,	/*!< word_clock_sync is valid */
	CLOCK_CONFIG_RSP_LOGGING_VALID = 0x0004	/*!< logging is valid */
};

/**
* @enum clock_pullup_control_flags
* @brief Clock pullup control_flags values for ddp_clock_pullup_request_t
*/
enum clock_pullup_control_flags
{
	CLOCK_PULLUP_SET_PULLUP = 0x0001,		/*!< Set a pullup */
	CLOCK_PULLUP_SET_SUBDOMAIN = 0x0002		/*!< Set the subdomain */
};

/**
* @enum clock_pullup
* @brief Clock pullup values for ddp_clock_pullup_request_t
*/
enum clock_pullup
{
	CLOCK_PULLUP_NO_PULLUP = 0,		/*!< No pullup value */
	CLOCK_PULLUP_POS_4_1667_PCENT,	/*!< Pullup of +4.1667% */
	CLOCK_PULLUP_POS_0_1_PCENT,		/*!< Pullup of +0.1% */
	CLOCK_PULLUP_NEG_0_1_PCENT,		/*!< Pullup of -0.1% */
	CLOCK_PULLUP_NEG_4_0_PCENT4		/*!< Pullup of -4.0% */
};

/**
* @enum clock_pullup_valid_flags
* @brief Clock pullup valid_flags values for ddp_clock_pullup_response_t
*/
enum clock_pullup_valid_flags
{
	CLOCK_PULLUP_CURR_PULLUP_VALID = 0x0001,	/*!< current_pullup field is valid */
	CLOCK_PULLUP_REBOOT_PULLUP_VALID = 0x0002,	/*!< reboot_pullup field is valid */
	CLOCK_PULLUP_SUPPORTED_PULLUPS_VALID = 0x0004,	/*!< num_supported_pullups, supported_pullups_size, and supported_pullups_offset fields are valid */
	CLOCK_PULLUP_CURR_SUBDOMAIN_VALID = 0x0008,	/*!< current_subdomain_offset field is valid */
	CLOCK_PULLUP_REBOOT_SUBDOMAIN_VALID = 0x0010	/*!< reboot_subdomain_offset field is valid */
};

/**@}*/

/**
* \defgroup ClockingStructs DDP Clocking Structures
* @{
*/

/**
* @struct ddp_clock_basic_request
* @brief Structure format for the "clock basic request" message
*/
struct ddp_clock_basic_request
{
	ddp_message_header_t header;		/*!< message header */
	ddp_request_subheader_t subheader;	/*!< request message subheader */	
};

/**
* @struct ddp_clock_basic_response
* @brief Structure format for the "clock basic response" message
*/
struct ddp_clock_basic_response
{
	ddp_message_header_t header;			/*!< message header */
	ddp_response_subheader_t subheader;		/*!< response message subheader */

	struct
	{
		uint32_t capability_flags;			/*!< Capability flags @see clock_basic_capability_flags */
		uint8_t clock_state;				/*!< Current PTP clock state @see clock_state */
		uint8_t mute_state;					/*!< Current mute state of audio which is related to the PTP clocking @see clock_mute_state */
		uint8_t ext_wc_state;				/*!< Current external word clock state @see clock_ext_wc_state */
		uint8_t preferred;					/*!< Current PTP clocking master preferred setting @see clock_preferred */
		uint32_t drift;						/*!< Current clock drift in parts per million (PPM) */
	} payload;	/*!< fixed payload */
};

/**
* @struct ddp_clock_config_request
* @brief Structure format for the "clock config request" message
*/
struct ddp_clock_config_request
{
	ddp_message_header_t header;			/*!< message header */
	ddp_request_subheader_t subheader;		/*!< request message subheader */

	struct
	{
		uint16_t control_flags;				/*!< Control flags @see clock_config_control_flags */
		uint8_t preferred;					/*!< Preferred PTP clock master setting @see clock_preferred */
		uint8_t ext_word_clock_sync;		/*!< External word clock sync setting @see clock_ext_wc_sync */
		uint8_t logging;					/*!< PTP logging setting @see clock_logging */
		uint8_t pad1;						/*!< Pad for alignment - must be '0' */
		uint16_t pad2;						/*!< Pad for alignment - must be '0' */
	} payload;	/*!< fixed payload */
};

/**
* @struct ddp_clock_config_response
* @brief Structure format for the "clock config response" message
*/
struct ddp_clock_config_response
{
	ddp_message_header_t header;			/*!< message header */
	ddp_response_subheader_t subheader;		/*!< response message subheader */

	struct
	{
		uint16_t valid_flags;				/*!< Valid flags - which fields in this message is valid @see clock_config_response_valid_flags */
		uint8_t preferred;					/*!< Preferred PTP clock master setting, valid only if CLOCK_CONFIG_RSP_PREFERRED_VALID is set in the valid_flags */
		uint8_t word_clock_sync;			/*!< Word clock sync setting, valid only if CLOCK_CONFIG_RSP_WC_SYNC_VALID is set in the valid_flags */
		uint8_t logging;					/*!< PTP logging setting, valid only if CLOCK_CONFIG_RSP_LOGGING_VALID is set in the valid_flags */
		uint8_t pad1;						/*!< Pad for alignment - must be '0' */
		uint16_t pad2;						/*!< Pad for alignment - must be '0' */
	} payload;	/*!< fixed payload */
};

/**
* @struct ddp_clock_pullup_request
* @brief Structure format for the "clock pullup request" message
*/
struct ddp_clock_pullup_request
{
	ddp_message_header_t header;		/*!< message header */
	ddp_request_subheader_t subheader;	/*!< request message subheader */

	struct
	{
		uint16_t control_flags;					/*!< Control flags @see clock_pullup_control_flags */
		uint8_t pullup;							/*!< Pullup value which is to be applied @see clock_pullup */
		uint8_t pad1;							/*!< Pad for alignment - must be '0' */
		ddp_raw_offset_t subdomain_offset;		/*!< Offset from the start of the TLV0 header to the first subdomain field */
		uint16_t subdomain_size;				/*!< Size of the subdomain field */
	} payload;	/*!< fixed payload */

	// Heap goes here
};

/**
* @struct ddp_clock_pullup_response
* @brief Structure format for the "clock pullup response" message
*/
struct ddp_clock_pullup_response
{
	ddp_message_header_t header;			/*!< message header */
	ddp_response_subheader_t subheader;		/*!< response message subheader */

	struct
	{
		uint16_t valid_flags;						/*!< Valid flags - which fields in this message is valid @see clock_pullup_valid_flags */
		uint8_t current_pullup;						/*!< Currently used pullup, valid only if CLOCK_PULLUP_CURR_PULLUP_VALID is set in the valid_flags */
		uint8_t reboot_pullup;						/*!< Pullup used after reboot, valid only if CLOCK_PULLUP_REBOOT_PULLUP_VALID is set in the valid_flags */
		ddp_raw_offset_t current_subdomain_offset;	/*!< Offset from the start of the TLV0 header to the first current subdomain field, valid only if CLOCK_PULLUP_CURR_SUBDOMAIN_VALID is set in the valid_flags */
		ddp_raw_offset_t reboot_subdomain_offset;	/*!< Offset from the start of the TLV0 header to the first reboot subdomain field, valid only if CLOCK_PULLUP_REBOOT_SUBDOMAIN_VALID is set in the valid_flags */
		uint16_t current_subdomain_size;			/*!< Size of each current subdomain field, valid only if CLOCK_PULLUP_CURR_SUBDOMAIN_VALID is set in the valid_flags */
		uint16_t reboot_subdomain_size;				/*!< Size of each reboot subdomain field, valid only if CLOCK_PULLUP_REBOOT_SUBDOMAIN_VALID is set in the valid_flags */
		uint16_t num_supported_pullups;				/*!< Number of supported pullups, valid only if CLOCK_PULLUP_SUPPORTED_PULLUPS_VALID is set in the valid_flags */
		uint16_t supported_pullups_size;			/*!< Size of each supported pullup field size, valid only if CLOCK_PULLUP_SUPPORTED_PULLUPS_VALID is set in the valid_flags */
		ddp_raw_offset_t supported_pullups_offset;	/*!< Offset from the start of the TLV0 header to the first supported pullup field */
		uint16_t pad0;								/*!< Pad for alignment - must be '0' */
	} payload;	/*!< fixed payload */

	// Heap goes here
};

//! @cond Doxygen_Suppress
typedef struct ddp_clock_basic_request ddp_clock_basic_request_t;
typedef struct ddp_clock_basic_response ddp_clock_basic_response_t;
typedef struct ddp_clock_config_request ddp_clock_config_request_t;
typedef struct ddp_clock_config_response ddp_clock_config_response_t;
typedef struct ddp_clock_pullup_request ddp_clock_pullup_request_t;
typedef struct ddp_clock_pullup_response ddp_clock_pullup_response_t;
//! @endcond

/**@}*/
/**@}*/

#endif // _DDP_CLOCKING_STRUCTURES_H
