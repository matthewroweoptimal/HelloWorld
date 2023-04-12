// --------------------------------

#include "stdafx.h"
#include "LOUD_types.h"
extern "C" 
{
#include "mandolin.h"
}

#include "RUglyDlg.h"

#include <stdlib.h>
//#include "resource.h"


#include "SettingsFile.h"



CSettingsFile::CSettingsFile()
{
	m_dlgParent = 0;
	m_bSettingsFileInProgress = false;
	m_bSettingsFileWaitingAck = false;


	
	m_bSettingsFileDontWaitForAcks = false;	
	m_bSettingsFileBlockSends = false;

}
CSettingsFile::~CSettingsFile()
{

}


void CSettingsFile::OnTimer(UINT nIDEvent)
{
	char pcBuffer[200];

	switch (nIDEvent)
	{
		case MSGTIMEOUT_TIMERID:

			if((m_bSettingsFileInProgress) && (m_bSettingsFileWaitingAck) && (m_msgSettingsFile.id != -1))
			{
				sprintf(pcBuffer, "Setting File downlaod error: did not receive a response: 0x%x", m_nSettingsFileReplySequence);
				m_dlgParent->SetDlgItemText(IDC_RESPONSESTATUS,pcBuffer);

				m_nSettingsFileRetryCount++;


				if (m_nSettingsFileRetryCount < 5)
				{
					m_bSettingsFileWaitingAck = true;
					if (!m_bSettingsFileBlockSends) 
					{
						m_dlgParent->TryToSendMandolinMessage(&m_msgSettingsFile, true);
						m_dlgParent->SetTimer(SETTINGSFILE_TIMERID,1000,NULL);
					}
					else
					{
						m_dlgParent->SetTimer(SETTINGSFILE_TIMERID,1,NULL);
					}
				}
				else
				{
					sprintf(pcBuffer, "Setting File downlaod error: STOPPED, too many retries");
					m_dlgParent->SetDlgItemText(IDC_RESPONSESTATUS,pcBuffer);
					m_bSettingsFileInProgress = false;
					m_dlgParent->KillTimer(SETTINGSFILE_TIMERID);
				}

			}
			break;



	case	SETTINGSFILE_TIMERID:
		if (m_bSettingsFileInProgress)
			{
				if (m_bSettingsFileWaitingAck)
				{
					sprintf(pcBuffer, "Setting File downlaod error: did not receive a response: 0x%x", m_nSettingsFileReplySequence);
					m_dlgParent->SetDlgItemText(IDC_RESPONSESTATUS,pcBuffer);
	
					m_nSettingsFileRetryCount++;
	
					if (m_nSettingsFileRetryCount < 5)
					{

					if (m_bSettingsFileDontWaitForAcks || m_bSettingsFileBlockSends)
					{
							m_bSettingsFileWaitingAck = false;
					}
					else
					{
						m_bSettingsFileWaitingAck = true;
						if (!m_bSettingsFileBlockSends) 
						{
							m_dlgParent->TryToSendMandolinMessage(&m_msgSettingsFile, true);
							m_dlgParent->SetTimer(SETTINGSFILE_TIMERID,1000,NULL);
						}
						else
						{
							m_dlgParent->SetTimer(SETTINGSFILE_TIMERID,1,NULL);
						}
						break;
					}

					}
					else
					{
						sprintf(pcBuffer, "Setting File downlaod error: STOPPED, too many retries");
						m_dlgParent->SetDlgItemText(IDC_RESPONSESTATUS,pcBuffer);
						m_bSettingsFileInProgress = false;
						m_dlgParent->KillTimer(SETTINGSFILE_TIMERID);
					}

				}
				CString strErrorMsg;
				bool bDone = false;

				// default for the commands
				if (!m_bSettingsFileBlockSends) 
				{
					m_dlgParent->SetTimer(SETTINGSFILE_TIMERID,300,NULL); // set the time out
				}
				else
				{
					m_dlgParent->SetTimer(SETTINGSFILE_TIMERID,1,NULL); // set the time out
				}

				m_bSettingsFileWaitingAck = true;


				if (!SettingsFileSendNext(m_strSettingsFileName.GetBuffer(),m_nSettingsFilePos,bDone, strErrorMsg))
				{
					m_strSettingsFileName.ReleaseBuffer();
					m_dlgParent->SetDlgItemText(IDC_RESPONSESTATUS,strErrorMsg);
					m_bSettingsFileInProgress  = false;
					m_bSettingsFileWaitingAck = false;
					bDone = true;
					m_dlgParent->KillTimer(SETTINGSFILE_TIMERID); // set the time out

				}
				m_strSettingsFileName.ReleaseBuffer();
				if (bDone)
				{
					m_dlgParent->KillTimer(SETTINGSFILE_TIMERID);
					if (!m_bSettingsFileBlockSends)
						m_dlgParent->SetDlgItemText(IDC_RESPONSESTATUS,"Sending Settings file done");
					else 
						m_dlgParent->SetDlgItemText(IDC_RESPONSESTATUS,"Loading Settings file done");

					m_bSettingsFileInProgress  = false;
					m_bSettingsFileWaitingAck = false;
				}
			}
			break;
	}
}
int CSettingsFile::SettingsFileInit(char* strInputTextFile, CString& strErrorMsg)
{

	CFile fReadFile;
	CString errorMsg;

	strErrorMsg.Empty();

//	SetDlgItemText(IDC_FILEOPENFILENAME,strInputTextFile);

	if(!fReadFile.Open(strInputTextFile, CFile::modeRead))
	{
		errorMsg = "Can not open file: ";
		errorMsg += strInputTextFile;
		strErrorMsg = errorMsg;
		//CWnd::MessageBox(errorMsg, "Error", MB_OK | MB_ICONERROR);
		return -1;
	}
	else
	{
		fReadFile.Close();
		m_strSettingsFileName = strInputTextFile;
		if (!SettingsFileRead(strInputTextFile, strErrorMsg))
		{
			return -1;
		}

		return 0;
	}

}

