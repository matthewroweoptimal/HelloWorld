/**
 * File     : example_uhip_main.c
 * Created  : October 2014
 * Updated  : 2014/10/02
 * Author   : Chamira Perera, Michael Ung
 * Synopsis : HostCPU UHIP API main
 *
 * Copyright 2005-2014, Audinate Pty Ltd and/or its licensors
 * Confidential
 * Audinate Copyright Header Version 1
 */

#include "uhip.h"
#include "tx_uhip.h"
#include "rx_uhip.h"
#include "frame.h"

#include "uhip_hostcpu_rx_timer.h"
#include "uhip_hostcpu_tx_timer.h"
#include "hostcpu_transport.h"

char *g_pszNewFriendlyName = 0;

/**
 * Contextual data used across the HostCPU UHIP
 */
static hostcpu_uhip_t g_hostcpu_uhip;

/**
 * Checks the rx and tx UHIP timers
 */
void check_uhip_timers(void)
{
	//If the RX timer has fired / expired
	if (uhip_hostcpu_rx_timer_has_fired()) 
	{
		//increment the counters
		g_hostcpu_uhip.stats.num_rx_timeouts++;

		//we have probably lost synchronisation on the RX path. We need to reset the RX buffer.
		reset_uhip_rx_buffers();

		reset_cobs_state();

		//stop the rx timer
		uhip_hostcpu_rx_timer_stop();
	}
	
	//If the TX timer has fired / expired
	if (uhip_hostcpu_tx_timer_has_fired())
	{
		switch (g_hostcpu_uhip.state)
		{
			case HOSTCPU_UHIP_STATE_NO_TX_WAIT_ACK:
			{
				//stop the timer
				uhip_hostcpu_tx_timer_stop();

				//increment the counters
				g_hostcpu_uhip.stats.num_tx_timeouts++;

				//change the state, so we don't send until the timer expires again
				g_hostcpu_uhip.state = HOSTCPU_UHIP_STATE_NO_TX_RESYNC;

				//start the tx timer again
				uhip_hostcpu_tx_timer_start(TX_RESYNC_STOP_TIMEOUT_MILLISECONDS);
			}
			break;

			case HOSTCPU_UHIP_STATE_NO_TX_RESYNC:
			{
				//stop the tx timer
				uhip_hostcpu_tx_timer_stop();

//				g_hostcpu_uhip.tx_flags |= HOSTCPU_UHIP_TX_DDP_NET_IF_QUERY_FLAG;		//send a DDP network interface message - see send_network_interface_query()
				//change the state - it is OK to send now
				g_hostcpu_uhip.state = HOSTCPU_UHIP_STATE_NORMAL;
			}
			break;

			default:
			{
				printf("ERROR tx timer fired in unknown state %d\n", g_hostcpu_uhip.state);
			}
			break;
		}
	}
}

ddp_request_id_t hostcpu_uhip_get_ddp_tx_request_id()
{
	return g_hostcpu_uhip.ddp_tx_request_id;
}

void hostcpu_uhip_set_ddp_tx_request_id(ddp_request_id_t request_id)
{
	g_hostcpu_uhip.ddp_tx_request_id = request_id;
}

aud_bool_t hostcpu_uhip_is_tx_flag_set(hostcpu_uhip_tx_packet_flags_t tx_flag)
{
	return ((g_hostcpu_uhip.tx_flags & tx_flag) != 0);
}

void hostcpu_uhip_set_tx_flag(hostcpu_uhip_tx_packet_flags_t tx_flag)
{
	g_hostcpu_uhip.tx_flags |= tx_flag;
}

void hostcpu_uhip_clear_tx_flag(hostcpu_uhip_tx_packet_flags_t tx_flag)
{
	g_hostcpu_uhip.tx_flags &= ~tx_flag;
}

void hostcpu_uhip_increment_num_tx_prot_ctrl_pkts()
{
	g_hostcpu_uhip.stats.num_tx_prot_ctrl_pkts++;
}

void hostcpu_uhip_increment_num_tx_cmc_pb_pkts()
{
	g_hostcpu_uhip.stats.num_tx_cmc_pb_pkts++;
}

void hostcpu_uhip_increment_num_tx_udp_pb_pkts()
{
	g_hostcpu_uhip.stats.num_tx_udp_pb_pkts++;
}

void hostcpu_uhip_increment_num_rx_overflows()
{
	g_hostcpu_uhip.stats.num_rx_overflows++;
}

void hostcpu_uhip_increment_num_rx_ddp_pkts()
{
	g_hostcpu_uhip.stats.num_rx_ddp_pkts++;
}

void hostcpu_uhip_increment_num_rx_dante_evnt_pkts()
{
	g_hostcpu_uhip.stats.num_rx_dante_evnt_pkts++;
}

void hostcpu_uhip_increment_num_rx_udp_pb_pkts()
{
	g_hostcpu_uhip.stats.num_rx_udp_pb_pkts++;
}

void hostcpu_uhip_increment_num_rx_cmc_pb_pkts()
{
	g_hostcpu_uhip.stats.num_rx_cmc_pb_pkts++;
}

void hostcpu_uhip_increment_num_rx_malformed_errs()
{
	g_hostcpu_uhip.stats.num_rx_malformed_errs++;
}

void hostcpu_uhip_increment_num_rx_cobs_decode_errs()
{
	g_hostcpu_uhip.stats.num_rx_cobs_decode_errs++;
}

void hostcpu_uhip_increment_num_rx_prot_ctrl_pkts()
{
	g_hostcpu_uhip.stats.num_rx_prot_ctrl_pkts++;
}

