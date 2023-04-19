/*
 * Region.h
 *
 *  Created on: Jan 28, 2016
 *	  Author: Ted.Wolfe
 *
 *	  Management of Regions for firmware updating
 */
#include "Upgrade.h"
#include "LOUD_defines.h"

extern LOUD_brand g_systemBrand;

extern int32 g_nSystemModel;

extern uint16 g_hardwareRev;

extern uint32 g_uiSerialNumber;

#pragma once

typedef enum regionState {
	usmInvalid = 0,
	usmStarted = 1
} RGN_STATE;

typedef struct regionCrc16Data {
    uint16_t currentCrc;    //!< Current CRC value.
} REGION_CRC, *P_REGION_CRC;

class Region {
protected:
	OLY_BLOCK 	olyBlock;

public:
	Region();
	~Region();

	//	!!!! Kris - This is the API you need to run from the Mandolin message handler
	bool StartUpgrade(P_OLY_REGION pOlyRegion);
	bool UpgradeChunk(unsigned char *pChunk, unsigned int uiOffset, unsigned int uiBytes);
	bool GetUpgradeRemaining(uint32_t &uiTotal, uint32_t &uiRemaining);
	void CancelUpgrade();
	bool EndUpgrade();
	bool SetLaunchType(OLY_REGION_TYPE launchType);
	bool ValidateMAC(uint8_t mac[6]);
	void SetStaticIp(uint32_t uiIp);
	void SetIpSettings(uint32_t uiIp, uint32_t uiGateway, uint32_t uiMask);
	uint32_t GetStaticIp();
	uint32_t GetStaticGateway();
	uint32_t GetStaticMask();
	void WriteIdentity(uint8_t mac[6], int32_t nBrand, int32_t nModel, uint16_t hardwareRev, uint32_t uiSerialNumber);

	//	Stataic methods
	static void SetSystemBrand(LOUD_brand brand);
	static void SetSystemModel(int32 nModel);
	static const char *GetMandolinModelName(LOUD_brand mandolinBrand, int nMandolinModel);
	static const char *GetMandolinBrandName(LOUD_brand mandolinBrand);
	static unsigned char *GetSystemMAC();
	static LOUD_brand GetSystemBrand();
	static int32 GetSystemModel();
	static uint16 GetHardwareRevision();
	static uint32 GetSerialNumber();

private:
	void Load();
	void Save();
	bool Verify();
	void Initialize();
	int FindFirst(OLY_REGION_TYPE type);
	bool ClearRegion(int nRegion);
	bool WriteSpiFlashHeader( OLY_REGION *pRegion );
	bool ClearSpiFlashBootloaderSignal( void );
	bool WriteChunk(uint32_t uiFlashAdddress, uint8_t *pChunk);
	bool VerifyChunk(uint32_t uiFlashAdddress, uint8_t *pChunk);
	bool WriteRegion(int nRegion, P_OLY_REGION pOlyRegion);         // Updates RAM olyBlock region
	bool VerifyRegion(int nRegion);

	void Crc16Init(P_REGION_CRC crc);
	void Crc16UpdateFromSpiFlash(P_REGION_CRC crc, const uint8_t * pSrc, uint32_t uiBytes);
	void Crc16UpdateFromApromFlash(P_REGION_CRC rgnCrc, const uint8_t *pSrc, uint32_t uiBytes);
	void Crc16Finalize(P_REGION_CRC crc, uint16_t * usHash);
	void launchBootloader(void);

private:
	OLY_REGION_TYPE m_defaultType;
	uint32_t m_uiDefaultStart;
	uint32_t m_uiDefaultLength;
	OLY_REGION m_rgnUpgrade;
	RGN_STATE m_UpgradeState;
	int m_nUpgradeRegion;
	unsigned int m_uiUpgradeFilled;
};
