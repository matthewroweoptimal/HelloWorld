/**
* File     : example_tx_ddp.c
* Created  : October 2014
* Updated  : 2014/10/08
* Author   : Chamira Perera
* Synopsis : HostCPU Implementation of sending various example DDP requests
*
* This module implements the following DDP requests
* 1. Querying the Dante device's network interface state 
* 2. Configuring static IP address for a Dante device
* 3. Subscribe the Rx audio channels of a Dante device to another Dante device (transmitter, Tx)
* 4. Creating a multicast Tx for flow for a Dante device
* 5. Delete the multicast Tx flow for a Dante device created in example 4
* 6. Upgrade the application firmware for a Dante device using XMODEM over SPI
* 7. Change the audio sample rate of a Dante device
* 8. Change the clock pullup of a Dante device
* 9. Set a GPIO output of a Dante device
*
* Copyright 2005-2014, Audinate Pty Ltd and/or its licensors
* Confidential
* Audinate Copyright Header Version 1
*/

#include "tx_ddp.h"

#include "uhip.h"
#include "tx_uhip.h"
#include "hostcpu_transport.h"
#include "cobs.h"
#include "frame.h"
#include "uhip_structures.h"
#include "uhip_packet_write.h"
#include "uhip_hostcpu_tx_timer.h"
#include "oly.h"

#include "inc/audio_client.h"
#include "inc/clocking_client.h"
#include "inc/device_client.h"
#include "inc/local_client.h"
#include "inc/network_client.h"
#include "inc/routing_client.h"

extern uint8_t tx_buffer[];
extern uint8_t cobs_tx_buffer[];

