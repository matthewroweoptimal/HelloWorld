// MandolinComm.h

//#pragma once
//using namespace System;

//namespace MandolinComm {
#ifndef	MANDOLINCOMM_H
#define MANDOLINCOMM_H

#include "LOUD_types.h"
extern "C"
{
#include "mandolin.h"
#include "Upgrade.h"
}
extern "C" uint32 tick_count;

#define MANDOLIN_PAYLOAD_GETUINT32(in_ptr,offset)	   ((uint32)(((uint32)in_ptr[offset+3]&0x0ff)<<24) + (uint32)((in_ptr[offset+2]&0x0ff)<<16) + (uint32)((in_ptr[offset+1]&0x0ff)<<8) + (uint32)((in_ptr[offset+0]&0x0ff)<<0))
#define MANDOLIN_PAYLOAD_SETUINT32(payload,offset,value)     payload[offset]=(uint8)(value & 0x0ff); payload[offset+1]= (uint8)((value>>8) & 0x0ff); payload[offset+2]= (uint8)((value>>16) & 0x0ff); payload[offset+3]= (uint8)((value>>24) & 0x0ff)
//#define MANDOLIN_PAYLOAD_GETUINT32(in_ptr,offset)	   ((uint32)(((uint32)in_ptr[offset+0]&0x0ff)<<24) + (uint32)((in_ptr[offset+1]&0x0ff)<<16) + (uint32)((in_ptr[offset+2]&0x0ff)<<8) + (uint32)((in_ptr[offset+3]&0x0ff)<<0))
//#define MANDOLIN_PAYLOAD_SETUINT32(payload,offset,value)     payload[offset+3]=(uint8)(value & 0x0ff); payload[offset+2]= (uint8)((value>>8) & 0x0ff); payload[offset+1]= (uint8)((value>>16) & 0x0ff); payload[offset+0]= (uint8)((value>>24) & 0x0ff)


// CConmonMandolinDlg dialog
class CMandolinComm
{
// Construction
public:
	CMandolinComm();
	~CMandolinComm();

// Implementation
//protected:
	void (*m_funcPing)(mandolin_message* pMsg, void* pData);
	void* m_vpPingUserData;

	void (*m_funcSetApplicationParameter)(mandolin_message* pMsg,uint32 uiTarget, uint32* uiPIDs, uint32* uiValues,int nNumofParams, void* pData);
	void* m_vpSetApplicationParameterUserData;
	void (*m_funcFileOpenResponse) (mandolin_message* pMsg,bool success, int fileIdOrFalureReason, void* pData);
	void* m_vpFileOpenResponseUserData;
	void (*m_funcProcessFileResponse) (mandolin_message* pMsg, int status, int percentage, void* pData);
	void* m_vpProcessFileResponseUserData;
	void (*m_funcGetFileInfoResponse) (mandolin_message* pMsg, int status, int value, int fileSize, void* pData);
	void* m_vpGetFileInfoResponseUserData;
	void (*m_funcGenericResponse) (mandolin_message* pMsg, void* pData);
	void* m_vpGenericResponseUserData;
	void (*m_funcSoftwareInfoResponse) (mandolin_message* pMsg,  uint32* uiIDs, uint32* uiBuilds,int nNumofIDs, void* pData);
	void* m_vpSoftwareInfoResponseUserData;
	void (*m_funcFileGetResponse) (mandolin_message* pMsg, int fileID, int fileSize, int fileOffset, int bytesReceived, char* pFileData, void* pData); 
	void* m_vpFileGetResponseUserData;
	void (*m_funcDiagnosticFaultResponse)(mandolin_message* pMsg, uint32* pValues, int nMsgLength, void* pData);
	void* m_vpDiagnosticFaultResponseUserData;
	void (*m_funcTiltSensorResponse)(mandolin_message* pMsg, float xTiltValue, float yTiltValue, void* pData); 
	void* m_vpTiltSensorResponseUserData;
	void (*m_funcMicSweepResponse)(mandolin_message* pMsg, int status, int percent, uint8 channelProgress[22][4], void* pData);
	void* m_vpMicSweepResponseUserData;

	//mandolin_fifo fifoTx;
	//mandolin_fifo fifoRx;


public:
	//bool InitRxFifo(int nSize);
	//bool InitTxFifo(int nSize);

	int ConvertMessage2Text(char* pcBuffer, int nMaxLength, char* pcDevice, uint8* pMsgData, uint16 uiLength);

