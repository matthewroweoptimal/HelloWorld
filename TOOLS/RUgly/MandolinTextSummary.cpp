#include "stdafx.h"

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

extern "C"
{
#include "ProductMap.h"
}

#include "OLY_MT_enums.h"
#include "MandolinTextSummary.h"
#include "RUglyDlg.h"

void mht_convertTestID2Text(int nID,char* pcBuffer,int nMaxLen);


CMandolinTextSummary::CMandolinTextSummary()
{
	m_dlgParent = 0;
}


bool CMandolinTextSummary::MANDOLIN_handle_bHasSubId(int nMsgId)
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




void CMandolinTextSummary::MANDOLIN_handle_Error2Text(char *pDest, int mError)
{
	pDest[0] = 0;
	switch (mError)
	{
	case MANDOLIN_NO_ERROR_NO_FURTHER_REPLY 	 :	strcpy(pDest,"MANDOLIN_NO_ERROR_NO_FURTHER_REPLY"); break;
	case MANDOLIN_NO_ERROR_FORWARD_IN_PLACE 	 :	strcpy(pDest,"MANDOLIN_NO_ERROR_FORWARD_IN_PLACE"); break;
	case MANDOLIN_NO_ERROR						 :	strcpy(pDest,"MANDOLIN_NO_ERROR"); break;
	case MANDOLIN_ERROR 						 :	strcpy(pDest,"MANDOLIN_ERROR"); break;
	case MANDOLIN_ERROR_BUSY					 :	strcpy(pDest,"MANDOLIN_ERROR_BUSY"); break;
	case MANDOLIN_ERROR_LOCKED					 :	strcpy(pDest,"MANDOLIN_ERROR_LOCKED"); break;
	case MANDOLIN_ERROR_OUT_OF_MEMORY			 :	strcpy(pDest,"MANDOLIN_ERROR_OUT_OF_MEMORY"); break;
	case MANDOLIN_ERROR_TIMEOUT 				 :	strcpy(pDest,"MANDOLIN_ERROR_TIMEOUT"); break;
	case MANDOLIN_ERROR_CHECKSUM_MISMATCH		 :	strcpy(pDest,"MANDOLIN_ERROR_CHECKSUM_MISMATCH"); break;
	case MANDOLIN_ERROR_LENGTH_MISMATCH 		 :	strcpy(pDest,"MANDOLIN_ERROR_LENGTH_MISMATCH"); break;
	case MANDOLIN_ERROR_UNIMPLEMENTED_MSGID 	 :	strcpy(pDest,"MANDOLIN_ERROR_UNIMPLEMENTED_MSGID"); break;
	case MANDOLIN_ERROR_INVALID_MSGID			 :	strcpy(pDest,"MANDOLIN_ERROR_INVALID_MSGID"); break;
	case MANDOLIN_ERROR_INVALID_FLAGS			 :	strcpy(pDest,"MANDOLIN_ERROR_INVALID_FLAGS"); break;
	case MANDOLIN_ERROR_INVALID_ID				 :	strcpy(pDest,"MANDOLIN_ERROR_INVALID_ID"); break;
	case MANDOLIN_ERROR_INVALID_DATA			 :	strcpy(pDest,"MANDOLIN_ERROR_INVALID_DATA"); break;
	case MANDOLIN_ERROR_FLASH_ERASE 			 :	strcpy(pDest,"MANDOLIN_ERROR_FLASH_ERASE"); break;
	case MANDOLIN_ERROR_FLASH_WRITE 			 :	strcpy(pDest,"MANDOLIN_ERROR_FLASH_WRITE"); break;
	case MANDOLIN_ERROR_FLASH_READ				 :	strcpy(pDest,"MANDOLIN_ERROR_FLASH_READ"); break;

	default 									 :	strcpy(pDest,"MANDOLIN_ERROR_UNKNOWN"); 	break;
	}
	return;
	
}


