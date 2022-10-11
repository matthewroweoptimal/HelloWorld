/*
 * File     : device_structures.h
 * Created  : May 2014
 * Updated  : Date: 2014/05/16
 * Author   : Michael Ung <michael.ung@audinate.com>
 * Synopsis : All device ddp messages structure definitions.
 *
 * Copyright 2005-2014, Audinate Pty Ltd and/or its licensors
	Confidential
 * Audinate Copyright Header Version 1
 */
#ifndef _DDP_DEVICE_STRUCTURES_H
#define _DDP_DEVICE_STRUCTURES_H

#include "ddp.h"

/**
* @file device_structures.h
* @brief All device DDP messages structure definitions.
*/

/** \addtogroup Device
*  @{
*/

/**
* \defgroup DeviceEnums DDP Device Enums
* @{
*/

/**
* @enum device_general_capability_flags
* @brief device general capability flags
*/
enum device_general_capability_flags
{
	CAPABILITY_FLAG_SUPPORT_IDENTIFY = 0x00000001, /*!< Supports the identify message */
	CAPABILITY_FLAG_SUPPORT_DEVICE_MANUF_REQ = 0x00000002, /*!< Supports the device manufacturer request message */
	CAPABILITY_FLAG_SUPPORT_GPIO_CMD = 0x00000004, /*!< Support the GPIO command message */
	CAPABILITY_FLAG_SUPPORT_DEVICE_RESET_CMD = 0x00000008, /*!< Supports the device reset command message */
	CAPABILITY_FLAG_SUPPORT_DEVICE_SECURITY_CMD = 0x00000010, /*!< Supports the device security command message */
};

/**
* @enum device_general_status_flags
* @brief device general status flags
*/
enum device_general_status_flags
{
	STATUS_FLAG_CORRUPT_CAPABILITY = 0x00000001, /*!< Device has a corrupt capability */
	STATUS_FLAG_CORRUPT_USER_PARTITION = 0x00000002, /*!< Device has a corrupt user partition */
	STATUS_FLAG_CONFIG_STORE_ERROR = 0x00000004, /*!< Device has an error storing the Dante configuration */
};

/**
* @enum device_manuf_override_valid_flags
* @brief device manufacturer request override valid flags bitmask - which fields in the override_valid_flags of the ddp_device_manf_request message is valid
*/
enum device_manuf_override_valid_flags
{
	MANF_OVERRIDE_MANF_NAME_STRING_VALID	= 0x00000001, /*!< Overriding the manufacturer name is valid */
	MANF_OVERRIDE_MODEL_ID_STRING_VALID		= 0x00000002, /*!< Overriding the model ID string is valid */
	MANF_OVERRIDE_SOFTWARE_VER_VALID		= 0x00000004, /*!< Overriding the 32-bit manufacturer software version is valid */
	MANF_OVERRIDE_SOFTWARE_BUILD_VALID		= 0x00000008, /*!< Overriding the 32-bit manufacturer software build is valid */
	MANF_OVERRIDE_FIRMWARE_VER_VALID		= 0x00000010, /*!< Overriding the 32-bit manufacturer firmware version is valid */
	MANF_OVERRIDE_FIRMWARE_BUILD_VALID		= 0x00000020, /*!< Overriding the 32-bit manufacturer firmware build is valid */
	MANF_OVERRIDE_CAPABILITY_FLAGS_VALID	= 0x00000040, /*!< Overriding the 32-bit manufacturer capability flags is valid */
	MANF_OVERRIDE_MODEL_VER_VALID			= 0x00000080, /*!< Overriding the 32-bit manufacturer model version is valid */
	MANF_OVERRIDE_MODEL_VER_STRING_VALID	= 0x00000100  /*!< Overriding the manufacturer model version string is valid */
};

/**
* @enum device_upgrade_request_valid_flags
* @brief device upgrade request valid flags bitmask - which fields in the "device upgrade command/request" messages are valid
*/
enum device_upgrade_request_valid_flags
{
	UPGRADE_REQUEST_ACTIONS_VALID = 0x0001, /*!< The upgrade_actions field is valid */
	UPGRADE_REQUEST_FLAGS_VALID = 0x0002, /*!< The upgrade_flags field is valid */
	UPGRADE_REQUEST_PROTOCOL_TYPE_VALID = 0x0004, /*!< The protocol_type field is valid */
	UPGRADE_REQUEST_PROTOCOL_PARAM_OFFSET_VALID = 0x0008, /*!< The prot params offset field is valid */
	UPGRADE_REQUEST_MANF_OVERRIDE_VALID = 0x0010, /*!< The manf override field is valid */
	UPGRADE_REQUEST_MODEL_OVERRIDE_VALID = 0x0020, /*!< The model override field is valid */
};

