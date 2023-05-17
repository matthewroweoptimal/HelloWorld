/**
* File     : example_rx_ddp.c
* Created  : October 2014
* Updated  : 2014/10/08
* Author   : Chamira Perera
* Synopsis : HostCPU Handling of received DDP responses and events and implementation of reading all DDP events
*
* This module handles all received valid DDP responses and events and it provides example code to read all DDP events
*
* Copyright 2005-2014, Audinate Pty Ltd and/or its licensors
* Confidential
* Audinate Copyright Header Version 1
*/


#include "rx_ddp.h"

#include "aud_platform.h"
#include "cobs.h"
#include "inc/packet.h"
#include "inc/audio_client.h"
#include "inc/device_client.h"
#include "inc/clocking_client.h"
#include "inc/local_client.h"
#include "inc/routing_client.h"
#include "inc/network_client.h"
#include "inc/util.h"
#include "inc/dante_common.h"
#include "uhip_structures.h"

static unsigned char g_ucUltimoOsVersion1;
static unsigned char g_ucUltimoOsVersion2;
static unsigned short g_usUltimoOsVersion3;
static unsigned int g_uiUltimoDevStatus;
static unsigned char g_ucUltimoCapVersion1;
static unsigned char g_ucUltimoCapVersion2;
static unsigned short g_usUltimoCapVersion3;
static unsigned int g_uiUltimoMfgStatus;
static unsigned int g_uiUltimoOsVersionBld;
static unsigned int g_uiUltimoCapVersionBld;
static unsigned short g_usUltimoFlags;
static unsigned char g_macDante[6];
static unsigned long int g_ipAddressDante;
static unsigned int g_uiUltimoSpeed;
static unsigned char g_bUltimoOsVersionFound = 0;
static unsigned char g_bUltimoCapVersionFound = 0;
static unsigned char g_bUltimoLinkFound = 0;
static char g_szUltimoCapModelName[64];
static dante_id64_t g_ultimoCapModelId;
static char g_szUltimoDevFriendlyName[64];
static dante_device_id_t g_ultimoDevId;
static unsigned short g_usIdentityStatus = 0;
static unsigned char g_bUltimoDevIdFound = 0;
static unsigned short g_usChannelStatus = 0;
static unsigned char g_usChannelStatusAvailable = 0;
static unsigned char g_usChannelStatusActive = 0;
static unsigned char g_usChannelMute = 1;
static unsigned char g_bChannelStatusChange = 0;
static unsigned char g_bUltimoIdentifyReceived = 0;
static unsigned long int  g_UltimoConfigIPAddress 	= 0;
static unsigned long int  g_UltimoConfigIPGateway 	= 0;
static unsigned long int  g_UltimoConfigIPMask 		= 0;
static unsigned char 	  g_UltimoConfigIPChanged 	= 0;

static unsigned char g_bUltimoRebootMessageReceived = 0;
static unsigned char g_bCodecResetRequired = 0;
static unsigned char g_bUltimoUpgradeIminent = 0;

aud_error_t handle_dante_event_audio_format(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result;
	dante_samplerate_t curr_srate, reboot_srate;
	ddp_event_timestamp_t *timestamp;

	result = ddp_read_local_audio_format(ddp_rinfo, offset, &timestamp, &curr_srate, &reboot_srate);
	
	//	Possibly should set g_bCodecResetRequired here as we might miss this notification if packet corrupt.
	//	But we don't want to reset codec if comm error resulted in another packet being interpretted as this.
	if (result != AUD_SUCCESS)
	{
		return result;
	}

#ifdef DANTE_DEBUG
	printf("UHIP:Timestamp: %u:%u\n", timestamp->seconds, timestamp->subseconds);
	printf("UHIP:Current sample rate: %u\n", curr_srate);
	printf("UHIP:Reboot sample rate: %u\n", reboot_srate);
	
#endif	//	 DANTE_DEBUG
//	printf("UHIP:!CUSTOMER TODO: handle reconfiguration of the CODEC due to sample rate change here!!!!!\n");

	g_bCodecResetRequired = 1;

	return AUD_SUCCESS;
}

aud_error_t handle_dante_event_clock_pullup(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result;
	uint16_t curr_pullup, reboot_pullup;
	ddp_event_timestamp_t *timestamp;

	result = ddp_read_local_clock_pullup(ddp_rinfo, offset, &timestamp, &curr_pullup, &reboot_pullup);
	//	Possibly should set g_bCodecResetRequired here as we might miss this notification if packet corrupt.
	//	But we don't want to reset codec if comm error resulted in another packet being interpretted as this.
	if (result != AUD_SUCCESS)
	{
		return result;
	}

#ifdef DANTE_DEBUG
	printf("UHIP:Timestamp: %u:%u\n", timestamp->seconds, timestamp->subseconds);
	printf("UHIP:Current pullup: %s\n", ddp_pullup_to_string(curr_pullup));
	printf("UHIP:Reboot pullup: %s\n", ddp_pullup_to_string(reboot_pullup));
#endif	//	 DANTE_DEBUG

//	printf("UHIP:!CUSTOMER TODO: handle reconfiguration of the CODEC due to pullup change here!!!!!\n");

	g_bCodecResetRequired = 1;

	return AUD_SUCCESS;
}

aud_error_t handle_ddp_device_general(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result;
	ddp_request_id_t request_id;
	ddp_status_t status;
	char *model_id_str = 0;
	dante_id64_t *model_id = 0;
	uint32_t sw_ver, sw_bld, fw_ver, fw_bld, btld_ver, btld_bld, api_ver, cap_flags, status_flags;

	result = ddp_read_device_general_response(ddp_rinfo, offset, &request_id, &status,
											&model_id, &model_id_str, &sw_ver, &sw_bld, &fw_ver, &fw_bld,
											&btld_ver, &btld_bld, &api_ver, &cap_flags, &status_flags);

	if (result != AUD_SUCCESS)
	{
		return result;
	}

#ifdef DANTE_DEBUG
	if (model_id)
		printf("UHIP:Model ID: 0x%02x%02x%02x%02x%02x%02x%02x%02x\n",
			model_id->data[0],
			model_id->data[1],
			model_id->data[2],
			model_id->data[3],
			model_id->data[4],
			model_id->data[5],
			model_id->data[6],
			model_id->data[7]);
	if (model_id_str) {
		printf("UHIP:Model ID string: %s\n", model_id_str);
	}
	printf("UHIP:Software version: %d.%d.%d.%d\n", sw_ver >> 24, (sw_ver >> 16) & 0xFF, sw_ver & 0xFFFF, sw_bld);

	printf("UHIP:Firmware version: %d.%d.%d.%d\n", fw_ver >> 24, (fw_ver >> 16) & 0xFF, fw_ver & 0xFFFF, fw_bld);
	printf("UHIP:Bootloader version: %d.%d.%d.%d\n", btld_ver >> 24, (btld_ver >> 16) & 0xFF, btld_ver & 0xFFFF, btld_bld);
	printf("UHIP:API version: %d\n", api_ver);
	printf("UHIP:Capability flags: 0x%08X\n", cap_flags);
	printf("UHIP:Status flags: 0x%08X\n", status_flags);
#endif	//	DANTE_DEBUG

	g_ucUltimoOsVersion1 = (unsigned char)(sw_ver >> 24);
	g_ucUltimoOsVersion2 = (unsigned char)(sw_ver >> 16);
	g_usUltimoOsVersion3 = (unsigned short)(sw_ver);
	g_uiUltimoOsVersionBld = sw_bld;
	g_uiUltimoDevStatus = status_flags;
	g_bUltimoOsVersionFound = 1;

	return AUD_SUCCESS;
}

