/**
 * File     : example_uhip_main.h
 * Created  : October 2014
 * Updated  : 2014/10/02
 * Author   : Chamira Perera, Michael Ung
 * Synopsis : HostCPU example UHIP main
 *
 * Copyright 2005-2014, Audinate Pty Ltd and/or its licensors
 * Confidential
 * Audinate Copyright Header Version 1
 */
#ifndef __EXAMPLE_UHIP_MAIN_H__
#define __EXAMPLE_UHIP_MAIN_H__

#include "aud_platform.h"
#include "inc/packet.h"

/**
* @enum hostcpu_uhip_state
* @brief UHIP protocol states
*/
enum hostcpu_uhip_state
{
	HOSTCPU_UHIP_STATE_NOINIT = 0,		/*!< Not initialised */
	HOSTCPU_UHIP_STATE_NORMAL,		/*!< Normal */
	HOSTCPU_UHIP_STATE_NO_TX_WAIT_ACK,	/*!< Transmitted a packet, but waiting on the ACK/NACK */
	HOSTCPU_UHIP_STATE_NO_TX_RESYNC,	/*!< We haven't received an ACK, and the timer has expired - delay to allow RESYNC */
	HOSTCPU_UHIP_STATE_MAX,			/*!< MAX placeholder */
};

/** @cond */
typedef enum hostcpu_uhip_state hostcpu_uhip_state_t;
/** @endcond */

/**
* @enum hostcpu_uhip_tx_packet
* @brief TX packet pending
*/
enum hostcpu_uhip_tx_packet
{
	HOSTCPU_UHIP_TX_PROT_CTRL_ACK = 0,		/*!< Transmit a protocol control ACK */
	HOSTCPU_UHIP_TX_CMC_PB,				/*!< Transmit a ConMon packet bridge */
	HOSTCPU_UHIP_TX_UDP_PB,				/*!< Transmit a UDP packet bridge */
	HOSTCPU_UHIP_TX_DDP_NET_IF_QUERY,		/*!< Transmit a DDP Network interface query */
	HOSTCPU_UHIP_TX_DDP_CONFIG_STATIC_IP,		/*!< Transmit a DDP Network configuration request to configure the static IP */
	HOSTCPU_UHIP_TX_DDP_SUB_RX_CHANS,		/*!< Transmit a DDP Routing Rx subscribe set request */
	HOSTCPU_UHIP_TX_DDP_ADD_MCAST_TX,		/*!< Transmit a DDP Routing multicast Tx request */
	HOSTCPU_UHIP_TX_DDP_DELETE_FLOW,		/*!< Transmit a DDP Routing delete flow request */
	HOSTCPU_UHIP_TX_DDP_UPGRADE_XMODEM,		/*!< Transmit a DDP Device upgrade request */
	HOSTCPU_UHIP_TX_DDP_CONFIG_SRATE,		/*!< Transmit a DDP Audio configure sample rate request */
	HOSTCPU_UHIP_TX_DDP_CONFIG_CLOCK_PULLUP,	/*!< Transmit a DDP Clocking configure clock pullup request */
	HOSTCPU_UHIP_TX_DDP_DEVICE_GPIO,			/*!< Transmit a DDP Device GPIO request */
	HOSTCPU_UHIP_TX_DDP_DEVICE_SWITCH_LED,			/*!< Transmit a DDP Device Switch LED request */
	HOSTCPU_UHIP_TX_DDP_DEV_GEN_QUERY,		/*!< Transmit a DDP device geeral query request */
	HOSTCPU_UHIP_TX_DDP_DEV_MANUF_INFO,		/*!< Transmit a DDP device manufacurer info request */
	HOSTCPU_UHIP_TX_DDP_DEV_ID,		/*!< Transmit a DDP device id info request */
	HOSTCPU_UHIP_TX_DDP_CHANNEL_STATUS,		/*!< Transmit a DDP channel status info request */
};

