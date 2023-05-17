#include "util.h"
#include "clocking_structures.h"

const char * ddp_opcode_to_string(ddp_opcode_t opcode)
{
	switch (opcode)
	{
	case DDP_OP_DEVICE_GENERAL:                   return "DEVICE_GENERAL";
	case DDP_OP_DEVICE_MANF:                      return "DEVICE_MANF";
	case DDP_OP_DEVICE_SECURITY:                  return "DEVICE_SECURITY";
	case DDP_OP_DEVICE_UPGRADE:                   return "DEVICE_UPGRADE";
	case DDP_OP_DEVICE_ERASE_CONFIG:              return "ERASE_CONFIG";
	case DDP_OP_DEVICE_REBOOT:                    return "DEVICE_REBOOT";
	case DDP_OP_DEVICE_IDENTITY:                  return "DEVICE_IDENTITY";
	case DDP_OP_DEVICE_IDENTIFY:                  return "DEVICE_IDENTIFY";
	case DDP_OP_DEVICE_GPIO:                      return "DEVICE_GPIO";
	case DDP_OP_DEVICE_SWITCH_LED:                return "DEVICE_SWITCH_LED";

	case DDP_OP_NETWORK_BASIC:                    return "NETWORK_BASIC";
	case DDP_OP_NETWORK_CONFIG:                   return "NETWORK_CONFIG";

	case DDP_OP_CLOCK_BASIC:                      return "CLOCK_BASIC";
	case DDP_OP_CLOCK_CONFIG:                     return "CLOCK_CONFIG";
	case DDP_OP_CLOCK_PULLUP:                     return "CLOCK_PULLUP";

	case DDP_OP_ROUTING_BASIC:                    return "ROUTING_BASIC";
	case DDP_OP_ROUTING_READY_STATE:              return "ROUTING_READY_STATE";
	case DDP_OP_ROUTING_PERFORMANCE_CONFIG:       return "ROUTING_PERFORMANCE_CONFIG";
	case DDP_OP_ROUTING_RX_CHAN_CONFIG_STATE:     return "ROUTING_RX_CHAN_CONFIG_STATE";
	case DDP_OP_ROUTING_TX_CHAN_CONFIG_STATE:     return "ROUTING_TX_CHAN_CONFIG_STATE";
	case DDP_OP_ROUTING_RX_FLOW_CONFIG_STATE:     return "ROUTING_RX_FLOW_CONFIG_STATE";
	case DDP_OP_ROUTING_TX_FLOW_CONFIG_STATE:     return "ROUTING_TX_FLOW_CONFIG_STATE";
	case DDP_OP_ROUTING_RX_CHAN_STATUS:           return "ROUTING_RX_CHAN_STATUS";
	case DDP_OP_ROUTING_RX_FLOW_STATUS:           return "ROUTING_RX_FLOW_STATUS";
	case DDP_OP_ROUTING_RX_SUBSCRIBE_SET:         return "ROUTING_RX_SUBSCRIBE_SET";
	case DDP_OP_ROUTING_RX_UNSUB_CHAN:            return "ROUTING_RX_UNSUB_CHAN";
	case DDP_OP_ROUTING_RX_CHAN_LABEL_SET:        return "ROUTING_RX_CHAN_LABEL_SET";
	case DDP_OP_ROUTING_TX_CHAN_LABEL_SET:        return "ROUTING_TX_CHAN_LABEL_SET";
	case DDP_OP_ROUTING_MCAST_TX_FLOW_CONFIG_SET:  return "ROUTING_MCAST_TX_FLOW_CONFIG_SET";
	case DDP_OP_ROUTING_MANUAL_TX_FLOW_CONFIG_SET: return "ROUTING_MANUAL_TX_FLOW_CONFIG_SET";
	case DDP_OP_ROUTING_MANUAL_RX_FLOW_CONFIG_SET: return "ROUTING_MANUAL_RX_FLOW_CONFIG_SET";
	case DDP_OP_ROUTING_FLOW_DELETE:              return "ROUTING_FLOW_DELETE";

	case DDP_OP_AUDIO_BASIC:                      return "AUDIO_BASIC";
	case DDP_OP_AUDIO_SRATE_CONFIG:               return "AUDIO_SRATE_CONFIG";
	case DDP_OP_AUDIO_ENC_CONFIG:                 return "AUDIO_ENC_CONFIG";
	case DDP_OP_AUDIO_ERROR:                      return "AUDIO_ERROR";

	case DDP_OP_MDNS_CONFIG:                      return "MDNS_CONFIG";
	case DDP_OP_MDNS_REGISTER_SERVICE:            return "MDNS_REGISTER_SERVICE";
	case DDP_OP_MDNS_DEREGISTER_SERVICE:          return "MDNS_DEREGISTER_SERVICE";

	case DDP_OP_LOCAL_AUDIO_FORMAT:               return "LOCAL_AUDIO_FORMAT";
	case DDP_OP_LOCAL_CLOCK_PULLUP:               return "LOCAL_CLOCK_PULLUP";

	case DDP_OP_MONITOR_INTERFACE_STATISTICS:     return "MONITOR_INTERFACE_STATISTICS";
	case DDP_OP_MONITOR_CLOCK:                    return "MONITOR_CLOCK";
	case DDP_OP_MONITOR_SIGNAL_PRESENCE:          return "MONITOR_SIGNAL_PRESENCE";
	case DDP_OP_MONITOR_RXFLOW_MAX_LATENCY:       return "MONITOR_RXFLOW_MAX_LATENCY";
	case DDP_OP_MONITOR_RXFLOW_LATE_PACKETS:      return "MONITOR_LATE_PACKETS";
	case DDP_OP_MONITOR_TIMER_ACCURACY:           return "MONITOR_TIMER_ACCURACY";
	}
	return "?";
}

