/**
* File     : frame.h
* Created  : October 2014
* Updated  : 2014/10/30
* Author   : Chamira Perera
* Synopsis : Implementation of utility functions to handle serial frames
*
* Copyright 2005-2014, Audinate Pty Ltd and/or its licensors
Confidential
* Audinate Copyright Header Version 1
*/

#ifndef __FRAME_H__
#define __FRAME_H__

#include "aud_platform.h"

typedef void(*timer_start_t)(int);
typedef void(*timer_stop_t)(void);

/**
* Adds the delimiter and pad bytes to a COBS encoded data packet
* @param cobs_buffer [in] Pointer to the COBS encoded message
* @param cobs_encoded_size [in] Size of the COBS encoded message
* @return The total size of the data including the delimiter and pad bytes
*/
size_t prepare_tx_frame(uint8_t *cobs_buffer, size_t cobs_encoded_size);

/**
* Extracts the COBS encoded data within a frame iteratively. The function should
* be called for all of the received data. 
* @param current_byte [in] the current received serial data byte
* @param out_buffer [in out] pointer to the buffer which contains the COBS data
* @param num_extracted [out] pointer to the amount of data extracted
* @param rx_timer_start [in optional] function pointer to the function to start the data receive timer
* @param rx_timer_stop [in optional] function pointer to the function to stop the data receive timer
* @param timeout [in optional] timeout value passed into the rx_timer_start function
*/
aud_error_t extract_cobs_from_rx_frame(uint8_t current_byte, uint8_t *out_buffer, size_t *num_extracted, timer_start_t rx_timer_start, timer_stop_t rx_timer_stop, int timeout);

void reset_cobs_state();

#endif // __FRAME_H__
