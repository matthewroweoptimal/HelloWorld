/*
 * UltimoPort.cpp
 *
 *  Created on: Oct 10, 2013
 *      Author: Ted.Wolfe
 */

#include "UltimoPort.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

extern "C" {

#include <oly_logo.h>
#include <uhip.h>
#include <uhip_structures.h>
#include <uhip_hostcpu_rx_timer.h>
#include <frame.h>
#include <rx_uhip.h>
//#include "fsl_uart_hal.h"
//#include "fsl_uart_edma_driver.h"
//#include "fsl_edma_driver.h"
//#include "fsl_uart_driver_modified.h"
//#include "fsl_uart_driver_modified.h"
#include "rx_ddp.h"
//#include "SpeakerConfiguration.h"
#include "uart_ultimo.h"
#if USE_OLY_UI
#include "LcdCoordinates.h"
#endif
}

#include "SpeakerConfiguration.h"	// Now .cpp file

extern uint8_t cobs_rx_buffer[];
#ifdef DEBUG
extern uint16_t rx_packet_size_bytes;
extern uint32_t g_ultimo_rx_overflow;
extern uint32_t g_irA_rx_overflow;
extern uint32_t g_irB_rx_overflow;
extern uint32_t g_irC_rx_overflow;
extern uint32_t g_irD_rx_overflow;
#endif	//	DEBUG

/* circular buffer from the UART */
extern uint8_t __attribute__ ((section(".external_ram"))) g_u8RecData[ULT_RX_BUF_SIZE];
extern volatile uint32_t g_u32comRbytes;
extern volatile uint32_t g_u32comRhead;
extern volatile uint32_t g_u32comRtail;
extern volatile int32_t g_bWait;


extern "C" {
	int hostcpu_uhip_Initialize(int nfdDriver);
	void hostcpu_transport_cleanup();
	void hostcpu_uhip_Check();
	unsigned char hostcpu_dev_gen_query();
	unsigned char hostcpu_dev_manuf_info();
	unsigned char hostcpu_dev_id(char *pszNewFriendlyName);
	unsigned char hostcpu_network_status();
	unsigned char hostcpu_channel_status();
	unsigned int aud_configure_dhcp();
	unsigned int aud_reboot_ultimo();
	void check_uhip_timers(void);
	void handle_uhip_tx(void);
	unsigned char process_cobs_rx_packet(uint8_t* rx_pkt_buffer, uint16_t rx_buff_size);

}


UltimoPort::UltimoPort(void * olyConfig)
{
	m_bCapVersionValid = false;
	m_bOsVersionValid = false;
	m_bDevIdValid = false;
	m_bLinkInfoValid = false;
	m_bChannelStatusValid = false;
	m_nLastCheckVersion = 0;
	m_nLastCheckReady = 0;
	m_bRefreshedStatusAfterBoot = 0;
	m_bDante_ready = -1;
	memset(&m_osVersion, 0, sizeof(m_osVersion));
	memset(&m_capVersion, 0, sizeof(m_capVersion));
	memset(m_szModelName, 0, sizeof(m_szModelName));
	memset(&m_modelId, 0, sizeof(m_modelId));
	memset(m_szFriendlyName, 0, sizeof(m_szFriendlyName));
	memset(&m_devId, 0, sizeof(m_devId));
	memset(&m_macDante, 0, sizeof(m_macDante));
	m_ipAddressDante = 0;
	m_uiLinkSpeed = 0;
	m_usLinkFlags = 0;
	m_usChannelStatus = 0;
	m_usChannelAvail = 0;
	m_usChannelActive = 0;
	m_usChannelMute = 1;
	m_bIpResetComplete = false;
	m_bChangeFriendlyName = false;
	m_bInitialized = false;
	m_bUltimoConfigIPAddressValid = false;
	m_UltimoConfigIPAddress = 0;
	m_UltimoConfigIPGateway = 0;
	m_UltimoConfigIPMask = 0;
	pConfig= (Config*)olyConfig;
}

UltimoPort::~UltimoPort()
{
}

bool UltimoPort::Initialize(int nPort)
{
	//changed from lwmsgq implementation of NXP to circular buffer added to by the interrupt call back and checked in UlitmoPort::Run. Is there and issue with MUTEX etc?

	m_nPort = nPort;

	hostcpu_uhip_Initialize(m_nPort);

	//	For debug only, you can enable this line and put your own name in.
	//	DON'T CHECK IN THIS CHANGE!  Or, everyone will have units with the same name.
	//	SetFriendlyName("<YOURNAME>'s Oly");	//	For test

	m_bInitialized = true;

	return(true);
}