aud_error_t handle_ddp_device_manuf_info(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result;
	ddp_request_id_t request_id;
	ddp_status_t status;
	char *manuf_str=0, *model_ver_str=0, *model_id_str=0;
	dante_id64_t *manuf_id=0, *model_id=0;
	uint32_t sw_ver, sw_bld, fw_ver, fw_bld, cap_flags, model_ver;	

	result = ddp_read_device_manufacturer_response(ddp_rinfo, offset, &request_id, &status,
												&manuf_id, &manuf_str, &model_id, &model_id_str, &sw_ver,
												&sw_bld, &fw_ver, &fw_bld, &cap_flags, &model_ver, &model_ver_str);

	if (result != AUD_SUCCESS)
	{
		return result;
	}

#ifdef DANTE_DEBUG
	if (manuf_id)
		printf("UHIP:Manufacturer ID: 0x%02x%02x%02x%02x%02x%02x%02x%02x\n",
			manuf_id->data[0],
			manuf_id->data[1],
			manuf_id->data[2],
			manuf_id->data[3],
			manuf_id->data[4],
			manuf_id->data[5],
			manuf_id->data[6],
			manuf_id->data[7]);
	if (model_id_str) {
		printf("UHIP:Manufacturer ID string: %s\n", model_id_str);
	}
	if (model_id)
	printf("UHIP:Model ID: 0x%02x%02x%02x%02x%02x%02x%02x%02x\n",
			model_id->data[0],
			model_id->data[1],
			model_id->data[2],
			model_id->data[3],
			model_id->data[4],
			model_id->data[5],
			model_id->data[6],
			model_id->data[7]);
	if (model_id_str) {
		printf("UHIP:Model ID string: %s\n", model_id_str);
	}
	printf("UHIP:Software version: %d.%d.%d.%d\n", sw_ver >> 24, (sw_ver >> 16) & 0xFF, sw_ver & 0xFFFF, sw_bld);
	printf("UHIP:Firmware version: %d.%d.%d.%d\n", fw_ver >> 24, (fw_ver >> 16) & 0xFF, fw_ver & 0xFFFF, fw_bld);
	printf("UHIP:Capability flags: 0x%08X\n", cap_flags);
	printf("UHIP:Model version: %d.%d.%d\n", (model_ver >> 24) & 0xFF, (model_ver >> 16) & 0xFF, model_ver & 0xFFFF);

	if (model_ver_str) {
		printf("UHIP:Model version string: %s\n", model_ver_str);
	}
#endif	//	DANTE_DEBUG

	if (model_id)
		g_ultimoCapModelId = *(model_id);
	if (model_id_str) 
	{
		//	Protect against bad string due to corrrupt packet
		memset(g_szUltimoCapModelName, 0, sizeof(g_szUltimoCapModelName));
		strncpy(g_szUltimoCapModelName, model_id_str, sizeof(g_szUltimoCapModelName)-1);
	}
	g_ucUltimoCapVersion1 = (unsigned char)(fw_ver >> 24);
	g_ucUltimoCapVersion2 = (unsigned char)(fw_ver >> 16);
	g_usUltimoCapVersion3 = (unsigned short)(fw_ver);
	g_uiUltimoCapVersionBld = fw_bld;
	g_bUltimoCapVersionFound = 1;

	return AUD_SUCCESS;
}

aud_error_t handle_ddp_device_upgrade(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result;
	ddp_request_id_t request_id;
	upgrade_rsp_payload_t upgradePayload;
	ddp_status_t status;

	result = ddp_read_device_upgrade_response(ddp_rinfo, offset, &request_id, &status, &upgradePayload);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

#ifdef DANTE_DEBUG
	printf("UHIP:Upgrade Valid Flags = 0x%x\n", (unsigned int)upgradePayload.valid_flags);
	printf("UHIP:Upgrade Stage = %d\n", (unsigned int)upgradePayload.upgrade_stage);
	printf("UHIP:Upgrade Error = %d\n", upgradePayload.upgrade_error);
	printf("UHIP:Upgrade Progress Current = %d\n", upgradePayload.progress_current);
	printf("UHIP:Upgrade Progress Total = %d\n", upgradePayload.progress_total);
#endif	//	DANTE_DEBUG

	if (DEVICE_UPGRADE_STAGE_GET_FILE==upgradePayload.upgrade_stage)
		g_bUltimoUpgradeIminent = 1;

	return AUD_SUCCESS;
}				

aud_error_t handle_ddp_device_erase_config(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result;
	ddp_request_id_t request_id;
	uint16_t supported_flags, executed_flags;
	ddp_status_t status;

	result = ddp_read_device_erase_response(ddp_rinfo, offset, &request_id, &status, &supported_flags, &executed_flags);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

#ifdef DANTE_DEBUG
	printf("UHIP:Supported flags: 0x%04X\n", supported_flags);
	printf("UHIP:Executed flags: 0x%04X\n", executed_flags);
#endif	//	DANTE_DEBUG

	return AUD_SUCCESS;
}

aud_error_t handle_ddp_device_reboot(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result;
	ddp_request_id_t request_id;
	uint16_t supported_flags, executed_flags;
	ddp_status_t status;

	result = ddp_read_device_reboot_response(ddp_rinfo, offset, &request_id, &status, &supported_flags, &executed_flags);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	g_bUltimoRebootMessageReceived = 1;

#ifdef DANTE_DEBUG
	printf("UHIP:Supported flags: 0x%04X\n", supported_flags);
	printf("UHIP:Executed flags: 0x%04X\n", executed_flags);
#endif	//	DANTE_DEBUG

	return AUD_SUCCESS;
}

aud_error_t handle_ddp_device_id(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result;
	ddp_request_id_t request_id;
	ddp_status_t status;
	uint16_t status_flags, process_id;	
	dante_device_id_t *device_id;
	char *default_name=0, *friendly_name=0, *domain, *advertised_name=0;

	result = ddp_read_device_identity_response(ddp_rinfo, offset, &request_id, &status, &status_flags, &process_id, 
												&device_id, (const char**)&default_name, (const char**)&friendly_name, (const char**)&domain, (const char**)&advertised_name);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

#ifdef DANTE_DEBUG
	printf("UHIP:Identity status flags: 0x%04X\n", status_flags);
	printf("UHIP:Process ID: %d\n", process_id);
	printf("UHIP:Dante device ID: 0x%llx\n", *((long long unsigned int *)&device_id));
	if (default_name) {
		printf("UHIP:Default name: %s\n", default_name);
	}
	if (friendly_name) {
		printf("UHIP:Friendly name: %s\n", friendly_name);
	}
	if (domain) {
		printf("UHIP:Dante domain: %s\n", domain);
	}
	if (advertised_name) {
		printf("UHIP:Advertised name: %s\n", advertised_name);
	}
#endif	//	DANTE_DEBUG

	g_ultimoDevId = *(device_id);
	if (friendly_name)
	{
		memset(g_szUltimoDevFriendlyName, 0, sizeof(g_szUltimoDevFriendlyName));
		strncpy(g_szUltimoDevFriendlyName,  friendly_name, sizeof(g_szUltimoDevFriendlyName)-1);
	}

	g_usIdentityStatus = status_flags;
	g_bUltimoDevIdFound = 1;

	return AUD_SUCCESS;
}
#if (defined UHIP_HOSTCPU_API_SUPPORT_DDP_DEVICE_IDENTIFY)
aud_error_t handle_ddp_device_identify(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result;
	ddp_request_id_t request_id;
	ddp_status_t status;

	result = ddp_read_device_identify_response(ddp_rinfo, offset, &request_id, &status);
	//	Possibly should set g_bUltimoIdentifyReceived here as we might miss this notification if packet corrupt.
	//	But we don't want to identify if comm error resulted in another packet being interpretted as this.
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	AUD_PRINTF("Identify message received!\n");
	g_bUltimoIdentifyReceived = 1;

	return AUD_SUCCESS;
}
#endif //UHIP_HOSTCPU_API_SUPPORT_DDP_DEVICE_IDENTIFY