/*
		if (bSend)
		{
			m_strSettingsFileName = strInputTextFile;
			m_nSettingsFilePos = 0;

			SetTimer(SETTINGFILESREAD_TIMERID,1,NULL)
		}
		else
		{
			}
	}
*/

int CSettingsFile::GetLine(CFile* fReadFile, char* pcNewLine, bool *bEndFile, int nMaxLen)
{
	bool bLineDone = false;
	int nLineLength = 0;
	char *pC;
	int i;
	char c;

	*bEndFile = false;
	nLineLength = 0;
	pC = pcNewLine;
	

	while( !bLineDone) 
	{
		i = fReadFile->Read(&c,1);
		if (i != 1)
		{
			*bEndFile = true;
			break;
		}

			// skip empty lines
		if (nLineLength == 0) 
		{
			while ((c  == '\n') || (c == 0) || (c =='\r'))
			{
				i = fReadFile->Read(&c,1);
				if (i != 1)
				{
					*bEndFile = true;
					break;
				}

			}
		}

		if ((c  == '\n') || (c == 0) || (c =='\r') )
		{
			while (c=='\r')
			{
				// read the \n
				i = fReadFile->Read(&c,1);
				if (i != 1)
				{
					*bEndFile = true;
					break;
				}

			}
			c = 0;
			bLineDone = true;
			break;
		}
		if (nLineLength < (nMaxLen-1))
		{
			*pC = c;
			*pC++;
		}
		else {
			c = 0;
		}
		nLineLength++;
	}
	*pC = 0;

	return nLineLength;
}
int CSettingsFile::GetToken(char* pcToken, char* pcSrc, int& nPos, int nSize)
{

	// returns length of token

	int	nLength = 0;
	
	*pcToken = 0;
	if (nPos >= nSize) 
		return nLength;


	// skip spaces
	while ( (isspace(pcSrc[nPos]) != 0) && (nPos<nSize))
	{
		nPos++;
	}
	
	// fill token
	while ( (isspace(pcSrc[nPos]) == 0) && (nPos<nSize) && 
		((nLength == 0) || (pcSrc[nPos] != ';')) )
	{
		pcToken[nLength++] = pcSrc[nPos++];
	}
	pcToken[nLength] = 0;

	if (nLength>1)
	{
		if ((pcToken[nLength-1] == ',') ||  (pcToken[nLength-1] == ':'))
		{
			pcToken[nLength-1] = 0;
		}
	}
	return nLength;

	
}