/**
* @enum hostcpu_uhip_tx_packet_flags
* @brief TX packet pending flags
*/
enum hostcpu_uhip_tx_packet_flags
{
	HOSTCPU_UHIP_TX_PROT_CTRL_ACK_FLAG			=	(1 << HOSTCPU_UHIP_TX_PROT_CTRL_ACK),		/*!< Transmit a protocol control ACK */
	HOSTCPU_UHIP_TX_CMC_PB_FLAG				=	(1 << HOSTCPU_UHIP_TX_CMC_PB),			/*!< Transmit a ConMon packet bridge */
	HOSTCPU_UHIP_TX_UDP_PB_FLAG				=	(1 << HOSTCPU_UHIP_TX_UDP_PB),			/*!< Transmit a UDP packet bridge */
	HOSTCPU_UHIP_TX_DDP_NET_IF_QUERY_FLAG			=	(1 << HOSTCPU_UHIP_TX_DDP_NET_IF_QUERY),	/*!< Transmit a DDP Network interface query */
	HOSTCPU_UHIP_TX_DDP_CONFIG_STATIC_IP_FLAG		=	(1 << HOSTCPU_UHIP_TX_DDP_CONFIG_STATIC_IP),	/*!< Transmit a DDP Network static IP config request */
	HOSTCPU_UHIP_TX_DDP_SUB_RX_CHANS_FLAG			=	(1 << HOSTCPU_UHIP_TX_DDP_SUB_RX_CHANS),	/*!< Transmit a DDP Routing subscribe Rx channels request */
	HOSTCPU_UHIP_TX_DDP_ADD_MCAST_TX_FLAG			=	(1 << HOSTCPU_UHIP_TX_DDP_ADD_MCAST_TX),	/*!< Transmit a DDP Routing multicast Tx flow config request */
	HOSTCPU_UHIP_TX_DDP_DELETE_FLOW_FLAG			=	(1 << HOSTCPU_UHIP_TX_DDP_DELETE_FLOW),		/*!< Transmit a DDP Routing delete flow request */
	HOSTCPU_UHIP_TX_DDP_UPGRADE_XMODEM_FLAG			=	(1 << HOSTCPU_UHIP_TX_DDP_UPGRADE_XMODEM),	/*!< Transmit a DDP Device upgrade via XMODEM request */
	HOSTCPU_UHIP_TX_DDP_CONFIG_SRATE_FLAG			=	(1 << HOSTCPU_UHIP_TX_DDP_CONFIG_SRATE),	/*!< Transmit a DDP Audio sample rate config request */
	HOSTCPU_UHIP_TX_DDP_CONFIG_CLOCK_PULLUP_FLAG	=	(1 << HOSTCPU_UHIP_TX_DDP_CONFIG_CLOCK_PULLUP),		/*!< Transmit a DDP Clocking pullup config request */
	HOSTCPU_UHIP_TX_DDP_DEVICE_GPIO_FLAG			=	(1 << HOSTCPU_UHIP_TX_DDP_DEVICE_GPIO),				/*!< Transmit a DDP Device GPIO request */
	HOSTCPU_UHIP_TX_DDP_DEVICE_SWITCH_LED_FLAG			=	(1 << HOSTCPU_UHIP_TX_DDP_DEVICE_SWITCH_LED),				/*!< Transmit a DDP Device GPIO request */
	HOSTCPU_UHIP_TX_DDP_DEV_GEN_QUERY_FLAG			=	(1 << HOSTCPU_UHIP_TX_DDP_DEV_GEN_QUERY),	/*!< Transmit a DDP device general query */
	HOSTCPU_UHIP_TX_DDP_DEV_MANUF_INFO_FLAG			=	(1 << HOSTCPU_UHIP_TX_DDP_DEV_MANUF_INFO),	/*!< Transmit a DDP device manuf info query */
	HOSTCPU_UHIP_TX_DDP_DEV_ID_FLAG					=	(1 << HOSTCPU_UHIP_TX_DDP_DEV_ID),	/*!< Transmit a DDP device id query */
	HOSTCPU_UHIP_TX_DDP_CHANNEL_STATUS_FLAG			=	(1 << HOSTCPU_UHIP_TX_DDP_CHANNEL_STATUS),	/*!< Transmit a DDP channel status query */
};

/** @cond */
typedef enum hostcpu_uhip_tx_packet_flags hostcpu_uhip_tx_packet_flags_t;
/** @endcond */

/**
* @struct hostcpu_uhip_stats
* @brief UHIP protocol statistics and counters
*/
struct hostcpu_uhip_stats
{
	//rx
	uint32_t num_rx_timeouts;			/*!< Number of RX timer timeouts - most likely lost synchronisation */
	uint32_t num_rx_overflows;			/*!< Number of RX overflow - either loss of sync, or dropped bytes in the rx transport layer */
	uint32_t num_rx_cobs_decode_errs;		/*!< Number of RX cobs decode errors - mostly likely dropped bytes in the rx transport layer  */
	uint32_t num_rx_malformed_errs;			/*!< Number of RX malformed packet errors - mostly likely dropped bytes in the rx transport layer  */
	uint32_t num_rx_prot_ctrl_pkts;			/*!< Number of protocol control packets received */
	uint32_t num_rx_cmc_pb_pkts;			/*!< Number of ConMon Packet Bridge packets received */
	uint32_t num_rx_udp_pb_pkts;			/*!< Number of UDP Packet Bridge packets received */
	uint32_t num_rx_dante_evnt_pkts;		/*!< Number of Dante Event packets received */
	uint32_t num_rx_ddp_pkts;			/*!< Number of Dante Device Protocol packets received */

