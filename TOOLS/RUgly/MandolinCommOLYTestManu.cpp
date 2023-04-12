extern "C" {

#include "LOUD_defines.h"
#include "LOUD_types.h"
#include "mandolin.h"
}

#include "string.h"
#include <stdio.h>

#include "assert.h"

#include "MandolinCommOLYTestManu.h"
#include "OLY_MT_enums.h"

CMandolinCommOLYTestManu::CMandolinCommOLYTestManu()
{

}

CMandolinCommOLYTestManu::~CMandolinCommOLYTestManu()
{
}


// -----------------------------------------------------------------------------------------------------------------
bool CMandolinCommOLYTestManu::CreateTestSetBrandModel(mandolin_message* pMsg, uint8 uiSequence,uint32 uiBrand, uint32 uiModel)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	


	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,TEST_CMD_SET_MODEL);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,uiBrand);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,uiModel);

	pMsg->length = 3;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

// -----------------------------------------------------------------------------------------------------------------
bool CMandolinCommOLYTestManu::CreateTestSetIdentity(mandolin_message* pMsg, uint8 uiSequence,uint8* macaddr, uint32 uiBrand, uint32 uiModel,uint32 uiHardwareRev,uint32 uiSerialNumber)
{
	uint8* pPayload = (uint8*)pMsg->payload;
	uint32 uiValue;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,TEST_CMD_SET_IDENTITY);
	uiValue = ((macaddr[0] & 0x0ff) << 8) + (macaddr[1] & 0x0ff);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,uiValue);
	uiValue = ((macaddr[2] & 0x0ff) << 24) + ((macaddr[3] & 0x0ff) << 16)  + ((macaddr[4] & 0x0ff) << 8) + (macaddr[5] & 0x0ff);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,uiValue);

	MANDOLIN_PAYLOAD_SETUINT32(pPayload,3*4,uiBrand);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,4*4,uiModel);

	uiHardwareRev &= 0x0ffff;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,5*4,uiHardwareRev);

	MANDOLIN_PAYLOAD_SETUINT32(pPayload,6*4,uiSerialNumber);

	pMsg->length = 7;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

// -----------------------------------------------------------------------------------------------------------------

bool CMandolinCommOLYTestManu::CreateTestDSPGetSerialNum(mandolin_message* pMsg, uint8 uiSequence)
{
	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,TEST_CMD_GET_SERIAL_NUMBER);
	pMsg->length = 1;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

bool CMandolinCommOLYTestManu::CreateTestDSPSetSerialNum(mandolin_message* pMsg, uint8 uiSequence, uint32 uiSerial)
{
	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,TEST_CMD_SET_SERIAL_NUMBER);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,uiSerial);
	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

bool CMandolinCommOLYTestManu::CreateTestDSPRestoreDefaults(mandolin_message* pMsg, uint8 uiSequence)
{
	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,TEST_CMD_RESTORE_DEFAULTS);
	pMsg->length = 1;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

// -----------------------------------------------------------------------------------------------------------------
bool CMandolinCommOLYTestManu::CreateTestGetMACAddr(mandolin_message* pMsg, uint8 uiSequence)
{
	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,TEST_CMD_GET_MAC_ADDR);
	pMsg->length = 1;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}
// -----------------------------------------------------------------------------------------------------------------
bool CMandolinCommOLYTestManu::CreateTestDSPSetFuncGenOutput(mandolin_message* pMsg, uint8 uiSequence,int nChannel)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	


	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,TEST_CMD_SET_OUTPUT);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,nChannel);

	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}


bool CMandolinCommOLYTestManu::CreateTestDSPSetFuncGen(mandolin_message* pMsg, uint8 uiSequence,int nType,float fAmplitude, float fFreq)
{

	uint8* pPayload = (uint8*)pMsg->payload;
	intfloat j;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,TEST_CMD_SET_FUNC_GEN);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,nType);
	j.f = fAmplitude;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,j.ui);
	j.f = fFreq;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,3*4,j.ui);

	pMsg->length = 4;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}


// ----------------------------------------------------------------------------------
bool CMandolinCommOLYTestManu::CreateTestDSPSetRMSMeterInput(mandolin_message* pMsg, uint8 uiSequence,int nType)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	

	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,TEST_CMD_SET_INPUT);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,nType);

	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}



// ----------------------------------------------------------------------------------
bool CMandolinCommOLYTestManu::CreateTestDSPGetRMSMeterLevel(mandolin_message* pMsg, uint8 uiSequence)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,TEST_CMD_GET_RMS_METER_LEVEL);

	pMsg->length = 1;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

// ----------------------------------------------------------------------------------
bool CMandolinCommOLYTestManu::CreateTestDSPGetRMSMeterRange(mandolin_message* pMsg, uint8 uiSequence, float fMin, float fMax)
{

	uint8* pPayload = (uint8*)pMsg->payload;
	intfloat j;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,TEST_CMD_GET_RMS_METER_RANGE);
	j.f = fMin;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,j.ui);
	j.f = fMax;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,j.ui);

	pMsg->length = 3;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}


// ----------------------------------------------------------------------------------
bool CMandolinCommOLYTestManu::CreateTestDSPWriteIO(mandolin_message* pMsg, uint8 uiSequence,int nIndex, int nValue)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,TEST_CMD_WRITE_IO);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,nIndex);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,nValue);

	pMsg->length = 3;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

 
// ----------------------------------------------------------------------------------
bool CMandolinCommOLYTestManu::CreateTestDSPReadIO(mandolin_message* pMsg, uint8 uiSequence, int nIndex)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,TEST_CMD_READ_IO);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,nIndex);

	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

// ----------------------------------------------------------------------------------
bool CMandolinCommOLYTestManu::CreateTestDSPReadAmpTemperature(mandolin_message* pMsg, uint8 uiSequence,int nIndex)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,TEST_CMD_READ_AMP_TEMPERATURE);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,nIndex);

	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}


bool  CMandolinCommOLYTestManu::CreateTestValidateSPIFlash(mandolin_message* pMsg, uint8 uiSequence)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,TEST_CMD_VALIDATE_SPI_FLASH);

	pMsg->length = 1;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}
bool  CMandolinCommOLYTestManu::CreateTestGetIMonValue(mandolin_message* pMsg, uint8 uiSequence, int nChannel)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,TEST_CMD_GET_IMON_VALUE);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,nChannel);

	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}