const char * ddp_status_to_string(ddp_status_t status)
{
	switch (status)
	{
	case DDP_STATUS_NOERROR:						return "NO_ERROR";
	case DDP_STATUS_ERROR_OTHER:					return "ERROR_OTHER";
	case DDP_STATUS_ERROR_UNSUPPORTED_OPCODE:		return "ERROR_UNSUPPORTED_OPCODE";
	case DDP_STATUS_ERROR_UNSUPPORTED_VERSION:		return "ERROR_UNSUPPORTED_VERSION";
	case DDP_STATUS_ERROR_INVALID_FORMAT:			return "ERROR_INVALID_FORMAT";
	case DDP_STATUS_ERROR_INVALID_DATA:				return "ERROR_INVALID_DATA";
	}
	return "?";
}

const char * ddp_pullup_to_string(uint32_t pullup)
{
	switch (pullup)
	{
	case CLOCK_PULLUP_NO_PULLUP:			return "PULLUP_NONE";
	case CLOCK_PULLUP_POS_4_1667_PCENT:		return "PULLUP_+4.1667%";
	case CLOCK_PULLUP_POS_0_1_PCENT:		return "PULLUP_+0.1%";
	case CLOCK_PULLUP_NEG_0_1_PCENT:		return "PULLUP_-0.1%";
	case CLOCK_PULLUP_NEG_4_0_PCENT4:		return "PULLUP_-4.0%";
	}
	return "?";
}

aud_bool_t
ddp_verify_spi_baud_rate(uint32_t baud)
{
	aud_bool_t result = AUD_TRUE;

	switch (baud)
	{
		case 25000000:
		case 12500000:
		case 6250000:
		case 3125000:
		case 1562500:
		case 781250:
		case 390625:
		case 195312:
		case 97656:
		case 48828:
			break;
		default:
			result = AUD_FALSE;
			break;
	}

	return result;
}

aud_bool_t
ddp_verify_uart_baud_rate(uint32_t baud)
{
	aud_bool_t result = AUD_TRUE;

	switch (baud)
	{
		case 115200:
		case 57600:
		case 38400:
		case 19200:
		case 9600:
			break;
		default:
			result = AUD_FALSE;
			break;
	}

	return result;
}

