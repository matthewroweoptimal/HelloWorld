/*
 * Region.cpp
 *
 *  Created on: Jan 28, 2016
 *	  Author: Ted.Wolfe
 *
 *	  Management of Regions for firmware updating
 *	  Code is written for upgrade of APP firmware
 */

#include "Region.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include "MQX_To_FreeRTOS.h"
#include "network.h"
#include "TimeKeeper.h"

extern "C" {
 #include "oly.h"
 #include "LOUD_types.h"
 #include "otpFlash.h"
}

#include "flash_params.h"

// Define to 1 if we allow writing of MAC address to OTP Flash area.
// Currently, we DON'T ALLOW writing of MAC to OTP flash, so set to 0.  (We write MAC to Data Flash instead)
#define ALLOW_WRITING_MAC_TO_OTP_FLASH  0

// Definitions for the Firmware upgrade LEDs
#define ALL_LEDS_OFF    		0x00
#define ALL_LEDS_ON     		0x0F
#define NUM_OF_FW_UPDATE_LEDS	4

#define OLY_MANF_STATIC_IP			IPADDR(169,254,1,101)	// Manufacturing Static IP 169.254.1.101
#define OLY_MANF_MASK				IPADDR(255,255,0,0)		//Manufacturing Static IP Mask is : 255.255.0.0

#if 0	// EXCLUDE INITIALISATION OF 'OLY_BLOCK' & 'OLY_IDENTITY' DATA FLASH SECTIONS FROM FIRMWARE BINARY.
		// (Will now be handled by code after checking for sane data)
// Define OLY_BLOCK within Data Flash Region
OLY_BLOCK __attribute__ ((section(".oly_block"))) g_dataFlashOlyBlock =
{
	sizeof(OLY_BLOCK),	// size
	0,					// generation
	OLY_MAGIC_WORD,		// magicWord
	OLY_BLOCK_VERSION,  // version
	0,					// isIpv6
#if defined(Martin_Generic)         // 'Debug Manf' build configuration
	{OLY_MANF_STATIC_IP, 0},		// Manufacturing build Static IP configured is	: 169.254.1.101
	{OLY_DEFAULT_GATEWAY, 0},		// Manufacturing build, No Gateway configured
	{OLY_MANF_MASK, 0},				// Manufacturing build, Static IP mask is 		: 255.255.0.0
#else
	{OLY_DEFAULT_STATIC_IP, 0},	// staticIp
	{OLY_DEFAULT_GATEWAY, 0},	// gatewayIp
	{OLY_DEFAULT_MASK, 0},		// mask
#endif
	OLY_REGION_APPLICATION,	// launchType
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	// reserved[21]
	{					// rgn[0]
		OLY_REGION_APPLICATION,	// type
		{						// version
		 (OLY_FW_VERSION & 0xFF),       // major
		 (OLY_FW_VERSION >> 8) & 0xFF,  // minor
		 (OLY_FW_VERSION >> 16) & 0xFF, // sub
		 BUILD_VERSION                  // build
		},
		OLY_DEFAULT_START_ADDR,	// address
		DATA_FLASH_ADDR,		// length (504kB)
		OLY_DEFAULT_VECTOR_TABLE,// vectorTable
		OLY_DEFAULT_STACK_PTR,	// stackPtr
		0,						// crc
		{0, 0},					// align[2]
		0,						// upgradeFwCode
		{0, 0, 0, 0, 0, 0, 0, 0}	// reserved[8]
	}
};

// Define OLY_IDENTITY within Data Flash Region
OLY_IDENTITY __attribute__ ((section(".oly_identity"))) g_dataFlashOlyIdentity =
{
	{								// mac
		OLY_DEFAULT_MAC_ADDR0,
		OLY_DEFAULT_MAC_ADDR1,
		OLY_DEFAULT_MAC_ADDR2,
		OLY_DEFAULT_MAC_ADDR3,
		OLY_DEFAULT_MAC_ADDR4,
		OLY_DEFAULT_MAC_ADDR5
	},
	OLY_DEFAULT_HARDWARE_VERSION,	// hardwareRev
#if defined(CSXL_118)               // 'Debug 118' build configuration
	LOUD_BRAND_MARTIN,				// nBrand
	MARTIN_MODEL_CSXL118_NU,		// nModel
#elif defined(CDDL_12)	            // 'Debug 12' build configuration
	LOUD_BRAND_MARTIN,				// nBrand
	MARTIN_MODEL_CDDL12_NU,			// nModel
#elif defined(CDDL_15)              // 'Debug 15' build configuration
	LOUD_BRAND_MARTIN,				// nBrand
	MARTIN_MODEL_CDDL15_NU,			// nModel
#elif defined(CSXL_218)             // 'Debug 218' build configuration
	LOUD_BRAND_MARTIN,				// nBrand
	MARTIN_MODEL_CSXL218_NU,		// nModel
#elif defined(CDDL_8)               // 'Debug 8' build configuration
	LOUD_BRAND_MARTIN,				// nBrand
	MARTIN_MODEL_CDDL8_NU		    // nModel
#elif defined(Martin_Generic)       // 'Debug Manf' build configuration
	LOUD_BRAND_MARTIN,				// nBrand
	MARTIN_MODEL_GENERIC_NU			// nModel
#else
                                    // Defaults used in 'Debug' and 'Release' build configurations
	OLY_DEFAULT_BRAND,				// nBrand (LOUD_BRAND_MARTIN)
	OLY_DEFAULT_MODEL,			    // nModel (MARTIN_MODEL_CDDL12_NU)
#endif
	OLY_DEFAULT_SERIAL_NUMBER 		// uiSerialNumber
};
#endif	// End of initialisation of 'OLY_BLOCK' & 'OLY_IDENTITY' Data Flash sections. (when included with firmware binary)


//#define FORCE_STATIC_IP 1

uint8_t g_System_MAC[6] = {
		OLY_DEFAULT_MAC_ADDR0,
		OLY_DEFAULT_MAC_ADDR1,
		OLY_DEFAULT_MAC_ADDR2,
		OLY_DEFAULT_MAC_ADDR3,
		OLY_DEFAULT_MAC_ADDR4,
		OLY_DEFAULT_MAC_ADDR5 };

LOUD_brand g_systemBrand = OLY_DEFAULT_BRAND;
int32  g_nSystemModel    = OLY_DEFAULT_MODEL;
uint16 g_hardwareRev     = OLY_DEFAULT_HARDWARE_VERSION;
uint32 g_uiSerialNumber  = OLY_DEFAULT_SERIAL_NUMBER;