aud_error_t send_network_interface_query()
{
	aud_error_t result;
	uhip_packet_write_info_t uhip_winfo;
	ddp_packet_write_info_t ddp_winfo;
	ddp_request_id_t const network_if_request_id = 1;
	uint8_t *ddp_tx_buffer_ptr;
	size_t cobs_encoded_size, total_data_to_send, bytes_sent;

#ifdef DANTE_DEBUG
	printf("UHIP:Sending DDP network basic request to query network interface\n");
#endif	//	DANTE_DEBUG
	hostcpu_uhip_set_ddp_tx_request_id(network_if_request_id);

	// 1. Allocate room to store the UHIP header before adding the DDP data
	ddp_tx_buffer_ptr = &tx_buffer[sizeof(uhip_header_t)];

	// 2. Intialize the DDP packet buffer
	result = ddp_packet_init_write(&ddp_winfo, (uint32_t *)ddp_tx_buffer_ptr, UHIP_PACKET_SIZE_MAX - sizeof(uhip_header_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 3. Create the DDP netwok basic request message
	result = ddp_add_network_basic_request(&ddp_winfo, network_if_request_id);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 4. Initialize the UHIP packet buffer
	result = uhip_packet_init_write(&uhip_winfo, (uint32_t *)&tx_buffer, UHIP_PACKET_SIZE_MAX);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 5. Create the UHIP packet with the DDP packet created from step 3
	result = uhip_packet_write_dante_device_protcol(&uhip_winfo, (uint32_t *)ddp_tx_buffer_ptr, ddp_packet_write_get_length_bytes(&ddp_winfo));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 6. COBS encode the UHIP+DDP packet from step 5. Ensure to keep the first byte free in the buffer that
	// stores the COBS encoded data to add the first delimiter byte
	cobs_encoded_size = cobs_encode(tx_buffer, uhip_winfo.curr_length_bytes, &cobs_tx_buffer[1]);

	// 7. Add the delimeter bytes to frame and add pad bytes to make the data to be sent a multiple of 32-bytes
	total_data_to_send = prepare_tx_frame(cobs_tx_buffer, cobs_encoded_size);
	if (total_data_to_send == 0)
	{
		return AUD_ERR_SYSTEM;
	}

	// 8. Send the data to the Dante device
	bytes_sent = hostcpu_transport_write(cobs_tx_buffer, total_data_to_send);
	if (bytes_sent != total_data_to_send)
	{
		return AUD_ERR_SYSTEM;
	}

	return AUD_SUCCESS;
}

aud_error_t configure_static_ip()
{
	aud_error_t result;
	uhip_packet_write_info_t uhip_winfo;
	ddp_packet_write_info_t ddp_winfo;
	ddp_request_id_t const config_static_ip_request_id = 2;
	uint8_t *ddp_tx_buffer_ptr;
	size_t cobs_encoded_size, total_data_to_send, bytes_sent;
	network_ip_config_t ip_config;

#ifdef DANTE_DEBUG
	printf("UHIP:Sending DDP network config request to configure a static IP address\n");
#endif	//	DANTE_DEBUG

	hostcpu_uhip_set_ddp_tx_request_id(config_static_ip_request_id);

	// 1. Allocate room to store the UHIP header before adding the DDP data
	ddp_tx_buffer_ptr = &tx_buffer[sizeof(uhip_header_t)];

	// 2. Intialize the DDP packet buffer
	result = ddp_packet_init_write(&ddp_winfo, (uint32_t *)ddp_tx_buffer_ptr, UHIP_PACKET_SIZE_MAX - sizeof(uhip_header_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 3. Populate the static IP parameters
	ip_config.ip_addr = (169 << 24) | (254 << 16) | (44 << 8) | 33;	// The static IP is 169.254.44.33
	ip_config.subnet_mask = (255 << 24) | (255 << 16) | (0 << 8) | 0;	// The subnet mask is 255.255.0.0
	ip_config.gateway = (169 << 24) | (254 << 16) | (0 << 8) | 1;	// The gateway IP is 169.254.0.1

	// 4. Create the DDP netwok basic request message, we will not be setting a DNS and domain name
	result = ddp_add_network_config_request(&ddp_winfo, config_static_ip_request_id, NETWORK_INTERFACE_MODE_STATIC, &ip_config, NULL, NULL);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 5. Initialize the UHIP packet buffer
	result = uhip_packet_init_write(&uhip_winfo, (uint32_t *)&tx_buffer, UHIP_PACKET_SIZE_MAX);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 6. Create the UHIP packet with the DDP packet created from step 4
	result = uhip_packet_write_dante_device_protcol(&uhip_winfo, (uint32_t *)ddp_tx_buffer_ptr, ddp_packet_write_get_length_bytes(&ddp_winfo));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 7. COBS encode the UHIP+DDP packet from step 6. Ensure to keep the first byte free in the buffer that
	// stores the COBS encoded data to add the first delimiter byte
	cobs_encoded_size = cobs_encode(tx_buffer, uhip_winfo.curr_length_bytes, &cobs_tx_buffer[1]);

	// 8. Add the delimeter bytes to frame and add pad bytes to make the data to be sent a multiple of 32-bytes
	total_data_to_send = prepare_tx_frame(cobs_tx_buffer, cobs_encoded_size);
	if (total_data_to_send == 0)
	{
		return AUD_ERR_SYSTEM;
	}

	// 9. Send the data to the Dante device
	bytes_sent = hostcpu_transport_write(cobs_tx_buffer, total_data_to_send);
	if (bytes_sent != total_data_to_send)
	{
		return AUD_ERR_SYSTEM;
	}

	return AUD_SUCCESS;
}

aud_error_t subscribe_channels()
{
	aud_error_t result;
	uhip_packet_write_info_t uhip_winfo;
	ddp_packet_write_info_t ddp_winfo;
	ddp_request_id_t const sub_chans_request_id = 3;
	uint8_t *ddp_tx_buffer_ptr;
	size_t cobs_encoded_size, total_data_to_send, bytes_sent;
	ddp_rx_chan_sub_params_t sub_params[2];

#ifdef DANTE_DEBUG
	printf("UHIP:Sending a DDP routing Rx subscribe set request to subscribe channels\n");
#endif	//	DANTE_DEBUG

	hostcpu_uhip_set_ddp_tx_request_id(sub_chans_request_id);

	// 1. Allocate room to store the UHIP header before adding the DDP data
	ddp_tx_buffer_ptr = &tx_buffer[sizeof(uhip_header_t)];

	// 2. Intialize the DDP packet buffer
	result = ddp_packet_init_write(&ddp_winfo, (uint32_t *)ddp_tx_buffer_ptr, UHIP_PACKET_SIZE_MAX - sizeof(uhip_header_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 3. Populate parameters for the subscription request
	sub_params[0].rx_chan_id = 1;
	sub_params[0].tx_chan_id = "01";
	sub_params[0].tx_device = "BKLYN-II-0615b2";
	sub_params[1].rx_chan_id = 2;
	sub_params[1].tx_chan_id = "02";
	sub_params[1].tx_device = "BKLYN-II-0615b2";

	// 4. Create the DDP routing Rx subscribe request message
	result = ddp_add_routing_rx_sub_set_request(&ddp_winfo, sub_chans_request_id, 2, sub_params);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 5. Initialize the UHIP packet buffer
	result = uhip_packet_init_write(&uhip_winfo, (uint32_t *)&tx_buffer, UHIP_PACKET_SIZE_MAX);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 6. Create the UHIP packet with the DDP packet created from step 4
	result = uhip_packet_write_dante_device_protcol(&uhip_winfo, (uint32_t *)ddp_tx_buffer_ptr, ddp_packet_write_get_length_bytes(&ddp_winfo));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 7. COBS encode the UHIP+DDP packet from step 6. Ensure to keep the first byte free in the buffer that
	// stores the COBS encoded data to add the first delimiter byte
	cobs_encoded_size = cobs_encode(tx_buffer, uhip_winfo.curr_length_bytes, &cobs_tx_buffer[1]);

	// 8. Add the delimeter bytes to frame and add pad bytes to make the data to be sent a multiple of 32-bytes
	total_data_to_send = prepare_tx_frame(cobs_tx_buffer, cobs_encoded_size);
	if (total_data_to_send == 0)
	{
		return AUD_ERR_SYSTEM;
	}

	// 8. Send the data to the Dante device
	bytes_sent = hostcpu_transport_write(cobs_tx_buffer, total_data_to_send);
	if (bytes_sent != total_data_to_send)
	{
		return AUD_ERR_SYSTEM;
	}

	return AUD_SUCCESS;
}

aud_error_t create_tx_multicast_flow()
{
	aud_error_t result;
	uhip_packet_write_info_t uhip_winfo;
	ddp_packet_write_info_t ddp_winfo;
	ddp_request_id_t const tx_multicast_request_id = 4;
	uint8_t *ddp_tx_buffer_ptr;
	size_t cobs_encoded_size, total_data_to_send, bytes_sent;
	ddp_multicast_tx_flow_params_t mcast_params;
	uint16_t slots[2];

#ifdef DANTE_DEBUG
	printf("UHIP:Sending a DDP routing multicast Tx flow configuration request to create a Tx multicast flow\n");
#endif	//	DANTE_DEBUG

	hostcpu_uhip_set_ddp_tx_request_id(tx_multicast_request_id);

	// 1. Allocate room to store the UHIP header before adding the DDP data
	ddp_tx_buffer_ptr = &tx_buffer[sizeof(uhip_header_t)];

	// 2. Intialize the DDP packet buffer
	result = ddp_packet_init_write(&ddp_winfo, (uint32_t *)ddp_tx_buffer_ptr, UHIP_PACKET_SIZE_MAX - sizeof(uhip_header_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 3. Populate the parameters for the multicast Tx
	mcast_params.encoding = DANTE_ENCODING_PCM24;
	mcast_params.flow_id = 2;
	mcast_params.fpp = 32;
	mcast_params.label = "Tx Mcast Flow";
	mcast_params.latency = 0;
	mcast_params.num_slots = 2;
	mcast_params.sample_rate = 0;

	slots[0] = 1;										// Slot 1 transmits Tx channel 1
	slots[1] = 2;										// Slot 2 transmits Tx channel 2

	// 4. Add the slot array to the multicast Tx parameters
	ddp_routing_multicast_tx_flow_config_add_slot_params(&mcast_params, slots);

	// 5. Create the DDP routing Rx subscribe request message
	result = ddp_add_routing_multicast_tx_flow_config_request(&ddp_winfo, tx_multicast_request_id, &mcast_params);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 6. Initialize the UHIP packet buffer
	result = uhip_packet_init_write(&uhip_winfo, (uint32_t *)&tx_buffer, UHIP_PACKET_SIZE_MAX);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 7. Create the UHIP packet with the DDP packet created from step 5
	result = uhip_packet_write_dante_device_protcol(&uhip_winfo, (uint32_t *)ddp_tx_buffer_ptr, ddp_packet_write_get_length_bytes(&ddp_winfo));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 8. COBS encode the UHIP+DDP packet from step 7. Ensure to keep the first byte free in the buffer that
	// stores the COBS encoded data to add the first delimiter byte
	cobs_encoded_size = cobs_encode(tx_buffer, uhip_winfo.curr_length_bytes, &cobs_tx_buffer[1]);

	// 9. Add the delimeter bytes to frame and add pad bytes to make the data to be sent a multiple of 32-bytes
	total_data_to_send = prepare_tx_frame(cobs_tx_buffer, cobs_encoded_size);
	if (total_data_to_send == 0)
	{
		return AUD_ERR_SYSTEM;
	}

	// 10. Send the data to the Dante device
	bytes_sent = hostcpu_transport_write(cobs_tx_buffer, total_data_to_send);
	if (bytes_sent != total_data_to_send)
	{
		return AUD_ERR_SYSTEM;
	}

	return AUD_SUCCESS;
}

aud_error_t delete_flow()
{
	aud_error_t result;
	uhip_packet_write_info_t uhip_winfo;
	ddp_packet_write_info_t ddp_winfo;
	ddp_request_id_t const flow_delete_request_id = 5;
	uint8_t *ddp_tx_buffer_ptr;
	size_t cobs_encoded_size, total_data_to_send, bytes_sent;

#ifdef DANTE_DEBUG
	printf("UHIP:Sending a DDP routing flow delete request to delete a flow\n");
#endif	//	DANTE_DEBUG

	hostcpu_uhip_set_ddp_tx_request_id(flow_delete_request_id);

	// 1. Allocate room to store the UHIP header before adding the DDP data
	ddp_tx_buffer_ptr = &tx_buffer[sizeof(uhip_header_t)];

	// 2. Intialize the DDP packet buffer
	result = ddp_packet_init_write(&ddp_winfo, (uint32_t *)ddp_tx_buffer_ptr, UHIP_PACKET_SIZE_MAX - sizeof(uhip_header_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 3. Create the DDP routing delete flow request message, the multicast Tx flow created in create_tx_multicast_flow has an ID
	// of 2. Therefore we are only deleting flow ID 2
	result = ddp_add_routing_flow_delete_request(&ddp_winfo, flow_delete_request_id, 2, 2, DDP_ROUTING_FLOW_FILTER__TX_MANUAL);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 4. Initialize the UHIP packet buffer
	result = uhip_packet_init_write(&uhip_winfo, (uint32_t *)&tx_buffer, UHIP_PACKET_SIZE_MAX);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 5. Create the UHIP packet with the DDP packet created from step 3
	result = uhip_packet_write_dante_device_protcol(&uhip_winfo, (uint32_t *)ddp_tx_buffer_ptr, ddp_packet_write_get_length_bytes(&ddp_winfo));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 8. COBS encode the UHIP+DDP packet from step 7. Ensure to keep the first byte free in the buffer that
	// stores the COBS encoded data to add the first delimiter byte
	cobs_encoded_size = cobs_encode(tx_buffer, uhip_winfo.curr_length_bytes, &cobs_tx_buffer[1]);

	// 9. Add the delimeter bytes to frame and add pad bytes to make the data to be sent a multiple of 32-bytes
	total_data_to_send = prepare_tx_frame(cobs_tx_buffer, cobs_encoded_size);
	if (total_data_to_send == 0)
	{
		return AUD_ERR_SYSTEM;
	}

	// 10. Send the data to the Dante device
	bytes_sent = hostcpu_transport_write(cobs_tx_buffer, total_data_to_send);
	if (bytes_sent != total_data_to_send)
	{
		return AUD_ERR_SYSTEM;
	}

	return AUD_SUCCESS;
}

aud_error_t upgrade_via_xmodem()
{
	aud_error_t result;
	uhip_packet_write_info_t uhip_winfo;
	ddp_packet_write_info_t ddp_winfo;
	ddp_request_id_t const upgrade_request_id = 6;
	uint8_t *ddp_tx_buffer_ptr;
	size_t cobs_encoded_size, total_data_to_send, bytes_sent;
	upgrade_xmodem_t xmodem_params;

#ifdef DANTE_DEBUG
	printf("UHIP:Sending a DDP device upgrade request to upgrade the firmware on the device\n");
#endif	//	DANTE_DEBUG

	hostcpu_uhip_set_ddp_tx_request_id(upgrade_request_id);

	// 1. Allocate room to store the UHIP header before adding the DDP data
	ddp_tx_buffer_ptr = &tx_buffer[sizeof(uhip_header_t)];

	// 2. Intialize the DDP packet buffer
	result = ddp_packet_init_write(&ddp_winfo, (uint32_t *)ddp_tx_buffer_ptr, UHIP_PACKET_SIZE_MAX - sizeof(uhip_header_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 3. Populate the XMODEM related parameters for the upgrade
	xmodem_params.port = UPGRADE_XMODEM_PORT_SPI0;														// Use the SPI serial interface
	xmodem_params.baud_rate = DDP_SPI_BAUD_3125000;														// Dante device SPI master clock rate is 3.125 MHz
	xmodem_params.file_length = 410004;																	// File size of the firmware DNT file (typical Ultimo application DNT size)
	xmodem_params.s_params.spi[DDP_XMODEM_SPI_DEV_MASTER].polarity = DDP_SPI_CPOL_IDLE_HIGH;			// SPI master polarity of the Dante device
	xmodem_params.s_params.spi[DDP_XMODEM_SPI_DEV_MASTER].phase = DDP_SPI_CPHA_SAMPLE_TRAILING_EDGE;	// SPI master phase of the Dante device
	xmodem_params.s_params.spi[DDP_XMODEM_SPI_DEV_SLAVE].polarity = DDP_SPI_CPOL_IDLE_HIGH;				// SPI slave polarity of the Dante device
	xmodem_params.s_params.spi[DDP_XMODEM_SPI_DEV_SLAVE].phase = DDP_SPI_CPHA_SAMPLE_TRAILING_EDGE;		// SPI slave phase of the Dante device

	// 4. Create the DDP device upgrade request message 
	result = ddp_add_device_upgrade_xmodem_request(&ddp_winfo, upgrade_request_id, (UPGRADE_ACTION_DOWNLOAD | UPGRADE_ACTION_FLASH), 0, 0, NULL, NULL, &xmodem_params);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 5. Initialize the UHIP packet buffer
	result = uhip_packet_init_write(&uhip_winfo, (uint32_t *)&tx_buffer, UHIP_PACKET_SIZE_MAX);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 6. Create the UHIP packet with the DDP packet created from step 4
	result = uhip_packet_write_dante_device_protcol(&uhip_winfo, (uint32_t *)ddp_tx_buffer_ptr, ddp_packet_write_get_length_bytes(&ddp_winfo));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 7. COBS encode the UHIP+DDP packet from step 4. Ensure to keep the first byte free in the buffer that
	// stores the COBS encoded data to add the first delimiter byte
	cobs_encoded_size = cobs_encode(tx_buffer, uhip_winfo.curr_length_bytes, &cobs_tx_buffer[1]);

	// 8. Add the delimeter bytes to frame and add pad bytes to make the data to be sent a multiple of 32-bytes
	total_data_to_send = prepare_tx_frame(cobs_tx_buffer, cobs_encoded_size);
	if (total_data_to_send == 0)
	{
		return AUD_ERR_SYSTEM;
	}

	// 9. Send the data to the Dante device
	bytes_sent = hostcpu_transport_write(cobs_tx_buffer, total_data_to_send);
	if (bytes_sent != total_data_to_send)
	{
		return AUD_ERR_SYSTEM;
	}

	return AUD_SUCCESS;
}

aud_error_t configure_sample_rate()
{
	aud_error_t result;
	uhip_packet_write_info_t uhip_winfo;
	ddp_packet_write_info_t ddp_winfo;
	ddp_request_id_t const config_srate_request_id = 7;
	uint8_t *ddp_tx_buffer_ptr;
	size_t cobs_encoded_size, total_data_to_send, bytes_sent;

#ifdef DANTE_DEBUG
	printf("UHIP:Sending a DDP audio sample rate configuration request to configure the sample rate on the device\n");
#endif	//	DANTE_DEBUG

	hostcpu_uhip_set_ddp_tx_request_id(config_srate_request_id);

	// 1. Allocate room to store the UHIP header before adding the DDP data
	ddp_tx_buffer_ptr = &tx_buffer[sizeof(uhip_header_t)];

	// 2. Intialize the DDP packet buffer
	result = ddp_packet_init_write(&ddp_winfo, (uint32_t *)ddp_tx_buffer_ptr, UHIP_PACKET_SIZE_MAX - sizeof(uhip_header_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 3. Create the DDP audio sample rate config request message, we will be setting the audio sample rate to 44.1 KHz
	result = ddp_add_audio_sample_rate_config_request(&ddp_winfo, config_srate_request_id, SET_SRATE_FLAGS, 44100);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 4. Initialize the UHIP packet buffer
	result = uhip_packet_init_write(&uhip_winfo, (uint32_t *)&tx_buffer, UHIP_PACKET_SIZE_MAX);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 5. Create the UHIP packet with the DDP packet created from step 3
	result = uhip_packet_write_dante_device_protcol(&uhip_winfo, (uint32_t *)ddp_tx_buffer_ptr, ddp_packet_write_get_length_bytes(&ddp_winfo));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 6. COBS encode the UHIP+DDP packet from step 5. Ensure to keep the first byte free in the buffer that
	// stores the COBS encoded data to add the first delimiter byte
	cobs_encoded_size = cobs_encode(tx_buffer, uhip_winfo.curr_length_bytes, &cobs_tx_buffer[1]);

	// 7. Add the delimeter bytes to frame and add pad bytes to make the data to be sent a multiple of 32-bytes
	total_data_to_send = prepare_tx_frame(cobs_tx_buffer, cobs_encoded_size);
	if (total_data_to_send == 0)
	{
		return AUD_ERR_SYSTEM;
	}

	// 8. Send the data to the Dante device
	bytes_sent = hostcpu_transport_write(cobs_tx_buffer, total_data_to_send);
	if (bytes_sent != total_data_to_send)
	{
		return AUD_ERR_SYSTEM;
	}

	return AUD_SUCCESS;
}

aud_error_t configure_clock_pullup()
{
	aud_error_t result;
	uhip_packet_write_info_t uhip_winfo;
	ddp_packet_write_info_t ddp_winfo;
	ddp_request_id_t const config_clock_pullup_request_id = 8;
	uint8_t *ddp_tx_buffer_ptr;
	size_t cobs_encoded_size, total_data_to_send, bytes_sent;
	clock_pullup_control_fields_t clock_pullup_params;

#ifdef DANTE_DEBUG
	printf("UHIP:Sending a DDP clock pullup request to configure the clock pullup on the device\n");
#endif	//	DANTE_DEBUG

	hostcpu_uhip_set_ddp_tx_request_id(config_clock_pullup_request_id);

	// 1. Allocate room to store the UHIP header before adding the DDP data
	ddp_tx_buffer_ptr = &tx_buffer[sizeof(uhip_header_t)];

	// 2. Intialize the DDP packet buffer
	result = ddp_packet_init_write(&ddp_winfo, (uint32_t *)ddp_tx_buffer_ptr, UHIP_PACKET_SIZE_MAX - sizeof(uhip_header_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 3. Populate the clock pullup parameters
	clock_pullup_params.valid_flags = CLOCK_PULLUP_SET_PULLUP;	// Configuring both the clock pullup and subdomain
	clock_pullup_params.pullup = CLOCK_PULLUP_POS_4_1667_PCENT;

	// 4. Create the DDP clock pullup request message
	result = ddp_add_clock_pullup_request(&ddp_winfo, config_clock_pullup_request_id, &clock_pullup_params);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 4. Initialize the UHIP packet buffer
	result = uhip_packet_init_write(&uhip_winfo, (uint32_t *)&tx_buffer, UHIP_PACKET_SIZE_MAX);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 5. Create the UHIP packet with the DDP packet created from step 3
	result = uhip_packet_write_dante_device_protcol(&uhip_winfo, (uint32_t *)ddp_tx_buffer_ptr, ddp_packet_write_get_length_bytes(&ddp_winfo));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 6. COBS encode the UHIP+DDP packet from step 5. Ensure to keep the first byte free in the buffer that
	// stores the COBS encoded data to add the first delimiter byte
	cobs_encoded_size = cobs_encode(tx_buffer, uhip_winfo.curr_length_bytes, &cobs_tx_buffer[1]);

	// 7. Add the delimeter bytes to frame and add pad bytes to make the data to be sent a multiple of 32-bytes
	total_data_to_send = prepare_tx_frame(cobs_tx_buffer, cobs_encoded_size);
	if (total_data_to_send == 0)
	{
		return AUD_ERR_SYSTEM;
	}

	// 8. Send the data to the Dante device
	bytes_sent = hostcpu_transport_write(cobs_tx_buffer, total_data_to_send);
	if (bytes_sent != total_data_to_send)
	{
		return AUD_ERR_SYSTEM;
	}

	return AUD_SUCCESS;
}

aud_error_t send_device_general_query()
{
	aud_error_t result;
	uhip_packet_write_info_t uhip_winfo;
	ddp_packet_write_info_t ddp_winfo;
	ddp_request_id_t const device_general_request_id = 9;
	uint8_t *ddp_tx_buffer_ptr;
	size_t cobs_encoded_size, total_data_to_send, bytes_sent;

#ifdef DANTE_DEBUG
	printf("UHIP:Sending DDP device general request to query device interface\n");
#endif	//	DANTE_DEBUG

	hostcpu_uhip_set_ddp_tx_request_id(device_general_request_id);

	// 1. Allocate room to store the UHIP header before adding the DDP data
	ddp_tx_buffer_ptr = &tx_buffer[sizeof(uhip_header_t)];

	// 2. Intialize the DDP packet buffer
	result = ddp_packet_init_write(&ddp_winfo, (uint32_t *)ddp_tx_buffer_ptr, UHIP_PACKET_SIZE_MAX - sizeof(uhip_header_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 3. Create the DDP device general request message
	result = ddp_add_device_general_request(&ddp_winfo, device_general_request_id);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 4. Initialize the UHIP packet buffer
	result = uhip_packet_init_write(&uhip_winfo, (uint32_t *)&tx_buffer, UHIP_PACKET_SIZE_MAX);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 5. Create the UHIP packet with the DDP packet created from step 3
	result = uhip_packet_write_dante_device_protcol(&uhip_winfo, (uint32_t *)ddp_tx_buffer_ptr, ddp_packet_write_get_length_bytes(&ddp_winfo));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 6. COBS encode the UHIP+DDP packet from step 5. Ensure to keep the first byte free in the buffer that
	// stores the COBS encoded data to add the first delimiter byte
	cobs_encoded_size = cobs_encode(tx_buffer, uhip_winfo.curr_length_bytes, &cobs_tx_buffer[1]);

	// 7. Add the delimeter bytes to frame and add pad bytes to make the data to be sent a multiple of 32-bytes
	total_data_to_send = prepare_tx_frame(cobs_tx_buffer, cobs_encoded_size);
	if (total_data_to_send == 0)
	{
		return AUD_ERR_SYSTEM;
	}

	// 8. Send the data to the Dante device
	bytes_sent = hostcpu_transport_write(cobs_tx_buffer, total_data_to_send);
	if (bytes_sent != total_data_to_send)
	{
		return AUD_ERR_SYSTEM;
	}

	return AUD_SUCCESS;
}


aud_error_t set_gpio_output()
{
	aud_error_t result;
	uhip_packet_write_info_t uhip_winfo;
	ddp_packet_write_info_t ddp_winfo;
	ddp_request_id_t const gpio_request_id = 8;
	uint8_t *ddp_tx_buffer_ptr;
	size_t cobs_encoded_size, total_data_to_send, bytes_sent;
	uint32_t const output_mask = (0x01 << 6) | (0x01 << 5) | (0x01 << 4);
	uint32_t const output_values = (0x01 << 5) | (0x01 << 4);

	AUD_PRINTF("Sending DDP GPIO control message to set GPIO outputs\n");

	hostcpu_uhip_set_ddp_tx_request_id(gpio_request_id);

	// 1. Allocate room to store the UHIP header before adding the DDP data
	ddp_tx_buffer_ptr = &tx_buffer[sizeof(uhip_header_t)];

	// 2. Intialize the DDP packet buffer
	result = ddp_packet_init_write(&ddp_winfo, (uint32_t *)ddp_tx_buffer_ptr, UHIP_PACKET_SIZE_MAX - sizeof(uhip_header_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 3. Create the DDP device GPIO request message
	// The output valid state flags are 0x70, so we are setting the values on GPIO 6, 5, 4
	// The output values are 0x30 so we are setting GPIO 6 = LOW, GPIO5 = HIGH, GPIO4 = HIGH
	result = ddp_add_device_gpio_request(&ddp_winfo, gpio_request_id, output_mask, output_values);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 4. Initialize the UHIP packet buffer
	result = uhip_packet_init_write(&uhip_winfo, (uint32_t *)&tx_buffer, UHIP_PACKET_SIZE_MAX);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 5. Create the UHIP packet with the DDP packet created from step 3
	result = uhip_packet_write_dante_device_protcol(&uhip_winfo, (uint32_t *)ddp_tx_buffer_ptr, ddp_packet_write_get_length_bytes(&ddp_winfo));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 6. COBS encode the UHIP+DDP packet from step 5. Ensure to keep the first byte free in the buffer that
	// stores the COBS encoded data to add the first delimiter byte
	cobs_encoded_size = cobs_encode(tx_buffer, uhip_winfo.curr_length_bytes, &cobs_tx_buffer[1]);

	// 7. Add the delimeter bytes to frame and add pad bytes to make the data to be sent a multiple of 32-bytes
	total_data_to_send = prepare_tx_frame(cobs_tx_buffer, cobs_encoded_size);
	if (total_data_to_send == 0)
	{
		return AUD_ERR_SYSTEM;
	}

	// 8. Send the data to the Dante device
	bytes_sent = hostcpu_transport_write(cobs_tx_buffer, total_data_to_send);
	if (bytes_sent != total_data_to_send)
	{
		return AUD_ERR_SYSTEM;
	}

	return AUD_SUCCESS;
}

aud_error_t turn_off_switch_leds()
{
	aud_error_t result;
	uhip_packet_write_info_t uhip_winfo;
	ddp_packet_write_info_t ddp_winfo;
	ddp_request_id_t const sw_led_request_id = 8;
	uint8_t *ddp_tx_buffer_ptr;
	size_t cobs_encoded_size, total_data_to_send, bytes_sent;

	AUD_PRINTF("Sending DDP Switch LED control message to turn off Switch LEDs\n");

	hostcpu_uhip_set_ddp_tx_request_id(sw_led_request_id);

	// 1. Allocate room to store the UHIP header before adding the DDP data
	ddp_tx_buffer_ptr = &tx_buffer[sizeof(uhip_header_t)];

	// 2. Intialize the DDP packet buffer
	result = ddp_packet_init_write(&ddp_winfo, (uint32_t *)ddp_tx_buffer_ptr, UHIP_PACKET_SIZE_MAX - sizeof(uhip_header_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 3. Create the DDP device Switch LED request message
	result = ddp_add_device_switch_led_request(&ddp_winfo, sw_led_request_id, SWITCH_LED_MODE_FORCE_OFF);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 4. Initialize the UHIP packet buffer
	result = uhip_packet_init_write(&uhip_winfo, (uint32_t *)&tx_buffer, UHIP_PACKET_SIZE_MAX);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 5. Create the UHIP packet with the DDP packet created from step 3
	result = uhip_packet_write_dante_device_protcol(&uhip_winfo, (uint32_t *)ddp_tx_buffer_ptr, ddp_packet_write_get_length_bytes(&ddp_winfo));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 6. COBS encode the UHIP+DDP packet from step 5. Ensure to keep the first byte free in the buffer that
	// stores the COBS encoded data to add the first delimiter byte
	cobs_encoded_size = cobs_encode(tx_buffer, uhip_winfo.curr_length_bytes, &cobs_tx_buffer[1]);

	// 7. Add the delimeter bytes to frame and add pad bytes to make the data to be sent a multiple of 32-bytes
	total_data_to_send = prepare_tx_frame(cobs_tx_buffer, cobs_encoded_size);
	if (total_data_to_send == 0)
	{
		return AUD_ERR_SYSTEM;
	}

	// 8. Send the data to the Dante device
	bytes_sent = hostcpu_transport_write(cobs_tx_buffer, total_data_to_send);
	if (bytes_sent != total_data_to_send)
	{
		return AUD_ERR_SYSTEM;
	}

	return AUD_SUCCESS;
}

/*
 * Dante firmware update to V4.x.x.x. introduced a new version of this function (ddp_add_device_manufacturer_request)
 * it allows the host to override manufacturer capabilities version numbers!
 * This expansion caused our function to fail. This is an attempt to override
 *
 * @struct ddp_device_manf_request
 * @brief Structure format for the "device manufacturer request" message
 *
 */

aud_error_t send_device_manuf_info()
{
	aud_error_t result;
	uhip_packet_write_info_t uhip_winfo;
	ddp_packet_write_info_t ddp_winfo;
	ddp_request_id_t const device_manuf_info_request_id = 10;
	uint8_t *ddp_tx_buffer_ptr;
	size_t cobs_encoded_size, total_data_to_send, bytes_sent;

	ddp_manf_override_values_t overrides = {0};

#ifdef DANTE_DEBUG
	printf("UHIP:Sending DDP device manuf info request to query device interface\n");
#endif	//	DANTE_DEBUG

	hostcpu_uhip_set_ddp_tx_request_id(device_manuf_info_request_id);

	// 1. Allocate room to store the UHIP header before adding the DDP data
	ddp_tx_buffer_ptr = &tx_buffer[sizeof(uhip_header_t)];

	// 2. Intialize the DDP packet buffer
	result = ddp_packet_init_write(&ddp_winfo, (uint32_t *)ddp_tx_buffer_ptr, UHIP_PACKET_SIZE_MAX - sizeof(uhip_header_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}


	// Set manufacturer overrride values - These overrides can be viewed in DC. To clear the overrides, reset the device
/*	overrides.manf_name_string = "MyTestManf";
	overrides.model_id_string = "MyTestModel";
	overrides.software_version = 10 << 24 | 20 << 16 | 30;
	overrides.software_build = 40;
	overrides.firmware_version = OLY_FW_VERSION;
	overrides.firmware_build = 80;
	overrides.capability_flags = 0x00000003;
	overrides.model_version_string = "MyTestVersion";
*/

	// 3. Create the DDP device manuf info request message
	result = ddp_add_device_manufacturer_request(&ddp_winfo, device_manuf_info_request_id, &overrides);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 4. Initialize the UHIP packet buffer
	result = uhip_packet_init_write(&uhip_winfo, (uint32_t *)&tx_buffer, UHIP_PACKET_SIZE_MAX);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 5. Create the UHIP packet with the DDP packet created from step 3
	result = uhip_packet_write_dante_device_protcol(&uhip_winfo, (uint32_t *)ddp_tx_buffer_ptr, ddp_packet_write_get_length_bytes(&ddp_winfo));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 6. COBS encode the UHIP+DDP packet from step 5. Ensure to keep the first byte free in the buffer that
	// stores the COBS encoded data to add the first delimiter byte
	cobs_encoded_size = cobs_encode(tx_buffer, uhip_winfo.curr_length_bytes, &cobs_tx_buffer[1]);

	// 7. Add the delimeter bytes to frame and add pad bytes to make the data to be sent a multiple of 32-bytes
	total_data_to_send = prepare_tx_frame(cobs_tx_buffer, cobs_encoded_size);
	if (total_data_to_send == 0)
	{
		return AUD_ERR_SYSTEM;
	}

	// 8. Send the data to the Dante device
	bytes_sent = hostcpu_transport_write(cobs_tx_buffer, total_data_to_send);
	if (bytes_sent != total_data_to_send)
	{
		return AUD_ERR_SYSTEM;
	}

	return AUD_SUCCESS;
}

aud_error_t send_device_dev_id(char *pszNewName)
{
	aud_error_t result;
	uhip_packet_write_info_t uhip_winfo;
	ddp_packet_write_info_t ddp_winfo;
	ddp_request_id_t const device_dev_id_request_id = 10;
	uint8_t *ddp_tx_buffer_ptr;
	size_t cobs_encoded_size, total_data_to_send, bytes_sent;

#ifdef DANTE_DEBUG
	printf("UHIP:Sending DDP device id request to query device interface\n");
#endif	//	DANTE_DEBUG

	hostcpu_uhip_set_ddp_tx_request_id(device_dev_id_request_id);

	// 1. Allocate room to store the UHIP header before adding the DDP data
	ddp_tx_buffer_ptr = &tx_buffer[sizeof(uhip_header_t)];

	// 2. Intialize the DDP packet buffer
	result = ddp_packet_init_write(&ddp_winfo, (uint32_t *)ddp_tx_buffer_ptr, UHIP_PACKET_SIZE_MAX - sizeof(uhip_header_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 3. Create the DDP device id request message
	result = ddp_add_device_identity_request(&ddp_winfo, device_dev_id_request_id, pszNewName);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 4. Initialize the UHIP packet buffer
	result = uhip_packet_init_write(&uhip_winfo, (uint32_t *)&tx_buffer, UHIP_PACKET_SIZE_MAX);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 5. Create the UHIP packet with the DDP packet created from step 3
	result = uhip_packet_write_dante_device_protcol(&uhip_winfo, (uint32_t *)ddp_tx_buffer_ptr, ddp_packet_write_get_length_bytes(&ddp_winfo));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 6. COBS encode the UHIP+DDP packet from step 5. Ensure to keep the first byte free in the buffer that
	// stores the COBS encoded data to add the first delimiter byte
	cobs_encoded_size = cobs_encode(tx_buffer, uhip_winfo.curr_length_bytes, &cobs_tx_buffer[1]);

	// 7. Add the delimeter bytes to frame and add pad bytes to make the data to be sent a multiple of 32-bytes
	total_data_to_send = prepare_tx_frame(cobs_tx_buffer, cobs_encoded_size);
	if (total_data_to_send == 0)
	{
		return AUD_ERR_SYSTEM;
	}

	// 8. Send the data to the Dante device
	bytes_sent = hostcpu_transport_write(cobs_tx_buffer, total_data_to_send);
	if (bytes_sent != total_data_to_send)
	{
		return AUD_ERR_SYSTEM;
	}

	return AUD_SUCCESS;
}

aud_error_t aud_configure_dhcp()
{
	aud_error_t result;
	uhip_packet_write_info_t uhip_winfo;
	ddp_packet_write_info_t ddp_winfo;
	ddp_request_id_t const config_dhcp_request_id = 11;
	uint8_t *ddp_tx_buffer_ptr;
	size_t cobs_encoded_size, total_data_to_send, bytes_sent;
	network_ip_config_t ip_config;

#ifdef DANTE_DEBUG
	printf("UHIP:Sending DDP network config request to configure DHCP\n");
#endif	//	DANTE_DEBUG

	hostcpu_uhip_set_ddp_tx_request_id(config_dhcp_request_id);

	// 1. Allocate room to store the UHIP header before adding the DDP data
	ddp_tx_buffer_ptr = &tx_buffer[sizeof(uhip_header_t)];

	// 2. Intialize the DDP packet buffer
	result = ddp_packet_init_write(&ddp_winfo, (uint32_t *)ddp_tx_buffer_ptr, UHIP_PACKET_SIZE_MAX - sizeof(uhip_header_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 2.5 Populate the static IP parameters (maybe not necessary for DHCP)
	ip_config.ip_addr = 0;
	ip_config.subnet_mask = 0;
	ip_config.gateway = 0;

	// 3. Create the DDP netwok basic request message, we will not be setting a DNS and domain name
	result = ddp_add_network_config_request(&ddp_winfo, config_dhcp_request_id, NETWORK_INTERFACE_MODE_LINKLOCAL_DHCP, &ip_config, NULL, NULL);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 4. Initialize the UHIP packet buffer
	result = uhip_packet_init_write(&uhip_winfo, (uint32_t *)&tx_buffer, UHIP_PACKET_SIZE_MAX);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 5. Create the UHIP packet with the DDP packet created from step 4
	result = uhip_packet_write_dante_device_protcol(&uhip_winfo, (uint32_t *)ddp_tx_buffer_ptr, ddp_packet_write_get_length_bytes(&ddp_winfo));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 6. COBS encode the UHIP+DDP packet from step 6. Ensure to keep the first byte free in the buffer that
	// stores the COBS encoded data to add the first delimiter byte
	cobs_encoded_size = cobs_encode(tx_buffer, uhip_winfo.curr_length_bytes, &cobs_tx_buffer[1]);

	// 7. Add the delimeter bytes to frame and add pad bytes to make the data to be sent a multiple of 32-bytes
	total_data_to_send = prepare_tx_frame(cobs_tx_buffer, cobs_encoded_size);
	if (total_data_to_send == 0)
	{
		return AUD_ERR_SYSTEM;
	}

	// 8. Send the data to the Dante device
	bytes_sent = hostcpu_transport_write(cobs_tx_buffer, total_data_to_send);
	if (bytes_sent != total_data_to_send)
	{
		return AUD_ERR_SYSTEM;
	}

	return AUD_SUCCESS;
}

aud_error_t aud_reboot_ultimo()
{
	aud_error_t result;
	uhip_packet_write_info_t uhip_winfo;
	ddp_packet_write_info_t ddp_winfo;
	ddp_request_id_t const config_reboot_request_id = 12;
	uint8_t *ddp_tx_buffer_ptr;
	size_t cobs_encoded_size, total_data_to_send, bytes_sent;

#ifdef DANTE_DEBUG
	printf("UHIP:Sending DDP request to reboot device\n");
#endif	//	DANTE_DEBUG

	hostcpu_uhip_set_ddp_tx_request_id(config_reboot_request_id);

	// 1. Allocate room to store the UHIP header before adding the DDP data
	ddp_tx_buffer_ptr = &tx_buffer[sizeof(uhip_header_t)];

	// 2. Intialize the DDP packet buffer
	result = ddp_packet_init_write(&ddp_winfo, (uint32_t *)ddp_tx_buffer_ptr, UHIP_PACKET_SIZE_MAX - sizeof(uhip_header_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 3. Create the DDP network basic request message, we will not be setting a DNS and domain name
	result = ddp_add_device_reboot_request(&ddp_winfo, config_reboot_request_id, REBOOT_MODE_SOFTWARE, REBOOT_MODE_SOFTWARE);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 4. Initialize the UHIP packet buffer
	result = uhip_packet_init_write(&uhip_winfo, (uint32_t *)&tx_buffer, UHIP_PACKET_SIZE_MAX);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 5. Create the UHIP packet with the DDP packet created from step 4
	result = uhip_packet_write_dante_device_protcol(&uhip_winfo, (uint32_t *)ddp_tx_buffer_ptr, ddp_packet_write_get_length_bytes(&ddp_winfo));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 6. COBS encode the UHIP+DDP packet from step 6. Ensure to keep the first byte free in the buffer that
	// stores the COBS encoded data to add the first delimiter byte
	cobs_encoded_size = cobs_encode(tx_buffer, uhip_winfo.curr_length_bytes, &cobs_tx_buffer[1]);

	// 7. Add the delimeter bytes to frame and add pad bytes to make the data to be sent a multiple of 32-bytes
	total_data_to_send = prepare_tx_frame(cobs_tx_buffer, cobs_encoded_size);
	if (total_data_to_send == 0)
	{
		return AUD_ERR_SYSTEM;
	}

	// 8. Send the data to the Dante device
	bytes_sent = hostcpu_transport_write(cobs_tx_buffer, total_data_to_send);
	if (bytes_sent != total_data_to_send)
	{
		return AUD_ERR_SYSTEM;
	}

	return AUD_SUCCESS;
}


aud_error_t rquest_channel_status(uint16_t usFromChan, uint16_t usToChan)
{
	aud_error_t result;
	uhip_packet_write_info_t uhip_winfo;
	ddp_packet_write_info_t ddp_winfo;
	ddp_request_id_t const channel_status_request_id = 13;
	uint8_t *ddp_tx_buffer_ptr;
	size_t cobs_encoded_size, total_data_to_send, bytes_sent;

#ifdef DANTE_DEBUG
	printf("UHIP:Sending a DDP channel status request\n");
#endif	//	DANTE_DEBUG

	hostcpu_uhip_set_ddp_tx_request_id(channel_status_request_id);

	// 1. Allocate room to store the UHIP header before adding the DDP data
	ddp_tx_buffer_ptr = &tx_buffer[sizeof(uhip_header_t)];

	// 2. Intialize the DDP packet buffer
	result = ddp_packet_init_write(&ddp_winfo, (uint32_t *)ddp_tx_buffer_ptr, UHIP_PACKET_SIZE_MAX - sizeof(uhip_header_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 3. Create the DDP audio sample rate config request message, we will be setting the audio sample rate to 44.1 KHz
	result = ddp_add_routing_rx_chan_config_state_request(&ddp_winfo, channel_status_request_id, usFromChan, usToChan);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 4. Initialize the UHIP packet buffer
	result = uhip_packet_init_write(&uhip_winfo, (uint32_t *)&tx_buffer, UHIP_PACKET_SIZE_MAX);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 5. Create the UHIP packet with the DDP packet created from step 3
	result = uhip_packet_write_dante_device_protcol(&uhip_winfo, (uint32_t *)ddp_tx_buffer_ptr, ddp_packet_write_get_length_bytes(&ddp_winfo));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 6. COBS encode the UHIP+DDP packet from step 5. Ensure to keep the first byte free in the buffer that
	// stores the COBS encoded data to add the first delimiter byte
	cobs_encoded_size = cobs_encode(tx_buffer, uhip_winfo.curr_length_bytes, &cobs_tx_buffer[1]);

	// 7. Add the delimeter bytes to frame and add pad bytes to make the data to be sent a multiple of 32-bytes
	total_data_to_send = prepare_tx_frame(cobs_tx_buffer, cobs_encoded_size);
	if (total_data_to_send == 0)
	{
		return AUD_ERR_SYSTEM;
	}

	// 8. Send the data to the Dante device
	bytes_sent = hostcpu_transport_write(cobs_tx_buffer, total_data_to_send);
	if (bytes_sent != total_data_to_send)
	{
		return AUD_ERR_SYSTEM;
	}

	return AUD_SUCCESS;
}
