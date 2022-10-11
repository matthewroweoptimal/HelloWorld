#include "audio_client.h"
#include "audio_structures.h"

aud_error_t
ddp_add_audio_basic_request
(
	ddp_packet_write_info_t * packet_info,
	ddp_request_id_t request_id
)
{
	aud_error_t result = AUD_SUCCESS;
	ddp_message_write_info_t message_info;

	if (!packet_info)
	{
		return AUD_ERR_INVALIDPARAMETER;
	}

	result = ddp_packet_allocate_request (packet_info, &message_info,
											DDP_OP_AUDIO_BASIC,
											request_id,
											0);
	return result;
}

aud_error_t
ddp_add_audio_sample_rate_config_request
(
	ddp_packet_write_info_t * packet_info,
	ddp_request_id_t request_id,
	uint16_t control_flags,
	uint32_t sample_rate
)
{
	aud_error_t result = AUD_SUCCESS;
	ddp_message_write_info_t message_info;
	ddp_audio_srate_config_request_t * raw_header;

	if (!packet_info)
	{
		return AUD_ERR_INVALIDPARAMETER;
	}

	result = ddp_packet_allocate_request(packet_info, &message_info,
										   DDP_OP_AUDIO_SRATE_CONFIG,
										   request_id,
										   sizeof(raw_header->payload));

	if (result != AUD_SUCCESS)
	{
		return result;
	}

	raw_header = (ddp_audio_srate_config_request_t *)message_info._.buf32;
	raw_header->payload.control_flags = htons(control_flags);
	
	if (control_flags & SET_SRATE_FLAGS)
	{
		raw_header->payload.sample_rate = htonl(sample_rate);
	}
	else
	{
		raw_header->payload.sample_rate = 0;
	}
	
	raw_header->payload.pad1 = 0;

	return AUD_SUCCESS;
}

aud_error_t
ddp_add_audio_encoding_config_request
(
	ddp_packet_write_info_t * packet_info,
	ddp_request_id_t request_id,
	uint16_t control_flags,
	uint16_t encoding
)
{
	aud_error_t result = AUD_SUCCESS;
	ddp_message_write_info_t message_info;
	ddp_audio_encoding_config_request_t * raw_header;
	if (!packet_info)
	{
		return AUD_ERR_INVALIDPARAMETER;
	}

	result = ddp_packet_allocate_request(packet_info, &message_info,
										   DDP_OP_AUDIO_ENC_CONFIG,
										   request_id,
										   sizeof(raw_header->payload));

	if (result != AUD_SUCCESS)
	{
		return result;
	}

	raw_header = (ddp_audio_encoding_config_request_t *)message_info._.buf32;

	raw_header->payload.control_flags = htons(control_flags);

	if (AUD_FLAG_ISSET(control_flags, AUDIO_ENC_CONFIG__SET_ENCODING_FLAGS))
	{
		raw_header->payload.encoding = htons(encoding);
	}
	else
	{
		raw_header->payload.encoding = 0;
	}

	return AUD_SUCCESS;
}

