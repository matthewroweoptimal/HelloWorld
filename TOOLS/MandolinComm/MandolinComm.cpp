// This is the main DLL file.


extern "C" {

#include "LOUD_defines.h"
#include "LOUD_types.h"
#include "mandolin.h"
#include "CMWrap.h"
}

#include "string.h"
#include <stdio.h>

#include "assert.h"

#include "MandolinComm.h"
uint32 tick_count;

#define NULL 0



#define MANDOLIN_PAYLOAD_GETUINT32(in_ptr,offset)	   ((uint32)(((uint32)in_ptr[offset+3]&0x0ff)<<24) + (uint32)((in_ptr[offset+2]&0x0ff)<<16) + (uint32)((in_ptr[offset+1]&0x0ff)<<8) + (uint32)((in_ptr[offset+0]&0x0ff)<<0))
#define MANDOLIN_PAYLOAD_SETUINT32(payload,offset,value)     payload[offset]=(uint8)(value & 0x0ff); payload[offset+1]= (uint8)((value>>8) & 0x0ff); payload[offset+2]= (uint8)((value>>16) & 0x0ff); payload[offset+3]= (uint8)((value>>24) & 0x0ff)

int i;

CMandolinComm::CMandolinComm() :
	m_funcPing(NULL),
	m_vpPingUserData(NULL),
	m_funcSetApplicationParameter(NULL),
	m_vpSetApplicationParameterUserData(NULL),
	m_funcFileOpenResponse(NULL),
	m_vpFileOpenResponseUserData(NULL),
	m_funcProcessFileResponse(NULL),
	m_vpProcessFileResponseUserData(NULL),
	m_funcGetFileInfoResponse(NULL),
	m_vpGetFileInfoResponseUserData(NULL),
	m_funcGenericResponse(NULL),
	m_vpGenericResponseUserData(NULL),
	m_funcDiagnosticFaultResponse(NULL),
	m_vpDiagnosticFaultResponseUserData(NULL),
	m_funcTiltSensorResponse(NULL),
	m_vpTiltSensorResponseUserData(NULL),
	m_funcMicSweepResponse(NULL),
	m_vpMicSweepResponseUserData(NULL)
{

}

CMandolinComm::~CMandolinComm()
{
}

// ----------------------------------------------------------------------------------------
bool CMandolinComm::CheckIfHasSubId(int nMsgId)
{
	bool bHasSubId = false;
	switch(nMsgId)
	{
		case MANDOLIN_MSG_INVALID:									break;
		case MANDOLIN_MSG_PING: 									break;
		case MANDOLIN_MSG_ADVERTIZE_HARDWARE_INFO:					break;
		case MANDOLIN_MSG_GET_HARDWARE_INFO:						break;
		case MANDOLIN_MSG_GET_SOFTWARE_INFO:						break;
		case MANDOLIN_MSG_REBOOT:									break;
		case MANDOLIN_MSG_TRIGGER_SYSTEM_EVENT: 					bHasSubId = true; break;
		case MANDOLIN_MSG_TRIGGER_APPLICATION_EVENT:				bHasSubId = true; break;
		case MANDOLIN_MSG_TEST: 									bHasSubId = true; break;
		case MANDOLIN_MSG_GET_INFO: 								bHasSubId = true; break;
		case MANDOLIN_MSG_FILE_OPEN:								break;
		case MANDOLIN_MSG_FILE_CLOSE:								break;
		case MANDOLIN_MSG_GET_FILE: 								break;
		case MANDOLIN_MSG_POST_FILE:								break;
		case MANDOLIN_MSG_GET_SYSTEM_STATUS:						bHasSubId = true; break;
		case MANDOLIN_MSG_SET_SYSTEM_STATE: 						bHasSubId = true; break;
		case MANDOLIN_MSG_RESET_SYSTEM_STATE:						break;
		case MANDOLIN_MSG_SUBSCRIBE_SYSTEM_STATE:					break;
		case MANDOLIN_MSG_GET_APPLICATION_PARAMETER:				break;
		case MANDOLIN_MSG_SET_APPLICATION_PARAMETER:				break;
		case MANDOLIN_MSG_RESET_APPLICATION_PARAMETER:				break;
		case MANDOLIN_MSG_SUBSCRIBE_APPLICATION_PARAMETER:			break;
		case MANDOLIN_MSG_CREATE_PARAMETER_LIST:					break;
		case MANDOLIN_MSG_GET_APPLICATION_STRING:					break;
		case MANDOLIN_MSG_SET_APPLICATION_STRING:					break;
		case MANDOLIN_MSG_RESET_APPLICATION_STRING: 				break;
		case MANDOLIN_MSG_CUSTOM:									bHasSubId = true; break;
		default:													break;
	}
	return bHasSubId;
}

int CMandolinComm::ConvertMessage2Text(char* pcBuffer, int nMaxLength, char* pcDevice, uint8* pMsgData, uint16 uiLength)
{
	char pcTemp[1000];
	int i;

	pcBuffer[0] = 0;
	nMaxLength -= (strlen(pcDevice)+1);
	if (nMaxLength <0) return 0;
	sprintf(pcBuffer,"[%s]\n",pcDevice);


	i = 0;
	while((i<uiLength)&& (nMaxLength>6))
	{
		if((i!=0) && ((i%16) == 0))
		{
			strcat(pcBuffer,"\n");
			nMaxLength -= strlen(pcTemp);
		}

		sprintf(pcTemp,"%2.2x ",*pMsgData++ & 0x0ff);
		strncat(pcBuffer,pcTemp,nMaxLength);
		nMaxLength -= strlen(pcTemp);
		i++;
	}
	if (i%16)
	{
		strcat(pcBuffer,"\n");
		nMaxLength -= 1;
	}

	return strlen(pcBuffer);
}
	
// ----------------------------------------------------------------------------
// Processs Incoming Messages
// ----------------------------------------------------------------------------

int CMandolinComm::RxMandolinBytes(uint8* bytes, uint16  nLength)
{
	// assume the Bytes contain full messages

	mandolin_byte_type **pBuffer = &((mandolin_byte_type *)bytes);
	int nReturn = nLength;

	if (nLength == 0) 
		return 0;
	while (MANDOLIN_MSG_readbuffer(pBuffer, &nLength,&mInMsg, mInMsgPayload))
	{
		ProcessMessage(&mInMsg);

		if (nLength == 0) 
			break;
	}
	return nReturn;


}

bool CMandolinComm::DecodeSetApplicationParameter( mandolin_message* pInMsg, uint32* nTarget,uint32* uiPIDs,uint32* uiValues,int* nNumofParams)
{

	uint32	flags;
	uint8 *in_ptr = (uint8 *) pInMsg->payload;
	
	bool32 valid_edit = false;
	bool bMultiParameter = false;
	bool bUseParameterList = false;

	int nParameterNumber;
	int nLength;
	int i;
	intfloat j;
	int nDirection;
	int k;

	*nTarget = 0;
	*nNumofParams = 0;


	nParameterNumber = MANDOLIN_PAYLOAD_GETUINT32(in_ptr,0);
	flags	= MANDOLIN_PAYLOAD_GETUINT32(in_ptr,4);
	*nTarget =  MANDOLIN_EDIT_TARGET(flags);
	nDirection =  MANDOLIN_WILD_DIRECTION(flags);

	nLength = pInMsg->length - 2;	// for flags and PID

	valid_edit = true;
	// --------------------------------
	//	checks to see if Multiparameter
	// --------------------------------
	if (MANDOLIN_WILD_TYPE(flags) ==  MANDOLIN_TYPE_MULTIPARAMETER)
	{
		bMultiParameter = true;

		//return	MANDOLIN_handle_SET_MULTIPARAMETER(in);
	}

	if (MANDOLIN_WILD_TYPE(flags) == MANDOLIN_TYPE_LIST)
	{
		if (nParameterNumber == 1)
		{
			bUseParameterList = false;
		}
		else
		{
			bUseParameterList = true;
			// check to see if the parameter list is been set?
			return false; // for now do not decode
		}

		nDirection = MANDOLIN_DIRECTION_NEXT;
	}
	else
	{
		nDirection = MANDOLIN_WILD_DIRECTION(flags);
	}


	k = 0;
	// --------------------
	// do the edit
	// --------------------
	for (i=0;i<nLength;i++)
	{

		if ((i & 0x01) && (bMultiParameter))
		{
			// 0=value, 1=nextid, 2=value, 3=next id
			nParameterNumber = MANDOLIN_PAYLOAD_GETUINT32(in_ptr,i*4+8);
		}
		else
		{
			//if (bUseParameterList)
			//{
			//	nParameterNumber =  pPortDesc->nParameterListParameterId[i]; // parameter id list
			//}


			j.ui = MANDOLIN_PAYLOAD_GETUINT32(in_ptr,i*4+8);

			uiPIDs[k] = nParameterNumber;
			uiValues[k++] = j.ui;

			// --------------------------------------------------------
			// next parameter
			// --------------------------------------------------------


			if (nDirection == MANDOLIN_DIRECTION_PREVIOUS)
			{
				nParameterNumber--;
			}
			else
			{
				nParameterNumber++;
			}
		}
	}

	*nNumofParams = k;
	return true;
}

bool CMandolinComm::DecodeSoftwareInfoResponse( mandolin_message* pInMsg, uint32* uiID, uint32* uiBuild, int* nNumofIDs)
{
    uint8 *in_ptr = (uint8 *) pInMsg->payload;
	int nMsgLength;
	int nMsgId;
	int nPos = 0;
	
	nMsgLength = pInMsg->length;
	nMsgId = pInMsg->id;

	// decode the xml version
	if (nMsgLength)
	{
		uiID[nPos] = ANYA_APPID_XML;
		uiBuild[nPos] = MANDOLIN_PAYLOAD_GETUINT32(in_ptr,0*4);
		nPos++;
		//sprintf(pcBuffer,"XML Version = 0x%8.8x\n",uiBuild[nPos]);
		nMsgLength--;

	}

	// decode other builds versions
	while (nMsgLength>1)
	{
		nMsgLength -= 2;

		uiID[nPos] = MANDOLIN_PAYLOAD_GETUINT32(in_ptr,(nPos*2-1)*4);
		uiBuild[nPos] = MANDOLIN_PAYLOAD_GETUINT32(in_ptr,(nPos*2)*4);
		nPos++;

		//pcBuffer[0] = 0;
		//switch(*in_ptr++)
		//{
		//case ANYA_APPID_UNDEFINED:		sprintf(pcBuffer,"ANYA_APPID_UNDEFINED = 0x%8.8x\n",*in_ptr++); break;	
		//case ANYA_APPID_WHITEAPP:		sprintf(pcBuffer,"ANYA_APPID_WHITEAPP = 0x%8.8x\n",*in_ptr++); break;	
		//case ANYA_APPID_GOVNAPP:		sprintf(pcBuffer,"ANYA_APPID_GOVNAPP = 0x%8.8x\n",*in_ptr++); break;	
		//case ANYA_APPID_BROOKLYNAPP:	sprintf(pcBuffer,"ANYA_APPID_BROOKLYNAPP = 0x%8.8x\n",*in_ptr++); break;
		//case ANYA_APPID_DSPBOOT:		sprintf(pcBuffer,"ANYA_APPID_DSPBOOT = 0x%8.8x\n",*in_ptr++); break;	
		//case ANYA_APPID_DSPOS:			sprintf(pcBuffer,"ANYA_APPID_DSPOS = 0x%8.8x\n",*in_ptr++); break;		
		//default:
		//	in_ptr++;
		//	break;
		//}

	}

	*nNumofIDs = nPos;
	return true;

}

