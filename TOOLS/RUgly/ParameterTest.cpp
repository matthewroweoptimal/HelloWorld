// ParameterTest.cpp : implementation file
//

#include "stdafx.h"
#include "RUgly.h"
#include "ParameterTest.h"

#include <math.h>



#include "RUglyDlg.h"

#define PARAMETERTEST_TIMEOUT_REFRESH  2000
#define	PARAMETERTEST_RATE 100


// CParameterTest dialog

IMPLEMENT_DYNAMIC(CParameterTest, CDialog)

CParameterTest::CParameterTest(CWnd* pParent /*=NULL*/)
	: CDialog(CParameterTest::IDD, pParent)
{

	m_dlgParent = NULL;
	m_nProductType = ePRODUCT_OLYSPEAKER;
	OnResetResults();

	OnClearMessages();

	m_bTestInProgress = false;
	m_nMaxNumParameters = 0;
	m_nMaxOnGoing = 0;

	m_nNumOnGoing = 0;
	m_fAverageOnGoing = 0;
	m_nAverageOnGoing = 0;

	char pcLogfileName[50] = "ParameterEditTestLogFile.txt";
		
	char ModuleName[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	char szFullPath[256];
	char strTempName[_MAX_PATH];

	::GetModuleFileName(AfxGetApp()->m_hInstance, szFullPath, sizeof(szFullPath));
	_splitpath(szFullPath,drive,dir,fname,ext);

	strcpy(strTempName,drive);
	strcat(strTempName,dir);
	strcat(strTempName,pcLogfileName);

	strcpy(m_pcLogFileName,strTempName);
	m_strLogFileName = strTempName;

}


CParameterTest::~CParameterTest()
{
}

void CParameterTest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParameterTest, CDialog)
	ON_BN_CLICKED(IDC_PARAMETERTEST_CLEAR, &CParameterTest::OnBnClickedParametertestClear)
	ON_BN_CLICKED(IDC_PARAMETERTEST_START, &CParameterTest::OnBnClickedParametertestStart)
	ON_BN_CLICKED(IDC_PARAMETERTEST_STOP, &CParameterTest::OnBnClickedParametertestStop)
	ON_BN_CLICKED(IDC_PARAMETERTEST_LOGFILESELECT, &CParameterTest::OnBnClickedParametertestLogfileselect)
	ON_BN_CLICKED(IDC_PARAMETERTEST_LOGFILECLEAR, &CParameterTest::OnBnClickedParametertestLogfileclear)
	ON_BN_CLICKED(IDC_PARAMETERTEST_STOPONFIRSTERROR, &CParameterTest::OnBnClickedParametertestStoponfirsterror)
END_MESSAGE_MAP()


// CParameterTest message handlers
BOOL CParameterTest::OnInitDialog()
{
	bool bRetVal;
	bRetVal = CDialog::OnInitDialog();

	OnUpdateResults();

	SetDlgItemText(IDC_PARAMETERTEST_MAXPARAM,"10");
	SetDlgItemText(IDC_PARAMETERTEST_MAXATONCE,"5");


	SetDlgItemText(IDC_PARAMETERTEST_LOGFILENAME, m_strLogFileName);

	return bRetVal;
}

void CParameterTest::OnTimer(UINT nIDEvent)
{
	switch(nIDEvent)
	{
	case PARAMETERTEST_TIMERID:
		ProcessTimer();
		SetParameterTestTimer();
		break;
	}

}

void CParameterTest::SetProductType(int nProductType)
{
	m_nProductType = nProductType;

	OnBnClickedParametertestLogfileclear();
}