void UltimoPort::Run(int nMSec)
{
#ifdef DEBUG
	static uint32_t s_uiUltimoLastErrCnt = 0;
	static uint32_t s_uiIrALastErrCnt = 0;
	static uint32_t s_uiIrBLastErrCnt = 0;
	static uint32_t s_uiIrCLastErrCnt = 0;
	static uint32_t s_uiIrDLastErrCnt = 0;

	if (g_ultimo_rx_overflow!=s_uiUltimoLastErrCnt)
	{
		printf("ULTIMO Serial port overflow count now %d! - Invalidating channel status\n", g_ultimo_rx_overflow);
		s_uiUltimoLastErrCnt = g_ultimo_rx_overflow;
		m_bChannelStatusValid = false;
	}
	if (g_irA_rx_overflow!=s_uiIrALastErrCnt)
	{
		printf("IRA Serial port overflow count now %d!\n", g_irA_rx_overflow);
		s_uiIrALastErrCnt = g_irA_rx_overflow;
	}
	if (g_irB_rx_overflow!=s_uiIrBLastErrCnt)
	{
		printf("IRB Serial port overflow count now %d!\n", g_irB_rx_overflow);
		s_uiIrBLastErrCnt = g_irB_rx_overflow;
	}
	if (g_irC_rx_overflow!=s_uiIrBLastErrCnt)
	{
		printf("IRC Serial port overflow count now %d!\n", g_irC_rx_overflow);
		s_uiIrCLastErrCnt = g_irC_rx_overflow;
	}
	if (g_irD_rx_overflow!=s_uiIrBLastErrCnt)
	{
		printf("IRD Serial port overflow count now %d!\n", g_irD_rx_overflow);
		s_uiIrDLastErrCnt = g_irD_rx_overflow;
	}
#endif	//	DEBUG

	//check the uhip rx and tx timers
	check_uhip_timers();

	//send any uhip packet
	handle_uhip_tx();

	//process any received uhip packets
	HandleRx();

	//	For reseting codec when Dante Clock changes or reboot
//	if (IsDSPStarted()) {
//		if (handle_ddp_GetRebootState())
//			SendCodecResetToSharc(true);
//		if (handle_ddp_GetAudioFormatChangeState())
//			SendCodecResetToSharc(false);
//	}

	Check(nMSec);
}

/**
 * Handles any pending RX messages
 */
void UltimoPort::HandleRx()
{
	uint8_t tempRxBuffer[UHIP_CHUNK_SIZE];
	uint16_t bytesRead = 0;
	aud_error_t result = AUD_SUCCESS;
	size_t extractedSize;
	size_t i;
	bool bValid;

	//read bytes received from the transport layer one UHIP_CHUNK_SIZE at a time (IQ - UHIP_CHUNK_SIZE is the max this function will return)
	bytesRead = DoRead(tempRxBuffer, UHIP_CHUNK_SIZE, &bValid);
	if ((bytesRead==0) && (!bValid))
	{
		reset_uhip_rx_buffers();
		reset_cobs_state();
	}
#ifdef DANTE_DEBUG_FINE
	if (bytesRead)
		printf("UHIP:Read x%x bytes:", (unsigned int)bytesRead);
#endif	//	DANTE_DEBUG_FINE
	while (bytesRead != 0)
	{
		for (i = 0; i < bytesRead; ++i)
		{
#ifdef DANTE_DEBUG_FINE
			printf("%02X,", (unsigned int)tempRxBuffer[i]);
#endif	//	DANTE_DEBUG_FINE
			result = extract_cobs_from_rx_frame(tempRxBuffer[i], cobs_rx_buffer, &extractedSize, uhip_hostcpu_rx_timer_start, uhip_hostcpu_rx_timer_stop, RX_TIMEOUT_MILLISECONDS);

			// see if there is full packet (try to process) what we received so far but if there is an error you want to reset
			if (result == AUD_ERR_NOMEMORY)
			{
				printf("\nUHIP:AUD_ERR_NOMEMORY\n");
				hostcpu_uhip_increment_num_rx_overflows();
				reset_uhip_rx_buffers();
			
				while (bytesRead=DoRead(tempRxBuffer, UHIP_CHUNK_SIZE, &bValid)) printf("UHIP:Tossing %d bytes\n", bytesRead);
				reset_cobs_state();
				break;	//	Throw away rest of buffer
			}
			else if (result == AUD_SUCCESS)
			{
#ifdef DANTE_DEBUG_FINE
				printf("\nUHIP:AUD_SUCCESS\n");
#endif	//	DANTE_DEBUG_FINE
				//process the rx packet
				if (!process_cobs_rx_packet(cobs_rx_buffer, extractedSize))
				{
					while (bytesRead=DoRead(tempRxBuffer, UHIP_CHUNK_SIZE, &bValid)) printf("UHIP:Tossing %d bytes\n", bytesRead);
					reset_cobs_state();
					break;	//	Throw away rest of buffer
				}
			}

			if (!bValid)
			{
				reset_uhip_rx_buffers();
				reset_cobs_state();
				break;
			}

		}
#ifdef DANTE_DEBUG_FINE
		printf("\n");
#endif	//	DANTE_DEBUG_FINE

		//get the next chunk (IQ??)
		bytesRead = DoRead(tempRxBuffer, UHIP_CHUNK_SIZE, &bValid);
		if ((bytesRead==0) && (!bValid))
		{
			reset_uhip_rx_buffers();
			reset_cobs_state();
		}

#ifdef DANTE_DEBUG_FINE
		if (bytesRead)
			printf("UHIP:Reread x%x bytes:", (unsigned int)bytesRead);
#endif	//	DANTE_DEBUG_FINE
	}
//	if (result == AUD_ERR_INPROGRESS)
//		printf("UHIP:Invalid or incomplete COBS packet received -attempting to read more\n");
}