/**
* @enum device_upgrade_action_flags
* @brief device upgrade action flags bitmask - which actions to trigger on the upgrade
*/
enum device_upgrade_action_flags
{
	UPGRADE_ACTION_DOWNLOAD = 0x0001, /*!< Download the upgrade */
	UPGRADE_ACTION_FLASH = 0x0002, /*!< Flash the upgrade */
};

/**
* @enum device_upgrade_protocol
* @brief device upgrade protocol type
*/
enum device_upgrade_protocol
{
	UPGRADE_PROTOCOL_FILE_LOCAL = 1, /*!< local file */
	UPGRADE_PROTOCOL_TFTP_GET, 		 /*!< tftp */
	UPGRADE_PROTOCOL_XMODEM, 		 /*!< x-modem */
};

/**
* @enum device_upgrade_tftp_prot_valid_flags
* @brief device upgrade tftp params valid flags bitmask - which fields in the tftp protocol parameters structure are valid
*/
enum device_upgrade_tftp_prot_valid_flags
{
	UPGRADE_TFTP_PROT_PARAM_FILENAME_VALID = 0x0001, /*!< The filename offset is valid */
	UPGRADE_TFTP_PROT_PARAM_FILE_LEN_VALID = 0x0002, /*!< The file length is valid */
	UPGRADE_TFTP_PROT_PARAM_FAMILY_VALID = 0x0004, /*!< The family is valid */
	UPGRADE_TFTP_PROT_PARAM_PORT_VALID = 0x0008, /*!< The port is valid */
	UPGRADE_TFTP_PROT_PARAM_IPADDR_VALID = 0x0010, /*!< The IPv4 address is valid */
};

/**
* @enum device_upgrade_xmodem_valid_flags
* @brief device upgrade xmodem params valid flags bitmask - which fields in the xmodem protocol parameters structure are valid
*/
enum device_upgrade_xmodem_valid_flags
{
	UPGRADE_XMODEM_PROT_PARAM_PORT_VALID = 0x0001, /*!< The port is valid */
	UPGRADE_XMODEM_PROT_PARAM_MASTER_CPOL_VALID = 0x0002, /*!< The SPI master clock polarity is valid */
	UPGRADE_XMODEM_PROT_PARAM_MASTER_CPHA_VALID = 0x0004, /*!< The SPI master clock phase is valid */
	UPGRADE_XMODEM_PROT_PARAM_SLAVE_CPOL_VALID = 0x0008, /*!< The SPI slave clock polarity is valid */
	UPGRADE_XMODEM_PROT_PARAM_SLAVE_CPHA_VALID = 0x0010, /*!< The SPI slave clock phase is valid */
	UPGRADE_XMODEM_PROT_PARAM_PARITY_VALID = 0x0020, /*!< The UART parity is valid */
	UPGRADE_XMODEM_PROT_PARAM_FLOW_CONTROL_VALID = 0x0040, /*!< The UART flow control is valid */
	UPGRADE_XMODEM_PROT_PARAM_BAUDRATE_VALID = 0x0080, /*!< The SPI/UART baudrate is valid */
	UPGRADE_XMODEM_PROT_PARAM_FILE_LEN_VALID = 0x0100, /*!< The file length is valid */
};

/**
* @enum device_upgrade_xmodem_port
* @brief device upgrade xmodem port
*/
enum device_upgrade_xmodem_port
{
	UPGRADE_XMODEM_PORT_UARTA = 1,	/*!< UART A */
	UPGRADE_XMODEM_PORT_UARTB = 2,	/*!< UART B */
	UPGRADE_XMODEM_PORT_SPI0 = 3,	/*!< SPI */
};

/**
* @enum ddp_spi_cpol
* @brief SPI master and slave polarity for ddp_device_upgrade_xmodem_protocol_params
*/
enum ddp_spi_cpol
{
	DDP_SPI_CPOL_IDLE_LOW = 1,	/*!< SPI Clock polarity idle low (active high) */
	DDP_SPI_CPOL_IDLE_HIGH = 2,	/*!< SPI Clock polarity idle high (active low) */
};