void CParameterTest::SetParameterTestTimer()
{
	// set the clock
	uiStartTickCount = ((clock()*1000) / CLOCKS_PER_SEC);

	if (m_dlgParent)
		m_dlgParent->SetTimer(PARAMETERTEST_TIMERID,PARAMETERTEST_RATE,NULL);

}
void CParameterTest::ProcessTimer()
{
	// get the current clock and compare when started
	uint32 uiNowTick;
	uint32 uiDiff;
	uiNowTick = ((clock()*1000) / CLOCKS_PER_SEC);
	uiDiff = uiNowTick-uiStartTickCount;
	int i;

	// ----------------------------
	// check and process timeouts
	// ----------------------------
	if (!m_bTestInProgress) 
		return;
	for(i=0;i<MAX_PARAMETERTEST_MAXONGOING;i++)
	{
		if ((m_tTestMessages[i].nNumParameters > 0) && 
			(m_tTestMessages[i].nTimeoutCounter))
		{
			m_tTestMessages[i].nTimeoutCounter -= uiDiff;
			if (m_tTestMessages[i].nTimeoutCounter<= 0)
			{
				// Message Time out
				m_nNumTimeouts++;
				OnUpdateResults();
				OnFreeMessage(i);
				m_nNumOnGoing = OnGetOnGoing();

				if(	m_bStopOnFirstError)
				{
					OnBnClickedParametertestStop();
				}

			}
		}
	}

	// ----------------------------
	// start a new message
	// ----------------------------
	for(i=0;i<7; i++)		// try sending multiple in one loop just to get ongoing high enough
	{
		m_nNumOnGoing = OnGetOnGoing();
		if (m_nNumOnGoing < m_nMaxOnGoing) 
		{
			int nIndex = OnNewMessages();
			if	(nIndex >=0) 
			{
				SendParameterTestMessage(nIndex);
			}
		}
		else
		{
			break;
		}

	}

	m_nNumOnGoing = OnGetOnGoing();
	m_fAverageOnGoing = m_fAverageOnGoing*.8 + m_nNumOnGoing*.2;

	i = m_fAverageOnGoing+.5;
	if (i != m_nAverageOnGoing)
	{
		m_nAverageOnGoing = i;
		OnUpdateResults();
	}

	//uiStartTickCount = uiNowTick;

}
void	CParameterTest::ProcessMessageParameterSetReply(mandolin_message* pInMsg, int theLength)
{

	bool bIsReply;
	bool bFound;
	int i;

	bIsReply = IS_MANDOLIN_REPLY(pInMsg->transport);

	if (!bIsReply) return;
	if (!m_dlgParent) return;

	if (!m_bTestInProgress) return;

	if(IS_MANDOLIN_NACK(pInMsg->transport))
	{
		bFound = false;
		for(i=0;i<MAX_PARAMETERTEST_MAXONGOING;i++)
		{
			if ((m_tTestMessages[i].nNumParameters != 0)  &&
				(m_tTestMessages[i].nSequenceSet == pInMsg->sequence))
			{
				bFound = true;
				break;
			}

		}
		if (bFound)
		{
			// found an error on the set value
			OnFreeMessage(i);
			m_nNumInvalidSet++;
			OnUpdateResults();
			if(	m_bStopOnFirstError)
			{
				OnBnClickedParametertestStop();
			}

		}
	}
}