Region::Region( oly::Config *pConfigOwner ) :
        m_pConfigOwner(pConfigOwner),
		m_fwUpdateLedPattern(0x01),
		m_spoofingBootLoader(false)
{
	uint8_t systemMac[6];
	int32 nBrand;
	int32 nModel;
	const char *pszBrand;
	const char *pszModel;

	m_defaultType = OLY_REGION_APPLICATION;
	m_uiDefaultStart = OLY_DEFAULT_VECTOR_TABLE;
	m_uiDefaultLength = (unsigned int)OLY_BLOCK_LOCATION - m_uiDefaultStart;
	
	// Perform loading, verification and re-initialisation of OLY_IDENTITY Data Flash area if necessary.
	LoadIdentity();
	if ( !VerifyIdentity() )
	{   // Initialise Identity to defaults and save to Data Flash.
		InitializeIdentity();
		SaveIdentity();
	}

    // Get the MAC address from Flash in this order : OTP Flash MAC, Data Flash MAC, Default MAC
    readMACAddressFromOTP(g_System_MAC);

	//	Evaluate Brand
	nBrand = m_olyIdentity.nBrand;
	if ((LOUD_BRAND_EAW!=nBrand) && (LOUD_BRAND_MARTIN!=nBrand) && (LOUD_BRAND_MACKIE!=nBrand))
	{
		nBrand = OLY_DEFAULT_BRAND;
		printf("Invalid Brand, substituting default!\r\n");
	}
	switch ((LOUD_brand)nBrand) {
	case LOUD_BRAND_GENERIC:
		pszBrand = "LOUD_BRAND_GENERIC! - Substituting LOUD_BRAND_EAW!";
		nBrand = LOUD_BRAND_EAW;
		break;
	case LOUD_BRAND_EAW:
		pszBrand = "LOUD_BRAND_EAW.";
		break;
	case LOUD_BRAND_MACKIE:
		pszBrand = "LOUD_BRAND_MACKIE.";
		break;
	case LOUD_BRAND_MARTIN:
		pszBrand = "LOUD_BRAND_MARTIN.";
		break;
	default:
		pszBrand = "INVALID! - Substituting LOUD_BRAND_EAW!";
		nBrand = LOUD_BRAND_EAW;
		break;
	}
	g_systemBrand = (LOUD_brand)nBrand;
	printf("Identifying as Brand %s\r\n", pszBrand);


	//	Evaluate Model
	nModel = m_olyIdentity.nModel;
	switch (nBrand) {
	case LOUD_BRAND_EAW:
		if ((nModel<EAW_MODEL_RSX208L) || (nModel>EAW_MODEL_RSX212L))
		{
			printf("Invalid Model, substituting default!\r\n");
			nModel = OLY_DEFAULT_MODEL;
		}
		switch (nModel) {
		case EAW_MODEL_GENERIC:
			pszModel = "EAW_MODEL_GENERIC.";
			break;
		case EAW_MODEL_RSX208L:
			pszModel = "EAW_MODEL_RSX208L.";
			break;
		case EAW_MODEL_RSX86:
			pszModel = "EAW_MODEL_RSX86.";
			break;
		case EAW_MODEL_RSX89:
			pszModel = "EAW_MODEL_RSX89.";
			break;
		case EAW_MODEL_RSX126:
			pszModel = "EAW_MODEL_RSX126.";
			break;
		case EAW_MODEL_RSX129:
			pszModel = "EAW_MODEL_RSX129.";
			break;
		case EAW_MODEL_RSX12M:
			pszModel = "EAW_MODEL_RSX12M.";
			break;
		case EAW_MODEL_RSX12:
			pszModel = "EAW_MODEL_RSX12.";
			break;
		case EAW_MODEL_RSX18:
			pszModel = "EAW_MODEL_RSX18.";
			break;
		case EAW_MODEL_RSX218:
			pszModel = "EAW_MODEL_RSX218.";
			break;
		case EAW_MODEL_RSX18F:
			pszModel = "EAW_MODEL_RSX18F.";
			break;
		case EAW_MODEL_RSX212L:
			pszModel = "EAW_MODEL_RSX212L.";
			break;
		default:
			if ((nModel<EAW_MODEL_RSX208L) || (nModel>EAW_MODEL_RSX212L))
			{
				pszModel = "INVALID! - Substituting EAW_MODEL_GENERIC!";
				nModel = EAW_MODEL_GENERIC;
			}
			else
			{
				pszModel = "Unknown - Model post-dates primary boot loader.";
			}
			break;
		}
		break;
	case LOUD_BRAND_MACKIE:
			if ((nModel<MACKIE_MODEL_R8_AA) || (nModel>=MACKIE_MODEL_MASTER_FADER))
			{
				printf("Invalid Model, substituting default!\r\n");
				nModel = OLY_DEFAULT_MODEL;
			}
			switch (nModel) {
			case MACKIE_MODEL_GENERIC:
				pszModel = "MACKIE_MODEL_GENERIC.";
				break;
			case MACKIE_MODEL_R8_AA:
				pszModel = "MACKIE_MODEL_R8_AA.";
				break;
			case MACKIE_MODEL_R8_64:
				pszModel = "MACKIE_MODEL_R8_64.";
				break;
			case MACKIE_MODEL_R8_96:
				pszModel = "MACKIE_MODEL_R8_96.";
				break;
			case MACKIE_MODEL_R12_64:
				pszModel = "MACKIE_MODEL_R12_64.";
				break;
			case MACKIE_MODEL_R12_96:
				pszModel = "MACKIE_MODEL_R12_96.";
				break;
			case MACKIE_MODEL_R12_SM:
				pszModel = "MACKIE_MODEL_R12_SM.";
				break;
			case MACKIE_MODEL_R12_SW:
				pszModel = "MACKIE_MODEL_R12_SW.";
				break;
			case MACKIE_MODEL_R18_SW:
				pszModel = "MACKIE_MODEL_R18_SW.";
				break;
			default:
				if ((nModel<MACKIE_MODEL_R8_AA) || (nModel>=MACKIE_MODEL_MASTER_FADER))
				{
					pszModel = "INVALID! - Substituting MACKIE_MODEL_GENERIC!";
					nModel = MACKIE_MODEL_GENERIC;
				}
				else
				{
					pszModel = "Unknown - Model post-dates primary boot loader.";
				}
				break;
			}
			break;
	case LOUD_BRAND_MARTIN:
		if ((nModel<MARTIN_MODEL_GENERIC) || (nModel>=0x20))
		{
			printf("Invalid Model, substituting default!\r\n");
			nModel = OLY_DEFAULT_MODEL;
		}
		switch (nModel) {
		case MARTIN_MODEL_GENERIC:
			pszModel = "MARTIN_MODEL_GENERIC.";
			break;			
		case MARTIN_MODEL_GENERIC_NU:
			pszModel = "MARTIN_MODEL_GENERIC_NU.";
			break;
		case MARTIN_MODEL_CDDL8:
			pszModel = "MARTIN_MODEL_CDDL8.";
			break;			
		case MARTIN_MODEL_CDDL8_NU:
			pszModel = "MARTIN_MODEL_CDDL8_NU.";
			break;
		case MARTIN_MODEL_CDDL12:
			pszModel = "MARTIN_MODEL_CDDL12.";	
			break;				
		case MARTIN_MODEL_CDDL12_NU:
			pszModel = "MARTIN_MODEL_CDDL12_NU.";
			break;
		case MARTIN_MODEL_CDDL15:
			pszModel = "MARTIN_MODEL_CDDL15.";	
			break;				
		case MARTIN_MODEL_CDDL15_NU:		
			pszModel = "MARTIN_MODEL_CDDL15_NU.";
			break;
		case MARTIN_MODEL_CSXL118:
			pszModel = "MARTIN_MODEL_CSXL118.";	
			break;				
		case MARTIN_MODEL_CSXL118_NU:		
			pszModel = "MARTIN_MODEL_CSXL118_NU.";
			break;
		case MARTIN_MODEL_CSXL218:
			pszModel = "MARTIN_MODEL_CSXL218.";
			break;		
		case MARTIN_MODEL_CSXL218_NU:		
			pszModel = "MARTIN_MODEL_CSXL218_NU.";
			break;
		default:
			if ((nModel<MARTIN_MODEL_GENERIC) || (nModel>=0x20))
			{
				pszModel = "INVALID! - Substituting MARTIN_MODEL_GENERIC!";
				nModel = MARTIN_MODEL_GENERIC_NU;
			}
			else
			{
				pszModel = "Unknown - Model post-dates primary boot loader.";
			}
			break;
		}
		break;
	default:
		pszModel = "Error!";
		break;
	}
	g_nSystemModel = nModel;
	printf("Identifying as Model %s\r\n", pszModel);


	g_hardwareRev = m_olyIdentity.hardwareRev;
	if ((unsigned short)0xFFFF==(unsigned short)g_hardwareRev)
	{
		g_hardwareRev = OLY_DEFAULT_HARDWARE_VERSION;
		printf("Invalid hardware revision, substituting default!\r\n");
	}
	printf("Identifying as hardware revision %d\r\n", (unsigned int)g_hardwareRev);

	g_uiSerialNumber = m_olyIdentity.uiSerialNumber;
	if (0xFFFFFFFF==g_uiSerialNumber)
	{
		g_uiSerialNumber = OLY_DEFAULT_SERIAL_NUMBER;
		printf("Invalid Serial Number, substituting default!\r\n");
	}
	printf("Identifying as serial number %d(0x%08X)\r\n", g_uiSerialNumber, g_uiSerialNumber);

	m_UpgradeState = usmInvalid;

	// Perform loading, verification and re-initialisation of OLY_BLOCK Data Flash area if necessary.
	Load();
	if (!Verify())
	{   // Initialise OLY_BLOCK to defaults and save to Data Flash.
		Initialize();
		Save();
	}

	//	if you need to force to static IP - do it here
#ifdef	FORCE_STATIC_IP
	olyBlock.staticIp.ipv4.ip = IPADDR(192,168,1,50);
	olyBlock.gatewayIp.ipv4.ip = IPADDR(192,168,1,0);
	olyBlock.mask.ipv4.ip = IPADDR(255,255,255,0);
#endif	//	FORCE_STATIC_IP

	// On initialisation, we clear any 'upgrade firmware' signal to BootLoader as we have
	// now successfully launched APP by virtue of being here.
	if (ClearSpiFlashBootloaderSignal() == false)
		printf("Failed to Clear BOOTLOADER signal\n");
}