bool CMandolinComm::DecodeGetParameterReply(mandolin_message* pInMsg, uint32* nTarget,uint32* uiPIDs,uint32* uiValues,int* nNumofParams)
{

    uint32  flags;
    uint32 *in_ptr = (uint32 *) pInMsg->payload;
    
    bool valid_edit = false;
	bool bMultiParameter = false;
    

    int nParameterNumber;
	int k;
	int nDirection;
	int nNumOfParameters;
		
	*nTarget = 0;
	*nNumofParams = 0;



	intfloat j;
   
   
    nParameterNumber = in_ptr[0];
	flags = in_ptr[1];

	nDirection = MANDOLIN_WILD_DIRECTION(flags);
	*nTarget = MANDOLIN_EDIT_TARGET(flags);

	nNumOfParameters = pInMsg->length-2;

	if (MANDOLIN_WILD_TYPE(flags) == MANDOLIN_TYPE_MULTIPARAMETER)
	{
		bMultiParameter = true;
	}

 
	int n = 0;

	for(k=0;k<nNumOfParameters;k++)
	{

		if ((k & 0x01) && (bMultiParameter))
		{
			// 0=value, 1=nextid, 2=value, 3=next id
			nParameterNumber = in_ptr[k+2];

		}
		else
		{

			// --------------------------------------------------------
			// decode the Parameter
			// --------------------------------------------------------

			j.ui = in_ptr[k+2];


			uiPIDs[n] = nParameterNumber;
			uiValues[n] = j.ui;
			n++;

			// --------------------------------------------------------
			// next parameter
			// --------------------------------------------------------


			if (nDirection == MANDOLIN_DIRECTION_PREVIOUS)
			{
				nParameterNumber--;
			}
			else
			{
				nParameterNumber++;
			}
		}
	}


	*nNumofParams = nNumOfParameters;

	return true;
}

#pragma warning (push)
#pragma warning (disable: 4748)

