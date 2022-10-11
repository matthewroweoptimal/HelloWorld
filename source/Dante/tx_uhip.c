/**
 * File     : example_tx_uhip.c
 * Created  : October 2014
 * Updated  : 2014/10/02
 * Author   : Michael Ung, Chamira Perera
 * Synopsis : UHIP TX helper functions
 *
 * Copyright 2005-2014, Audinate Pty Ltd and/or its licensors
 * Confidential
 * Audinate Copyright Header Version 1
 */

#include "uhip.h"
#include "tx_uhip.h"
#include "hostcpu_transport.h"
#include "cobs.h"
#include "frame.h"
#include "uhip_structures.h"
#include "uhip_packet_write.h"
#include "uhip_hostcpu_tx_timer.h"
#include "tx_ddp.h"
#include "tx_pb.h"

uint8_t __attribute__ ((aligned (4))) tx_buffer[UHIP_PACKET_SIZE_MAX];
// jeffwh - 16 Sept 2015 - Fixing 32 bit alignment problem
//uint8_t cobs_tx_buffer[_COBS_PAD(UHIP_PACKET_SIZE_MAX) + UHIP_PACKET_SIZE_MAX + COBS_ZERO_PAIR_NUM];
uint8_t cobs_tx_buffer[((_COBS_PAD(UHIP_PACKET_SIZE_MAX) + UHIP_PACKET_SIZE_MAX + COBS_ZERO_PAIR_NUM)+3)&0xFFFFFFFC];

// Forward declarations
size_t add_delim_pad_data(uint8_t *cobs_buffer, size_t cobs_encoded_size);
aud_error_t send_uhip_protocol_control(UhipProtocolControlType type);

extern char *g_pszNewFriendlyName;


static void tx_success()
{
	//change the state - we are now waiting on an ACK
	hostcpu_uhip_set_state(HOSTCPU_UHIP_STATE_NO_TX_WAIT_ACK);

	//start the ACK timer
	uhip_hostcpu_tx_timer_start(TX_ACK_TIMEOUT_MILLISECONDS);
}

/**
 * Handles any pending TX messages
 */