Region::~Region()
{

}

unsigned char *Region::GetSystemMAC()
{
	return(g_System_MAC);
}

LOUD_brand Region::GetSystemBrand()
{
	return(g_systemBrand);
}

int32 Region::GetSystemModel()
{
	return(g_nSystemModel);
}

uint16 Region::GetHardwareRevision()
{
	return(g_hardwareRev);
}

uint32 Region::GetSerialNumber()
{
	return(g_uiSerialNumber);
}

//	For debugging use only - overwrites globals but does not change flash settings
void Region::SetSystemBrand(LOUD_brand brand)
{
	g_systemBrand = brand;
}

//	For debugging use only - overwrites globals but does not change flash settings
void Region::SetSystemModel(int32 nModel)
{
	g_nSystemModel = nModel;
}

bool Region::ValidateMAC(uint8 mac[6])
{
	uint32 upper,lower;

	upper = ((mac[0]&0xFF) << 8) | (mac[1] & 0xFF);
	lower = ((mac[2]&0xFF) << 24) | ((mac[3]&0xFF) << 16) | ((mac[4]&0xFF) << 8) | (mac[5] & 0xFF);
	if (upper != 0x0000000F)
		return false;
	if ((lower < 0xF2080000) | (lower > 0xF208FFFF))
		return false;

	return true;
}

// Load Identity from Data Flash OLY_IDENTITY area
void Region::LoadIdentity()
{
	memcpy(&m_olyIdentity, OLY_IDENTITY_LOCATION, sizeof(m_olyIdentity));
}

// Verify the contents of m_olyIdentity look correct.
// Returns true if content looks ok, false otherwise.
bool Region::VerifyIdentity()
{
	if ( !ValidateMAC(m_olyIdentity.mac) )
		return false;	// Invalid MAC address in OLY_IDENTITY

	if ( m_olyIdentity.nBrand != LOUD_BRAND_MARTIN )
		return false;	// We only expect MARTIN brand at present

	switch ( m_olyIdentity.nModel )
	{
#if defined(CSXL_118)               // 'Release/Debug 118' build configuration
	case MARTIN_MODEL_CSXL118_NU:
#elif defined(CDDL_12)	            // 'Release/Debug 12' build configuration
	case MARTIN_MODEL_CDDL12_NU:
#elif defined(CDDL_15)              // 'Release/Debug 15' build configuration
	case MARTIN_MODEL_CDDL15_NU:
#elif defined(CSXL_218)             // 'Release/Debug 218' build configuration
	case MARTIN_MODEL_CSXL218_NU:
#elif defined(CDDL_8)               // 'Release/Debug 8' build configuration
	case MARTIN_MODEL_CDDL8_NU:
#elif defined(Martin_Generic)       // 'Release/Debug Manf' build configuration
	case MARTIN_MODEL_GENERIC_NU:
#else								// 'Release/Debug' builds (Identify as CDDL_12)
	case OLY_DEFAULT_MODEL:
#endif
		break;	// All Above are valid Models that we expect

	default:
		return false;	// Unsupported Model
	}

	return true;	// Identity Block checks out ok.
}

// Initialise the Identity block to defaults for the case where it's contents are unexpected.
void Region::InitializeIdentity()
{
	// Initialise Identity to defaults according to Brand / Model.
	// (Currently only MARTIN models : CDDL8, CDDL12, CDDL15, CSXL118, CSXL218 are supported)
	if ( !ValidateMAC(m_olyIdentity.mac) )
	{   // Only initialise MAC to default if it is invalid.
    	m_olyIdentity.mac[0] = OLY_DEFAULT_MAC_ADDR0;
    	m_olyIdentity.mac[1] = OLY_DEFAULT_MAC_ADDR1;
    	m_olyIdentity.mac[2] = OLY_DEFAULT_MAC_ADDR2;
    	m_olyIdentity.mac[3] = OLY_DEFAULT_MAC_ADDR3;
    	m_olyIdentity.mac[4] = OLY_DEFAULT_MAC_ADDR4;
    	m_olyIdentity.mac[5] = OLY_DEFAULT_MAC_ADDR5;
    }
    printf("Initialising Identity to Defaults, Data Flash MAC is %02X:%02X:%02X:%02X:%02X:%02X\r\n",
        	m_olyIdentity.mac[0], m_olyIdentity.mac[1], m_olyIdentity.mac[2],
        	m_olyIdentity.mac[3], m_olyIdentity.mac[4], m_olyIdentity.mac[5] );
	m_olyIdentity.hardwareRev = OLY_DEFAULT_HARDWARE_VERSION;
	m_olyIdentity.nBrand = OLY_DEFAULT_BRAND;		// Default Brand is : LOUD_BRAND_MARTIN
	m_olyIdentity.uiSerialNumber = OLY_DEFAULT_SERIAL_NUMBER;
#if defined(CSXL_118)               // 'Release/Debug 118' build configuration
	m_olyIdentity.nModel = MARTIN_MODEL_CSXL118_NU;
#elif defined(CDDL_12)	            // 'Release/Debug 12' build configuration
	m_olyIdentity.nModel = MARTIN_MODEL_CDDL12_NU;
#elif defined(CDDL_15)              // 'Release/Debug 15' build configuration
	m_olyIdentity.nModel = MARTIN_MODEL_CDDL15_NU;
#elif defined(CSXL_218)             // 'Release/Debug 218' build configuration
	m_olyIdentity.nModel = MARTIN_MODEL_CSXL218_NU;
#elif defined(CDDL_8)               // 'Release/Debug 8' build configuration
	m_olyIdentity.nModel = MARTIN_MODEL_CDDL8_NU;
#elif defined(Martin_Generic)       // 'Release/Debug Manf' build configuration
	m_olyIdentity.nModel = MARTIN_MODEL_GENERIC_NU;
#else								// 'Release/Debug' builds identify as CDDL_12
	m_olyIdentity.nModel = OLY_DEFAULT_MODEL;
#endif
}

