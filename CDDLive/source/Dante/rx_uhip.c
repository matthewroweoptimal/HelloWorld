/**
 * File     : example_rx_uhip.h
 * Created  : October 2014
 * Updated  : 2014/10/02
 * Author   : Michael Ung
 * Synopsis : UHIP RX helper functions
 *
 * Copyright 2005-2014, Audinate Pty Ltd and/or its licensors
 * Confidential
 * Audinate Copyright Header Version 1
 */

#include "uhip.h"
#include "rx_uhip.h"

#include "cobs.h"
#include "inc/packet.h"
#include "inc/util.h"
#include "rx_ddp.h"
#include "frame.h"
#include "uhip_structures.h"
#include "uhip_packet_read.h"
#include "uhip_util.h"

#include "hostcpu_transport.h"
#include "uhip_hostcpu_rx_timer.h"
#include "uhip_hostcpu_tx_timer.h"

//rx cobs decoded buffer
typedef struct uhip_rx_buffer
{
	//NOTE: we use the union to force 32-bit alignment
	union
	{
		uint32_t	alignment;
		uint8_t		buffer[UHIP_PACKET_SIZE_MAX];
	} _;
} uhip_rx_buffer_t;

static uhip_rx_buffer_t __attribute__ ((section(".external_ram"))) rx_buffer;

unsigned char g_dante_comm_error_detect = 0;

//raw rx cobs encoded buffer
//static uint8_t cobs_rx_buffer[_COBS_PAD(UHIP_PACKET_SIZE_MAX) + UHIP_PACKET_SIZE_MAX + COBS_ZERO_PAIR_NUM];
// jeffwh - 16 Sep 2015 - fixing 32 bit alignment
uint8_t cobs_rx_buffer[((_COBS_PAD(UHIP_PACKET_SIZE_MAX) + UHIP_PACKET_SIZE_MAX + COBS_ZERO_PAIR_NUM)+3)&0xFFFFFFFC];

unsigned char dante_comm_error_detect()
{
	unsigned char err_detect = g_dante_comm_error_detect;
	g_dante_comm_error_detect = 0;
	return(err_detect);
}

/**
* Resets the RX buffers
*/
void reset_uhip_rx_buffers(void)
{
	memset(&rx_buffer, 0x00, sizeof(uhip_rx_buffer_t));
	memset(cobs_rx_buffer, 0x00, sizeof(cobs_rx_buffer));
}