uint16_t UltimoPort::DoRead(uint8_t* pBuffer, uint16_t maxBytesToRead, bool* bValid)
{
	static size_t s_ReadIndex = 0;
	static size_t s_TotalRx = 0;
	static uint8_t s_RxInBuffer[ULT_RX_BUF_SIZE];
	int nBytesRead = 0;

	*bValid = true;
	size_t bytesRead;

	if (s_TotalRx == 0)
	{
		uint8_t rx_byte;

		if (-1!=m_nPort)
		{
			for(int nByte=0;nByte<ULT_RX_BUF_SIZE;nByte++)			//IQ this is 1024 bytes. a chunk is 32 bytes?
			{
				_mqx_uint uiMsg;
				_mqx_uint uiReturnStatus;

				//uiReturnStatus = _lwmsgq_receive((void *)m_ServerQueue, &uiMsg, 0, 0, 0);	//	LWMSGQ_RECEIVE_BLOCK_ON_EMPTY IQ - Nuvoton can't use this, do we add to translation layer?
				/*get something from the circular buffer. I am confused if this is blocking or not! -  just a byte I think! */
				/*for now I am implementing this a bit squiffy*/
				/* circular buffer from the UART */
			    //while(g_bWait);	//suspend() would be better. we are blocking until something arrives in the rx buffer. what if we need to tx first?
				//suspect at the least I should wait in this loop until we have collected a chunk, or timeout?

			    uint32_t tmp = g_u32comRtail;

			    if(g_u32comRhead != tmp)
			    {
			    	int8_t tempByte = g_u8RecData[g_u32comRhead];
			    	s_RxInBuffer[s_TotalRx++] = tempByte;
			    	nBytesRead++;
			    	g_u32comRhead = (g_u32comRhead == (ULT_RX_BUF_SIZE - 1)) ? 0 : (g_u32comRhead + 1);
			    	g_u32comRbytes--;
			    	uiReturnStatus == MQX_OK;
			    }
			    else
			    {
			    	break;		//we have got all that is there so far!
			    }


				/*if ((uiReturnStatus == MQX_OK) && (uiMsg & 0xff000000))				//IQ - this checks for the end of a set of recieved bytes. Will unlikely be
				{
					*bValid = false;	// process what we have already received so break out of loop but tell cob to reset after processing this stuff.
					break;
				}
				s_RxInBuffer[s_TotalRx] = (uint8_t)(uiMsg&0x0FF);
				if (uiReturnStatus == MQX_OK)
				{
					s_TotalRx++;
					nBytesRead++;
				}
				else
				{
					break;
				}*/
			}
		}
	}

#ifdef DANTE_DEBUG_FINE
	if (nBytesRead)
		printf("R%d\n", nBytesRead);
#endif	//	DANTE_DEBUG_FINE

	if (s_TotalRx == 0) {
		return 0;
	}

	if (s_TotalRx >= maxBytesToRead) {
		bytesRead = maxBytesToRead;
	}
	else {
		bytesRead = s_TotalRx;
	}

	memcpy(pBuffer, &s_RxInBuffer[s_ReadIndex], bytesRead);

	s_ReadIndex = (s_ReadIndex + bytesRead);
	s_TotalRx -= bytesRead;

	if (s_TotalRx == 0) {
		s_ReadIndex = 0;
	}

	return bytesRead;
}