/**
* @enum ddp_spi_cpha
* @brief SPI master and slave phase for ddp_device_upgrade_xmodem_protocol_params
*/
enum ddp_spi_cpha
{
	DDP_SPI_CPHA_SAMPLE_LEADING_EDGE = 1,	/*!< SPI Clock phase - sample data leading edge, change data following edge */
	DDP_SPI_CPHA_SAMPLE_TRAILING_EDGE = 2,	/*!< SPI Clock phase - change data leading edge, sample data following edge */
};

/**
* @enum ddp_uart_parity
* @brief UART parity for ddp_device_upgrade_xmodem_protocol_params
*/
enum ddp_uart_parity
{
	DDP_UART_PARITY_NONE = 1,	/*!< UART parity - none / disabled */
	DDP_UART_PARITY_EVEN = 2,	/*!< UART parity even */
	DDP_UART_PARITY_ODD = 3,	/*!< UART parity odd */
};

/**
* @enum ddp_uart_flow_control
* @brief UART flow control for ddp_device_upgrade_xmodem_protocol_params
*/
enum ddp_uart_flow_control
{
	DDP_UART_FLOWCONTROL_NONE = 1,	/*!< UART flow control - none / disabled */
	DDP_UART_FLOWCONTROL_RTS_CTS = 2,	/*!< UART flow control RTS/CTS */
};

/**
* @enum device_upgrade_response_valid_flags
* @brief device upgrade response valid flags bitmask - which fields in the ddp_device_upgrade_response message are valid
*/
enum device_upgrade_response_valid_flags
{
	UPGRADE_RESPONSE_UPGRADE_ERROR_VALID = 0x0001, /*!< The upgrade error is valid */
	UPGRADE_RESPONSE_PROGRESS_CURRENT_VALID = 0x0002, /*!< The progress current is valid */
	UPGRADE_RESPONSE_PROGRESS_TOTAL_VALID = 0x0004, /*!< The progress total is valid */
	UPGRADE_RESPONSE_MANF_ID_VALID = 0x0008, /*!< The manufacturer id is valid */
	UPGRADE_RESPONSE_MODEL_ID_VALID = 0x0010, /*!< The model id is valid */
};

/**
* @enum device_upgrade_stage
* @brief device upgrade stage - current state of the upgrade
*/
enum device_upgrade_stage
{
	DEVICE_UPGRADE_STAGE_NONE = 0,	/*!< No ugprade actions are in progress */
	DEVICE_UPGRADE_STAGE_GET_FILE = 1,	/*!< Upgrade is is progress - downloading the file */
	DEVICE_UPGRADE_STAGE_SUCCESS_DONE = 2,	/*!< Upgrade has completed successfully */
	DEVICE_UPGRADE_STAGE_FAIL_DONE = 3,	/*!< Upgrade has completed with a failure */
	DEVICE_UPGRADE_STAGE_WRITE = 4,	/*!< Upgrade is is progress - writing the file */
};

/**
* @enum device_upgrade_error
* @brief device upgrade error - error that occurred during the upgrade
*/
enum device_upgrade_error
{
	DEVICE_UPGRADE_ERROR_NONE = 0x0000, 	/*!< No error */

	DEVICE_UPGRADE_ERROR_DEV_CODE_MISMATCH = 0x0101, 	/*!< error - device code mismatch */
	DEVICE_UPGRADE_ERROR_MANF_ID_MISMATCH = 0x0102, 	/*!< error - manufacturer id mismatch */
	DEVICE_UPGRADE_ERROR_MODEL_ID_MISMATCH = 0x0103, 	/*!< error - model id mismatch */

	DEVICE_UPGRADE_ERROR_DNT_FILE_MALFORMED = 0x0111, 	/*!< error - DNT file is malformed */
	DEVICE_UPGRADE_ERROR_DNT_BAD_CRC = 0x0112, 	/*!< error - DNT file has a bad CRC */
	DEVICE_UPGRADE_ERROR_DNT_DNT_VERSION_UNSUPPORTED = 0x0113, 	/*!< error - DNT version is not supported */

	DEVICE_UPGRADE_ERROR_UPSUPPORTED_UPGRADE_ACTION = 0x0121,	/*!< error - unsupported upgrade action(s) */

	DEVICE_UPGRADE_ERROR_FLASH_ERROR = 0x0200,	/*!< error - flash */
	DEVICE_UPGRADE_ERROR_FLASH_ACCESS_FAILED = 0x0201,	/*!< error - flash access failed */

