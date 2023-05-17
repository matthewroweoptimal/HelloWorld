/*
 * File     : device_client.h
 * Created  : August 2014
 * Updated  : Date: 2014/08/22
 * Author   : Michael Ung <michael.ung@audinate.com>
 * Synopsis : All device DDP client helper functions.
 *
 * Copyright 2005-2014, Audinate Pty Ltd and/or its licensors
	Confidential
 * Audinate Copyright Header Version 1
 */

#ifndef _DDP_DEVICE_CLIENT_H
#define _DDP_DEVICE_CLIENT_H

#include "packet.h"
#include "device_structures.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @file device_client.h
* @brief All device DDP client helper functions.
*/

/**
* \defgroup Device DDP Device
* @{
*/

/** \addtogroup DeviceEnums
*  @{
*/

/**
* @enum ddp_xmodem_spi_dev
* @brief Available SPI device types
*/
enum ddp_xmodem_spi_dev
{
	DDP_XMODEM_SPI_DEV_MASTER = 0,	/*!< SPI master which transmits data from the device to the host processor */
	DDP_XMODEM_SPI_DEV_SLAVE = 1,	/*!< SPI slave which receives data from the host processor to the device */
	DDP_XMODEM_SPI_NUM_DEVS				/*!< The number of SPI device types on the device */
};

/**
* @enum ddp_xmodem_uart_supported_baud
* @brief UART baud rates used for XMODEM device upgrade
*/
enum ddp_xmodem_uart_supported_baud
{
	DDP_UART_BAUD_115200 = 115200,	/*!< Baud 115200 */
	DDP_UART_BAUD_57600 = 57600,	/*!< Baud 57600 */
	DDP_UART_BAUD_38400 = 38400,	/*!< Baud 38400 */
	DDP_UART_BAUD_19200 = 19200,	/*!< Baud 19200 */
	DDP_UART_BAUD_9600 = 9600		/*!< Baud 9600 */
};

/**
* @enum ddp_xmodem_spi_supported_baud
* @brief SPI baud rates used for XMODEM device upgrade
*/
enum ddp_xmodem_spi_supported_baud
{
	DDP_SPI_BAUD_25000000 = 25000000,		/*!< Baud 25000000 */
	DDP_SPI_BAUD_12500000 = 12500000,		/*!< Baud 12500000 */
	DDP_SPI_BAUD_6250000 = 6250000,		/*!< Baud 6250000 */
	DDP_SPI_BAUD_3125000 = 3125000,		/*!< Baud 3125000 */
	DDP_SPI_BAUD_1562500 = 1562500,		/*!< Baud 1562500 */
	DDP_SPI_BAUD_781250 = 781250,		/*!< Baud 781250 */
	DDP_SPI_BAUD_390625 = 390625,		/*!< Baud 390625 */
	DDP_SPI_BAUD_195312 = 195312,		/*!< Baud 195312 */
	DDP_SPI_BAUD_97656 = 97656,		/*!< Baud 97656 */
	DDP_SPI_BAUD_48828 = 48828			/*!< Baud 48828 */
};

/**@}*/

/**
* \defgroup DeviceStructs DDP Device Structures
* @{
*/

/**
* @struct upgrade_tftp
* @brief TFTP related parameters used for the device upgrade
*/
struct upgrade_tftp
{
	const char* filename;		/*!< Name of the DNT file used for the upgrade */
	uint32_t file_length;		/*!< Length of the DNT file */
	uint16_t family;			/*!< IP Address family @see ddp_address_family */
	uint16_t port;				/*!< Port of the TFTP server, the default port is 69 */
	uint32_t ip_address;		/*!< IP address of the TFTP server */
};

/** @cond */
typedef struct upgrade_tftp upgrade_tftp_t;
/** @endcond */