void	CParameterTest::ProcessMessageParameterGetReply(mandolin_message* pInMsg, int theLength)
{
	uint32  flags;
    uint32 *in_ptr = (uint32 *) pInMsg->payload;
    
    bool valid_edit = false;
    
	int nReceived = 0;

    int nParameterNumber;
	int k;
	int nDirection;
	int nNumOfParameters;
	int nInstance;
	int nSequence;
	int nTarget;
	tParameterDesc* pParameterDesc;

	tParameterDesc* pParameterDescList[MAX_PARAMETERTEST_PARAMETERS];
	uint32  uiPIDList[MAX_PARAMETERTEST_PARAMETERS];
	intfloat jValues[MAX_PARAMETERTEST_PARAMETERS];



	bool bMultiParameter = false;


	intfloat j;
   	bool bIsReply;

	bIsReply = IS_MANDOLIN_REPLY(pInMsg->transport);

	if (!bIsReply) return;
	if (!m_dlgParent) return;
	if (!m_bTestInProgress) return;

   
    nParameterNumber = in_ptr[0];
	flags = in_ptr[1];

	nSequence = pInMsg->sequence;
	nDirection = MANDOLIN_WILD_DIRECTION(flags);
	nTarget = MANDOLIN_EDIT_TARGET(flags);
	nInstance = MANDOLIN_WILD_MAP(flags);
	if (nInstance < 0) nInstance = 0;
	nNumOfParameters = pInMsg->length-2;

	if (MANDOLIN_WILD_TYPE(flags) == MANDOLIN_TYPE_MULTIPARAMETER)
	{
		bMultiParameter = true;
	}
 

	if (nTarget == m_dlgParent->GetMetersTarget(m_nProductType))
	{
		return;
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

			pParameterDesc = m_dlgParent->GetParameterDesc(m_nProductType, nTarget,nParameterNumber);
			if (!pParameterDesc)
			{
				//valid_edit = false;
				break;

			}

			// --------------------------------------------------------
			// decode the Parameter
			// --------------------------------------------------------

			j.ui = in_ptr[k+2];

			pParameterDescList[nReceived] = pParameterDesc;
			uiPIDList[nReceived] = nParameterNumber;
			jValues[nReceived].vp = j.vp;
			nReceived++;

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

	CompareReceivedMessage(nSequence,nTarget,nInstance,uiPIDList, jValues, pParameterDescList,nReceived);


//	return true;
}



void CParameterTest::OnClearMessages()
{
	int i;

	for(i=0;i<MAX_PARAMETERTEST_MAXONGOING;i++)
	{
		m_tTestMessages[i].nStatus = PARAMETERTEST_STATUS_EMPTY;
		m_tTestMessages[i].nNumParameters = 0;
	}

	m_nNumOnGoing = 0;


}



int CParameterTest::OnGetOnGoing()
{
		// compute the number of on going
	int i;
	int nCount = 0;

	for(i=0;i<MAX_PARAMETERTEST_MAXONGOING;i++)
	{
		if (m_tTestMessages[i].nNumParameters != 0)
			nCount++;
	}

	return nCount;

}
bool CParameterTest::SendParameterTestMessage(int nIndex)
{
	if (!m_dlgParent) return false;
	
	m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
	if (m_dlgParent->m_MandolinComm.CreateParameterEditSet(&m_dlgParent->m_msgTx,m_dlgParent->m_nTxSequence,
		m_tTestMessages[nIndex].nTarget,
		m_tTestMessages[nIndex].uiPID,
		m_tTestMessages[nIndex].jValues,
		m_tTestMessages[nIndex].nNumParameters, 
		false,
		m_tTestMessages[nIndex].nInstance)) 
	{
		if (!m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true))
		{
			OnFreeMessage(nIndex);
			return false;
		}
		else
		{
			m_tTestMessages[nIndex].nSequenceSet = m_dlgParent->m_msgTx.sequence;
			LogEditsToFile(m_tTestMessages[nIndex].nTarget,m_tTestMessages[nIndex].uiPID, m_tTestMessages[nIndex].jValues, m_tTestMessages[nIndex].nNumParameters, m_tTestMessages[nIndex].nInstance);
		}
	}
	else
	{
			OnFreeMessage(nIndex);
			return false;
	}
	
		
	
	m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
	if (m_dlgParent->m_MandolinComm.CreateParameterEditGetMulti(&m_dlgParent->m_msgTx,m_dlgParent->m_nTxSequence,
		m_tTestMessages[nIndex].nTarget,
		m_tTestMessages[nIndex].uiPID,
		m_tTestMessages[nIndex].nNumParameters, 
		m_tTestMessages[nIndex].nInstance))   
	{
		if (!m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true))
		{
			OnFreeMessage(nIndex);
			return false;
		}
		m_tTestMessages[nIndex].nSequenceGet = m_dlgParent->m_msgTx.sequence;
	}
	else
	{
			OnFreeMessage(nIndex);
			return false;
	}
	m_tTestMessages[nIndex].nStatus = PARAMETERTEST_STATUS_SENT;
	m_tTestMessages[nIndex].nTimeoutCounter = PARAMETERTEST_TIMEOUT_REFRESH;

	m_nNumTests++;
	OnUpdateResults();

	m_nNumOnGoing++;
	return true;

}
void CParameterTest::CompareReceivedMessage(int nSequence,int nTarget,int nInstance,uint32* uiPID, intfloat* jValues, tParameterDesc** pParameterDesc,int nReceived)
{

	int i,j;
	bool bFound = false;
	bool bFailed = false;
	float fDiff;
	uint32 nDiff;

	for(i=0;i<MAX_PARAMETERTEST_MAXONGOING;i++)
	{
		if ((m_tTestMessages[i].nNumParameters != 0)  &&
			(m_tTestMessages[i].nSequenceGet == nSequence)  &&
			(m_tTestMessages[i].nTarget == nTarget)  &&
			(m_tTestMessages[i].nInstance == nInstance))
		{
			bFound = true;
			break;
		}

	}
	if (!bFound) 
		return;

	if (m_tTestMessages[i].nNumParameters != nReceived)
	{
		bFailed = true;
	}
	else
	{

		for(j=0;j<nReceived; j++)
		{
			if (m_tTestMessages[i].uiPID[j] != uiPID[j])
			{
				bFailed = true;
			}
			else
			{
				switch(pParameterDesc[j]->format)
				{
				case eFORMAT_FLOAT:
					fDiff = m_tTestMessages[i].jValues[j].f - jValues[j].f;
					if ((fDiff < -0.005) || (fDiff > 0.005))
						bFailed = true;
					
					break;
				case eFORMAT_SIGNED:
				case eFORMAT_UNSIGNED:
				case eFORMAT_BOOL:
				default:
					nDiff = m_tTestMessages[i].jValues[j].ui - jValues[j].ui;
					if (nDiff != 0)
						bFailed = true;
					break;
				}
			}
		}
	}
	OnFreeMessage(i);
	if (bFailed)
	{
		m_nNumValueMismatch++;
		if(	m_bStopOnFirstError)
		{
			OnBnClickedParametertestStop();
		}
	}
	else
	{
		// passed
		m_nNumPasses++;
	}
	OnUpdateResults();
	return;

}