#if (defined UHIP_HOSTCPU_API_SUPPORT_DDP_DEVICE_GPIO)
aud_error_t handle_ddp_device_gpio(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result;
	ddp_request_id_t request_id;
	ddp_status_t status;
	uint32_t interrupt_trigger_mask, input_state_valid_mask, input_state_values, output_state_valid_mask, output_state_values;

	result = ddp_read_device_gpio_response(ddp_rinfo, offset, &request_id, &status, &interrupt_trigger_mask, &input_state_valid_mask, &input_state_values, &output_state_valid_mask, &output_state_values);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	AUD_PRINTF("Interrupt Trigger Mask: 0x%08X\n", interrupt_trigger_mask);
	AUD_PRINTF("Input State Valid Mask: 0x%08X\n", input_state_valid_mask);
	AUD_PRINTF("Input State Values: 0x%08X\n", input_state_values);
	AUD_PRINTF("Output State Valid Mask: 0x%08X\n", output_state_valid_mask);
	AUD_PRINTF("Output State Values: 0x%08X\n", output_state_values);

	return AUD_SUCCESS;
}
#endif //UHIP_HOSTCPU_API_SUPPORT_DDP_DEVICE_GPIO

#if (defined UHIP_HOSTCPU_API_SUPPORT_DDP_DEVICE_SWITCH_LED)
aud_error_t handle_ddp_device_switch_led(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result;
	ddp_request_id_t request_id;
	ddp_status_t status;
	uint8_t switch_led_mode;

	result = ddp_read_device_switch_led_response(ddp_rinfo, offset, &request_id, &status, &switch_led_mode);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	AUD_PRINTF("Switch LED Mode: ");

	switch(switch_led_mode)
	{
		case SWITCH_LED_MODE_FORCE_OFF :
			AUD_PRINTF("Forced OFF\n");
			break;
		case SWITCH_LED_MODE_FORCE_ON :
			AUD_PRINTF("Forced ON\n");
			break;
		case SWITCH_LED_MODE_NORMAL :
			AUD_PRINTF("Normal\n");
			break;
		default:
			AUD_PRINTF("UNKNOWN!\n");
			break;
	}

	return AUD_SUCCESS;
}
#endif //UHIP_HOSTCPU_API_SUPPORT_DDP_DEVICE_SWITCH_LED

aud_error_t handle_ddp_network_basic(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result;
	ddp_message_read_info_t msg_info;
	ddp_request_id_t request_id;
	ddp_status_t status;
	uint16_t i;
	network_basic_control_t nw_basic_info;

	result = ddp_read_network_basic_response_header(ddp_rinfo, offset, &msg_info, &request_id, &status, &nw_basic_info);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

#ifdef DANTE_DEBUG
	printf("UHIP:Number of interfaces: %d\n", nw_basic_info.num_intfs);
	printf("UHIP:Number of DNS servers: %d\n", nw_basic_info.num_dns_srvrs);
#endif	//	DANTE_DEBUG

#ifdef DANTE_DEBUG	//	Don't need this info, no sense taking the risk of reading it in case it is corrupt
	//	Validity checks - Check ranges
	if (nw_basic_info.num_dns_srvrs>10)
	{
		printf("Invalid DNS count:%d!\n", (unsigned int)nw_basic_info.num_dns_srvrs);
		return(AUD_ERR_INVALIDDATA);
	}

	if (nw_basic_info.num_dns_srvrs)
	{
		uint16_t family;
		uint32_t ip_addr;

		for (i = 0; i < nw_basic_info.num_dns_srvrs; ++i)
		{
			result = ddp_read_network_basic_response_dns(&msg_info, i, &family, &ip_addr);
			if (result != AUD_SUCCESS)
			{
				return result;
			}

#ifdef DANTE_DEBUG
			printf("UHIP:DNS[%d]: Family: %d\n", i + 1, family);
			printf("UHIP:DNS[%d]: IP address: %d.%d.%d.%d\n", i + i,
				(ip_addr >> 24) & 0xFF, (ip_addr >> 16) & 0xFF, (ip_addr >> 8) & 0xFF, (ip_addr & 0xFF));
#endif	//	DANTE_DEBUG
		}
	}
#endif	//	DANTE_DEBUG

	if (nw_basic_info.num_intfs>10)
	{
		printf("Invalid interface count:%d!\n", (unsigned int)nw_basic_info.num_intfs);
		return(AUD_ERR_INVALIDDATA);
	}

	if (nw_basic_info.num_intfs)
	{
		uint16_t flags, mode, num_addrs, j;
		uint32_t speed;
		ddp_network_etheraddr_t mac_addr;

		for (i = 0; i < nw_basic_info.num_intfs; ++i)
		{
			result = ddp_read_network_basic_response_interface(&msg_info, i, &flags, &mode, &speed, &mac_addr, &num_addrs);

			if ((0!=speed) && (10!=speed) &&(100!=speed) &&(1000!=speed))
			{
				printf("Invalid link speed:%d!\n", (unsigned int)speed);
				return(AUD_ERR_INVALIDDATA);
			}
			if ((0x00!=mac_addr.addr[0]) ||  (0x1D!=mac_addr.addr[1]) || (0xC1!=mac_addr.addr[2]))
			{
				printf("Invalid Dante MAC:%02x:%02x:%02x:%02x:%02x:%02x!\n",
						mac_addr.addr[0],
						mac_addr.addr[1],
						mac_addr.addr[2],
						mac_addr.addr[3],
						mac_addr.addr[4],
						mac_addr.addr[5]);
				return(AUD_ERR_INVALIDDATA);
			}

			g_usUltimoFlags = flags;
			g_uiUltimoSpeed = speed;
			memcpy(g_macDante, mac_addr.addr, sizeof(g_macDante));

#ifdef DANTE_DEBUG
			printf("UHIP:Interface[%d]: \n", i + i);
			printf("UHIP:\tFlags: 0x%04X\n", flags);
			printf("UHIP:\tMode: 0x%d\n", mode);
			printf("UHIP:\tLink speed: %d\n", speed);
			printf("UHIP:\tMAC address: %02X:%02X:%02X:%02X:%02X:%02X\n", mac_addr.addr[0], mac_addr.addr[1],
																	mac_addr.addr[2], mac_addr.addr[3], mac_addr.addr[4], mac_addr.addr[5]);
			printf("UHIP:\tNum addresses: %d\n", num_addrs);
#endif	//	DANTE_DEBUG

			if (num_addrs>10)
			{
				printf("Invalid IP address count :%d!\n", (unsigned int)num_addrs);
				return(AUD_ERR_INVALIDDATA);
			}

			if (num_addrs)
			{
				uint16_t family;
				uint32_t ip_addr, net_mask, gateway;

				for (j = 0; j < num_addrs; ++j)
				{
					result = ddp_read_network_basic_response_interface_address(&msg_info, i, j, &family, &ip_addr, &net_mask, &gateway);
					g_ipAddressDante = ip_addr;
#ifdef DANTE_DEBUG
					printf("UHIP:\tAddress[%d]: \n", j + i);
					printf("UHIP:\t\tFamily: %d\n", family);
					printf("UHIP:\t\tIP address: %d.%d.%d.%d\n", 
						(ip_addr >> 24) & 0xFF, (ip_addr >> 16) & 0xFF, (ip_addr >> 8) & 0xFF, (ip_addr & 0xFF));
					printf("UHIP:\t\tNet mask: %d.%d.%d.%d\n", 
						(net_mask >> 24) & 0xFF, (net_mask >> 16) & 0xFF, (net_mask >> 8) & 0xFF, (net_mask & 0xFF));
					printf("UHIP:\t\tGateway: %d.%d.%d.%d\n",
						(gateway >> 24) & 0xFF, (gateway >> 16) & 0xFF, (gateway >> 8) & 0xFF, (gateway & 0xFF));
#endif	//	DANTE_DEBUG
				}
			}

			g_bUltimoLinkFound = 1;
		}
	}

#ifdef DANTE_DEBUG	//	Don't need this info, no sense taking the risk of reading it in case it is corrupt
	if (nw_basic_info.num_domains>10)
	{
		printf("Invalid domain count:%d!\n", (unsigned int)nw_basic_info.num_domains);
		return(AUD_ERR_INVALIDDATA);
	}

	if (nw_basic_info.num_domains)
	{
		char *domain;

		for (i = 0; i < nw_basic_info.num_domains; ++i)
		{
			result = ddp_read_network_basic_response_domain(&msg_info, i, &domain);
			if (domain)
			{
#ifdef DANTE_DEBUG
				printf("UHIP:Domain[%d]: %32s\n", i + 1, domain);
#endif	//	DANTE_DEBUG
			}
		}
	}
#endif	//	DANTE_DEBUG

	return AUD_SUCCESS;
}