/**
* @struct upgrade_xmodem
* @brief XMODEM related parameters used for the device upgrade
*/
struct upgrade_xmodem
{
	uint16_t port;							/*!< Serial interface which is used to transfer the DNT file from the host processor to the device @see device_upgrade_xmodem_port  */
	uint32_t baud_rate;						/*!< The data rate of the data transfer, for UART @see ddp_xmodem_uart_supported_baud, for SPI @see ddp_xmodem_spi_supported_baud */
	uint32_t file_length;					/*!< Length of the DNT file */

	union
	{
		struct
		{
			uint16_t parity;				/*!< UART parity mode @see ddp_uart_parity */
			uint16_t flow_control;			/*!< UART flow control mode @see ddp_uart_flow_control */
		} uart;

		struct
		{
			uint16_t polarity;				/*!< SPI polarity mode for a SPI device type @see ddp_spi_cpol and @see ddp_xmodem_spi_dev */
			uint16_t phase;					/*!< SPI phase mode for a SPI device type @see ddp_spi_cpha and @see ddp_xmodem_spi_dev */
		} spi[DDP_XMODEM_SPI_NUM_DEVS];
	} s_params;	/*!< UART or SPI xmodem upgrade parameters */
};

/** @cond */
typedef struct upgrade_xmodem upgrade_xmodem_t;
/** @endcond */

/**
* @struct upgrade_rsp_payload
* @brief Information related to a device upgrade
*/
struct upgrade_rsp_payload
{
	uint16_t valid_flags;			/*!< Bitwise OR'd valid flags @see device_upgrade_response_valid_flags  */
	uint16_t upgrade_stage;			/*!< Current stage of the upgrade process @see device_upgrade_stage */
	uint32_t upgrade_error;			/*!< Current upgrade error if any @see device_upgrade_error */
	uint32_t progress_current;		/*!< Current amount of data in bytes written into flash */
	uint32_t progress_total;		/*!< Total amount of data which needs to be written into flash */
	const dante_id64_t* manf_id;	/*!< Manufacturer ID of the device @see dante_id64_t */
	const dante_id64_t* model_id;	/*!< Model ID of the device @see dante_id64_t */
};

/** @cond */
typedef struct upgrade_rsp_payload upgrade_rsp_payload_t;
/** @endcond */


/**
* @struct ddp_manf_override_values
* @brief Device manufacturer information that needs to be overriden
*/
struct ddp_manf_override_values
{
	char const * manf_name_string;		/*!< Pointer to NULL terminated manufacturer name string to override, use NULL to retain the existing value */
	char const * model_id_string;		/*!< Pointer to NULL terminated model ID string to override, use NULL to retain the existing value */
	uint32_t software_version; 			/*!< 32-bit software version to override, use 0 to retain the existing value */
	uint32_t software_build; 			/*!< 32-bit software build to override, use 0 to retain the existing value */
	uint32_t firmware_version; 			/*!< 32-bit firmware version to override, use 0 to retain the existing value */
	uint32_t firmware_build; 			/*!< 32-bit firmware build to override, use 0 to retain the existing value */
	uint32_t capability_flags;			/*!< 32-bit capability flags to override, use 0 to retain the existing value */
	uint32_t model_version; 			/*!< 32-bit model version to override, use 0 to retain the existing value */
	char const * model_version_string;	/*!< Pointer to NULL terminated model name string to override, use NULL to retain the existing value */
};

/** @cond */
typedef struct ddp_manf_override_values ddp_manf_override_values_t;
/** @endcond */






/**@}*/

/**
* \defgroup DeviceFuncs DDP Device Functions
* @{
*/

