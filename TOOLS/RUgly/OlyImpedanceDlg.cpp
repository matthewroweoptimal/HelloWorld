// OlyImpedanceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RUgly.h"


#include "stdafx.h"
#include "LOUD_types.h"
extern "C" 
{
#include "mandolin.h"
}
#include "Product_arch.h"
#include "RUglyDlg.h"
#include "OlyImpedanceDlg.h"
#include <stdlib.h>
#include "resource.h"
#include <math.h>

#include "ProductMap.h"

// COlyImpedanceDlg dialog

IMPLEMENT_DYNAMIC(COlyImpedanceDlg, CDialog)

COlyImpedanceDlg::COlyImpedanceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COlyImpedanceDlg::IDD, pParent)
{
	m_dlgParent = NULL;
	m_nProductType = ePRODUCT_OLYSPEAKER;
	m_bInProgress = false;

	m_uiSequence = 1;

	char pcLogfileName[50] = "OLYImpedanceLogFile.txt";
		
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



	uint32* pValue;

	//	m_jMeters jMeters ={0};

	pValue = (uint32*) (void*)(&m_jMeters.ANGLE);
	for(int i=0;i<sizeof(m_jMeters)/sizeof(uint32);i++)
	{
		*(pValue+i) = 0;  // clear out alll the status values to 0
	}

}

COlyImpedanceDlg::~COlyImpedanceDlg()
{
}

void COlyImpedanceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COlyImpedanceDlg, CDialog)
	ON_BN_CLICKED(IDC_OLYIMPEDANCE_START, &COlyImpedanceDlg::OnBnClickedOlyimpedanceStart)
	ON_BN_CLICKED(IDC_OLYIMPEDANCE_STOP, &COlyImpedanceDlg::OnBnClickedOlyimpedanceStop)
	ON_BN_CLICKED(IDC_OLYIMPEDANCE_FILESELECT, &COlyImpedanceDlg::OnBnClickedOlyimpedanceFileselect)
	ON_BN_CLICKED(IDC_OLYIMPEDANCE_LOGCLEAR, &COlyImpedanceDlg::OnBnClickedOlyimpedanceLogclear)
END_MESSAGE_MAP()


// COlyImpedanceDlg message handlers

BOOL COlyImpedanceDlg::OnInitDialog()
{
	bool bStatus;
	bStatus = CDialog::OnInitDialog();

	SetDlgItemText(IDC_OLYIMPEDANCE_FILENAME, m_strLogFileName);

	UpdateStatusValues();

	return bStatus;
}
void COlyImpedanceDlg::OnTimer(UINT nIDEvent)
{

	switch(nIDEvent)
	{
	case OLYIMPEDANCE_TIMERID:
		m_mGetMeters.payload = m_pcGetMeterPayload;
		if (m_dlgParent)
		{
			m_dlgParent->m_MandolinComm.CreateParameterEditGetNeighborhood(&m_mGetMeters, m_uiSequence,eTARGET_STATUS,1,sizeof(tOLYStatusMeters)/(sizeof(int)));
			m_dlgParent->TryToSendMandolinMessage(&m_mGetMeters, true);
			m_dlgParent->SetTimer(OLYIMPEDANCE_TIMERID,500,NULL);
		}
		m_uiSequence++;
		if (m_uiSequence==0) m_uiSequence++;
		break;
	}
}