int CParameterTest::OnNewMessages()
{	
	// compute the number of on going
	int i;
	int j;
	int nIndex;
	int nTarget;
	int nInstance;
	int nNumParameters;
	int nMaxTargetParameters;	// for the target
	int nMaxMsgParameters;
	double r;
	uint32 nPID;
	bool bFound;
	tParameterDesc* pParameterDesc;
	intfloat jVal;	
	char pcBuffer[200];
	int nRandomMsgLength;

	if (!m_dlgParent)
		return -1;

	m_nNumOnGoing = OnGetOnGoing();
	if (m_nNumOnGoing >= m_nMaxOnGoing) 
		return -1;

	nIndex = FindEmptyMessageIndex();
	if (nIndex < 0) 
		return -1;

	nMaxTargetParameters = m_dlgParent->GetRandomTarget(&nTarget,&nInstance);	// 1..n

	if (nMaxTargetParameters <= 0) 
		return -1;

	m_tTestMessages[nIndex].nTarget = nTarget;
	m_tTestMessages[nIndex].nInstance = nInstance;

	// get the small of the 2
	nMaxMsgParameters = (nMaxTargetParameters < m_nMaxNumParameters)?nMaxTargetParameters:m_nMaxNumParameters;

	if (nMaxMsgParameters < 1) 
		return -1;

	nRandomMsgLength = nMaxMsgParameters;
	r = (double)(rand()*1.0)/RAND_MAX;
	nRandomMsgLength = r*(nMaxMsgParameters-1);
	nRandomMsgLength  = nRandomMsgLength + 1;
	if (nRandomMsgLength > nMaxMsgParameters) 
		nRandomMsgLength = nMaxMsgParameters;
	if (nRandomMsgLength < 1) 
		nRandomMsgLength = 1;
	//nRandomMsgLength = nMaxMsgParameters;		// DEBUG for now

	// found Message index
	for(i=0;i<nRandomMsgLength;i++)
	{
		bFound = false;
		int nTries = 0;
		while (!bFound)
		{
			// skip nPID 0... find random number
			r = (double)(rand()*1.0)/RAND_MAX;
			nPID = r*(nMaxTargetParameters-1);
			if (nPID >= (nMaxTargetParameters-1)) nPID = (nMaxTargetParameters-1);
			nPID++; 
			
			bFound = true;
			for(j=0;j<i;j++)
			{
				if (m_tTestMessages[nIndex].uiPID[j] == nPID)
				{
					bFound = false;
				}
			}

			if (bFound) 
				break;

			nTries++;
			if (nTries > 500) 
				return -1;
		}
		m_tTestMessages[nIndex].uiPID[i] = nPID;


		pParameterDesc = m_dlgParent->GetParameterDesc(m_nProductType, nTarget, nPID);
		if (!pParameterDesc)
			return -1;

		jVal  =  m_dlgParent->GetParameterRandomValue(pParameterDesc,pcBuffer);
		m_tTestMessages[nIndex].jValues[i] = jVal;
	}
	m_tTestMessages[nIndex].nSequenceSet = 0;
	m_tTestMessages[nIndex].nSequenceGet = 0;
	m_tTestMessages[nIndex].nTimeoutCounter = 0;
	m_tTestMessages[nIndex].nNumParameters = nRandomMsgLength;
	m_tTestMessages[nIndex].nStatus = PARAMETERTEST_STATUS_CREATED;

	return nIndex;

}
void  CParameterTest::OnFreeMessage(int nIndex)
{
	m_tTestMessages[nIndex].nNumParameters = 0;
	m_tTestMessages[nIndex].nStatus = PARAMETERTEST_STATUS_EMPTY;

}
int CParameterTest::FindEmptyMessageIndex()
{
	// return -1 if not found
	int i;

	for(i=0;i<MAX_PARAMETERTEST_MAXONGOING;i++)
	{
		if (m_tTestMessages[i].nNumParameters == 0) 
			return i;
	}
	return -1;

}