aud_error_t handle_ddp_network_config(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result;
	ddp_request_id_t request_id;
	ddp_status_t status;
	uint8_t curr_mode, reboot_mode, change_flags;
	network_ip_config_t ip_config = {0};
	network_dns_config_t dns_config = {0};
	char *domain;

	result = ddp_read_network_config_response(ddp_rinfo, offset, &request_id, &status, &change_flags, &curr_mode, 
											&reboot_mode, &ip_config, &dns_config, &domain);
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	g_UltimoConfigIPChanged = 1;
	g_UltimoConfigIPAddress = ip_config.ip_addr;
	g_UltimoConfigIPGateway = ip_config.gateway;
	g_UltimoConfigIPMask	= ip_config.subnet_mask;
#ifdef DANTE_DEBUG
	printf("UHIP:Change flags: 0x%02X\n", change_flags);
	printf("UHIP:Current mode: %d\n", curr_mode);
	printf("UHIP:Reboot mode: %d\n", reboot_mode);
	if (ip_config.ip_addr)
	{
		printf("UHIP:Reboot IP address: %d.%d.%d.%d\n", (ip_config.ip_addr >> 24) & 0xFF, (ip_config.ip_addr >> 16) & 0xFF,
			(ip_config.ip_addr >> 8) & 0xFF, ip_config.ip_addr & 0xFF);
	}
	if (ip_config.subnet_mask)
	{
		printf("UHIP:Reboot subnet mask: %d.%d.%d.%d\n", (ip_config.subnet_mask >> 24) & 0xFF, (ip_config.subnet_mask >> 16) & 0xFF,
			(ip_config.subnet_mask >> 8) & 0xFF, ip_config.subnet_mask & 0xFF);
	}
	if (ip_config.gateway)
	{
		printf("UHIP:Reboot gateway: %d.%d.%d.%d\n", (ip_config.gateway >> 24) & 0xFF, (ip_config.gateway >> 16) & 0xFF,
			(ip_config.gateway >> 8) & 0xFF, ip_config.gateway & 0xFF);
	}
	if (dns_config.ip_addr)
	{
		printf("UHIP:Reboot static DNS server: %d.%d.%d.%d\n", (dns_config.ip_addr >> 24) & 0xFF, (dns_config.ip_addr >> 16) & 0xFF,
			(dns_config.ip_addr >> 8) & 0xFF, dns_config.ip_addr & 0xFF);
	}
	if (domain)
	{
		printf("UHIP:Reboot static domain: %32s\n", domain);
	}
#endif	//	DANTE_DEBUG

	return AUD_SUCCESS;
}

aud_error_t handle_ddp_clock_basic(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result = AUD_SUCCESS;
	ddp_request_id_t request_id;
	ddp_status_t status;
	uint8_t clock_state, mute_state, ext_wc_state, preferred;
	uint32_t cap_flags, drift;

	result = ddp_read_clock_basic_response(ddp_rinfo, offset, &request_id, &status, &cap_flags, &clock_state, 
										&mute_state, &ext_wc_state, &preferred, &drift);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	g_usChannelMute = mute_state;

	g_bChannelStatusChange = 1;
#ifdef DANTE_DEBUG
	printf("UHIP:Capability flags: 0x%08X\n", cap_flags);
	printf("UHIP:Clock state: %d\n", clock_state);
	printf("UHIP:Mute state: %d\n", mute_state);
	printf("UHIP:External word clock state: %d\n", ext_wc_state);
	printf("UHIP:Preferred: %d\n", preferred);
	printf("UHIP:Drift: %d\n", drift);
#else
	//printf("UHIP:Clock flag 0x%04X state %d mute %d ext clock %d pref %d drift %d\n", cap_flags, clock_state, mute_state, ext_wc_state, preferred, drift);
#endif	//	DANTE_DEBUG

	return AUD_SUCCESS;
}

aud_error_t handle_ddp_clock_config(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result;
	ddp_request_id_t request_id;
	ddp_status_t status;
	uint8_t preferred, wcs, logging;

	result = ddp_read_clock_config_response(ddp_rinfo, offset, &request_id, &status, &preferred, &wcs, &logging);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

#ifdef DANTE_DEBUG
	printf("UHIP:Preferred: %d\n", preferred);
	printf("UHIP:Word clock sync: %d\n", wcs);
	printf("UHIP:Logging: %d\n", logging);
#endif	//	DANTE_DEBUG

	return AUD_SUCCESS;
}

aud_error_t handle_ddp_clock_pullup(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	AUD_UNUSED(ddp_rinfo);
	AUD_UNUSED(offset);
	return AUD_SUCCESS;
}

aud_error_t handle_ddp_audio_basic(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result = AUD_SUCCESS;
	ddp_request_id_t request_id;
	ddp_status_t status;
	uint8_t change_flags;
	uint16_t default_enc, rx_chans, tx_chans;
	uint32_t cap_flags, default_srate;

	result = ddp_read_audio_basic_response(ddp_rinfo, offset, &request_id, &status, &cap_flags, &default_srate, 
										&default_enc, &rx_chans, &tx_chans, &change_flags);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

#ifdef DANTE_DEBUG
	printf("UHIP:Capability flags: 0x%08X\n", cap_flags);
	printf("UHIP:Default sample rate: %d\n", default_srate);
	printf("UHIP:Default encoding: %d\n", default_enc);
	printf("UHIP:Rx chans: %d\n", rx_chans);
	printf("UHIP:Tx chans: %d\n", tx_chans);
	printf("UHIP:Change flags: %d\n", change_flags);
#endif	//	DANTE_DEBUG

	return AUD_SUCCESS;
}

aud_error_t handle_ddp_audio_srate(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result = AUD_SUCCESS;
	ddp_message_read_info_t msg_info;
	ddp_request_id_t request_id;
	ddp_status_t status;
	uint32_t current_srate, reboot_srate;
	uint16_t num_supported_srates, i;
	dante_samplerate_t supported_srate;

	result = ddp_read_audio_sample_rate_config_response(ddp_rinfo, offset, &msg_info, &request_id, &status,
													&current_srate, &reboot_srate, &num_supported_srates);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

#ifdef DANTE_DEBUG
	printf("UHIP:Current sample rate: %d\n", current_srate);
	printf("UHIP:Reboot sample rate: %d\n", reboot_srate);
	printf("UHIP:Num supported sample rates: %d\n", num_supported_srates);
#endif	//	DANTE_DEBUG

#ifdef DANTE_DEBUG	//	Don't need this info, no sense taking the risk of reading it in case it is corrupt
	if (num_supported_srates>16)
	{
		printf("Invalid supported sample rates:%d!\n", (unsigned int)num_supported_srates);
		return(AUD_ERR_INVALIDDATA);
	}

	if (num_supported_srates)
	{
		for (i = 0; i < num_supported_srates; ++i)
		{
			result = ddp_read_audio_sample_rate_config_supported_srate(&msg_info, i, &supported_srate);
#ifdef DANTE_DEBUG
			printf("UHIP:Supported sample rate[%d]: %d\n", i + 1, supported_srate);
#endif	//	DANTE_DEBUG
		}
	}
#endif	//	DANTE_DEBUG

	return AUD_SUCCESS;
}