	DEVICE_UPGRADE_ERROR_DOWNLOAD_ERROR = 0x0300,	/*!< error - download error */
	DEVICE_UPGRADE_ERROR_UNKNOWN_FILE = 0x0301,	/*!< error - unknown file */
	DEVICE_UPGRADE_ERROR_FILE_PERMISSION_DENIED = 0x0302,	/*!< error - file permission denied */
	DEVICE_UPGRADE_ERROR_NO_SUCH_SERVER = 0x0303,	/*!< error - no such server */
	DEVICE_UPGRADE_ERROR_SERVER_ACCESS_FAILED = 0x0304,	/*!< error - server access failed */
	DEVICE_UPGRADE_ERROR_SERVER_TIMEOUT = 0x0305,	/*!< error - server timeout */
	DEVICE_UPGRADE_ERROR_UNSUPPORTED_PROTOCOL = 0x0306,	/*!< error - unsupported protocol */
	DEVICE_UPGRADE_ERROR_FILENAME_TOO_LONG = 0x0307,	/*!< error - filename too long */
	DEVICE_UPGRADE_ERROR_OTHER = 0x0308,	/*!< error - not defined / other */
	DEVICE_UPGRADE_ERROR_ILLEGAL_OPERATION = 0x0309,	/*!< error - illegal operation */
	DEVICE_UPGRADE_ERROR_UNKNOWN_TRANSFER_ID = 0x030A,	/*!< error - unknown transfer id */
	DEVICE_UPGRADE_ERROR_MALLOC = 0x030B,	/*!< error - malloc error */
	DEVICE_UPGRADE_ERROR_PARTITION_VERSION_UNSUPPORTED = 0x030C,	/*!< the flash partition contained in the DNT file is too old / unsupported on this platform */
};

/**
* @enum device_erase_mode
* @brief device erase mode bitmask
*/
enum device_erase_mode
{
	ERASE_MODE_FACTORY_DEFAULTS = 0x0001, 	/*!< Erase to factory defaults */
	ERASE_MODE_KEEP_IP = 0x0002, 	/*!< Erase to factory defaults, but keep static network configuration */
};

/**
* @enum device_reboot_mode
* @brief Device reboot modes
*/
enum device_reboot_mode
{
	REBOOT_MODE_SOFTWARE = 0x0001, 	/*!< Software reset */
};

/**
* @enum device_identity_request_valid_flags
* @brief Device identity request valid flags bitmask - which fields in the "device identity command/request" messages are valid
*/
enum device_identity_request_valid_flags
{
	IDENTITY_REQUEST_NAME_CHANGE_VALID = 0x0001, /*!< The name_change_string_offset field is valid */
};

/**
* @enum device_identity_status_valid_flags
* @brief Device identity status valid flags bitmask
*/
enum device_identity_status_valid_flags
{
	IDENTITY_STATUS_NAME_CONFLICT = 0x0001, /*!< The friendly name is conflicting with another device */
};

/**
* @enum device_switch_led_mode
* @brief Device switch led modes
*/
enum device_switch_led_mode
{
	SWITCH_LED_MODE_NONE      = 0x00,	/*!< Switch LEDs mode is none / not available */
	SWITCH_LED_MODE_FORCE_OFF = 0x01, 	/*!< Switch LEDs are forced off */
	SWITCH_LED_MODE_FORCE_ON  = 0x02, 	/*!< Switch LEDs are forced on */
	SWITCH_LED_MODE_NORMAL    = 0x03, 	/*!< Switch LEDs are normal - link / activity controlled by the switch/PHY */
};

/**
* @enum device_switch_led_request_valid_flags
* @brief Device switch led request valid flags bitmask - which fields in the "device switch led command/request" messages are valid
*/
enum device_switch_led_request_valid_flags
{
	SWITCH_LED_REQUEST_SWITCH_LED_MODE_VALID = 0x0001, /*!< The switch_led_mode field is valid */
};

/**@}*/

/**
* \defgroup DeviceStructs DDP Device Structures
* @{
*/

/**
 * @struct ddp_device_general_request
 * @brief Structure format for the "device general request" message
 */
struct ddp_device_general_request
{
	ddp_message_header_t header;			/*!< message header */
	ddp_request_subheader_t subheader;	/*!< request message subheader */
};

/**
 * @struct ddp_device_general_response
 * @brief Structure format for the "device general response" message
 */
struct ddp_device_general_response
{
	ddp_message_header_t header;			/*!< message header */
	ddp_response_subheader_t subheader;		/*!< response message subheader */