// ----------------------------------------------------------------------------------------------------------
void CMandolinTextSummary::MANDOLIN_handle_MsgId2Text(char* pDest, int nId, int nSubId)
{
	char pcBuffer[100];
	pDest[0] = 0;
	
	switch(nId)
	{
	case MANDOLIN_MSG_INVALID:						strcpy(pDest,"INVALID"); break; 					
	case MANDOLIN_MSG_PING: 						strcpy(pDest,"PING"); break;							
	case MANDOLIN_MSG_ADVERTIZE_HARDWARE_INFO:		strcpy(pDest,"ADVERTIZE_HARDWARE_INFO"); break; 	
	case MANDOLIN_MSG_GET_HARDWARE_INFO:			strcpy(pDest,"GET_HARDWARE_INFO"); break;			
	case MANDOLIN_MSG_GET_SOFTWARE_INFO:			strcpy(pDest,"GET_SOFTWARE_INFO"); break;			
	case MANDOLIN_MSG_REBOOT:						strcpy(pDest,"REBOOT"); break;						
	case MANDOLIN_MSG_TRIGGER_SYSTEM_EVENT: 		strcpy(pDest,"TRIGGER_SYSTEM_EVENT");
		switch(nSubId)
		{
		case	MANDOLIN_SE_CONNECT:				strcat(pDest," - CONNECT"); break;				
		case	MANDOLIN_SE_DISCONNECT: 			strcat(pDest," - DISCONNECT"); break;			
		case	MANDOLIN_SE_LOCK_CHANGES:			strcat(pDest," - LOCK_CHANGES"); break; 		
		case	MANDOLIN_SE_UNLOCK_CHANGES: 		strcat(pDest," - UNLOCK_CHANGES"); break;		
		case	MANDOLIN_SE_IDENTIFY:				strcat(pDest," - IDENTIFY"); break; 			
		case	MANDOLIN_SE_SYNC_CURRENT_STATE: 	strcat(pDest," - SYNC_CURRENT_STATE"); break;	
		case	MANDOLIN_SE_BEGIN_UPDATE:			strcat(pDest," - BEGIN_UPDATE"); break; 		
		case	MANDOLIN_SE_END_UPDATE: 			strcat(pDest," - END_UPDATE"); break;			
		case	MANDOLIN_SE_PROCESS_FILES:			strcat(pDest," - PROCESS_FILES"); break;		
		default:
			break;			  
		}
		break;	
	case MANDOLIN_MSG_TRIGGER_APPLICATION_EVENT:	strcpy(pDest,"TRIGGER_APPLICATION_EVENT");
		switch(nSubId)
		{
		case	MANDOLIN_AE_LOAD_SNAPSHOT:			strcat(pDest," - LOAD_SNAPSHOT"); break;
		case	MANDOLIN_AE_STORE_SNAPSHOT: 		strcat(pDest," - STORE_SNAPSHOT"); break;
		case	MANDOLIN_AE_DELETE_SNAPSHOT: 		strcat(pDest," - DELETE_SNAPSHOT"); break;
		case	MANDOLIN_AE_MOVE_SNAPSHOT:			strcat(pDest," - MOVE_SNAPSHOT"); break;
		case	MANDOLIN_AE_NAME_SNAPSHOT:			strcat(pDest," - NAME_SNAPSHOT"); break;
		case	MANDOLIN_AE_PUSH_SHOW:				strcat(pDest," - PUSH_SHOW"); break;
		case	MANDOLIN_AE_PULL_SHOW:				strcat(pDest," - PULL_SHOW"); break;	
		case	MANDOLIN_AE_NAME_SHOW:				strcat(pDest," - NAME_SHOW"); break;	

		case	MANDOLIN_AE_STORE_USER_EDIT:		strcat(pDest," - STORE USER EDIT"); break;	
		case	MANDOLIN_AE_CANCEL_USER_EDIT:		strcat(pDest," - CANCEL USER EDIT"); break;	
		case	MANDOLIN_AE_SET_AVAILABLE:			strcat(pDest," - SET AVAIL"); break;	
		case	MANDOLIN_AE_SET_SELF_TEST_MODE:		strcat(pDest," - SELF TEST MODE"); break;	
		case	MANDOLIN_AE_START_AUTO_OPTIMIZE:	strcat(pDest," - START AUTO OPTIMIZE"); break;	
		case	MANDOLIN_AE_RESET_AUTO_OPTIMIZE:	strcat(pDest," - RESET AUTO OPTIMIZE"); break;	

		default:									
			break;									
		}
		break;
	case MANDOLIN_MSG_TEST: 						strcpy(pDest,"TEST"); 
		if (nSubId != 0)
		{
			pcBuffer[0] = 0;
			mht_convertTestID2Text(nSubId,pcBuffer,sizeof(pcBuffer)-1);
			strcat(pDest," - ");
			strcat(pDest,pcBuffer);
		}
		break;							
	case MANDOLIN_MSG_GET_INFO: 					strcpy(pDest,"GET_INFO");
		switch(nSubId)
		{
		case	MANDOLIN_GI_FILE:					strcat(pDest," - FILE"); break;
		case	MANDOLIN_GI_LIST:					strcat(pDest," - LIST"); break;
		case	MANDOLIN_GI_SNAPSHOT:				strcat(pDest," - SNAPSHOT"); break;
		case	MANDOLIN_GI_SHOW:					strcat(pDest," - SHOW"); break;
		default:									
			break;									

		}
		break;
	case MANDOLIN_MSG_FILE_OPEN:					strcpy(pDest,"FILE_OPEN"); break;					
	case MANDOLIN_MSG_FILE_CLOSE:					strcpy(pDest,"FILE_CLOSE"); break;					
	case MANDOLIN_MSG_GET_FILE: 					strcpy(pDest,"GET_FILE"); break;						
	case MANDOLIN_MSG_POST_FILE:					strcpy(pDest,"POST_FILE"); break;					
	case MANDOLIN_MSG_GET_SYSTEM_STATUS:			strcpy(pDest,"GET_SYSTEM_STATUS");
		switch(nSubId)
		{
		case	MANDOLIN_SS_BOOT_CODE:				strcat(pDest," - BOOT_CODE"); break;
		case	MANDOLIN_SS_MAX_MSG_WORDS:			strcat(pDest," - MAX_MSG_WORDS"); break;
		case	MANDOLIN_SS_SYSTEM_LABEL:			strcat(pDest," - SYSTEM_LABEL"); break;
		case	MANDOLIN_SS_SYSTEM_IP_ADDRESS:		strcat(pDest," - SYSTEM_IP_ADDRESS"); break;
		default:		
			break;			
		}
		break;
	case MANDOLIN_MSG_SET_SYSTEM_STATE: 			strcpy(pDest,"SET_SYSTEM_STATE"); 
		switch(nSubId)
		{
		case	MANDOLIN_SS_BOOT_CODE:				strcat(pDest," - BOOT_CODE"); break;
		case	MANDOLIN_SS_MAX_MSG_WORDS:			strcat(pDest," - MAX_MSG_WORDS"); break;
		case	MANDOLIN_SS_SYSTEM_LABEL:			strcat(pDest," - SYSTEM_LABEL"); break;
		case	MANDOLIN_SS_SYSTEM_IP_ADDRESS:		strcat(pDest," - SYSTEM_IP_ADDRESS"); break;
		default:		
			break;			
		}
		break;
	case MANDOLIN_MSG_RESET_SYSTEM_STATE:			strcpy(pDest,"RESET_SYSTEM_STATE"); break;			
	case MANDOLIN_MSG_SUBSCRIBE_SYSTEM_STATE:		strcpy(pDest,"SUBSCRIBE_SYSTEM_STATE"); break;		
	case MANDOLIN_MSG_GET_APPLICATION_PARAMETER:	strcpy(pDest,"GET_APPLICATION_PARAMETER"); break;	
	case MANDOLIN_MSG_SET_APPLICATION_PARAMETER:	strcpy(pDest,"SET_APPLICATION_PARAMETER"); break;	
	case MANDOLIN_MSG_RESET_APPLICATION_PARAMETER:	strcpy(pDest,"RESET_APPLICATION_PARAMETER"); break; 
	case MANDOLIN_MSG_SUBSCRIBE_APPLICATION_PARAMETER:strcpy(pDest,"SUBSCRIBE_APPLICATION_PARAMETER"); break;
	case MANDOLIN_MSG_CREATE_PARAMETER_LIST:		strcpy(pDest,"CREATE_PARAMETER_LIST"); break;		
	case MANDOLIN_MSG_GET_APPLICATION_STRING:		strcpy(pDest,"GET_APPLICATION_STRING"); break;		
	case MANDOLIN_MSG_SET_APPLICATION_STRING:		strcpy(pDest,"SET_APPLICATION_STRING"); break;		
	case MANDOLIN_MSG_RESET_APPLICATION_STRING: 	strcpy(pDest,"RESET_APPLICATION_STRING"); break;

	case MANDOLIN_MSG_CUSTOM:						strcpy(pDest,"CUSTOM");
		switch(nSubId)
		{
		case	MANDOLIN_CUSTOM_MAKE_FACTORY:		strcat(pDest," - MAKE_FACTORY"); break;
		case	MANDOLIN_CUSTOM_ECHO:				strcat(pDest," - ECHO"); break;
		case	MANDOLIN_CUSTOM_FLASH_WRITE:		strcat(pDest," - FLASH_WRITE"); break;
		case	MANDOLIN_CUSTOM_FLASH_ERASE:		strcat(pDest," - FLASH_ERASE"); break;
		case	MANDOLIN_CUSTOM_FLASH_VERIFY:		strcat(pDest," - FLASH_VERIFY"); break;
		case	MANDOLIN_CUSTOM_LINUX_COMMAND:		strcat(pDest," - LINUX_COMMAND"); break;
		case	MANDOLIN_CUSTOM_MICSWEEP:			strcat(pDest," - MICSWEEP"); break;
		case	MANDOLIN_CUSTOM_MEMORY_READ:		strcat(pDest," - MEMORY_READ"); break;
		case	MANDOLIN_CUSTOM_MEMORY_WRITE:		strcat(pDest," - MEMORY_WRITE"); break;
		case	MANDOLIN_CUSTOM_IRDA:				strcat(pDest," - IRDA"); break;
		case	MANDOLIN_CUSTOM_DIAGNOSTIC:			strcat(pDest," - DIAGNOSTIC"); break;
		case	MANDOLIN_CUSTOM_TIME:				strcat(pDest," - TIME"); break;
		case	MANDOLIN_CUSTOM_TILTSENSOR:			strcat(pDest," - TILTSENSOR"); break;
		case	MANDOLIN_CUSTOM_GETHARDWAREINFODETAILS: strcat(pDest," - GETHARDWAREINFODETAILS"); break;
		case	MANDOLIN_CUSTOM_SETMODELTYPE: 		strcat(pDest," - SETMODELTYPE"); break;				
		case	MANDOLIN_CUSTOM_UPDATEFROMLOCALFILE: strcat(pDest," - UPDATEFROMLOCALFILE"); break;				

		default:		
			break;			
		}
		break;

	default:
		strcpy(pDest,"INVALID"); break;

		break;
	}


}