void CMandolinComm::ProcessMessage(mandolin_message* pInMsg)
{
	uint32 nNestedTargetId = 0;
	bool bIsReply;
	uint8* in_ptr = (uint8*)  pInMsg->payload;
	bool bReplyFuncCalled = false;

	uint32 seqID = pInMsg->sequence;

	nNestedTargetId = 0;
	bIsReply = IS_MANDOLIN_REPLY(pInMsg->transport);

	if (pInMsg->id == MANDOLIN_MSG_NESTED)
	{
		if ((pInMsg->length >= 1) && (pInMsg->payload))
		{

			nNestedTargetId = MANDOLIN_PAYLOAD_GETUINT32(in_ptr,0);
			pInMsg->id = (nNestedTargetId >> 16) & 0x0ffff;
			nNestedTargetId &= 0x0ffff;

			pInMsg->length--;
			if (pInMsg->length>0)
			{
				pInMsg->payload =  (void*)(&in_ptr[4]);
				in_ptr = (uint8*)  pInMsg->payload;
			}
			else
			{
				pInMsg->length = 0;
				pInMsg->payload = NULL;
			}
		}
	}

	switch (pInMsg->id)
	{

	case MANDOLIN_MSG_PING:		// Ping message
		if (bIsReply)
		{
			//if (m_funcGenericResponse)
			//	m_funcGenericResponse(pInMsg, m_vpGenericResponseUserData);
		}
		else
		{
			if (m_funcPing)
			{
				m_funcPing(pInMsg,m_vpPingUserData);
			}
			//Send_reply_ACK(theMessage);
		}
		break;
	case MANDOLIN_MSG_GET_SOFTWARE_INFO:
		if (bIsReply)
		{
			if (m_funcSoftwareInfoResponse)
			{
				uint32 uiIDs[20];
				uint32 uiBuilds[20];
				int nNumofIDs;
				DecodeSoftwareInfoResponse(pInMsg,uiIDs,uiBuilds, &nNumofIDs);
				m_funcSoftwareInfoResponse(pInMsg, uiIDs, uiBuilds, nNumofIDs, m_vpSoftwareInfoResponseUserData);
				bReplyFuncCalled = true;
			}
		}
		else
		{
		}
		break;
	case MANDOLIN_MSG_REBOOT:
		if(bIsReply)
		{
			//if (m_funcGenericResponse)
			//	m_funcGenericResponse(pInMsg, m_vpGenericResponseUserData);
		}
		break;
	case MANDOLIN_MSG_POST_FILE:
		if(bIsReply)
		{
			//if (m_funcGenericResponse)
			//	m_funcGenericResponse(pInMsg, m_vpGenericResponseUserData);
		}
		else
		{
		}
		break;
	case MANDOLIN_MSG_GET_FILE:
		if(bIsReply)
		{
			if (m_funcFileGetResponse)
			{
				if (pInMsg->length >= 4)// FIXME: should this be 4 or 5?
				{
					int *payload = (int*)pInMsg->payload;
					int fileID = payload[0];
					int fileSize = payload[1];
					int fileOffset = payload[2];
					int bytesReceived = payload[3];
					char* data = NULL;
					if (pInMsg->length > 4)
					{
						data = (char*)(payload+4);
					}
					m_funcFileGetResponse(pInMsg, fileID, fileSize, fileOffset, bytesReceived, data, m_vpFileGetResponseUserData);
					bReplyFuncCalled = true;

				}
				else
				{
					//if (m_funcFileGetResponse)
					//{
					//	m_funcGenericResponse(pInMsg, m_vpGenericResponseUserData);
					//}
				}
			}
		}
		break;
	case MANDOLIN_MSG_FILE_OPEN:
		if(bIsReply)
		{
			if(m_funcFileOpenResponse)
			{
				int *payload = (int*)pInMsg->payload;
				bool success = (1 == payload[0]);
				int fileID = payload[1]; // -1 to signal fopen error
				m_funcFileOpenResponse(pInMsg, success, fileID, m_vpFileOpenResponseUserData);
				bReplyFuncCalled = true;

			}
		}
		else
		{
		}
		break;
	case MANDOLIN_MSG_FILE_CLOSE:
		if(bIsReply)
		{
			//if (m_funcGenericResponse)
			//	m_funcGenericResponse(pInMsg, m_vpGenericResponseUserData);
		}
		else
		{
		}
		break;
	case MANDOLIN_MSG_SET_APPLICATION_PARAMETER:
		if (bIsReply)
		{
			//if (m_funcGenericResponse)
			//	m_funcGenericResponse(pInMsg, m_vpGenericResponseUserData);
		}
		else
		{
			if (m_funcSetApplicationParameter)
			{
				uint32 nTarget;
				uint32 uiPIDs[500];
				uint32 uiValues[500];
				int nNumofParams;
				DecodeSetApplicationParameter(pInMsg, &nTarget,uiPIDs,uiValues,&nNumofParams);
				m_funcSetApplicationParameter(pInMsg,nTarget, uiPIDs,uiValues, nNumofParams, m_vpSetApplicationParameterUserData);
			}
		}
		break;
	case MANDOLIN_MSG_GET_APPLICATION_PARAMETER:
		if (bIsReply)
		{
			if (m_funcSetApplicationParameter)
			{
				uint32 nTarget = 0;
				uint32 uiPIDs[500] = {0};
				uint32 uiValues[500] = {0};
				int nNumofParams;
				DecodeGetParameterReply(pInMsg, &nTarget, uiPIDs, uiValues, &nNumofParams);
				m_funcSetApplicationParameter(pInMsg,nTarget, uiPIDs,uiValues, nNumofParams, m_vpSetApplicationParameterUserData);
				bReplyFuncCalled = true;

			}

			//switch(MANDOLIN_EDIT_TARGET(pPayload[1]))
			//{
			//case eTARGET_CURRENT_STATE:
			//	//DecodeGetParameterReply(theMessage, theLength);
			//	break;
			//case eTARGET_METERS:
			//	//DecodeGetMetersReply(theMessage,theLength);
			//	break;
			//default:
			//	break;
			//}

		}
		else
		{
			
		}
		break;
	case MANDOLIN_MSG_TRIGGER_SYSTEM_EVENT:
		if (bIsReply)
		{
			int *pPayload = (int*)pInMsg->payload;
			switch(pPayload[0])
			{												
			case    MANDOLIN_SE_CONNECT:				     break;
			case    MANDOLIN_SE_DISCONNECT:				     break;
			case    MANDOLIN_SE_LOCK_CHANGES:			     break;
			case    MANDOLIN_SE_UNLOCK_CHANGES:			     break;
			case    MANDOLIN_SE_IDENTIFY:				     break;
			case    MANDOLIN_SE_SYNC_CURRENT_STATE:			 break;
			case MANDOLIN_SE_BEGIN_UPDATE:			     
				//if (m_funcGenericResponse)
				//	m_funcGenericResponse(pInMsg, m_vpGenericResponseUserData);
				break;
			case MANDOLIN_SE_END_UPDATE:				     
				//if (m_funcGenericResponse)
				//	m_funcGenericResponse(pInMsg, m_vpGenericResponseUserData);
				break;
			case MANDOLIN_SE_PROCESS_FILES:
				if(m_funcProcessFileResponse)
				{
					int percent = 0;
					if( MANDOLIN_PROGRESS_BUSY == pPayload[1])
					{
						percent = pPayload[2];
					}
					m_funcProcessFileResponse(pInMsg, pPayload[1], percent,m_vpProcessFileResponseUserData);
					bReplyFuncCalled = true;


				}
				break;
			}
		}
		else
		{
		}
		break;
	case MANDOLIN_MSG_GET_INFO:
		if (bIsReply)
		{
			int *pPayload = (int*)pInMsg->payload;

			if(pInMsg->length < 5) // error
			{
				int errorCode = -1;
				if(pInMsg->length >= 2 && pPayload[0] == MANDOLIN_GI_FILE) // NACK
					errorCode = pPayload[1];

				if(m_funcGetFileInfoResponse)
				{
					m_funcGetFileInfoResponse(pInMsg, MANDOLIN_PROGRESS_FAIL, errorCode, -1,m_vpGetFileInfoResponseUserData);
					bReplyFuncCalled = true;
				}

			}
			else
			{
				// 0 sub id
				// 1 file id
				// 2 file size
				// 3 status (0=fail, 1=success (with next word CRC), 2=busy (with next word percent)
				int nSubId = pPayload[0];
				int nFileId = pPayload[1];
				int nFileSize = pPayload[2];
				int nStatus = pPayload[3];
				int nPercent = pPayload[4]; // percent or CRC

				switch(nSubId)
				{	
				case MANDOLIN_GI_FILE:	
					if(m_funcGetFileInfoResponse)
					{
						m_funcGetFileInfoResponse(pInMsg, nStatus, nPercent, nFileSize, m_vpGetFileInfoResponseUserData);
						bReplyFuncCalled = true;
					}
					break;
				case	MANDOLIN_GI_LIST:	break;
				case	MANDOLIN_GI_SNAPSHOT: break;
				case	MANDOLIN_GI_SHOW: break;
				}
			}
		}
		else
		{			
		}
		break;
	case MANDOLIN_MSG_CUSTOM:
		if (bIsReply)
		{
			int *pPayload = (int*)pInMsg->payload;
			if(pInMsg->length > 1)
			{
				int nSubId = pPayload[0];
				int typeId = 0;
				switch(nSubId)
				{
				case MANDOLIN_CUSTOM_IRDA:	
					m_funcGenericResponse(pInMsg, m_vpGenericResponseUserData);
					bReplyFuncCalled = true;

					break;
				case MANDOLIN_CUSTOM_DIAGNOSTIC:
					typeId = pPayload[1];
					switch(typeId)
					{
					case MANDOLIN_CUSTOM_DIAGNOSTIC_CHAN_FAULT_FAST:
						if(m_funcDiagnosticFaultResponse && pInMsg->length > 2)
						{
							int nMsgLength = pInMsg->length - 2;
							m_funcDiagnosticFaultResponse(pInMsg, (uint32*)pPayload + 2, nMsgLength, m_vpDiagnosticFaultResponseUserData);
							bReplyFuncCalled = true;

						}
						break;
					}
					break;
				case MANDOLIN_CUSTOM_TIME:
					typeId = pPayload[1];
					switch(typeId)
					{
					case MANDOLIN_CUSTOM_TIME_SET:
						if(m_funcGenericResponse)
						{
							//bool success = pPayload[2];
							m_funcGenericResponse(pInMsg, m_vpGenericResponseUserData);
							bReplyFuncCalled = true;

						}
						break;
	
					case MANDOLIN_CUSTOM_TIME_GET:
						if(m_funcGenericResponse)
						{
							//bool success = pPayload[2];
							m_funcGenericResponse(pInMsg, m_vpGenericResponseUserData);
							bReplyFuncCalled = true;

						}
						break;
					}
					break;

				case MANDOLIN_CUSTOM_TILTSENSOR:
					typeId = pPayload[1];
					switch(typeId)
					{
					case MANDOLIN_CUSTOM_TILTSENSOR_GET:
						if(m_funcGenericResponse)
						{
							//bool success = pPayload[2];
							m_funcGenericResponse(pInMsg, m_vpGenericResponseUserData);
							bReplyFuncCalled = true;

						}
						break;
					case  MANDOLIN_CUSTOM_TILTSENSOR_CALIBRATE:
						if(m_funcGenericResponse)
						{
							//bool success = pPayload[2];
							m_funcGenericResponse(pInMsg, m_vpGenericResponseUserData);
							bReplyFuncCalled = true;

						}
						break;
					case MANDOLIN_CUSTOM_TILTSENSOR_SET:
					default:
						if(m_funcGenericResponse)
						{
							//bool success = pPayload[2];
							m_funcGenericResponse(pInMsg, m_vpGenericResponseUserData);
							bReplyFuncCalled = true;

						}
						break;
					}

				case MANDOLIN_CUSTOM_MICSWEEP:
					typeId = pPayload[1];
					switch(typeId)
					{
					case MANDOLIN_CUSTOM_MICSWEEP_CALIBRATE_START:
						if(m_funcMicSweepResponse)
						{
							int status = pPayload[2];
							m_funcMicSweepResponse(pInMsg, status, 0, NULL, m_vpMicSweepResponseUserData);
							bReplyFuncCalled = true;

						}
						break;
					case MANDOLIN_CUSTOM_MICSWEEP_CALIBRATE_PROGRESS:
						if(m_funcMicSweepResponse)
						{
							int status = pPayload[2];
							int percentage = 0;
							uint8 channelProgress[22][4]; 
							if(status == MANDOLIN_PROGRESS_BUSY)
							{
								 percentage = pPayload[3];
								 if(pInMsg->length == 26)
								 {
									 for(int i = 0; i < 22; i++)
									 {
										 channelProgress[i][0] = (pPayload[i+4] >> 24) & 0xFF; // channel number
										 channelProgress[i][1] = (pPayload[i+4] >> 16) & 0xFF; // channel status
										 channelProgress[i][2] = (pPayload[i+4] >> 8) & 0xFF; // channel percent complete
										 channelProgress[i][3] = (pPayload[i+4]) & 0xFF; // channel try number
									 }
								 }
								 else
								 {
									channelProgress[0][0] = 0;
								 }
								
							}
							m_funcMicSweepResponse(pInMsg, status, percentage, channelProgress, m_vpMicSweepResponseUserData);
							bReplyFuncCalled = true;

						}
						break;
					case MANDOLIN_CUSTOM_MICSWEEP_CALIBRATE_STOP:
						if(m_funcMicSweepResponse)
						{
							int status = pPayload[2];
							m_funcMicSweepResponse(pInMsg, status, 0, NULL, m_vpMicSweepResponseUserData);
							bReplyFuncCalled = true;

						}
						break;
					case MANDOLIN_CUSTOM_MICSWEEP_SINGLE_START:
						if(m_funcMicSweepResponse)
						{
							int status = pPayload[2];
							m_funcMicSweepResponse(pInMsg, status, 0, NULL, m_vpMicSweepResponseUserData);
							bReplyFuncCalled = true;

						}
						break;
					case MANDOLIN_CUSTOM_MICSWEEP_SINGLE_PROGRESS:
						if(m_funcMicSweepResponse)
						{
							int status = pPayload[2];
							int percentage = 0;
							uint8 channelProgress[22][4]; 
							if(status == MANDOLIN_PROGRESS_BUSY)
							{
								 percentage = pPayload[3];
								 if(pInMsg->length == 26)
								 {
									 for(int i = 0; i < 22; i++)
									 {
										 channelProgress[i][0] = (pPayload[i+4] >> 24) & 0xFF; // channel number
										 channelProgress[i][1] = (pPayload[i+4] >> 16) & 0xFF; // channel status
										 channelProgress[i][2] = (pPayload[i+4] >> 8) & 0xFF; // channel percent complete
										 channelProgress[i][3] = (pPayload[i+4]) & 0xFF; // channel try number
									 }
								 }
								 else
								 {
									channelProgress[0][0] = 0;
								 }
							}
							m_funcMicSweepResponse(pInMsg, status, percentage, channelProgress, m_vpMicSweepResponseUserData);
							bReplyFuncCalled = true;

						}
						break;
					case MANDOLIN_CUSTOM_MICSWEEP_SINGLE_STOP:
						if(m_funcMicSweepResponse)
						{
							int status = pPayload[2];
							m_funcMicSweepResponse(pInMsg, status, 0, NULL, m_vpMicSweepResponseUserData);
							bReplyFuncCalled = true;

						}
						break;
					}
					break;
				}
			}
			else
			{
				if(m_funcGenericResponse)
				{
					m_funcGenericResponse(pInMsg, m_vpGenericResponseUserData);
					bReplyFuncCalled = true;

				}
			}
		}
		else
		{
			int *pPayload = (int*)pInMsg->payload;
			if(pInMsg->length > 1)
			{
				int nSubId = pPayload[0];
				int typeId = 0;
				switch(nSubId)
				{
				case MANDOLIN_CUSTOM_TILTSENSOR:
					typeId = pPayload[1];
					switch(typeId)
					{
					case MANDOLIN_CUSTOM_TILTSENSOR_SET:
						if(m_funcTiltSensorResponse)
						{
							intfloat xTiltValue;
							xTiltValue.ui = pPayload[2];

							intfloat yTiltValue;
							yTiltValue.ui = pPayload[3];

							m_funcTiltSensorResponse(pInMsg, xTiltValue.f, yTiltValue.f, m_vpTiltSensorResponseUserData);
							bReplyFuncCalled = true;

						}
						break;
					}
					break;
				}
			}
		}
		break;
	default:
		if(m_funcGenericResponse)
		{
			m_funcGenericResponse(pInMsg, m_vpGenericResponseUserData);
			bReplyFuncCalled = true;

		}
		break;
	
	}


	// call the generic response
	if (bIsReply && !bReplyFuncCalled &&  m_funcGenericResponse)
	{
		m_funcGenericResponse(pInMsg, m_vpGenericResponseUserData);
		
		bReplyFuncCalled = true;
	}
}

#pragma warning (pop)





// ---------------------------------------------------
// Handle Functions
// ---------------------------------------------------

void CMandolinComm::SetHandlerPing(void (*func)(mandolin_message* pMsg, void* pData), void* pUserData)
{
	m_funcPing = func;
	m_vpPingUserData = pUserData;
}

void  CMandolinComm::SetHandlerGenericResponse(void (*func)(mandolin_message* pMsg, void* pData), void* pUserData)
{
	m_funcGenericResponse = func;
	m_vpGenericResponseUserData = pUserData;
}


void  CMandolinComm::SetHandlerFileGetResponse(void (*func)(mandolin_message* pMsg, int fileID, int fileSize, int fileOffset, int  bytesReceived, char *FileData, void* pData), void* pUserData)
{
	m_funcFileGetResponse = func;
	m_vpFileGetResponseUserData = pUserData;
}

void CMandolinComm::SetHandlerSetApplicationParameter(void (*func)(mandolin_message* pMsg,uint32 uiTarget, uint32* uiPIDs, uint32* uiValues,int nNumofParams, void* pData),void* pUserData)
{
	m_funcSetApplicationParameter = func;
	m_vpSetApplicationParameterUserData = pUserData;
}