aud_error_t handle_ddp_audio_enc(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result = AUD_SUCCESS;
	ddp_message_read_info_t msg_info;
	ddp_request_id_t request_id;
	ddp_status_t status;
	uint16_t current_enc, reboot_enc, num_supported_encs, i, supported_enc;

	result = ddp_read_audio_encoding_config_response(ddp_rinfo, offset, &msg_info, &request_id, &status,
													&current_enc, &reboot_enc, &num_supported_encs);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

#ifdef DANTE_DEBUG
	printf("UHIP:Current encoding: %d\n", current_enc);
	printf("UHIP:Reboot encoding: %d\n", reboot_enc);
	printf("UHIP:Num supported encodings: %d\n", num_supported_encs);
#endif	//	DANTE_DEBUG

#ifdef DANTE_DEBUG	//	Don't need this info, no sense taking the risk of reading it in case it is corrupt
	if (num_supported_encs>10)	//	Check range
	{
		printf("Invalid supported encoders:%d!\n", (unsigned int)num_supported_encs);
		return(AUD_ERR_INVALIDDATA);
	}

	for (i = 0; i < num_supported_encs; ++i)
	{
		result = ddp_read_audio_encoding_config_supported_encoding(&msg_info, i, &supported_enc);
#ifdef DANTE_DEBUG
		printf("UHIP:Supported encoding[%d]: %d\n", i + 1, supported_enc);
#endif	//	DANTE_DEBUG
	}
#endif //	DANTE_DEBUG

	return AUD_SUCCESS;
}

aud_error_t handle_ddp_routing_basic(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	AUD_UNUSED(ddp_rinfo);
	AUD_UNUSED(offset);
	return AUD_SUCCESS;
}

aud_error_t handle_ddp_routing_ready_state(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result;
	ddp_request_id_t request_id;
	ddp_status_t status;
	uint8_t network_ready_state, routing_ready_state;

	result = ddp_read_routing_ready_state_response(ddp_rinfo, offset, &request_id, &status, &network_ready_state, &routing_ready_state);

	if (result != AUD_SUCCESS) 
	{
		return result;
	}

#ifdef DANTE_DEBUG
	printf("UHIP:Network ready state: %d\n", network_ready_state);
	printf("UHIP:Routing ready state: %d\n", routing_ready_state);
#endif	//	DANTE_DEBUG

	return AUD_SUCCESS;
}

aud_error_t handle_ddp_routing_performance_config(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result;
	ddp_request_id_t request_id;
	ddp_status_t status;
	uint16_t unicast_fpp, multicast_fpp;
	uint32_t unicast_latency, multicast_latency;

	result = ddp_read_routing_performance_config_response(ddp_rinfo, offset, &request_id, &status,
														&unicast_fpp, &unicast_latency, &multicast_fpp, &multicast_latency);

	if (result != AUD_SUCCESS) 
	{
		return result;
	}

#ifdef DANTE_DEBUG
	printf("UHIP:Unicast fpp: %d\n", unicast_fpp);
	printf("UHIP:Unicast latency %d \n", unicast_latency);
	printf("UHIP:Multicast fpp %d\n", multicast_fpp);
	printf("UHIP:Multicast latency %d\n", multicast_latency);
#endif	//	DANTE_DEBUG

	return AUD_SUCCESS;
}

aud_error_t handle_ddp_routing_rx_chan_config_state(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result;
	ddp_request_id_t request_id;
	ddp_status_t status;
	uint16_t i, num_chans, num_custom_encs;
	ddp_rx_chan_state_params_t rx_chan_state;
	ddp_message_read_info_t message_info;

	result = ddp_read_routing_rx_chan_config_state_response_header(ddp_rinfo, offset, &message_info, &request_id, &status, &num_chans);
	if (result != AUD_SUCCESS)
	{		
		return result;
	}

	if (num_chans>2)	//	Check range
	{
		printf("Invalid channel count:%d!\n", (unsigned int)num_chans);
		return(AUD_ERR_INVALIDDATA);
	}

	for (i = 0; i < num_chans; ++i)
	{
		result = ddp_read_routing_rx_chan_config_state_response_chan_params(&message_info, i, &rx_chan_state, &num_custom_encs);
		if (result != AUD_SUCCESS)
		{
			return result;
		}

		if (i==0)
		{
			//	Check for invalid states
			if ((5==rx_chan_state.status) ||
				(6==rx_chan_state.status) ||
				(11==rx_chan_state.status) ||
				(12==rx_chan_state.status) ||
				(13==rx_chan_state.status) ||
				((34<=rx_chan_state.status) && (63>=rx_chan_state.status)) ||
				((69<=rx_chan_state.status) && (254>=rx_chan_state.status)))
			{
				printf("Invalid channel status:%d!\n", (unsigned int)rx_chan_state.status);
//				return(AUD_ERR_INVALIDDATA);
			}
			if (48000!=rx_chan_state.sample_rate)
			{
				printf("Invalid sample rate:%d!\n", (unsigned int)rx_chan_state.sample_rate);
				return(AUD_ERR_INVALIDDATA);
			}
			if (4<rx_chan_state.flow_id)
			{
				printf("Invalid flow ID:%d!\n", (unsigned int)rx_chan_state.flow_id);
				return(AUD_ERR_INVALIDDATA);
			}
			if (4<rx_chan_state.slot_id)
			{
				printf("Invalid slot ID:%d!\n", (unsigned int)rx_chan_state.slot_id);
				return(AUD_ERR_INVALIDDATA);
			}
			if ((8!=rx_chan_state.encoding) &&
				(12!=rx_chan_state.encoding) &&
				(16!=rx_chan_state.encoding) &&
				(20!=rx_chan_state.encoding) &&
				(24!=rx_chan_state.encoding) &&
				(32!=rx_chan_state.encoding))
			{
				printf("Invalid bit width:%d!\n", (unsigned int)rx_chan_state.encoding);
				return(AUD_ERR_INVALIDDATA);
			}

			g_usChannelStatus = rx_chan_state.status;
			g_usChannelStatusAvailable = rx_chan_state.avail;
			g_usChannelStatusActive = rx_chan_state.active;
			g_bChannelStatusChange = 1;
		}

#ifdef DANTE_DEBUG
		printf("UHIP:Channel Id %d: Flags 0x%04x ", rx_chan_state.channel_id, rx_chan_state.channel_flags);
		printf("rate=%d enc=%d pcm_map=0x%04x  ", rx_chan_state.sample_rate, rx_chan_state.encoding, rx_chan_state.pcm_map);
		printf("status=%d avail=%d active=%d  \n", rx_chan_state.status, rx_chan_state.avail, rx_chan_state.active);
#if 0
		if (rx_chan_state.channel_name)
		{
			printf(" Name=%63s", rx_chan_state.channel_name);
		}
		else
		{
			printf(" Name=''");
		}
		if (rx_chan_state.channel_label)
		{
			printf(" Label %63s", rx_chan_state.channel_label);
		}
		else
		{
			printf(" Label=''");
		}

		printf(" in Flow Id %d slot id %d status=0x%04x ", rx_chan_state.flow_id, rx_chan_state.slot_id, rx_chan_state.status);


		if (rx_chan_state.subscribed_channel)
		{
			printf("sub=%63s", rx_chan_state.subscribed_channel);
		}
		else
		{
			printf("sub='' ");
		}
		if (rx_chan_state.subcribed_device)
		{
//			printf("@%s ", rx_chan_state.subcribed_device);
		}
#endif  // 0
#endif	//	DANTE_DEBUG

#ifdef DANTE_DEBUG	//	Don't need this info, no sense taking the risk of reading it in case it is corrupt
		if (num_custom_encs>16)
		{
			printf("Invalid custom encoder count:%d!\n", (unsigned int)num_custom_encs);
			return(AUD_ERR_INVALIDDATA);
		}

		if (num_custom_encs != 0)
		{
#ifdef DANTE_DEBUG
			printf("Custom Encs= ");
#endif	//	DANTE_DEBUG

			uint16_t j, custom_enc = 0;
			for (j = 0; j < num_custom_encs; ++j)
			{
				result = ddp_read_routing_rx_chan_config_state_response_custom_encoding(&message_info, i, j, &custom_enc);
				if (result != AUD_SUCCESS)
				{
					return result;
				}
#ifdef DANTE_DEBUG
				printf("%u ", custom_enc);
#endif	//	DANTE_DEBUG
			}
		}
#endif	//	DANTE_DEBUG

#ifdef DANTE_DEBUG
		printf("\n");
#endif	//	DANTE_DEBUG
	}

	return AUD_SUCCESS;
}

