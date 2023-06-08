/**
* File     : example_rx_ddp.h
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

#ifndef __EXAMPLE_RX_DDP_H__
#define __EXAMPLE_RX_DDP_H__

#include "inc/packet.h"

/**
* retrieves the returned Cap version structure
* @param v1 [out] unsigned char to receive capabilities v1 info
* @param v2 [out] unsigned char to receive capabilities v2 info
* @param v3 [out] unsigned short to receive capabilities v3 info
* @param bld [out] unsigned int to receive capabilities bld info
* @return 1 if the version is valid
*/
unsigned char handle_ddp_GetCapVersion(unsigned char *v1, unsigned char *v2, unsigned short *v3, unsigned int *bld, char *pszModelName, int nMaxModelNameSize, dante_id64_t *pModelId);

/**
* retrieves the returned Os version structure
* @param v1 [out] unsigned char to receive OS v1 info
* @param v2 [out] unsigned char to receive OS v2 info
* @param v3 [out] unsigned short to receive OS v3 info
* @param bld [out] unsigned int to receive OS bld info
* @return 1 if the version is valid
*/
unsigned char handle_ddp_GetOsVersion(unsigned char *v1, unsigned char *v2, unsigned short *v3, unsigned int *bld, unsigned int *pDevStatus);

/**
* retrieves the returned Link Flags and Speed
* @param pFlags [out] unsigned short to receive Link Flags info
* @param pSpeed [out] unsigned int to receive Link Speed info
* @param pMac [out] unsigned char[6] to receive Dante MAC
* @return 1 if the Link Info is valid
*/
unsigned char handle_ddp_GetLinkInfo(unsigned short *pFlags, unsigned int *pSpeed, unsigned char *pMac, unsigned long* pIpAddress);

/**
* retrieves if a remote identify message was received
* @return 1 if a remote identify message was received
*/
unsigned char handle_ddp_Identfy();

/**
* retrieves the returned Link Flags and Speed
* @param psFriendlyName [out] string to receive new friendly name info
* @param nMaxFriendlyNameSize [in] characters in buffer to receive friendly name
* @param pDevId [out] dante_device_id_t] to receive Device ID
* @return 1 if the device ID Info is valid
*/
unsigned char handle_ddp_GetDevId(char *psFriendlyName, int nMaxFriendlyNameSize, dante_device_id_t *pDevId, unsigned short *pIdentityStatus);

/**
* retrieves the returned Channel Status for th Dante input channel
* @param pStatus [out] unsigned short to receive Channel Status info
* @return 1 if the Channel Status Info is valid
*/
//unsigned char handle_ddp_GetChannelStatus(unsigned short *pChannelStatus);
unsigned char handle_ddp_GetChannelStatus(unsigned short *pChannelStatus, unsigned char *pChannelAvail, unsigned char *pChannelActive, unsigned char *pChannelMute);

/**
* checks if a reboot message has been received, and clears the flag
* @param void
* @return returns value of g_bUltimoRebootMessageReceived and resets it to false (0)
*/
unsigned char handle_ddp_GetRebootState(void);

/**
* checks if an audio format change message has been received, and clears the flag
* @param void
* @return returns value of g_bCodecResetRequired and resets it to false (0)
*/
unsigned char handle_ddp_GetCodecResetRequired(void);

/**
* checks if an upgrade is imminent and clears the flag
* @param void
* @return returns value of g_bUltimoUpgradeImminent and resets it to false (0)
*/
unsigned char handle_ddp_GetUpgradeImminent(void);



/**
* checks if the Dante Network Configuration (configured IP address - this is different than the actual current IP address)
* has changed and clears the flag
* @param void
* @return returns value of g_ipChangedDante (1 or 0) and resets it to false (0)
*/
unsigned char handle_ddp_GetNetworkConfig(uint32_t* pIpAddress, uint32_t* pIpGate, uint32_t* pIpMask);