void  CMandolinComm::SetHandlerFileOpenResponse(void (*func)(mandolin_message* pMsg,bool success, int fileIdOrFalureReason, void* pData), void *pUserData)
{
	m_funcFileOpenResponse = func;
	m_vpFileOpenResponseUserData = pUserData;
}

void CMandolinComm::SetHandlerProcessFileResponse(void (*func)(mandolin_message* pMsg, int status, int percentage, void* pData), void *pUserData)
{
	m_funcProcessFileResponse = func;
	m_vpProcessFileResponseUserData = pUserData;
}

void CMandolinComm::SetHandlerGetFileInfoResponse(void (*func)(mandolin_message* pMsg, int status, int value, int fileSize, void* pData), void *pUserData)
{
	m_funcGetFileInfoResponse = func;
	m_vpGetFileInfoResponseUserData = pUserData;
}

void CMandolinComm::SetHandlerSoftwareInfoResponse(void (*func)(mandolin_message* pMsg,  uint32* uiIDs, uint32* uiBuilds,int nNumofIDs, void* pData), void* pUserData)
{
	m_funcSoftwareInfoResponse = func;
	m_vpSoftwareInfoResponseUserData = pUserData;
}

void CMandolinComm::SetHandlerDiagnosticFaultResponse(void (*func)(mandolin_message* pMsg, uint32* pValues, int nMsgLength, void* pData), void* pUserData)
{
	m_funcDiagnosticFaultResponse = func;
	m_vpDiagnosticFaultResponseUserData = pUserData;
}

void CMandolinComm::SetHandlerTiltSensorResponse(void (*func)(mandolin_message* pMsg, float xTiltValue, float yTiltValue, void* pData), void *pUserData)
{
	m_funcTiltSensorResponse = func;
	m_vpTiltSensorResponseUserData = pUserData;
}

void CMandolinComm::SetHandlerMicSweepResponse(void (*func)(mandolin_message* pMsg, int status, int percent, uint8 channelProgress[22][4], void* pData), void *pUserData)
{
	m_funcMicSweepResponse = func;
	m_vpMicSweepResponseUserData = pUserData;
}

// -------------------------------------------------------------------------------------------------------
// Create Messages
// -------------------------------------------------------------------------------------------------------
bool  CMandolinComm::CreateReplyAck(mandolin_message* pMsg, mandolin_message* pMsgSrc)
{
	bool bHasSubId;
	uint32 uSubId;
	uint8* pPayload = (uint8*) pMsg->payload;
	uint8* pPayloadSrc = (uint8*) pMsgSrc->payload;
    
	if (pMsgSrc->transport & MANDOLIN_TRANSPORT_NOACK)
		return false;

	bHasSubId = CheckIfHasSubId(pMsgSrc->id);

	pMsg->id =  pMsgSrc->id;
	pMsg->length = 0;
    pMsg->transport = pMsgSrc->transport | MANDOLIN_TRANSPORT_REPLY;

	if (bHasSubId)

	{
		if ((pPayload == 0) ||  (pPayloadSrc == 0))  
			return false;
		
		uSubId = MANDOLIN_PAYLOAD_GETUINT32(pPayloadSrc,0);
		MANDOLIN_PAYLOAD_SETUINT32(pPayload,0,uSubId);

		pMsg->length = 1;
	}
	MANDOLIN_MSG_pack(pMsg, pMsgSrc->sequence);

	return true;
 
}
// -------------------------------------------------------------------------------------------------------
bool  CMandolinComm::CreateReplyWithPayload(mandolin_message* pMsg,  mandolin_message* pMsgSrc, uint8* pPayloadSrc, int nWordLengthSrc)
{
	uint8*	pPayload = (uint8*) pMsg->payload;
    int i;
	bool bHasSubId;

	if (pMsgSrc->transport & MANDOLIN_TRANSPORT_NOACK)
		return false;

	bHasSubId = CheckIfHasSubId(pMsgSrc->id);

	if (!pPayload) 
		return false;

	if ((pPayloadSrc == 0) && (nWordLengthSrc > 0)) 
		return false;

	pMsg->id =  pMsgSrc->id;
    pMsg->transport = pMsgSrc->transport | MANDOLIN_TRANSPORT_REPLY;

	for(i=0;i<nWordLengthSrc*4;i++)
	{
		*pPayload++ = *pPayloadSrc++;
	}
	pMsg->length = nWordLengthSrc;

	MANDOLIN_MSG_pack(pMsg, pMsgSrc->sequence);

	return true;
 
}
// -------------------------------------------------------------------------------------------------------
#pragma warning (push)
#pragma warning (disable: 4748)

bool CMandolinComm::CMWrapMandolinMsg(const mandolin_message* pMsg, uint32 srcID[2], uint8* buffer, uint32* len)
{
	bool rval = false;
	unsigned char pMsgBuffer[1024*3];
	CMWrap_message mCMWrapMsg = {0};
	unsigned char pCMWrapMsgBuffer[1024*3];

	uint32 nLength = MANDOLIN_MSG_write(pMsgBuffer,const_cast<mandolin_message*>(pMsg));

	// kludge... add extra bytes at end so readbuffer does not have an issues that extra bytes in buffer the checksum match.  &&&&gm
	//for(i=0;i<4*8;i++)
	//{
	//	pMsgBuffer[nLength+i] = 0;
	//}
	//nLength += i;
	// end of kludge
	bool bControl = true;

	mCMWrapMsg.source_id_hi = srcID[0];
	mCMWrapMsg.source_id_lo = srcID[1];
	mCMWrapMsg.payload = pMsgBuffer; // the mandolin message
	mCMWrapMsg.transport = (IS_MANDOLIN_REPLY(pMsg->transport)?CMWRAP_TRANSPORT_REPLY:0) + (bControl?CMWRAP_TRANSPORT_STATUSCONTROL:0);
	mCMWrapMsg.length = (nLength/4); // length of payload (the mandolin message length)
	CMWrap_MSG_pack(&mCMWrapMsg);
	nLength = CMWrap_MSG_write(pCMWrapMsgBuffer,&mCMWrapMsg);

	if (nLength <= *len)
	{
		memcpy(buffer, pCMWrapMsgBuffer, nLength);
		*len = nLength;
		rval = true;
	}

	return rval;
}

#pragma warning (pop)


// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreatePingMsg(mandolin_message* pMsg, uint8 uiSequence)
{

	pMsg->id =  MANDOLIN_MSG_PING;
	pMsg->length = 0;
	pMsg->payload = 0;
	pMsg->transport = 0;
	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

#define MAX_PARAMEDIT_NEIGHBORHOOD 150

// -------------------------------------------------------------------------------------------------------
bool  CMandolinComm::CreateParameterEditSetNeighborhood(mandolin_message* pMsg, uint8 uiSequence,uint32 nTarget, uint32 uiStartPID, intfloat ifParamValue[], uint32 nNumOfParams,bool bNoAck, int nInstance)
{

	uint8* pPayload = (uint8*)pMsg->payload;
	uint32 uiValue;
	uint32 i;


	pMsg->id =  MANDOLIN_MSG_SET_APPLICATION_PARAMETER;
	if (pMsg->payload == 0) 
		return false;
	if (nNumOfParams < 1) 
		return false;
	pMsg->transport = (bNoAck?MANDOLIN_TRANSPORT_NOACK:0);


//	MANDOLIN_MSG_pack(pMsg, uiSequence);

	assert(nNumOfParams <= MAX_PARAMEDIT_NEIGHBORHOOD);

	if (nNumOfParams > MAX_PARAMEDIT_NEIGHBORHOOD) 
		nNumOfParams = MAX_PARAMEDIT_NEIGHBORHOOD;

	pMsg->length = 1+1+nNumOfParams;

	uiValue = ((nTarget <<  MANDOLIN_EDIT_TARGET_SHIFT) & MANDOLIN_EDIT_TARGET_MASK) +
						((nInstance <<  MANDOLIN_WILDCARD_MAP_SHIFT) & MANDOLIN_WILDCARD_MAP_MASK) +
						((MANDOLIN_DIRECTION_NEXT <<  MANDOLIN_WILDCARD_DIRECTION_SHIFT) & MANDOLIN_WILDCARD_DIRECTION_MASK) +
						((MANDOLIN_TYPE_NEIGHBORHOOD <<  MANDOLIN_WILDCARD_TYPE_SHIFT) & MANDOLIN_WILDCARD_TYPE_MASK);

	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,uiStartPID);             // 0 first parameter id
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,uiValue);              // 1 parameter flags
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,ifParamValue[0].ui);   // 2 first param data
	pPayload += 3*4;
	
	// additional parameters
	for (i=1;i<nNumOfParams;i++)
	{
		MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ifParamValue[i].ui);
		pPayload+=4;
	}
	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;
}
#define MAX_PARAMEDIT_MULTI 150

// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateParameterEditSet(mandolin_message* pMsg, uint8 uiSequence,uint32 nTarget, uint32 uiPID[], intfloat ifParamValue[], uint32 nNumofParams, bool bNoAck, int nInstance)
{

	uint8* pPayload = (uint8*)pMsg->payload;
	uint32 uiValue;
	uint32 i;


	pMsg->id =  MANDOLIN_MSG_SET_APPLICATION_PARAMETER;
	if (pMsg->payload == 0) 
		return false;
	if (nNumofParams < 1) 
		return false;
	pMsg->transport = (bNoAck?MANDOLIN_TRANSPORT_NOACK:0);


	//MANDOLIN_MSG_pack(pMsg, uiSequence);

	assert(nNumofParams <= MAX_PARAMEDIT_MULTI);

	if (nNumofParams > MAX_PARAMEDIT_MULTI) 
		nNumofParams = MAX_PARAMEDIT_MULTI;

	pMsg->length = 1+nNumofParams*2;


	uiValue = ((nTarget <<  MANDOLIN_EDIT_TARGET_SHIFT) & MANDOLIN_EDIT_TARGET_MASK) +
						((nInstance <<  MANDOLIN_WILDCARD_MAP_SHIFT) & MANDOLIN_WILDCARD_MAP_MASK) +
						((MANDOLIN_DIRECTION_NEXT <<  MANDOLIN_WILDCARD_DIRECTION_SHIFT) & MANDOLIN_WILDCARD_DIRECTION_MASK) +
						((MANDOLIN_TYPE_MULTIPARAMETER <<  MANDOLIN_WILDCARD_TYPE_SHIFT) & MANDOLIN_WILDCARD_TYPE_MASK);

	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,uiPID[0]);             // 0 first parameter id
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,uiValue);              // 1 parameter flags
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,ifParamValue[0].ui);   // 2 first param data
	pPayload += 3*4;
	
	// additional parameters
	for (i=1;i<nNumofParams;++i)
	{
		MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,uiPID[i]);
		MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,ifParamValue[i].ui);
		pPayload+=8;
	}
	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;

}
// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateParameterEditGetNeighborhood(mandolin_message* pMsg, uint8 uiSequence,uint32 nTarget, uint32 uiStartPID, uint32 nNumofParams, int nInstance)
{
	uint8* pPayload = (uint8*)pMsg->payload;
	uint32 uiValue;
	uint32 i;
	
	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_GET_APPLICATION_PARAMETER;
	//if (nNumofParams < 0) 
	//	return false;
	pMsg->transport = 0;

	uiValue =	((nTarget <<					MANDOLIN_EDIT_TARGET_SHIFT) &			MANDOLIN_EDIT_TARGET_MASK) +
				((nInstance <<  MANDOLIN_WILDCARD_MAP_SHIFT) & MANDOLIN_WILDCARD_MAP_MASK) +
				((MANDOLIN_DIRECTION_NEXT <<	MANDOLIN_WILDCARD_DIRECTION_SHIFT) &	MANDOLIN_WILDCARD_DIRECTION_MASK) +
				((nNumofParams <<				MANDOLIN_WILDCARD_COUNT_SHIFT) &		MANDOLIN_WILDCARD_COUNT_MASK) +
				((MANDOLIN_TYPE_NEIGHBORHOOD <<	MANDOLIN_WILDCARD_TYPE_SHIFT) &			MANDOLIN_WILDCARD_TYPE_MASK);
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,uiValue);

	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,uiStartPID);

	//for(i=0;i<nNumofParams;i++)
	//{
	//	if (i==0)
	//	{
	//		MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,uiStartPID);
	//	}
	//	else
	//	{
	//		MANDOLIN_PAYLOAD_SETUINT32(pPayload,(i+1)*4,uiStartPID);
	//	}
	//	uiStartPID++;
	//}
	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;
}

// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateParameterEditGetMulti(mandolin_message* pMsg, uint8 uiSequence,uint32 nTarget, uint32 uiPID[], uint32 nNumofParams, int nInstance)
{
	uint8* pPayload = (uint8*)pMsg->payload;
	uint32 uiValue;
	uint32 i;
	
	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_GET_APPLICATION_PARAMETER;
	if (nNumofParams < 1) 
		return false;
	pMsg->transport = 0;
//	pMsg->transport = MANDOLIN_TRANSPORT_ENDIAN;

	uiValue =	((nTarget <<						MANDOLIN_EDIT_TARGET_SHIFT) &			MANDOLIN_EDIT_TARGET_MASK) +
				((nInstance <<						MANDOLIN_WILDCARD_MAP_SHIFT) &			MANDOLIN_WILDCARD_MAP_MASK) +
				((MANDOLIN_DIRECTION_NEXT <<		MANDOLIN_WILDCARD_DIRECTION_SHIFT) &	MANDOLIN_WILDCARD_DIRECTION_MASK) +	// don't need this for multiparameter
				((MANDOLIN_TYPE_MULTIPARAMETER <<	MANDOLIN_WILDCARD_TYPE_SHIFT) &			MANDOLIN_WILDCARD_TYPE_MASK);

	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,uiValue);

	for(i=0;i<nNumofParams;i++)
	{
		if (i==0)
		{
			MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,uiPID[i]);
		}
		else
		{
			MANDOLIN_PAYLOAD_SETUINT32(pPayload,(i+1)*4,uiPID[i]);
		}
	}
	pMsg->length = 1+nNumofParams;

	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;
}
// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateParameterEditGetList(mandolin_message* pMsg, uint8 uiSequence,uint32 nTarget, uint32 uiListID)
{
	// not yet fully implemented - need to implement create parameter list, etc
	uint8* pPayload = (uint8*)pMsg->payload;
	uint32 uiValue;
	uint32 i;
	
	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_GET_APPLICATION_PARAMETER;

	pMsg->transport = 0;

	uiValue =	((nTarget <<					MANDOLIN_EDIT_TARGET_SHIFT) &			MANDOLIN_EDIT_TARGET_MASK) +
				((MANDOLIN_DIRECTION_NEXT <<	MANDOLIN_WILDCARD_DIRECTION_SHIFT) &	MANDOLIN_WILDCARD_DIRECTION_MASK) +	// don't need this for parameter list
				((MANDOLIN_TYPE_LIST <<			MANDOLIN_WILDCARD_TYPE_SHIFT) &			MANDOLIN_WILDCARD_TYPE_MASK);

	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,uiValue);
	// TODO: put list ID in here
	pMsg->length = 2;

//	MANDOLIN_MSG_pack(pMsg, uiSequence);
//	return true;
	return false;
}

// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateSnapshotStore(mandolin_message* pMsg, uint8 uiSequence, int nSnapshot )
{
	uint8* pPayload = (uint8*)pMsg->payload;
	
	if (pMsg->payload == 0) return false;
	pMsg->transport = 0;

	pMsg->id =  MANDOLIN_MSG_TRIGGER_APPLICATION_EVENT;

	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_AE_STORE_SNAPSHOT);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,nSnapshot);
	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;
}

// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateSnapshotRecall(mandolin_message* pMsg, uint8 uiSequence, int nSnapshot )
{
	uint8* pPayload = (uint8*)pMsg->payload;
	
	if (pMsg->payload == 0) return false;
	pMsg->transport = 0;

	pMsg->id =  MANDOLIN_MSG_TRIGGER_APPLICATION_EVENT;

	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_AE_LOAD_SNAPSHOT);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,nSnapshot);
	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;
}

// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateNested(mandolin_message* pMsg, uint8 uiSequence, mandolin_message* pMsgSrc, int nNestedDeviceId )
{
	// pMsgSrc can be the same as pMsg so you can use the same buffer
	uint8* pPayload = (uint8*)pMsg->payload;
	uint8* pPayloadSrc = (uint8*)pMsgSrc->payload;
	uint32 uiValue;
	int i;
	if (pMsg->payload == 0) return false;
	if (pMsgSrc == 0) return false;
	
	
	for (i=pMsgSrc->length*4-1;i>=0; i--)
	{
		*(pPayload+i+4) = *(pPayloadSrc+i);
	}

	pMsg->length = pMsgSrc->length+1;
	pMsg->transport = 0;
	uiValue = ((pMsgSrc->id & 0x0ffff) << 16) + (nNestedDeviceId & 0x0ffff);
	pMsg->id =  MANDOLIN_MSG_NESTED;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,uiValue);

	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;

}
// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateMakeFactory(mandolin_message* pMsg, uint8 uiSequence )
{
	uint8* pPayload = (uint8*)pMsg->payload;
	if (pMsg->payload == 0) return false;
	
	
	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_CUSTOM;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_CUSTOM_MAKE_FACTORY);

	pMsg->length = 1;
	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;
}

// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateGetHardwareInfo(mandolin_message* pMsg, uint8 uiSequence )
{
	// The response to a hardware info get consists of two or more payload words.  
	// The first two words are required to consist of the device’s brand (4-bits), model ID (12-bits), 
	// and the device’s individual ID or serial number (48-bits).  
	// Further words (as specified in the length field) are device specific and can be ignored in the general case.

	uint8* pPayload = (uint8*)pMsg->payload;
	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_GET_HARDWARE_INFO;
	pMsg->length = 0;
	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;

}
// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateGetSoftwareInfo(mandolin_message* pMsg, uint8 uiSequence )
{
	// The response to a software info get consists of three or more payload words. 
	// The first word is required to consist of the latest protocol version supported by the (host or device) 
	// (16-bits) and the (host or device’s) XML parameter map version (16-bits).  
	// The second word is required to be the responding (host or device’s) 32-bit application ID.  
	// The third word is required to be the application’s soft- or firmware version.  
	//  Further words (as specified in the length field) are (host or) device specific and can be ignored in the general case
	uint8* pPayload = (uint8*)pMsg->payload;
	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_GET_SOFTWARE_INFO;
	pMsg->length = 0;
	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;
}
// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateGetMaxMessageSize(mandolin_message* pMsg, uint8 uiSequence )
{
	uint8* pPayload = (uint8*)pMsg->payload;
	if (pMsg->payload == 0) return false;

	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_GET_SYSTEM_STATUS;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_SS_MAX_MSG_WORDS);

	pMsg->length = 1;
	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;
}



// -------------------------------------------------------------------------------------------------------
//typedef enum {
//    ANYA_REBOOT_HARDWARE = 0,
//    ANYA_REBOOT_TO_BOOT, 
//    ANYA_REBOOT_TO_OS,   
//} ANYA_reboot_type;
bool CMandolinComm::CreateReboot(mandolin_message* pMsg, uint8 uiSequence, uint32 uiRebootCode )
{
	uint8* pPayload = (uint8*)pMsg->payload;
	if (pMsg->payload == 0) return false;

	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_REBOOT;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,uiRebootCode);

	pMsg->length = 1;
	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;
}

// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateRebootBrooklyn(mandolin_message* pMsg, uint8 uiSequence, uint32 uiRebootCode)
{
	uint8* pPayload = (uint8*)pMsg->payload;
	if (pMsg->payload == 0) return false;

	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_NESTED;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,((MANDOLIN_MSG_REBOOT << 16)+(MANDOLIN_NESTED_TARGETID_BROOKLYN & 0x0ffff)));
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,uiRebootCode);

	pMsg->length = 2;
	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;
}
// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateRebootDSP1(mandolin_message* pMsg, uint8 uiSequence, uint32 uiRebootCode)
{
	uint8* pPayload = (uint8*)pMsg->payload;
	if (pMsg->payload == 0) return false;

	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_NESTED;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,((MANDOLIN_MSG_REBOOT << 16)+(MANDOLIN_NESTED_TARGETID_DSP1 & 0x0ffff)));
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,uiRebootCode);

	pMsg->length = 2;
	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;
}
// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateRebootDSP2(mandolin_message* pMsg, uint8 uiSequence, uint32 uiRebootCode)
{
	uint8* pPayload = (uint8*)pMsg->payload;
	if (pMsg->payload == 0) return false;

	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_NESTED;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,((MANDOLIN_MSG_REBOOT << 16)+(MANDOLIN_NESTED_TARGETID_DSP2 & 0x0ffff)));
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,uiRebootCode);

	pMsg->length = 2;
	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;
}
// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateUpdateStart(mandolin_message* pMsg, uint8 uiSequence )
{
	uint8* pPayload = (uint8*)pMsg->payload;
	if (pMsg->payload == 0) return false;

	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_TRIGGER_SYSTEM_EVENT;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_SE_BEGIN_UPDATE);

	pMsg->length = 1;
	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;
}

// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateUpdateEnd(mandolin_message* pMsg, uint8 uiSequence )
{
	uint8* pPayload = (uint8*)pMsg->payload;
	if (pMsg->payload == 0) return false;

	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_TRIGGER_SYSTEM_EVENT;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_SE_END_UPDATE);

	pMsg->length = 1;
	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;
}

// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateUpdateProcess(mandolin_message* pMsg, uint8 uiSequence )
{
	uint8* pPayload = (uint8*)pMsg->payload;
	if (pMsg->payload == 0) return false;

	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_TRIGGER_SYSTEM_EVENT;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_SE_PROCESS_FILES);

	pMsg->length = 1;
	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;
}

// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateLock(mandolin_message* pMsg, uint8 uiSequence, uint32 uiLockReason )
{
	//Additional payload:	0 -> NO REASON, 1 -> SNAPSHOT SYNC, 2 -> SHOW UPDATE
	//No further reply payload.

	uint8* pPayload = (uint8*)pMsg->payload;
	if (pMsg->payload == 0) return false;

	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_TRIGGER_SYSTEM_EVENT;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_SE_LOCK_CHANGES);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,uiLockReason);

	pMsg->length = 2;
	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;
}
// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateUnLock(mandolin_message* pMsg, uint8 uiSequence, uint32 uiUnLockReason )
{
	//Additional payload:	0 -> CONNECTED, 1 -> SNAPSHOT SYNCED, 2 -> SHOW UPDATED
	//No further reply payload.

	uint8* pPayload = (uint8*)pMsg->payload;
	if (pMsg->payload == 0) return false;

	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_TRIGGER_SYSTEM_EVENT;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_SE_UNLOCK_CHANGES);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,uiUnLockReason);

	pMsg->length = 2;
	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;
}

// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateIdentify(mandolin_message* pMsg, uint8 uiSequence, int nIndentifyPeriodMSec )
{
	// nIndentifyPeriodMSec = 0xFFFFFFFF (-1) specifies infinite duration; 
	// nIndentifyPeriodMSec = (0) specifies that the identification should immediately terminate.
	// nIndentifyPeriodMSec = amount of msec to be on
	uint8* pPayload = (uint8*)pMsg->payload;
	if (pMsg->payload == 0) return false;

	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_TRIGGER_SYSTEM_EVENT;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_SE_IDENTIFY);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,nIndentifyPeriodMSec);

	pMsg->length = 2;
	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;
}

// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateSyncState(mandolin_message* pMsg, uint8 uiSequence )
{
	uint8* pPayload = (uint8*)pMsg->payload;
	if (pMsg->payload == 0) return false;

	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_TRIGGER_SYSTEM_EVENT;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_SE_SYNC_CURRENT_STATE);

	pMsg->length = 1;
	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;
}

// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateFileOpen(mandolin_message* pMsg, uint8 uiSequence, char* pcFileName, uint32 uiFileType, bool bWrite )
{
	//Reply (success):	1 -> SUCCESS
	//		File ID.
	//Reply (failure):	0 -> FAIL
	//			0 -> NO REASON, 1 -> FILE LOCKED, 1 -> BAD TYPE, 2 -> BAD NAME

	uint8* pPayload = (uint8*)pMsg->payload;
	if (pMsg->payload == 0) return false;
	int nPos = 0;
	uint32 i;

	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_FILE_OPEN;
	uiFileType = (uiFileType&0x00ffffff) | (bWrite?0:0x80000000);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,uiFileType);
	nPos = 4;

	for(i=0;i<strlen(pcFileName);i++)
	{
		pPayload[nPos++] = pcFileName[i];
	}
	pPayload[nPos++] = 0;

	while ((nPos%4) != 0)
	{
		pPayload[nPos++] = 0;
		i++;
	}
	pMsg->length = nPos/4;

	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;

}

// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateFileClose(mandolin_message* pMsg, uint8 uiSequence,  uint32 uiFileId )
{
	uint8* pPayload = (uint8*)pMsg->payload;
	if (pMsg->payload == 0) return false;
	int nPos = 0;

	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_FILE_CLOSE;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,uiFileId);
	pMsg->length = 1;

	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;

}

// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateFileDelete(mandolin_message* pMsg, uint8 uiSequence,  uint32 uiFileId )
{
	uint8* pPayload = (uint8*)pMsg->payload;
	if (pMsg->payload == 0) return false;
	int nPos = 0;

	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_POST_FILE;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,uiFileId);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,0);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,0);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,3*4,0);

	pMsg->length = 4;

	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;

}
// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateFilePost(mandolin_message* pMsg, uint8 uiSequence,  uint32 uiFileId, uint32 uiFileByteSize, uint32 uiFileBytePos, uint32 uiByteLength, uint8* pData )
{
	// Reply:	0 -> DELETE, 1 -> POST.

	// To signal the end of a POST, send a POST with file offset equal to file size and zero bytes to post.


	uint8* pPayload = (uint8*)pMsg->payload;
	if (pMsg->payload == 0) return false;
	uint32 i = 0;

	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_POST_FILE;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,uiFileId);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,uiFileByteSize);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,uiFileBytePos);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,3*4,uiByteLength);

	for(i=0;i< uiByteLength;i++)
	{
		pPayload[16+i] = *pData++;
	}
	// pad for word align
	for(i=0;i< (int)((uiByteLength+3)/4)*4-uiByteLength;i++)
	{
		pPayload[uiByteLength+16+i] = 0;
	}
	pMsg->length = ((int)(uiByteLength+3)/4)+4;

	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;

}



// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateFileGet(mandolin_message* pMsg, uint8 uiSequence,  uint32 uiFileId, uint32 uiFileByteSize, uint32 uiFileBytePos, uint32 uiByteLength )
{
	// Reply:	FileID + File Size + File offset + Bytes to Get + File data.

	uint8* pPayload = (uint8*)pMsg->payload;
	if (pMsg->payload == 0) return false;

	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_GET_FILE;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,uiFileId);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,uiFileByteSize);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,uiFileBytePos);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,3*4,uiByteLength);

	
	pMsg->length = 4;

	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;

}

// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateFileInfo(mandolin_message* pMsg, uint8 uiSequence,  uint32 uiFileId )
{
	// returns:
	// 0 sub id
	// 1 file id
	// 2 file size
	// 3 status (0=fail, 1=success (with next word CRC), 2=busy (with next word percent)
	// 4 CRC (if status == success)  OR 
	// 4 percent busy (if status == busy)

	uint8* pPayload = (uint8*)pMsg->payload;
	if (pMsg->payload == 0) return false;

	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_GET_INFO;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_GI_FILE);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,uiFileId);
	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;

}


// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateSubscribeApplicationParameter(mandolin_message* pMsg, uint8 uiSequence,  uint32 uiRateMSec, uint32 uiListId, uint32 nTarget, bool bNoAck)
{
	// rate = 0 for off
	// usete nTarget = eTARGET_METERS and uiListID =1 for Anya all meters 

	uint8* pPayload = (uint8*)pMsg->payload;
	uint32 uiValue;
	if (pMsg->payload == 0) return false;

	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_SUBSCRIBE_APPLICATION_PARAMETER;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,uiListId);

	uiValue = ((0x0f<<MANDOLIN_WILDCARD_MAP_SHIFT) & MANDOLIN_WILDCARD_MAP_MASK) +
			 ((MANDOLIN_TYPE_LIST <<MANDOLIN_WILDCARD_TYPE_SHIFT) & MANDOLIN_WILDCARD_TYPE_MASK) +
			 ((nTarget <<MANDOLIN_EDIT_TARGET_SHIFT) & MANDOLIN_EDIT_TARGET_MASK); //  wildcard (next), wildcard number, meter space
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,uiValue);

	//uiValue = ((uiRateMSec & 0x0ffff) << 16) + 0x01;	// no ack
	uiValue = ((uiRateMSec & 0x0ffff) << 16);
	if (bNoAck)
	{
		uiValue += 0x01;
	}
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,uiValue);

	pMsg->length = 3;
	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;

}

// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateParameterList(mandolin_message* pMsg, uint8 uiSequence, int nListId, uint32 uiPIDs[], uint32 uiNumofParams )
{
	// uiNumofParams = 0 to delete the list

	uint8* pPayload = (uint8*)pMsg->payload;
	if (pMsg->payload == 0) return false;
	uint32 i;

	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_CREATE_PARAMETER_LIST;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,nListId);

	if (uiNumofParams == 0)
	{
		MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,0);
		pMsg->length = 2;
	}
	else
	{
		for(i=0;i<uiNumofParams;i++)
		{
			MANDOLIN_PAYLOAD_SETUINT32(pPayload,(i+1)*4,uiPIDs[i]);
		}
		pMsg->length = uiNumofParams+1;
	}
	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;

}
// -------------------------------------------------------------------------------------------------------


bool CMandolinComm::CreateGetBufferCRC(mandolin_message* pMsg, uint8 uiSequence)
{
	uint8* pPayload = (uint8*)pMsg->payload;
	if (pMsg->payload == 0) return false;

	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_GET_SYSTEM_STATUS;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_MSG_GET_SYSTEM_STATUS_CRC);

	pMsg->length = 1;
	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;
}


 bool CMandolinComm::CreateConnect(mandolin_message* pMsg, uint8 uiSequence, uint32 uiConnectType)
 {
 	uint8* pPayload = (uint8*)pMsg->payload;
	if (pMsg->payload == 0) return false;

	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_TRIGGER_SYSTEM_EVENT;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_SE_CONNECT);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,uiConnectType);

	pMsg->length = 2;
	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;
}
 bool CMandolinComm::CreateDisconnect(mandolin_message* pMsg, uint8 uiSequence, uint32 uiDisconnectReason)
 {
  	uint8* pPayload = (uint8*)pMsg->payload;
	if (pMsg->payload == 0) return false;

	pMsg->transport = 0;
	pMsg->id =  MANDOLIN_MSG_TRIGGER_SYSTEM_EVENT;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_SE_DISCONNECT);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,uiDisconnectReason);

	pMsg->length = 2;
	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;
 }