aud_error_t handle_ddp_routing_rx_subscribe_set(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	AUD_UNUSED(ddp_rinfo);
	AUD_UNUSED(offset);
	return AUD_SUCCESS;
}

aud_error_t handle_ddp_routing_rx_chan_label_set(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	AUD_UNUSED(ddp_rinfo);
	AUD_UNUSED(offset);
	return AUD_SUCCESS;
}

aud_error_t handle_ddp_routing_tx_chan_config_state(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result;
	ddp_request_id_t request_id;
	ddp_status_t status;
	uint16_t num_chans, i, num_custom_encs;
	ddp_tx_chan_state_params_t tx_chan_state;
	ddp_message_read_info_t message_info;

	result = ddp_read_routing_tx_chan_config_state_response_header(ddp_rinfo, offset, &message_info, &request_id, &status, &num_chans);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

#ifdef DANTE_DEBUG	//	Don't need this info, no sense taking the risk of reading it in case it is corrupt
	if (num_chans>2)
	{
		printf("Invalid channel count:%d!\n", (unsigned int)num_chans);
		return(AUD_ERR_INVALIDDATA);
	}
	if (num_custom_encs>16)
	{
		printf("Invalid custom encoders count:%d!\n", (unsigned int)num_custom_encs);
		return(AUD_ERR_INVALIDDATA);
	}

	for (i = 0; i < num_chans; ++i)
	{
		result = ddp_read_routing_tx_chan_config_state_response_chan_params(&message_info, i, &tx_chan_state, &num_custom_encs);
		if (result != AUD_SUCCESS)
		{
			return result;
		}

#ifdef DANTE_DEBUG
		printf("UHIP:Channel Id %d: flags=0x%04x", tx_chan_state.channel_id, tx_chan_state.channel_flags);
		printf(" rate=%d  enc=%d pcm map=0x%04x  ", tx_chan_state.sample_rate, tx_chan_state.encoding, tx_chan_state.pcm_map);

		if (tx_chan_state.channel_name)
		{
			printf(" Name=%63s", tx_chan_state.channel_name);
		}
		else
		{
			printf(" Name=''");
		}
		if (tx_chan_state.channel_label)
		{
			printf("Label=%63s ", tx_chan_state.channel_label);
		}
		else
		{
			printf("Label='' ");
		}
#endif	//	DANTE_DEBUG

		if (num_custom_encs != 0)
		{
#ifdef DANTE_DEBUG
			printf("Custom Encs= ");
#endif	//	DANTE_DEBUG

			uint16_t j, custom_enc = 0;
			for (j = 0; j < num_custom_encs; ++j)
			{
				result = ddp_read_routing_tx_chan_config_state_response_custom_encoding(&message_info, i, j, &custom_enc);
				if (result != AUD_SUCCESS)
				{
					return result;
				}
#ifdef DANTE_DEBUG
				printf("%u ", custom_enc);
#endif	//	DANTE_DEBUG
			}
		}
#ifdef DANTE_DEBUG
		printf("\n");
#endif	//	DANTE_DEBUG
	}
#endif	//	DANTE_DEBUG

	return AUD_SUCCESS;
}

aud_error_t handle_ddp_routing_tx_chan_label_set(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	AUD_UNUSED(ddp_rinfo);
	AUD_UNUSED(offset);
	return AUD_SUCCESS;
}

aud_error_t handle_ddp_routing_rx_flow_config_state(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result;
	ddp_request_id_t request_id;
	ddp_status_t status;
	uint16_t i, j, num_flows;
	ddp_rx_flow_state_params_t rx_flow_state;
	uint16_t channels[4];
	ddp_message_read_info_t message_info;
	
	result = ddp_read_routing_rx_flow_config_state_response_header(ddp_rinfo, offset, &message_info, &request_id, &status, &num_flows);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

//#ifdef DANTE_DEBUG	//	Don't need this info, no sense taking the risk of reading it in case it is corrupt
	if (num_flows>4)
	{
		printf("Invalid rx flow count:%d!\n", (unsigned int)num_flows);
		return(AUD_ERR_INVALIDDATA);
	}
	


	for (i = 0; i < num_flows; ++i)		//&&&&gm is num_flows 1?
	{
		uint16_t num_slots, num_addrs;

		result = ddp_read_routing_rx_flow_config_state_response_flow_params(&message_info, i, &num_slots, &num_addrs, &rx_flow_state);
		if (result != AUD_SUCCESS)
		{
			return result;
		}
		if (i==0)
		{
			g_usChannelStatus = rx_flow_state.status;
			g_usChannelStatusAvailable = rx_flow_state.avail_mask;	// this assumes only ONE flow... we should have a local variable and do at end of all num_flows
			g_usChannelStatusActive = rx_flow_state.active_mask;
			g_bChannelStatusChange = 1;
		}


#ifdef DANTE_DEBUG
		char * flow_label = "";
		if (rx_flow_state.flow_label)
		{
			flow_label = rx_flow_state.flow_label;
		}
		printf("UHIP: %d: '%s' flags=%04x rate=%d enc=%u",
			rx_flow_state.flow_id, flow_label, rx_flow_state.flow_flags, rx_flow_state.sample_rate, rx_flow_state.encoding);

		printf(" chans=[(%d)", num_slots);
#endif	//	DANTE_DEBUG

		if (num_slots>4)
		{
			printf("Invalid rx slot count:%d!\n", (unsigned int)num_slots);
			return(AUD_ERR_INVALIDDATA);
		}

#ifdef DANTE_DEBUG	//	Don't need this info, no sense taking the risk of reading it in case it is corrupt

		if (num_slots)
		{
			uint16_t num_chans;

			for (j = 0; j < num_slots; ++j)
			{
				result = ddp_read_routing_rx_flow_config_state_response_flow_slot(&message_info, i, j, &num_chans);
				if (result != AUD_SUCCESS)
				{
					return result;
				}

				if (num_chans>2)
				{
					printf("Invalid rx channel count:%d!\n", (unsigned int)num_chans);
					return(AUD_ERR_INVALIDDATA);
				}

				if (num_chans >= 1)
				{
					uint16_t k;
					
					result = ddp_read_routing_rx_flow_config_state_response_flow_slot_chans(&message_info, i, j, channels);
					if (result != AUD_SUCCESS)
					{
						return result;
					}

#ifdef DANTE_DEBUG
					printf(" {");
					for (k = 0; k < num_chans; ++k)
					{
						if (k > 0)
						{
							printf(",");
						}
						printf("%d", channels[k]);
					}
					printf("}");
#endif	//	DANTE_DEBUG
				}
				else
				{
#ifdef DANTE_DEBUG
					printf(" {0}");
#endif	//	DANTE_DEBUG
				}
			}
		}
#endif	//	DANTE_DEBUG

#ifdef DANTE_DEBUG
		printf("]");
#endif	//	DANTE_DEBUG

		uint16_t port;
		uint32_t ip_addr;

#ifdef DANTE_DEBUG
		printf(" addrs=[(%d)", num_addrs);

		if (num_addrs>10)
		{
			printf("Invalid IP Address count:%d!\n", (unsigned int)num_addrs);
			return(AUD_ERR_INVALIDDATA);
		}

#endif	//	DANTE_DEBUG
		for (j = 0; j < num_addrs; ++j)
		{
			result = ddp_read_routing_rx_flow_config_state_response_flow_address(&message_info, i, j, &ip_addr, &port);
			if (result != AUD_SUCCESS)
			{
				return result;
			}

#ifdef DANTE_DEBUG
			if (ip_addr && port)
			{
				printf(" %d.%d.%d.%d:%d", (ip_addr >> 24) & 0xFF, (ip_addr >> 16) & 0xFF, (ip_addr >> 8) & 0xFF, (ip_addr & 0xFF), port);
			}
			else
			{
				printf(" <none>");
			}

			printf("]");
#endif	//	DANTE_DEBUG
		}
#ifdef DANTE_DEBUG
		printf("UHIP: Flow Config");

		printf(" status=%d 0x%x|0x%x l=%d user=%04x|%04x", rx_flow_state.status, rx_flow_state.avail_mask, 
			rx_flow_state.active_mask, rx_flow_state.latency, rx_flow_state.user_conf_flags, rx_flow_state.user_conf_mask);
		printf("\n");
#endif	//	DANTE_DEBUG
	}
//#endif	//	DANTE_DEBUG

	return AUD_SUCCESS;
}