void COlyImpedanceDlg::OnStatusDecode(mandolin_message* pInMsg)
{
	
	uint32  flags;
    uint32 *in_ptr = (uint32 *) pInMsg->payload;
    
	uint32* puiValue;;

    int nParameterNumber;
	int k;
	int nTarget;
	int nDirection;
	int nNumOfParameters;
		
	bool bMultiParameter = false;
   	bool bIsReply;
	int nMaxZImpedanceMeter = sizeof(tOLYStatusMeters)/4;
	
	bIsReply = IS_MANDOLIN_REPLY(pInMsg->transport);

	if (!bIsReply) return;
   
    nParameterNumber = in_ptr[0];
	flags = in_ptr[1];

	nDirection = MANDOLIN_WILD_DIRECTION(flags);
	nTarget = MANDOLIN_EDIT_TARGET(flags);

	nNumOfParameters = pInMsg->length-2;

	if (MANDOLIN_WILD_TYPE(flags) == MANDOLIN_TYPE_MULTIPARAMETER)
	{
		bMultiParameter = true;
	}

	m_dlgParent->IsTargetMeters(m_nProductType,nTarget);

	if (nTarget != eTARGET_STATUS)
	{
		return;
	}

	if (m_nProductType != ePRODUCT_OLYSPEAKER)
	{
		return;
	}

	puiValue = (uint32*) (void*)(&m_jMeters.ANGLE);
	for(k=0;k<nNumOfParameters;k++)
	{

		if ((k & 0x01) && (bMultiParameter))
		{
			// 0=value, 1=nextid, 2=value, 3=next id
			nParameterNumber = in_ptr[k+2];

		}
		else
		{


			if (nParameterNumber <= nMaxZImpedanceMeter)
			{
				puiValue[nParameterNumber-1] = in_ptr[k+2];
			}
			else
			{
				break;
			}

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

	// update the meters
	char pcBuffer[200];

	UpdateStatusValues();


	UpdateStatusValues();
	LogResultsToFile(&m_jMeters);
//	return true;
}

void	COlyImpedanceDlg::UpdateStatusValues()
{
	char pcBuffer[200];

	// ---------------------------------------
	// status
	// ---------------------------------------
	//sprintf(pcBuffer,"%s",m_jMeters.GLOBAL_MUTE?"TRUE":"FALSE");
	//SetDlgItemText(IDC_OLYIMPEDANCE_MUTE,pcBuffer);
	sprintf(pcBuffer,"%d",m_jMeters.LIMITING);
	SetDlgItemText(IDC_OLYIMPEDANCE_LIMITING,pcBuffer);
	sprintf(pcBuffer,"%0x8.8x",m_jMeters.IDENTIFY);
	SetDlgItemText(IDC_OLYIMPEDANCE_IDENTIFY,pcBuffer);
	sprintf(pcBuffer,"%d",m_jMeters.FAN_ENABLED);
	SetDlgItemText(IDC_OLYIMPEDANCE_FANENABLE,pcBuffer);
	sprintf(pcBuffer,"%d",m_jMeters.ANGLE);
	SetDlgItemText(IDC_OLYIMPEDANCE_ANGLE,pcBuffer);

	// ---------------------------------------
	// array
	// ---------------------------------------
	sprintf(pcBuffer,"%d",m_jMeters.ARRAY_SIZE);
	SetDlgItemText(IDC_OLYIMPEDANCE_ARRAYSIZE,pcBuffer);
	sprintf(pcBuffer,"%d",m_jMeters.ARRAY_INDEX);
	SetDlgItemText(IDC_OLYIMPEDANCE_ARRAYINDEX,pcBuffer);
	sprintf(pcBuffer,"%8.8x",m_jMeters.ARRAY_ID_ABOVE);
	SetDlgItemText(IDC_OLYIMPEDANCE_ARRAYABOVE,pcBuffer);
	sprintf(pcBuffer,"%8.8x",m_jMeters.ARRAY_ID_BELOW);
	SetDlgItemText(IDC_OLYIMPEDANCE_ARRAYBELOW,pcBuffer);
	sprintf(pcBuffer,"0x%8.8x",m_jMeters.ARRAY_MISMATCH);
	SetDlgItemText(IDC_OLYIMPEDANCE_ARRAYMISMATCH,pcBuffer);



	// ---------------------------------------
	// temperatures
	// ---------------------------------------
	sprintf(pcBuffer,"%6.4f",m_jMeters.AMP1_TEMP);
	SetDlgItemText(IDC_OLYIMPEDANCE_AMPTEMP_1,pcBuffer);
	sprintf(pcBuffer,"%6.4f",m_jMeters.AMP2_TEMP);
	SetDlgItemText(IDC_OLYIMPEDANCE_AMPTEMP_2,pcBuffer);
	sprintf(pcBuffer,"%d",m_jMeters.AMP_FAULT);
	SetDlgItemText(IDC_OLYIMPEDANCE_AMPFAULT,pcBuffer);

	// ---------------------------------------
	// Impedance CH1
	// ---------------------------------------
	sprintf(pcBuffer,"%6.4f",m_jMeters.CS_V_CH1);
	SetDlgItemText(IDC_OLYIMPEDANCE_V_CH1,pcBuffer);
	sprintf(pcBuffer,"%6.4f",m_jMeters.CS_I_CH1);
	SetDlgItemText(IDC_OLYIMPEDANCE_I_CH1,pcBuffer);
	sprintf(pcBuffer,"%6.4f",m_jMeters.CS_Z_CH1);
	SetDlgItemText(IDC_OLYIMPEDANCE_Z_CH1,pcBuffer);
	sprintf(pcBuffer,"%d",m_jMeters.CS_FAULT_CH1);
	SetDlgItemText(IDC_OLYIMPEDANCE_FAULT_CH1,pcBuffer);

	// ---------------------------------------
	// Impedance CH2
	// ---------------------------------------
	sprintf(pcBuffer,"%6.4f",m_jMeters.CS_V_CH2);
	SetDlgItemText(IDC_OLYIMPEDANCE_V_CH2,pcBuffer);
	sprintf(pcBuffer,"%6.4f",m_jMeters.CS_I_CH2);
	SetDlgItemText(IDC_OLYIMPEDANCE_I_CH2,pcBuffer);
	sprintf(pcBuffer,"%6.4f",m_jMeters.CS_Z_CH2);
	SetDlgItemText(IDC_OLYIMPEDANCE_Z_CH2,pcBuffer);
	sprintf(pcBuffer,"%d",m_jMeters.CS_FAULT_CH2);
	SetDlgItemText(IDC_OLYIMPEDANCE_FAULT_CH2,pcBuffer);

	// ---------------------------------------
	// Impedance CH3
	// ---------------------------------------
	sprintf(pcBuffer,"%6.4f",m_jMeters.CS_V_CH3);
	SetDlgItemText(IDC_OLYIMPEDANCE_V_CH3,pcBuffer);
	sprintf(pcBuffer,"%6.4f",m_jMeters.CS_I_CH3);
	SetDlgItemText(IDC_OLYIMPEDANCE_I_CH3,pcBuffer);
	sprintf(pcBuffer,"%6.4f",m_jMeters.CS_Z_CH3);
	SetDlgItemText(IDC_OLYIMPEDANCE_Z_CH3,pcBuffer);
	sprintf(pcBuffer,"%d",m_jMeters.CS_FAULT_CH3);
	SetDlgItemText(IDC_OLYIMPEDANCE_FAULT_CH3,pcBuffer);


	// ---------------------------------------
	// Impedance CH4
	// ---------------------------------------
	sprintf(pcBuffer,"%6.4f",m_jMeters.CS_V_CH4);
	SetDlgItemText(IDC_OLYIMPEDANCE_V_CH4,pcBuffer);
	sprintf(pcBuffer,"%6.4f",m_jMeters.CS_I_CH4);
	SetDlgItemText(IDC_OLYIMPEDANCE_I_CH4,pcBuffer);
	sprintf(pcBuffer,"%6.4f",m_jMeters.CS_Z_CH4);
	SetDlgItemText(IDC_OLYIMPEDANCE_Z_CH4,pcBuffer);
	sprintf(pcBuffer,"%d",m_jMeters.CS_FAULT_CH4);
	SetDlgItemText(IDC_OLYIMPEDANCE_FAULT_CH4,pcBuffer);

}


void COlyImpedanceDlg::LogResultsToFile(tOLYStatusMeters* jMeters)
{
	// TODO: Add your control notification handler code here
	CString strLogRecord;
	struct tm *newtime;
	CFile ZLogfile;
	char pcLogRecord[200] = {0};
	char pcLogRecordWithTime[300];
	static char pcLogRecordOld[200] = {0};
	char pcBuffer[200];
	time_t tNow;



	bool bEnable = true;
	CButton* pButton = (CButton*) GetDlgItem(IDC_OLYIMPEDANCE_LOGENABLE);

	if (pButton)
	{
		bEnable = (pButton->GetCheck() != 0);
	}
	if (!bEnable) return;

	float fTemp0 = 0;
	//fTemp0 = ((signed char)(jMeters->uiTemperature & 0x0ff))/2.0;

	float fTemp1 = 0;
	//fTemp1 = ((signed char)((jMeters->uiTemperature >> 8) & 0x0ff))/2.0;


	if (ZLogfile.Open(m_pcLogFileName,CFile::modeWrite | CFile::modeNoTruncate | CFile::modeCreate)) //NoTruncate))
	{
		int pos = ZLogfile.SeekToEnd();
		if (pos == 0)
		{
			sprintf(pcBuffer,"time,temp1,temp2,fault, CH1_V,CH1_I,CH1_Z, CH1_FAULT,\t  CH2_V,CH2_I,CH2_Z, CH2_FAULT,  \t  CH3_V,CH3_I,CH3_Z, CH3_FAULT,\t  CH4_V,CH4_I,CH4_Z, CH4_FAULT \r\n"); 
			ZLogfile.Write(pcBuffer, strlen(pcBuffer));
		}
		time(&tNow);
		newtime = localtime(&tNow);
		strcpy(pcLogRecordWithTime, asctime(newtime));
		sprintf(pcLogRecord,", %5.3f, %5.3f,%8.8x,    %5.3f, %5.3f, %5.3f, %8.8x,      %5.3f, %5.3f, %5.3f, %8.8x,     %5.3f, %5.3f, %5.3f, %8.8x,     %5.3f, %5.3f, %5.3f, %8.8x\r\n",
			jMeters->AMP1_TEMP,	
			jMeters->AMP2_TEMP,		
			jMeters->AMP_FAULT,		
			jMeters->CS_V_CH1,			
			jMeters->CS_I_CH1,			
			jMeters->CS_Z_CH1,			
			jMeters->CS_FAULT_CH1,      
			jMeters->CS_V_CH2,			
			jMeters->CS_I_CH2,			
			jMeters->CS_Z_CH2,			
			jMeters->CS_FAULT_CH2,      
			jMeters->CS_V_CH3,			
			jMeters->CS_I_CH3,			
			jMeters->CS_Z_CH3,			
			jMeters->CS_FAULT_CH3,
			jMeters->CS_V_CH4,			
			jMeters->CS_I_CH4,			
			jMeters->CS_Z_CH4,			
			jMeters->CS_FAULT_CH4);   

		if (strcmp(pcLogRecord,pcLogRecordOld) != 0)
		{
			strcat(pcLogRecordWithTime, pcLogRecord);

			ZLogfile.Write(pcLogRecordWithTime,strlen(pcLogRecordWithTime));

			strcpy(pcLogRecordOld,pcLogRecord);
		}
		else
		{
			fTemp0 += 0;

		}
		ZLogfile.Close();
	}



	return;
}	

void COlyImpedanceDlg::OnBnClickedOlyimpedanceStart()
{
	// TODO: Add your control notification handler code here
	m_bInProgress = true;
	if (m_dlgParent)
	{
		m_dlgParent->SetTimer(OLYIMPEDANCE_TIMERID,500,NULL);
	}
}

void COlyImpedanceDlg::OnBnClickedOlyimpedanceStop()
{
	// TODO: Add your control notification handler code here
	m_bInProgress = false;
	if (m_dlgParent)
	{
		m_dlgParent->KillTimer(ZIMPEDANCE_TIMERID);
	}
}

void COlyImpedanceDlg::OnBnClickedOlyimpedanceFileselect()
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
			SetDlgItemText(IDC_OLYIMPEDANCE_FILENAME, m_strLogFileName);
			//m_strLogFileName = ;
		}

}


void COlyImpedanceDlg::OnBnClickedOlyimpedanceLogclear()
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