	struct
	{
		// base payload. Additional elements must be appended to the end of this list.
		dante_id64_t model_id;						/*!< 64-bit model id of the device */
		ddp_raw_offset_t model_id_string_offset;	/*!< offset from the start of the payload to a NULL terminated model id string */
		uint16_t pad0;								/*!< pad byte for alignment, must be 0x0000 */
		uint32_t software_version; 					/*!< software version as a 8/8/16 major/minor/bugfix version */
		uint32_t software_build;					/*!< software build */
		uint32_t firmware_version; 					/*!< firmware version as a 8/8/16 major/minor/bugfix version */
		uint32_t firmware_build;					/*!< firmware build */
		uint32_t bootloader_version;				/*!< bootloader/u-boot version as a 8/8/16 major/minor/bugfix version */
		uint32_t bootloader_build;					/*!< bootloader build */
		uint32_t api_version;						/*!< api version as a 8/8/16 major/minor/build version */
		uint32_t capability_flags;					/*!< capability flags bitmask @see device_general_capability_flags  */
		uint32_t status_flags;						/*!< status flags bitmask @see device_general_status_flags*/
	} payload; /*!< fixed payload */

	//heap goes here!
};

/**
 * @struct ddp_device_manf_request
 * @brief Structure format for the "device manufacturer request" message
 */
struct ddp_device_manf_request
{
	ddp_message_header_t header;		/*!< message header */
	ddp_request_subheader_t subheader;	/*!< request message subheader */

	//for Dante v4 Compatibility we need an extra payload struct
#ifdef DANTE_V4
	// below was added when this funct was expanded in version 4.blah.blah
	struct
	{
		uint32_t override_valid_flags;					/*!< bitwise OR'd flags to indicate which fields to override in the ddp_device_general_response. @see device_manuf_override_valid_flags */
		uint8_t pad0[8];								/*!< pad bytes for alignment, must be zero */
		ddp_raw_offset_t manf_name_string_offset;		/*!< offset from the start of the payload to a NULL terminated manufacturer name string */
		ddp_raw_offset_t model_id_string_offset;		/*!< offset from the start of the payload to a NULL terminated model name string */
		uint8_t pad1[8];								/*!< pad bytes for alignment, must be zero */
		uint32_t software_version; 						/*!< manufacturer software version as a 8/8/16 major/minor/bugfix version */
		uint32_t software_build; 						/*!< manufacturer software build version */
		uint32_t firmware_version; 						/*!< manufacturer firmware version as a 8/8/16 major/minor/bugfix version */
		uint32_t firmware_build; 						/*!< manufacturer firmware build version */
		uint32_t capability_flags;						/*!< manufacturer device capability flags bitmask */
		uint32_t model_version; 						/*!< manufacturer model version as a 8/8/16 major/minor/bugfix version */
		ddp_raw_offset_t model_version_string_offset;	/*!< offset from the start of the payload to a NULL terminated model version string */
		uint16_t pad2;									/*!< pad byte for alignment, must be 0x0000 */
	} payload; /*!< fixed payload */
#endif



};

/**
 * @struct ddp_device_manf_response
 * @brief Structure format for the "device general response" message
 */
struct ddp_device_manf_response
{
	ddp_message_header_t header;				/*!< message header */
	ddp_response_subheader_t subheader;			/*!< response message subheader */

	struct
	{
		// base payload. Additional elements must be appended to the end of this list.
		dante_id64_t manf_id;							/*!< 64-bit manufacturer id of the device */
		ddp_raw_offset_t manf_name_string_offset;		/*!< offset from the start of the payload to a NULL terminated manufacturer name string */
		ddp_raw_offset_t model_id_string_offset;		/*!< offset from the start of the payload to a NULL terminated model id string */
		dante_id64_t model_id;							/*!< 64-bit model id of the device */
		uint32_t software_version; 						/*!< manufacturer software version as a 8/8/16 major/minor/bugfix version */
		uint32_t software_build; 						/*!< manufacturer software build version */
		uint32_t firmware_version; 						/*!< manufacturer firmware version as a 8/8/16 major/minor/bugfix version */
		uint32_t firmware_build; 						/*!< manufacturer firmware build version */
		uint32_t capability_flags;						/*!< manufacturer device capability flags bitmask */
		uint32_t model_version; 						/*!< manufacturer model version as a 8/8/16 major/minor/bugfix version */
		ddp_raw_offset_t model_version_string_offset;	/*!< offset from the start of the payload to a NULL terminated model version string */
		uint16_t pad0;									/*!< pad byte for alignment, must be 0x0000 */
	} payload; /*!< fixed payload */

	//heap goes here!
};

/**
 * @struct ddp_device_upgrade_request
 * @brief Structure format for the "device upgrade command/request" message
 */
