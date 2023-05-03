/*
 * network.h
 *
 *  Created on: Sep 14, 2015
 *      Author: Kris.Simonsen
 */

#ifndef SOURCES_NETWORK_H_
#define SOURCES_NETWORK_H_

#include "oly.h"
#include "CommMandolin.h"
#include "InputHandler.h"
#include "Threads.h"


void network_init(void);
void network_test(void);

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
#ifndef PING_DEBUG
#define PING_DEBUG     		LWIP_DBG_ON
#endif

/* Helpful macros.*/
#define IPBYTES(a)          ((a)&0xFF),(((a)>>8)&0xFF),(((a)>>16)&0xFF),(((a)>> 24)&0xFF)
#define IPADDR(a,b,c,d)     ((((uint32_t)(d)&0xFF)<<24)|(((uint32_t)(c)&0xFF)<<16)|(((uint32_t)(b)&0xFF)<<8)|((uint32_t)(a)&0xFF))
#define IPREVERSEBYTES(a)	((a>>24)&0x0ff) | ((a>>8)&0x0ff00) | ((a&0x0ff00)<<8) | ((a&0x0ff)<<24);

#define MANDOLIN_PORT_TCP	50001
#define NETWORK_MANDOLIN_FIFO_SIZE	(MANDOLIN_MAX_MESSAGE_BYTES + MANDOLIN_BUFFER_SLOP_BYTES)

extern mandolin_fifo		netRxFifo;
extern uint8_t				netRxFifo_bytes[NETWORK_MANDOLIN_FIFO_SIZE];

//	Public API
//	retrieves read-only pointer to network data and optionally copies to passed structure if pointer non-zero
const struct netif *network_get_netif(struct netif *p_netif);
void networkInitialisation(ip_addr_t *pIpAddr, ip_addr_t *pGateway, ip_addr_t *pNetmask);
void network_UseDHCP(void);
void network_UseStaticIP(uint32_t *ipaddr, uint32_t *gateway, uint32_t *mask);

#endif /* SOURCES_NETWORK_H_ */