void Region::SaveIdentity()
{
	printf("Saving Identity Sector...");
    if ( data_flash_program((uint32_t)OLY_IDENTITY_LOCATION, (uint32_t *)&m_olyIdentity, sizeof(m_olyIdentity)/sizeof(uint32_t)) )
    {
	    printf("Success.\r\n");
    }
}

//	Load region data from Flash
void Region::Load()
{
	// Need the proper Flash layout for this to work on Nuvoton board
	memcpy(&olyBlock, OLY_BLOCK_LOCATION, sizeof(olyBlock));
}

//	Verify the RAM version is valid
//	In future, will have to deal with changes to structure for backwards compatibility
bool Region::Verify()
{
	bool bValid = true;

	//	Is it a valid header?
	if ((sizeof(OLY_BLOCK)!=olyBlock.size) || (OLY_MAGIC_WORD!=olyBlock.magicWord))
		bValid = false;
	else
	{
		//	Check header version
		printf("System block has header version %d\r\n", olyBlock.version);
		//	but warn if version is newer than me, provide place for interpretation and upgrade of old header
		if (OLY_BLOCK_VERSION<olyBlock.version)
		{
			printf("Warning! System block is newer header version than main firmware!\r\n");
			//	Do any necessary upgrades or special interpretation of newer header here!
		}
		else if (OLY_BLOCK_VERSION > olyBlock.version)
		{
			//	Do any necessary upgrades or special interpretation of older header here!
		}
	}

	//	Look for invalid launch application
	if ( olyBlock.launchType != OLY_REGION_APPLICATION )
		bValid = false;

	//	Verify region descriptors as best as possible, not knowing what the future will change
	for (uint32_t nRegion=0; nRegion < OLY_MAX_REGIONS; nRegion++)
	{
		if ( olyBlock.rgn[nRegion].type == OLY_REGION_APPLICATION )
		{
			if ((olyBlock.rgn[nRegion].length == 0) || (olyBlock.rgn[nRegion].length == 0xFFFFFFFF))
				bValid = false;
		}
	}

	printf(bValid?"System block valid.\r\n":"System block invalid!\r\n");
	return(bValid);
}

//	Initialize an invalid region database
void Region::Initialize()
{
	printf("Initializing system block to defaults!\r\n");

	memset(&olyBlock, 0, sizeof(olyBlock));

	olyBlock.size = sizeof(olyBlock);
	olyBlock.generation = 0;
	olyBlock.magicWord = OLY_MAGIC_WORD;
	olyBlock.version = OLY_BLOCK_VERSION;
	olyBlock.launchType = m_defaultType;
#if defined(Martin_Generic)         // 'Release/Debug Manf' build configuration only : FORCE STATIC IP
	olyBlock.staticIp.ipv4.ip = OLY_MANF_STATIC_IP;		// Manufacturing build Static IP configured is	: 169.254.1.101
	olyBlock.gatewayIp.ipv4.ip = OLY_DEFAULT_GATEWAY;	// Manufacturing build, No Gateway configured
	olyBlock.mask.ipv4.ip = OLY_MANF_MASK;				// Manufacturing build, Static IP mask is 		: 255.255.0.0
#else
	olyBlock.staticIp.ipv4.ip = OLY_DEFAULT_STATIC_IP;
	olyBlock.gatewayIp.ipv4.ip = OLY_DEFAULT_GATEWAY;
	olyBlock.mask.ipv4.ip = OLY_DEFAULT_MASK;
#endif

	olyBlock.rgn[0].type = m_defaultType;			// Default is APPLICATION
	olyBlock.rgn[0].version.u32 = OLY_FW_VERSION;
	olyBlock.rgn[0].address = m_uiDefaultStart;
	olyBlock.rgn[0].length = m_uiDefaultLength;
	olyBlock.rgn[0].vectorTable = m_uiDefaultStart;
	olyBlock.rgn[0].stackPtr = OLY_DEFAULT_STACK_PTR;

	//	Calculate CRC
	unsigned char *pCrcPtr = (unsigned char *)olyBlock.rgn[0].address;
	unsigned short crcCalc;
	REGION_CRC rgnCrc;
	Crc16Init(&rgnCrc);
	// Need to CRC APROM image and store CRC in olyBlock
	Crc16UpdateFromApromFlash(&rgnCrc, pCrcPtr, olyBlock.rgn[0].length);
	Crc16Finalize(&rgnCrc, &crcCalc);
	olyBlock.rgn[0].crc =crcCalc;
	olyBlock.rgn[0].upgradeFwCode = 0;
}

void Region::SetStaticIp(uint32_t uiIp)
{
	olyBlock.staticIp.ipv4.ip = uiIp;
	Save();
	
	if ( m_pConfigOwner )
		m_pConfigOwner->SetIP();	// Force network change
}

uint32_t Region::GetStaticIp()
{
	//	Protection for un-initialized Flash
	if (0xFFFFFFFF==olyBlock.staticIp.ipv4.ip)
		return(0);
	return(olyBlock.staticIp.ipv4.ip);
}

uint32_t Region::GetStaticGateway()
{
	//	Protection for un-initialized Flash
	if (0xFFFFFFFF==olyBlock.staticIp.ipv4.ip)
		return(0);
	return(olyBlock.gatewayIp.ipv4.ip);
}

uint32_t Region::GetStaticMask()
{
	//	Protection for un-initialized Flash
	if (0xFFFFFFFF==olyBlock.staticIp.ipv4.ip)
		return(0);
	return(olyBlock.mask.ipv4.ip);
}

void Region::SetIpSettings(uint32_t uiIp, uint32_t uiGateway, uint32_t uiMask)
{
	//	Don't allow invalid IP
	if (0xFFFFFFFF==olyBlock.staticIp.ipv4.ip)
	{
		uiIp = 0;
		uiGateway = 0;
		uiMask = 0;
	}
	olyBlock.staticIp.ipv4.ip = uiIp;
	olyBlock.gatewayIp.ipv4.ip = uiGateway;
	olyBlock.mask.ipv4.ip = uiMask;
	Save();
	
	if ( m_pConfigOwner )
		m_pConfigOwner->SetIP();	// Force network change
}

//---------------------------------------------------------------------------
// Save region data to DATA Flash
//---------------------------------------------------------------------------
void Region::Save()
{
	printf("Saving Region Block to Data Flash.\r\n");
	uint32_t uiFlashAdddress = (unsigned int)OLY_BLOCK_LOCATION;

	olyBlock.generation++;
	
	if (data_flash_program(uiFlashAdddress, (uint32_t *)&olyBlock, sizeof(OLY_BLOCK)/sizeof(uint32_t)) == false )
		printf("Failed to save Region to Data Flash\n");
}

//	Search RAM copy of Region Table for matching region type
int Region::FindFirst(OLY_REGION_TYPE type)
{
	for(int nRegion=0;nRegion<OLY_MAX_REGIONS;nRegion++)
	{
		if (type==olyBlock.rgn[nRegion].type)
		{
			return(nRegion);
		}
	}
	return(-1);
}

