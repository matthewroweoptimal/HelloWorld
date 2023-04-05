// MandolinCommOLyTestMAnu.h

#ifndef	MANDOLINCOMMOLYTESTMANU_H
#define MANDOLINCOMMOLYTESTMANU_H

#include "LOUD_types.h"
extern "C"
{
#include "mandolin.h"
}
//extern "C" uint32 tick_count;

#define MANDOLIN_PAYLOAD_GETUINT32(in_ptr,offset)			((uint32)(((uint32)in_ptr[offset+3]&0x0ff)<<24) + (uint32)((in_ptr[offset+2]&0x0ff)<<16) + (uint32)((in_ptr[offset+1]&0x0ff)<<8) + (uint32)((in_ptr[offset+0]&0x0ff)<<0))
#define MANDOLIN_PAYLOAD_SETUINT32(payload,offset,value)     payload[offset]=(uint8)(value & 0x0ff); payload[offset+1]= (uint8)((value>>8) & 0x0ff); payload[offset+2]= (uint8)((value>>16) & 0x0ff); payload[offset+3]= (uint8)((value>>24) & 0x0ff)
//#define MANDOLIN_PAYLOAD_GETUINT32(in_ptr,offset)	   ((uint32)(((uint32)in_ptr[offset+0]&0x0ff)<<24) + (uint32)((in_ptr[offset+1]&0x0ff)<<16) + (uint32)((in_ptr[offset+2]&0x0ff)<<8) + (uint32)((in_ptr[offset+3]&0x0ff)<<0))
//#define MANDOLIN_PAYLOAD_SETUINT32(payload,offset,value)     payload[offset+3]=(uint8)(value & 0x0ff); payload[offset+2]= (uint8)((value>>8) & 0x0ff); payload[offset+1]= (uint8)((value>>16) & 0x0ff); payload[offset+0]= (uint8)((value>>24) & 0x0ff)


// CConmonMandolinDlg dialog
class CMandolinCommOLYTestManu
{
// Construction
public:
	CMandolinCommOLYTestManu();
	~CMandolinCommOLYTestManu();

// Implementation
//protected:

public:
	//bool InitRxFifo(int nSize);
	//bool InitTxFifo(int nSize);

	//int ConvertMessage2Text(char* pcBuffer, int nMaxLength, char* pcDevice, uint8* pMsgData, uint16 uiLength);
	bool CreateTestSetIdentity(mandolin_message* pMsg, uint8 uiSequence,uint8* macaddr, uint32 uiBrand  = 0xFFFFFFFF, uint32 uiModel = 0xFFFFFFFF,uint32 uiHardwareRev  = 0x0FFFF,uint32 uiSerialNumber  = 0xFFFFFFFF);
	bool CreateTestSetBrandModel(mandolin_message* pMsg, uint8 uiSequence,uint32 uiBrand, uint32 uiModel);

	bool CreateTestDSPGetSerialNum(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateTestDSPSetSerialNum(mandolin_message* pMsg, uint8 uiSequence, uint32 uiSerial);
	bool CreateTestDSPRestoreDefaults(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateTestGetMACAddr(mandolin_message* pMsg, uint8 uiSequence);

	bool CreateTestDSPSetFuncGenOutput(mandolin_message* pMsg, uint8 uiSequence,int nChannel);
	bool CreateTestDSPSetFuncGen(mandolin_message* pMsg, uint8 uiSequence,int nType,float fAmplitude, float fFreq);
	bool CreateTestDSPSetRMSMeterInput(mandolin_message* pMsg, uint8 uiSequence,int nType);
	bool CreateTestDSPGetRMSMeterLevel(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateTestDSPGetRMSMeterRange(mandolin_message* pMsg, uint8 uiSequence, float fMin, float fMax);
	bool CreateTestDSPWriteIO(mandolin_message* pMsg, uint8 uiSequence,int nIndex, int nValue);
	bool CreateTestDSPReadIO(mandolin_message* pMsg, uint8 uiSequence, int nIndex);
	bool CreateTestDSPReadAmpTemperature(mandolin_message* pMsg, uint8 uiSequence,int nIndex);
	bool CreateTestValidateSPIFlash(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateTestGetIMonValue(mandolin_message* pMsg, uint8 uiSequence, int nChannel);
	



 
};

#endif