bool CSettingsFile::SettingsFileParseLine(char* pcNewLine, int nLineLength, tSettingFileCommandLine* pCommand, CString& strError)
{
	// return if true if pass

	int nPos;
	char pcToken[256];
	int nTokenLength;
	tParameterDesc* pParameterDesc;
	CString strToken;
	char* next;
	int nValue;

	nPos = 0;
	nTokenLength = GetToken(pcToken,pcNewLine, nPos, nLineLength);

	pCommand->nType = SETTINGSFILECOMMANDTYPE_NONE;
	pCommand->pcArg1[0] = 0;
	pCommand->pcArg2[0] = 0;
	pCommand->nPID = 0;
	pCommand->nPIDTarget = 0;
	pCommand->nPIDInstance = 0;
	pCommand->nArg1ValueType = 0;
	pCommand->nArg1ValueType = 0;
	pCommand->jArg1.i = 0;
	pCommand->jArg2.i = 0;
	pCommand->nRawDataPos = 0;

	if (nTokenLength == 0) return true;
	if (pcToken[0] == ';') return true;



	// find the parameter
	if (strcmp(pcToken,"SPEAKER") == 0)
	{
		pCommand->nType = SETTINGSFILECOMMANDTYPE_SPEAKER;
		nTokenLength = GetToken(pcToken,pcNewLine, nPos, nLineLength);	 // get the value
		if (nTokenLength>0)
		{
			pCommand->nArg1ValueType = 2;
			strcpy(pCommand->pcArg1,pcToken);
			return true;
		}
		else
		{
			strError.Format("No Speaker Name");
			return false;
		}
		return true;

	}
	else if (strcmp(pcToken,"PRODUCT") == 0)
	{
		pCommand->nType = SETTINGSFILECOMMANDTYPE_PRODUCT;
		strcpy(pCommand->pcArg1,"");
		nTokenLength = GetToken(pcToken,pcNewLine, nPos, nLineLength);	 // get the value
		if (nTokenLength>0)
		{
			strToken = pcToken;
			strToken.MakeUpper();
			strcpy(pCommand->pcArg1,strToken.GetBuffer());
			strToken.ReleaseBuffer();

			pCommand->nArg1ValueType = 0;
			if (strToken == "OLYSPEAKER")
			{
				pCommand->jArg1.i = ePRODUCT_OLYSPEAKER;
			}
			else if (strToken == "REDLINESPEAKER")
			{
				pCommand->jArg1.i = ePRODUCT_REDLINESPEAKER1;
			}
		else
			{
				pCommand->jArg1.i = ePRODUCT_UNKNOWN;
			}
			m_nSettingsFileProductType = pCommand->jArg1.i;
			m_dlgParent->SetProducttype(m_nSettingsFileProductType);

		}

	}

	else if (strcmp(pcToken,"STORE") == 0)
	{
		pCommand->nType = SETTINGSFILECOMMANDTYPE_STORE;
		pCommand->jArg1.i = 1;
		nTokenLength = GetToken(pcToken,pcNewLine, nPos, nLineLength);	 // get the value
		if (nTokenLength>0)
		{
			strToken = pcToken;
			pCommand->jArg1 = m_dlgParent->ConvertString2Value(strToken,pCommand->nArg1ValueType);
			if (pCommand->nArg1ValueType != 0)
			{
				pCommand->jArg1.i = (int)(pCommand->jArg1.f + 0.5);
			}

		}

	}
	else if (strcmp(pcToken,"RECALL") == 0)
	{
		pCommand->nType = SETTINGSFILECOMMANDTYPE_RECALL;
		pCommand->jArg1.i = 1;
		nTokenLength = GetToken(pcToken,pcNewLine, nPos, nLineLength);	 // get the value
		if (nTokenLength>0)
		{
			strToken = pcToken;
			pCommand->jArg1 = m_dlgParent->ConvertString2Value(strToken,pCommand->nArg1ValueType);
			if (pCommand->nArg1ValueType != 0)
			{
				pCommand->jArg1.i = (int)(pCommand->jArg1.f + 0.5);
			}

		}


	}
	else if (strcmp(pcToken,"PAUSE") == 0)
	{
		pCommand->nType = SETTINGSFILECOMMANDTYPE_PAUSE;
		pCommand->jArg1.i = 1000;
		nTokenLength = GetToken(pcToken,pcNewLine, nPos, nLineLength);	 // get the value
		if (nTokenLength>0)
		{
			strToken = pcToken;
			pCommand->jArg1 = m_dlgParent->ConvertString2Value(strToken,pCommand->nArg1ValueType);
			if (pCommand->nArg1ValueType != 0)
			{
				pCommand->jArg1.i = (int)(pCommand->jArg1.f + 0.5);
			}

		}

	}
	else if (strcmp(pcToken,"PROMPT") == 0)
	{
		pCommand->nType = SETTINGSFILECOMMANDTYPE_PROMPT;

		strncpy(pCommand->pcArg1,pcNewLine+nPos,255);
		//nTokenLength = GetToken(pcToken,pcNewLine, nPos, nLineLength);	 // get the value
		//if (nTokenLength>0)
		//{
		//	strcpy(pCommand->pcArg2,pcToken);
		//}
		//else
		//{
		//	strcpy(pCommand->pcArg2,"Prompt");
		//}
	}
	else if (strcmp(pcToken,"MAP") == 0)
	{
		pCommand->nType = SETTINGSFILECOMMANDTYPE_MAP;

		nTokenLength = GetToken(pcToken,pcNewLine, nPos, nLineLength);	 // get the value
		if (nTokenLength>0)
		{
			strToken = pcToken;
			if (strcmp(pcToken,"reset")==0)
			{
				pCommand->jArg1.i = 0;
			}
			else
			{
				pCommand->jArg1 = m_dlgParent->ConvertString2Value(strToken,pCommand->nArg1ValueType);

				nTokenLength = GetToken(pcToken,pcNewLine, nPos, nLineLength);	 // get the value
				pCommand->nArg2ValueType = 2;
				strcpy(pCommand->pcArg2,pcToken);
			}
			return true;
		}
		else
		{
			strError.Format("No Speaker Name");
			return false;
		}
		return true;
	}
	else
	{
		if (strlen(pcToken) == 2)
		{
			pCommand->nType = SETTINGSFILECOMMANDTYPE_RAWDATA;
			nValue = strtol(pcToken,&next,16);
			pCommand->pcRawData[pCommand->nRawDataPos++] = nValue & 0x0ff;
			while ((nTokenLength>0) &&  (pCommand->nRawDataPos < (SETTINGSFILE_RAWDATA_MAX-1)) )
			{
				nTokenLength = GetToken(pcToken,pcNewLine, nPos, nLineLength);	 // get the value
				if (nTokenLength > 0)
				{
					pCommand->pcRawData[pCommand->nRawDataPos++] = strtol(pcToken,&next,16) & 0x0ff;
				}
			}
			return true;
		}
		else
		{
			pCommand->nType = SETTINGSFILECOMMANDTYPE_PARAMETEREDIT;
			strcpy(pCommand->pcArg1,pcToken);
			pCommand->nArg1ValueType = 2; // string
			pParameterDesc = m_dlgParent->ConvertString2ParameterDesc(m_nSettingsFileProductType, pcToken,&pCommand->nPIDTarget,&pCommand->nPID);

			if (pParameterDesc)
			{
				nTokenLength = GetToken(pcToken,pcNewLine, nPos, nLineLength);	 // get the value
				if (nTokenLength == 0)
				{
					strError.Format("No Value found for Parameter %s",pCommand->pcArg1);
					return false;
				}
				strToken = pcToken;
				pCommand->jArg2 = m_dlgParent->ConvertString2Value(strToken,pCommand->nArg2ValueType);

				pCommand->nPIDInstance = 0;
				nTokenLength = GetToken(pcToken,pcNewLine, nPos, nLineLength);	 // get the value
				if (nTokenLength != 0)
				{
					strToken = pcToken;
					int nType;
					intfloat j;
					j =  m_dlgParent->ConvertString2Value(strToken,nType);
					if (nType ==0)
					{
						pCommand->nPIDInstance = j.i;
					}
					else
					{
						pCommand->nPIDInstance = j.f;
					}
				}

				return true;
			}
			else
			{
				strError.Format("Error: Parameter not found for %s",pCommand->pcArg1);
				return false;
			}
		}
	}
	return true;
}
/*
			switch(pParameterDesc->format)
			{
			case eFORMAT_FLOAT:
				if (nValueType == 0) 
					nValue.f = nValue.i;
				SendParamEdit(nTarget,nPID,nValue.ui);
				break;
			case eFORMAT_SIGNED:
			case eFORMAT_UNSIGNED:
			case eFORMAT_BOOL:
				if (nValueType == 1)
					nValue.i = nValue.f;
				SendParamEdit(nTarget,nPID,nValue.ui);
				break;
			default:
				break;

			}
*/

	