	//tx
	uint32_t num_tx_timeouts;			/*!< Number of TX timer timeouts - most likely lost synchronisation */
	uint32_t num_tx_prot_ctrl_pkts;			/*!< Number of protocol control packets transmitted */
	uint32_t num_tx_cmc_pb_pkts;			/*!< Number of ConMon Packet Bridge packets transmitted */
	uint32_t num_tx_udp_pb_pkts;			/*!< Number of UDP Packet Bridge packets transmitted */
};

/** @cond */
typedef struct hostcpu_uhip_stats hostcpu_uhip_stats_t;
/** @endcond */

/**
* @struct hostcpu_uhip
* @brief Host CPU UHIP global memory structure
*/
struct hostcpu_uhip
{
	hostcpu_uhip_state_t state;			/*!< UHIP protocol state */
	uint32_t tx_flags;				/*!< bitmask of which TX packets are pending */
	hostcpu_uhip_stats_t stats;			/*!< UHIP protocol statistics & counters */
	ddp_request_id_t ddp_tx_request_id;			/*!< Request ID of the sending DDP packet, this will be used when matching the request ID of the DDP response (not applicable for received events) */
};

/** @cond */
typedef struct hostcpu_uhip hostcpu_uhip_t;
/** @endcond */

/**
* Initializes the serial port to use
*/
int hostcpu_uhip_Initialize(int nPort);


/**
* Gets the DDP request ID of the sending packet
*/
ddp_request_id_t hostcpu_uhip_get_ddp_tx_request_id(void);

/**
* Sets the DDP request ID of the sending packet
*/
void hostcpu_uhip_set_ddp_tx_request_id(ddp_request_id_t request_id);

/**
* Sets a Tx flag @see hostcpu_uhip_tx_packet_flags
*/
void hostcpu_uhip_set_tx_flag(hostcpu_uhip_tx_packet_flags_t tx_flag);

/**
* Clears a Tx flag @see hostcpu_uhip_tx_packet_flags
*/
void hostcpu_uhip_clear_tx_flag(hostcpu_uhip_tx_packet_flags_t tx_flag);

/**
* Checks if a Tx flag is set @see hostcpu_uhip_tx_packet_flags
*/
aud_bool_t hostcpu_uhip_is_tx_flag_set(hostcpu_uhip_tx_packet_flags_t tx_flag);

/**
* Increments the number of UHIP Tx protocol control packets 
*/
void hostcpu_uhip_increment_num_tx_prot_ctrl_pkts(void);

/**
* Increments the number of UHIP Tx ConMon packet bridge packets
*/
void hostcpu_uhip_increment_num_tx_cmc_pb_pkts(void);

/**
* Increments the number of UHIP Tx UDP packets
*/
void hostcpu_uhip_increment_num_tx_udp_pb_pkts(void);

/**
* Increments the number of Serial Rx data overflows
*/
void hostcpu_uhip_increment_num_rx_overflows(void);

/**
* Increments the number of UHIP Rx DDP packets
*/
void hostcpu_uhip_increment_num_rx_ddp_pkts(void);

/**
* Increments the number of UHIP Rx Dante event packets
*/
void hostcpu_uhip_increment_num_rx_dante_evnt_pkts(void);

/**
* Increments the number of UHIP Rx UDP packet bridge packets
*/
void hostcpu_uhip_increment_num_rx_udp_pb_pkts(void);

/**
* Increments the number of UHIP Rx ConMon packet bridge packets
*/
void hostcpu_uhip_increment_num_rx_cmc_pb_pkts(void);

/**
* Increments the number of UHIP Rx malformed errors
*/
void hostcpu_uhip_increment_num_rx_malformed_errs(void);

/**
* Increments the number of Rx COBS decode errors
*/
void hostcpu_uhip_increment_num_rx_cobs_decode_errs(void);

/**
* Increments the number of UHIP Rx protocol control packets
*/
void hostcpu_uhip_increment_num_rx_prot_ctrl_pkts(void);

/**
* Gets the state @see hostcpu_uhip_state
*/
hostcpu_uhip_state_t hostcpu_uhip_get_state(void);

/**
* Sets the state @see hostcpu_uhip_state
 */
void hostcpu_uhip_set_state(hostcpu_uhip_state_t state);

#endif // __HOSTCPU_UHIP_API_H__
