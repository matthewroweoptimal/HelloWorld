#include "clocking_client.h"

aud_error_t
ddp_read_clock_basic_response
(
		const ddp_packet_read_info_t * packet_info,
		uint16_t offset,
		ddp_request_id_t * out_request_id,
		ddp_status_t * out_status,
		uint32_t* out_capability_flags,
		uint8_t* out_clock_state,
		uint8_t* out_mute_state,
		uint8_t* out_ext_wc_state,
		uint8_t* out_preferred,
		uint32_t* out_drift
)
{
	aud_error_t result;
	ddp_message_read_info_t message_info;
	ddp_clock_basic_response_t * header;

	if (!packet_info || offset >= packet_info->packet_length_bytes)
	{
		return AUD_ERR_INVALIDPARAMETER;
	}
	result = ddp_packet_read_message(packet_info, &message_info, offset);
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	result = ddp_packet_validate_message(&message_info, DDP_OP_CLOCK_BASIC, sizeof(ddp_clock_basic_response_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	header = (ddp_clock_basic_response_t *)message_info.header.header;

	if (out_request_id)
	{
		*out_request_id = ntohs(header->subheader.request_id);
	}
	if (out_status)
	{
		*out_status = ntohs(header->subheader.status);
	}

	if (out_capability_flags)
	{
		*out_capability_flags = ntohl(header->payload.capability_flags);
	}
	if(out_clock_state)
	{
		*out_clock_state = header->payload.clock_state;
	}
	if(out_mute_state)
	{
		*out_mute_state = header->payload.mute_state;
	}
	if(out_ext_wc_state)
	{
		*out_ext_wc_state = header->payload.ext_wc_state;
	}
	if(out_preferred)
	{
		*out_preferred = header->payload.preferred;
	}
	if(out_drift)
	{
		*out_drift = ntohl(header->payload.drift);
	}

	return AUD_SUCCESS;
}

aud_error_t
ddp_read_clock_config_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t * out_request_id,
	ddp_status_t * out_status,
	uint8_t * out_preferred,
	uint8_t * out_word_clock_sync,
	uint8_t * out_logging
)
{
	aud_error_t result;
	ddp_message_read_info_t message_info;
	ddp_clock_config_response_t * header;
	uint16_t valid_flags;

	if (!packet_info || offset >= packet_info->packet_length_bytes)
	{
		return AUD_ERR_INVALIDPARAMETER;
	}
	result = ddp_packet_read_message(packet_info, &message_info, offset);
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	result = ddp_packet_validate_message(&message_info, DDP_OP_CLOCK_CONFIG, sizeof(ddp_clock_config_response_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	header = (ddp_clock_config_response_t *)message_info.header.header;

	if (out_request_id)
	{
		*out_request_id = ntohs(header->subheader.request_id);
	}
	if (out_status)
	{
		*out_status = ntohs(header->subheader.status);
	}

	valid_flags = ntohs(header->payload.valid_flags);
	if(out_preferred && (valid_flags & CLOCK_CONFIG_RSP_PREFERRED_VALID))
	{
		*out_preferred = header->payload.preferred;
	}
	if(out_word_clock_sync && (valid_flags & CLOCK_CONFIG_RSP_WC_SYNC_VALID))
	{
		*out_word_clock_sync =  header->payload.word_clock_sync;
	}
	if(out_logging && (valid_flags & CLOCK_CONFIG_RSP_LOGGING_VALID))
	{
		*out_logging = header->payload.logging;
	}

	return AUD_SUCCESS;
}

aud_error_t
ddp_read_clock_pullup_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t * out_request_id,
	ddp_status_t * out_status,
	uint8_t * out_current_pullup,
	uint8_t * out_reboot_pullup,
	const uint8_t ** out_current_subdomain,
	uint16_t * out_current_subdomain_length,
	const uint8_t ** out_reboot_subdomain,
	uint16_t* out_reboot_subdomain_length,
	const clock_supported_pullup_t ** out_supported_pullups,
	uint16_t * out_num_supported_pullups
)
{
	aud_error_t result;
	ddp_message_read_info_t message_info;
	ddp_clock_pullup_response_t * header;
	uint16_t valid_flags;
	ddp_heap_read_info_t heap_info;

	if (!packet_info || offset >= packet_info->packet_length_bytes)
	{
		return AUD_ERR_INVALIDPARAMETER;
	}
	result = ddp_packet_read_message(packet_info, &message_info, offset);
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	result = ddp_packet_validate_message(&message_info, DDP_OP_CLOCK_PULLUP, sizeof(ddp_clock_pullup_response_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	header = (ddp_clock_pullup_response_t *)message_info.header.header;

	if (out_request_id)
	{
		*out_request_id = ntohs(header->subheader.request_id);
	}
	if (out_status)
	{
		*out_status = ntohs(header->subheader.status);
	}

	//get the valid flags
	valid_flags = ntohs(header->payload.valid_flags);

	if (out_current_pullup && (valid_flags & CLOCK_PULLUP_CURR_PULLUP_VALID) )
	{
		*out_current_pullup = header->payload.current_pullup;
	}

	if (out_reboot_pullup && (valid_flags & CLOCK_PULLUP_REBOOT_PULLUP_VALID) )
	{
		*out_reboot_pullup = header->payload.reboot_pullup;
	}
	if (out_current_subdomain && out_current_subdomain_length && (valid_flags & CLOCK_PULLUP_CURR_SUBDOMAIN_VALID))
	{
		if
		(
			ddp_read_heap_array8
			(
				&message_info,
				&heap_info,
				ntohs(header->payload.current_subdomain_offset),
				ntohs(header->payload.current_subdomain_size)
			) == AUD_SUCCESS
		)
		{
			*out_current_subdomain = heap_info._.array8;
			*out_current_subdomain_length = heap_info.length_bytes;
		}
		else
		{
			*out_current_subdomain = NULL;
			*out_current_subdomain_length = 0;
		}
	}

	if (out_reboot_subdomain && out_reboot_subdomain_length && (valid_flags & CLOCK_PULLUP_REBOOT_SUBDOMAIN_VALID))
	{
		if
		(
			ddp_read_heap_array8
			(
				&message_info,
				&heap_info,
				ntohs(header->payload.reboot_subdomain_offset),
				ntohs(header->payload.reboot_subdomain_size)
			) == AUD_SUCCESS
		)
		{
			*out_reboot_subdomain = heap_info._.array8;
			*out_reboot_subdomain_length = heap_info.length_bytes;
		}
		else
		{
			*out_reboot_subdomain = NULL;
			*out_reboot_subdomain_length = 0;
		}
	}

	if (out_supported_pullups && out_num_supported_pullups && (valid_flags & CLOCK_PULLUP_SUPPORTED_PULLUPS_VALID))
	{
		//check that the sizes match
		if(sizeof(clock_supported_pullup_t) != ntohs(header->payload.supported_pullups_size))
		{
			return AUD_ERR_INVALIDDATA;
		}

		if
		(
			ddp_read_heap_arraystruct
			(
				&message_info,
				&heap_info,
				ntohs(header->payload.supported_pullups_offset),
				ntohs(header->payload.num_supported_pullups),
				ntohs(header->payload.supported_pullups_size)
			) == AUD_SUCCESS
		)
		{
			*out_supported_pullups = (clock_supported_pullup_t*)heap_info._.array16;
			*out_num_supported_pullups = heap_info.num_elements;
		}
		else
		{
			*out_supported_pullups = NULL;
			*out_num_supported_pullups = 0;
		}
	}

	return AUD_SUCCESS;
}