// ----------------------------------------------------------------------------------------
void CMandolinTextSummary::MANDOLIN_handle_MsgTextSummary_GET_SOFTWARE_INFO_REPLY(char *pDest, mandolin_message* pMsg, int nMaxLength)
{

	uint32  flags;
    uint32 *in_ptr = (uint32 *) pMsg->payload;
	int nMsgLength;
	int nMsgId;
	char pcBuffer[100];

	*pDest = 0;

	nMsgLength = pMsg->length;
	nMsgId = pMsg->id;
   	if (nMsgId == MANDOLIN_MSG_NESTED)
	{
		nMsgId = ((in_ptr[0] >> 16 ) & 0x0ffff);
		in_ptr += 1;
		nMsgLength--;
	}

	if (nMsgLength)
	{
		sprintf(pcBuffer,"XML Version = 0x%8.8x\n",*in_ptr++);
		strcat(pDest,pcBuffer);
		nMsgLength--;
	}

	// app id
	if (nMsgLength>1)
	{
		nMsgLength -= 2;
		pcBuffer[0] = 0;
		switch(*in_ptr++)
		{
		case ANYA_APPID_UNDEFINED:		sprintf(pcBuffer,"ANYA_APPID_UNDEFINED = 0x%8.8x\n",*in_ptr++); break;	
		case ANYA_APPID_WHITEAPP:		sprintf(pcBuffer,"ANYA_APPID_WHITEAPP = 0x%8.8x\n",*in_ptr++); break;	
		case ANYA_APPID_GOVNAPP:		sprintf(pcBuffer,"ANYA_APPID_GOVNAPP = 0x%8.8x\n",*in_ptr++); break;	
		case ANYA_APPID_BROOKLYNAPP:	sprintf(pcBuffer,"ANYA_APPID_BROOKLYNAPP = 0x%8.8x\n",*in_ptr++); break;
		case ANYA_APPID_DSPBOOT:		sprintf(pcBuffer,"ANYA_APPID_DSPBOOT = 0x%8.8x\n",*in_ptr++); break;	
		case ANYA_APPID_DSPOS:			sprintf(pcBuffer,"ANYA_APPID_DSPOS = 0x%8.8x\n",*in_ptr++); break;
		case ANYA_APPID_DSPTST:			sprintf(pcBuffer,"ANYA_APPID_DSPTST = 0x%8.8x\n",*in_ptr++); break;
		default:
			in_ptr++;
			break;
		}
		strcat(pDest,pcBuffer);

	}
	while(nMsgLength>1)
	{
		nMsgLength -= 2;
		pcBuffer[0] = 0;
		switch(*in_ptr++)
		{
		case ANYA_FIRM_UNDEFINED:	sprintf(pcBuffer,"ANYA_FIRM_UNDEFINED = 0x%8.8x\n",*in_ptr++); break;
		case ANYA_FIRM_MINI_OS:		sprintf(pcBuffer,"ANYA_FIRM_MINI_OS = 0x%8.8x\n",*in_ptr++); break;
		case ANYA_FIRM_MINI_APP:	sprintf(pcBuffer,"ANYA_FIRM_MINI_APP = 0x%8.8x\n",*in_ptr++); break;
		case ANYA_FIRM_MINI_INTERFACE: sprintf(pcBuffer,"ANYA_FIRM_MINI_INTERFACE = 0x%8.8x\n",*in_ptr++); break;
		case ANYA_FIRM_APP:			sprintf(pcBuffer,"ANYA_FIRM_APP = 0x%8.8x\n",*in_ptr++); break;
		case ANYA_FIRM_INTERFACE:	sprintf(pcBuffer,"ANYA_FIRM_INTERFACE = 0x%8.8x\n",*in_ptr++); break;
		case ANYA_FIRM_DSP:			sprintf(pcBuffer,"ANYA_FIRM_DSP = 0x%8.8x\n",*in_ptr++); break;
		case ANYA_FIRM_OS:			sprintf(pcBuffer,"ANYA_FIRM_OS(KERNEL) = 0x%8.8x\n",*in_ptr++); break;	

		default:
			in_ptr++;
			break;
		}
		strcat(pDest,pcBuffer);
	}



}

void CMandolinTextSummary::MANDOLIN_handle_MsgTextSummary_ERROR(char *pDest, mandolin_message* pMsg, int nMaxLength)
{
    uint32 *in_ptr = (uint32 *) pMsg->payload;
    
	int nMsgLength;
 	int nLength;
	char pcBuffer[1000];
	int i;
	int nMsgId;


	nLength = 0;
	*pDest = 0;


	intfloat j;
   
	nMsgLength = pMsg->length;
	nMsgId = pMsg->id;
   	if (nMsgId == MANDOLIN_MSG_NESTED)
	{
		nMsgId = ((in_ptr[0] >> 16 ) & 0x0ffff);
		in_ptr += 1;
		nMsgLength--;
	} else if ((nMsgId == MANDOLIN_MSG_TEST) && (nMsgLength > 0)) {
		int nTestId = in_ptr[0];
		in_ptr += 1;
		nMsgLength--;
		if ((nTestId == TEST_DSP_ERROR) && (nMsgLength > 0)) {
			int nErrorId = in_ptr[0];
			in_ptr += 1;
			nMsgLength--;
			if (nErrorId == DSP_ERROR_INVALID_FLOAT) {
				sprintf(pcBuffer,"INVALID FLOATING POINT OPERATION (0x%8.8x)\n", nErrorId);
			} else {
				sprintf(pcBuffer,"ERROR (0x%8.8x)\n", nErrorId);
			}
			strcat(pDest,pcBuffer);
		}
	}
	for(i=0;i<nMsgLength;i++)
	{
		j.ui = in_ptr[i];

		sprintf(pcBuffer,"%d, %d, 0x%8.8x,  %f\n",i, j.i, j.ui, j.f);
		if ((nLength + strlen(pcBuffer) + 1) > nMaxLength)
			break;
		strcat(pDest,pcBuffer);
		nLength += strlen(pcBuffer);
	}
}