// Verify the Region held in Data Flash against APROM contents
bool Region::VerifyRegion(int nRegion)
{
	if ((nRegion>=0) && (nRegion<OLY_MAX_REGIONS))
	{
		if ((olyBlock.rgn[nRegion].length) && (0xFFFFFFFF!=olyBlock.rgn[nRegion].length))
		{
			unsigned char *pCrcPtr = (unsigned char *)olyBlock.rgn[nRegion].address;
			unsigned short crcCalc;
			REGION_CRC rgnCrc;
			Crc16Init(&rgnCrc);
			Crc16UpdateFromApromFlash(&rgnCrc, pCrcPtr, olyBlock.rgn[nRegion].length);
			Crc16Finalize(&rgnCrc, &crcCalc);
			printf("Verifying region %d Adr=0x%08X, Len=0x%08X, C1=0x%08X C2=0x%08X...\r\n",
					nRegion, olyBlock.rgn[nRegion].address, olyBlock.rgn[nRegion].length, olyBlock.rgn[nRegion].crc, crcCalc);
			if (olyBlock.rgn[nRegion].crc==crcCalc)
			{
				printf("Region #%d verified.\r\n", nRegion);
				return(true);
			}
			else
				printf("Region CRC invalid!\r\n");
		}
		else
			printf("Region length invalid!\r\n");
	}
	else
		printf("Region number invalid!\r\n");
		
	return(false);
}

//	Clear out a region that will be upgraded
bool Region::ClearRegion(int nRegion)
{
	printf("Clearing Region #%d\r\n", nRegion);
	if ((nRegion>=0) && (nRegion<OLY_MAX_REGIONS))
	{
		memset(&olyBlock.rgn[nRegion], 0, sizeof(OLY_REGION));
		return(true);
	}
	return(false);
}

//	Copy region information (e.g. from FW upgrade) to olyBlock
bool Region::WriteRegion(int nRegion, P_OLY_REGION pOlyRegion)
{
	printf("Writing Region #%d.\r\n", nRegion);

	if ((nRegion>=0) && (nRegion<OLY_MAX_REGIONS))
	{
		memcpy(&olyBlock.rgn[nRegion], pOlyRegion, sizeof(OLY_REGION));
		return(true);
	}
	return(false);
}

//------------------------------------------------------------------------------------
// Update the Header within the SPI flash firmware area 
//------------------------------------------------------------------------------------
bool Region::WriteSpiFlashHeader( OLY_REGION *pRegion )
{
//	printf("FW: Update Header in SPI Flash\n");
//	printf( "\tupgradeFwCode = %08X\n", pRegion->upgradeFwCode);
	return spi_flash_writeFwHeader( pRegion );
}

//------------------------------------------------------------------------------------
// Clear Signal to Bootloader within SPI Flash FW Header (always called on startup)
//------------------------------------------------------------------------------------
bool Region::ClearSpiFlashBootloaderSignal( void )
{
	bool bResult = true;
	OLY_REGION *pOlyRegion = &olyBlock.rgn[0];
	uint32_t upgradeFwCodeOffset = (uint32_t)&(pOlyRegion->upgradeFwCode) - (uint32_t)pOlyRegion;
	printf("------------------------------------\n");
	printf("FIRMWARE VERSION %d.%d.%d Build %d",
	        pOlyRegion->version.major, pOlyRegion->version.minor, pOlyRegion->version.sub, pOlyRegion->version.build );
	        
	unsigned int upgradeFwCode = (unsigned int) spi_flash_read_fw_word( upgradeFwCodeOffset );
	if ( upgradeFwCode == OLY_UPGRADE_THE_FIRMWARE )
	{
		printf(", NEW FIRMWARE Detected");
		// We can now invalidate the firmware header in SPI Flash by erasing the first block.
		if (spi_flash_fw_erase_block(0) == 0)
			bResult = false;	// Failed to Erase
	}
	printf("\n------------------------------------\n");
	return( bResult );
}

//------------------------------------------------------------------------------------
// Program a chunk of SPI flash firmware area (offset file is within upgrade file from mandolin)
//------------------------------------------------------------------------------------
bool Region::WriteChunk( uint32_t uiFlashOffset, uint8_t *pChunk )
{
//	printf("\tFW: Write Chunk, offset 0x%08X (%d kB)\n", uiFlashOffset, uiFlashOffset/1024);
	bool bResult = spi_flash_write_fw( pChunk, uiFlashOffset, (uint32_t)FW_UPGRADE_CHUNK_SIZE );  // Write sector to SPI flash FW area
	return( bResult );
}

//------------------------------------------------------------------------------------
// Verify the programming of a chunk of SPI flash firmware area
//------------------------------------------------------------------------------------
bool Region::VerifyChunk(uint32_t uiFlashOffset, uint8_t *pChunk)
{
	return spi_flash_verify_fw_chunk( uiFlashOffset, pChunk );
}

//----------------------------------------------------------------------------------------------
// Called by Mandolin Process when it received the upgrade file header
// via 'MANDOLIN_MSG_FILE_OPEN' msg
//----------------------------------------------------------------------------------------------
bool Region::StartUpgrade(P_OLY_REGION pOlyRegion)
{
	int nRegion;

	printf("Starting Upgrade Process for region type %d, New Version %d.%d.%d Build %d\r\n", (int)pOlyRegion->type,
			pOlyRegion->version.major, pOlyRegion->version.minor, pOlyRegion->version.sub, pOlyRegion->version.build );

	if ( (pOlyRegion->version.major < MIN_FW_VERSION_MAJOR) ||
		 ((pOlyRegion->version.major == MIN_FW_VERSION_MAJOR) && (pOlyRegion->version.minor < MIN_FW_VERSION_MINOR)) ||
		 ((pOlyRegion->version.major == MIN_FW_VERSION_MAJOR) && (pOlyRegion->version.minor == MIN_FW_VERSION_MINOR) &&
		  (pOlyRegion->version.sub < MIN_FW_VERSION_SUB)) )
	{
		printf("Firmware version too old for upgrade\n");
		m_spoofingBootLoader = false;
		return( false );
	}

	if ( pOlyRegion->address != APROM_APP_LOCATION )
	{	// This should avoid any old unsupported CDDLive firmware files being accepted
		printf("Only APP firmware with starting address 0x%08X is accepted\n", APROM_APP_LOCATION);
		m_spoofingBootLoader = false;
		return( false );
	}

	// 	CDDLive : We are upgrading image in SPI Flash Region, so OK to accept APP type image, so check below not necessary.
#if 0
	//	Verify that region is compatible with upgrade type
	//	Only allow one of eeach type for now
	if (m_defaultType==pOlyRegion->type)
	{
		printf("Aborting trying to upgrade currently running application type %d.\r\n", (int)pOlyRegion->type);
		m_spoofingBootLoader = false;
		return(false);
	}
#endif

	//	determine which region will be updated
	nRegion = FindFirst(pOlyRegion->type);

	//	Delete existing region
	if ( -1 != nRegion )
	{
		printf("Found existing region #%d of type %d to upgrade.\r\n", nRegion, (int)pOlyRegion->type);
		ClearRegion(nRegion);
		// New Upgrade process will just erase the firmware area in SPI Flash.
        printf("Erasing SPI Flash FW....\r\n");
        spi_flash_fw_erase();
	}
	else
	{
		nRegion = FindFirst(OLY_REGION_UNUSED);
		printf("No existing region of type %d found, choosing first empty region #%d.\r\n", (int)pOlyRegion->type, nRegion);
	}

	if ( -1 != nRegion )
	{
		//	Save for later updating
		m_nUpgradeRegion = nRegion;
		m_rgnUpgrade = *pOlyRegion;		// Region info from firmware update file.
		m_UpgradeState = usmStarted;
		m_uiUpgradeFilled = 0;
		printf("Upgrade Process Started.\r\n");
	}

	// Initialise visual indication to user that we are in UPDATE mode (rotating LEDs)
	cddl_getLedStatePriorToFwUpdate( m_storedLedState );	// Remember the current state of the LEDs
	cddl_set_meter_override(BACKPANEL_LED_MODE);
	return(true);
}