hostcpu_uhip_state_t hostcpu_uhip_get_state()
{
	return g_hostcpu_uhip.state;
}

void hostcpu_uhip_set_state(hostcpu_uhip_state_t state)
{
	g_hostcpu_uhip.state = state;
}

int hostcpu_uhip_Initialize(int nPort)
{
	//initialise the global shared memory structure
	memset(&g_hostcpu_uhip, 0x00, sizeof(hostcpu_uhip_t));

	// Initialize transport & timers
	if (hostcpu_transport_init(nPort) != AUD_TRUE) {
		printf("FATAL ERROR - unable to initialise hostcpu transport\n");
		return -1;
	}
	uhip_hostcpu_rx_timer_init();
	uhip_hostcpu_tx_timer_init();

	//reset the rx buffers
	reset_uhip_rx_buffers();

	// NOTE: This example application sends a SINGLE DDP Tx request and prints out the response when it is received
	// It will also print the contents of any DDP events received while running
	// only enable ONE FLAG at a time as the host application is required to wait for a response before sending the next Tx request
	// In a complete application the sending of DDP requests would need to be triggered by an external event such as a push button press or by a periodic timer

	//g_hostcpu_uhip.tx_flags |= HOSTCPU_UHIP_TX_CMC_PB_FLAG;			//send a ConMon packet bridge message - see send_conmon_packetbridge_packet()
	//g_hostcpu_uhip.tx_flags |= HOSTCPU_UHIP_TX_UDP_PB_FLAG;			//send a UDP packet bridge message - see send_udp_packetbridge_packet()
	//g_hostcpu_uhip.tx_flags |= HOSTCPU_UHIP_TX_DDP_NET_IF_QUERY_FLAG;		//send a DDP network interface message - see send_network_interface_query()
	//g_hostcpu_uhip.tx_flags |= HOSTCPU_UHIP_TX_DDP_CONFIG_STATIC_IP_FLAG;		//send a DDP network config message - see configure_static_ip()
	//g_hostcpu_uhip.tx_flags |= HOSTCPU_UHIP_TX_DDP_SUB_RX_CHANS_FLAG;		//send a DDP routing rx subscribe message - see subscribe_channels()
	//g_hostcpu_uhip.tx_flags |= HOSTCPU_UHIP_TX_DDP_ADD_MCAST_TX_FLAG;		//send a DDP routing tx multicast flow message - see create_tx_multicast_flow()
	//g_hostcpu_uhip.tx_flags |= HOSTCPU_UHIP_TX_DDP_DELETE_FLOW_FLAG;		//send a DDP routing delete flow message - see delete_flow()
	//g_hostcpu_uhip.tx_flags |= HOSTCPU_UHIP_TX_DDP_UPGRADE_XMODEM_FLAG;		//send a DDP device upgrade message - see upgrade_via_xmodem()
	//g_hostcpu_uhip.tx_flags |= HOSTCPU_UHIP_TX_DDP_CONFIG_SRATE_FLAG;		//send a DDP device audio sample rate config message - see configure_sample_rate()
	//g_hostcpu_uhip.tx_flags |= HOSTCPU_UHIP_TX_DDP_CONFIG_CLOCK_PULLUP_FLAG;	//send a DDP clock pullup message - see configure_clock_pullup()
	//g_hostcpu_uhip.tx_flags |= HOSTCPU_UHIP_TX_DDP_DEV_GEN_QUERY_FLAG;	//send a DDP device general interface message - see send_device_general_query()
	//g_hostcpu_uhip.tx_flags |= HOSTCPU_UHIP_TX_DDP_DEV_MANUF_INFO_FLAG;	//send a DDP device general interface message - see send_device_manuf_info()

	//set normal state
	g_hostcpu_uhip.state = HOSTCPU_UHIP_STATE_NORMAL;
	
	return(1);
}

unsigned char hostcpu_dev_gen_query()
{
	if (g_hostcpu_uhip.state != HOSTCPU_UHIP_STATE_NORMAL)
		return(0);

	g_hostcpu_uhip.tx_flags |= HOSTCPU_UHIP_TX_DDP_DEV_GEN_QUERY_FLAG;
	return(1);
}

unsigned char hostcpu_dev_manuf_info()
{
	if (g_hostcpu_uhip.state != HOSTCPU_UHIP_STATE_NORMAL)
		return(0);

	g_hostcpu_uhip.tx_flags |= HOSTCPU_UHIP_TX_DDP_DEV_MANUF_INFO_FLAG;
	return(1);
}

unsigned char hostcpu_dev_id(char *pszNewFriendlyName)
{
	if (g_hostcpu_uhip.state != HOSTCPU_UHIP_STATE_NORMAL)
		return(0);

	g_hostcpu_uhip.tx_flags |= HOSTCPU_UHIP_TX_DDP_DEV_ID_FLAG;
	g_pszNewFriendlyName = pszNewFriendlyName;
	return(1);
}

unsigned char hostcpu_network_status()
{
	if (g_hostcpu_uhip.state != HOSTCPU_UHIP_STATE_NORMAL)
		return(0);

	g_hostcpu_uhip.tx_flags |= HOSTCPU_UHIP_TX_DDP_NET_IF_QUERY_FLAG;
	return(1);
}

unsigned char hostcpu_channel_status()
{
	if (g_hostcpu_uhip.state != HOSTCPU_UHIP_STATE_NORMAL)
		return(0);

	g_hostcpu_uhip.tx_flags |= HOSTCPU_UHIP_TX_DDP_CHANNEL_STATUS_FLAG;
	return(1);
}


