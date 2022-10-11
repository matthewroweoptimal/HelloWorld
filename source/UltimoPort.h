/*
 * UltimoPort.h
 *
 *  Created on: Oct 10, 2013
 *      Author: Ted.Wolfe
 */

#pragma once

#include "LOUD_types.h"
//#include "mqx.h"
//#include "MQX_To_FreeRTOS.h"
//#include <lwmsgq.h>
#include <dante_common.h>
#include "ConfigManager.h"

#define ULT_MAX_PAYLOAD		1500
#define ULT_STARTUP_DELAY	8000	//	How long to wait after boot before trying to send a command to the Ultimo
#define ULT_CMD_TIMEOUT		1000	//	How long to wait after sending command and not getting response before trying or giving up.
#define ULT_STATUS_REFRESH_TIMEOUT	10000	//	How long to wait while link status is supposedly known but unlinked before polling to make sure we didn't miss it.
#define ULT_NUM_MESSAGES	1024
#define ULT_MSG_SIZE		1
#define ULT_RX_BUF_SIZE 	1024

typedef struct structUltimoVersion {
	unsigned char v1;
	unsigned char v2;
	unsigned short v3;
	unsigned int bld;
} ULTIMO_VERSION;

typedef enum	{
	DDP_RX_CHAN_STATUS_NONE = 0,	/*!< Channel is not subscribed or otherwise doing anything interesting */
	DDP_RX_CHAN_STATUS_UNRESOLVED = 1,	/*!< Name not yet found */
	DDP_RX_CHAN_STATUS_RESOLVED = 2,	/*!< Name has been found, but not yet processed. This is an transient state */
	DDP_RX_CHAN_STATUS_RESOLVE_FAIL = 3,	/*!< Error: an error occurred while trying to resolve name */
	DDP_RX_CHAN_STATUS_SUBSCRIBE_SELF = 4,	/*!< Channel is successfully subscribed to own TX channels (local loopback mode) */
	DDP_RX_CHAN_STATUS_IDLE = 7,	/*!< A flow has been configured but does not have sufficient information to establish an audio connection. For example, configuring a template with no associations. */
	DDP_RX_CHAN_STATUS_IN_PROGRESS = 8,	/*!< Name has been found and processed; setting up flow. This is an transient state */
	DDP_RX_CHAN_STATUS_DYNAMIC = 9,	/*!< Active subscription to an automatically configured source flow */
	DDP_RX_CHAN_STATUS_STATIC = 10,	/*!< Active subscription to a manually configured source flow */

	DDP_RX_CHAN_STATUS_MANUAL = 14,	/*!< Manual flow configuration bypassing the standard subscription process */
	DDP_RX_CHAN_STATUS_NO_CONNECTION = 15,	/*!< Error: The name was found but the connection process failed (the receiver could not communicate with the transmitter) */
	DDP_RX_CHAN_STATUS_CHANNEL_FORMAT = 16,	/*!< Error: Channel formats do not match */
	DDP_RX_CHAN_STATUS_BUNDLE_FORMAT = 17,	/*!< Error: Flow formats do not match, e.g. Multicast flow with more slots than receiving device can handle */
	DDP_RX_CHAN_STATUS_NO_RX = 18,	/*!< Error: Receiver is out of resources (e.g. flows) */
	DDP_RX_CHAN_STATUS_RX_FAIL = 19,	/*!< Error: Receiver couldn't set up the flow */
	DDP_RX_CHAN_STATUS_NO_TX = 20,	/*!< Error: Transmitter is out of resources (e.g. flows) */
	DDP_RX_CHAN_STATUS_TX_FAIL = 21,	/*!< Error: Transmitter couldn't set up the flow */
	DDP_RX_CHAN_STATUS_QOS_FAIL_RX = 22,	/*!< Error: Receiver got a QoS failure (too much data) when setting up the flow */
	DDP_RX_CHAN_STATUS_QOS_FAIL_TX = 23,	/*!< Error: Transmitter got a QoS failure (too much data) when setting up the flow */
	DDP_RX_CHAN_STATUS_TX_REJECTED_ADDR = 24,	/*!< Error: TX rejected the address given by rx (usually indicates an arp failure) */
	DDP_RX_CHAN_STATUS_INVALID_MSG = 25,	/*!< Error: Transmitter rejected the flow request as invalid */
	DDP_RX_CHAN_STATUS_CHANNEL_LATENCY = 26,	/*!< Error: TX channel latency higher than maximum supported RX latency */
	DDP_RX_CHAN_STATUS_CLOCK_DOMAIN = 27,	/*!< Error: TX and RX and in different clock subdomains */
	DDP_RX_CHAN_STATUS_UNSUPPORTED = 28,	/*!< Error: Attempt to use an unsupported feature */
	DDP_RX_CHAN_STATUS_RX_LINK_DOWN = 29,	/*!< Error: All rx links are down */
	DDP_RX_CHAN_STATUS_TX_LINK_DOWN = 30,	/*!< Error: All tx links are down */
	DDP_RX_CHAN_STATUS_DYNAMIC_PROTOCOL = 31,	/*!< Error: can't find suitable protocol for dynamic connection */
	DDP_RX_CHAN_STATUS_INVALID_CHANNEL = 32,	/*!< Channel does not exist (eg no such local channel) */
	DDP_RX_CHAN_STATUS_TX_SCHEDULER_FAILURE = 33,	/*!< Tx Scheduler failure */

	DDP_RX_CHAN_STATUS_TEMPLATE_MISMATCH_DEVICE = 64,	/*!< Template-based subscription failed: template and subscription device names don't match */
	DDP_RX_CHAN_STATUS_TEMPLATE_MISMATCH_FORMAT = 65,	/*!< Template-based subscription failed: flow and channel formats don't match */
	DDP_RX_CHAN_STATUS_TEMPLATE_MISSING_CHANNEL = 66,	/*!< Template-based subscription failed: the channel is not part of the given multicast flow */
	DDP_RX_CHAN_STATUS_TEMPLATE_MISMATCH_CONFIG = 67,	/*!< Template-based subscription failed: something else about the template configuration made it impossible to complete the subscription using the given flow */
	DDP_RX_CHAN_STATUS_TEMPLATE_FULL = 68,	/*!< Template-based subscription failed: the unicast template is full */

	DDP_RX_CHAN_STATUS_SYSTEM_FAIL = 255	/*!< Unexpected system failure. */
} ULTIMO_DDP_Chanel_Status;

