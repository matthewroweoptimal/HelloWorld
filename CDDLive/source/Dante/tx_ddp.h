/**
* File     : example_tx_ddp.h
* Created  : October 2014
* Updated  : 2014/10/08
* Author   : Chamira Perera
* Synopsis : HostCPU Implementation of various example DDP requests
*
* This module implements the following DDP requests
* 1. Querying the Dante device's network interface state
* 2. Configuring static IP address for a Dante device
* 3. Subscribe the Rx audio channels of a Dante device to another Dante device (transmitter, Tx)
* 4. Creating a multicast Tx for flow for a Dante device
* 5. Delete the multicast Tx flow for a Dante device created in example 4
* 6. Upgrade the application firmware for a Dante device using XMODEM over SPI
* 7. Change the audio sample rate of a Dante device
* 8. Change the clock pullup of a Dante device
* 9. Set a GPIO output of a Dante device
*
* Copyright 2005-2014, Audinate Pty Ltd and/or its licensors
Confidential
* Audinate Copyright Header Version 1
*/

#ifndef __EXAMPLE_TX_DDP_H__
#define __EXAMPLE_TX_DDP_H__

#include "aud_platform.h"

/**
* Send a request to retreive information about the network interface of the Dante device
* @return AUD_SUCCESS if the packet was sucessfully created and transmitted, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t send_network_interface_query(void);

/**
* Configures a Dante device with a static IP address
* @return AUD_SUCCESS if the packet was sucessfully created and transmitted, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t configure_static_ip(void);

/**
* Susbscribe two Rx audio channels to two Tx audio channels
* @return AUD_SUCCESS if the packet was sucessfully created and transmitted, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t subscribe_channels(void);

/**
* Create a multicast Tx flow with two slots (two Tx channels)
* @return AUD_SUCCESS if the packet was sucessfully created and transmitted, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t create_tx_multicast_flow(void);

/**
* Deletes the multicast Tx flow created in create_tx_multicast_flow
* @return AUD_SUCCESS if the packet was sucessfully created and transmitted, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t delete_flow(void);

/**
* Upgrade the firmware on the Dante device using SPI and XMODEM
* @return AUD_SUCCESS if the packet was sucessfully created and transmitted, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t upgrade_via_xmodem(void);

/**
* Configures a Dante device with a new audio sampling rate
* @return AUD_SUCCESS if the packet was sucessfully created and transmitted, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t configure_sample_rate(void);

/**
* Configures a Dante device with a new clock pullup and subdomain
* @return AUD_SUCCESS if the packet was sucessfully created and transmitted, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t configure_clock_pullup(void);

/**
* Set the GPIO output Dante device to a particular state
* @return AUD_SUCCESS if the packet was sucessfully created and transmitted, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t set_gpio_output(void);

/**
* Turn off Switch LEDs on the Dante device
* @return AUD_SUCCESS if the packet was sucessfully created and transmitted, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t turn_off_switch_leds(void);

/**
* Send a request to retreive general information about the Dante device
* @return AUD_SUCCESS if the packet was sucessfully created and transmitted, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t send_device_general_query(void);

/**
* Send a request to retreive manufacturer information about the Dante device
* @return AUD_SUCCESS if the packet was sucessfully created and transmitted, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t send_device_manuf_info(void);

/**
* Send a request to retreive manufacturer information about the Dante device
* @return AUD_SUCCESS if the packet was sucessfully created and transmitted, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t send_device_dev_id(char *pszNewName);

/**
* Send a request to retreive channel status information about the Dante device
* @return AUD_SUCCESS if the packet was sucessfully created and transmitted, otherwise a value other than AUD_SUCCESS is returned
*/
aud_error_t rquest_channel_status(uint16_t usFromToChan, uint16_t usToChan);

#endif // __EXAMPLE_TX_DDP_H__