// ------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------
// ------------
// CheckVersion
// This method is called in the foreground, and should be safe to handle long tasks.
// Despite the name, it handles more than just checking the Dante versions - it also handles the
// query for Dante Ready status.
void UltimoPort::Check(int nMSec)
{
	static int s_TimeoutCnt = 0;
	static int s_nTimeout = ULT_STARTUP_DELAY;
	static int s_nLinkStatusRefreshCnt = 0;
	char szModelName[64];
	dante_id64_t modelId;
	char szFriendlyName[64];
	dante_device_id_t devId;
	unsigned short usLinkFlags;
	unsigned int uiLinkSpeed;
	unsigned long int ulIpAddressDante;
	unsigned char macDante[6];
	unsigned short usChannelStatus;
	unsigned char usChannelAvail;
	unsigned char usChannelActive;
	unsigned char usChannelMute;
	uint32_t danteConfigIpAddress;
	uint32_t danteConfigIpGateway;
	uint32_t danteConfigIpMask;

	unsigned int uiDevStatus;
	unsigned short usIdentityStatus;

	if (s_TimeoutCnt > s_nTimeout) {
		if (!m_bRefreshedStatusAfterBoot) {
			m_bChannelStatusValid = false;	// Request channel status once after boot timeout
			m_bLinkInfoValid = false;	// Same with link, non-sync can occur after firmware update
			m_bRefreshedStatusAfterBoot = true;
		}
	} else {
		s_TimeoutCnt += nMSec;
	}

	ULTIMO_VERSION os;
	if (handle_ddp_GetOsVersion(&os.v1, &os.v2, &os.v3, &os.bld, &uiDevStatus))
	{
#ifdef DANTE_DEBUG_FINE
		printf("handle_ddp_GetOsVersion\n");
#endif	//	DANTE_DEBUG_FINE
		m_bOsVersionValid = true;

		/* Show HW Error Screen if the Dante OS Version comes back as 0s */
		if (os.v1 == 0 &&
			os.v2 == 0 &&
			os.v3 == 0 &&
			os.bld == 0)
		{
#if USE_OLY_UI
			pConfig->olyUI.ShowHWErrorScreen(HW_ERROR_ULTIMO);
#endif // USE_OLY_UI
		}

		if ( 	(m_osVersion.v1 != os.v1) ||
				(m_osVersion.v2 != os.v2) ||
				(m_osVersion.v3 != os.v3) ||
				(m_osVersion.bld != os.bld) )
		{
			m_osVersion = os;
			//	Kris: Notify any one who needs to know OS version changed
			pConfig->OnDanteChange_OSVersion((uint8_t*)&m_osVersion);
			printf("Dante OS Version: %X.%X.%X.%X\n", os.v1, os.v2, os.v3, os.bld);
		}

		if (uiDevStatus)
		{
			printf("Device Status Error 0x%X\n!", uiDevStatus);
			//	Kris: Notify any one who needs to know there was a device status error
			pConfig->OnDanteChange_DeviceStatus(uiDevStatus);
			//	Possible sources are: 
			//	STATUS_FLAG_CORRUPT_CAPABILITY = 0x00000001, /*!< Device has a corrupt capability */
			//	STATUS_FLAG_CORRUPT_USER_PARTITION = 0x00000002, /*!< Device has a corrupt user partition */
			//	STATUS_FLAG_CONFIG_STORE_ERROR = 0x00000004, /*!< Device has an error storing the Dante configuration */
		}
	}

	if (!m_bOsVersionValid)
	{
		if (s_TimeoutCnt>s_nTimeout)
		{
			s_TimeoutCnt = 0;
			s_nTimeout = ULT_CMD_TIMEOUT;
			hostcpu_dev_gen_query();
		}
	}

	ULTIMO_VERSION cap;
	if (handle_ddp_GetCapVersion(&cap.v1, &cap.v2, &cap.v3, &cap.bld, szModelName, sizeof(szModelName), &modelId))
	{
#ifdef DANTE_DEBUG_FINE
		printf("handle_ddp_GetCapVersion\n");
#endif	//	DANTE_DEBUG_FINE
		m_bCapVersionValid = true;
		if ( 	(m_capVersion.v1 != cap.v1) ||
				(m_capVersion.v2 != cap.v2) ||
				(m_capVersion.v3 != cap.v3) ||
				(m_capVersion.bld != cap.bld) )
		{
			m_capVersion = cap;
			//	Kris: Notify any one who needs to know capabilities version changed
			pConfig->OnDanteChange_CapVersion((uint8_t*)&m_capVersion);
		}

		if (strcmp(m_szModelName, szModelName))
		{
			mystrcpy(m_szModelName, sizeof(m_szModelName), szModelName);
			//	Kris: Notify any one who needs to know model changed
			pConfig->OnDanteChange_ModelName((char*)m_szModelName);
		}
		if (memcmp(&m_modelId.data[0], &modelId.data[0], sizeof(m_modelId)))
		{
			m_modelId = modelId;
			//	Kris: Notify any one who needs to know model Id changed
			pConfig->OnDanteChange_ModelID((uint8_t*)&m_modelId);
		}
	}

	if (!m_bCapVersionValid)
	{
		if (s_TimeoutCnt>s_nTimeout)
		{
			s_TimeoutCnt = 0;
			s_nTimeout = ULT_CMD_TIMEOUT;
			hostcpu_dev_manuf_info();
		}
	}

	if (handle_ddp_GetDevId(szFriendlyName, sizeof(szFriendlyName), &devId, &usIdentityStatus))
	{
#ifdef DANTE_DEBUG_FINE
		printf("handle_ddp_GetDevId\n");
#endif	//	DANTE_DEBUG_FINE
		m_bDevIdValid = true;
		if (strcmp(m_szFriendlyName, szFriendlyName))
		{
			printf("Dante name changed from %s to %s\n", m_szFriendlyName, szFriendlyName);
			mystrcpy(m_szFriendlyName, sizeof(m_szFriendlyName), szFriendlyName);
			//	Kris: Notify any one who needs to know Friendly name changed
			pConfig->OnDanteChange_FriendlyName((char*)m_szFriendlyName);
		}
		if (memcmp(&m_devId.data[0], &devId.data[0], sizeof(m_devId)))
		{
			m_devId = devId;
			//	Kris: Notify any one who needs to know device Id changed
			pConfig->OnDanteChange_DeviceID((uint8_t*)&m_devId);
		}
		if (uiDevStatus)
		{
			printf("Identity Status Error 0x%X\n!", usIdentityStatus);
			//	Kris: Notify any one who needs to know there was an identity status error
			pConfig->OnDanteChange_IdentityStatus(usIdentityStatus);
			//	Possible sources are: 
			//	IDENTITY_STATUS_NAME_CONFLICT = 0x0001, /*!< The friendly name is conflicting with another device */
		}
	}

	if (!m_bDevIdValid)
	{
		if (s_TimeoutCnt>s_nTimeout)
		{
			s_TimeoutCnt = 0;
			s_nTimeout = ULT_CMD_TIMEOUT;
			hostcpu_dev_id(0);
		}
	}

	if (m_bOsVersionValid && m_bCapVersionValid && m_bDevIdValid)
	{
		//	If there is a new Friendly name and the current Friendly name has already been synced
		if (m_bChangeFriendlyName && m_bDevIdValid)
		{
			//	Does new model name and current model name match?
			if (!strcmp(m_szNewFriendlyName, m_szFriendlyName))
				m_bChangeFriendlyName = false;

			if (m_bChangeFriendlyName)
			{
				hostcpu_dev_id(m_szNewFriendlyName);
				m_bChangeFriendlyName = false;
			}
		}
	}

	if (handle_ddp_Identfy())
	{
		//	Kris: Notify any one who needs to know Identfy has been remotely triggered
		printf("Remote Identify Triggered.\n");
		pConfig->IdentifyStart(5000, eIDENTIFY_MODE_IDENTIFY);
	}

	if (handle_ddp_GetLinkInfo(&usLinkFlags, &uiLinkSpeed, macDante, &ulIpAddressDante))
	{
#ifdef DANTE_DEBUG_FINE
		printf("handle_ddp_GetLinkInfo\n");
#endif	//	DANTE_DEBUG_FINE
		m_bLinkInfoValid = true;
		if ((usLinkFlags&1)!=(m_usLinkFlags&1))
		{
			printf("Dante link status changed from %d to %d\n", m_usLinkFlags&1, usLinkFlags&1);
			m_usLinkFlags = usLinkFlags;
			//	Kris: Notify any one who needs to know Link Status changed
			pConfig->OnDanteChange_LinkFlags(m_usLinkFlags);
		}
		if (uiLinkSpeed!=m_uiLinkSpeed)
		{
			printf("Dante link speed changed from %d to %d\n", m_uiLinkSpeed, uiLinkSpeed);
			m_uiLinkSpeed = uiLinkSpeed;
			//	Kris: Notify any one who needs to know Link Speed  changed
			pConfig->OnDanteChange_LinkSpeed(m_uiLinkSpeed);
		}
		if (memcmp(macDante, m_macDante, sizeof(m_macDante)))
		{
			printf("Dante mac changed %02X:%02X:%02X:%02X:%02X:%02X to %02X:%02X:%02X:%02X:%02X:%02X\n",
					m_macDante[0], m_macDante[1],	m_macDante[2], m_macDante[3], m_macDante[4], m_macDante[5],
					macDante[0], macDante[1],	macDante[2], macDante[3], macDante[4], macDante[5]);
			memcpy(m_macDante, macDante, sizeof(m_macDante));
			//	Kris: Notify any one who needs to know Dante MAC changed
			pConfig->OnDanteChange_MacAddress(m_macDante);
		}
		if(m_ipAddressDante != ulIpAddressDante)
		{
			printf("Dante IP Address: %d.%d.%d.%d\n", (ulIpAddressDante>>24)&0x0ff, (ulIpAddressDante>>16)&0x0ff, (ulIpAddressDante>>8)&0x0ff, ulIpAddressDante&0x0ff);
			m_ipAddressDante = ulIpAddressDante;
			if(pConfig)
				pConfig->OnDanteChange_IpAddress(m_ipAddressDante);
		}
	}

	if(handle_ddp_GetNetworkConfig( &danteConfigIpAddress, &danteConfigIpGateway, &danteConfigIpMask))
	{

		if(m_UltimoConfigIPAddress	!= danteConfigIpAddress)
			m_UltimoConfigIPAddress = danteConfigIpAddress;

		if(m_UltimoConfigIPGateway	!= danteConfigIpGateway)
			m_UltimoConfigIPGateway = danteConfigIpGateway;

		if(m_UltimoConfigIPMask != danteConfigIpMask)
			m_UltimoConfigIPMask = danteConfigIpMask;

		m_bUltimoConfigIPAddressValid = true;

		if(pConfig)
			pConfig->OnDanteChange_ConfigIpAddress(m_UltimoConfigIPAddress, m_UltimoConfigIPGateway, m_UltimoConfigIPMask);
	}

	if (!m_bLinkInfoValid)
	{
		if (s_TimeoutCnt>s_nTimeout)
		{
			s_TimeoutCnt = 0;
			s_nTimeout = ULT_CMD_TIMEOUT;
			hostcpu_network_status();
		}
	}
	else if (1)	//	0==(m_usLinkFlags&1))	//	Poll link state whether connected or not, because we may have missed it.  We used to poll only if unconnected.
	{
		//	Make sure you don't miss link becoming active - poll a t aslow rate
		if (s_nLinkStatusRefreshCnt>ULT_STATUS_REFRESH_TIMEOUT)
		{
			s_nLinkStatusRefreshCnt = 0;
			m_bLinkInfoValid = false;
		}
		else
			s_nLinkStatusRefreshCnt += nMSec;
	}


	if (handle_ddp_GetChannelStatus(&usChannelStatus, &usChannelAvail, &usChannelActive, &usChannelMute))
	{
#ifdef DANTE_DEBUG_FINE
		printf("handle_ddp_GetChannelStatus\n");
#endif	//	DANTE_DEBUG_FINE
		m_bChannelStatusValid = true;

#if 0   // use channel status only
		if (usChannelStatus!=m_usChannelStatus)
		{
			printf("Dante input channel status changed from %d to %d\n", m_usChannelStatus, usChannelStatus);
			m_usChannelStatus = usChannelStatus;
			AutoSelectAudioSource( usChannelStatus );
			//	Kris: Notify any one who needs to know Channel Status changed
			pConfig->OnDanteChange_ChannelStatus(m_usChannelStatus);
		}
#else	// Use all status parameters
		if ((usChannelStatus!=m_usChannelStatus) || (usChannelAvail!=m_usChannelAvail) || (usChannelActive!=m_usChannelActive) || (usChannelMute!=m_usChannelMute))
		{
			printf("Dante input channel Status ");
			if(m_usChannelStatus != usChannelStatus)
				printf("to");
			else
				printf("  ");

			printf(" %d   Avail ", usChannelStatus);
			if(m_usChannelAvail != usChannelAvail)
				printf("to");
			else
				printf("  ");

			printf(" %d   Active ", usChannelAvail);
			if(m_usChannelActive != usChannelActive)
				printf("to");
			else
				printf("  ");

			printf(" %d   Mute ", usChannelActive);
			if(m_usChannelMute != usChannelMute)
				printf("to");
			else
				printf("  ");
			printf(" %d\n", usChannelMute);

			m_usChannelStatus = usChannelStatus;
			m_usChannelActive = usChannelActive;
			m_usChannelAvail = usChannelAvail;
			m_usChannelMute = usChannelMute;

#if USE_BACKPANEL_DEBUG && USE_DEBUG_ULTIMO
			{
				char tempString[32] = {0};
				sprintf(tempString, "s%02x c%02x v%02x m%02x", m_usChannelStatus, m_usChannelActive, m_usChannelAvail, m_usChannelMute);
				pConfig->olyUI.DebugLineOut(0, tempString);
			}
#endif

			if (usChannelActive && usChannelAvail)
			{
				AutoSelectAudioSource( usChannelStatus ); // could be Dante TODO reimplement
				pConfig->OnDanteChange_ChannelStatus(m_usChannelStatus);

			}
			else
			{
				AutoSelectAudioSource( 0 );		// can not be Dante TODO reimplement
				pConfig->OnDanteChange_ChannelStatus(0);

			}
		//	Kris: Notify any one who needs to know Channel Status changed

			//	From my experimenting DDP_RX_CHAN_STATUS_DYNAMIC or DDP_RX_CHAN_STATUS_STATIC indicates connected, everything else, not
			//	But here are all values for convienence
			//
			//	DDP_RX_CHAN_STATUS_NONE = 0,	/*!< Channel is not subscribed or otherwise doing anything interesting */
			//	DDP_RX_CHAN_STATUS_UNRESOLVED = 1,	/*!< Name not yet found */
			//	DDP_RX_CHAN_STATUS_RESOLVED = 2,	/*!< Name has been found, but not yet processed. This is an transient state */
			//	DDP_RX_CHAN_STATUS_RESOLVE_FAIL = 3,	/*!< Error: an error occurred while trying to resolve name */
			//	DDP_RX_CHAN_STATUS_SUBSCRIBE_SELF = 4,	/*!< Channel is successfully subscribed to own TX channels (local loopback mode) */
			//	DDP_RX_CHAN_STATUS_IDLE = 7,	/*!< A flow has been configured but does not have sufficient information to establish an audio connection. For example, configuring a template with no associations. */
			//	DDP_RX_CHAN_STATUS_IN_PROGRESS = 8,	/*!< Name has been found and processed; setting up flow. This is an transient state */
			//	DDP_RX_CHAN_STATUS_DYNAMIC = 9,	/*!< Active subscription to an automatically configured source flow */
			//	DDP_RX_CHAN_STATUS_STATIC = 10,	/*!< Active subscription to a manually configured source flow */

			//	DDP_RX_CHAN_STATUS_MANUAL = 14,	/*!< Manual flow configuration bypassing the standard subscription process */
			//	DDP_RX_CHAN_STATUS_NO_CONNECTION = 15,	/*!< Error: The name was found but the connection process failed (the receiver could not communicate with the transmitter) */
			//	DDP_RX_CHAN_STATUS_CHANNEL_FORMAT = 16,	/*!< Error: Channel formats do not match */
			//	DDP_RX_CHAN_STATUS_BUNDLE_FORMAT = 17,	/*!< Error: Flow formats do not match, e.g. Multicast flow with more slots than receiving device can handle */
			//	DDP_RX_CHAN_STATUS_NO_RX = 18,	/*!< Error: Receiver is out of resources (e.g. flows) */
			//	DDP_RX_CHAN_STATUS_RX_FAIL = 19,	/*!< Error: Receiver couldn't set up the flow */
			//	DDP_RX_CHAN_STATUS_NO_TX = 20,	/*!< Error: Transmitter is out of resources (e.g. flows) */
			//	DDP_RX_CHAN_STATUS_TX_FAIL = 21,	/*!< Error: Transmitter couldn't set up the flow */
			//	DDP_RX_CHAN_STATUS_QOS_FAIL_RX = 22,	/*!< Error: Receiver got a QoS failure (too much data) when setting up the flow */
			//	DDP_RX_CHAN_STATUS_QOS_FAIL_TX = 23,	/*!< Error: Transmitter got a QoS failure (too much data) when setting up the flow */
			//	DDP_RX_CHAN_STATUS_TX_REJECTED_ADDR = 24,	/*!< Error: TX rejected the address given by rx (usually indicates an arp failure) */
			//	DDP_RX_CHAN_STATUS_INVALID_MSG = 25,	/*!< Error: Transmitter rejected the flow request as invalid */
			//	DDP_RX_CHAN_STATUS_CHANNEL_LATENCY = 26,	/*!< Error: TX channel latency higher than maximum supported RX latency */
			//	DDP_RX_CHAN_STATUS_CLOCK_DOMAIN = 27,	/*!< Error: TX and RX and in different clock subdomains */
			//	DDP_RX_CHAN_STATUS_UNSUPPORTED = 28,	/*!< Error: Attempt to use an unsupported feature */
			//	DDP_RX_CHAN_STATUS_RX_LINK_DOWN = 29,	/*!< Error: All rx links are down */
			//	DDP_RX_CHAN_STATUS_TX_LINK_DOWN = 30,	/*!< Error: All tx links are down */
			//	DDP_RX_CHAN_STATUS_DYNAMIC_PROTOCOL = 31,	/*!< Error: can't find suitable protocol for dynamic connection */
			//	DDP_RX_CHAN_STATUS_INVALID_CHANNEL = 32,	/*!< Channel does not exist (eg no such local channel) */
			//	DDP_RX_CHAN_STATUS_TX_SCHEDULER_FAILURE = 33,	/*!< Tx Scheduler failure */

			//	DDP_RX_CHAN_STATUS_TEMPLATE_MISMATCH_DEVICE = 64,	/*!< Template-based subscription failed: template and subscription device names don't match */
			//	DDP_RX_CHAN_STATUS_TEMPLATE_MISMATCH_FORMAT = 65,	/*!< Template-based subscription failed: flow and channel formats don't match */
			//	DDP_RX_CHAN_STATUS_TEMPLATE_MISSING_CHANNEL = 66,	/*!< Template-based subscription failed: the channel is not part of the given multicast flow */
			//	DDP_RX_CHAN_STATUS_TEMPLATE_MISMATCH_CONFIG = 67,	/*!< Template-based subscription failed: something else about the template configuration made it impossible to complete the subscription using the given flow */
			//	DDP_RX_CHAN_STATUS_TEMPLATE_FULL = 68,	/*!< Template-based subscription failed: the unicast template is full */

			//	DDP_RX_CHAN_STATUS_SYSTEM_FAIL = 255	/*!< Unexpected system failure. */
		}
#endif
	}

	if (dante_comm_error_detect())
	{
		printf("UHIP:re-request channel status!\n");
		m_bChannelStatusValid = false;
	}

	if (!m_bChannelStatusValid)
	{
		if (s_TimeoutCnt>s_nTimeout)
		{
			s_TimeoutCnt = 0;
			s_nTimeout = ULT_CMD_TIMEOUT;
			hostcpu_channel_status();
		}
	}

	if (handle_ddp_GetRebootState())
	{
		printf("Ultimo chip is about to reboot.\n");
		//	Force re-syncing of all information
		m_bChannelStatusValid = false;
		m_bOsVersionValid = false;
		m_bCapVersionValid = false;
		m_bDevIdValid = false;
		m_bLinkInfoValid = false;
		m_bChannelStatusValid = false;
		s_TimeoutCnt = 0;
		s_nTimeout = ULT_STARTUP_DELAY;		//	Wait until Ultimo has a chance to reboot before starting the questions.

		//	Kris: Notify any one who needs to know Ultimo is about to reboot
		pConfig->OnDanteChange_Reboot();
	}

	if (handle_ddp_GetCodecResetRequired())
	{
		printf("CODEC reset is required.\n");
		//	Kris: Notify any one who needs to know CODEC needs to be reset.
		pConfig->OnDanteChange_CodecReset();
		//	This is triggered by an audio format change or pull up change currently.
		//	If other things need to trigger a CODEC reset, the file rx_ddp.c should be modified
		//		for all notification handlers that should cause a CODEC reset to set the g_bCodecResetRequired variable
	}

	if (handle_ddp_GetUpgradeImminent())
	{
		printf("Ultimo is about to be upgraded.\n");
		//	Kris: Notify any one who needs to know Ultimo is about to be upgraded.
		pConfig->OnDanteChange_Upgrade();
		//	THis is based on an Ultimo notification received that an upgrade file is being received.
	}
}