void Region::abortFwUpgrade( void )
{
	m_spoofingBootLoader = false;
	cddl_restoreLedState( m_storedLedState );	// Restore the LED state
}

//----------------------------------------------------------------------------------------------
//	Called by Mandolin Process when it received firmware chunk via 'MANDOLIN_MSG_POST_FILE' msg
//----------------------------------------------------------------------------------------------
bool Region::UpgradeChunk(unsigned char *pChunk, unsigned int uiOffset, unsigned int uiBytes)
{
	unsigned char *pChar = (unsigned char *)(m_rgnUpgrade.address + uiOffset);

	if (usmStarted != m_UpgradeState)
	{
		printf("UpgradeChunk rejected because not in upgrade mode!\r\n");
		abortFwUpgrade();
		return(false);
	}

	if ((uiBytes + uiOffset) > (m_rgnUpgrade.length + sizeof(OLY_REGION)))
	{
		printf("UpgradeChunk rejected because of too many bytes received!\r\n");
		abortFwUpgrade();
		return(false);
	}

#if 0	// CDDLive : This check not necessary since we are now upgrading to SPI flash FW region
	if (((m_rgnUpgrade.address + uiOffset) >= 0) && ((m_rgnUpgrade.address + uiOffset) < FW_UPGRADE_CHUNK_SIZE))
	{
		printf("UpgradeChunk rejected because overwriting first sector!\r\n");
		abortFwUpgrade();
		return(false);
	}
#endif

	if (uiOffset &(FW_UPGRADE_CHUNK_SIZE-1))
	{
		printf("UpgradeChunk rejected because start of write not on sector boundary!\r\n");
		abortFwUpgrade();
		return(false);
	}

	printf("Write %d bytes to offset 0x%08X (%d kB)...",
				uiBytes, m_rgnUpgrade.address + uiOffset, (m_rgnUpgrade.address + uiOffset)/1024);

	if ( VerifyChunk(m_rgnUpgrade.address + uiOffset, pChunk) )
		printf("Skipping\r\n");
	else if ( !WriteChunk(m_rgnUpgrade.address + uiOffset, pChunk) )
	{
		printf("Failed\r\n");
		abortFwUpgrade();
		return(false);
	}
	else
		printf("Done\r\n");

	if (!VerifyChunk(m_rgnUpgrade.address + uiOffset, pChunk))
	{
		printf("\tVerify failed @ offset 0x%08X\r\n", m_rgnUpgrade.address + uiOffset);
		abortFwUpgrade();
		return(false);
	}

	if ((m_uiUpgradeFilled % 0x2000) == 0)
	{	// Move on LED pattern every 8kB
		cddl_showNextFwUpdateLedPattern(false);
	}
	m_uiUpgradeFilled += uiBytes;

	return(true);
}

//	Called to determine how many bytes are remaining in upgrade
bool Region::GetUpgradeRemaining(uint32_t &uiTotal, uint32_t &uiRemaining)
{
	uiTotal = 0;
	uiRemaining = 0;

	if (usmStarted != m_UpgradeState)
	{
		return(false);
	}

	uiTotal = m_rgnUpgrade.length;
	uiRemaining = uiTotal - m_uiUpgradeFilled + sizeof(OLY_REGION);

	return(true);
}

//	called to terminate an upgrade inprocess
void Region::CancelUpgrade()
{
	m_UpgradeState = usmInvalid;
	printf("Upgrade Cancelled!\r\n");
	abortFwUpgrade();
}

//----------------------------------------------------------------------------------------------
//Called by Mandolin Process (via 'MANDOLIN_MSG_FILE_CLOSE' msg) to finalize an upgrade
// and verify it is good 
//----------------------------------------------------------------------------------------------
bool Region::EndUpgrade()
{
	const uint32_t FW_UPGRADE_REBOOT_DELAY_MS = 500;
    unsigned char *pCrcPtr = (unsigned char *)m_rgnUpgrade.address;	// Offset (after SPI Flash FW header) for CRC start
	unsigned short crcCalc;
	REGION_CRC rgnCrc;

	printf("Verifying Upgrade...\r\n");

	//	In upgrade?
	if (usmStarted!=m_UpgradeState)
	{
		printf("EndUpgrade failed because not currently upgrading!\r\n");
		return(false);
	}

	//	Finished ?
	if (m_uiUpgradeFilled != m_rgnUpgrade.length)
	{
		printf("EndUpgrade failed because not enough bytes have been written, %d file, expected (Header %d + FW %d)\r\n",
		            m_uiUpgradeFilled, sizeof(OLY_REGION), m_rgnUpgrade.length);
		abortFwUpgrade();
		return(false);
	}

	//	Verify CRC
	cddl_showNextFwUpdateLedPattern(true);      // Clear LEDs as verification takes a little while.
	Crc16Init(&rgnCrc);
	Crc16UpdateFromSpiFlash(&rgnCrc, pCrcPtr, m_rgnUpgrade.length);
	Crc16Finalize(&rgnCrc, &crcCalc);
	if (m_rgnUpgrade.crc != crcCalc)
	{
		printf("EndUpgrade failed because CRC didn't match Adr=0x%08X, Len=0x%08X, C1=0x%08X, C2=0x%08X!\r\n",
			m_rgnUpgrade.address, m_rgnUpgrade.length, m_rgnUpgrade.crc, crcCalc);
		abortFwUpgrade();
		return(false);
	}
	printf("EndUpgrade CRC matched Adr=0x%08X, Len=0x%08X, C1=0x%08X.\r\n",
		m_rgnUpgrade.address, m_rgnUpgrade.length, m_rgnUpgrade.crc);

	//	Write new region to 'olyBlock', then Save that to Data Flash Region area.
	m_rgnUpgrade.upgradeFwCode = OLY_UPGRADE_THE_FIRMWARE;  // Signal to BOOTLOADER that it needs to upgrade firmware with this image.
	if ( WriteSpiFlashHeader( &m_rgnUpgrade ) == false )
	{
		printf("Failed to write SPI Flash Header\n");
		abortFwUpgrade();
		return false;
	}

//	WriteRegion(m_nUpgradeRegion, &m_rgnUpgrade);	// Not required since Upgrade is now updating SPI Flash region
//	Save();

	printf("Upgrade complete.\n");
	m_UpgradeState = usmInvalid;	// Clear upgrade state in case called again before reboot

	cddl_restoreLedState( m_storedLedState );	// Restore the LED state
	
	// Launch into BOOTLOADER which will detect new image in SPI Flash and program it into APROM Flash.
	// We wait 500 milliseconds to reboot, this allows this routine to return and send response message to VU-NET.
	DeferredReboot(FW_UPGRADE_REBOOT_DELAY_MS);

	return(true);
}