struct ddp_device_upgrade_request
{
	ddp_message_header_t header;		/*!< message header */
	ddp_request_subheader_t subheader;	/*!< request message subheader */

	struct
	{
		uint16_t valid_flags;					/*!< valid flags bitmask indicating which fields in this messages are valid @see device_upgrade_request_valid_flags */
		uint16_t upgrade_action_flags;			/*!< which upgrade actions to trigger @see device_upgrade_action_flags */
		uint32_t upgrade_flags;					/*!< upgrade flags - not currently used */
		uint32_t upgrade_flags_mask;			/*!< mask of valid upgrade flags - not currently used */
		uint16_t protocol_type;					/*!< protocol type to use for the upgrade @see device_upgrade_protocol */
		ddp_raw_offset_t prot_params_offset;	/*!< offset from the start of the payload to the protocol parameter structure */
		dante_id64_t manf_override;				/*!< 64-bit manufacturer override id for the upgrade DNT */
		dante_id64_t model_override;			/*!< 64-bit model override id for the upgrade DNT */
	} payload; /*!< fixed payload */

	//heap goes here!
};

/**
 * @struct ddp_device_upgrade_tftp_protocol_params
 * @brief Structure format for the tftp protocol parameters in the "device upgrade command/request" message
 */
struct ddp_device_upgrade_tftp_protocol_params
{
	uint16_t valid_flags;				/*!< valid flags bitmask indicating which field in this structure is valid @see device_upgrade_tftp_prot_valid_flags*/
	ddp_raw_offset_t filename_offset;	/*!< filename - offset from the start of the payload to a NULL terminated string */
	uint32_t file_length;				/*!< length of the upgrade file in bytes */
	uint16_t family; 					/*!< IP address family for the TFTP server IP address @see ddp_address_family */
	uint16_t port;						/*!< port for the TFTP server */
	uint32_t ip_address;				/*!< IPv4 address for the TFTP server */
};

//! @cond Doxygen_Suppress
typedef struct ddp_device_upgrade_tftp_protocol_params ddp_device_upgrade_tftp_protocol_params_t;
//! @endcond

/**
 * @struct ddp_device_upgrade_xmodem_protocol_params
 * @brief Structure format for the xmodem protocol parameters in the "device upgrade command/request" message
 */
struct ddp_device_upgrade_xmodem_protocol_params
{
	uint16_t valid_flags;		/*!< valid flags bitmask indicating which field in this structure is valid @see device_upgrade_response_valid_flags*/
	uint16_t port;				/*!< port for the xmodem upgrade @see device_upgrade_xmodem_port */
	uint8_t master_cpol;		/*!< SPI master clock polarity @see ddp_spi_cpol*/
	uint8_t master_cpha;		/*!< SPI master clock phase @see ddp_spi_cpha*/
	uint8_t slave_cpol;			/*!< SPI slave clock polarity @see ddp_spi_cpol*/
	uint8_t slave_cpha;			/*!< SPI slave clock phase @see ddp_spi_cpha*/
	uint8_t parity;				/*!< UART parity @see ddp_uart_parity*/
	uint8_t flow_control;		/*!< UART flow control @see ddp_uart_flow_control*/
	uint16_t pad0;				/*!< pad bytes for alignment, must be 0x0000 */
	uint32_t baudrate;			/*!< SPI/UART buadrate */
	uint32_t file_length;		/*!< length of the upgrade file in bytes */
};

//! @cond Doxygen_Suppress
typedef struct ddp_device_upgrade_xmodem_protocol_params ddp_device_upgrade_xmodem_protocol_params_t;
// @endcond

/**
 * @struct ddp_device_upgrade_response
 * @brief Structure format for the "device upgrade response" message
 */
struct ddp_device_upgrade_response
{
	ddp_message_header_t header;				/*!< message header */
	ddp_response_subheader_t subheader;		/*!< response message subheader */

	struct
	{
		// base payload. Additional elements must be appended to the end of this list.
		uint16_t valid_flags;		/*!< valid flags bitmask indicating which field in this message is valid @see device_upgrade_response_valid_flags */
		uint16_t upgrade_stage;		/*!< current upgrade stage @see device_upgrade_stage */
		uint32_t upgrade_error;		/*!< upgrade error @see device_upgrade_error */
		uint32_t progress_current;	/*!< current upgrade progress for this stage (progress = progress_current / progress_total) */
		uint32_t progress_total;	/*!< total upgrade progress for this stage (progress = progress_current / progress_total) */
		dante_id64_t manf_id;		/*!< 64-bit manufacturer id of the device */
		dante_id64_t model_id;		/*!< 64-bit model id of the device */
	} payload; /*!< fixed payload */