bool UltimoPort::GetOsVersion(uint32 *pVersion)
{
	if (pVersion)
	{
		*(pVersion) = m_bOsVersionValid?((m_osVersion.v1<<24) | (m_osVersion.v2<<16) | m_osVersion.v3):0;
	}
	return(m_bOsVersionValid);
}

bool UltimoPort::GetCapVersion(uint32 *pVersion, uint32 *pBuild)
{
	if (pVersion)
	{
		*(pVersion) = m_bCapVersionValid?((m_capVersion.v1<<24) | (m_capVersion.v2<<16) | m_capVersion.v3):0;
		*(pBuild) = m_bCapVersionValid?(m_capVersion.bld):0;
	}
	return(m_bCapVersionValid);
}

bool UltimoPort::GetModelName(char *pModelName, int nMaxSize)
{
	if (pModelName && nMaxSize)
	{
		if (m_bCapVersionValid)
			mystrcpy(pModelName, nMaxSize, m_szModelName);
		else
			memset(pModelName, 0, nMaxSize);
	}
	return(m_bCapVersionValid);
}

bool UltimoPort::GetModelId(dante_id64_t *pModelId)
{
	if (pModelId)
	{
		if (m_bCapVersionValid)
			*(pModelId) = m_modelId;
		else
			memset(pModelId, 0, sizeof(dante_id64_t));
	}
	return(m_bCapVersionValid);
}

