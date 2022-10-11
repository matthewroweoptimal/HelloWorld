#include "packet.h"

aud_error_t
ddp_packet_init_read
(
	ddp_packet_read_info_t * packet_info,
	const uint32_t * buf,
	ddp_size_t len
) {
	if (!packet_info || !buf || !len)
	{
		return AUD_ERR_INVALIDPARAMETER;
	}
	packet_info->_.buf32 = buf;
	packet_info->packet_length_bytes = len;

	if (ntohs(packet_info->_.header->block._.length_bytes) < sizeof(ddp_packet_header_t))
	{
		return AUD_ERR_INVALIDDATA;
	}
	packet_info->version = ntohs(packet_info->_.header->version);
	return AUD_SUCCESS;
}

aud_error_t
ddp_packet_read_block_header
(
	const ddp_packet_read_info_t * packet_info,
	uint16_t packet_offset_bytes,
	ddp_block_header_t * block_header
) {
	ddp_block_header_t * raw_header;
	if (!packet_info || !block_header || (packet_offset_bytes & 0x3))
	{
		return AUD_ERR_INVALIDPARAMETER;
	}
	if (packet_offset_bytes + sizeof(ddp_block_header_t) >= packet_info->packet_length_bytes)
	{
		return AUD_ERR_INVALIDDATA;
	}
	raw_header = (ddp_block_header_t *)(packet_info->_.buf32 + packet_offset_bytes / 4);
	block_header->_.length_bytes = ntohs(raw_header->_.length_bytes);
	block_header->_.opcode = ntohs(raw_header->_.opcode);
	return AUD_SUCCESS;
}

aud_error_t
ddp_packet_read_message
(
	const ddp_packet_read_info_t * packet_info,
	ddp_message_read_info_t * message_info,
	uint16_t packet_offset_bytes
) {
	if (!packet_info || !message_info || (packet_offset_bytes & 0x3))
	{
		return AUD_ERR_INVALIDPARAMETER;
	}
	message_info->packet_info = packet_info;
	message_info->packet_offset_bytes = packet_offset_bytes;
	message_info->header.value8 = packet_info->_.buf8 + packet_offset_bytes;

	// generic message-level validation
	message_info->length_bytes = ntohs(message_info->header.header->block._.length_bytes);
	message_info->opcode = htons(message_info->header.header->block._.opcode);
	message_info->subheader_length_bytes = ntohs(message_info->header.header->subheader_length_bytes);
	message_info->payload_length_bytes = ntohs(message_info->header.header->payload_length_bytes);
	message_info->heap_length_bytes = message_info->length_bytes - sizeof(ddp_message_header_t) - message_info->subheader_length_bytes - message_info->payload_length_bytes;

	// validate message block starting alignment
	if (message_info->length_bytes & 0x3)
	{
		return AUD_ERR_INVALIDDATA;
	}
	// validate message block length fits inside packet
	if (packet_offset_bytes + message_info->length_bytes > packet_info->packet_length_bytes)
	{
		return AUD_ERR_INVALIDDATA;
	}

	// validate that the message header length is as big as a minimum-size
	if (message_info->length_bytes < sizeof(ddp_message_header_t))
	{
		return AUD_ERR_INVALIDDATA;
	}
	// validate that the subheader and payload are 32-bit aligned
	if (message_info->subheader_length_bytes & 0x3 || message_info->payload_length_bytes & 0x3)
	{
		if (message_info->opcode != DDP_OP_MONITOR_TIMER_ACCURACY)
		{
			return AUD_ERR_INVALIDDATA;
		}
	}
	// validate that the message header length fits into the message length
	if (message_info->length_bytes < sizeof(ddp_message_header_t) + message_info->subheader_length_bytes + message_info->payload_length_bytes)
	{
		return AUD_ERR_INVALIDDATA;
	}
	message_info->subheader.value8 = (const uint8_t *) (message_info->header.header + 1);
	message_info->payload.value8 = message_info->payload_length_bytes ?
		(message_info->subheader.value8 + message_info->subheader_length_bytes) : 0;
	return AUD_SUCCESS;
}

aud_error_t
ddp_packet_first_message_offset
(
	ddp_packet_read_info_t * packet_info,
	ddp_offset_t * first_message_offset_p,
	const uint32_t * buf,
	size_t buflen
)
{
	aud_error_t result;

	result = ddp_packet_init_read(packet_info, buf, (ddp_size_t)buflen);
	if (result != AUD_SUCCESS)
		return result;

	return ddp_packet_next_message_offset(packet_info, first_message_offset_p, 0);
}