void Region::launchBootloader(void)
{
	#define APROM_WITH_IAP_MASK		0x00000040	// Mask for CONFIG0, APROM with IAP boot mode
    uint32_t  au32Config[2];           // User Configuration
    printf("Resetting to launch BootLoader\n");

	SYS_UnlockReg();
	FMC_Open();
    if (FMC_ReadConfig(au32Config, 2) < 0)
    {	// Read User Configuration CONFIG0 and CONFIG1
        printf("FMC_ReadConfig failed, please reboot to launch Bootloader\n");
    	FMC_Close();
        SYS_LockReg();
        return;
    }

    // Check if configured for boot from APROM with IAP
    if (au32Config[0] & APROM_WITH_IAP_MASK)
    {
        au32Config[0] &= ~APROM_WITH_IAP_MASK;        // Select APROM with IAP boot mode.

        // Update User Configuration CONFIG0 and CONFIG1.
        if (FMC_WriteConfig(au32Config, 2) != 0)
        {
            printf("FMC_WriteConfig failed, please reboot to launch Bootloader\n");
        	FMC_Close();
            SYS_LockReg();
            return;
        }
    }

    //---- Perform chip reset to make new User Config take effect, Bootloader will launch ----
    /*  NOTE!
     *     Before change VECMAP, user MUST disable all interrupts.
     */
    __disable_irq();
    FMC_SetVectorPageAddr(APROM_BOOT_LOCATION);	/* Vector remap APROM page 0 to start of BOOTLOADER at 0x0000. */
    if (g_FMC_i32ErrCode != 0)
    {
        printf("Vector Remap Fail\n");
        while (1);
    }

    FMC_Close();
    printf("CONFIG0 = %08X, CONFIG1 = %08X\n", au32Config[0], au32Config[1]);
    printf("RESETTING CHIP - Please Reboot if Bootloader doesn't launch\r\n\r\n");
    while (!(UART0->FIFOSTS & UART_FIFOSTS_TXEMPTY_Msk));       /* wait until UART0 TX FIFO is empty */
    SYS_ResetChip();

	SYS_LockReg();	// Never gets here ?
	while (1);
}

//	returns true if launch type needed to be changed
bool Region::SetLaunchType(OLY_REGION_TYPE launchType)
{
#if 0	// Not Required since we do not have secondary BootLoader
	printf("Setting launch type to %d.\n", launchType);

	if (olyBlock.launchType != launchType)
	{
		//	Look for valid region of requested type
		int nRgn = FindFirst(launchType);
		printf("Found region #%d containing desired launch type %d.\r\n", nRgn);
		if (-1==nRgn)
		{
			printf("There is no region of the requested type to launch!\r\n");
			return(false);
		}
		if (!VerifyRegion(nRgn))
		{
			printf("The region for the requested launch type is invalid!\r\n");
			return(false);
		}

		olyBlock.launchType = launchType;
		Save();
		return(true);
	}
#else
	// During firmware upgrade we spoof being the Secondary BootLoader so that 'GetSoftwareInfo' replies to VU-NET
	// will identify us as having rebooted into bootloader so that VU-NET sends across the firmware file.
	m_spoofingBootLoader = (launchType == OLY_REGION_SECONDARY_BOOT);
	printf("Launch type set to %s\n", m_spoofingBootLoader ? "BOOT":"APP");
#endif
	return(true);
}

// Required when we spoof being the secondary bootloader during firmware upgrade to keep VU-NET happy.
// Called to formulate the reply to GetSoftwareInfo mandolin message sent by VU-NET
OLY_REGION_TYPE Region::GetLaunchType(void)
{
	if ( m_spoofingBootLoader )
		return OLY_REGION_SECONDARY_BOOT;
	else
		return OLY_REGION_APPLICATION;
}