bool UltimoPort::GetFriendlyName(char *pFriendlyName, int nMaxSize)
{
	if (pFriendlyName && nMaxSize)
	{
		if (m_bDevIdValid)
			mystrcpy(pFriendlyName, nMaxSize, m_szFriendlyName);
		else
			memset(pFriendlyName, 0, nMaxSize);
	}
	return(m_bDevIdValid);
}

void UltimoPort::SetFriendlyName(const char *pFriendlyName)
{
	mystrcpy(m_szNewFriendlyName, sizeof(m_szNewFriendlyName), pFriendlyName);
	m_bChangeFriendlyName = true;
}

bool UltimoPort::GetDevId(dante_device_id_t *pDevId)
{
	if (pDevId)
	{
		if (m_bDevIdValid)
			*(pDevId) = m_devId;
		else
			memset(pDevId, 0, sizeof(dante_device_id_t));
	}
	return(m_bDevIdValid);
}

bool UltimoPort::GetLinkSpeed(uint32 *pLinkSpeed)
{
	if (pLinkSpeed)
	{
		if (m_bLinkInfoValid)
		{
			if (m_usLinkFlags&1)
				*(pLinkSpeed) = m_uiLinkSpeed;
			else
				*(pLinkSpeed) = 0;
		}
		else
			*(pLinkSpeed) = 0;
	}
	return(m_bLinkInfoValid);
}