aud_error_t
ddp_packet_next_message_offset
(
	const ddp_packet_read_info_t * packet_info,
	ddp_offset_t * message_offset_p,
	ddp_offset_t curr_offset
)
{
	aud_error_t result;
	ddp_block_header_t bh;

	if (! (packet_info))
		return AUD_ERR_INVALIDPARAMETER;

	result = ddp_packet_read_block_header(packet_info, curr_offset, &bh);
	if (result != AUD_SUCCESS)
		return result;

	if (curr_offset == 0 && bh._.opcode != DDP_OP_PACKET_HEADER)
	{
		return AUD_ERR_INVALIDDATA;
	}

	curr_offset += bh._.length_bytes;
	if (curr_offset + sizeof(ddp_block_header_t) < packet_info->packet_length_bytes)
	{
		if (message_offset_p)
		{
			* message_offset_p = curr_offset;
		}
		return AUD_SUCCESS;
	}
	return AUD_ERR_NOTFOUND;
}


aud_error_t
ddp_packet_init_read_first_message
(
	ddp_packet_read_info_t * packet_info,
	ddp_message_read_info_t * message_info,
	const uint32_t * buf,
	size_t buflen
)
{
	aud_error_t result;
	ddp_offset_t packet_offset;

	result = ddp_packet_first_message_offset(packet_info, &packet_offset, buf, buflen);
	if (result != AUD_SUCCESS)
		return result;

	return ddp_packet_read_message(packet_info, message_info, packet_offset);
}


aud_error_t
ddp_read_heap_array
(
	const ddp_message_read_info_t * message_info,
	ddp_heap_read_info_t * heap_info,
	ddp_offset_t msg_payload_offset,
	uint16_t num_elements,
	uint16_t element_size_bytes
)
{
	uint16_t heap_size;
	if (! (message_info && heap_info && num_elements && element_size_bytes))
	{
		return AUD_ERR_INVALIDPARAMETER;
	}

	//validate that the offset is on a 32-bit boundary
	if (msg_payload_offset & 0x03)
	{
		return AUD_ERR_INVALIDPARAMETER;
	}

	// validate that the heap fits inside the message
	//NOTE: the heap payload offset is a offset from the start of the payload, so to check that it fits
	heap_size = num_elements * element_size_bytes;
	if(msg_payload_offset + heap_size > message_info->length_bytes)
	{
		return AUD_ERR_INVALIDDATA;
	}

	//populate the heap read info structure
	heap_info->message_info = message_info;
	heap_info->num_elements = num_elements;
	heap_info->element_size_bytes = element_size_bytes;
	heap_info->length_bytes = heap_size;
	heap_info->_.array8 = message_info->header.value8 + msg_payload_offset;
	heap_info->offset_array = NULL;

	return AUD_SUCCESS;
}


const void *
ddp_read_heap_element_at_index
(
	const ddp_heap_read_info_t * heap,
	unsigned index
)
{
	if (! (heap && heap->_.array8 && index < heap->num_elements))
		return NULL;

	return (const void *) (heap->_.array8 + (index * heap->element_size_bytes));
}


const void *
ddp_heap_resolve_sized
(
	const ddp_message_read_info_t * message_info,
	ddp_offset_t message_offset,
	ddp_size_t min_size
)
{
	const uint8_t * result;
	if (message_offset == 0
		|| ! (message_info && message_info->payload.value8)
		|| message_offset + min_size > message_info->length_bytes
	)
		return NULL;

	result = message_info->header.value8 + message_offset;
	if (result < message_info->payload.value8)
		return NULL;

	return result;
}


char *
ddp_heap_read_string
(
	const ddp_message_read_info_t * message_info,
	ddp_offset_t message_offset
)
{
	unsigned i;
	const uint8_t * result = (const uint8_t *) ddp_heap_resolve_sized(message_info, message_offset, 1);
	if (! result)
		return NULL;

	// validate overruns
	for (i = message_offset; message_info->header.value8[i]; i++)
	{
		if (i >= message_info->length_bytes)
			return NULL;
	}

	return (char *) result;
}

ddp_request_id_t
ddp_packet_read_response_request_id
(
	ddp_message_read_info_t * message_info
)
{
	return ntohs(message_info->subheader.response->request_id);
}

ddp_status_t
ddp_packet_read_response_status
(
	ddp_message_read_info_t * message_info
)
{
	return ntohs(message_info->subheader.response->status);
}
