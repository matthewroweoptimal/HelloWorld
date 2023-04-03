/*
 * File     : ddp_shared_structures.h
 * Created  : June 2014
 * Author   : Andrew White <andrew.white@audinate.com>
 * Synopsis : Shared structures and types
 *
 * Copyright 2005-2014, Audinate Pty Ltd and/or its licensors
	Confidential
 * Audinate Copyright Header Version 1
 */
#ifndef _DDP_SHARED_STRUCTURES__H
#define _DDP_SHARED_STRUCTURES__H

//----------
// Include

#include "ddp.h"

//----------
// Audio Transports

/**
* @enum ddp_audio_transport
* @brief Audio Transport protocol values used for various messages
*/
enum ddp_audio_transport
{
	DDP_AUDIO_TRANSPORT_UNDEF	= 0,		/*!< Undefined audio transport */
	DDP_AUDIO_TRANSPORT_ATP	= 1				/*!< Dante ATP */
};


//----------
// Addresses

/**
* @enum ddp_address_family
* @brief Address family values used for various messages
*/
enum ddp_address_family
{
	DDP_ADDRESS_FAMILY_UNDEF = 0, 	/*!< Undefined address family */
	DDP_ADDRESS_FAMILY_INET	= 2, 	/*!< IPv4 address family */
};

/**
* @struct ddp_addr_inet
* @brief Structure format for the DDP address format (IP address : port)
*/
struct ddp_addr_inet
{
	uint16_t family;			/*!< IP address family @see ddp_address_family */
	uint16_t port;				/*!< UDP/IP port */
	uint32_t in_addr;			/*!< IP address */
};

/** @cond */
typedef struct ddp_addr_inet ddp_addr_inet_t;
/** @endcond */


//----------

#endif // _DDP_SHARED_STRUCTURES__H