void CMandolinTextSummary::MANDOLIN_handle_MsgTextSummary_SET_APPLICATION_PARAMETER(char *pDest, mandolin_message* pMsg, int nMaxLength)
{

	uint32  flags;
    uint32 *in_ptr = (uint32 *) pMsg->payload;
    bool valid_edit = false;
    

    int nParameterNumber;
	int k;
	int nDirection;
	int nNumOfParameters;
	int nMsgLength;
	char pcBuffer[1000];
	int nInstance;

	
	tParameterDesc* pParameterDesc;
	//int nScaleType;
	int nTarget;
	int nMsgId;
	bool bUseParameterList = false;
	unsigned int *pParameterList;
	int nParameterListSize;

	bool bMultiParameter = false;
	int nLength;

	nLength = 0;
	*pDest = 0;


	intfloat j;
   
	nMsgLength = pMsg->length;
	nMsgId = pMsg->id;
   	if (nMsgId == MANDOLIN_MSG_NESTED)
	{
		nMsgId = ((in_ptr[0] >> 16 ) & 0x0ffff);
		in_ptr += 1;
		nMsgLength--;
	}

    nParameterNumber = in_ptr[0];
	flags = in_ptr[1];

	if (MANDOLIN_WILD_TYPE(flags) == MANDOLIN_TYPE_LIST)
	{
		bUseParameterList = true;

		return;

		if (nParameterNumber == m_nParameterListId)
		{
			pParameterList = m_nParameterList;
			nParameterListSize = m_nParameterListNum;
		}
		else
		{
			return;
		}
		nDirection = MANDOLIN_DIRECTION_NEXT;
		nNumOfParameters = nMsgLength-2;

	}
	else
	{
		nDirection = MANDOLIN_WILD_DIRECTION(flags);
		nNumOfParameters = nMsgLength-2;
	}

	nTarget = MANDOLIN_EDIT_TARGET(flags);
	nInstance = MANDOLIN_WILD_MAP(flags);

	sprintf(pcBuffer,"Instance = %d\n",nInstance);
	strcat(pDest,pcBuffer);
	nLength += strlen(pcBuffer);

	// --------------------------------
	//  checks to see if Multiparameter
	// --------------------------------
	if ( MANDOLIN_WILD_TYPE(flags) == MANDOLIN_TYPE_MULTIPARAMETER)
	{
		bMultiParameter = true;

		//return	MANDOLIN_handle_SET_MULTIPARAMETER(in);
	}




	for(k=0;k<nNumOfParameters;k++)
	{

		if ((k & 0x01) && (bMultiParameter))
		{
			// 0=value, 1=nextid, 2=value, 3=next id
			nParameterNumber = in_ptr[k+2];

		}
		else
		{
			
			j.ui = in_ptr[k+2];
			pcBuffer[0] = 0;


			if (bUseParameterList)
			{
			/*
				if ((k==nParameterListSize) && (nTarget==eTARGET_METERS))
				{
					// get the rms
					nParameterNumber = eMID_FENCE;
				}
				else if ((k==(nParameterListSize+1)) && (nTarget==eTARGET_METERS))
				{
					nParameterNumber = eMID_FENCE;
				}
		
				else if (k>nParameterListSize) 
				if (k>nParameterListSize)
					break;
				else 
					nParameterNumber = pParameterList[k];
			*/
			}



				pParameterDesc = m_dlgParent->GetParameterDesc(m_nProductType, nTarget,nParameterNumber);

				if (!pParameterDesc)
				{
					//valid_edit = false;
					break;

				}
				
				// --------------------------------------------------------
				// decode the Parameter
				// --------------------------------------------------------
				pcBuffer[0] = 0;
				ConvertParameter2Text(pParameterDesc, j, pcBuffer, true, 0, true);
				//strcat(pcBuffer,"\n");

			if ((nLength+strlen(pcBuffer)) >= nMaxLength) 
				return;
			strcat(pDest,pcBuffer);
			nLength += strlen(pcBuffer);

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



}

void CMandolinTextSummary::MANDOLIN_handle_MsgTextSummary_GET_APPLICATION_PARAMETER_REPLY(char *pDest, mandolin_message* pMsg, int nMaxLength)
{

	uint32  flags;
    uint32 *in_ptr = (uint32 *) pMsg->payload;
    bool valid_edit = false;
    

    int nParameterNumber;
	int k;
	int nDirection;
	int nNumOfParameters;
	int nMsgLength;
	char pcBuffer[1000];

	
	tParameterDesc* pParameterDesc;
	int nScaleType;
	int nTarget;
	int nInstance;
	int nMsgId;
	unsigned int *pParameterList;
	int nParameterListSize;

	bool bMultiParameter = false;
	int nLength;

	intfloat j;


	nLength = 0;
	*pDest = 0;
	nMsgLength = pMsg->length;
	nMsgId = pMsg->id;
   	if (nMsgId == MANDOLIN_MSG_NESTED)
	{
		nMsgId = ((in_ptr[0] >> 16 ) & 0x0ffff);
		in_ptr += 1;
		nMsgLength--;
	}

  
   
    nParameterNumber = in_ptr[0];
	flags = in_ptr[1];

	nDirection = MANDOLIN_WILD_DIRECTION(flags);
	nTarget = MANDOLIN_EDIT_TARGET(flags);
	nInstance = MANDOLIN_WILD_MAP(flags);

	sprintf(pcBuffer,"Instance = %d\n",nInstance);
	strcat(pDest,pcBuffer);
	nLength += strlen(pcBuffer);

	 if (MANDOLIN_WILD_TYPE(flags) == MANDOLIN_TYPE_MULTIPARAMETER)
	{
		bMultiParameter = true;
	}

 
	nNumOfParameters = nMsgLength-2;


	for(k=0;k<nNumOfParameters;k++)
	{

		if ((k & 0x01) && (bMultiParameter))
		{
			// 0=value, 1=nextid, 2=value, 3=next id
			nParameterNumber = in_ptr[k+2];

		}
		else
		{

			pParameterDesc =  m_dlgParent->GetParameterDesc(m_nProductType,nTarget,nParameterNumber);
			if (!pParameterDesc)
			{
				//valid_edit = false;
				break;

			}

			// --------------------------------------------------------
			// decode the Parameter
			// --------------------------------------------------------

			j.ui = in_ptr[k+2];


			valid_edit = true;


			ConvertParameter2Text(pParameterDesc, j, pcBuffer, true, 0, true);
			
			if ((nLength+strlen(pcBuffer)) >= nMaxLength) 
				return;
			strcat(pDest,pcBuffer);
			nLength += strlen(pcBuffer);
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

	//UpdateData(FALSE);

}

void CMandolinTextSummary::MANDOLIN_handle_MsgTextSummary_GET_SYSTEM_STATUS_REPLY(char *pDest, mandolin_message* pMsg, int nMaxLength)
{
	uint32  flags;
    uint32 *in_ptr = (uint32 *) pMsg->payload;
	int nMsgLength;
	int nMsgId;
	char pcBuffer[300];

	*pDest = 0;
	if (!in_ptr) return;
	
	nMsgLength = pMsg->length;
	nMsgId = pMsg->id;
   	if (nMsgId == MANDOLIN_MSG_NESTED)
	{
		nMsgId = ((in_ptr[0] >> 16 ) & 0x0ffff);
		in_ptr += 1;
		nMsgLength--;
	}

	switch(in_ptr[0])
	{
	case MANDOLIN_SS_SYSTEM_IP_ADDRESS:
		sprintf(pcBuffer,"IP = 0x%8.8X,  %d.%d.%d.%d",in_ptr[1],
			(in_ptr[1]>>24)&0x0ff,
			(in_ptr[1]>>16)&0x0ff,
			(in_ptr[1]>>8)&0x0ff,
			(in_ptr[1]>>0)&0x0ff
			);
		strcat(pDest,pcBuffer);
		break;
	case MANDOLIN_MSG_GET_SYSTEM_STATUS_CRC:
		sprintf(pcBuffer,"CRC = 0x%8.8X",in_ptr[1]);
		strcat(pDest,pcBuffer);
		break;
	}




}

void mht_convertTestID2Text(int nID,char* pcBuffer,int nMaxLen)
{
	switch(nID)
	{


     case TEST_CMD_SET_STATIC_IP:			strncpy(pcBuffer,"SET_STATIC_IP",nMaxLen); break;
     case TEST_CMD_SET_STATIC_ALL:			strncpy(pcBuffer,"SET_STATIC_ALL",nMaxLen); break;
     case TEST_CMD_SET_IDENTITY:			strncpy(pcBuffer,"TEST_CMD_SET_IDENTITY",nMaxLen); break;
     case TEST_CMD_GET_MAC_ADDR:			strncpy(pcBuffer,"GET_MAC_ADDR",nMaxLen); break;
     case TEST_CMD_GET_STATIC_ALL:			strncpy(pcBuffer,"GET_STATIC_ALL",nMaxLen); break;
     case TEST_CMD_SET_MODEL:				strncpy(pcBuffer,"SET_MODEL",nMaxLen); break;
     case TEST_CMD_SET_SERIAL_NUMBER:		strncpy(pcBuffer,"SET_SERIAL_NUMBER",nMaxLen); break;
     case TEST_CMD_GET_SERIAL_NUMBER:		strncpy(pcBuffer,"GET_SERIAL_NUMBER",nMaxLen); break;
     case TEST_CMD_RESTORE_DEFAULTS:		strncpy(pcBuffer,"RESTORE_DEFAULTS",nMaxLen); break;
     case TEST_CMD_GET_STATUS_ALL:			strncpy(pcBuffer,"GET_STATUS_ALL",nMaxLen); break;
     case TEST_CMD_PRINT_MESSAGE:			strncpy(pcBuffer,"PRINT_MESSAGE",nMaxLen); break;

     case TEST_CMD_SET_OUTPUT:					strncpy(pcBuffer,"FUNC GEN OUTPUT",nMaxLen); break;
     case TEST_CMD_SET_FUNC_GEN:				strncpy(pcBuffer,"FUNC GEN",nMaxLen); break;
     case TEST_CMD_SET_INPUT:					strncpy(pcBuffer,"SET TEST INPUT",nMaxLen); break;
     case TEST_CMD_GET_RMS_METER_LEVEL:			strncpy(pcBuffer,"GET RMS METER LEVEL",nMaxLen); break;
     case TEST_CMD_GET_RMS_METER_RANGE:			strncpy(pcBuffer,"GET RMS METER RANGE",nMaxLen); break;
     case TEST_CMD_WRITE_IO:					strncpy(pcBuffer,"WRITE IO",nMaxLen); break;
     case TEST_CMD_READ_IO:						strncpy(pcBuffer,"READ IO",nMaxLen); break;
     case TEST_CMD_READ_AMP_TEMPERATURE:		strncpy(pcBuffer,"READ AMP TEMPERATURE",nMaxLen); break;
     case TEST_CMD_VALIDATE_SPI_FLASH:			strncpy(pcBuffer,"VALIDATE SPI FLASH",nMaxLen); break;
     case TEST_CMD_GET_IMON_VALUE:				strncpy(pcBuffer,"GET IMON VALUE",nMaxLen); break;
     case TEST_DSP_ERROR:						strncpy(pcBuffer,"DSP ERROR",nMaxLen); break;


 	default:
		strncpy(pcBuffer,"unknown",nMaxLen);
		break;
	}

}

void CMandolinTextSummary::MANDOLIN_handle_MsgTextSummary_TRIGGER_SYSTEM_EVENT(char *pDest, mandolin_message* pMsg, int nMaxLength)
{

	uint32 *in_ptr = (uint32 *) pMsg->payload;
	int nMsgLength;
	int nMsgId;
	char pcBuffer[300];
	int nPos;
	uint32 uiValue;
	intfloat jValue;
	int i;
	int nEventID;
	intfloat j;
	int nState;



	*pDest = 0;
	if (!in_ptr) return;
	
	nMsgLength = pMsg->length;
	nMsgId = pMsg->id;
   	if (nMsgId == MANDOLIN_MSG_NESTED)
	{
		nMsgId = ((in_ptr[0] >> 16 ) & 0x0ffff);
		in_ptr += 1;
		nMsgLength--;
	}

	nPos = 0;
	nEventID = in_ptr[nPos];
	//mht_convertTestID2Text(nTestID,pcBuffer,sizeof(pcBuffer)-1);
	//strcat(pDest,pcBuffer);
	//strcat(pDest,"\n");
	nPos++;


	switch(nEventID)
	{
	case	MANDOLIN_SE_CONNECT:				break;
	case	MANDOLIN_SE_DISCONNECT: 			break;	
	case	MANDOLIN_SE_LOCK_CHANGES:			break;		
	case	MANDOLIN_SE_UNLOCK_CHANGES:			break;		
	case	MANDOLIN_SE_IDENTIFY:				break;
	case	MANDOLIN_SE_SYNC_CURRENT_STATE: 	break;
	case	MANDOLIN_SE_BEGIN_UPDATE:			break;		
	case	MANDOLIN_SE_END_UPDATE: 			break;
	case	MANDOLIN_SE_PROCESS_FILES:			
		j.ui = in_ptr[nPos];
		switch (j.ui)
		{
		case MANDOLIN_PROGRESS_FAIL: sprintf(pcBuffer,"FAIL\n"); break;
		case MANDOLIN_PROGRESS_SUCCESS: sprintf(pcBuffer,"SUCCESS\n"); break;
		case MANDOLIN_PROGRESS_BUSY: 		
			j.ui = in_ptr[nPos+1];
			sprintf(pcBuffer,"BUSY - %d%%\n", j.ui);
			break;
		default:
			printf(pcBuffer,"UNKNOWN\n"); break;
		}

		if (j.ui == MANDOLIN_PROGRESS_BUSY) // BUSY
		{
			j.ui = in_ptr[nPos+1];
			sprintf(pcBuffer,"BUSY - %d%%\n", j.ui);

		}
		strcat(pDest,pcBuffer);
		break;
	}

}
void CMandolinTextSummary::MANDOLIN_handle_MsgTextSummary_TEST_REPLY(char *pDest, mandolin_message* pMsg, int nMaxLength)
{
    uint32 *in_ptr = (uint32 *) pMsg->payload;
	int nMsgLength;
	int nMsgId;
	char pcBuffer[300];
	int nPos;
	uint32 uiValue;
	intfloat jValue;
	int i;
	int nTestID;
	intfloat j;
	int nState;
	char pcError[100];



	*pDest = 0;
	if (!in_ptr) return;
	
	nMsgLength = pMsg->length;
	nMsgId = pMsg->id;
   	if (nMsgId == MANDOLIN_MSG_NESTED)
	{
		nMsgId = ((in_ptr[0] >> 16 ) & 0x0ffff);
		in_ptr += 1;
		nMsgLength--;
	}

	nPos = 0;
	nTestID = in_ptr[nPos];
	mht_convertTestID2Text(nTestID,pcBuffer,sizeof(pcBuffer)-1);
	strcat(pDest,pcBuffer);
	strcat(pDest,"\n");
	nPos++;


	switch(nTestID)
	{
	case	TEST_CMD_GET_SERIAL_NUMBER:
		sprintf(pcBuffer,"Serial Num = 0x%8.8X", in_ptr[nPos]);
		strcat(pDest,pcBuffer);
		break;


	case TEST_CMD_GET_MAC_ADDR:
		sprintf(pcBuffer,"MAC Addr= %2.2X-%2.2X-%2.2X-%2.2X-%2.2X-%2.2X", 
			((in_ptr[nPos] >>  8) & 0x0ff),
			((in_ptr[nPos] >>  0) & 0x0ff),
			((in_ptr[nPos+1] >> 24) & 0x0ff),
			((in_ptr[nPos+1] >> 16) & 0x0ff),
			((in_ptr[nPos+1] >>  8) & 0x0ff),
			((in_ptr[nPos+1] >>  0) & 0x0ff));
		strcat(pDest,pcBuffer);
		break;

	case   TEST_CMD_SET_OUTPUT:
    case   TEST_CMD_SET_FUNC_GEN:
		break;

    case   TEST_CMD_SET_INPUT:
		break;

    case   TEST_CMD_GET_RMS_METER_LEVEL:
		j.ui = in_ptr[nPos];
		sprintf(pcBuffer,"Level=%f", j.f);
		strcat(pDest,pcBuffer);
		break;


    case   TEST_CMD_WRITE_IO:
		break;
    case   TEST_CMD_READ_IO:
		sprintf(pcBuffer,"Address=%d, value=0x%x", in_ptr[nPos],in_ptr[nPos+1]);
		strcat(pDest,pcBuffer);
		break;

    case   TEST_CMD_GET_RMS_METER_RANGE:
		// subid/passorfail/percent
		if (in_ptr[nPos] ==  MANDOLIN_PROGRESS_SUCCESS)
		{
			strcat(pDest,"PASS\n");
		}
		else if (in_ptr[nPos] ==  MANDOLIN_PROGRESS_FAIL)
		{
			strcat(pDest,"FAIL\n");
		}
		else
		{
			nPos++;
			sprintf(pcBuffer,"BUSY - %d %% \n",in_ptr[nPos]);
			strcat(pDest,pcBuffer);
		}
		break;

    case   TEST_CMD_READ_AMP_TEMPERATURE:
		// address index
		// temp (as voltage 0.2.5)
		j.ui = in_ptr[nPos+1];
		sprintf(pcBuffer,"Address=%d, temp voltage=%f", in_ptr[nPos],j.f);
		strcat(pDest,pcBuffer);
		break;

    case   TEST_CMD_VALIDATE_SPI_FLASH:
		j.ui = in_ptr[nPos+1];
		sprintf(pcBuffer,"SPI Flash is %s", (in_ptr[nPos]==0)?"Not Present":"Present");
		strcat(pDest,pcBuffer);
		break;

    case   TEST_CMD_GET_IMON_VALUE:
		j.ui = in_ptr[nPos+1];
		sprintf(pcBuffer,"Channel=%d, adc value=0x%8.8x (%d)", in_ptr[nPos],j.ui,j.ui);
		strcat(pDest,pcBuffer);
		break;

  //  case   ANYA_TESTID_READ_AMP_IMPEDANCE:
		//// address index
		//// impedance
		//j.ui = in_ptr[nPos+1];
		//sprintf(pcBuffer,"Address=%d, impedance=%f", in_ptr[nPos+0],j.f);
		//strcat(pDest,pcBuffer);
		//break;


  //  case ANYA_TESTID_GOVN_BATTERY:			

	
		//// subid/passorfail/percent
		//if (in_ptr[nPos] ==  MANDOLIN_PROGRESS_SUCCESS)
		//{
		//	strcat(pDest,"PASS\n");
		//}
		//else if (in_ptr[nPos] ==  MANDOLIN_PROGRESS_FAIL)
		//{
		//	strcat(pDest,"FAIL\n");
		//}
		//else
		//{
		//	nPos++;
		//	sprintf(pcBuffer,"BUSY - %d %% \n",in_ptr[nPos]);
		//	strcat(pDest,pcBuffer);
		//}
		//break;

 
	}


		
}


void CMandolinTextSummary::MANDOLIN_handle_MsgTextSummary_CUSTOM_REPLY(char *pDest, mandolin_message* pMsg, int nMaxLength)
{
	uint32  flags;
    uint32 *in_ptr = (uint32 *) pMsg->payload;
	int nMsgLength;
	int nMsgId;
	char pcBuffer[300];
	char pcError[300];
	int nPos;
	uint32 uiValue;
	int nChannel;
	intfloat jValue;
	int i;

	int nState;



	*pDest = 0;
	if (!in_ptr) return;
	
	
}

void CMandolinTextSummary::MANDOLIN_handle_MsgTextSummary(char *pDest, mandolin_message* pMsg, int nMaxLength)
{

	char pcMessageType[50];
	char pcMessageTime[50];
	uint8* in_ptr;
	bool bWantPayload;
	int nSubId = 0;
	bool bSubId = false;
	int nMsgId;
	bool bNested = false;
	char pcError[100];
	int nError;
	intfloat j;

	*pDest = 0;
	nMsgId= pMsg->id;
	in_ptr = (uint8*) pMsg->payload;

	pcMessageType[0] = 0;

	if (nMsgId == MANDOLIN_MSG_NESTED)
	{
		bNested = true;
		nMsgId = MANDOLIN_PAYLOAD_GETUINT32(in_ptr,0);
		nMsgId = ((nMsgId >> 16 ) & 0x0ffff);
		in_ptr += 4;
	}
	sprintf(pcMessageTime,"msgid=0x%2.2X, seq=0x%2.2X, msec=%d %s",nMsgId,pMsg->sequence,tick_count, bNested?", NESTED":"");

	bSubId = MANDOLIN_handle_bHasSubId(nMsgId);
	if ((in_ptr) && (bSubId))
	{
		nSubId = MANDOLIN_PAYLOAD_GETUINT32(in_ptr,0);
	}

	MANDOLIN_handle_MsgId2Text(pcMessageType,nMsgId, nSubId);
	


	if(IS_MANDOLIN_REPLY(pMsg->transport)) 
	{
		if(IS_MANDOLIN_NACK(pMsg->transport))
		{
			if ((pMsg->length > 0) && (in_ptr != 0))
			{
				if (bSubId)
				{
					j.ui = MANDOLIN_PAYLOAD_GETUINT32(in_ptr,4);
				}
				else
				{
					j.ui = MANDOLIN_PAYLOAD_GETUINT32(in_ptr,0);
				}
				nError = j.i;
				MANDOLIN_handle_Error2Text(pcError,nError);
				sprintf(pDest,"%s, ***NACK REPLY: %s, nack=%d (%s)",pcMessageTime,pcMessageType,nError,pcError);

				return;
			}
			else
			{
				sprintf(pDest,"%s, ***NACK REPLY: %s, nack=NONE",
						pcMessageTime, pcMessageType);
			}
		}
		else
		{
			sprintf(pDest,"%s, REPLY: %s\n",pcMessageTime,pcMessageType);


			if (nMsgId == MANDOLIN_MSG_GET_SOFTWARE_INFO)
			{
				int nLen = strlen(pDest);
				MANDOLIN_handle_MsgTextSummary_GET_SOFTWARE_INFO_REPLY((char*)(pDest+nLen),pMsg,nMaxLength-nLen );
			}
			else if (nMsgId == MANDOLIN_MSG_GET_APPLICATION_PARAMETER)
			{
				int nLen = strlen(pDest);

				MANDOLIN_handle_MsgTextSummary_GET_APPLICATION_PARAMETER_REPLY((char*)(pDest+nLen),pMsg,nMaxLength-nLen);
					
			}
			else if (nMsgId == MANDOLIN_MSG_GET_SYSTEM_STATUS)
			{
				int nLen = strlen(pDest);
				MANDOLIN_handle_MsgTextSummary_GET_SYSTEM_STATUS_REPLY((char*)(pDest+nLen),pMsg,nMaxLength-nLen);
			}
			else if (nMsgId == MANDOLIN_MSG_TEST)
			{
				int nLen = strlen(pDest);
				MANDOLIN_handle_MsgTextSummary_TEST_REPLY((char*)(pDest+nLen),pMsg,nMaxLength-nLen);
			}

			else if (nMsgId == MANDOLIN_MSG_TRIGGER_SYSTEM_EVENT)
			{
				int nLen = strlen(pDest);
				MANDOLIN_handle_MsgTextSummary_TRIGGER_SYSTEM_EVENT((char*)(pDest+nLen),pMsg,nMaxLength-nLen);
			}

			else if  (nMsgId == MANDOLIN_MSG_GET_HARDWARE_INFO)
			{
				//int nLen;
				//intfloat j;

				j.ui = MANDOLIN_PAYLOAD_GETUINT32(in_ptr,0*4);	// get brand and model

				switch((j.ui >> 28) & 0x0f)
				{
				case LOUD_BRAND_MACKIE:
					strcat(pDest,"MACKIE ");
					switch ((j.ui>>16)&0x0fff)
					{
					case MACKIE_MODEL_GENERIC: strcat(pDest,"GENERIC\n"); break;
					case MACKIE_MODEL_DL1608:	strcat(pDest,"DL1608\n"); break;
					case MACKIE_MODEL_DL806:	strcat(pDest,"DL806\n"); break;
					case     MACKIE_MODEL_DL1608_DL806:		strcat(pDest,"DL1608_DL806:\n"); break;  // WhiteApp doesn't know...
					case     MACKIE_MODEL_DL3214:			strcat(pDest,"DL3214\n"); break;
					case     MACKIE_MODEL_DL32R:			strcat(pDest,"DL32R\n"); break;
					case     MACKIE_MODEL_DL24XX:			strcat(pDest,"DL24XX\n"); break;
 					case    MACKIE_MODEL_DL24XX_RACK:		strcat(pDest,"DL24XX_RACK\n"); break;
 					case    MACKIE_MODEL_DC16:				strcat(pDest,"DC16\n"); break;
					/*  Loudspeakers */
					case    MACKIE_MODEL_R8_AA:				strcat(pDest,"R8_AA\n"); break;
					case    MACKIE_MODEL_R8_64:				strcat(pDest,"R8_64\n"); break;
					case    MACKIE_MODEL_R8_96:				strcat(pDest,"R8_96\n"); break;
					case    MACKIE_MODEL_R12_64:			strcat(pDest,"R12_64\n"); break;
					case    MACKIE_MODEL_R12_96:			strcat(pDest,"R12_96\n"); break;
					case    MACKIE_MODEL_R12_SM:			strcat(pDest,"R12_SM\n"); break;
					case    MACKIE_MODEL_R12_SW:			strcat(pDest,"R12_SW\n"); break;
					case	MACKIE_MODEL_R18_SW:			strcat(pDest,"R18_SW\n"); break;

					/*  Software Products */
					case     MACKIE_MODEL_MASTER_FADER:		strcat(pDest,"MASTER_FADER\n"); break;
					case     MACKIE_MODEL_MY_FADER:			strcat(pDest,"MY_FADER\n"); break;
					case     MACKIE_MODEL_MASTER_RIG:		strcat(pDest,"MASTER_RIG\n"); break;
					default: strcat(pDest,"UNKNOWN\n"); break;

					}
					break;
				case LOUD_BRAND_EAW: 
					strcat(pDest,"EAW ");
					switch ((j.ui >> 16)&0x0fff)
					{
					case EAW_MODEL_GENERIC: strcat(pDest,"GENERIC\n"); break;
					case EAW_MODEL_DX1208:	strcat(pDest,"DX1208\n"); break;
					case EAW_MODEL_ANYA:	strcat(pDest,"ANYA\n"); break;
					case EAW_MODEL_OTTO:	strcat(pDest,"OTTO\n"); break;
					case EAW_MODEL_HALARCH: strcat(pDest,"HALARCH\n"); break;
					case EAW_MODEL_ANNA:	strcat(pDest,"ANNA\n"); break;
	
					case EAW_MODEL_RSX208L: strcat(pDest,"RSX208L\n"); break;  // Radius 8" Articulated Array Module
					case EAW_MODEL_RSX86: strcat(pDest,"RSX86\n"); break;   // Radius 8" Full-Range, 60x40 Horn
					case EAW_MODEL_RSX89: strcat(pDest,"RSX89\n"); break;   // Radius 8" Full-Range, 90x60 Horn
					case EAW_MODEL_RSX126: strcat(pDest,"RSX126\n"); break;   // Radius 12" Full-Range, 60x40 Horn
					case EAW_MODEL_RSX129: strcat(pDest,"RSX129\n"); break;   // Radius 12" Full-Range, 90x60 Horn
					case EAW_MODEL_RSX12M: strcat(pDest,"RSX12M\n"); break;   // Radius 12" Stage Monitor
					case EAW_MODEL_RSX12: strcat(pDest,"RSX12\n"); break;   // Radius 12" Subwoofer
					case EAW_MODEL_RSX18: strcat(pDest,"RSX18\n"); break;   // Radius 18" Subwoofer
					case EAW_MODEL_RSX218: strcat(pDest, "RSX218\n"); break;   // Radius 18" Subwoofer
					case EAW_MODEL_RSX18F: strcat(pDest, "RSX18F\n"); break;   // Radius 18" Subwoofer
					case EAW_MODEL_RSX212L: strcat(pDest, "RSX212L\n"); break;   //  Radius 12" Articulated Array Module
					default: strcat(pDest,"UNKNOWN\n"); break;
					}
					break;

				case LOUD_BRAND_MARTIN: 
					strcat(pDest,"MARTIN ");
					switch ((j.ui >> 16)&0x0fff)
					{

					case MARTIN_MODEL_GENERIC: strcat(pDest,"GENERIC\n"); break;
					case     MARTIN_MODEL_CDDL8:			strcat(pDest,"CDDL8\n"); break;
					case     MARTIN_MODEL_CDDL12:			strcat(pDest,"CDDL12\n"); break;
					case     MARTIN_MODEL_CDDL15:			strcat(pDest,"CDDL15\n"); break;
					case     MARTIN_MODEL_CSXL118:			strcat(pDest,"CSXL118\n"); break;
					case     MARTIN_MODEL_CSXL218:			strcat(pDest,"CSXL218\n"); break;
					default: strcat(pDest,"UNKNOWN\n"); break;
					}
					break;

				default:
					strcat(pDest,"UNKNOWN BRAND AND MODEL\n");
					break;

				}
				sprintf(pcMessageType,"%8.8X-",j.ui&0x0ffff);
				strcat(pDest,pcMessageType);

				j.ui = MANDOLIN_PAYLOAD_GETUINT32(in_ptr,1*4);	//get 2nd half of serial
				sprintf(pcMessageType,"%8.8X\n",j.ui);
				strcat(pDest,pcMessageType);
				

				//int nLen2 = strlen(pDest);
			}


			else if (nMsgId == MANDOLIN_MSG_CUSTOM)
			{
				int nLen = strlen(pDest);
				MANDOLIN_handle_MsgTextSummary_CUSTOM_REPLY((char*)(pDest+nLen),pMsg,nMaxLength-nLen);
			}

			return;
		}
	}
	else
	{
		sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType);

		bWantPayload = false;
		switch(nMsgId)									 
		{					 
			

		case MANDOLIN_MSG_INVALID:						break;
		case MANDOLIN_MSG_PING: 						break;
		case MANDOLIN_MSG_ADVERTIZE_HARDWARE_INFO:		break;
		case MANDOLIN_MSG_GET_HARDWARE_INFO:			sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType); bWantPayload = true; break;
		case MANDOLIN_MSG_GET_SOFTWARE_INFO:			sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType); bWantPayload = true; break;
		case MANDOLIN_MSG_REBOOT:						sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType); bWantPayload = true; break;
		case MANDOLIN_MSG_TRIGGER_SYSTEM_EVENT: 		sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType); bWantPayload = true; break;
		case MANDOLIN_MSG_TRIGGER_APPLICATION_EVENT:	sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType); bWantPayload = true; break;
		case MANDOLIN_MSG_TEST: 						sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType); bWantPayload = true; break;
		case MANDOLIN_MSG_GET_INFO: 					sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType); bWantPayload = true; break;
		case MANDOLIN_MSG_FILE_OPEN:					sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType); bWantPayload = true; break;
		case MANDOLIN_MSG_FILE_CLOSE:					sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType); bWantPayload = true; break;
		case MANDOLIN_MSG_GET_FILE: 					sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType); bWantPayload = true; break;
		case MANDOLIN_MSG_POST_FILE:					sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType); bWantPayload = true; break;
		case MANDOLIN_MSG_GET_SYSTEM_STATUS:			sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType); bWantPayload = true; break;
		case MANDOLIN_MSG_SET_SYSTEM_STATE: 			sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType); bWantPayload = true; break;
		case MANDOLIN_MSG_RESET_SYSTEM_STATE:			sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType); bWantPayload = true; break;
		case MANDOLIN_MSG_SUBSCRIBE_SYSTEM_STATE:		sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType); bWantPayload = true; break;
		case MANDOLIN_MSG_GET_APPLICATION_PARAMETER:	sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType); bWantPayload = true; break;
		case MANDOLIN_MSG_SET_APPLICATION_PARAMETER:	sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType); bWantPayload = true; break;
		case MANDOLIN_MSG_RESET_APPLICATION_PARAMETER:	sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType); bWantPayload = true; break;
		case MANDOLIN_MSG_SUBSCRIBE_APPLICATION_PARAMETER: sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType); bWantPayload = true; break;
		case MANDOLIN_MSG_CREATE_PARAMETER_LIST:		   sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType); bWantPayload = true; break;
		case MANDOLIN_MSG_GET_APPLICATION_STRING:		   sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType); bWantPayload = true; break;
		case MANDOLIN_MSG_SET_APPLICATION_STRING:		   sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType); bWantPayload = true; break;
		case MANDOLIN_MSG_RESET_APPLICATION_STRING: 	   sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType); bWantPayload = true; break;
		case MANDOLIN_MSG_CUSTOM: 						sprintf(pDest,"%s, %s",pcMessageTime,pcMessageType); bWantPayload = true; break;

		default:
			break;
		}
		if ((bWantPayload) && (in_ptr))
		{
			if (bSubId)
			{
				if (pMsg->length > 1)
				{
					sprintf(pcMessageType,"(0x%X)",MANDOLIN_PAYLOAD_GETUINT32(in_ptr,4));
				}
				else
				{
					sprintf(pcMessageType,"(NONE)");
				}
			}
			else
			{
				if (pMsg->length > 0)
				{
					sprintf(pcMessageType,"(0x%X)",MANDOLIN_PAYLOAD_GETUINT32(in_ptr,0));
				}
				else
				{
					strcpy(pcMessageType,"");
				}
			}

			strcat(pDest,pcMessageType);
		}




		// Special case handling.. more info
		if  (nMsgId == MANDOLIN_MSG_SET_APPLICATION_PARAMETER)
		{
			int nLen;
			strcat(pDest,"\n");
			nLen = strlen(pDest);
			MANDOLIN_handle_MsgTextSummary_SET_APPLICATION_PARAMETER((char*)(pDest+strlen(pDest)),pMsg,nMaxLength-nLen);
			int nLen2 = strlen(pDest);
		}

				// Special case handling.. more info
		if  ((nMsgId == MANDOLIN_MSG_TEST) && (bSubId) && (nSubId == TEST_DSP_ERROR))
		{
			int nLen;
			strcat(pDest,"\n");
			nLen = strlen(pDest);
			MANDOLIN_handle_MsgTextSummary_ERROR((char*)(pDest+strlen(pDest)),pMsg,nMaxLength-nLen);
			int nLen2 = strlen(pDest);
		}

		return;
	}


}