/**
* Handles the Dante Local Audio Format event
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the Dante local audio format event was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_dante_event_audio_format(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the Dante Local Clock Pullup event
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the Dante local clock pullup event was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_dante_event_clock_pullup(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP device general response
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP device general response was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_device_general(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP device manufacturer response
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP device manufacturer response was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_device_manuf_info(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP device upgrade response
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP device upgrade response was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_device_upgrade(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP device erase response and event
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP device erase response and event was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_device_erase_config(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP device reboot response and event
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP device reboot response and event was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_device_reboot(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP device identity response and event
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP device identity response and event was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_device_id(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

#if (defined UHIP_HOSTCPU_API_SUPPORT_DDP_DEVICE_IDENTIFY)
/**
* Handles the DDP device identify event
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP device identify response and event was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_device_identify(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);
#endif //UHIP_HOSTCPU_API_SUPPORT_DDP_DEVICE_IDENTIFY

#if (defined UHIP_HOSTCPU_API_SUPPORT_DDP_DEVICE_GPIO)
/**
* Handles the DDP device gpio response and event
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP device identify response and event was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_device_gpio(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);
#endif //UHIP_HOSTCPU_API_SUPPORT_DDP_DEVICE_GPIO

#if (defined UHIP_HOSTCPU_API_SUPPORT_DDP_DEVICE_SWITCH_LED)
/**
* Handles the DDP device Switch LED response and event
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP device identify response and event was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_device_switch_led(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);
#endif //UHIP_HOSTCPU_API_SUPPORT_DDP_DEVICE_SWITCH_LED

/**
* Handles the DDP network basic response and event
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP network basic response and event was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_network_basic(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP network config response and event
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP network config response and event was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_network_config(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP clock basic response and event
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP clock basic response and event was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_clock_basic(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP clock config response and event
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP clock config response and event was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_clock_config(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP clock pullup response
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP clock pullup response was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_clock_pullup(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP audio basic response and event
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP audio basic response and event was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_audio_basic(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP audio sample rate config response and event
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP audio sample rate config response and event was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_audio_srate(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP audio encoding config response and event
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP audio encoding config response and event was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_audio_enc(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP routing basic response
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP routing basic response was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_routing_basic(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP routing basic ready response and event
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP routing ready response and event was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_routing_ready_state(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP routing performance config response and event
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP routing performance config response and event was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_routing_performance_config(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP routing Rx chan config state response and event
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP routing Rx chan config state response and event was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_routing_rx_chan_config_state(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP routing Rx subscribe set response
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP routing Rx subscribe set response was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_routing_rx_subscribe_set(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP routing Rx chan label set response
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP routing Rx chan label set response was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_routing_rx_chan_label_set(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP routing Tx chan config state response and event
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP routing Tx chan config state response and event was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_routing_tx_chan_config_state(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP routing Tx chan label set response
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP routing Tx chan label set response was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_routing_tx_chan_label_set(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP routing Rx flow config state response and event
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP routing Rx flow config state response and event was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_routing_rx_flow_config_state(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP routing multicast Tx flow config response
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP routing multicast Tx flow config response was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_routing_multicast_tx_flow_config(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP routing Tx flow config state response and event
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP routing Tx flow config state response and event was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_routing_tx_flow_config_state(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP routing Rx chan status response and event
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP routing Rx chan status response and event was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_routing_rx_chan_status(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP routing Rx flow status response and event
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP routing Rx flow status response and event was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_routing_rx_flow_status(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP routing Rx unsubscribe chan response
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP routing Rx unsubscribe chan response was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_routing_rx_unsub_chan(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

/**
* Handles the DDP routing flow delete response
* @param ddp_rinfo [in] Pointer to a structure which has information about the DDP packet buffer, it is assumed that the ddp_packet_init_read function is called beforehand @see ddp_packet_init_read
* @param offset [in] Current offset into the packet
* @return AUD_SUCCESS if the DDP flow delete response was successfully read, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t handle_ddp_routing_flow_delete(ddp_packet_read_info_t *ddp_rinfo, uint16_t offset);

#endif // __EXAMPLE_RX_DDP_H__
