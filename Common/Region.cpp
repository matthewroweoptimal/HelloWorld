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

extern "C" {
#include "oly.h"
#include "LOUD_types.h"
}

#include "flash_params.h"

// Define OLY_BLOCK within Data Flash Region
OLY_BLOCK __attribute__ ((section(".oly_block"))) g_dataFlashOlyBlock =
{
	sizeof(OLY_BLOCK),	// size
	0,					// generation
	OLY_MAGIC_WORD,		// magicWord
	OLY_BLOCK_VERSION,  // version
	0,					// isIpv6
	{OLY_DEFAULT_STATIC_IP, 0},	// staticIp
	{OLY_DEFAULT_GATEWAY, 0},	// gatewayIp
	{OLY_DEFAULT_MASK, 0},	// mask
	OLY_REGION_APPLICATION,	// launchType
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	// reserved[21]
	{					// rgn[0]
		OLY_REGION_APPLICATION,	// type
		{						// version
		 (OLY_FW_VERSION >> 24) & 0xFF,
		 (OLY_FW_VERSION >> 16) & 0xFF,
		 (OLY_FW_VERSION >> 8) & 0xFF,
		 BUILD_VERSION
		},
		OLY_DEFAULT_START_ADDR,	// address
		DATA_FLASH_ADDR,		// length (504kB)
		OLY_DEFAULT_VECTOR_TABLE,// vectorTable
		OLY_DEFAULT_STACK_PTR,	// stackPtr
		0,						// crc
		{0, 0},					// align[2]
		{0, 0, 0, 0, 0, 0, 0, 0, 0}	// reserved[9]
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
	OLY_DEFAULT_BRAND,				// nBrand
	OLY_DEFAULT_MODEL,				// nModel
	OLY_DEFAULT_SERIAL_NUMBER 		// uiSerialNumber
};


//#define FORCE_STATIC_IP 1

uint8_t g_System_MAC[6] = {
		OLY_DEFAULT_MAC_ADDR0,
		OLY_DEFAULT_MAC_ADDR1,
		OLY_DEFAULT_MAC_ADDR2,
		OLY_DEFAULT_MAC_ADDR3,
		OLY_DEFAULT_MAC_ADDR4,
		OLY_DEFAULT_MAC_ADDR5 };

LOUD_brand g_systemBrand = OLY_DEFAULT_BRAND;

int32 g_nSystemModel = OLY_DEFAULT_MODEL;

uint16 g_hardwareRev = OLY_DEFAULT_HARDWARE_VERSION;

uint32 g_uiSerialNumber = OLY_DEFAULT_SERIAL_NUMBER;

Region::Region()
{
	uint8_t systemMac[6];
	int32 nBrand;
	int32 nModel;
	const char *pszBrand;
	const char *pszModel;

	m_defaultType = OLY_REGION_APPLICATION;
	m_uiDefaultStart = OLY_DEFAULT_VECTOR_TABLE;
	m_uiDefaultLength = (unsigned int)OLY_BLOCK_LOCATION - m_uiDefaultStart;

	memcpy(systemMac, OLY_IDENTITY_LOCATION->mac, sizeof(systemMac));
	printf("Checking MAC Address at 0x%08X - %02x:%02x:%02x:%02x:%02x:%02x\r\n",
		(unsigned int)&OLY_IDENTITY_LOCATION->mac,
		systemMac[0],
		systemMac[1],
		systemMac[2],
		systemMac[3],
		systemMac[4],
		systemMac[5]);

	//	Needs to  move inside Config object
	if (!ValidateMAC((unsigned char *)OLY_IDENTITY_LOCATION->mac))
	{
		/* MAC not set, load defaults */
		printf("MAC Address not set or invalid range, loading default %02x:%02x:%02x:%02x:%02x:%02x!\r\n",
				g_System_MAC[0],
				g_System_MAC[1],
				g_System_MAC[2],
				g_System_MAC[3],
				g_System_MAC[4],
				g_System_MAC[5]);
	}
	else
	{
		memcpy(g_System_MAC, (unsigned char *)OLY_IDENTITY_LOCATION->mac, sizeof(g_System_MAC));
		printf("MAC Address is valid\r\n");
	}

	//	Evaluate Brand
	nBrand = OLY_IDENTITY_LOCATION->nBrand;
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
	nModel = OLY_IDENTITY_LOCATION->nModel;
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
		case MARTIN_MODEL_CDDL8:
			pszModel = "MARTIN_MODEL_CDDL8.";
			break;
		case MARTIN_MODEL_CDDL12:
			pszModel = "MARTIN_MODEL_CDDL12.";
			break;
		case MARTIN_MODEL_CDDL15:
			pszModel = "MARTIN_MODEL_CDDL15.";
			break;
		case MARTIN_MODEL_CSXL118:
			pszModel = "MARTIN_MODEL_CSXL118.";
			break;
		case MARTIN_MODEL_CSXL218:
			pszModel = "MARTIN_MODEL_CSXL218.";
			break;
		default:
			if ((nModel<MARTIN_MODEL_GENERIC) || (nModel>=0x20))
			{
				pszModel = "INVALID! - Substituting MARTIN_MODEL_GENERIC!";
				nModel = MARTIN_MODEL_GENERIC;
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


	g_hardwareRev = OLY_IDENTITY_LOCATION->hardwareRev;
	if ((unsigned short)0xFFFF==(unsigned short)g_hardwareRev)
	{
		g_hardwareRev = OLY_DEFAULT_HARDWARE_VERSION;
		printf("Invalid hardware revision, substituting default!\r\n");
	}
	printf("Identifying as hardware revision %d\r\n", (unsigned int)g_hardwareRev);

	g_uiSerialNumber = OLY_IDENTITY_LOCATION->uiSerialNumber;
	if (0xFFFFFFFF==g_uiSerialNumber)
	{
		g_uiSerialNumber = OLY_DEFAULT_SERIAL_NUMBER;
		printf("Invalid Serial Number, substituting default!\r\n");
	}
	printf("Identifying as serial number %d(0x%08X)\r\n", g_uiSerialNumber, g_uiSerialNumber);

	m_UpgradeState = usmInvalid;

	Load();

	if (!Verify())
	{   // Initialise to defaults and save Region to Data Flash.
		Initialize();
		Save();
	}

	//	if you need to force to static IP - do it here
#ifdef	FORCE_STATIC_IP
	olyBlock.staticIp.ipv4.ip = IPADDR(192,168,1,50);
	olyBlock.gatewayIp.ipv4.ip = IPADDR(192,168,1,0);
	olyBlock.mask.ipv4.ip = IPADDR(255,255,255,0);
#endif	//	FORCE_STATIC_IP

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
	olyBlock.staticIp.ipv4.ip = OLY_DEFAULT_STATIC_IP;
	olyBlock.gatewayIp.ipv4.ip = OLY_DEFAULT_GATEWAY;
	olyBlock.mask.ipv4.ip = OLY_DEFAULT_MASK;

	olyBlock.rgn[0].type = m_defaultType;
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
}

void Region::SetStaticIp(uint32_t uiIp)
{
	olyBlock.staticIp.ipv4.ip = uiIp;
	Save();
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
}

//---------------------------------------------------------------------------
// Save region data to DATA Flash
//---------------------------------------------------------------------------
void Region::Save()
{
	printf("Saving Region Block to Data Flash.\r\n");
	uint32_t uiFlashAdddress = (unsigned int)OLY_BLOCK_LOCATION;

	olyBlock.generation++;
	
	data_flash_program( uiFlashAdddress, (uint32_t *)&olyBlock, sizeof(OLY_BLOCK)/sizeof(uint32_t) );
//	write_sector((unsigned int)uiFlashAdddress, (uint8_t*)&olyBlock);
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
// Program a chunk of SPI flash firmware area 
//	 (I am told we currently can't write less than a full sector, so length ignored)
//------------------------------------------------------------------------------------
bool Region::WriteChunk( uint32_t uiFlashOffset, uint8_t *pChunk )
{
//	uiResult = write_sector( uiFlashAdddress, pChunk );
	printf("\tFW: Write Chunk, offset 0x%08X (%d kB)\n", uiFlashOffset, uiFlashOffset/1024);
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

//	Called by Mandolin Process when it received the upgrade file header
bool Region::StartUpgrade(P_OLY_REGION pOlyRegion)
{
	int nRegion;

	printf("Starting Upgrade Process for region type %d.\r\n", (int)pOlyRegion->type);

	// 	CDDLive : We are upgrading image in SPI Flash Region, so OK to accept APP type image, so check below not necessary.
#if 0


	//	Verify that region is compatible with upgrade type
	//	Only allow one of eeach type for now
	if (m_defaultType==pOlyRegion->type)
	{
		printf("Aborting trying to upgrade currently running application type %d.\r\n", (int)pOlyRegion->type);
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
	return(true);
}

//	Called by Mandolin Process when it received the upgrade file header
bool Region::UpgradeChunk(unsigned char *pChunk, unsigned int uiOffset, unsigned int uiBytes)
{
	unsigned char *pChar = (unsigned char *)(m_rgnUpgrade.address + uiOffset);

	if (usmStarted != m_UpgradeState)
	{
		printf("UpgradeChunk rejected because not in upgrade mode!\r\n");
		return(false);
	}

	if ((uiBytes + uiOffset) > (m_rgnUpgrade.length + sizeof(OLY_REGION)))
	{
		printf("UpgradeChunk rejected because of too many bytes received!\r\n");
		return(false);
	}

#if 0	// CDDLive : This check not necessary since we are now upgrading to SPI flash FW region
	if (((m_rgnUpgrade.address + uiOffset) >= 0) && ((m_rgnUpgrade.address + uiOffset) < FW_UPGRADE_CHUNK_SIZE))
	{
		printf("UpgradeChunk rejected because overwriting first sector!\r\n");
		return(false);
	}
#endif

	if (uiOffset &(FW_UPGRADE_CHUNK_SIZE-1))
	{
		printf("UpgradeChunk rejected because start of write not on sector boundary!\r\n");
		return(false);
	}

	printf("UpgradeChunk writing %d bytes to offset 0x%08X...\r\n", uiBytes, m_rgnUpgrade.address + uiOffset);

	if ( VerifyChunk(m_rgnUpgrade.address + uiOffset, pChunk) )
		printf("\tSkipping.\r\n");
	else if ( !WriteChunk(m_rgnUpgrade.address + uiOffset, pChunk) )
	{
		printf("\tFailed!\r\n");
		return(false);
	}
	else
		printf("\tDone.\r\n");

	if (!VerifyChunk(m_rgnUpgrade.address + uiOffset, pChunk))
	{
		printf("\tVerify failed @ offset 0x%08X\r\n", m_rgnUpgrade.address + uiOffset);
		return(false);
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
}

//	Called to finalize an upgrade and verify it is good
bool Region::EndUpgrade()
{
	unsigned char *pCrcPtr = (unsigned char *)m_rgnUpgrade.address;
	unsigned short crcCalc;
	REGION_CRC rgnCrc;

	printf("Verifying Upgrade...\r\n");

	//	In upgrade?
	if (usmStarted!=m_UpgradeState)
	{
		printf("EndUpgrade failed because not currently upgrading!\r\n");
		return(false);
	}

	//	Finished?
	if (m_uiUpgradeFilled != (m_rgnUpgrade.length + sizeof(OLY_REGION)))
	{
		printf("EndUpgrade failed because not enough bytes have been written!\r\n");
		return(false);
	}

	//	Verify CRC
	Crc16Init(&rgnCrc);
	Crc16UpdateFromSpiFlash(&rgnCrc, pCrcPtr, m_rgnUpgrade.length);
	Crc16Finalize(&rgnCrc, &crcCalc);
	if (m_rgnUpgrade.crc != crcCalc)
	{
		printf("EndUpgrade failed because CRC didn't match Adr=0x%08X, Len=0x%08X, C1=0x%08X, C2=0x%08X!\r\n",
			m_rgnUpgrade.address, m_rgnUpgrade.length, m_rgnUpgrade.crc, crcCalc);
		return(false);
	}
	printf("EndUpgrade CRC matched Adr=0x%08X, Len=0x%08X, C1=0x%08X.\r\n",
		m_rgnUpgrade.address, m_rgnUpgrade.length, m_rgnUpgrade.crc);

	//	Write new region to 'olyBlock', then Save that to Data Flash Region area.
	WriteRegion(m_nUpgradeRegion, &m_rgnUpgrade);
	Save();

	printf("Upgrade complete.\n");
	m_UpgradeState = usmInvalid;	// Clear upgrade state in case called again before reboot
	
	// TODO : Launch into BOOTLOADER which will detect image in SPI Flash and program it into APROM Flash.
	
	return(true);
}

//	returns true if launch type needed to be changed
bool Region::SetLaunchType(OLY_REGION_TYPE launchType)
{
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
	return(true);
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
		case MARTIN_MODEL_CDDL12:
			pszModel = "CDD-L12";
			break;
		case MARTIN_MODEL_CDDL15:
			pszModel = "CDD-L15";
			break;
		case MARTIN_MODEL_CSXL118:
			pszModel = "CSX-L118";
			break;
		case MARTIN_MODEL_CSXL218:
			pszModel = "CSX-L218";
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
	uint8_t *pSectorZero = new uint8_t[P_SECTOR_SIZE];
	if (pSectorZero)
	{
		memcpy(pSectorZero, (uint8_t *)0, P_SECTOR_SIZE);
		P_OLY_IDENTITY pIdentity = (P_OLY_IDENTITY)(pSectorZero+OLY_IDENTITY_OFFSET);
		if (ValidateMAC(mac))
			memcpy(pIdentity->mac, mac, sizeof(pIdentity->mac));

		//	Assign additional info if available
		if (-1!=nBrand)
			pIdentity->nBrand = nBrand;
		if (-1!=nModel)
			pIdentity->nModel = nModel;
		if (0xFFFF!=hardwareRev)
			pIdentity->hardwareRev = hardwareRev;
		if (0xFFFFFFFF!=uiSerialNumber)
			pIdentity->uiSerialNumber = uiSerialNumber;

		//	Verify contents are valid values
		if (!ValidateMAC(pIdentity->mac))
		{
			printf("Invalid MAC in identity flash replaced with default!\r\n");
			pIdentity->mac[0] = OLY_DEFAULT_MAC_ADDR0;
			pIdentity->mac[1] = OLY_DEFAULT_MAC_ADDR1;
			pIdentity->mac[2] = OLY_DEFAULT_MAC_ADDR2;
			pIdentity->mac[3] = OLY_DEFAULT_MAC_ADDR3;
			pIdentity->mac[4] = OLY_DEFAULT_MAC_ADDR4;
			pIdentity->mac[5] = OLY_DEFAULT_MAC_ADDR5;
		}

		if ((LOUD_BRAND_EAW!=pIdentity->nBrand) && (LOUD_BRAND_MARTIN!=pIdentity->nBrand) && (LOUD_BRAND_MACKIE!=pIdentity->nBrand))
		{
			printf("Invalid brand in identity flash replaced with default!\r\n");
			pIdentity->nBrand = LOUD_BRAND_EAW;
		}

		if ((LOUD_BRAND_EAW==pIdentity->nBrand) && (EAW_MODEL_GENERIC!=pIdentity->nModel) && ((pIdentity->nModel<(int32_t)EAW_MODEL_RSX208L) || (pIdentity->nModel>(int32_t)EAW_MODEL_RSX212L)   ) )
		{
			printf("Invalid EAW model in identity flash replaced with default!\r\n");
			pIdentity->nModel = (int32_t)EAW_MODEL_GENERIC;
		}

		if ((LOUD_BRAND_MARTIN==pIdentity->nBrand) && (MARTIN_MODEL_GENERIC!=pIdentity->nModel)  && ((pIdentity->nModel<(int32_t)MARTIN_MODEL_GENERIC) || (pIdentity->nModel>=((int32_t)MARTIN_MODEL_GENERIC+0x20)) ) )
		{
			printf("Invalid Martin Model in identity flash replaced with default!\r\n");
			pIdentity->nModel = (int32_t)MARTIN_MODEL_GENERIC;
		}
		if ((LOUD_BRAND_MACKIE==pIdentity->nBrand) && (MACKIE_MODEL_GENERIC!=pIdentity->nModel)  && ((pIdentity->nModel<(int32_t)MACKIE_MODEL_GENERIC) || (pIdentity->nModel>=((int32_t)MACKIE_MODEL_MASTER_RIG+0x20)) ) )
		{
			printf("Invalid Mackie Model in identity flash replaced with default!\r\n");
			pIdentity->nModel = (int32_t)MARTIN_MODEL_GENERIC;
		}

		if (0xFFFF==pIdentity->hardwareRev)
		{
			printf("Invalid hardware revision in identity flash replaced with default!\r\n");
			pIdentity->hardwareRev = 0;
		}

		if (0xFFFFFFFF==pIdentity->uiSerialNumber)
		{
			printf("Invalid serial number in identity flash replaced with default!\r\n");
			pIdentity->hardwareRev = 0;
		}

		printf("Saving Identity Sector, hold on...");
        if ( data_flash_program((uint32_t)OLY_IDENTITY_LOCATION, (uint32_t *)pIdentity, sizeof(OLY_IDENTITY)/sizeof(uint32_t)) )
        {
		    printf("Success.\r\n");
	    }
		delete pSectorZero;
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
    uint32_t flashOffset = sizeof(OLY_REGION) + (uint32_t)pSrc;
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
        uint32_t byte = *pSrc;
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