// -------------------------------------------------------------------------------------------------------
 bool CMandolinComm::CreateGeneric(mandolin_message* pMsg, uint8 uiSequence, uint8 uiMessageID, uint32 nNumPayload, uint32 uiPayload[])
 {
  	uint8* pPayload = (uint8*)pMsg->payload;
	if (pMsg->payload == 0) return false;

	pMsg->transport = 0;
	pMsg->id =  uiMessageID;

	for(int i=0;i<nNumPayload;i++)
	{
		MANDOLIN_PAYLOAD_SETUINT32(pPayload,i*4,uiPayload[i]);
	}
	pMsg->length = nNumPayload;
	if (nNumPayload == 0)
	{
		pMsg->payload = 0;
	}
	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;
 }
// -------------------------------------------------------------------------------------------------------

bool CMandolinComm::CreateMicSweepCalibrateStart(mandolin_message* pMsg, uint8 uiSequence, uint32 uiChannel, float fLevel, int nBurstLength)
{
	uint8* pPayload = (uint8*)pMsg->payload;
	intfloat j;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_CUSTOM_MICSWEEP);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,MANDOLIN_CUSTOM_MICSWEEP_CALIBRATE_START);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,uiChannel);
	j.f = fLevel;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,3*4,j.ui);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,4*4,nBurstLength);

	pMsg->length = 5;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;

}



bool CMandolinComm::CreateMicSweepCalibrateProgress(mandolin_message* pMsg, uint8 uiSequence)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_CUSTOM_MICSWEEP);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,MANDOLIN_CUSTOM_MICSWEEP_CALIBRATE_PROGRESS);

	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}
bool CMandolinComm::CreateMicSweepCalibrateStop(mandolin_message* pMsg, uint8 uiSequence)
{
	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_CUSTOM_MICSWEEP);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,MANDOLIN_CUSTOM_MICSWEEP_CALIBRATE_STOP);
	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;

}


// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateMicSweepSingleStart(mandolin_message* pMsg, uint8 uiSequence, uint32 uiChannel, float fLevel, int nBurstLength)
{
	uint8* pPayload = (uint8*)pMsg->payload;
	intfloat j;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_CUSTOM_MICSWEEP);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,MANDOLIN_CUSTOM_MICSWEEP_SINGLE_START);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,uiChannel);
	j.f = fLevel;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,3*4,j.ui);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,4*4,nBurstLength);

	pMsg->length = 5;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;

}

bool CMandolinComm::CreateMicSweepSingleProgress(mandolin_message* pMsg, uint8 uiSequence)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_CUSTOM_MICSWEEP);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,MANDOLIN_CUSTOM_MICSWEEP_SINGLE_PROGRESS);

	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}
bool CMandolinComm::CreateMicSweepSingleStop(mandolin_message* pMsg, uint8 uiSequence)
{
	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_CUSTOM_MICSWEEP);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,MANDOLIN_CUSTOM_MICSWEEP_SINGLE_STOP);
	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;

}
// -------------------------------------------------------------------------------------------------------

bool CMandolinComm::CreateMicSweepDSPStart(mandolin_message* pMsg, uint8 uiSequence, uint32 uiChannel, float fLevel, int nBurstLength)
{

	uint8* pPayload = (uint8*)pMsg->payload;
	intfloat j;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_CUSTOM_MICSWEEP);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,MANDOLIN_CUSTOM_MICSWEEP_DSP_START);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,uiChannel);
	j.f = fLevel;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,3*4,j.ui);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,4*4,nBurstLength);

	pMsg->length = 5;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

bool CMandolinComm::CreateMicSweepDSPProgress(mandolin_message* pMsg, uint8 uiSequence)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_CUSTOM_MICSWEEP);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,MANDOLIN_CUSTOM_MICSWEEP_DSP_PROGRESS);

	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

bool CMandolinComm::CreateMicSweepDSPStop(mandolin_message* pMsg, uint8 uiSequence)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_CUSTOM_MICSWEEP);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,MANDOLIN_CUSTOM_MICSWEEP_DSP_STOP);

	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}
//MANDOLIN_CUSTOM_MICSWEEP_CALIBRATE_START = 1,	// channel # (1..22 or 0 for all), returns ok/no
//MANDOLIN_CUSTOM_MICSWEEP_CALIBRATE_PROGRESS,	// no args, returns  no, yes, busy with percent
//MANDOLIN_CUSTOM_MICSWEEP_CALIBRATE_STOP,		// no args, returns  no, yes
//MANDOLIN_CUSTOM_MICSWEEP_SINGLE_START,		// channel # (1..22), returns ok/no
//MANDOLIN_CUSTOM_MICSWEEP_SINGLE_PROGRESS,	// no args, returns  no, yes, busy with percent
//MANDOLIN_CUSTOM_MICSWEEP_SINGLE_STOP,		// no args, returns  no, yes
//MANDOLIN_CUSTOM_MICSWEEP_DSP_START,		// channel # (1..22), returns ok/no and if ok which DSP is implementing sweep A/B
//MANDOLIN_CUSTOM_MICSWEEP_DSP_PROGRESS,	// no args, returns  no, yes, busy with percent
//MANDOLIN_CUSTOM_MICSWEEP_DSP_STOP,		// no args, returns  no, yes



// -----------------------------------------------------------------------------------------------------------------

bool CMandolinComm::CreateIRDANeighborListReset(mandolin_message* pMsg, uint8 uiSequence)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_CUSTOM_IRDA);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,MANDOLIN_CUSTOM_IRDA_NEIGHBOR_LIST_RESET);

	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}
bool CMandolinComm::CreateIRDANeighborListGet(mandolin_message* pMsg, uint8 uiSequence)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_CUSTOM_IRDA);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,MANDOLIN_CUSTOM_IRDA_NEIGHBOR_LIST_GET);

	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

bool CMandolinComm::CreateIRDANeighborTransmitHardwareInfo(mandolin_message* pMsg, uint8 uiSequence, int nPos, uint32 uiStartTime, uint32 uiEndTime)
{

	uint8* pPayload = (uint8*)pMsg->payload;
	intfloat j;
	j.i = nPos;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_CUSTOM_IRDA);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,MANDOLIN_CUSTOM_IRDA_NEIGHBORTRANSMIT_HARDWAREINFO);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,j.ui);

	pMsg->length = 3;

	if ((uiStartTime != 0) || (uiEndTime != 0))
	{
		MANDOLIN_PAYLOAD_SETUINT32(pPayload,3*4,uiStartTime);
		MANDOLIN_PAYLOAD_SETUINT32(pPayload,4*4,uiEndTime);
		pMsg->length = 5;
	}

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}	

bool CMandolinComm::CreateIRDANeighborRequestHardwareInfo(mandolin_message* pMsg, uint8 uiSequence, int nPos, uint32 uiStartTime, uint32 uiEndTime)
{

	uint8* pPayload = (uint8*)pMsg->payload;
	intfloat j;
	j.i = nPos;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_CUSTOM_IRDA);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,MANDOLIN_CUSTOM_IRDA_NEIGHBORREQUEST_HARDWAREINFO);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,j.ui);

	pMsg->length = 3;

	if ((uiStartTime != 0) || (uiEndTime != 0))
	{
		MANDOLIN_PAYLOAD_SETUINT32(pPayload,3*4,uiStartTime);
		MANDOLIN_PAYLOAD_SETUINT32(pPayload,4*4,uiEndTime);
		pMsg->length = 5;
	}

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

bool  CMandolinComm::CreateIRDANeighborInterrogate(mandolin_message* pMsg, uint8 uiSequence, int nPos, uint32 uiHardwareInfoHi, uint32 uiHardwareInfoLo)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_CUSTOM_IRDA);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,MANDOLIN_CUSTOM_IRDA_INTERROGATE_NEIGHBOR);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,nPos);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,3*4,uiHardwareInfoHi);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,4*4,uiHardwareInfoLo);

	pMsg->length = 5;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}
bool  CMandolinComm::CreateIRDASendEcho(mandolin_message* pMsg, uint8 uiSequence, int nPos, uint8* puiByteValues, uint32 uiByteLength)
{

	// uiByteLength must be divisible by 4
	uint32 i;
	uint8* pPayload = (uint8*)pMsg->payload;
	uint32 nWords;

	nWords = (uiByteLength+3)/4;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_CUSTOM_IRDA);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,MANDOLIN_CUSTOM_IRDA_SENDECHO);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,nPos);

	for(i=0;i<uiByteLength;i++)
	{
		pPayload[3*4+i] = puiByteValues[i];
	}
	for(;i<nWords*4;i++)
	{
		pPayload[3*4+i]  = 0;
	}
	pMsg->length = 3+nWords;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}
bool  CMandolinComm::CreateIRDAEcho(mandolin_message* pMsg, uint8 uiSequence, uint8* puiByteValues, uint32 uiByteLength)
{
	// uiByteLength must be divisible by 4
	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_CUSTOM_IRDA);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,MANDOLIN_CUSTOM_IRDA_SENDECHO);

	for(unsigned int i=0;i<(uiByteLength/4);i++)
	{
		pPayload[2*4+i] = puiByteValues[i];
	}
	pMsg->length = 2+uiByteLength/4;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateDiagnosticFaultMsg(mandolin_message *pMsg, uint8 uiSequence, uint8 channel)
{
	pMsg->id = MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;

	uint32 uiChannel = channel;
	uint8* pPayload  = (uint8*)pMsg->payload;

	MANDOLIN_PAYLOAD_SETUINT32(pPayload, 0*4, MANDOLIN_CUSTOM_DIAGNOSTIC);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload, 1*4, MANDOLIN_CUSTOM_DIAGNOSTIC_CHAN_FAULT_FAST);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload, 2*4, uiChannel);

	pMsg->length = 3;
	
	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}


// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateTimeSetMsg(mandolin_message *pMsg, uint8 uiSequence, 
									 uint16 uiYear, uint8 uiMonth, uint8 uiDay, uint8 uiHour, uint8 uiMin, uint8 uiSec)
{

	
	uint8* pPayload = (uint8*)pMsg->payload;
	uint32 uiValue;

	pMsg->id = MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;

	MANDOLIN_PAYLOAD_SETUINT32(pPayload, 0*4, MANDOLIN_CUSTOM_TIME);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload, 1*4, MANDOLIN_CUSTOM_TIME_SET);

	uiValue = ((uiYear & 0x0ffff) << 16) + ((uiMonth & 0x0ff)<<8)  + (uiDay & 0x0ff);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload, 2*4, uiValue);

	uiValue = ((uiHour & 0x0ff) << 16) + ((uiMin & 0x0ff)<<8)  + (uiSec & 0x0ff);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload, 3*4, uiValue);

	pMsg->length = 4;
	
	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateTimeGetMsg(mandolin_message *pMsg, uint8 uiSequence)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	pMsg->id = MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;

	MANDOLIN_PAYLOAD_SETUINT32(pPayload, 0*4, MANDOLIN_CUSTOM_TIME);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload, 1*4, MANDOLIN_CUSTOM_TIME_GET);
	pMsg->length = 2;
	
	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}