void CParameterTest::OnResetResults()
{
	m_nNumTests = 0;
	m_nNumTimeouts = 0;
	m_nNumValueMismatch = 0;
	m_nNumInvalidSet = 0;
	m_nNumPasses = 0;

	m_fAverageOnGoing = 0;
	m_nAverageOnGoing = 0;

}

void CParameterTest::OnUpdateResults()
{
	char pcBuffer[200];

	sprintf(pcBuffer, "%d",m_nNumTests);
	SetDlgItemText(IDC_PARAMETERTEST_NUMTESTS,pcBuffer);
	
	sprintf(pcBuffer, "%d",m_nNumTimeouts);
	SetDlgItemText(IDC_PARAMETERTEST_NUMTIMEOUTS,pcBuffer);

	sprintf(pcBuffer, "%d",m_nNumValueMismatch);
	SetDlgItemText(IDC_PARAMETERTEST_NUMVALUEERRORS,pcBuffer);

	sprintf(pcBuffer, "%d",m_nNumInvalidSet);
	SetDlgItemText(IDC_PARAMETERTEST_NUMINVALIDSET,pcBuffer);


	sprintf(pcBuffer, "%d",m_nNumPasses);
	SetDlgItemText(IDC_PARAMETERTEST_NUMPASSES,pcBuffer);

	sprintf(pcBuffer, "%d",m_nAverageOnGoing);
	SetDlgItemText(IDC_PARAMETERTEST_AVEONGOING,pcBuffer);


	
}

void CParameterTest::OnBnClickedParametertestClear()
{
	// TODO: Add your control notification handler code here
	OnResetResults();

	OnUpdateResults();

}