bool CSettingsFile::SettingsFileRead(char* strInputTextFile, CString& strErrorMsg)
{
	// read the file and looks for errors
	// return true if ok
	bool bDone = false;
	int nLineLength;
	char pcNewLine[256];
	char pcNewFileName[256];
	int nLineNum = 0;
	tSettingFileCommandLine pCommand;
	CString errorMsg;
	bool bOk = true;
	CFile fReadFile;
	
//	int nParameterDelayCount = 10;


	if(!fReadFile.Open(strInputTextFile, CFile::modeRead))
	{
		strErrorMsg.Format("Error Opening file: %s",strInputTextFile);
		return false;
	}

	while(!bDone)	// file is done
	{
		nLineNum++;
		nLineLength = GetLine(&fReadFile,pcNewLine,&bDone, 255-1);

		if (nLineLength >  0)
		{
			if (!SettingsFileParseLine(pcNewLine,nLineLength, &pCommand, errorMsg))
			{
				strErrorMsg.Append(errorMsg);
				strErrorMsg.Append("\n");
				bOk = false;
			}
		}
	}
	fReadFile.Close();
	return bOk;
}		
bool IsInteger(char* pcString)
{
	int i;
	for(i=0;i<strlen(pcString);i++)
	{
		if ((pcString[i]<'0') || (pcString[i]>'9')) return false;
	}
	return true;
}

