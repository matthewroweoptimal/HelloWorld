/*
 * TcpThread.cpp
 *
 *  Created on: 21 Feb 2022
 *      Author: Matthew.Rowe
 */

#include "TcpThread.h"
#include "lwip/apps/mdns.h"
#include "network.h"
#include "os_tasks.h"
#include "Region.h"
#include "ConfigManager.h"
#include <cstring>

extern oly::Config *olyConfig;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MANDO_THREAD_PRIO    ( tskIDLE_PRIORITY + 2UL )
#define MANDO_THREAD_STACKSIZE  300
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


unsigned char macAddr[6] = {0x00, 0x0F, 0xF2, 0x08, 0x5B, 0xB9};
const char mdnsName[] = "cddlive";

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

TcpThread::TcpThread(uint16_t usStackDepth, UBaseType_t uxPriority, SemaphoreHandle_t& semMainThreadComplete)
     : Thread("TCP Config Thread", usStackDepth, uxPriority),
       _semMainThreadComplete(semMainThreadComplete)
{
}

void TcpThread::Run()
{
	printf("TCP thread waiting on MainThread completing...\n");
	//--- Wait until MainThread complete ---
    xSemaphoreTake(_semMainThreadComplete, portMAX_DELAY);
    xSemaphoreGive(_semMainThreadComplete);						//this feels dodgy, any thread that picks up the semaphore gives again for other threads to start.
    printf("TCP thread running\n");

	ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    IP4_ADDR(&gw, 169,254,1,1);
    IP4_ADDR(&ipaddr, 169,254,1,101);
    IP4_ADDR(&netmask, 255,255,255,0);

    tcpip_init(NULL, NULL);

    netif_add(&netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input);

    netif_set_default(&netif);
    netif_set_up(&netif);

    // register a callback for interface changes
    netif_add_ext_callback(&netif_callback, netifStatusCallback);

    mdns_resp_init();
    mdns_resp_add_netif(&netif, mdnsName);
    mdns_resp_add_service(&netif, olyConfig->GetDiscoServiceName(), "_mandolin", DNSSD_PROTO_TCP, 50001, srv_txt, NULL);

    dhcp_start(&netif);

    NVIC_SetPriority(EMAC_TX_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
    NVIC_EnableIRQ(EMAC_TX_IRQn);
    NVIC_SetPriority(EMAC_RX_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
    NVIC_EnableIRQ(EMAC_RX_IRQn);

//    network_UseDHCP();
	Config_Task( 0 );	// In CDD code os_tasks.cpp, this ends up calling network_UseDHCP() which spawns TCP thread
	//--- Above will process messages until terminated ---

    Suspend();
}