void CParameterTest::OnBnClickedParametertestStart()
{
	// TODO: Add your control notification handler code here
	intfloat jValue;
	int	nValueType;
	char pcBuffer[200];

	if (!m_dlgParent)
		return;
	
	m_bStopOnFirstError = false;
	CButton* pButton = (CButton*) GetDlgItem(IDC_PARAMETERTEST_STOPONFIRSTERROR);
	if (pButton)
	{
		m_bStopOnFirstError = (pButton->GetCheck() != 0);
	}

	if (m_bTestInProgress)
	{
		// stop
		OnBnClickedParametertestStop();
	}

	// ------------------
	// read Max Params
	// ------------------
	GetDlgItemText(IDC_PARAMETERTEST_MAXPARAM,pcBuffer, sizeof(pcBuffer)-1);
	jValue = m_dlgParent->ConvertString2Value(pcBuffer,nValueType);
	if (nValueType == 1) // float
	{
		jValue.i = jValue.f;
	}
	if (jValue.i > MAX_PARAMETERTEST_PARAMETERS) jValue.i = MAX_PARAMETERTEST_PARAMETERS;
	if (jValue.i <= 0) jValue.i = 1;
	m_nMaxNumParameters = jValue.i;

	// ------------------
	// read On Going
	// ------------------
	GetDlgItemText(IDC_PARAMETERTEST_MAXATONCE,pcBuffer, sizeof(pcBuffer)-1);
	jValue = m_dlgParent->ConvertString2Value(pcBuffer,nValueType);
	if (nValueType == 1) // float
	{
		jValue.i = jValue.f;
	}
	if (jValue.i > MAX_PARAMETERTEST_MAXONGOING) jValue.i = MAX_PARAMETERTEST_MAXONGOING;
	if (jValue.i <= 0) jValue.i = 1;
	m_nMaxOnGoing = jValue.i;

	sprintf(pcBuffer,"%d",m_nMaxNumParameters);
	SetDlgItemText(IDC_PARAMETERTEST_MAXPARAM,pcBuffer);
	
	sprintf(pcBuffer,"%d",m_nMaxOnGoing);
	SetDlgItemText(IDC_PARAMETERTEST_MAXATONCE,pcBuffer);

	OnResetResults();
	OnClearMessages();
	OnUpdateResults();

	m_bTestInProgress = true;
	SetParameterTestTimer();		// starts the timer
}

void CParameterTest::OnBnClickedParametertestStop()
{
	// TODO: Add your control notification handler code here
	m_bTestInProgress = false;
	KillTimer(PARAMETERTEST_TIMERID);


}

void CParameterTest::OnBnClickedParametertestLogfileselect()
{
	// TODO: Add your control notification handler code here
			char szSelectFileTitle[_MAX_PATH];
		char szSelectFileFilter[256];
		BOOL bStatus;
		char szInputFileName[_MAX_PATH];

		strcpy(szInputFileName, m_strLogFileName.GetBuffer());
		m_strLogFileName.ReleaseBuffer();

		OPENFILENAME ofnSelectFileOpen = {  
		0,					//   DWORD        lStructSize;
		NULL,				//   HWND         hwndOwner;
		0,					//   HINSTANCE    hInstance;
		szSelectFileFilter,	//   LPCWSTR      lpstrFilter;
		NULL,				//   LPWSTR       lpstrCustomFilter;
		256,				//   DWORD        nMaxCustFilter;
		1,					//   DWORD        nFilterIndex;
		szInputFileName,//   LPWSTR       lpstrFile;
		256,				//   DWORD        nMaxFile;
		szSelectFileTitle,//  LPWSTR       lpstrFileTitle;
		256,				//   DWORD        nMaxFileTitle;
		NULL,//szSelectFileInitialDir,// LPCWSTR      lpstrInitialDir;
		"Select Input File",	//   LPCWSTR      lpstrTitle;
		0,					//   DWORD        Flags;
		0,					//   WORD         nFileOffset;
		0,					//   WORD         nFileExtension;
		"*.txt",			//   LPCWSTR      lpstrDefExt;
		0,					//   LPARAM       lCustData;
		NULL,				//   LPOFNHOOKPROC lpfnHook;
		NULL				//   LPCWSTR      lpTemplateName;
		};

		sprintf (szSelectFileFilter,
			"%s%c%s%c%s%c%s%c",
			"Text files (*.txt)",0,"*.txt",0,
			"All Files",0,"*.*",0,0);

		if (NULL != strstr(szInputFileName,".txt")) {
		 		ofnSelectFileOpen.nFilterIndex = 1;
		}

		ofnSelectFileOpen.lStructSize = sizeof(ofnSelectFileOpen);
		ofnSelectFileOpen.hwndOwner = this->m_hWnd;
		bStatus = GetOpenFileName(&ofnSelectFileOpen);


  		if (bStatus)
		{
			m_strLogFileName =  ofnSelectFileOpen.lpstrFile; //strcpy(m_pcLogFileName, ofnSelectFileOpen.lpstrFile);
			SetDlgItemText(IDC_PARAMETERTEST_LOGFILENAME, m_strLogFileName);
			//m_strLogFileName = ;
		}
}

