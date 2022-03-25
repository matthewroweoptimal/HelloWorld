/*
 * network.c
 *
 *  Created on: Sep 14, 2015
 *      Author: Kris.Simonsen
 */

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////


// Standard C Included Files
#include <stdio.h>
// lwip Included Files
/* Includes ------------------------------------------------------------------*/
#include "network.h"
#include "string.h"

extern "C" {
#include "lwip/opt.h"
#include "lwip/api.h"
}

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TCPECHOSERVER_THREAD_PRIO    ( tskIDLE_PRIORITY + 2UL )
#define TCPECHOSERVER_THREAD_STACKSIZE  300
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


mandolin_fifo	netRxFifo;
uint8_t			netRxFifo_bytes[NETWORK_MANDOLIN_FIFO_SIZE];
uint32_t		g_CurrentIpAddress;

//_queue_id	tcp_qid;

#if 0
struct netif fsl_netif0;

//extern oly::Config *olyConfig;	//	Messy going through Config object, won't be necessary when this file combined with NetworkPort.cpp

void dhcp_callback(struct netif* pnetif)
{
	if (g_CurrentIpAddress != pnetif->ip_addr.addr)
	{
		printf("\nIPv4 Address changed to %d.%d.%d.%d.\r\n", IPBYTES(pnetif->ip_addr.addr));
		g_CurrentIpAddress = pnetif->ip_addr.addr;
//		olyConfig->OnIPAddressChanged(pnetif->ip_addr.addr);
	}
}
#endif

static void tcp_mandolin_thread(void *arg)
{
    struct netconn *conn, *newconn;
    int i;
    err_t err;
    mandolin_parse_error error;
    bool connected;
    LWIP_UNUSED_ARG(arg);

    // Create a new connection identifier
    conn = netconn_new(NETCONN_TCP);

    netconn_bind(conn, NULL, MANDOLIN_PORT_TCP);

    // Tell connection to go into listening mode
    netconn_listen(conn);

    while (1)
    {
        /* Grab new connection. */
        err = netconn_accept(conn, &newconn);		// Blocking until connection received
    	printf("Accept:%s/n", lwip_strerr(err));

        /* Process the new connection. */
        if (err == ERR_OK)
        {
            struct netbuf *buf;
            uint8_t * data;
            uint16_t len;

            netconn_set_recvtimeout(newconn, 10);
            connected = true;
            printf("Connection established on port %d\n", MANDOLIN_PORT_TCP);

            while (connected)
            {
            	err = netconn_recv(newconn, &buf);	// Blocking until data received or timeout
            	if (err == ERR_OK)
            	{
					do {
						// Data received, copy packet to Rx FIFO
						netbuf_data(buf, (void **)&data, &len);
						if (MANDOLIN_FIFO_bytes_empty(&netRxFifo) > len)
							MANDOLIN_FIFO_enqueue_bytes(&netRxFifo, data, len);
						else
							printf("Network Mandolin FIFO overfull\n");

						for (i=0; i< len;i++) {
							if (!MANDOLIN_FIFO_is_full(&netRxFifo))
							{
								MANDOLIN_FIFO_enqueue_byte(&netRxFifo, data[i]);
								printf("Message received\n");
							}
							else
							{
								printf("Network Mandolin FIFO overfull\n");
							}

						}
					}

					while (netbuf_next(buf) >= 0);
					{
					netbuf_delete(buf);
					}
            	}
            	else if (err == ERR_TIMEOUT)
            	{
            			int nErr;
                 		/* Close connection and discard connection identifier. */
						//nErr = netconn_close(newconn);
						//printf("time out triggered\n");

						//nErr = netconn_delete(newconn);
						//printf("netconn-delete: %s\n", lwip_strerr(nErr));

						/* lwip wiki states it could be there may be issues if attempt to bind to same address. although address not specified so may not be issue... */
						//connected = false;



            	} else {

                    printf("%s Connection terminated\n", lwip_strerr(err));

            		/* Close connection and discard connection identifier. */
                    connected = false;
					netconn_close(newconn);
					netconn_delete(newconn);

            	}
            }
        }
        else if(err == ERR_TIMEOUT)
        {// Haven't received any information in TIMEOUT, check if it's because the network has disconnected.
        }
    }
}
#if 0
void network_test(void)
{
	ip_addr_t fsl_netif0_ipaddr, fsl_netif0_netmask, fsl_netif0_gw;

	LWIP_DEBUGF(PING_DEBUG,("TCP/IP initializing...\r\n"));
	tcpip_init(NULL,NULL);
	LWIP_DEBUGF(PING_DEBUG,("TCP/IP initialized.\r\n"));

	IP4_ADDR(&fsl_netif0_ipaddr, 10,1,7,200);
	IP4_ADDR(&fsl_netif0_netmask, 255,255,255,0);
	IP4_ADDR(&fsl_netif0_gw, 10,1,1,1);

	netif_add(&fsl_netif0, &fsl_netif0_ipaddr, &fsl_netif0_netmask, &fsl_netif0_gw, NULL, ethernetif_init, tcpip_input);
	netif_set_default(&fsl_netif0);

	netif_set_up(&fsl_netif0);
	sys_thread_new("tcp_mandolin_thread", tcp_mandolin_thread, NULL, 3000, 3);
}
#endif
void network_UseStaticIP(uint32_t * ipaddr, uint32_t * gateway, uint32_t * mask)
{
	sys_thread_new("tcp_mandolin_thread", tcp_mandolin_thread, NULL, 5000, 6);
}

void network_UseDHCP(void)
{
#if 0
	ip_addr_t fsl_netif0_ipaddr, fsl_netif0_netmask, fsl_netif0_gateway;

	LWIP_DEBUGF(PING_DEBUG,("TCP/IP initializing...\r\n"));
	tcpip_init(NULL,NULL);
	LWIP_DEBUGF(PING_DEBUG,("TCP/IP initialized.\r\n"));

	IP4_ADDR(&fsl_netif0_ipaddr, 0,0,0,0);
	IP4_ADDR(&fsl_netif0_netmask, 0,0,0,0);
	IP4_ADDR(&fsl_netif0_gateway, 0,0,0,0);

	netif_add(&fsl_netif0, &fsl_netif0_ipaddr, &fsl_netif0_netmask, &fsl_netif0_gateway, NULL, ethernetif_init, tcpip_input);
	netif_set_default(&fsl_netif0);

	//netif_set_up(&fsl_netif0);

	netif_set_status_callback(&fsl_netif0, dhcp_callback);

	dhcp_start(&fsl_netif0);
//	autoip_start(&fsl_netif0);
#endif
	network_init();
	sys_thread_new("tcp_mandolin_thread", tcp_mandolin_thread, NULL, 5000, 6);

}

void network_init(void)
{
	uint32_t 	result;

	MANDOLIN_FIFO_init(&netRxFifo,NETWORK_MANDOLIN_FIFO_SIZE, netRxFifo_bytes);
}
#if 0
extern "C" {
//	retrieves read-only pointer to network data and optionally copies to passed structure.
//	if p_netif is null, skips copy.
const struct netif *network_get_netif(struct netif *p_netif)
{
	if (p_netif)
		*(p_netif) = fsl_netif0;
	return(&fsl_netif0);
}

}
#endif
