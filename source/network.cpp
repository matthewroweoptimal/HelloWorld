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
#include "network.h"

#if 0
// lwip Included Files
#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"

//#include "lwip/timers.h"
#include "lwip/timeouts.h"

#include "lwip/inet_chksum.h"
#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/tcpip.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "lwip/dhcp.h"
#include "lwip/tcp.h"
#endif

// SDK Included Files
//#include "fsl_clock_manager.h"
//#include "fsl_os_abstraction.h"
extern "C" {
#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/netif.h"

//#include "ethernetif.h"
}
#include "board.h"
//#include "lwevent.h"
#include "ConfigManager.h"

#include "os_tasks.h"


mandolin_fifo	netRxFifo;
uint8_t			__attribute__ ((section(".external_ram"))) netRxFifo_bytes[NETWORK_MANDOLIN_FIFO_SIZE];
uint32_t		g_CurrentIpAddress;

_queue_id	tcp_qid;

struct netif fsl_netif0;

extern oly::Config *olyConfig;	//	Messy going through Config object, won't be necessary when this file combined with NetworkPort.cpp

void dhcp_callback(struct netif* pnetif)
{
	if (g_CurrentIpAddress != pnetif->ip_addr.addr)
	{
		printf("\nIPv4 Address changed to %d.%d.%d.%d.\r\n", IPBYTES(pnetif->ip_addr.addr));
		g_CurrentIpAddress = pnetif->ip_addr.addr;
		olyConfig->OnIPAddressChanged(pnetif->ip_addr.addr);
	}
}

