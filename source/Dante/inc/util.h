#ifndef _DDP_UTIL_H
#define _DDP_UTIL_H

#include "ddp.h"

#ifdef __cplusplus
extern "C" {
#endif

const char * ddp_opcode_to_string(ddp_opcode_t opcode);

const char * ddp_status_to_string(ddp_status_t status);

const char * ddp_pullup_to_string(uint32_t pullup);

aud_bool_t
ddp_verify_spi_baud_rate(uint32_t baud);

aud_bool_t
ddp_verify_uart_baud_rate(uint32_t baud);

#ifdef __cplusplus
}
#endif

#endif // _DDP_UTIL_H