	mandolin_message mInMsg;
	uint8 mInMsgPayload[MANDOLIN_MAX_PAYLOAD_DATA_WORDS*MANDOLIN_BYTES_PER_WORD];

	bool CheckIfHasSubId(int nMsgId);
	bool CreateReplyAck(mandolin_message* pMsg, mandolin_message* pMsgSrc);
	bool CreateReplyWithPayload(mandolin_message* pMsg,  mandolin_message* pMsgSrc, uint8* pPayloadSrc, int nWordLengthSrc);

	int  RxMandolinBytes(uint8* bytes, uint16  nLength);
	bool DecodeGetParameterReply(mandolin_message* pInMsg, uint32* nTarget,uint32* uiPIDs,uint32* uiValues,int* nNumofParams);
	bool DecodeSetApplicationParameter( mandolin_message* pInMsg, uint32* nTarget,uint32* uiPIDs,uint32* uiValues,int* nNumofParams);
	bool DecodeSoftwareInfoResponse( mandolin_message* pInMsg, uint32* uiID, uint32* uiBuild, int* nNumofIDs);

	void ProcessMessage(mandolin_message* pMsg);

	// handler
	void SetHandlerPing(void (*func)(mandolin_message* pMsg, void* pData), void* pUserData);
	void SetHandlerGenericResponse(void (*func)(mandolin_message* pMsg, void* pData), void* pUserData);
	void SetHandlerFileGetResponse(void (*func)(mandolin_message* pMsg, int fileID, int fileSize, int fileOffset, int  bytesReceived, char *FileData, void*), void* pUserData);
	void SetHandlerSetApplicationParameter(void (*func)(mandolin_message* pMsg,uint32 uiTarget, uint32* uiPIDs, uint32* uiValues,int nNumofParams, void* pData),void* pUserData);
	void SetHandlerFileOpenResponse(void (*func)(mandolin_message* pMsg,bool success, int fileIdOrFalureReason, void* pData), void *pUserData);
	void SetHandlerProcessFileResponse(void (*func)(mandolin_message* pMsg, int status, int percentage, void* pData), void *pUserData);
	void SetHandlerGetFileInfoResponse(void (*func)(mandolin_message* pMsg, int status, int value, int fileSize, void* pData), void *pUserData);
	void SetHandlerSoftwareInfoResponse(void (*func)(mandolin_message* pMsg,  uint32* uiIDs, uint32* uiBuilds,int nNumofIDs, void* pData), void* pUserData);
	void SetHandlerDiagnosticFaultResponse(void (*func)(mandolin_message* pMsg, uint32* pValues, int nMsgLength, void* pData), void* pUserData);
	void SetHandlerTiltSensorResponse(void (*func)(mandolin_message* pMsg, float xTiltValue, float yTiltValue, void* pData), void *pUserData);
	void SetHandlerMicSweepResponse(void (*func)(mandolin_message* pMsg, int status, int percent, uint8 channelProgress[22][4], void* pData), void *pUserData);

	// Wrap msg
	bool CMWrapMandolinMsg(const mandolin_message* pMsg, uint32 srcID[2], uint8* buffer, uint32* len);