void handle_uhip_tx()
{
	//send any pending ACK's
	if (hostcpu_uhip_is_tx_flag_set(HOSTCPU_UHIP_TX_PROT_CTRL_ACK_FLAG))
	{
		if (send_uhip_protocol_control(UhipProtocolControlType_ACK_SUCCESS) == AUD_SUCCESS)
		{
			hostcpu_uhip_increment_num_tx_prot_ctrl_pkts();

			//clear the flag
			hostcpu_uhip_clear_tx_flag(HOSTCPU_UHIP_TX_PROT_CTRL_ACK_FLAG);
		}
	}

	//if we are not in a normal state we can't send a non-protocl control message
	if (hostcpu_uhip_get_state() != HOSTCPU_UHIP_STATE_NORMAL) {
		return;
	}
	
	//send packets based on the tx flags bitmask
	if (hostcpu_uhip_is_tx_flag_set(HOSTCPU_UHIP_TX_CMC_PB_FLAG))
	{
		if (send_conmon_packetbridge_packet() == AUD_SUCCESS)
		{
			hostcpu_uhip_increment_num_tx_cmc_pb_pkts();

			//clear the flag
			hostcpu_uhip_clear_tx_flag(HOSTCPU_UHIP_TX_CMC_PB_FLAG);

			//change the state and start the tx timer
			tx_success();
		}
	}
	else if (hostcpu_uhip_is_tx_flag_set(HOSTCPU_UHIP_TX_UDP_PB_FLAG))
	{
		if (send_udp_packetbridge_packet() == AUD_SUCCESS)
		{
			hostcpu_uhip_increment_num_tx_udp_pb_pkts();

			//clear the flag
			hostcpu_uhip_clear_tx_flag(HOSTCPU_UHIP_TX_UDP_PB_FLAG);

			//change the state and start the tx timer
			tx_success();
		}
	}
	else if (hostcpu_uhip_is_tx_flag_set(HOSTCPU_UHIP_TX_DDP_NET_IF_QUERY_FLAG))
	{
		if (send_network_interface_query() == AUD_SUCCESS) 
		{
#ifdef DANTE_DEBUG
			printf("UHIP:Sent UHIP HOSTCPU_UHIP_TX_DDP_NET_IF_QUERY command\n");
#endif	//	DANTE_DEBUG
			//clear the flag
			hostcpu_uhip_clear_tx_flag(HOSTCPU_UHIP_TX_DDP_NET_IF_QUERY_FLAG);

			//change the state and start the tx timer
			tx_success();
		}
	}
	else if (hostcpu_uhip_is_tx_flag_set(HOSTCPU_UHIP_TX_DDP_CONFIG_STATIC_IP_FLAG))
	{
		if (configure_static_ip() == AUD_SUCCESS)
		{
			//clear the flag
			hostcpu_uhip_clear_tx_flag(HOSTCPU_UHIP_TX_DDP_CONFIG_STATIC_IP_FLAG);

			//change the state and start the tx timer
			tx_success();
		}
	}
	else if (hostcpu_uhip_is_tx_flag_set(HOSTCPU_UHIP_TX_DDP_SUB_RX_CHANS_FLAG))
	{
		if (subscribe_channels() == AUD_SUCCESS)
		{
			//clear the flag
			hostcpu_uhip_clear_tx_flag(HOSTCPU_UHIP_TX_DDP_SUB_RX_CHANS_FLAG);

			//change the state and start the tx timer
			tx_success();
		}
	}
	else if (hostcpu_uhip_is_tx_flag_set(HOSTCPU_UHIP_TX_DDP_ADD_MCAST_TX_FLAG))
	{
		if (create_tx_multicast_flow() == AUD_SUCCESS)
		{
			//clear the flag
			hostcpu_uhip_clear_tx_flag(HOSTCPU_UHIP_TX_DDP_ADD_MCAST_TX_FLAG);

			//change the state and start the tx timer
			tx_success();
		}
	}
	else if (hostcpu_uhip_is_tx_flag_set(HOSTCPU_UHIP_TX_DDP_DELETE_FLOW_FLAG))
	{
		if (delete_flow() == AUD_SUCCESS)
		{
			//clear the flag
			hostcpu_uhip_clear_tx_flag(HOSTCPU_UHIP_TX_DDP_DELETE_FLOW_FLAG);

			//change the state and start the tx timer
			tx_success();
		}
	}
	else if (hostcpu_uhip_is_tx_flag_set(HOSTCPU_UHIP_TX_DDP_UPGRADE_XMODEM_FLAG))
	{
		if (upgrade_via_xmodem() == AUD_SUCCESS)
		{
			//clear the flag
			hostcpu_uhip_clear_tx_flag(HOSTCPU_UHIP_TX_DDP_UPGRADE_XMODEM_FLAG);

			//change the state and start the tx timer
			tx_success();
		}
	}
	else if (hostcpu_uhip_is_tx_flag_set(HOSTCPU_UHIP_TX_DDP_CONFIG_SRATE_FLAG))
	{
		if (configure_sample_rate() == AUD_SUCCESS)
		{
			//clear the flag
			hostcpu_uhip_clear_tx_flag(HOSTCPU_UHIP_TX_DDP_CONFIG_SRATE_FLAG);

			//change the state and start the tx timer
			tx_success();
		}
	}
	else if (hostcpu_uhip_is_tx_flag_set(HOSTCPU_UHIP_TX_DDP_CONFIG_CLOCK_PULLUP_FLAG))
	{
		if (configure_clock_pullup() == AUD_SUCCESS)
		{
			//clear the flag
			hostcpu_uhip_clear_tx_flag(HOSTCPU_UHIP_TX_DDP_CONFIG_CLOCK_PULLUP_FLAG);

			//change the state and start the tx timer
			tx_success();
		}
	}
	else if (hostcpu_uhip_is_tx_flag_set(HOSTCPU_UHIP_TX_DDP_DEV_GEN_QUERY_FLAG))
	{
		if (send_device_general_query() == AUD_SUCCESS)
		{
#ifdef DANTE_DEBUG
			printf("UHIP:Sent UHIP HOSTCPU_UHIP_TX_DDP_DEV_GEN_QUERY command\n");
#endif	//	DANTE_DEBUG
			//clear the flag
			hostcpu_uhip_clear_tx_flag(HOSTCPU_UHIP_TX_DDP_DEV_GEN_QUERY_FLAG);

			//change the state and start the tx timer
			tx_success();
		}
	}
	else if (hostcpu_uhip_is_tx_flag_set(HOSTCPU_UHIP_TX_DDP_DEV_MANUF_INFO_FLAG))
	{
		if (send_device_manuf_info() == AUD_SUCCESS)
		{
#ifdef DANTE_DEBUG
			printf("UHIP:Sent UHIP HOSTCPU_UHIP_TX_DDP_DEV_MANUF_INFO command\n");
#endif	//	DANTE_DEBUG
			//clear the flag
			hostcpu_uhip_clear_tx_flag(HOSTCPU_UHIP_TX_DDP_DEV_MANUF_INFO_FLAG);

			//change the state and start the tx timer
			tx_success();
		}
	}
	else if (hostcpu_uhip_is_tx_flag_set(HOSTCPU_UHIP_TX_DDP_DEV_ID_FLAG))
	{
		if (send_device_dev_id(g_pszNewFriendlyName) == AUD_SUCCESS)
		{
#ifdef DANTE_DEBUG
			printf("UHIP:Sent UHIP HOSTCPU_UHIP_TX_DDP_DEV_ID command\n");
#endif	//	DANTE_DEBUG
			//clear the flag
			hostcpu_uhip_clear_tx_flag(HOSTCPU_UHIP_TX_DDP_DEV_ID_FLAG);

			//change the state and start the tx timer
			tx_success();
		}
	}
	else if (hostcpu_uhip_is_tx_flag_set(HOSTCPU_UHIP_TX_DDP_CHANNEL_STATUS_FLAG))
	{
		if (rquest_channel_status(1, 2) == AUD_SUCCESS)
		{
#ifdef DANTE_DEBUG
			printf("UHIP:Sent UHIP HOSTCPU_UHIP_TX_CHANNEL_STATUS command\n");
#endif	//	DANTE_DEBUG
			//clear the flag
			hostcpu_uhip_clear_tx_flag(HOSTCPU_UHIP_TX_DDP_CHANNEL_STATUS_FLAG);

			//change the state and start the tx timer
			tx_success();
		}
	}
}