/**
* Creates a DDP device general request message. This message can only be used to query the current state of the device
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param request_id [in] A non-zero sequence number which is populated in this message
* @return AUD_SUCCESS if the DDP device general request message was successfully created, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_add_device_general_request
(
	ddp_packet_write_info_t * packet_info,
	ddp_request_id_t request_id
);

/**
* Reads data fields from a DDP device general response message
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param offset [in] Starting point in the packet buffer to read this message
* @param out_seqnum [out optional] Pointer to the sequence number of this message
* @param out_status [out optional] Pointer to the status of this message
* @param out_model_id [out optional] Pointer to the pointer to the 8-byte hexadecimal format model ID @see dante_id64_t of the device
* @param out_model_id_string [out optional] Pointer to the pointer to the null terminated model ID string of the device
* @param out_software_version [out optional] Pointer to the software version of the device as a 8/8/16 major/minor/bugfix version
* @param out_software_build [out optional] Pointer to the software build of the device
* @param out_firmware_version [out optional] Pointer to the firmware version of the device as a 8/8/16 major/minor/bugfix version
* @param out_firmware_build [out optional] Pointer to the firmware build of the device
* @param out_bootloader_version [out optional] Pointer to the bootloader version the device as a 8/8/16 major/minor/bugfix version
* @param out_bootloader_build [out optional] Pointer to the bootloader build of the device
* @param out_api_version [out optional] Pointer to the API version of the device as a 8/8/16 major/minor/build version
* @param out_capability_flags [out optional] Pointer to the bitwise OR'd capability flags @see device_general_capability_flags
* @param out_status_flags [out optional] Pointer to the bitwise OR'd status flags @see device_general_status_flags
* @return AUD_SUCCESS if the DDP device general response message was successfully read, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_read_device_general_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t * out_seqnum,
	ddp_status_t * out_status,
	dante_id64_t** out_model_id,
	char** out_model_id_string,
	uint32_t* out_software_version,
	uint32_t* out_software_build,
	uint32_t* out_firmware_version,
	uint32_t* out_firmware_build,
	uint32_t* out_bootloader_version,
	uint32_t* out_bootloader_build,
	uint32_t* out_api_version,
	uint32_t* out_capability_flags,
	uint32_t* out_status_flags
);

/**
* Creates a DDP device manufacturer request message. This message can only be used to query the current state of the device
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param request_id [in] A non-zero sequence number which is populated in this message
* @param ddp_manf_override_values_t [in] contains the overide value (if any) for the device software/firmware versions etc.
* @return AUD_SUCCESS if the DDP device manufacturer request message was successfully created, else a non AUD_SUCCESS value is returned
*/

aud_error_t
ddp_add_device_manufacturer_request
(
	ddp_packet_write_info_t * packet_info,
	ddp_request_id_t request_id,
	ddp_manf_override_values_t * override_values
);