aud_error_t handle_ddp_routing_multicast_tx_flow_config(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	AUD_UNUSED(ddp_rinfo);
	AUD_UNUSED(offset);
	return AUD_SUCCESS;
}

aud_error_t handle_ddp_routing_tx_flow_config_state(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result;
	ddp_request_id_t request_id;
	ddp_status_t status;
	ddp_tx_flow_state_params_t tx_flow_state;
	uint16_t num_flows, i, j;
	uint16_t slots[4];
	ddp_message_read_info_t message_info;

	result = ddp_read_routing_tx_flow_config_state_response_header(ddp_rinfo, offset, &message_info, &request_id, &status, &num_flows);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

#ifdef DANTE_DEBUG	//	Don't need this info, no sense taking the risk of reading it in case it is corrupt
	if (num_flows>4)
	{
		printf("Invalid tx flow count:%d!\n", (unsigned int)num_flows);
		return(AUD_ERR_INVALIDDATA);
	}

	for (i = 0; i < num_flows; ++i)
	{
		uint16_t num_slots, num_addrs;

		result = ddp_read_routing_tx_flow_config_state_response_flow_params(&message_info, i, &num_slots, &num_addrs, &tx_flow_state);
		if (result != AUD_SUCCESS)
		{
			return result;
		}

		char * flow_label = "";
		if (tx_flow_state.flow_label)
		{
			flow_label = tx_flow_state.flow_label;
		}

#ifdef DANTE_DEBUG
		printf("UHIP: %d: '%s' flags=%04x rate=%d enc=%u",
			tx_flow_state.flow_id, flow_label, tx_flow_state.flow_flags, tx_flow_state.sample_rate, tx_flow_state.encoding);

		printf(" chans=[(%d)", num_slots);
#endif	//	DANTE_DEBUG

		if (num_slots>4)
		{
			printf("Invalid tx slot count:%d!\n", (unsigned int)num_slots);
			return(AUD_ERR_INVALIDDATA);
		}

		if (num_slots)
		{
			result = ddp_read_routing_tx_flow_config_state_response_flow_slots(&message_info, i, slots);
			if (result != AUD_SUCCESS)
			{
				return result;
			}

#ifdef DANTE_DEBUG
			for (j = 0; j < num_slots; ++j)
			{
				if (slots[j])
				{
					printf(" %u", slots[j]);
				}
				else
				{
					printf(" ?");
				}
			}
#endif	//	DANTE_DEBUG
		}
#ifdef DANTE_DEBUG
		putchar(']');
#endif	//	DANTE_DEBUG

		uint16_t port;
		uint32_t ip_addr;

#ifdef DANTE_DEBUG
		printf(" addrs=[(%d)", num_addrs);
#endif	//	DANTE_DEBUG

		if (num_addrs>10)
		{
			printf("Invalid tx IP address count:%d!\n", (unsigned int)num_addrs);
			return(AUD_ERR_INVALIDDATA);
		}

		for (j = 0; j < num_addrs; ++j)
		{
			result = ddp_read_routing_tx_flow_config_state_response_flow_address(&message_info, i, j, &ip_addr, &port);
			if (result != AUD_SUCCESS)
			{
				return result;
			}

#ifdef DANTE_DEBUG
			if (ip_addr && port)
			{
				printf(" %d.%d.%d.%d:%d", (ip_addr >> 24) & 0xFF, (ip_addr >> 16) & 0xFF, (ip_addr >> 8) & 0xFF, (ip_addr & 0xFF), port);
			}
			else
			{
				printf(" <none>");
			}

			printf("]");
#endif	//	DANTE_DEBUG
		}
#ifdef DANTE_DEBUG
		printf("]");

		printf(" active=0x%02x fpp=%d ", tx_flow_state.active_mask & tx_flow_state.avail_mask, tx_flow_state.fpp);
		if (tx_flow_state.peer_device_label && tx_flow_state.peer_flow_label)
		{
			printf("to='%63s@%63s' ", tx_flow_state.peer_flow_label, tx_flow_state.peer_device_label);
			printf("\n peer device id: process id %d device id %04x %04x user=%04x|%04x\n",
				tx_flow_state.peer_process_id, tx_flow_state.peer_device_id[0], tx_flow_state.peer_device_id[1],
				tx_flow_state.user_conf_flags, tx_flow_state.user_conf_mask);
		}
		printf("\n");
#endif	//	DANTE_DEBUG
	}
#endif	//	DANTE_DEBUG

	return AUD_SUCCESS;
}

aud_error_t handle_ddp_routing_rx_chan_status(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result;
	ddp_request_id_t request_id;
	ddp_status_t status;
	uint16_t i;
	uint16_t num_chans, channel_id, chan_status;
	uint8_t avail, active;
	ddp_message_read_info_t message_info;

	result = ddp_read_routing_rx_chan_status_response_header(ddp_rinfo, offset, &message_info, &request_id, &status, &num_chans);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

//#ifdef DANTE_DEBUG	//	Don't need this info, no sense taking the risk of reading it in case it is corrupt
	if (num_chans>2)
	{
		printf("Invalid tx channel count:%d!\n", (unsigned int)num_chans);
		return(AUD_ERR_INVALIDDATA);
	}

	for (i = 0; i < num_chans; ++i)
	{
		result = ddp_read_routing_rx_chan_status_response_chan_params(&message_info, i, &channel_id, &chan_status, &avail, &active);
		if (result != AUD_SUCCESS)
		{
			return result;
		}
		if (i==0)
		{
			g_usChannelStatus = chan_status;
			g_usChannelStatusAvailable = avail;
			g_usChannelStatusActive = active;
			g_bChannelStatusChange = 1;
		}


#ifdef DANTE_DEBUG
		printf("UHIP:Channel %u status %u 0x%x|0x%x\n", channel_id, chan_status, avail, active);
#endif	//	DANTE_DEBUG
	}
//#endif	//	DANTE_DEBUG

	return AUD_SUCCESS;
}

aud_error_t handle_ddp_routing_rx_flow_status(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	aud_error_t result;
	ddp_request_id_t request_id;
	ddp_status_t status;
	uint16_t i;
	uint16_t num_flows, flow_id, flow_status;
	uint8_t avail, active;
	ddp_message_read_info_t message_info;

	result = ddp_read_routing_rx_flow_status_response_header(ddp_rinfo, offset, &message_info, &request_id, &status, &num_flows);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

//#ifdef DANTE_DEBUG	//	Don't need this info, no sense taking the risk of reading it in case it is corrupt
	if (num_flows>4)
	{
		printf("Invalid rx flow count:%d!\n", (unsigned int)num_flows);
		return(AUD_ERR_INVALIDDATA);
	}

	for (i = 0; i < num_flows; ++i)
	{
		result = ddp_read_routing_rx_chan_status_response_chan_params(&message_info, i, &flow_id, &flow_status, &avail, &active);
		if (result != AUD_SUCCESS)
		{
			return result;
		}

		if (i==0)
		{
			// do we assume num_flows = 1
			g_usChannelStatus = flow_status;
			g_usChannelStatusAvailable = avail;
			g_usChannelStatusActive = active;
			g_bChannelStatusChange = 1;
		}

#ifdef DANTE_DEBUG
		printf("UHIP:Flow %u status %u 0x%x|0x%x\n", flow_id, flow_status, avail, active);
#endif	//	DANTE_DEBUG
	}
//#endif	//	DANTE_DEBUG

	return AUD_SUCCESS;
}