using namespace oly;

class UltimoPort
{

public:
	UltimoPort(void*);
	virtual ~UltimoPort();

	bool Initialized();
	bool Initialize(int nPort);
	void Check(int nMSec);
	void Run(int nMSec);
	void TriggerIpReset();
	bool IsIpResetComplete();
	int GetPort();

	//	API for Host control
	/**
	* retrieves the current OS Version unsigned integer formatted as an 8:8:16 version number or zero if not yet valid
	* @param pFlags [out] unsigned int to receive the OS Version info
	* @return true if the data is valid
	*/
	bool GetOsVersion(uint32 *pVersion);

	/**
	* retrieves the current Capabilities Version unsigned integer formatted as an 8:8:16 version number or zero if not yet valid
	* @param pFlags [out] unsigned int to receive the Capabilities Version info
	* @return true if the data is valid
	*/
	bool GetCapVersion(uint32 *pVersion, uint32 *pBuild);

	/**
	* retrieves the current Link Speed in MHz, or zero if not yet valid
	* @param pSpeed [out] unsigned int to receive Link Speed info
	* @return true if the data is valid
	*/
	bool GetLinkSpeed(uint32 *pLinkSpeed);

	/**
	* retrieves the current Link Flags, number or zero if not yet valid, bit zero is the link up bit
	* @param pFlags [out] unsigned short to receive Link Flags info
	* @return true if the data is valid
	*/
	bool GetLinkFlags(uint16 *pLinkFlags);

	/**
	* retrieves the current Dante MAC number or zero if not yet valid
	* @param pMac [out] unsigned char[6] to receive Dante MAC
	* @return true if the data is valid
	*/
	bool GetDanteMac(unsigned char *pDanteMac);

	/**
	* retrieves the current Model Name or fills with zeros if not yet valid
	* @param pModelName [out] unsigned char array to receive Model Name
	* @param nMaxSize [in] maximum size of buffer to receive data
	* @return true if the data is valid
	*/
	bool GetModelName(char *pModelName, int nMaxSize);