/**
* Reads data fields from a DDP device manufacturer response message
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param offset [in] Starting point in the packet buffer to read this message
* @param out_request_id [out optional] Pointer to the sequence number of this message
* @param out_status [out optional] Pointer to the status of this message
* @param out_manf_id [out optional] Pointer to the pointer to the 8-byte hexadecimal format manufacturer ID of the device @see dante_id64_t
* @param out_manf_name_string [out optional] Pointer to the pointer to the null terminated manufacturer name string of the device
* @param out_model_id [out optional] Pointer to the pointer to the 8-byte hexadecimal format model ID @see dante_id64_t of the device
* @param out_model_id_string [out optional] Pointer to the pointer to the null terminated model ID string of the device
* @param out_software_version [out optional] Pointer to the software version of the device as a 8 bytes / 8 bytes / 16 bytes major/minor/bugfix version
* @param out_software_build [out optional] Pointer to the software build of the device
* @param out_firmware_version [out optional] Pointer to the firmware version of the device as a 8 bytes / 8 bytes / 16 bytes major/minor/bugfix version
* @param out_firmware_build [out optional] Pointer to the firmware build of the device
* @param out_capability_flags [out optional] Pointer to the capability flags
* @param out_model_version [out optional] Pointer to the manufacturer model version of the device as a 8 bytes / 8 bytes / 16 bytes major/minor/bugfix version
* @param out_model_version_string [out optional] Pointer to the pointer to the null terminated manufacturer model version string of the device
* @return AUD_SUCCESS if the DDP device manufacturer response message was successfully read, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_read_device_manufacturer_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t* out_request_id,
	ddp_status_t* out_status,
	dante_id64_t** out_manf_id,
	char** out_manf_name_string,
	dante_id64_t** out_model_id,
	char** out_model_id_string,
	uint32_t* out_software_version,
	uint32_t* out_software_build,
	uint32_t* out_firmware_version,
	uint32_t* out_firmware_build,
	uint32_t* out_capability_flags,
	uint32_t* out_model_version,
	char** out_model_version_string
);

/**
* Creates a DDP device upgrade TFTP request message. This message can be used as a command to change the state of the device and as a request to query the current state of the device
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param request_id [in] A non-zero sequence number which is populated in this message
* @param upgrade_action_flags Bitwise OR'd upgrade action flags @see device_upgrade_action_flags, use zero when querying the current state of the device
* @param upgrade_flags Bitwise OR'd upgrade flags, currently none supported, use the value of zero
* @param upgrade_flags_mask Bitwise OR'd upgrade flags mask, currently none supported, use the value of zero
* @param manf_override [in optional] Pointer to the 8-byte hexadecimal format manufacturer ID which is used to override the current manufacturer ID @see dante_id64_t. If not overriding use NULL
* @param model_override [in optional] Pointer to the 8-byte hexadecimal format model ID which is used to override the current model ID @see dante_id64_t. If not overriding use NULL
* @param tftp_params [in] Pointer to the TFTP related parameters used for the upgrade
* @return AUD_SUCCESS if the DDP device manufacturer request message was successfully created, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_add_device_upgrade_tftp_request
(
	ddp_packet_write_info_t * packet_info,
	ddp_request_id_t request_id,
	uint16_t upgrade_action_flags,
	uint32_t upgrade_flags,
	uint32_t upgrade_flags_mask,
	dante_id64_t* manf_override,
	dante_id64_t* model_override,
	upgrade_tftp_t* tftp_params
);

/**
* Creates a DDP device upgrade XMODEM request message. This message can be used as a command to change the state of the device and as a request to query the current state of the device
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param request_id [in] A non-zero sequence number which is populated in this message
* @param upgrade_action_flags [in] Bitwise OR'd upgrade action flags @see device_upgrade_action_flags, use zero when querying the current state of the device
* @param upgrade_flags [in] Bitwise OR'd upgrade flags, currently none supported, use the value of zero
* @param upgrade_flags_mask [in] Bitwise OR'd upgrade flags mask, currently none supported, use the value of zero
* @param manf_override [in optional] Pointer to the 8-byte hexadecimal format manufacturer ID which is used to override the current manufacturer ID @see dante_id64_t. If not overriding use NULL
* @param model_override [in optional] Pointer to the 8-byte hexadecimal format model ID which is used to override the current model ID @see dante_id64_t. If not overriding use NULL
* @param xmodem_params [in] Pointer to the XMODEM related parameters used for the upgrade
* @return AUD_SUCCESS if the DDP device manufacturer request message was successfully created, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_add_device_upgrade_xmodem_request
(
	ddp_packet_write_info_t * packet_info,
	ddp_request_id_t request_id,
	uint16_t upgrade_action_flags,
	uint32_t upgrade_flags,
	uint32_t upgrade_flags_mask,
	dante_id64_t* manf_override,
	dante_id64_t* model_override,
	upgrade_xmodem_t* xmodem_params
);

/**
* Reads data fields from a DDP device upgrade response message
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param offset [in] Starting point in the packet buffer to read this message
* @param out_request_id [out optional] Pointer to the sequence number of this message
* @param out_status [out optional] Pointer to the status of this message
* @param out_payload [in out optional] Pointer to a structure which contains information related to the device upgrade process @see upgrade_rsp_payload
* @return AUD_SUCCESS if the DDP device upgrade response message was successfully read, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_read_device_upgrade_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t* out_request_id,
	ddp_status_t* out_status,
	upgrade_rsp_payload_t* out_payload
);

/**
* Creates a DDP device erase request message. This message can be used as a command to change the state of the device and as a request to query the current state of the device
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param request_id [in] A non-zero sequence number which is populated in this message
* @param mode [in] Bitwise OR'd device ease mode @see device_erase_mode
* @param mode_mask [in] The bitwise OR'd device erase mode mask @see device_erase_mode, bitwise AND of this argument with the mode argument indicates which device erase mode will be executed. Use zero to query the supported device erase modes
* @return AUD_SUCCESS if the DDP device erase request message was successfully created, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_add_device_erase_request
(
	ddp_packet_write_info_t * packet_info,
	ddp_request_id_t request_id,
	uint16_t mode,
	uint16_t mode_mask
);

/**
* Reads data fields from a DDP device erase response message
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param offset [in] Starting point in the packet buffer to read this message
* @param out_request_id [out optional] Pointer to the sequence number of this message
* @param out_status [out optional] Pointer to the status of this message
* @param out_supported_flags [out optional] Pointer to the bitwise OR'd supported flags @see device_erase_mode
* @param out_executed_flags [out optional] Pointer to the bitwise OR'd executed flags @see device_erase_mode
* @return AUD_SUCCESS if the DDP device erase response message was successfully read, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_read_device_erase_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t* out_request_id,
	ddp_status_t* out_status,
	uint16_t* out_supported_flags,
	uint16_t* out_executed_flags
);

/**
* Creates a DDP device reboot request message. This message can be used as a command to change the state of the device and as a request to query the current state of the device
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param request_id [in] A non-zero sequence number which is populated in this message
* @param mode [in] Bitwise OR'd reboot mode @see device_reboot_mode
* @param mode_mask [in] The bitwise OR'd reboot mode mask @see device_reboot_mode, bitwise AND of this argument with the mode argument indicates which reboot mode will be executed. Use zero to query the supported reboot modes
* @return AUD_SUCCESS if the DDP device reboot request message was successfully created, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_add_device_reboot_request
(
	ddp_packet_write_info_t * packet_info,
	ddp_request_id_t request_id,
	uint16_t mode,
	uint16_t mode_mask
);

/**
* Reads data fields from a DDP device reboot response message
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param offset [in] Starting point in the packet buffer to read this message
* @param out_request_id [out optional] Pointer to the sequence number of this message
* @param out_status [out optional] Pointer to the status of this message
* @param out_supported_flags [out optional] Pointer to bitwise OR'd supported flags @see device_reboot_mode
* @param out_executed_flags [out optional] Pointer to bitwise OR'd executed flags @see device_reboot_mode
* @return AUD_SUCCESS if the DDP device reboot response message was successfully read, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_read_device_reboot_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t* out_request_id,
	ddp_status_t* out_status,
	uint16_t* out_supported_flags,
	uint16_t* out_executed_flags
);

/**
* Creates a DDP device identity request message. This message can be used as a command to change the state of the device and as a request to query the current state of the device
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param request_id [in] A non-zero sequence number which is populated in this message
* @param name_change_string [in optional] Pointer to a null terminated string which is used to change the friendly name of the device, use NULL to query the current friendly name
* @return AUD_SUCCESS if the DDP device identity request message was successfully created, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_add_device_identity_request
(
	ddp_packet_write_info_t * packet_info,
	ddp_request_id_t request_id,
	const char * name_change_string
);

/**
* Reads data fields from a DDP device identity response message
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param offset [in] Starting point in the packet buffer to read this message
* @param out_request_id [out optional] Pointer to the sequence number of this message
* @param out_status [out optional] Pointer to the status of this message
* @param out_identity_status_flags [out optional] Bitwise OR'd status flags @see device_identity_status_valid_flags
* @param out_process_id [out optional] Process ID of the device
* @param out_dante_device_id [out optional] 8-byte hexadecimal format device ID @see dante_device_id_t
* @param out_default_name_string [out optional] Pointer to a pointer to a null terminated default name string set in module config
* @param out_friendly_name_string [out optional] Pointer to a pointer to a null terminated string which can override the default name
* @param out_dante_domain_string [out optional] Pointer to a pointer to a null terminated network domain name string
* @param out_advertised_name_string [out optional] Pointer to a pointer to a null terminated advertised name string
* @return AUD_SUCCESS if the DDP device identity response message was successfully read, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_read_device_identity_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t* out_request_id,
	ddp_status_t* out_status,
	uint16_t* out_identity_status_flags,
	uint16_t* out_process_id,
	dante_device_id_t** out_dante_device_id,
	const char** out_default_name_string,
	const char** out_friendly_name_string,
	const char** out_dante_domain_string,
	const char** out_advertised_name_string
);

/**
* Reads data fields from a DDP device identify response message
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param offset [in] Starting point in the packet buffer to read this message
* @param out_request_id [out optional] Pointer to the sequence number of this message
* @param out_status [out optional] Pointer to the status of this message
* @return AUD_SUCCESS if the DDP device identity response message was successfully read, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_read_device_identify_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t* out_request_id,
	ddp_status_t* out_status
);

/**
* Creates a DDP device gpio request message.
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param request_id [in] A non-zero sequence number which is populated in this message
* @param output_state_valid_flags [in] A bitmask of which output state values are valid
* @param output_state_values [in] A bitmask of output state values - this is the value to set each GPIO output pin
* @return AUD_SUCCESS if the DDP device gpio request message was successfully created, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_add_device_gpio_request
(
	ddp_packet_write_info_t * packet_info,
	ddp_request_id_t request_id,
	uint32_t output_state_valid_flags,
	uint32_t output_state_values
);


/**
* Reads data fields from a DDP device gpio response message
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param offset [in] Starting point in the packet buffer to read this message
* @param out_request_id [out optional] Pointer to the sequence number of this message
* @param out_status [out optional] Pointer to the status of this message
* @param out_interrupt_trigger_mask [out optional] Pointer to the bitmask of which GPIO's triggered an interrupt
* @param out_input_state_valid_mask [out optional] Pointer to the bitmask of which input_state_values are valid
* @param out_input_state_values [out optional] Pointer to the bitmask of input state values - this is the current value of each GPIO input pin
* @param out_output_state_valid_mask [out optional] Pointer to the bitmask of which output_state_values are valid
* @param out_output_state_values [out optional] Pointer to the bitmask of input state values - this is the current value of each GPIO output pin
* @return AUD_SUCCESS if the DDP device gpio response response message was successfully read, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_read_device_gpio_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t * out_request_id,
	ddp_status_t * out_status,
	uint32_t * out_interrupt_trigger_mask,
	uint32_t * out_input_state_valid_mask,
	uint32_t * out_input_state_values,
	uint32_t * out_output_state_valid_mask,
	uint32_t * out_output_state_values
);

/**
* Creates a DDP device switch led request message.
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param request_id [in] A non-zero sequence number which is populated in this message
* @param switch_led_mode [in] switch led mode to set on the device @see device_switch_led_mode
* @return AUD_SUCCESS if the DDP device switch led request message was successfully created, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_add_device_switch_led_request
(
	ddp_packet_write_info_t * packet_info,
	ddp_request_id_t request_id,
	uint8_t switch_led_mode
);

/**
* Reads data fields from a DDP device switch led response message
* @param packet_info [in] Pointer to a structure which has information about the DDP packet buffer
* @param offset [in] Starting point in the packet buffer to read this message
* @param out_request_id [out optional] Pointer to the sequence number of this message
* @param out_status [out optional] Pointer to the status of this message
* @param out_switch_led_mode [out optional] Pointer to the switch led mode currently set on the device @see device_switch_led_mode
* @return AUD_SUCCESS if the DDP device switch led response message was successfully read, else a non AUD_SUCCESS value is returned
*/
aud_error_t
ddp_read_device_switch_led_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t * out_request_id,
	ddp_status_t * out_status,
	uint8_t * out_switch_led_mode
);

/**@}*/
/**@}*/

#ifdef __cplusplus
}
#endif

#endif //_DDP_DEVICE_CLIENT_H
