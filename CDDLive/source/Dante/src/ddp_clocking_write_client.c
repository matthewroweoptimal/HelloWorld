#include "clocking_client.h"

/**
* @brief Clock pullup subdomain values
*/
static char const * clock_pullup_subdomains[] = { "_DFLT", "_ALT1", "_ALT2", "_ALT3", "_ALT4" };

aud_error_t
ddp_add_clock_basic_request
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

	result = ddp_packet_allocate_request
			(
				packet_info, &message_info,
				DDP_OP_CLOCK_BASIC,
				request_id,
				0
			);
	return result;
}

aud_error_t
ddp_add_clock_config_request
(
	ddp_packet_write_info_t * packet_info,
	ddp_request_id_t request_id,
	clock_config_control_fields_t * control_fields
)
{
	aud_error_t result;
	ddp_message_write_info_t message_info;
	ddp_clock_config_request_t * raw_header;
	uint16_t control_flags = 0;

	if (!packet_info)
	{
		return AUD_ERR_INVALIDPARAMETER;
	}

	result = ddp_packet_allocate_request
			(
				packet_info,
				&message_info,
				DDP_OP_CLOCK_CONFIG,
				request_id,
				sizeof(raw_header->payload)
			);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	raw_header = (ddp_clock_config_request_t *)message_info._.buf32;

	if(control_fields)
	{
		if(control_fields->valid_flags & CONTROL_FLAG_SET_PREFERRED) {
			raw_header->payload.preferred = control_fields->preferred;
			control_flags |= CONTROL_FLAG_SET_PREFERRED;
		}

		if(control_fields->valid_flags & CONTROL_FLAG_SET_SLAVE_TO_WC) {
			raw_header->payload.ext_word_clock_sync = control_fields->word_clock_sync;
			control_flags |= CONTROL_FLAG_SET_SLAVE_TO_WC;
		}

		if(control_fields->valid_flags & CONTROL_FLAGS_SET_LOGGING) {
			raw_header->payload.logging = control_fields->logging;
			control_flags |= CONTROL_FLAGS_SET_LOGGING;
		}
	}

	raw_header->payload.pad1 = 0;
	raw_header->payload.pad2 = 0;

	raw_header->payload.control_flags = htons(control_flags);

	return AUD_SUCCESS;
}

aud_error_t
ddp_add_clock_pullup_request
(
	ddp_packet_write_info_t * packet_info,
	ddp_request_id_t request_id,
	clock_pullup_control_fields_t* control_fields
)
{
	aud_error_t result;
	ddp_message_write_info_t message_info;
	ddp_clock_pullup_request_t * raw_header;
	uint16_t control_flags = 0;

	if (!packet_info)
	{
		return AUD_ERR_INVALIDPARAMETER;
	}

	result = ddp_packet_allocate_request
			(
				packet_info,
				&message_info,
				DDP_OP_CLOCK_PULLUP,
				request_id,				
				sizeof(raw_header->payload)
			);
	if (result != AUD_SUCCESS)
	{
		return result;
	}

	raw_header = (ddp_clock_pullup_request_t *)message_info._.buf32;

	if(control_fields)
	{
		if (control_fields->valid_flags & CLOCK_PULLUP_SET_PULLUP)
		{
			if (control_fields->pullup > CLOCK_PULLUP_NEG_4_0_PCENT4) {
				return AUD_ERR_INVALIDPARAMETER;
			}

			raw_header->payload.pullup = control_fields->pullup;
			control_flags |= CLOCK_PULLUP_SET_PULLUP;

			if (!ddp_message_allocate_string(
				&message_info,
				&raw_header->payload.subdomain_offset,
				clock_pullup_subdomains[control_fields->pullup]
				))
			{
				return AUD_ERR_NOMEMORY;
			}

			raw_header->payload.subdomain_size = htons((uint16_t)strlen(clock_pullup_subdomains[control_fields->pullup]));
			control_flags |= CLOCK_PULLUP_SET_SUBDOMAIN;
		}
		else
		{
			raw_header->payload.subdomain_offset = 0;
			raw_header->payload.subdomain_size = 0;
		}
	}

	raw_header->payload.pad1 = 0;
	raw_header->payload.control_flags = htons(control_flags);

	return AUD_SUCCESS;
}

