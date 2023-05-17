#include "device_client.h"

aud_error_t
ddp_read_device_general_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t* out_request_id,
	ddp_status_t* out_status,
	dante_id64_t** out_model_id,
	char** out_model_id_string,
	uint32_t* out_software_version,
	uint32_t* out_software_build,
	uint32_t* out_firmware_version,
	uint32_t* out_firmware_build,
	uint32_t* out_bootloader_version,
	uint32_t* out_bootloader_build,
	uint32_t* out_api_version,
	uint32_t* out_capability_flags,
	uint32_t* out_status_flags
)
{
	aud_error_t result;
	ddp_message_read_info_t message_info;
	ddp_device_general_response_t * header;

	if (!packet_info || offset >= packet_info->packet_length_bytes)
	{
		return AUD_ERR_INVALIDPARAMETER;
	}
	result = ddp_packet_read_message(packet_info, &message_info, offset);
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	result = ddp_packet_validate_message(&message_info, DDP_OP_DEVICE_GENERAL, sizeof(ddp_device_general_response_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	header = (ddp_device_general_response_t *) message_info.header.header;
	if (out_request_id)
	{
		*out_request_id = ntohs(header->subheader.request_id);
	}
	if (out_status)
	{
		*out_status = ntohs(header->subheader.status);
	}
	if (out_model_id)
	{
		*out_model_id = &header->payload.model_id;
	}
	if (out_model_id_string)
	{
		if(ntohs(header->payload.model_id_string_offset) != 0)
		{
			*out_model_id_string = (char*)header + ntohs(header->payload.model_id_string_offset);
		}
		else
		{
			*out_model_id_string = NULL;
		}
	}
	if (out_software_version)
	{
		*out_software_version = ntohl(header->payload.software_version);
	}
	if (out_software_build)
	{
		*out_software_build = ntohl(header->payload.software_build);
	}
	if (out_firmware_version)
	{
		*out_firmware_version = ntohl(header->payload.firmware_version);
	}
	if (out_firmware_build)
	{
		*out_firmware_build = ntohl(header->payload.firmware_build);
	}
	if (out_bootloader_version)
	{
		*out_bootloader_version = ntohl(header->payload.bootloader_version);
	}
	if (out_bootloader_build)
	{
		*out_bootloader_build = ntohl(header->payload.bootloader_build);
	}
	if (out_api_version)
	{
		*out_api_version = ntohl(header->payload.api_version);
	}
	if (out_capability_flags)
	{
		*out_capability_flags = ntohl(header->payload.capability_flags);
	}
	if (out_status_flags)
	{
		*out_status_flags = ntohl(header->payload.status_flags);
	}

	return AUD_SUCCESS;
}

aud_error_t
ddp_read_device_manufacturer_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t* out_request_id,
	ddp_status_t* out_status,
	dante_id64_t** out_manf_id,
	char** out_manf_name_string,
	dante_id64_t** out_model_id,
	char** out_model_id_string,
	uint32_t* out_software_version,
	uint32_t* out_software_build,
	uint32_t* out_firmware_version,
	uint32_t* out_firmware_build,
	uint32_t* out_capability_flags,
	uint32_t* out_model_version,
	char** out_model_version_string
)
{
	aud_error_t result;
	ddp_message_read_info_t message_info;
	ddp_device_manf_response_t * header;

	if (!packet_info || offset >= packet_info->packet_length_bytes)
	{
		return AUD_ERR_INVALIDPARAMETER;
	}
	result = ddp_packet_read_message(packet_info, &message_info, offset);
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	result = ddp_packet_validate_message(&message_info, DDP_OP_DEVICE_MANF, sizeof(ddp_device_manf_response_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	header = (ddp_device_manf_response_t *) message_info.header.header;
	if (out_request_id)
	{
		*out_request_id = ntohs(header->subheader.request_id);
	}
	if (out_status)
	{
		*out_status = ntohs(header->subheader.status);
	}
	if (out_manf_id)
	{
		*out_manf_id = &header->payload.manf_id;
	}
	if (out_manf_name_string)
	{
		if(ntohs(header->payload.manf_name_string_offset) != 0)
		{
			*out_manf_name_string = (char*)header + ntohs(header->payload.manf_name_string_offset);
		}
		else
		{
			*out_manf_name_string = NULL;
		}
	}
	if (out_model_id)
	{
		*out_model_id = &header->payload.model_id;
	}
	if (out_model_id_string)
	{
		if(ntohs(header->payload.model_id_string_offset) != 0)
		{
			*out_model_id_string = (char*)header + ntohs(header->payload.model_id_string_offset);
		}
		else
		{
			*out_model_id_string = NULL;
		}
	}
	if (out_software_version)
	{
		*out_software_version = ntohl(header->payload.software_version);
	}
	if (out_software_build)
	{
		*out_software_build = ntohl(header->payload.software_build);
	}
	if (out_firmware_version)
	{
		*out_firmware_version = ntohl(header->payload.firmware_version);
	}
	if (out_firmware_build)
	{
		*out_firmware_build = ntohl(header->payload.firmware_build);
	}

	if (out_capability_flags)
	{
		*out_capability_flags = ntohl(header->payload.capability_flags);
	}
	if (out_model_version)
	{
		*out_model_version = ntohl(header->payload.model_version);
	}
	if (out_model_version_string)
	{
		if(ntohs(header->payload.model_version_string_offset) != 0)
		{
			*out_model_version_string = (char*)header + ntohs(header->payload.model_version_string_offset);
		}
		else
		{
			*out_model_version_string = NULL;
		}
	}

	return AUD_SUCCESS;
}

aud_error_t
ddp_read_device_erase_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t* out_request_id,
	ddp_status_t* out_status,
	uint16_t* out_supported_flags,
	uint16_t* out_executed_flags
)
{
	aud_error_t result;
	ddp_message_read_info_t message_info;
	ddp_device_erase_response_t * header;

	if (!packet_info || offset >= packet_info->packet_length_bytes)
	{
		return AUD_ERR_INVALIDPARAMETER;
	}
	result = ddp_packet_read_message(packet_info, &message_info, offset);
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	result = ddp_packet_validate_message(&message_info, DDP_OP_DEVICE_ERASE_CONFIG, sizeof(ddp_device_erase_response_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	header = (ddp_device_erase_response_t *) message_info.header.header;
	if (out_request_id)
	{
		*out_request_id = ntohs(header->subheader.request_id);
	}
	if (out_status)
	{
		*out_status = ntohs(header->subheader.status);
	}

	if (out_supported_flags)
	{
		*out_supported_flags = ntohs(header->payload.supported_flags);
	}
	if (out_executed_flags)
	{
		*out_executed_flags = ntohs(header->payload.executed_flags);
	}

	return AUD_SUCCESS;
}

aud_error_t
ddp_read_device_reboot_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t* out_request_id,
	ddp_status_t* out_status,
	uint16_t* out_supported_flags,
	uint16_t* out_executed_flags
)
{
	aud_error_t result;
	ddp_message_read_info_t message_info;
	ddp_device_reboot_response_t * header;

	if (!packet_info || offset >= packet_info->packet_length_bytes)
	{
		return AUD_ERR_INVALIDPARAMETER;
	}
	result = ddp_packet_read_message(packet_info, &message_info, offset);
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	result = ddp_packet_validate_message(&message_info, DDP_OP_DEVICE_REBOOT, sizeof(ddp_device_reboot_response_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	header = (ddp_device_reboot_response_t *) message_info.header.header;
	if (out_request_id)
	{
		*out_request_id = ntohs(header->subheader.request_id);
	}
	if (out_status)
	{
		*out_status = ntohs(header->subheader.status);
	}

	if (out_supported_flags)
	{
		*out_supported_flags = ntohs(header->payload.supported_flags);
	}
	if (out_executed_flags)
	{
		*out_executed_flags = ntohs(header->payload.executed_flags);
	}

	return AUD_SUCCESS;
}

aud_error_t
ddp_read_device_upgrade_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t* out_request_id,
	ddp_status_t* out_status,
	upgrade_rsp_payload_t* out_payload
)
{
	aud_error_t result;
	ddp_message_read_info_t message_info;
	ddp_device_upgrade_response_t * header;

	if (!packet_info || offset >= packet_info->packet_length_bytes)
	{
		return AUD_ERR_INVALIDPARAMETER;
	}
	result = ddp_packet_read_message(packet_info, &message_info, offset);
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	result = ddp_packet_validate_message(&message_info, DDP_OP_DEVICE_UPGRADE, sizeof(ddp_device_upgrade_response_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	header = (ddp_device_upgrade_response_t *) message_info.header.header;
	if (out_request_id)
	{
		*out_request_id = ntohs(header->subheader.request_id);
	}
	if (out_status)
	{
		*out_status = ntohs(header->subheader.status);
	}

	if (out_payload)
	{
		out_payload->valid_flags = ntohs(header->payload.valid_flags);
		out_payload->upgrade_stage = ntohs(header->payload.upgrade_stage);
		out_payload->upgrade_error = ntohl(header->payload.upgrade_error);
		out_payload->progress_current = ntohl(header->payload.progress_current);
		out_payload->progress_total = ntohl(header->payload.progress_total);
		out_payload->manf_id = &header->payload.manf_id;
		out_payload->model_id = &header->payload.model_id;
	}

	return AUD_SUCCESS;
}


aud_error_t
ddp_read_device_identity_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t* out_request_id,
	ddp_status_t* out_status,
	uint16_t* out_identity_status_flags,
	uint16_t* out_process_id,
	dante_device_id_t** out_dante_device_id,
	const char** out_default_name_string,
	const char** out_friendly_name_string,
	const char** out_dante_domain_string,
	const char** out_advertised_name_string
)
{
	aud_error_t result;
	ddp_message_read_info_t message_info;
	ddp_device_identity_response_t * header;

	if (!packet_info || offset >= packet_info->packet_length_bytes)
	{
		return AUD_ERR_INVALIDPARAMETER;
	}
	result = ddp_packet_read_message(packet_info, &message_info, offset);
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	result = ddp_packet_validate_message(&message_info, DDP_OP_DEVICE_IDENTITY, sizeof(ddp_device_identity_response_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	header = (ddp_device_identity_response_t *)message_info.header.header;
	if (out_request_id)
	{
		*out_request_id = ntohs(header->subheader.request_id);
	}
	if (out_status)
	{
		*out_status = ntohs(header->subheader.status);
	}
	if (out_identity_status_flags)
	{
		*out_identity_status_flags = ntohs(header->payload.identity_status_flags);
	}
	if (out_process_id)
	{
		*out_process_id = ntohs(header->payload.process_id);
	}
	if (out_dante_device_id)
	{
		*out_dante_device_id = &header->payload.device_id;
	}
	if (out_default_name_string)
	{
		uint16_t default_name_offset = ntohs(header->payload.default_name_offset);
		if (default_name_offset != 0)
		{
			*out_default_name_string = (char*)header + default_name_offset;
		}
		else
		{
			*out_default_name_string = NULL;
		}
	}
	if (out_friendly_name_string)
	{
		uint16_t friendly_name_offset = ntohs(header->payload.friendly_name_offset);
		if (friendly_name_offset != 0)
		{
			*out_friendly_name_string = (char*)header + friendly_name_offset;
		}
		else
		{
			*out_friendly_name_string = NULL;
		}
	}
	if (out_dante_domain_string)
	{
		uint16_t dante_domain_offset = ntohs(header->payload.dante_domain_offset);
		if (dante_domain_offset != 0)
		{
			*out_dante_domain_string = (char*)header + dante_domain_offset;
		}
		else
		{
			*out_dante_domain_string = NULL;
		}
	}
	if (out_advertised_name_string)
	{
		uint16_t advertised_name_offset = ntohs(header->payload.advertised_name_offset);
		if (advertised_name_offset != 0)
		{
			*out_advertised_name_string = (char*)header + advertised_name_offset;
		}
		else
		{
			*out_advertised_name_string = NULL;
		}
	}

	return AUD_SUCCESS;
}

aud_error_t
ddp_read_device_identify_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t* out_request_id,
	ddp_status_t* out_status
)
{
	aud_error_t result;
	ddp_message_read_info_t message_info;
	ddp_device_identify_response_t * header;

	if (!packet_info || offset >= packet_info->packet_length_bytes)
	{
		return AUD_ERR_INVALIDPARAMETER;
	}
	result = ddp_packet_read_message(packet_info, &message_info, offset);
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	result = ddp_packet_validate_message(&message_info, DDP_OP_DEVICE_IDENTIFY, sizeof(ddp_device_identify_response_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	header = (ddp_device_identify_response_t *)message_info.header.header;
	if (out_request_id)
	{
		*out_request_id = ntohs(header->subheader.request_id);
	}
	if (out_status)
	{
		*out_status = ntohs(header->subheader.status);
	}

	return AUD_SUCCESS;
}

aud_error_t
ddp_read_device_gpio_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t * out_request_id,
	ddp_status_t * out_status,
	uint32_t * out_interrupt_trigger_mask,
	uint32_t * out_input_state_valid_mask,
	uint32_t * out_input_state_values,
	uint32_t * out_output_state_valid_mask,
	uint32_t * out_output_state_values
)
{
	aud_error_t result;
	ddp_message_read_info_t message_info;
	ddp_device_gpio_response_t * header;

	if (!packet_info || offset >= packet_info->packet_length_bytes)
	{
		return AUD_ERR_INVALIDPARAMETER;
	}
	result = ddp_packet_read_message(packet_info, &message_info, offset);
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	result = ddp_packet_validate_message(&message_info, DDP_OP_DEVICE_GPIO, sizeof(ddp_device_gpio_response_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	header = (ddp_device_gpio_response_t *)message_info.header.header;
	if (out_request_id)
	{
		*out_request_id = ntohs(header->subheader.request_id);
	}
	if (out_status)
	{
		*out_status = ntohs(header->subheader.status);
	}
	if(out_interrupt_trigger_mask)
	{
		*out_interrupt_trigger_mask = ntohl(header->payload.interrupt_trigger_mask);
	}
	if(out_input_state_valid_mask)
	{
		*out_input_state_valid_mask = ntohl(header->payload.input_state_valid_mask);
	}
	if(out_input_state_values)
	{
		*out_input_state_values = ntohl(header->payload.input_state_values);
	}
	if(out_output_state_valid_mask)
	{
		*out_output_state_valid_mask = ntohl(header->payload.output_state_valid_mask);
	}
	if(out_output_state_values)
	{
		*out_output_state_values = ntohl(header->payload.output_state_values);
	}

	return AUD_SUCCESS;

}

aud_error_t
ddp_read_device_switch_led_response
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t offset,
	ddp_request_id_t * out_request_id,
	ddp_status_t * out_status,
	uint8_t * out_switch_led_mode
)
{
	aud_error_t result;
	ddp_message_read_info_t message_info;
	ddp_device_switch_led_response_t * header;

	if (!packet_info || offset >= packet_info->packet_length_bytes)
	{
		return AUD_ERR_INVALIDPARAMETER;
	}
	result = ddp_packet_read_message(packet_info, &message_info, offset);
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	result = ddp_packet_validate_message(&message_info, DDP_OP_DEVICE_SWITCH_LED, sizeof(ddp_device_switch_led_response_t));
	if (result != AUD_SUCCESS)
	{
		return result;
	}
	header = (ddp_device_switch_led_response_t *)message_info.header.header;
	if (out_request_id)
	{
		*out_request_id = ntohs(header->subheader.request_id);
	}
	if (out_status)
	{
		*out_status = ntohs(header->subheader.status);
	}
	if(out_switch_led_mode)
	{
		*out_switch_led_mode = header->payload.switch_led_mode;
	}
	return AUD_SUCCESS;
}