//process the rx packet
unsigned char process_cobs_rx_packet(uint8_t* rx_pkt_buffer, uint16_t rx_packet_size_bytes)
{
	aud_error_t result;
	uint16_t uncobs_size = 0;
	uhip_packet_read_info_t uhip_read_info;
	UhipPacketType uhip_packet_type;

	//uncobs the packet
	uncobs_size = cobs_decode(rx_pkt_buffer, rx_packet_size_bytes, rx_buffer._.buffer, UHIP_PACKET_SIZE_MAX);

	//check for a corrupted packet
	if (uncobs_size == 0) 
	{
		hostcpu_uhip_increment_num_rx_cobs_decode_errs();
#ifdef DANTE_DEBUG_FINE
		printf("UHIP:!ERROR COBS decode error - discarding packet x%x bytes (%08X)!\n", rx_packet_size_bytes, (uint32_t)rx_pkt_buffer );
		printf("Error Data:");
		for(int nByte=0;nByte<rx_packet_size_bytes;nByte++)
		{
			printf("%02x,", *(rx_pkt_buffer+nByte));
		}
		printf("\n");
#else	//	DANTE_DEBUG_FINE
		printf("UHIP:decode!\n");
#endif	//	DANTE_DEBUG_FINE
		g_dante_comm_error_detect = 1;
		goto l__cleanup_return;
	}
#ifdef DANTE_DEBUG_FINE
	printf("Good DECODE of %d bytes!\n", rx_packet_size_bytes);
#endif	//	DANTE_DEBUG_FINE

	//read the UHIP header
	result = uhip_packet_init_read(&uhip_read_info, (uint32_t *)rx_buffer._.buffer, uncobs_size);
	if (result != AUD_SUCCESS) {
		hostcpu_uhip_increment_num_rx_malformed_errs();
#ifdef DANTE_DEBUG_FINE
		printf("UHIP:!ERROR UHIP malformed [uhip_packet_init_read()] - discarding packet!\n");
#else	//	DANTE_DEBUG_FINE
		printf("UHIP:malformed-1!\n");
#endif	//	DANTE_DEBUG_FINE
		g_dante_comm_error_detect = 1;
		goto l__cleanup_return;
	}

	//parse based on packet type
	result = uhip_packet_read_get_type(&uhip_read_info, &uhip_packet_type);
	if (result != AUD_SUCCESS) {
		hostcpu_uhip_increment_num_rx_malformed_errs();
#ifdef DANTE_DEBUG_FINE
		printf("UHIP:!ERROR UHIP malformed [uhip_packet_read_get_type()] - discarding packet!\n");
#else	//	DANTE_DEBUG_FINE
		printf("UHIP:malformed-2!\n");
#endif	//	DANTE_DEBUG_FINE
		g_dante_comm_error_detect = 1;
		goto l__cleanup_return;
	}

//	printf("*");
#ifdef DANTE_DEBUG_FINE
	printf("$uhip_packet_type=%d\n", uhip_packet_type);
#endif	//	DANTE_DEBUG_FINE
	switch (uhip_packet_type)
	{
		case UhipPacketType_PROTOCOL_CONTROL:
		{
			UhipProtocolControlType uhip_prot_control_type;

			//parse the protocol control packet
			if (uhip_packet_read_protocol_control(&uhip_read_info, &uhip_prot_control_type) == AUD_SUCCESS) 
			{
				hostcpu_uhip_increment_num_rx_prot_ctrl_pkts();
#ifdef DANTE_DEBUG
				printf("UHIP:Received UHIP Protocol Control type=%s(%d)\n", uhip_protocol_control_type_to_string(uhip_prot_control_type), uhip_prot_control_type);
#endif	//	DANTE_DEBUG

				switch (uhip_prot_control_type)
				{
					case UhipProtocolControlType_ACK_SUCCESS:
					case UhipProtocolControlType_ERROR_OTHER:
					case UhipProtocolControlType_ERROR_MALFORMED_PACKET:
					case UhipProtocolControlType_ERROR_UNKNOWN_TYPE:
					case UhipProtocolControlType_ERROR_NETWORK:
					case UhipProtocolControlType_ERROR_TIMEOUT:
					{
						//stop the tx timer as we have received an ACK and set the UHIP state back to normal
						uhip_hostcpu_tx_timer_stop();
						hostcpu_uhip_set_state(HOSTCPU_UHIP_STATE_NORMAL);
					}
					break;
					default:
						break;
				}
			}
		}
		break;

		case UhipPacketType_CONMON_PACKET_BRIDGE:
		{
			UhipConMonPacketBridgeChannel cmc_pb_chan;
			uint8_t* payload;
			uint16_t payload_length;
			int i;

			//trigger an ACK
			hostcpu_uhip_set_tx_flag(HOSTCPU_UHIP_TX_PROT_CTRL_ACK_FLAG);

#ifdef DANTE_DEBUG	//	Don't need this info, no sense taking the risk of reading it in case it is corrupt
			//parse the packet
			if (uhip_packet_read_cmc_packet_bridge(&uhip_read_info, &cmc_pb_chan, &payload, &payload_length) == AUD_SUCCESS)
			{
				hostcpu_uhip_increment_num_rx_cmc_pb_pkts();

				//print header
#ifdef DANTE_DEBUG
				printf("UHIP:Received ConMon Packet Bridge Packet chan=%s payload_length=%u payload=\n", 
					uhip_cmc_pb_chan_to_string(cmc_pb_chan), 
					payload_length);
#endif	//	DANTE_DEBUG

				//print payload in hex ascii
				for (i = 0; i<payload_length; i += 16)
				{
					int j;
					for (j = 0; j<16; j++)
					{
						if ((i + j) < payload_length)
						{
							printf("[%.2x] ", payload[i + j]);
						}
					}
					printf("\n");
				}
			}
#endif	//	DANTE_DEBUG
		}
		break;

		case UhipPacketType_UDP_PACKET_BRIDGE:
		{
			UhipUDPPacketBridgeChannel udp_pb_chan;
			uint16_t remote_port;
			uint32_t remote_ip;
			uint8_t* payload;
			uint16_t payload_length;
			int i;

			//trigger an ACK
			hostcpu_uhip_set_tx_flag(HOSTCPU_UHIP_TX_PROT_CTRL_ACK_FLAG);

#ifdef DANTE_DEBUG	//	Don't need this info, no sense taking the risk of reading it in case it is corrupt
			//parse the packet
			if (uhip_packet_read_udp_packet_bridge(&uhip_read_info, &udp_pb_chan, &remote_port, &remote_ip, &payload, &payload_length) == AUD_SUCCESS)
			{
				hostcpu_uhip_increment_num_rx_udp_pb_pkts();

				//print header
#ifdef DANTE_DEBUG
				printf("UHIP:Received UDP Packet Bridge Packet chan=%s remote_port=%u remote_ip=%u.%u.%u.%u payload_length=%u payload=\n", 
					uhip_udp_pb_chan_to_string(udp_pb_chan),
					remote_port,
					(remote_ip & 0xFF000000) >> 24,
					(remote_ip & 0x00FF0000) >> 16,
					(remote_ip & 0x0000FF00) >> 8,
					(remote_ip & 0x000000FF),
					payload_length
				);

				//print payload in hex ascii
				for (i = 0; i<payload_length; i += 16)
				{
					int j;
					for (j = 0; j<16; j++)
					{
						if ((i + j) < payload_length)
						{
							printf("[%.2x] ", payload[i + j]);
						}
					}
					printf("\n");
				}
#endif	//	DANTE_DEBUG

			}
#endif	//	DANTE_DEBUG
		}
		break;

		case UhipPacketType_DANTE_EVENTS:
		{
			ddp_packet_read_info_t ddp_rinfo;
			ddp_message_read_info_t ddp_msg_info;
			uint16_t packet_offset = 0;

			//trigger an ACK
			hostcpu_uhip_set_tx_flag(HOSTCPU_UHIP_TX_PROT_CTRL_ACK_FLAG);

			//parse the Dante Event message
			result = ddp_packet_init_read(&ddp_rinfo, (uint32_t *)(rx_buffer._.buffer + sizeof(uhip_header_t)+sizeof(ddp_packet_header_t)),
				sizeof(rx_buffer._.buffer) - sizeof(uhip_header_t));
			if (result != AUD_SUCCESS)
			{
				AUD_PRINTF("UHIP:ddp_packet_init_read failed!/n");
				g_dante_comm_error_detect = 1;
				goto l__cleanup_return;
			}
			hostcpu_uhip_increment_num_rx_dante_evnt_pkts();
			result = ddp_packet_read_message(&ddp_rinfo, &ddp_msg_info, packet_offset);

			//read every TLV block in the packet
			while (result == AUD_SUCCESS)
			{
				switch (ddp_msg_info.opcode)
				{
					case DDP_OP_LOCAL_AUDIO_FORMAT:
						result = handle_dante_event_audio_format(&ddp_rinfo, packet_offset);
						break;

					case DDP_OP_LOCAL_CLOCK_PULLUP:
						result = handle_dante_event_clock_pullup(&ddp_rinfo, packet_offset);
						break;

					default:
						printf("Unrecognized Dante Event opcode 0x%04x\n", ddp_msg_info.opcode);
						break;
				}//switch (ddp_msg_info.opcode)

				if (AUD_SUCCESS != result)
				{
					g_dante_comm_error_detect = 1;
					goto l__cleanup_return;
				}

				//jump to the next TLV block
				packet_offset += ddp_msg_info.length_bytes;
				result = ddp_packet_read_message(&ddp_rinfo, &ddp_msg_info, packet_offset);
			} //while (result == AUD_SUCCESS)
		}
		break;
		
		case UhipPacketType_DDP:
		{
			ddp_packet_read_info_t ddp_rinfo;
			ddp_message_read_info_t ddp_msg_info;
			ddp_request_id_t response_request_id;
			ddp_status_t response_status;
			uint16_t packet_offset = 0;

			//trigger an ACK
			hostcpu_uhip_set_tx_flag(HOSTCPU_UHIP_TX_PROT_CTRL_ACK_FLAG);

			//parse the DDP message
			result = ddp_packet_init_read(&ddp_rinfo, (uint32_t *)(rx_buffer._.buffer + sizeof(uhip_header_t) + sizeof(ddp_packet_header_t)), 
				sizeof(rx_buffer._.buffer) - sizeof(uhip_header_t));
			if (result != AUD_SUCCESS)
			{
				AUD_PRINTF("UHIP:ddp_packet_init_read failed!/n");
				g_dante_comm_error_detect = 1;
				goto l__cleanup_return;
			}
			hostcpu_uhip_increment_num_rx_ddp_pkts();
			result = ddp_packet_read_message(&ddp_rinfo, &ddp_msg_info, packet_offset);

			//read every TLV block in the packet
			while (result == AUD_SUCCESS)
			{
				//check the status for any errors
				response_status = ddp_packet_read_response_status(&ddp_msg_info);
				if (response_status != DDP_STATUS_NOERROR) {
					AUD_PRINTF("ERROR DDP message received with status %s for opcode %s - discarding packet\n", ddp_status_to_string(response_status), ddp_opcode_to_string(ddp_msg_info.opcode));
					g_dante_comm_error_detect = 1;
					goto l__cleanup_return;
				}

				//read the request id
				response_request_id = ddp_packet_read_response_request_id(&ddp_msg_info);
				AUD_PRINTF("\nReceived %s for %s\n", (response_request_id) ? "response" : "event", ddp_opcode_to_string(ddp_msg_info.opcode));
				if (response_request_id)
				{
					if (response_request_id != hostcpu_uhip_get_ddp_tx_request_id())
					{
						AUD_PRINTF("ERROR DDP request ID in response does not match with request message - discarding packet\n");
						g_dante_comm_error_detect = 1;
						goto l__cleanup_return;
					}

					hostcpu_uhip_set_ddp_tx_request_id(0);
				}


#if 1 //def DANTE_DEBUG_FINE
				if(ddp_msg_info.opcode != 0x1100)  // avoid normal opcode print to avoid screen clutter
					printf("opcode=%04x - %s\n", ddp_msg_info.opcode, ddp_opcode_to_string(ddp_msg_info.opcode));
#endif	//	DANTE_DEBUG_FINE
				switch (ddp_msg_info.opcode)
				{
				case DDP_OP_DEVICE_GENERAL:
					result = handle_ddp_device_general(&ddp_rinfo, packet_offset);
					break;
				case DDP_OP_DEVICE_MANF:
					result = handle_ddp_device_manuf_info(&ddp_rinfo, packet_offset);
					break;
				case DDP_OP_DEVICE_UPGRADE:
					result = handle_ddp_device_upgrade(&ddp_rinfo, packet_offset);
					break;
				case DDP_OP_DEVICE_ERASE_CONFIG:
					result = handle_ddp_device_erase_config(&ddp_rinfo, packet_offset);
					break;
				case DDP_OP_DEVICE_REBOOT:
					result = handle_ddp_device_reboot(&ddp_rinfo, packet_offset);
					break;
				case DDP_OP_DEVICE_IDENTITY:
					result = handle_ddp_device_id(&ddp_rinfo, packet_offset);
					break;

#if (defined UHIP_HOSTCPU_API_SUPPORT_DDP_DEVICE_IDENTIFY)
				case DDP_OP_DEVICE_IDENTIFY:
					result = handle_ddp_device_identify(&ddp_rinfo, packet_offset);
					break;
#endif //UHIP_HOSTCPU_API_SUPPORT_DDP_DEVICE_IDENTIFY
#if (defined UHIP_HOSTCPU_API_SUPPORT_DDP_DEVICE_GPIO)
				case DDP_OP_DEVICE_GPIO:
					result = handle_ddp_device_gpio(&ddp_rinfo, packet_offset);
					break;
#endif //UHIP_HOSTCPU_API_SUPPORT_DDP_DEVICE_GPIO
#if (defined UHIP_HOSTCPU_API_SUPPORT_DDP_DEVICE_SWITCH_LED)
				case DDP_OP_DEVICE_SWITCH_LED:
					result = handle_ddp_device_switch_led(&ddp_rinfo, packet_offset);
					break;
#endif //UHIP_HOSTCPU_API_SUPPORT_DDP_DEVICE_SWITCH_LED

				case DDP_OP_NETWORK_BASIC:
					result = handle_ddp_network_basic(&ddp_rinfo, packet_offset);
					break;
				case DDP_OP_NETWORK_CONFIG:
					result = handle_ddp_network_config(&ddp_rinfo, packet_offset);
					break;

				case DDP_OP_CLOCK_BASIC:
					result = handle_ddp_clock_basic(&ddp_rinfo, packet_offset);
					break;
				case DDP_OP_CLOCK_CONFIG:
					result = handle_ddp_clock_config(&ddp_rinfo, packet_offset);
					break;
				case DDP_OP_CLOCK_PULLUP:
					result = handle_ddp_clock_pullup(&ddp_rinfo, packet_offset);
					break;

				case DDP_OP_AUDIO_BASIC:
					result = handle_ddp_audio_basic(&ddp_rinfo, packet_offset);
					break;
				case DDP_OP_AUDIO_SRATE_CONFIG:
					result = handle_ddp_audio_srate(&ddp_rinfo, packet_offset);
					break;
				case DDP_OP_AUDIO_ENC_CONFIG:
					result = handle_ddp_audio_enc(&ddp_rinfo, packet_offset);
					break;

				case DDP_OP_ROUTING_BASIC:
					result = handle_ddp_routing_basic(&ddp_rinfo, packet_offset);
					break;
				case DDP_OP_ROUTING_READY_STATE:
					result = handle_ddp_routing_ready_state(&ddp_rinfo, packet_offset);
					break;
				case DDP_OP_ROUTING_PERFORMANCE_CONFIG:
					result = handle_ddp_routing_performance_config(&ddp_rinfo, packet_offset);
					break;
				case DDP_OP_ROUTING_RX_CHAN_CONFIG_STATE:
					result = handle_ddp_routing_rx_chan_config_state(&ddp_rinfo, packet_offset);
					break;
				case DDP_OP_ROUTING_RX_SUBSCRIBE_SET:
					result = handle_ddp_routing_rx_subscribe_set(&ddp_rinfo, packet_offset);
					break;
				case DDP_OP_ROUTING_RX_CHAN_LABEL_SET:
					result = handle_ddp_routing_rx_chan_label_set(&ddp_rinfo, packet_offset);
					break;
				case DDP_OP_ROUTING_TX_CHAN_CONFIG_STATE:
					result = handle_ddp_routing_tx_chan_config_state(&ddp_rinfo, packet_offset);
					break;
				case DDP_OP_ROUTING_TX_CHAN_LABEL_SET:
					result = handle_ddp_routing_tx_chan_label_set(&ddp_rinfo, packet_offset);
					break;
				case DDP_OP_ROUTING_RX_FLOW_CONFIG_STATE:
					result = handle_ddp_routing_rx_flow_config_state(&ddp_rinfo, packet_offset);
					break;
				case DDP_OP_ROUTING_MCAST_TX_FLOW_CONFIG_SET:
					result = handle_ddp_routing_multicast_tx_flow_config(&ddp_rinfo, packet_offset);
					break;
				case DDP_OP_ROUTING_TX_FLOW_CONFIG_STATE:
					result = handle_ddp_routing_tx_flow_config_state(&ddp_rinfo, packet_offset);
					break;
				case DDP_OP_ROUTING_RX_CHAN_STATUS:
					result = handle_ddp_routing_rx_chan_status(&ddp_rinfo, packet_offset);
					break;
				case DDP_OP_ROUTING_RX_FLOW_STATUS:
					result = handle_ddp_routing_rx_flow_status(&ddp_rinfo, packet_offset);
					break;
				case DDP_OP_ROUTING_RX_UNSUB_CHAN:
					result = handle_ddp_routing_rx_unsub_chan(&ddp_rinfo, packet_offset);
					break;
				case DDP_OP_ROUTING_FLOW_DELETE:
					result = handle_ddp_routing_flow_delete(&ddp_rinfo, packet_offset);
					break;
				default:
					printf("UHIP!Unrecognized DDP opcode 0x%04x!\n", ddp_msg_info.opcode);
					break;
				} //switch (ddp_msg_info.opcode)

#ifdef DANTE_DEBUG
				printf("\n");
#endif	//	DANTE_DEBUG
				if (AUD_SUCCESS!=result)
				{
					g_dante_comm_error_detect = 1;
					goto l__cleanup_return;
				}

				//jump to the next TLV block
				packet_offset += ddp_msg_info.length_bytes;
				result = ddp_packet_read_message(&ddp_rinfo, &ddp_msg_info, packet_offset);
			} //while (result == AUD_SUCCESS)
		}
		break;
		default:
			g_dante_comm_error_detect = 1;
			goto l__cleanup_return;
			break;
	}

	reset_uhip_rx_buffers();
	return(1);

l__cleanup_return:
	//reset the uhip buffers
	reset_uhip_rx_buffers();
	return(0);
}