int CSettingsFile::SendParamEditMultiFIR(int nTarget, uint32 nParam[], intfloat* jValue, int nNumofParams, int nInstance)
{
	bool bOk = true;
	if (nNumofParams>100) return 0;
	m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
	// nParams must be contiguous ... we can assum this.

	for(int i=1;i<nNumofParams;i++)
	{
		if (nParam[i] !=  (nParam[i-1]+1))
		{
			bOk = false;
		}
	}

	if (!bOk)
	{
		return 0;
	}

	m_dlgParent->m_MandolinComm.CreateParameterEditSetNeighborhood(&m_dlgParent->m_msgTx,m_dlgParent->m_nTxSequence,nTarget,nParam[0],jValue,nNumofParams,true,nInstance);
	if (!m_bSettingsFileBlockSends) m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);

	if (m_bSettingsFileInProgress)
	{
		m_msgSettingsFile.payload = m_uiSettingsFilePayload;
		m_dlgParent->CopyMandolinMessage(&m_msgSettingsFile,&m_dlgParent->m_msgTx);
		m_nSettingsFileRetryCount = 0;
	}

	return m_dlgParent->m_msgTx.sequence;	
}
int CSettingsFile::SendParamEditMulti(int nTarget, uint32* nParam, intfloat* jValue, int nNumofParams, int nInstance)
{

	if (nNumofParams>100) return 0;
	m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
	m_dlgParent->m_MandolinComm.CreateParameterEditSet(&m_dlgParent->m_msgTx,m_dlgParent->m_nTxSequence,nTarget,nParam,jValue,nNumofParams,true,nInstance);
	if (!m_bSettingsFileBlockSends) m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);

	if (m_bSettingsFileInProgress)
	{
		m_msgSettingsFile.payload = m_uiSettingsFilePayload;
		m_dlgParent->CopyMandolinMessage(&m_msgSettingsFile,&m_dlgParent->m_msgTx);
		m_nSettingsFileRetryCount = 0;
	}

	return m_dlgParent->m_msgTx.sequence;	
}
int  CSettingsFile::SendRawMessage(char *pMsgBuffer, uint16 nLength)
{
	//unsigned char pCMWrapBuffer[1024*3+CMWRAP_HEADER_WORDS*4];
	//CMWrap_message mCMWrapMsg;
	//unsigned char pCMWrapMsgBuffer[1024*3];
	//bool bControl = true;
	char* pBuffer = pMsgBuffer;
	bool bExpectingResponse = false;



	while (MANDOLIN_MSG_readbuffer((uint8**)&pBuffer, &nLength,&m_dlgParent->m_msgTx, m_dlgParent->m_msgTxPayload))
	{

		// see if parameter edit
		if (m_dlgParent->m_msgTx.id == MANDOLIN_MSG_SET_APPLICATION_PARAMETER)
		{
			// to update the values
			m_dlgParent->ProcessMessageParameterSet(&m_dlgParent->m_msgTx,
				m_dlgParent->m_msgTx.length?(m_dlgParent->m_msgTx.length+MANDOLIN_HEADER_WORDS+1):MANDOLIN_HEADER_WORDS); // to show the parameter
		}
		if (!m_bSettingsFileBlockSends) m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx,bExpectingResponse);
	}
	return (pBuffer-pMsgBuffer);
}


