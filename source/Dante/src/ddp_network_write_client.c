#include "network_client.h"

aud_error_t
ddp_add_network_basic_request
(
		ddp_packet_write_info_t * packet_info,
		ddp_request_id_t request_id
)
{
	aud_error_t result;
	ddp_message_write_info_t message_info;

	if (!packet_info)
	{
		return AUD_ERR_INVALIDPARAMETER;
	}

	result = ddp_packet_allocate_request(packet_info, &message_info,
		DDP_OP_NETWORK_BASIC, request_id,
		0);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	return AUD_SUCCESS;
}


aud_error_t
ddp_add_network_config_request
(
		ddp_packet_write_info_t * packet_info,
		ddp_request_id_t request_id,
		uint8_t mode,
		const network_ip_config_t * ip_config,
		const network_dns_config_t * dns_config,
		const char * domain_name
)
{
	aud_error_t result;
	ddp_message_write_info_t message_info;
	ddp_network_config_request_t * raw_header;
	uint16_t control_flags = 0;

	result = ddp_packet_allocate_request(packet_info, &message_info,
			DDP_OP_NETWORK_CONFIG, request_id, sizeof(raw_header->payload));
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	raw_header = (ddp_network_config_request_t *) message_info._.buf32;

	if(mode)
	{
		raw_header->payload.mode = mode;
		control_flags |= NETWORK_CONFIG_REQ_SET_MODE;
	}
	raw_header->payload.pad0 = 0;
	if(ip_config)
	{
		if(ip_config->ip_addr)
		{
			raw_header->payload._ip_config.ip_addr = htonl(ip_config->ip_addr);
			control_flags |= NETWORK_CONFIG_REQ_SET_STATIC_IP;
		}
		if(ip_config->subnet_mask)
		{
			raw_header->payload._ip_config.subnet_mask = htonl(ip_config->subnet_mask);
			control_flags |= NETWORK_CONFIG_REQ_SET_STATIC_SUBNET_MASK;
		}
		if(ip_config->gateway)
		{
			raw_header->payload._ip_config.gateway = htonl(ip_config->gateway);
			control_flags |= NETWORK_CONFIG_REQ_SET_STATIC_GATEWAY;
		}
	}
	else
	{
		raw_header->payload._ip_config.ip_addr = htonl(0);
		raw_header->payload._ip_config.subnet_mask = htonl(0);
		raw_header->payload._ip_config.gateway = htonl(0);
	}

	if(dns_config)
	{
		//allocate onto the heap
		ddp_dns_srvr_config_net_config_t* dns_conf_heap;
		ddp_heap_write_info_t heap_info;
		result = ddp_allocate_heap_arraystruct(&message_info, &heap_info,
						1, sizeof(ddp_dns_srvr_config_net_config_t));
		if (result != AUD_SUCCESS)
		{
			return result;
		}

		raw_header->payload.dns_srv_offset = heap_info.msg_offset_bytes;
		raw_header->payload.dns_srv_size = htons(sizeof(ddp_dns_srvr_config_net_config_t));

		dns_conf_heap = (ddp_dns_srvr_config_net_config_t*)heap_info._.array8;

		if (dns_config->ip_addr && dns_config->family)
		{
			dns_conf_heap->ip_addr = htonl(dns_config->ip_addr);
			dns_conf_heap->family = htons(dns_config->family);
			dns_conf_heap->pad = 0;
			control_flags |= NETWORK_CONFIG_REQ_SET_DNS_SERVER;
		}
		else
		{
			return AUD_ERR_INVALIDPARAMETER;
		}
	}
	else
	{
		raw_header->payload.dns_srv_offset = 0;
		raw_header->payload.dns_srv_size = 0;
	}

	raw_header->payload.pad1 = 0;

	if(domain_name)
	{
		if (!
			ddp_message_allocate_string(
				&message_info,
				&raw_header->payload.domain_offset,
				domain_name
		))
		{
			return AUD_ERR_NOMEMORY;
		}
		control_flags |= NETWORK_CONFIG_REQ_SET_STATIC_DOMAIN;
	}
	else
	{
		raw_header->payload.domain_offset = 0;
	}
	raw_header->payload.control_flags = htons(control_flags);
	return AUD_SUCCESS;
}

