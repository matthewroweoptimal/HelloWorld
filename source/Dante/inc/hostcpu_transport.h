/**
 * File     : hostcpu_transport.h
 * Created  : October 2014
 * Updated  : 2014/10/02
 * Author   : Michael Ung
 * Synopsis : HostCPU Transport (physical) Interface
 *
 * This is an interface into the SPI / UART peripheral on the Host CPU
 *
 * The implementation of this interface must be provided on the HostCPU platform by the customer.
 *
 * Copyright 2005-2014, Audinate Pty Ltd and/or its licensors
 * Confidential
 * Audinate Copyright Header Version 1
 */
#ifndef __HOSTCPU_TRANSPORT_H__
#define __HOSTCPU_TRANSPORT_H__

#include "aud_platform.h"

/**
 * Initialises the HOST CPU transport peripheral
 * @return AUD_TRUE if the init was successful, otherwise AUD_FALSE
 */
aud_bool_t hostcpu_transport_init(int nPort);

/**
 * Read bytes from the transport interface
 * @param buffer [in] Pointer to the buffer to read into
 * @param max_bytes_to_read [in] The maximum number of bytes to read
 * @return the number of bytes read
 */
uint16_t hostcpu_transport_read(uint8_t* buffer, uint16_t max_bytes_to_read);

/**
 * Write bytes to the transport interface
 * @param buffer [in] Pointer to the buffer to write
 * @param num_bytes [in] The number of bytes to write
 * @return the number of bytes written
 */
uint16_t hostcpu_transport_write(uint8_t const * buffer, uint16_t num_bytes);

#endif // __HOSTCPU_TRANSPORT_H__