aud_error_t handle_ddp_routing_rx_unsub_chan(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	AUD_UNUSED(ddp_rinfo);
	AUD_UNUSED(offset);
	return AUD_SUCCESS;
}

aud_error_t handle_ddp_routing_flow_delete(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset)
{
	AUD_UNUSED(ddp_rinfo);
	AUD_UNUSED(offset);
	return AUD_SUCCESS;
}


unsigned char handle_ddp_GetCapVersion(unsigned char *v1, unsigned char *v2, unsigned short *v3, unsigned int *bld, char *pszModelName, int nMaxModelNameSize, dante_id64_t *pModelId)
{
	unsigned char bChanged = g_bUltimoCapVersionFound;

	if (g_bUltimoCapVersionFound)
	{
		if (v1)
			*v1 = g_ucUltimoCapVersion1;
		if (v2)
			*v2 = g_ucUltimoCapVersion2;
		if (v3)
			*v3 = g_usUltimoCapVersion3;
		if (bld)
			*bld = g_uiUltimoCapVersionBld;
		int nLen = strlen(pszModelName);
		if( nLen> (nMaxModelNameSize-1))
			nLen = nMaxModelNameSize-1;
		if (pszModelName)
		{
			memcpy(pszModelName, g_szUltimoCapModelName, nLen);
			*(pszModelName+nLen) = 0;
		}
		if (pModelId)
			*(pModelId) = g_ultimoCapModelId;
		g_bUltimoCapVersionFound = 0;
	}
	else
	{
		if (v1)
			*v1 = 0;
		if (v2)
			*v2 = 0;
		if (v3)
			*v3 = 0;
		if (bld)
			*bld = 0;
		if (pszModelName)
			memset(pszModelName, 0, nMaxModelNameSize);
		if (pModelId)
			memset(pModelId, 0, sizeof(dante_id64_t));
	}
	return(bChanged?1:0);
}

unsigned char handle_ddp_GetOsVersion(unsigned char *v1, unsigned char *v2, unsigned short *v3, unsigned int *bld, unsigned int *pDevStatus)
{
	unsigned char bChanged = g_bUltimoOsVersionFound;

	if (g_bUltimoOsVersionFound)
	{
		if (v1)
			*v1 = g_ucUltimoOsVersion1;
		if (v2)
			*v2 = g_ucUltimoOsVersion2;
		if (v3)
			*v3 = g_usUltimoOsVersion3;
		if (bld)
			*bld = g_uiUltimoOsVersionBld;
		if (pDevStatus)
			*(pDevStatus) = g_uiUltimoDevStatus;
		g_bUltimoOsVersionFound = 0;
	}
	else
	{
		if (v1)
			*v1 = 0;
		if (v2)
			*v2 = 0;
		if (v3)
			*v3 = 0;
		if (bld)
			*bld = 0;
		if (pDevStatus)
			*(pDevStatus) = 0;
	}
	return(bChanged?1:0);
}


unsigned char handle_ddp_GetDevId(char *psFriendlyName, int nMaxFriendlyNameSize, dante_device_id_t *pDevId, unsigned short *pIdentityStatus)
{
	unsigned char bChanged = g_bUltimoDevIdFound;
	if (g_bUltimoDevIdFound)
	{
		if (psFriendlyName)
		{
			int nLen = strlen(g_szUltimoDevFriendlyName);
			if( nLen > (nMaxFriendlyNameSize-1))
				nLen = nMaxFriendlyNameSize-1;
			memcpy(psFriendlyName, g_szUltimoDevFriendlyName, nLen);
			*(psFriendlyName+nLen) = 0;
		}
		if (pDevId)
			*(pDevId) = g_ultimoDevId;
		if (pIdentityStatus)
			*(pIdentityStatus) = g_usIdentityStatus;
		g_bUltimoDevIdFound = 0;
	}
	else
	{
		if (psFriendlyName)
			memset(psFriendlyName, 0, nMaxFriendlyNameSize);
		if (pDevId)
			memset(pDevId, 0, sizeof(dante_device_id_t));
		if (pIdentityStatus)
			*(pIdentityStatus) = 0;
	}
	return(bChanged?1:0);
}

unsigned char handle_ddp_Identfy()
{
	unsigned char bChanged = g_bUltimoIdentifyReceived;
	g_bUltimoIdentifyReceived = 0;
	return(bChanged?1:0);
}

unsigned char handle_ddp_GetLinkInfo(unsigned short *pFlags, unsigned int *pSpeed, unsigned char *pMac, unsigned long* pIpAddress)
{
	unsigned char bChanged = g_bUltimoLinkFound;
	if (g_bUltimoLinkFound)
	{
		if (pFlags)
			*pFlags = g_usUltimoFlags;
		if (pSpeed)
			*pSpeed = g_uiUltimoSpeed;
		if (pMac)
			memcpy(pMac, g_macDante, sizeof(g_macDante));
		if(pIpAddress)
			*pIpAddress = g_ipAddressDante;
		g_bUltimoLinkFound = 0;
	}
	else
	{
		if (pFlags)
			*pFlags = 0;
		if (pSpeed)
			*pSpeed = 0;
	}
	return(bChanged?1:0);
}

unsigned char handle_ddp_GetChannelStatus(unsigned short *pChannelStatus, unsigned char *pChannelAvail, unsigned char *pChannelActive, unsigned char *pChannelMute)
{
	unsigned char bChanged = g_bChannelStatusChange;
	if (g_bChannelStatusChange)
	{
		if (pChannelStatus)
			*pChannelStatus = g_usChannelStatus;

		if (pChannelAvail)
			*pChannelAvail = g_usChannelStatusAvailable & 0x01;

		if (pChannelActive)
			*pChannelActive = g_usChannelStatusActive & 0x01;

		if (pChannelMute)
			*pChannelMute = g_usChannelMute;

		g_bChannelStatusChange = 0;
	}
	else
	{
		if (pChannelStatus)
			*pChannelStatus = 0;

		if (pChannelAvail)
			*pChannelAvail = 0;

		if (pChannelActive)
			*pChannelActive = 0;

		if (pChannelMute)
			*pChannelMute = 1;

	}
	return(bChanged?1:0);
}

unsigned char handle_ddp_GetRebootState(void) {
	unsigned char bChanged = g_bUltimoRebootMessageReceived;
	g_bUltimoRebootMessageReceived = 0;
	return(bChanged?1:0);
}

unsigned char handle_ddp_GetCodecResetRequired(void) {
	unsigned char bChanged = g_bCodecResetRequired;
	g_bCodecResetRequired = 0;
	return(bChanged?1:0);
}

unsigned char handle_ddp_GetUpgradeImminent(void) {
	unsigned char bChanged = g_bUltimoUpgradeIminent;
	g_bUltimoUpgradeIminent = 0;
	return(bChanged?1:0);
}

unsigned char handle_ddp_GetNetworkConfig(uint32_t* pIpAddress, uint32_t* pIpGate, uint32_t* pIpMask) {
	unsigned char bChanged = g_UltimoConfigIPChanged;

	if(g_UltimoConfigIPChanged)
	{
		if(pIpAddress)
			*pIpAddress = g_UltimoConfigIPAddress;

		if(pIpGate)
			*pIpGate = g_UltimoConfigIPGateway;

		if(pIpMask)
			*pIpMask = g_UltimoConfigIPMask;

		g_UltimoConfigIPChanged = 0;
	}
	else
	{
		if(pIpAddress)
			*pIpAddress = 0;

		if(pIpGate)
			*pIpGate = 0;

		if(pIpMask)
			*pIpMask = 0;

	}
	return(bChanged?1:0);
}

