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
#include <cstring>
#include "network.h"
#include "lwip/apps/mdns.h"

// SDK Included Files
extern "C"
{
 #include "lwip/opt.h"
 #include "lwip/api.h"
 #include "lwip/netif.h"
 #include "otpFlash.h"
}
#include "board.h"
#include "ConfigManager.h"

#include "os_tasks.h"
#include "TcpThread.h"
#include "Region.h"


mandolin_fifo	netRxFifo;
uint8_t			__attribute__ ((section(".external_ram"))) netRxFifo_bytes[NETWORK_MANDOLIN_FIFO_SIZE];
uint32_t		g_CurrentIpAddress;
_queue_id		tcp_qid;

// Define global macAddr[] which is used in LWIP low_level_init()
unsigned char macAddr[6] =
{
        OLY_DEFAULT_MAC_ADDR0,  // Match with Defaults.h definition of default MAC address
        OLY_DEFAULT_MAC_ADDR1,
        OLY_DEFAULT_MAC_ADDR2,
        OLY_DEFAULT_MAC_ADDR3,
        OLY_DEFAULT_MAC_ADDR4,
        OLY_DEFAULT_MAC_ADDR5
};

//const char mdnsName[] = "cddlive";

struct netif fsl_netif0;
NETIF_DECLARE_EXT_CALLBACK( netif_callback )

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

static void netifStatusCallback(struct netif *netif, netif_nsc_reason_t reason, const netif_ext_callback_args_t *args)
{
	LWIP_UNUSED_ARG(netif);
	LWIP_UNUSED_ARG(args);

	if (reason & LWIP_NSC_LINK_CHANGED) {
		printf("Netif link %s\n", args->link_changed.state? "up" : "down");
	}

	if (reason & (LWIP_NSC_IPV4_ADDRESS_CHANGED | LWIP_NSC_IPV4_GATEWAY_CHANGED |
			LWIP_NSC_IPV4_NETMASK_CHANGED | LWIP_NSC_IPV4_SETTINGS_CHANGED ))
	{
		printf("Netif IP change: %s\n", ip_ntoa(&(netif->ip_addr)));
		mdns_resp_netif_settings_changed(netif);
	}
}

static void srv_txt(struct mdns_service *service, void *txt_userdata)
{
    char szText[64];

	sprintf(szText, "Brand=%s", Region::GetMandolinBrandName(Region::GetSystemBrand()));
    int res = mdns_resp_add_service_txtitem(service, szText, strlen(szText));
    LWIP_ERROR("mdns add service txt failed\n", (res == ERR_OK), return );

	sprintf(szText, "Model=%s", Region::GetMandolinModelName(Region::GetSystemBrand(), Region::GetSystemModel()));
    res = mdns_resp_add_service_txtitem(service, szText, strlen(szText));
    LWIP_ERROR("mdns add service txt failed\n", (res == ERR_OK), return );

	sprintf(szText, "Revision=%d", (int)Region::GetHardwareRevision());
    res = mdns_resp_add_service_txtitem(service, szText, strlen(szText));
    LWIP_ERROR("mdns add service txt failed\n", (res == ERR_OK), return );

   	sprintf(szText, "Serial=%d", (int)Region::GetSerialNumber());
    res = mdns_resp_add_service_txtitem(service, szText, strlen(szText));
    LWIP_ERROR("mdns add service txt failed\n", (res == ERR_OK), return );

}

//-------------------------------------------------------------------------------------
// MANDOLIN Processing Thread
//-------------------------------------------------------------------------------------
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

static bool bMandolinThreadRunning = false;

// Called on startup
void network_UseStaticIP(uint32_t *ipaddr, uint32_t *gateway, uint32_t *mask)
{
    if ( !bMandolinThreadRunning )
    {
        network_init();
    	sys_thread_new("tcp_mandolin_thread", tcp_mandolin_thread, NULL, TCP_MANDOLIN_STACK_SIZE, TCP_MANDOLIN_THREAD_PRIORITY);
    	bMandolinThreadRunning = true;
	}

    printf("Stopping DHCP to go to STATIC IP\n");
    netifapi_dhcp_release_and_stop( &fsl_netif0 );

    ip4_addr_t ip, subnet, gw;
    ip.addr = *ipaddr;
    subnet.addr = *mask;
    gw.addr = *gateway;
    netifapi_netif_set_addr( &fsl_netif0, &ip, &subnet, &gw );

    mdns_resp_rename_netif(&fsl_netif0, olyConfig->GetMdnsName());
}

// Called on startup
void network_UseDHCP(void)
{
    if ( !bMandolinThreadRunning )
    {	
	    network_init();
	    sys_thread_new("tcp_mandolin_thread", tcp_mandolin_thread, NULL, TCP_MANDOLIN_STACK_SIZE, TCP_MANDOLIN_THREAD_PRIORITY);
	    bMandolinThreadRunning = true;
    }

    printf("Starting DHCP\n");
	dhcp_start(&fsl_netif0 );

    mdns_resp_rename_netif(&fsl_netif0, olyConfig->GetMdnsName());
}

void network_init(void)
{
	uint32_t 	result;

	MANDOLIN_FIFO_init(&netRxFifo,NETWORK_MANDOLIN_FIFO_SIZE, netRxFifo_bytes);
}

void networkInitialisation(ip_addr_t *pIpAddr, ip_addr_t *pGateway, ip_addr_t *pNetmask)
{
	ip_addr_t ipaddr = *pIpAddr;
    ip_addr_t netmask = *pNetmask;
    ip_addr_t gw = *pGateway;

	tcpip_init(NULL, NULL);

	// Get the MAC address from Flash in this order : OTP Flash MAC, Data Flash MAC, Default MAC
	readMACAddressFromOTP(macAddr);

	netif_add(&fsl_netif0, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input);

	netif_set_default(&fsl_netif0);
	netif_set_up(&fsl_netif0);

	// register a callback for interface changes
	netif_add_ext_callback(&netif_callback, netifStatusCallback);

	mdns_resp_init();
	//mdns_resp_add_netif(&fsl_netif0, mdnsName);
	mdns_resp_add_netif(&fsl_netif0, olyConfig->GetMdnsName());
	mdns_resp_add_service(&fsl_netif0, olyConfig->GetDiscoServiceName(), "_mandolin", DNSSD_PROTO_TCP, 50001, srv_txt, NULL);
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


void setNetworkLinkDown()
{
	printf( "Set Link DOWN\n");
    netif_set_link_down( &fsl_netif0 );
}

void setNetworkLinkUp()
{
	printf( "Set Link UP\n");
    netif_set_link_up( &fsl_netif0 );
}