	//heap goes here!
};

/**
 * @struct ddp_device_erase_request
 * @brief Structure format for the "device erase command/request" message
 */
struct ddp_device_erase_request
{
	ddp_message_header_t header;		/*!< message header */
	ddp_request_subheader_t subheader;	/*!< request message subheader */

	struct
	{
		uint16_t mode;			/*!< Which erase mode to trigger @see device_erase_mode */
		uint16_t mode_mask;		/*!< Bitwise OR'd flags of which mode to trigger @see device_erase_mode */
	} payload;	/*!< fixed payload */

	//heap goes here!
};

/**
 * @struct ddp_device_erase_response
 * @brief Structure format for the "device erase response" message
 */
struct ddp_device_erase_response
{
	ddp_message_header_t header;			/*!< message header */
	ddp_response_subheader_t subheader;		/*!< response message subheader */

	struct
	{
		// base payload. Additional elements must be appended to the end of this list.
		uint16_t supported_flags;		/*!< which erase modes are supported @see device_erase_mode */
		uint16_t executed_flags;		/*!< what modes have been executed @see device_erase_mode */
	} payload; /*!< fixed payload */

	//heap goes here!
};

/**
 * @struct ddp_device_reboot_request
 * @brief Structure format for the "device reboot command/request" message
 */
struct ddp_device_reboot_request
{
	ddp_message_header_t header;		/*!< message header */
	ddp_request_subheader_t subheader;	/*!< request message subheader */

	struct
	{
		uint16_t mode;					/*!< which erase mode to trigger @see device_reboot_mode */
		uint16_t mode_mask;				/*!< Bitwise OR'd flags of which mode to trigger @see device_reboot_mode */
	} payload; /*!< fixed payload */

	//heap goes here!
};

/**
 * @struct ddp_device_reboot_response
 * @brief Structure format for the "device reboot response" message
 */
struct ddp_device_reboot_response
{
	ddp_message_header_t header;			/*!< message header */
	ddp_response_subheader_t subheader;		/*!< response message subheader */

	struct
	{
		uint16_t supported_flags;		/*!< which reboot modes are supported @see device_reboot_mode */
		uint16_t executed_flags;		/*!< what modes have been executed @see device_reboot_mode */
	} payload; /*!< fixed payload */

	//heap goes here!
};

/**
 * @struct ddp_device_identity_request
 * @brief Structure format for the "device identity command/request" message
 */
struct ddp_device_identity_request
{
	ddp_message_header_t header;		/*!< message header */
	ddp_request_subheader_t subheader;	/*!< request message subheader */

	struct
	{
		uint16_t valid_flags;				/*!< valid flags bitmask indicating which fields in this messages are valid @see device_identity_request_valid_flags */
		uint16_t name_change_string_offset;	/*!< name change string - offset from the start of the payload to the NULL terminated string */
	} payload; /*!< fixed payload */

	// Heap goes here
};

/**
 * @struct ddp_device_identity_response_payload
 * @brief Structure format for the payload of the "device identity response" message
 */
struct ddp_device_identity_response_payload
{
	uint16_t identity_status_flags;				/*!< identity status flags bitmask @see device_identity_status_valid_flags */
	dante_process_id_t process_id;				/*!< process id - used in conjunction with the dante_device_id to uniquely identify a device */
	dante_device_id_t device_id;				/*!< EUI64 that when combined with the process_id uniquely identifies the dante device */
	ddp_raw_offset_t default_name_offset;		/*!< default name string - offset from the start of the payload to the NULL terminated string */
	ddp_raw_offset_t friendly_name_offset;		/*!< friendly name string - offset from the start of the payload to the NULL terminated string */
	ddp_raw_offset_t dante_domain_offset;		/*!< dante domain string - offset from the start of the payload to the NULL terminated string */
	ddp_raw_offset_t advertised_name_offset; 	/*!< advertised string - offset from the start of the payload to the NULL terminated string */
};

//! @cond Doxygen_Suppress
typedef struct ddp_device_identity_response_payload ddp_device_identity_response_payload_t;
//! @endcond

/**
* @struct ddp_device_identity_response
* @brief Structure format for the "device identity response" message
*/
struct ddp_device_identity_response
{
	ddp_message_header_t header;			/*!< message header */
	ddp_response_subheader_t subheader;		/*!< response message subheader */

	ddp_device_identity_response_payload_t payload;	/*!< Payload @see ddp_device_identity_response_payload */