// -------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateTiltSensorGetMsg(mandolin_message *pMsg, uint8 uiSequence)
{
	uint8* pPayload = (uint8*)pMsg->payload;

	pMsg->id = MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;

	MANDOLIN_PAYLOAD_SETUINT32(pPayload, 0*4, MANDOLIN_CUSTOM_TILTSENSOR);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload, 1*4, MANDOLIN_CUSTOM_TILTSENSOR_GET);
	pMsg->length = 2;
	
	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

bool  CMandolinComm::CreateTiltSensorCalibrateMsg(mandolin_message *pMsg, uint8 uiSequence, uint32 uiCalibratePos)
{
	uint8* pPayload = (uint8*)pMsg->payload;

	uiCalibratePos &= 0x3;	// 0(all) 1=x, 2=y
	pMsg->id = MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;

	MANDOLIN_PAYLOAD_SETUINT32(pPayload, 0*4, MANDOLIN_CUSTOM_TILTSENSOR);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload, 1*4,  MANDOLIN_CUSTOM_TILTSENSOR_CALIBRATE);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload, 2*4,  uiCalibratePos);

	pMsg->length = 3;
	
	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

bool  CMandolinComm::CreateTiltSensorSetMsg(mandolin_message *pMsg, uint8 uiSequence, float fTiltX, float fTiltY, float fTiltZ)
{
	
	
	uint8* pPayload = (uint8*)pMsg->payload;
	intfloat jTiltX;
	intfloat jTiltY;
	intfloat jTiltZ;

	jTiltX.f = fTiltX; 
	jTiltY.f = fTiltY; 
	jTiltZ.f = fTiltZ; 


	pMsg->id = MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;

	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_CUSTOM_TILTSENSOR);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,MANDOLIN_CUSTOM_TILTSENSOR_SET);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,jTiltX.ui);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,3*4,jTiltY.ui);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,4*4,jTiltZ.ui);

	pMsg->length = 5;
	
	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

// -----------------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateSetModelType(mandolin_message *pMsg, uint8 uiSequence, uint32 uiModelType)
{
	uint8* pPayload = (uint8*)pMsg->payload;


	pMsg->id = MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;

	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_CUSTOM_SETMODELTYPE);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,uiModelType);

	pMsg->length = 2;
	
	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;

}

// -----------------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateUpdateFromLocalFile(mandolin_message *pMsg, uint8 uiSequence, char* pcFileName, uint32 uiFileFlags)
{


	uint8* pPayload = (uint8*)pMsg->payload;
	int nPos;
	int i;



	pMsg->id = MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;

	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_CUSTOM_UPDATEFROMLOCALFILE);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,uiFileFlags);
	nPos = 8;

	for(i=0;i<strlen(pcFileName);i++)
	{
		pPayload[nPos++] = pcFileName[i];
	}
	pPayload[nPos++] = 0;

	while ((nPos%4) != 0)
	{
		pPayload[nPos++] = 0;
		i++;
	}
	pMsg->length = nPos/4;

	MANDOLIN_MSG_pack(pMsg, uiSequence);
	return true;

}
// -----------------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateGetHardwareInfoDetailsDSP(mandolin_message *pMsg, uint8 uiSequence)
{
	uint8* pPayload = (uint8*)pMsg->payload;


	pMsg->id = MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;

	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_CUSTOM_GETHARDWAREINFODETAILS);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,MANDOLIN_CUSTOM_GETHARDWAREINFODETAILS_DSP);

	pMsg->length = 2;
	
	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

// -----------------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateGetHardwareInfoDetailsGov(mandolin_message *pMsg, uint8 uiSequence)
{
	uint8* pPayload = (uint8*)pMsg->payload;


	pMsg->id = MANDOLIN_MSG_CUSTOM;
	pMsg->transport = 0;

	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,MANDOLIN_CUSTOM_GETHARDWAREINFODETAILS);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,MANDOLIN_CUSTOM_GETHARDWAREINFODETAILS_GOV);

	pMsg->length = 2;
	
	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}


// -----------------------------------------------------------------------------------------------------------------
bool CMandolinComm::CreateTestDSPSetFuncGenOutput(mandolin_message* pMsg, uint8 uiSequence,int nChannel)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_SET_FUNC_GEN_OUTPUT);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,nChannel);

	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestDSPSetFuncGen(mandolin_message* pMsg, uint8 uiSequence,int nType,float fAmplitude, float fFreq)
{

	uint8* pPayload = (uint8*)pMsg->payload;
	intfloat j;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_SET_FUNC_GEN);
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
bool CMandolinComm::CreateTestDSPSetRMSMeterInput(mandolin_message* pMsg, uint8 uiSequence,int nType)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_SET_RMS_METER_INPUT);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,nType);

	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}



// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestDSPGetRMSMeterLevel(mandolin_message* pMsg, uint8 uiSequence)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_GET_RMS_METER_LEVEL);

	pMsg->length = 1;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestDSPGetRMSMeterRange(mandolin_message* pMsg, uint8 uiSequence, float fMin, float fMax)
{

	uint8* pPayload = (uint8*)pMsg->payload;
	intfloat j;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_GET_RMS_METER_RANGE);
	j.f = fMin;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,j.ui);
	j.f = fMax;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,j.ui);

	pMsg->length = 3;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}


// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestDSPWriteIO(mandolin_message* pMsg, uint8 uiSequence,int nIndex, int nValue)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_WRITEIO);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,nIndex);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,nValue);

	pMsg->length = 3;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

 
// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestDSPReadIO(mandolin_message* pMsg, uint8 uiSequence, int nIndex)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_READIO);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,nIndex);

	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestDSPReadAmpTemperature(mandolin_message* pMsg, uint8 uiSequence,int nIndex)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_READ_AMP_TEMPERATURE);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,nIndex);

	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}


// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestDSPReadAmpImpedance(mandolin_message* pMsg, uint8 uiSequence,int nIndex)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_READ_AMP_IMPEDANCE);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,nIndex);

	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}
 
// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestDSPReadSDRAMTestStatus(mandolin_message* pMsg, uint8 uiSequence)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_READ_SD_RAM_TEST_STATUS);

	pMsg->length = 1;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

// ----------------------------------------------------------------------------------

bool CMandolinComm::CreateTestGovnKernel(mandolin_message* pMsg, uint8 uiSequence)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_GOVN_KERNEL);

	pMsg->length = 1;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}


// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestGovnRAM(mandolin_message* pMsg, uint8 uiSequence)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_GOVN_RAM);

	pMsg->length = 1;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

 
// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestGovnSDCard(mandolin_message* pMsg, uint8 uiSequence)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_GOVN_SD_CARD);

	pMsg->length = 1;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

 
// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestGovnNANDFlash(mandolin_message* pMsg, uint8 uiSequence)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_GOVN_NAND_FLASH);

	pMsg->length = 1;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

  
// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestGovnRTC(mandolin_message* pMsg, uint8 uiSequence)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_GOVN_RTC);

	pMsg->length = 1;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}


// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestGovnDebugUART(mandolin_message* pMsg, uint8 uiSequence)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_GOVN_DEBUG_UART);

	pMsg->length = 1;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestGovnIRDA(mandolin_message* pMsg, uint8 uiSequence, int nIndex)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_GOVN_IRDA);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,nIndex);

	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestGovnIRDAInternal(mandolin_message* pMsg, uint8 uiSequence, int nIndex)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_GOVN_IRDAINTERNAL);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,nIndex);

	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestGovnTemperatureDye(mandolin_message* pMsg, uint8 uiSequence,int nTemp, int nTolerance)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_GOVN_TEMPERATURE_DYE);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,nTemp);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,nTolerance);

	pMsg->length = 3;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestGovnTemperatureOnBoard(mandolin_message* pMsg, uint8 uiSequence,int nTemp, int nTolerance)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_GOVN_TEMPERATURE_ON_BOARD);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,nTemp);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,nTolerance);

	pMsg->length = 3;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}



// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestGovnReadFPButtons(mandolin_message* pMsg, uint8 uiSequence,uint32 uiMode)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_GOVN_READ_FP_BUTTONS);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,uiMode);

	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

 
// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestGovnSetFPLEDs(mandolin_message* pMsg, uint8 uiSequence,int nOnOffMask, int nColorMask)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_GOVN_SET_FP_LEDS);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,nOnOffMask);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,nColorMask);

	pMsg->length = 3;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestGovnSetFPStatusLED(mandolin_message* pMsg, uint8 uiSequence, int nValue)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_GOVN_SET_FP_STATUSLED);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,nValue);

	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestGovnAccelerometer(mandolin_message* pMsg, uint8 uiSequence, float fX, float fY,  
												float fXTolerance, float fYTolerance )
{

	uint8* pPayload = (uint8*)pMsg->payload;
	intfloat j;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_GOVN_ACCELEROMETER);
	j.f = fX;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,j.ui);
	j.f = fY;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,2*4,j.ui);
	j.f = fXTolerance;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,3*4,j.ui);
	j.f = fYTolerance;
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,4*4,j.ui);

	pMsg->length = 5;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}


// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestGovnBrooklynComms(mandolin_message* pMsg, uint8 uiSequence)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_GOVN_BROOKLYN_COMMS);

	pMsg->length = 1;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestGovnDSPComms(mandolin_message* pMsg, uint8 uiSequence, int nDSPNum)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_GOVN_DSP_COMMS);
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,1*4,nDSPNum);

	pMsg->length = 2;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestGovnUSBHost(mandolin_message* pMsg, uint8 uiSequence)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_GOVN_USB_HOST);

	pMsg->length = 1;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestGovnUSBDevice(mandolin_message* pMsg, uint8 uiSequence)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_GOVN_USB_DEVICE);

	pMsg->length = 1;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

// ----------------------------------------------------------------------------------
bool CMandolinComm::CreateTestGovnBattery(mandolin_message* pMsg, uint8 uiSequence)
{

	uint8* pPayload = (uint8*)pMsg->payload;

	if (pMsg->payload == 0) return false;

	pMsg->id =  MANDOLIN_MSG_TEST;
	pMsg->transport = 0;
	
	MANDOLIN_PAYLOAD_SETUINT32(pPayload,0*4,ANYA_TESTID_GOVN_BATTERY);

	pMsg->length = 1;

	MANDOLIN_MSG_pack(pMsg, uiSequence);

	return true;
}

// ----------------------------------------------------------------------------------
 