#define SETTINGFILESENDNEXT_MAX	50 //100 //MAX_PARAMEDIT_MULTI 		// number of parameter to send at the same time.
#define SETTINGSFILESENDNEXT_RAWDATA_MAX (24*22+100)
bool CSettingsFile::SettingsFileSendNext(char* strInputTextFile, int& nFilePos, bool& bDone, CString& strErrorMsg)
{
	// read the file and looks for errors
	// return true if ok
	int nLineLength;
	char pcNewLine[256];
	char pcNewFileName[256];
	int nLineNum = 0;
	tSettingFileCommandLine pCommand;
	CString errorMsg;
	bool bOk = true;
	CFile fReadFile;
	intfloat jValue[SETTINGFILESENDNEXT_MAX+10];
	uint32 nPID[SETTINGFILESENDNEXT_MAX+10];
	uint32 nTarget = -1; //eTARGET_INVALID;
	uint32 nInstance = 0;
	tParameterDesc* pParameterDesc;
	int nNumofCommands = 0;
	void** pParamAddress;

	char pcRawData[SETTINGSFILESENDNEXT_RAWDATA_MAX];
	int nRawDataPos = 0;
	int nNumPIDs = 0;
	int nFilePosPrev;
	tSettingsFileCommandType nCommandType =	SETTINGSFILECOMMANDTYPE_NONE;
	
//	int nParameterDelayCount = 10;

	bDone = false;

	if(!fReadFile.Open(strInputTextFile, CFile::modeRead))
	{
		strErrorMsg.Format("Error Opening file: %s",strInputTextFile);
		return false;
	}
	fReadFile.Seek(nFilePos,CFile::begin);

	nFilePosPrev = nFilePos;

	while(!bDone)	// file is done
	{
		nLineNum++;

		nLineLength = GetLine(&fReadFile,pcNewLine,&bDone, 255-1);

		if (nLineLength ==  0)
		{
			bDone = true;
			//return true;
		}
		else
		{
			if (!SettingsFileParseLine(pcNewLine,nLineLength, &pCommand, errorMsg))
			{
				strErrorMsg.Append(errorMsg);
				strErrorMsg.Append("\n");
				bOk = false;
				bDone = true;
				return false;
			}
			else
			{
				
				if ((nCommandType != pCommand.nType) && (nCommandType == SETTINGSFILECOMMANDTYPE_PARAMETEREDIT)
					&& (nNumPIDs > 0))  // previous was parmaeter edit
				{
					SendParamEditMulti(nTarget,(uint32*)nPID,jValue,nNumPIDs);
					nNumPIDs = 0;
					bDone = false;
					return true;

				}

				if ((nCommandType != pCommand.nType) && (nCommandType == SETTINGSFILECOMMANDTYPE_RAWDATA)
					&& (nRawDataPos > 0))  // previous was a raw data msg
				{


					//SendParamEditMulti(nTarget,(uint32*)nPID,jValue,nNumPIDs, m_pcSettingsFileConmonDeviceName);
					int nSent = SendRawMessage(pcRawData, nRawDataPos);
					nRawDataPos = 0;	//leave the remaining
					//for(int i=nSent;i<nRawDataPos;i++)
					//{
					//	pcRawData[i] = pcRawData[i+nSent];	// transfer the remaining
					//}
					//nRawDataPos = nRawDataPos-nSent;
					bDone = false;
					return true;

				}

				nFilePos = fReadFile.GetPosition();
				nFilePosPrev = nFilePos;
				nNumofCommands++;


				switch (pCommand.nType)
				{
				case SETTINGSFILECOMMANDTYPE_PARAMETEREDIT:

					if ((nCommandType == pCommand.nType) || (nCommandType == SETTINGSFILECOMMANDTYPE_NONE))  // same or none of
					{
						nCommandType = SETTINGSFILECOMMANDTYPE_PARAMETEREDIT;

						if (m_dlgParent->IsTargetFIR(m_nSettingsFileProductType,nTarget))
						{

							if (nNumPIDs > 0)
							{

								if ( (pCommand.nPIDTarget != nTarget)  || (pCommand.nPIDInstance != nInstance ||  ( (nPID[nNumPIDs-1]+1)!= pCommand.nPID)) )
								{
									SendParamEditMultiFIR(nTarget,(uint32*)nPID,jValue,nNumPIDs );
									nNumPIDs = 0;
									bDone = false;
									return true;
								}
							}

						}
						else
						{

							if (((pCommand.nPIDTarget != nTarget) || (pCommand.nPIDInstance != nInstance)) && (nNumPIDs > 0))
							{
								SendParamEditMulti(nTarget,(uint32*)nPID,jValue,nNumPIDs, pCommand.nPIDInstance);
								nNumPIDs = 0;
								bDone = false;
								return true;
	
							}
						}
						nTarget = pCommand.nPIDTarget;
						nInstance = pCommand.nPIDInstance;

						
						nPID[nNumPIDs] = pCommand.nPID;

						pParameterDesc = m_dlgParent->GetParameterDesc(m_nSettingsFileProductType, nTarget,nPID[nNumPIDs]);
 
						jValue[nNumPIDs] = pCommand.jArg2;
			
						switch(pParameterDesc->format)
						{
						case eFORMAT_FLOAT:
							if ( pCommand.nArg2ValueType == 0) 
								jValue[nNumPIDs].f =jValue[nNumPIDs].i;
							break;
						case eFORMAT_SIGNED:
						case eFORMAT_UNSIGNED:
						case eFORMAT_BOOL:
							if ( pCommand.nArg2ValueType == 1)
								jValue[nNumPIDs].i = jValue[nNumPIDs].f;
							break;
						default:
							break;

						}

						pParamAddress = m_dlgParent->GetParamAddress(nTarget,nPID[nNumPIDs]);
						if (pParamAddress)
						{
							*pParamAddress = (void*) jValue[nNumPIDs].vp;
						}
						m_dlgParent->ShowParameter(nTarget,nPID[nNumPIDs],true);
						nNumPIDs++;
						if (nNumPIDs == SETTINGFILESENDNEXT_MAX)
						{
							if (m_dlgParent->IsTargetFIR(m_nSettingsFileProductType,nTarget))
							{
								SendParamEditMultiFIR(nTarget,(uint32*)nPID,jValue,nNumPIDs );
							}
							else
							{
								SendParamEditMulti(nTarget,(uint32*)nPID,jValue,nNumPIDs );
							}
							nNumPIDs = 0;
							bDone = false;
							return true;
						}
	
					}
					else
					{	
						// previous was a speaker?
						if (nNumPIDs > 0)
						{
							if (m_dlgParent->IsTargetFIR(m_nSettingsFileProductType,nTarget))
							{
								SendParamEditMultiFIR(nTarget,(uint32*)nPID,jValue,nNumPIDs );
							}
							else
							{
								SendParamEditMulti(nTarget,(uint32*)nPID,jValue,nNumPIDs );
							}
							nNumPIDs = 0;
						}
						bDone = false;
						return true;

					}
					break;
				//case SETTINGSFILECOMMANDTYPE_MAP:

				//	//if ((nCommandType == pCommand.nType) || (nCommandType == SETTINGSFILECOMMANDTYPE_NONE))  // same or none of
				//	//{

				//		if (pCommand.jArg1.i == 0)
				//		{
				//			ResetSpeakerMap();
				//		}
				//		else
				//		{
				//		// do the speaker command
				//		// &&&& add code here
				//			AddSpeakerMap(pCommand.jArg1.i,pCommand.pcArg2);
				//		}
				//		
				//		// reset the command type
				//		nCommandType = SETTINGSFILECOMMANDTYPE_NONE;
				//		bDone = false;
				//		//return false;

				//	//}
				//	//else
				//	//{
				//	//}
	

				//	break;
				//case SETTINGSFILECOMMANDTYPE_SPEAKER:
				//	//if ((nCommandType == pCommand.nType) || (nCommandType == SETTINGSFILECOMMANDTYPE_NONE))  // same or none of
				//	//{
				//		// do the speaker command
				//		// &&&& add code here
				//		if (IsInteger(pCommand.pcArg1))
				//		{
				//			intfloat j;
				//			int k;
				//			int nType;
				//			j = m_dlgParent->ConvertString2Value(pCommand.pcArg1,nType);
				//			k = FindSpeakerMap(j.i);
				//			if (k>-1)
				//			{
				//				strcpy(pCommand.pcArg1, m_smSpeakerMap[k].strName);
				//			}
				//			else
				//			{
				//				strErrorMsg.Format("Speaker Name does not have a map");
				//				return false;
				//			}
				//			if (pCommand.pcArg1[0] != 0)
				//				strcpy(m_pcSettingsFileConmonDeviceName, pCommand.pcArg1);

				//		}
				//		else
				//		{
				//			if (pCommand.pcArg1[0] != 0)
				//				strcpy(m_pcSettingsFileConmonDeviceName, pCommand.pcArg1);

				//		}
				//		

				//		// reset the command type
				//		nCommandType = SETTINGSFILECOMMANDTYPE_NONE;
				//	//
				//	//else
				//	//{
				//	//	// previous did not match
				//	//	if (nNumPIDs > 0)
				//	//	{
				//	//		SendParamEditMulti(nTarget,(uint32*)nPID,jValue,nNumPIDs, m_pcSettingsFileConmonDeviceName);
				//	//		nNumPIDs = 0;
				//	//		bDone = false;
				//	//		return true;
				//	//	}
				//	//}
				//	break;

				case SETTINGSFILECOMMANDTYPE_PRODUCT:
					switch (pCommand.jArg1.i)
					{
					case ePRODUCT_OLYSPEAKER:
						m_nSettingsFileProductType = pCommand.jArg1.i;
						m_dlgParent->SetProducttype(m_nSettingsFileProductType);
						break;
					case ePRODUCT_REDLINESPEAKER1:
						m_nSettingsFileProductType = pCommand.jArg1.i;
						m_dlgParent->SetProducttype(m_nSettingsFileProductType);
						break;
					
					case ePRODUCT_UNKNOWN:
					default:
						m_nSettingsFileProductType = ePRODUCT_UNKNOWN;
						strErrorMsg.Format("Invalid Product name... either ANYA or OTTO or ANNA");
						return false;
					}

					// reset the command type
					nCommandType = SETTINGSFILECOMMANDTYPE_NONE;

					break;

				case SETTINGSFILECOMMANDTYPE_STORE:
					//SendSnapshotStore(pCommand.jArg1.i,m_pcSettingsFileConmonDeviceName);
					// reset the command type
					nCommandType = SETTINGSFILECOMMANDTYPE_NONE;

					bDone = false;
					return true;

					break;

				case SETTINGSFILECOMMANDTYPE_RECALL:
					//SendSnapshotRecall(pCommand.jArg1.i,m_pcSettingsFileConmonDeviceName);
					// reset the command type
					nCommandType = SETTINGSFILECOMMANDTYPE_NONE;
					break;

				case SETTINGSFILECOMMANDTYPE_PAUSE:
					m_dlgParent->SetTimer(SETTINGSFILE_TIMERID,pCommand.jArg1.i,NULL);
					// reset the command type
					nCommandType = SETTINGSFILECOMMANDTYPE_NONE;
					m_bSettingsFileWaitingAck = false;
					bDone = false;
					return true;

					break;

				case SETTINGSFILECOMMANDTYPE_PROMPT:
					// reset the command type
					nCommandType = SETTINGSFILECOMMANDTYPE_NONE;
					m_bSettingsFileWaitingAck = false;
					m_dlgParent->KillTimer(SETTINGSFILE_TIMERID); // set the time out
					m_dlgParent->MessageBox(pCommand.pcArg1,"Prompt",MB_OK);
					m_dlgParent->SetTimer(SETTINGSFILE_TIMERID,100,NULL);
					bDone = false;
					return true;


				case SETTINGSFILECOMMANDTYPE_RAWDATA:
					{
						int nSent = 0;
						m_bSettingsFileWaitingAck = false;

						if ((pCommand.nRawDataPos + nRawDataPos) > SETTINGSFILESENDNEXT_RAWDATA_MAX)
						{
							nSent = SendRawMessage(pcRawData, nRawDataPos);
							for(int i=nSent;i<nRawDataPos;i++)
							{
								pcRawData[i] = pcRawData[i+nSent];	// transfer the remaining
							}
							nRawDataPos = nRawDataPos-nSent;
							//nRawDataPos = 0;

							if ((pCommand.nRawDataPos + nRawDataPos) > SETTINGSFILESENDNEXT_RAWDATA_MAX)	// must be an error
								nRawDataPos = 0;

						}
						for(int i=0;i<pCommand.nRawDataPos;i++)
						{
							pcRawData[nRawDataPos++] = pCommand.pcRawData[i];
						}
					}
					break;


				default:
				case SETTINGSFILECOMMANDTYPE_NONE:
					break;
				}
			}
		}
	}

	if (nNumPIDs > 0)
	{
		SendParamEditMulti(nTarget,(uint32*)nPID,jValue,nNumPIDs);
		nNumPIDs = 0;
	}

	//if (nRawDataPos > 0)
	//{
	//	SendRawMessage(pcRawData, nRawDataPos,m_pcSettingsFileConmonDeviceName);
	//	nRawDataPos = 0;
	//}

	bDone = bDone && (nNumofCommands == 0);

	return bOk;
}		
	


 void CSettingsFile::OnStart()
 {

	CString strErrorMsg;
	m_dlgParent->KillTimer(SETTINGSFILE_TIMERID);
	m_bSettingsFileInProgress = false;
	m_bSettingsFileWaitingAck = false;

		
	m_bSettingsFileDontWaitForAcks = true; // until device returns acks	
	m_bSettingsFileBlockSends = false;


	m_msgSettingsFile.length = 0;
	m_msgSettingsFile.id = -1;

	SettingsFileInit(m_strSettingsFileName.GetBuffer(), strErrorMsg);
	m_strSettingsFileName.ReleaseBuffer();
	if (!strErrorMsg.IsEmpty())
	{
		m_dlgParent->UpdateOutputText(strErrorMsg); 
		return;
	}


	m_nSettingsFilePos = 0;
	m_bSettingsFileInProgress = true;
	//m_nSettingsFileProductType = m_dlgParent->m_nProductType; //ePRODUCT_OLYSPEAKER;

	m_bSettingsFileWaitingAck = false;
	m_dlgParent->SetTimer(SETTINGSFILE_TIMERID,1,NULL);  // start the sending of the settings file

 }
 void CSettingsFile::OnStop()
 {
	m_dlgParent->KillTimer(SETTINGSFILE_TIMERID);
	m_bSettingsFileInProgress = false;
 }
void CSettingsFile::OnRead()
 {

	CString strErrorMsg;
	m_dlgParent->KillTimer(SETTINGSFILE_TIMERID);
	m_bSettingsFileInProgress = false;
	m_bSettingsFileWaitingAck = false;
	m_bSettingsFileDontWaitForAcks = true; // dont actually wait for acks.
	m_bSettingsFileBlockSends = true;	// dont send

	m_msgSettingsFile.length = 0;
	m_msgSettingsFile.id = -1;

	SettingsFileInit(m_strSettingsFileName.GetBuffer(), strErrorMsg);
	m_strSettingsFileName.ReleaseBuffer();
	if (!strErrorMsg.IsEmpty())
	{
		m_dlgParent->UpdateOutputText(strErrorMsg); 
		return;
	}


	m_nSettingsFilePos = 0;
	m_bSettingsFileInProgress = true;
	//m_nSettingsFileProductType = m_dlgParent->m_nProductType; //ePRODUCT_OLYSPEAKER;

	m_bSettingsFileWaitingAck = false;
	m_dlgParent->SetTimer(SETTINGSFILE_TIMERID,1,NULL);  // start the sending of the settings file

 }