	// Heap goes here
};

/**
* @struct ddp_device_identify_response
* @brief Structure format for the "device identify response" message
*/
struct ddp_device_identify_response
{
	ddp_message_header_t header;			/*!< message header */
	ddp_response_subheader_t subheader;		/*!< response message subheader */
};

/**
 * @struct ddp_device_gpio_request
 * @brief Structure format for the "device GPIO command/request" message
 */
struct ddp_device_gpio_request
{
	ddp_message_header_t header;		/*!< message header */
	ddp_request_subheader_t subheader;	/*!< request message subheader */

	struct
	{
		uint32_t output_state_valid_flags;		/*!< bitmask of which output_state_values are valid */
		uint32_t output_state_values;			/*!< bitmask of output state values - this is the value to set each GPIO output pin */
	} payload; /*!< fixed payload */

	// Heap goes here
};

/**
 * @struct ddp_device_gpio_response
 * @brief Structure format for the "device gpio response" message
 */
struct ddp_device_gpio_response
{
	ddp_message_header_t header;			/*!< message header */
	ddp_response_subheader_t subheader;		/*!< response message subheader */

	struct
	{
		uint32_t interrupt_trigger_mask;		/*!< bitmask of which GPIO's triggered an interrupt  */
		uint32_t input_state_valid_mask;		/*!< bitmask of which input_state_values are valid */
		uint32_t input_state_values;			/*!< bitmask of input state values - this is the current value of each GPIO input pin */
		uint32_t output_state_valid_mask;		/*!< bitmask of which output_state_values are valid */
		uint32_t output_state_values;			/*!< bitmask of input state values - this is the current value of each GPIO output pin */
	} payload; /*!< fixed payload */

	//heap goes here!
};

/**
 * @struct ddp_device_switch_led_request
 * @brief Structure format for the "device switch led command/request" message
 */
struct ddp_device_switch_led_request
{
	ddp_message_header_t header;		/*!< message header */
	ddp_request_subheader_t subheader;	/*!< request message subheader */

	struct
	{
		uint16_t valid_flags;			/*!< valid flags bitmask indicating which fields in this messages are valid @see device_switch_led_request_valid_flags */
		uint8_t switch_led_mode;		/*!< switch led mode @see device_switch_led_mode */
		uint8_t pad0;					/*!< pad byte for alignment, must be 0x00 */
	} payload; /*!< fixed payload */

	// Heap goes here
};

/**
 * @struct ddp_device_switch_led_response
 * @brief Structure format for the "device switch led response" message
 */
struct ddp_device_switch_led_response
{
	ddp_message_header_t header;			/*!< message header */
	ddp_response_subheader_t subheader;		/*!< response message subheader */

	struct
	{
		uint8_t switch_led_mode;		/*!< switch led mode @see device_switch_led_mode */
		uint8_t pad0;					/*!< pad byte for alignment, must be 0x00 */
		uint16_t pad1;					/*!< pad byte for alignment, must be 0x0000 */
	} payload; /*!< fixed payload */

	//heap goes here!
};

//! @cond Doxygen_Suppress
typedef struct ddp_device_general_request ddp_device_general_request_t;
typedef struct ddp_device_general_response ddp_device_general_response_t;
typedef struct ddp_device_manf_request ddp_device_manf_request_t;
typedef struct ddp_device_manf_response ddp_device_manf_response_t;
typedef struct ddp_device_upgrade_request ddp_device_upgrade_request_t;
typedef struct ddp_device_upgrade_response ddp_device_upgrade_response_t;
typedef struct ddp_device_erase_request ddp_device_erase_request_t;
typedef struct ddp_device_erase_response ddp_device_erase_response_t;
typedef struct ddp_device_reboot_request ddp_device_reboot_request_t;
typedef struct ddp_device_reboot_response ddp_device_reboot_response_t;
typedef struct ddp_device_identity_request ddp_device_identity_request_t;
typedef struct ddp_device_identity_response ddp_device_identity_response_t;
typedef struct ddp_device_identify_response ddp_device_identify_response_t;
typedef struct ddp_device_gpio_request ddp_device_gpio_request_t;
typedef struct ddp_device_gpio_response ddp_device_gpio_response_t;
typedef struct ddp_device_switch_led_request ddp_device_switch_led_request_t;
typedef struct ddp_device_switch_led_response ddp_device_switch_led_response_t;
//! @endcond

/**@}*/

/**@}*/

#endif // _DDP_DEVICE_STRUCTURES_H