/**
* Sends a UHIP protocol control message 
* @param type [in] The type of protocol control message to send @see UhipProtocolControlType
* @return AUD_SUCCESS if the packet was sucessfully created and transmitted, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t send_uhip_protocol_control(UhipProtocolControlType type)
{
	aud_error_t result;
	uhip_packet_write_info_t uhip_winfo;
	size_t cobs_encoded_size, total_data_to_send, bytes_sent;

	// 1. Initialize the UHIP packet buffer
	result = uhip_packet_init_write(&uhip_winfo, (uint32_t *)&tx_buffer, sizeof(tx_buffer));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 2. Create the UHIP packet with the DDP packet created from step 3
	result = uhip_packet_write_protocol_control(&uhip_winfo, type);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	// 3. COBS encode the UHIP packet from step 2. Ensure to keep the first byte free in the buffer that
	// stores the COBS encoded data to add the first delimiter byte
	cobs_encoded_size = cobs_encode(tx_buffer, uhip_winfo.curr_length_bytes, &cobs_tx_buffer[1]);

	// 4. Add the delimeter bytes to frame and add pad bytes to make the data to be sent a multiple of 32-bytes
	total_data_to_send = prepare_tx_frame(cobs_tx_buffer, cobs_encoded_size);
	if (total_data_to_send == 0)
	{
		return AUD_ERR_SYSTEM;
	}

	// 5. Send the data to the Dante device
	bytes_sent = hostcpu_transport_write(cobs_tx_buffer, total_data_to_send);
	if (bytes_sent != total_data_to_send)
	{
		return AUD_ERR_SYSTEM;
	}

	return AUD_SUCCESS;
}