void CParameterTest::OnBnClickedParametertestLogfileclear()
{
	// TODO: Add your control notification handler code here
	FILE* fLogfile;
	fLogfile = fopen(m_pcLogFileName,"w");
	if (fLogfile)
	{
		fclose(fLogfile);
	}
	return;
}

void CParameterTest::LogEditsToFile(int nTarget, uint32 *PIDs, intfloat* jVal, int nNum, int nInstance)
{
	// TODO: Add your control notification handler code here
	CString strLogRecord;
	//struct tm *newtime;
	CFile ZLogfile;
	char pcLogRecord[200] = {0};
	//char pcLogRecordWithTime[300];
	//static char pcLogRecordOld[200] = {0};
	//char pcBuffer[200];
	//time_t tNow;
	char pcString[200];
	tParameterDesc* pParameterDesc;
	void** pParameterValue;
	//intfloat jVal;
	int i;
	//char pcFormat[200];



	bool bEnable = true;
	CButton* pButton = (CButton*) GetDlgItem(IDC_PARAMETERTEST_LOGENABLE);

	if (pButton)
	{
		bEnable = (pButton->GetCheck() != 0);
	}
	if (!bEnable) return;

	//float fTemp0 = 0;
	//fTemp0 = ((signed char)(jMeters->uiTemperature & 0x0ff))/2.0;

	//float fTemp1;
	//fTemp1 = ((signed char)((jMeters->uiTemperature >> 8) & 0x0ff))/2.0;


	if (ZLogfile.Open(m_pcLogFileName,CFile::modeWrite | CFile::modeNoTruncate | CFile::modeCreate)) //NoTruncate))
	{
		int pos = ZLogfile.SeekToEnd();
		if (pos == 0)
		{
			pcString[0] = 0;
			switch(m_nProductType)
			{
			case ePRODUCT_OLYSPEAKER:
				strcpy(pcString,	"PRODUCT OLYSPEAKER\n");
				break;
			case ePRODUCT_REDLINESPEAKER1:
				strcpy(pcString,	"PRODUCT REDLINESPEAKER\n");
				break;
			}
			ZLogfile.Write(pcString,strlen(pcString));

		}


		for(i=0;i<nNum;i++)
		{
				pcString[0] = 0;
				//jVal.vp = nTargetList[j].ppAddr[i];
				pParameterDesc =  m_dlgParent->GetParameterDesc( m_dlgParent->m_nProductType, nTarget,PIDs[i]);
				if (pParameterDesc)
				{
					m_dlgParent->m_MandolinTextSummary.ConvertParameter2Text(pParameterDesc,jVal[i],pcString,false,nInstance);
					ZLogfile.Write(pcString,strlen(pcString));
				}

		}
	

		ZLogfile.Close();
	}



	return;
}	

void CParameterTest::OnBnClickedParametertestStoponfirsterror()
{
	// TODO: Add your control notification handler code here
	m_bStopOnFirstError = false;
	CButton* pButton = (CButton*) GetDlgItem(IDC_PARAMETERTEST_STOPONFIRSTERROR);
	if (pButton)
	{
		m_bStopOnFirstError = (pButton->GetCheck() != 0);
	}

}