//	Get the model name based on a Mandolin brand and model
const char *Region::GetMandolinModelName(LOUD_brand mandolinBrand, int nMandolinModel)
{
	const char *pszModel = "Undefined";

	switch (mandolinBrand) {
	case LOUD_BRAND_MACKIE:
		switch (nMandolinModel) {
		case MACKIE_MODEL_R8_AA:
			pszModel = "R8-AA";
			break;
		case MACKIE_MODEL_R8_64:
			pszModel = "R8-64";
			break;
		case MACKIE_MODEL_R8_96:
			pszModel = "R8-96";
			break;
		case MACKIE_MODEL_R12_64:
			pszModel = "R12-64";
			break;
		case MACKIE_MODEL_R12_96:
			pszModel = "R12-96";
			break;
		case MACKIE_MODEL_R12_SM:
			pszModel = "R12-SM";
			break;
		case MACKIE_MODEL_R12_SW:
			pszModel = "R12-SW";
			break;
		case MACKIE_MODEL_R18_SW:
			pszModel = "R18-SW";
			break;
		default:
			break;
		}
		break;
	case LOUD_BRAND_EAW:
			switch (nMandolinModel) {
			case EAW_MODEL_RSX208L:
				pszModel = "RSX208L";
				break;
			case EAW_MODEL_RSX86:
				pszModel = "RSX86";
				break;
			case EAW_MODEL_RSX89:
				pszModel = "RSX89";
				break;
			case EAW_MODEL_RSX126:
				pszModel = "RSX126";
				break;
			case EAW_MODEL_RSX129:
				pszModel = "RSX129";
				break;
			case EAW_MODEL_RSX12M:
				pszModel = "RSX12M";
				break;
			case EAW_MODEL_RSX12:
				pszModel = "RSX12";
				break;
			case EAW_MODEL_RSX18:
				pszModel = "RSX18";
				break;
			case EAW_MODEL_RSX218:
				pszModel = "RSX218";
				break;
			case EAW_MODEL_RSX18F:
				pszModel = "RSX18F";
				break;
			case EAW_MODEL_RSX212L:
				pszModel = "RSX212L";
				break;
			default:
				break;
			}
			break;
	case LOUD_BRAND_MARTIN:
		switch (nMandolinModel) {
		case MARTIN_MODEL_CDDL8:
			pszModel = "CDD-L8";
			break;		
		case MARTIN_MODEL_CDDL8_NU:		
			pszModel = "CDD-L8-NU";
			break;
		case MARTIN_MODEL_CDDL12:
			pszModel = "CDD-L12";
			break;		
		case MARTIN_MODEL_CDDL12_NU:		
			pszModel = "CDD-L12-NU";
			break;
		case MARTIN_MODEL_CDDL15:
			pszModel = "CDD-L15";
			break;		
		case MARTIN_MODEL_CDDL15_NU:		
			pszModel = "CDD-L15-NU";
			break;
		case MARTIN_MODEL_CSXL118:
			pszModel = "CSX-L118";
			break;		
		case MARTIN_MODEL_CSXL118_NU:		
			pszModel = "CSX-L118-NU";
			break;
		case MARTIN_MODEL_CSXL218:
			pszModel = "CSX-L218";
			break;		
		case MARTIN_MODEL_CSXL218_NU:		
			pszModel = "CSX-L218-NU";
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	return pszModel;
}

//	Get the brand name based on a Mandolin brand and model
const char *Region::GetMandolinBrandName(LOUD_brand mandolinBrand)
{
	const char *pszBrand = "Undefined";

	switch (mandolinBrand) {
	case LOUD_BRAND_MACKIE:
		pszBrand = "Mackie";
		break;
	case LOUD_BRAND_EAW:
		pszBrand = "EAW";
		break;
	case LOUD_BRAND_MARTIN:
		pszBrand = "Martin";
		break;
	default:
		break;
	}

	return pszBrand;
};

//---------------------------------------------------------------------------
//	Changes are not used in current session, must be rebooted to take effect.
//---------------------------------------------------------------------------
void Region::WriteIdentity(uint8_t mac[6], int32_t nBrand, int32_t nModel, uint16_t hardwareRev, uint32_t uiSerialNumber)
{
	bool bOtpMacUpdated = false;

	// Set defaults
	m_olyIdentity.hardwareRev = OLY_DEFAULT_HARDWARE_VERSION;
	m_olyIdentity.nBrand = OLY_DEFAULT_BRAND;
	m_olyIdentity.nModel = OLY_DEFAULT_MODEL;
	m_olyIdentity.uiSerialNumber = OLY_DEFAULT_SERIAL_NUMBER;

	if ( ValidateMAC(mac) )
	{	// Valid MAC address is received. 
#if ALLOW_WRITING_MAC_TO_OTP_FLASH
        // Try to write this valid MAC address into OTP Flash.
		memcpy(m_olyIdentity.mac, mac, sizeof(m_olyIdentity.mac));
		if ( setMACAddressIntoOTP(m_olyIdentity.mac) )
		{
		    printf("MAC Address %02X:%02X:%02X:%02X:%02X:%02X programmed into OTP Flash\n",
		                m_olyIdentity.mac[0], m_olyIdentity.mac[1], m_olyIdentity.mac[2], m_olyIdentity.mac[3], m_olyIdentity.mac[4], m_olyIdentity.mac[5]);
		    // Also set into g_System_MAC for reporting via Mandolin message system.
		    memcpy( g_System_MAC, m_olyIdentity.mac, sizeof(g_System_MAC) );
		    bOtpMacUpdated = true;
		}
#else
    // Don't allow write to OTP Flash, so we will write to Data Flash instead.
	memcpy(m_olyIdentity.mac, mac, sizeof(m_olyIdentity.mac));  
    printf("MAC Address %02X:%02X:%02X:%02X:%02X:%02X will be written into Data Flash\n",
	                m_olyIdentity.mac[0], m_olyIdentity.mac[1], m_olyIdentity.mac[2], m_olyIdentity.mac[3], m_olyIdentity.mac[4], m_olyIdentity.mac[5]);
  
#endif
	}
	else
	{   // Get the MAC address from Flash in this order : OTP Flash MAC, Data Flash MAC, Default MAC
	    readMACAddressFromOTP(m_olyIdentity.mac);
	}

	//	Assign additional info if available
	if (-1!=nBrand)
		m_olyIdentity.nBrand = nBrand;
	if (-1!=nModel)
		m_olyIdentity.nModel = nModel;
	if (0xFFFF!=hardwareRev)
		m_olyIdentity.hardwareRev = hardwareRev;
	if (0xFFFFFFFF!=uiSerialNumber)
		m_olyIdentity.uiSerialNumber = uiSerialNumber;

	if ((LOUD_BRAND_EAW!=m_olyIdentity.nBrand) && (LOUD_BRAND_MARTIN!=m_olyIdentity.nBrand) && (LOUD_BRAND_MACKIE!=m_olyIdentity.nBrand))
	{
		printf("Invalid brand in identity flash replaced with default!\r\n");
		m_olyIdentity.nBrand = LOUD_BRAND_EAW;
	}

	if ((LOUD_BRAND_EAW==m_olyIdentity.nBrand) && (EAW_MODEL_GENERIC!=m_olyIdentity.nModel) && ((m_olyIdentity.nModel<(int32_t)EAW_MODEL_RSX208L) ||
			(m_olyIdentity.nModel>(int32_t)EAW_MODEL_RSX212L)   ) )
	{
		printf("Invalid EAW model in identity flash replaced with default!\r\n");
		m_olyIdentity.nModel = (int32_t)EAW_MODEL_GENERIC;
	}

	if ((LOUD_BRAND_MARTIN==m_olyIdentity.nBrand) && (MARTIN_MODEL_GENERIC!=m_olyIdentity.nModel)  && ((m_olyIdentity.nModel<(int32_t)MARTIN_MODEL_GENERIC) ||
			(m_olyIdentity.nModel>=((int32_t)MARTIN_MODEL_GENERIC+0x20)) ) )
	{
		printf("Invalid Martin Model in identity flash replaced with default!\r\n");
		m_olyIdentity.nModel = (int32_t)MARTIN_MODEL_GENERIC_NU;
	}
	if ((LOUD_BRAND_MACKIE==m_olyIdentity.nBrand) && (MACKIE_MODEL_GENERIC!=m_olyIdentity.nModel)  && ((m_olyIdentity.nModel<(int32_t)MACKIE_MODEL_GENERIC) ||
			(m_olyIdentity.nModel>=((int32_t)MACKIE_MODEL_MASTER_RIG+0x20)) ) )
	{
		printf("Invalid Mackie Model in identity flash replaced with default!\r\n");
		m_olyIdentity.nModel = (int32_t)MARTIN_MODEL_GENERIC;
	}

	if (0xFFFF==m_olyIdentity.hardwareRev)
	{
		printf("Invalid hardware revision in identity flash replaced with default!\r\n");
		m_olyIdentity.hardwareRev = 0;
	}

	if (0xFFFFFFFF==m_olyIdentity.uiSerialNumber)
	{
		printf("Invalid serial number in identity flash replaced with default!\r\n");
		m_olyIdentity.uiSerialNumber = OLY_DEFAULT_SERIAL_NUMBER;
	}

	printf("Saving Identity Sector, hold on...");
    if ( data_flash_program((uint32_t)OLY_IDENTITY_LOCATION, (uint32_t *)&m_olyIdentity, sizeof(OLY_IDENTITY)/sizeof(uint32_t)) )
    {
	    printf("Success.\r\n");
	    // Update Global variables with new information (for reporting via Mandolin message system)
	    g_systemBrand    = (LOUD_brand)m_olyIdentity.nBrand;
	    g_nSystemModel   = m_olyIdentity.nModel;
	    g_hardwareRev    = m_olyIdentity.hardwareRev;
	    g_uiSerialNumber = m_olyIdentity.uiSerialNumber;
	    if ( bOtpMacUpdated == false )
	    	memcpy( g_System_MAC, m_olyIdentity.mac, sizeof(g_System_MAC) );
    }
}

// **************************************************************
//	CRC Utilities
// **************************************************************
// Called at start of calculation
void Region::Crc16Init(P_REGION_CRC rgnCrc)
{
     // initialize running crc and byte count
	rgnCrc->currentCrc = 0;
}

// Called for each new block in SPI Flash to CRC
void Region::Crc16UpdateFromSpiFlash(P_REGION_CRC rgnCrc, const uint8_t * pSrc, uint32_t uiBytes)
{
    uint32_t crc = rgnCrc->currentCrc;

    uint32_t j;
    uint32_t flashOffset = sizeof(OLY_REGION) + (uint32_t)pSrc; // Start after header, plus the app offset (e.g. 0x4000)
    for ( j = 0; j < uiBytes; ++j )
    {
        uint32_t i;
        uint32_t byte = spi_flash_read_fw_byte(flashOffset + j);
        crc ^= byte << 8;
        for ( i = 0; i < 8; ++i )
        {
            uint32_t temp = crc << 1;
            if (crc & 0x8000)
            {
                temp ^= 0x1021;
            }
            crc = temp;
        }
    }

    rgnCrc->currentCrc = crc;
}

// Called for each new block in APROM Flash to CRC
void Region::Crc16UpdateFromApromFlash(P_REGION_CRC rgnCrc, const uint8_t *pSrc, uint32_t uiBytes)
{
    uint32_t crc = rgnCrc->currentCrc;

    uint32_t j;
    for ( j = 0; j < uiBytes; ++j )
    {
        uint32_t i;
        uint32_t byte = pSrc[j];
        crc ^= byte << 8;
        for ( i = 0; i < 8; ++i )
        {
            uint32_t temp = crc << 1;
            if (crc & 0x8000)
            {
                temp ^= 0x1021;
            }
            crc = temp;
        }
    }

    rgnCrc->currentCrc = crc;
}

//	Called to complete CRC calc
void Region::Crc16Finalize(P_REGION_CRC rgnCrc, uint16_t * pHash)
{
     *pHash = rgnCrc->currentCrc;
}