	// Create functions
	bool CreatePingMsg(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateParameterEditSet(mandolin_message* pMsg, uint8 uiSequence,uint32 nTarget, uint32 uiPID[], intfloat ifParamValue[], uint32 nNumOfParams,bool bNoAck, int nInstance=0);
	bool CreateParameterEditSetNeighborhood(mandolin_message* pMsg, uint8 uiSequence,uint32 nTarget, uint32 uiStartPID, intfloat ifParamValue[], uint32 nNumOfParams,bool bNoAck, int nInstance=0);
	bool CMandolinComm::CreateParameterEditGetMulti(mandolin_message* pMsg, uint8 uiSequence,uint32 nTarget, uint32 uiPID[], uint32 nNumofParams, int nInstance=0);
	bool CreateParameterEditGetNeighborhood(mandolin_message* pMsg, uint8 uiSequence,uint32 nTarget, uint32 uiStartPID, uint32 nNumofParams, int nInstance=0);
	bool CreateParameterEditGetList(mandolin_message* pMsg, uint8 uiSequence,uint32 nTarget, uint32 uiListID);
	bool CreateSnapshotStore(mandolin_message* pMsg, uint8 uiSequence, int nSnapshot );
	bool CreateSnapshotRecall(mandolin_message* pMsg, uint8 uiSequence, int nSnapshot);
	bool CreateNested(mandolin_message* pMsg, uint8 uiSequence, mandolin_message* pMsgSrc, int nNestedDeviceId);
	bool CreateMakeFactory(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateGetHardwareInfo(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateGetSoftwareInfo(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateGetMaxMessageSize(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateReboot(mandolin_message* pMsg, uint8 uiSequence, uint32 uiRebootCode);
	bool CreateRebootBrooklyn(mandolin_message* pMsg, uint8 uiSequence, uint32 uiRebootCode);
	bool CreateRebootDSP1(mandolin_message* pMsg, uint8 uiSequence, uint32 uiRebootCode);
	bool CreateRebootDSP2(mandolin_message* pMsg, uint8 uiSequence, uint32 uiRebootCode);
	bool CreateUpdateStart(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateUpdateEnd(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateUpdateProcess(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateLock(mandolin_message* pMsg, uint8 uiSequence, uint32 uiLockReason);
	bool CreateUnLock(mandolin_message* pMsg, uint8 uiSequence, uint32 uiUnLockReason);
	bool CreateIdentify(mandolin_message* pMsg, uint8 uiSequence, int nIndentifyPeriodMSec);
	bool CreateSyncState(mandolin_message* pMsg, uint8 uiSequence );
	bool CreateFileOpen(mandolin_message* pMsg, uint8 uiSequence, char* pcFileName, uint32 uiFileType, bool bWrite );
	bool CreateFileOpenCDDLive(mandolin_message* pMsg, uint8 uiSequence, OLY_REGION* pRegion);
	bool CreateFileClose(mandolin_message* pMsg, uint8 uiSequence,  uint32 uiFileId );
	bool CreateFileDelete(mandolin_message* pMsg, uint8 uiSequence,  uint32 uiFileId );
	bool CreateFilePost(mandolin_message* pMsg, uint8 uiSequence,  uint32 uiFileId, uint32 uiFileByteSize, uint32 uiFileBytePos, uint32 uiByteLength, uint8* pData );
	bool CreateFileGet(mandolin_message* pMsg, uint8 uiSequence,  uint32 uiFileId, uint32 uiFileByteSize, uint32 uiFileBytePos, uint32 uiByteLength );
	bool CreateFileInfo(mandolin_message* pMsg, uint8 uiSequence,  uint32 uiFileId );
	bool CreateSubscribeApplicationParameter(mandolin_message* pMsg, uint8 uiSequence,  uint32 uiRateMSec, uint32 uiListId, uint32 nTarget, bool bNoAck = false);
	bool CreateParameterList(mandolin_message* pMsg, uint8 uiSequence, int nListId, uint32 uiPIDs[], uint32 uiNumofParams );
	bool CreateGetBufferCRC(mandolin_message* pMsg, uint8 uiSequence);

	bool CreateConnect(mandolin_message* pMsg, uint8 uiSequence, uint32 uiConnectType);
	bool CreateDisconnect(mandolin_message* pMsg, uint8 uiSequence, uint32 uiDisconnectReason);

	bool CreateGeneric(mandolin_message* pMsg, uint8 uiSequence, uint8 uiMessageID, uint32 nNumPayload, uint32 uiPayload[]);


	bool CreateMicSweepCalibrateStart(mandolin_message* pMsg, uint8 uiSequence, uint32 uiChannel, float fLevel, int nBurstLength);
	bool CreateMicSweepCalibrateProgress(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateMicSweepCalibrateStop(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateMicSweepSingleStart(mandolin_message* pMsg, uint8 uiSequence, uint32 uiChannel, float fLevel, int nBurstLength);
	bool CreateMicSweepSingleProgress(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateMicSweepSingleStop(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateMicSweepDSPStart(mandolin_message* pMsg, uint8 uiSequence, uint32 uiChannel, float fLevel, int nBurstLength);
	bool CreateMicSweepDSPProgress(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateMicSweepDSPStop(mandolin_message* pMsg, uint8 uiSequence);

	
	bool CreateIRDANeighborListReset(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateIRDANeighborListGet(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateIRDANeighborTransmitHardwareInfo(mandolin_message* pMsg, uint8 uiSequence, int nPos, uint32 uiStartTime = 0, uint32 uiEndTime = 0);
	bool CreateIRDANeighborRequestHardwareInfo(mandolin_message* pMsg, uint8 uiSequence, int nPos, uint32 uiStartTime = 0, uint32 uiEndTime = 0);
	bool CreateIRDANeighborInterrogate(mandolin_message* pMsg, uint8 uiSequence, int nPos, uint32 uiHardwareInfoHi, uint32 uiHardwareInfoLo);
	bool CreateIRDASendEcho(mandolin_message* pMsg, uint8 uiSequence, int nPos, uint8* puiByteValues, uint32 uiByteLength);
	bool CreateIRDAEcho(mandolin_message* pMsg, uint8 uiSequence, uint8* puiByteValues, uint32 uiByteLength);
	bool CreateTestGovnIRDAInternal(mandolin_message* pMsg, uint8 uiSequence, int nIndex);

	bool CreateDiagnosticFaultMsg(mandolin_message *pMsg, uint8 uiSequence, uint8 channel);

	bool CreateTimeSetMsg(mandolin_message *pMsg, uint8 uiSequence, uint16 uiYear, uint8 uiMonth, uint8 uiDay, uint8 uiHour, uint8 uiMin, uint8 uiSec);
	bool CreateTimeGetMsg(mandolin_message *pMsg, uint8 uiSequence);

	bool CreateTiltSensorGetMsg(mandolin_message *pMsg, uint8 uiSequence);
	bool CreateTiltSensorCalibrateMsg(mandolin_message *pMsg, uint8 uiSequence, uint32 uiCalibratePos);
	bool CreateTiltSensorSetMsg(mandolin_message *pMsg, uint8 uiSequence, float fTiltX, float fTiltY, float fTiltZ);

	bool CreateGetHardwareInfoDetailsDSP(mandolin_message *pMsg, uint8 uiSequence);
	bool CreateGetHardwareInfoDetailsGov(mandolin_message *pMsg, uint8 uiSequence);

	bool CreateSetModelType(mandolin_message *pMsg, uint8 uiSequence, uint32 uiModelType);
	bool CreateUpdateFromLocalFile(mandolin_message *pMsg, uint8 uiSequence, char* pcFileName, uint32 uiFileFlags);


	bool CreateTestDSPSetFuncGenOutput(mandolin_message* pMsg, uint8 uiSequence,int nChannel);
	bool CreateTestDSPSetFuncGen(mandolin_message* pMsg, uint8 uiSequence,int nType,float fAmplitude, float fFreq);
	bool CreateTestDSPSetRMSMeterInput(mandolin_message* pMsg, uint8 uiSequence,int nType);
	bool CreateTestDSPGetRMSMeterLevel(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateTestDSPGetRMSMeterRange(mandolin_message* pMsg, uint8 uiSequence, float fMin, float fMax);
	bool CreateTestDSPWriteIO(mandolin_message* pMsg, uint8 uiSequence,int nIndex, int nValue);
	bool CreateTestDSPReadIO(mandolin_message* pMsg, uint8 uiSequence, int nIndex);
	bool CreateTestDSPReadAmpTemperature(mandolin_message* pMsg, uint8 uiSequence,int nIndex);
	bool CreateTestDSPReadAmpImpedance(mandolin_message* pMsg, uint8 uiSequence,int nIndex);
	bool CreateTestDSPReadSDRAMTestStatus(mandolin_message* pMsg, uint8 uiSequence);


	bool CreateTestGovnKernel(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateTestGovnRAM(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateTestGovnSDCard(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateTestGovnNANDFlash(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateTestGovnRTC(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateTestGovnDebugUART(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateTestGovnIRDA(mandolin_message* pMsg, uint8 uiSequence,int nIndex);
	bool CreateTestGovnTemperatureDye(mandolin_message* pMsg, uint8 uiSequence,int nTemp, int nTolerance);
	bool CreateTestGovnTemperatureOnBoard(mandolin_message* pMsg, uint8 uiSequence, int nTemp, int nTolerance);
	bool CreateTestGovnReadFPButtons(mandolin_message* pMsg, uint8 uiSequence, uint32 uiMode);
	bool CreateTestGovnSetFPLEDs(mandolin_message* pMsg, uint8 uiSequence,int nIndex, int nValue);
	bool CreateTestGovnSetFPStatusLED(mandolin_message* pMsg, uint8 uiSequence, int nValue);
	bool CreateTestGovnAccelerometer(mandolin_message* pMsg, uint8 uiSequence, float fX, float fY, float fXTolerance, float fYTolerance);
	bool CreateTestGovnBrooklynComms(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateTestGovnDSPComms(mandolin_message* pMsg, uint8 uiSequence, int nDSPNum);
	bool CreateTestGovnUSBHost(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateTestGovnUSBDevice(mandolin_message* pMsg, uint8 uiSequence);
	bool CreateTestGovnBattery(mandolin_message* pMsg, uint8 uiSequence);



 
};

#endif