bool UltimoPort::GetLinkFlags(uint16 *pLinkFlags)
{
	if (pLinkFlags)
	{
		if (m_bLinkInfoValid)
			*(pLinkFlags) = m_usLinkFlags;
		else
			*(pLinkFlags) = 0;
	}
	return(m_bLinkInfoValid);
}

bool UltimoPort::GetDanteMac(unsigned char *pDanteMac)
{
	if (pDanteMac)
	{
		if (m_bLinkInfoValid)
			memcpy(pDanteMac, m_macDante, sizeof(m_macDante));
		else
			memset(pDanteMac, 0, sizeof(m_macDante));
	}
	return(m_bLinkInfoValid);
}

bool UltimoPort::GetChannelStatus(unsigned short *pChannelStatus)
{
	if (pChannelStatus)
	{
		if (m_bChannelStatusValid)
			*(pChannelStatus) = m_usChannelStatus;
		else
			*(pChannelStatus) = 0;
	}
	return(m_bChannelStatusValid);
}

void UltimoPort::TriggerIpReset()
{
	m_bIpResetComplete = true;
	aud_configure_dhcp();
	aud_reboot_ultimo();
}

bool UltimoPort::IsIpResetComplete()
{
	return(m_bIpResetComplete);
}

void UltimoPort::mystrcpy(char *pszDest, int nMaxSize, const char *pszSrc)
{
	if (pszDest && pszSrc)
	{
		int nLen = strlen(pszSrc);
		if( nLen > (nMaxSize-1))
			nLen = nMaxSize-1;
		memcpy(pszDest, pszSrc, nLen);
		*(pszDest+nLen) = 0;
	}
}

int UltimoPort::GetPort()
{
	return(m_nPort);
}

bool UltimoPort::Initialized()
{
	return(m_bInitialized);
}

char UltimoPort::GetDanteDHCP()
{
	if(!m_bUltimoConfigIPAddressValid)
		return -1;
	else
		return((0==m_UltimoConfigIPAddress)?1:0);
}

uint32_t UltimoPort::GetDanteConfigIPAddress()
{
	if(!m_bUltimoConfigIPAddressValid)
		return 0;
	else
		return(m_UltimoConfigIPAddress);
}

uint32_t UltimoPort::GetDanteConfigIPGateway()
{
	if(!m_bUltimoConfigIPAddressValid)
		return 0;
	else
		return(m_UltimoConfigIPGateway);
}

uint32_t UltimoPort::GetDanteConfigIPMask()
{
	if(!m_bUltimoConfigIPAddressValid)
		return 0;
	else
		return(m_UltimoConfigIPMask);
}