static void tcp_mandolin_thread(void *arg)
{
    struct netconn *conn, *newconn;
    int i;
    err_t err;
    mandolin_parse_error error;
    bool connected;
    LWIP_UNUSED_ARG(arg);

    tcp_qid = _msgq_open(TCP_QUEUE, 0);

	if (tcp_qid == 0) {
		printf("\nError creating message queue\n");
		_task_block();
	}

    // Create a new connection identifier
    conn = netconn_new(NETCONN_TCP);

#if LWIP_DHCP
//    if(fsl_netif0.dhcp)
//    	netconn_set_recvtimeout(conn, 10000);
#endif

    netconn_bind(conn, NULL, MANDOLIN_PORT_TCP);

    // Tell connection to go into listening mode
    netconn_listen(conn);

    while (1)
    {
        /* Grab new connection. */
        err = netconn_accept(conn, &newconn);		// Blocking until connection received
//    	printf("Accept:%s/n", lwip_strerr(err));

        /* Process the new connection. */
        if (err == ERR_OK)
        {
            struct netbuf *buf;
            mandolin_mqx_message_ptr rx_msg = NULL;
            mandolin_mqx_msg_packed_ptr tx_msg = NULL;
            uint8_t * data;
            uint16_t len;

            netconn_set_recvtimeout(newconn, 10);
            connected = true;
            olyConfig->OpenNetworkPort(true, 0);	//	Messy going through Config object, won't be necessary when this file combined with NetworkPort.cpp
            _lwevent_set(&sys_event, event_net_connect);
            printf("Connection established on port %d\n", MANDOLIN_PORT_TCP);

            while (connected)
            {
            	err = netconn_recv(newconn, &buf);	// Blocking until data received or timeout
//            	printf("Rx:%s ", lwip_strerr(err));
            	TASKDEBUG_IN(TASK_NETWORK)
            	if (err == ERR_OK)
            	{
					do {
		            	TASKDEBUG_POS(TASK_NETWORK, 1)

						// Data received, copy packet to Rx FIFO
						netbuf_data(buf, (void **)&data, &len);
		            	TASKDEBUG_POS(TASK_NETWORK, 2)

#if 1
						if (MANDOLIN_FIFO_bytes_empty(&netRxFifo) > len)
							MANDOLIN_FIFO_enqueue_bytes(&netRxFifo, data, len);
						else
							printf("Network Mandolin FIFO overfull\n");

#else
		            	TASKDEBUG_POS(TASK_NETWORK, 3)

						for (i=0; i< len;i++) {
							if (!MANDOLIN_FIFO_is_full(&netRxFifo))
							{
				            	TASKDEBUG_POS(TASK_NETWORK, 4)

								MANDOLIN_FIFO_enqueue_byte(&netRxFifo, data[i]);
				            	TASKDEBUG_POS(TASK_NETWORK, 5)

							}
							else
							{
				            	TASKDEBUG_POS(TASK_NETWORK, 6)

								printf("Network Mandolin FIFO overfull\n");
			            		TASKDEBUG_POS(TASK_NETWORK, 7)
							}

						}
#endif

						// Attempt to parse data in FIFO
						//ParseMandolinMQX(tcp_qid, COMM_ETHERNET_TCP, &netRxFifo, 0, TASK_NETWORK);		// try to send stuff first to free message_pool in tcp_qid
		            	TASKDEBUG_POS(TASK_NETWORK, 8)

						while (tx_msg = (mandolin_mqx_msg_packed_ptr)_msgq_poll(tcp_qid))
						{
#if 0
							printf("Tx(%d):", tx_msg->length);
							int nBytes = tx_msg->length;
							if (nBytes>16)
								nBytes = 16;
							for(int nByte = 0;nByte<nBytes; nByte++)
								printf("%02X,", *(tx_msg->data+nByte));
							// Message available in buffer
							printf("\n");
#endif
							int nErr;

			            	TASKDEBUG_POS(TASK_NETWORK, 9)

//							printf("TCP Tx MSG during Rx : %08x\n", tx_msg);

#if 0
			            	nErr = netconn_write(newconn, tx_msg->data, tx_msg->length, NETCONN_COPY);//NETCONN_NOCOPY);
#else
							if (nErr=netconn_write(newconn, tx_msg->data, tx_msg->length, NETCONN_COPY))
							{
								TASKDEBUG_POS(TASK_NETWORK, 10)
								MSG_FREE(tcp_qid, tx_msg);
								if(nErr == ERR_TIMEOUT)	{
									while (tx_msg = (mandolin_mqx_msg_packed_ptr)_msgq_poll(tcp_qid))
										MSG_FREE(tcp_qid, tx_msg);
									olyConfig->olyNetworkPort.SetForceClose();
								}
							}
							else
#endif
								MSG_FREE(tcp_qid, tx_msg);
//							printf("%s\n", lwip_strerr(nErr));


							//printf("Writing NET msg of length %d\n", tx_msg->length);
			            	TASKDEBUG_POS(TASK_NETWORK, 11)


						}
		            	TASKDEBUG_POS(TASK_NETWORK, 12)

						// Attempt to parse data in FIFO
						ParseMandolinMQX(tcp_qid, COMM_ETHERNET_TCP, &netRxFifo, 0, TASK_NETWORK);	// if no room ... lose it
		            	TASKDEBUG_POS(TASK_NETWORK, 13)


					} while (netbuf_next(buf) >= 0);

	            	TASKDEBUG_POS(TASK_NETWORK, 14)

					netbuf_delete(buf);
	            	TASKDEBUG_POS(TASK_NETWORK, 15)

            	}
            	else if (err == ERR_TIMEOUT)
            	{
            		while (tx_msg = (mandolin_mqx_msg_packed_ptr)_msgq_poll(tcp_qid))
					{
#if 0
            			printf("TxA(%d):", tx_msg->length);
						int nBytes = tx_msg->length;
						if (nBytes>16)
							nBytes = 16;
						for(int nByte = 0;nByte<nBytes; nByte++)
							printf("%02X,", *(tx_msg->data+nByte));
						// Message available in buffer
						printf("\n");
#endif
						// Tx Message available in buffer
    					int nErr;

		            	TASKDEBUG_POS(TASK_NETWORK, 16)

//						printf("TCP Tx MSG : %08x\n", tx_msg);

						if (nErr=netconn_write(newconn, tx_msg->data, tx_msg->length, NETCONN_COPY))
						{
							TASKDEBUG_POS(TASK_NETWORK, 17);
							MSG_FREE(tcp_qid, tx_msg);
							if(nErr == ERR_TIMEOUT) {
								while (tx_msg = (mandolin_mqx_msg_packed_ptr)_msgq_poll(tcp_qid))
									MSG_FREE(tcp_qid, tx_msg);
								olyConfig->olyNetworkPort.SetForceClose();
							}
						}
						else MSG_FREE(tcp_qid, tx_msg);
						//printf("Writing NET msg of length (poll) %d\n", tx_msg->length);
//						printf(" %s\n", lwip_strerr(nErr));

						TASKDEBUG_POS(TASK_NETWORK, 18)
					}

            		if (olyConfig->GetForceNetworkPortClose(0))	//	Messy going through Config object, won't be necessary when this file combined with NetworkPort.cpp
            		{
            			int nErr;

		            	TASKDEBUG_POS(TASK_NETWORK, 19)

                        printf("%s Connection terminated\n", lwip_strerr(err));

		            	TASKDEBUG_POS(TASK_NETWORK, 20)
						 olyConfig->OpenNetworkPort(false, 0);	//	Messy going through Config object, won't be necessary when this file combined with NetworkPort.cpp
						 _lwevent_set(&sys_event, event_net_disconnect);


		            	TASKDEBUG_POS(TASK_NETWORK, 21)
//						printf("%s ", lwip_strerr(err));

                 		/* Close connection and discard connection identifier. */
						nErr = netconn_close(newconn);
						printf("netconn_close: %s\n", lwip_strerr(nErr));

						nErr = netconn_delete(newconn);
						printf("netconn-delete: %s\n", lwip_strerr(nErr));

						connected = false;


            		}
            	}else {
	            	TASKDEBUG_POS(TASK_NETWORK, 22)

                    printf("%s Connection terminated\n", lwip_strerr(err));

            		/* Close connection and discard connection identifier. */
                    connected = false;
                    olyConfig->OpenNetworkPort(false, 0);	//	Messy going through Config object, won't be necessary when this file combined with NetworkPort.cpp
	            	TASKDEBUG_POS(TASK_NETWORK, 23)

                    _lwevent_set(&sys_event, event_net_disconnect);
					netconn_close(newconn);
					netconn_delete(newconn);

	            	TASKDEBUG_POS(TASK_NETWORK, 24)

            	}

               	TASKDEBUG_OUT(TASK_NETWORK)

            }
        }
        else if(err == ERR_TIMEOUT)
        {// Haven't received any information in TIMEOUT, check if it's because the network has disconnected.
//        	if( fsl_netif0.dhcp && (fsl_netif0.dhcp->autoip_coop_state == DHCP_AUTOIP_COOP_STATE_OFF))
//        		dhcp_network_changed(&fsl_netif0);
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
#endif // 0

void network_UseStaticIP(uint32_t * ipaddr, uint32_t * gateway, uint32_t * mask)
{
#if 0
	LWIP_DEBUGF(PING_DEBUG,("\r\nTCP/IP initializing..."));
	tcpip_init(NULL,NULL);
	LWIP_DEBUGF(PING_DEBUG,("\nTCP/IP initialized.\r\n"));
	printf("IPv4 Address: %d.%d.%d.%d\r\n", IPBYTES(((ip_addr_t*)ipaddr)->addr));

	netif_add(&fsl_netif0, (ip_addr_t*)ipaddr, (ip_addr_t*)mask, (ip_addr_t*)gateway, NULL, ethernetif_init, tcpip_input);
	netif_set_default(&fsl_netif0);

	netif_set_up(&fsl_netif0);
#endif // 0
	sys_thread_new("tcp_mandolin_thread", tcp_mandolin_thread, NULL, TCP_MANDOLIN_STACK_SIZE, TCP_MANDOLIN_THREAD_PRIORITY);
}

void network_UseDHCP(void)
{
#if 0 // Handled in TcpThread::Run()
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
	sys_thread_new("tcp_mandolin_thread", tcp_mandolin_thread, NULL, TCP_MANDOLIN_STACK_SIZE, TCP_MANDOLIN_THREAD_PRIORITY);

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