	/**
	* retrieves the current Model ID Name or fills with zeros if not yet valid
	* @param pModelId [out] dante_id64_t structure to receive Model ID
	* @return true if the data is valid
	*/
	bool GetModelId(dante_id64_t *pModelId);

	/**
	* retrieves the current Friendly Name or fills with zeros if not yet valid
	* @param pFriendlyName [out] unsigned char array to receive Friendly Name
	* @param nMaxSize [in] maximum size of buffer to receive data
	* @return true if the data is valid
	*/
	bool GetFriendlyName(char *pFriendlyName, int nMaxSize);

	/**
	* retrieves the current Device ID Name or fills with zeros if not yet valid
	* @param pDevId [out] dante_device_id_t structure to receive Device ID
	* @return true if the data is valid
	*/
	bool GetDevId(dante_device_id_t *pDevId);

	/**
	* retrieves the current Channel Status or zero if not yet valid
	* @param pChannelStatus [out] unsigned short to receive Channel Status info
	* @return true if the data is valid
	*/
	bool GetChannelStatus(unsigned short *pChannelStatus);

	/**
	* requests setting of the Friendly Name
	* command will be scheduled once initial communication with Ultimo is established and current name is retrieved.
	* @param pFriendlyName [out] unsigned char array to receive Friendly Name
	*/
	void SetFriendlyName(const char *pFriendlyName);

	/**
	* retrieves the current Dante DHCP/Static IP mode
	* @param none
	* @return true 		(1) if the Ultimo is DHCP
	* 		  false 	(0) if the Ultimo is Static
	* 		  invalid 	(-1)  if the Ultimo hasn't sent an IP value yet
	*/
	char GetDanteDHCP();

	/**
	* retrieves the current Configured Dante IP Address
	* @param none
	* @return Current IP Address for the Dante. Will always return 0 if currently invalid.
	* @notes	This value is not necessarily the current Dante IP Address and is the current Configured IP Address.
	* 			For example, if the Dante is currently DHCP, this configured IP will be 0.0.0.0, which will not be the same
	* 			as the Dante's real IP address.
	*/
	uint32_t GetDanteConfigIPAddress();

	/**
	* retrieves the current Dante Gateway
	* @param none
	* @return Current Gateway for the Dante. Will always return 0 if currently invalid.
	*/
	uint32_t GetDanteConfigIPGateway();

	/**
	* retrieves the current Dante Subnet Mask
	* @param none
	* @return Subnet Mask for the Dante. Will always return 0 if currently invalid.
	*/
	uint32_t GetDanteConfigIPMask();

protected:
	void HandleRx();	
	uint16_t DoRead(uint8_t* buffer, uint16_t max_bytes_to_read, bool* bValid);
	void mystrcpy(char *pszDest, int nMaxSize, const char *pszSrc);

bool m_bIpResetComplete;

private:
	bool m_bOsVersionValid;
	bool m_bCapVersionValid;
	bool m_bDevIdValid;
	bool m_bLinkInfoValid;
	bool m_bRefreshedStatusAfterBoot;
	ULTIMO_VERSION m_osVersion;
	ULTIMO_VERSION m_capVersion;
	int m_nLastCheckVersion;
	int m_nLastCheckReady;
	char m_bDante_ready;
	int m_nPort;
	uint32 m_uiLinkSpeed;
	uint16 m_usLinkFlags;
	char m_szModelName[64];
	dante_id64_t m_modelId;
	char m_szFriendlyName[64];
	dante_device_id_t m_devId;
	bool m_bChangeFriendlyName;
	char m_szNewFriendlyName[64];
	unsigned char m_macDante[6];
	uint32 m_ipAddressDante;
	bool m_bUltimoConfigIPAddressValid;
	uint32 m_UltimoConfigIPAddress;
	uint32 m_UltimoConfigIPGateway;
	uint32 m_UltimoConfigIPMask;
	bool m_bChannelStatusValid;
	unsigned short m_usChannelStatus;
	unsigned char m_usChannelAvail;
	unsigned char m_usChannelActive;
	unsigned char m_usChannelMute;
	bool m_bInitialized;
	Config * pConfig;
};

