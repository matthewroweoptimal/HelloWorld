/*
 * TcpThread.cpp
 *
 *  Created on: 21 Feb 2022
 *      Author: Matthew.Rowe
 */

#include "TcpThread.h"
#include "server.h"

unsigned char macAddr[6] = {0x00, 0x00, 0x00, 0x55, 0x66, 0x77};
NETIF_DECLARE_EXT_CALLBACK(netif_callback)

static void netifStatusCallback(struct netif *netif, netif_nsc_reason_t reason, const netif_ext_callback_args_t *args)
{
	LWIP_UNUSED_ARG(netif);
	LWIP_UNUSED_ARG(args);

  if (reason & LWIP_NSC_LINK_CHANGED) {
    printf("Netif link %s\n", args->link_changed.state? "up" : "down");
  }

  if (reason & (LWIP_NSC_IPV4_ADDRESS_CHANGED | LWIP_NSC_IPV4_GATEWAY_CHANGED |
      LWIP_NSC_IPV4_NETMASK_CHANGED | LWIP_NSC_IPV4_SETTINGS_CHANGED )) {

	  printf("Netif IP change: %s\n", ip_ntoa(&(netif->ip_addr)));
  }
}

TcpThread::TcpThread(uint16_t usStackDepth, UBaseType_t uxPriority) : Thread("TCP Thread", usStackDepth, uxPriority) {}

void TcpThread::Run()
{
	printf("TCP thread startup\n");

	ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    IP4_ADDR(&gw, 192,168,0,1);
    IP4_ADDR(&ipaddr, 192,168,0,2);
    IP4_ADDR(&netmask, 255,255,255,0);

    tcpip_init(NULL, NULL);

    netif_add(&netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input);

    netif_set_default(&netif);
    netif_set_up(&netif);

    // register a callback for interface changes
    netif_add_ext_callback(&netif_callback, netifStatusCallback);

    dhcp_start(&netif);

    NVIC_SetPriority(EMAC_TX_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
    NVIC_EnableIRQ(EMAC_TX_IRQn);
    NVIC_SetPriority(EMAC_RX_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
    NVIC_EnableIRQ(EMAC_RX_IRQn);

    tcp_echoserver_netconn_init();

    Suspend();
}



