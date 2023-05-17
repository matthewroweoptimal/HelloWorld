#include "audio_client.h"
#include "audio_structures.h"

aud_error_t
ddp_read_audio_basic_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t * out_request_id,
	uint16_t * out_status,
	uint32_t * out_capability_flags,
	uint32_t * out_default_sample_rate,
	uint16_t * out_default_encoding,
	uint16_t * out_rx_chans,
	uint16_t * out_tx_chans,
	uint8_t * out_change_flags
)
{
	aud_error_t result;
	ddp_message_read_info_t message_info;
	ddp_audio_basic_response_t * header;

	if (!packet_info || offset >= packet_info->packet_length_bytes)
	{
		return AUD_ERR_INVALIDPARAMETER;
	}
	result = ddp_packet_read_message(packet_info, &message_info, offset);
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	result = ddp_packet_validate_message(&message_info, DDP_OP_AUDIO_BASIC, sizeof(ddp_audio_basic_response_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	header = (ddp_audio_basic_response_t *)message_info.header.header;

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
	if (out_default_sample_rate)
	{
		*out_default_sample_rate = ntohl(header->payload.default_sample_rate);
	}
	if (out_default_encoding)
	{
		*out_default_encoding = ntohs(header->payload.default_encoding);
	}
	if (out_rx_chans)
	{
		*out_rx_chans = ntohs(header->payload.rx_chans);
	}
	if (out_tx_chans)
	{
		*out_tx_chans = ntohs(header->payload.tx_chans);
	}
	if (out_change_flags)
	{
		*out_change_flags = header->payload.change_flags;
	}

	return AUD_SUCCESS;
}

aud_error_t
ddp_read_audio_sample_rate_config_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_message_read_info_t * out_message_info,
	ddp_request_id_t * out_request_id,
	uint16_t * out_status,	
	uint32_t * out_current_samplerate,
	uint32_t * out_reboot_srate,
	uint16_t * out_num_supported_srates
)
{
	aud_error_t result;
	ddp_audio_srate_config_response_t * header;
	uint16_t valid_flags;

	if (!packet_info || offset >= packet_info->packet_length_bytes)
	{
		return AUD_ERR_INVALIDPARAMETER;
	}
	result = ddp_packet_read_message(packet_info, out_message_info, offset);
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	result = ddp_packet_validate_message(out_message_info, DDP_OP_AUDIO_SRATE_CONFIG, sizeof(ddp_audio_srate_config_response_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	header = (ddp_audio_srate_config_response_t *)out_message_info->header.header;

	if (out_request_id)
	{
		*out_request_id = ntohs(header->subheader.request_id);
	}
	if (out_status)
	{
		*out_status = ntohs(header->subheader.status);
	}
	
	valid_flags = ntohs(header->payload.valid_flags);	
	
	if ((valid_flags & CURR_SAMPLE_RATE_VALID_FLAGS) && out_current_samplerate)
	{
		*out_current_samplerate = ntohl(header->payload.current_samplerate);
	}
	else
	{
		*out_current_samplerate = 0;
	}
	
	if ((valid_flags & REBOOT_SAMPLE_RATE_VALID_FLAGS) && out_reboot_srate)
	{
		*out_reboot_srate = ntohl(header->payload.reboot_samplerate);
	}
	else
	{
		*out_reboot_srate = 0;
	}
	if (out_num_supported_srates)
	{
		*out_num_supported_srates = ntohs(header->payload.num_supported_srates);
	}
	

	return AUD_SUCCESS;
}

aud_error_t
ddp_read_audio_sample_rate_config_supported_srate
(
	const ddp_message_read_info_t * message_info,
	uint16_t srate_idx,
	dante_samplerate_t * out_supported_srate
)
{
	aud_error_t result;
	ddp_audio_srate_config_response_t * header;
	uint16_t valid_flags;

	header = (ddp_audio_srate_config_response_t *)message_info->header.header;
	valid_flags = ntohs(header->payload.valid_flags);

	if ((valid_flags & SUPPORTED_SAMPLE_RATES_VALID_FLAGS) && out_supported_srate)
	{
		ddp_heap_read_info_t heap_info;
		
		result = ddp_read_heap_array32(message_info,
									 &heap_info,
									 ntohs(header->payload.supported_srates_offset),
									ntohs(header->payload.num_supported_srates));
		
		if (result != AUD_SUCCESS)
		{
			return result;
		}
			
		*out_supported_srate = ntohl(heap_info._.array32[srate_idx]);
	}

	return AUD_SUCCESS;
}



aud_error_t
ddp_read_audio_encoding_config_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_message_read_info_t * out_message_info,
	ddp_request_id_t * out_request_id,
	uint16_t * out_status,
	uint16_t * out_current_encoding,
	uint16_t * out_reboot_encoding,
	uint16_t * out_num_supported_encodings
)
{
	aud_error_t result;
	ddp_audio_encoding_config_response_t * header;
	uint16_t valid_flags;

	if (!packet_info || offset >= packet_info->packet_length_bytes)
	{
		return AUD_ERR_INVALIDPARAMETER;
	}
	result = ddp_packet_read_message(packet_info, out_message_info, offset);
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	result = ddp_packet_validate_message(out_message_info, DDP_OP_AUDIO_ENC_CONFIG, sizeof(ddp_audio_encoding_config_response_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	header = (ddp_audio_encoding_config_response_t *)out_message_info->header.header;

	if (out_request_id)
	{
		*out_request_id = ntohs(header->subheader.request_id);
	}
	if (out_status)
	{
		*out_status = ntohs(header->subheader.status);
	}

	valid_flags = ntohs(header->payload.valid_flags);

	if ((valid_flags & AUDIO_ENC_CONFIG__CURRENT_ENC_VALID) && out_current_encoding)
	{
		*out_current_encoding = ntohs(header->payload.current_encoding);
	}
	else
	{
		*out_current_encoding = 0;
	}

	if ((valid_flags & AUDIO_ENC_CONFIG__REBOOT_ENC_VALID) && out_reboot_encoding)
	{
		*out_reboot_encoding = ntohs(header->payload.reboot_encoding);
	}
	else
	{
		*out_reboot_encoding = 0;
	}

	if (*out_num_supported_encodings)
	{
		*out_num_supported_encodings = ntohs(header->payload.num_supported_encs);
	}
	
	
	return AUD_SUCCESS;
}

aud_error_t
ddp_read_audio_encoding_config_supported_encoding
(
	const ddp_message_read_info_t * message_info,
	uint16_t enc_idx,
	uint16_t * out_supported_enc
)
{
	aud_error_t result;
	ddp_audio_encoding_config_response_t * header;
	uint16_t valid_flags;
	ddp_supported_enc_array_t *enc_elem;

	header = (ddp_audio_encoding_config_response_t *)message_info->header.header;
	valid_flags = ntohs(header->payload.valid_flags);

	if ((valid_flags & AUDIO_ENC_CONFIG__SUPPORTED_ENC_VALID) && out_supported_enc)
	{
		ddp_heap_read_info_t heap_info;

		result = ddp_read_heap_arraystruct(message_info,
									&heap_info,
									ntohs(header->payload.supported_encs_offset),
										ntohs(header->payload.num_supported_encs),
										ntohs(header->payload.supported_encs_size));

		if (result != AUD_SUCCESS)
		{
			return result;
		}
	
		enc_elem = (ddp_supported_enc_array_t *)heap_info._.array8;
		enc_elem += enc_idx;
		*out_supported_enc = ntohs(enc_elem->encoding);
	}
	
	return AUD_SUCCESS;
}