void CMandolinTextSummary::ConvertParameter2Text(tParameterDesc* pParameterDesc, intfloat j, char* pcBuffer, bool bFormat, int nInstance, bool bHighPrec)
{
	char pcInstance[10];
	pcBuffer[0] = 0;
	if (!pParameterDesc) return;

	switch(pParameterDesc->format)
	{
		case eFORMAT_BOOL:
			if (bFormat)
				sprintf(pcBuffer,"%s %s",pParameterDesc->pName,j.i?"TRUE":"FALSE");
			else
				sprintf(pcBuffer,"%s %d",pParameterDesc->pName,j.i);
			break;

		case eFORMAT_SIGNED:
			sprintf(pcBuffer,"%s %d",pParameterDesc->pName,j.i);
			break;
		case eFORMAT_UNSIGNED:
			sprintf(pcBuffer,"%s %d",pParameterDesc->pName,j.ui);
			break;
		case eFORMAT_FLOAT:
			if (bFormat)
				if (bHighPrec)
					sprintf(pcBuffer,"%s %f",pParameterDesc->pName,j.f);
				else
					sprintf(pcBuffer,"%s %8.2f",pParameterDesc->pName,j.f);
			else
//				sprintf(pcBuffer,"%s %f\n",pParameterDesc->pName,j.f);			// seems to just do 6 sig figs no matter what?
				if ((j.f < 0.001) && (j.f > -0.001)) {
					sprintf(pcBuffer,"%s %.10e",pParameterDesc->pName,j.f);		// use scientific notation
				} else {
					sprintf(pcBuffer,"%s %.10f",pParameterDesc->pName,j.f);		// use scientific notation
				}
			break;
	}
	if (nInstance != 0)
	{
		sprintf(pcInstance," %d",nInstance);
		strcat(pcBuffer,pcInstance);
	}
	strcat(pcBuffer,"\n");

}