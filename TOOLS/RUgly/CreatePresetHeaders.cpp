// CreatePresetHeaders.cpp : implementation file
//

#include "stdafx.h"
#include "LOUD_types.h"
#include "LOUD_defines.h"
extern "C" 
{
#include "mandolin.h"
}
#include "Product_arch.h"
#include "RUglyDlg.h"
#include "CreatePresetHeaders.h"

#include <stdlib.h>
#include "resource.h"
#include <math.h>

extern "C" 
{
#include "mandolin.h"
#include "ProductMap.h"
}
// CCreatePresetHeaders dialog

IMPLEMENT_DYNAMIC(CCreatePresetHeaders, CDialog)

CCreatePresetHeaders::CCreatePresetHeaders(CWnd* pParent /*=NULL*/)
	: CDialog(CCreatePresetHeaders::IDD, pParent)
{
	m_dlgParent = NULL;
	m_nProductType = ePRODUCT_OLYSPEAKER;

	m_uiBrand = LOUD_BRAND_MACKIE;
	m_uiModel = MACKIE_MODEL_R8_AA;

	char ModuleName[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	char szFullPath[256];
	char strTempName[_MAX_PATH];

	char pcPrsetfileName[50] = "PresetHeader.h";


	::GetModuleFileName(AfxGetApp()->m_hInstance, szFullPath, sizeof(szFullPath));
	_splitpath(szFullPath,drive,dir,fname,ext);

	strcpy(strTempName,drive);
		dir[strlen(dir)-1]=0;	// take out the the the last 

	strcat(strTempName,dir);

	strcpy(m_pcFolderPath,strTempName);

	//strcat(strTempName,pcPrsetfileName);

	strcpy(m_pcPresetHeaderFileName,pcPrsetfileName);

}

CCreatePresetHeaders::~CCreatePresetHeaders()
{
}

void CCreatePresetHeaders::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCreatePresetHeaders, CDialog)
	ON_BN_CLICKED(IDC_CREATEPRESETHEADERS_CREATE, &CCreatePresetHeaders::OnBnClickedCreatepresetheadersCreate)
	ON_BN_CLICKED(IDC_CREATEPRESETHEADERS_GETHWSWINFO, &CCreatePresetHeaders::OnBnClickedCreatepresetheadersGethwswinfo)
	ON_BN_CLICKED(IDC_CREATEPRESETHEADERS_SELECT, &CCreatePresetHeaders::OnBnClickedCreatepresetheadersSelect)
	ON_CBN_SELCHANGE(IDC_CREATEPRESETHEADERS_TARGET, &CCreatePresetHeaders::OnCbnSelchangeCreatepresetheadersTarget)
	ON_BN_CLICKED(IDC_CREATEPRESETHEADERS_GETDEVICESTATE, &CCreatePresetHeaders::OnBnClickedCreatepresetheadersGetdevicestate)
	ON_BN_CLICKED(IDC_CREATEPRESETHEADERS_GETDEVICEFIRSTATE, &CCreatePresetHeaders::OnBnClickedCreatepresetheadersGetdevicefirstate)
END_MESSAGE_MAP()


// CCreatePresetHeaders message handlers

BOOL CCreatePresetHeaders::OnInitDialog()
{
	bool bStatus;
	bStatus = CDialog::OnInitDialog();

	//SetDlgItemText(IDC_OLYIMPEDANCE_FILENAME, m_strLogFileName);
	SetProductType(m_nProductType);
	SetBrandModel(LOUD_BRAND_MACKIE,MACKIE_MODEL_R8_AA,"Mackie","OLY8_AA");

	SetDlgItemText(IDC_CREATEPRESETHEADERS_FILENAME,m_pcPresetHeaderFileName);
	SetDlgItemText(IDC_CREATEPRESETHEADERS_DIRECTORYNAME,m_pcFolderPath);

	SetDlgItemText(IDC_CREATEPRESETHEADERS_SAVEINDEX,"1");
	SetDlgItemText(IDC_CREATEPRESETHEADERS_INDEX,"0");

	SetDlgItemText(IDC_CREATEPRESETHEADERS_STATUS,"");

	UpdatePresetFileName();


	return bStatus;
}
void CCreatePresetHeaders::SetProductType(int nProductType)
{
	char pcTargetName[30];
	int  pnTargetList[10];
	CComboBox* pCombo;
	int nNum;
	int i;
	int nIndex;


	m_nProductType = nProductType;
	switch(nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		nNum = m_dlgParent->m_ProductOLY.GetPresetHeaderList(pnTargetList,sizeof(pnTargetList)/sizeof(int));
		//SetEQSelectList(m_dlgParent->m_ProductOLY.GetEQSelectList(),m_dlgParent->m_ProductOLY.GetEQSelectListSize());
		break;
	}
	pCombo = (CComboBox*) GetDlgItem(IDC_CREATEPRESETHEADERS_TARGET);
	if (pCombo)
	{
		pCombo->ResetContent();


		for(i=0;i<nNum;i++)
		{
			switch(pnTargetList[i])
			{
			case eTARGET_DEVICE:	strcpy(pcTargetName,"DEVICE"); break;	
			case eTARGET_USER:		strcpy(pcTargetName,"USER"); break;				
			case eTARGET_PROFILE:	strcpy(pcTargetName,"PROFILE"); break;				
			case eTARGET_THROW:		strcpy(pcTargetName,"THROW"); break;				
			case eTARGET_VOICING:	strcpy(pcTargetName,"VOICING"); break;				
			case eTARGET_FIR:		strcpy(pcTargetName,"FIR"); break;		
			case eTARGET_XOVER:		strcpy(pcTargetName,"XOVER"); break;
			case eTARGET_AIRLOSS:	strcpy(pcTargetName,"AIRLOSS"); break;
			case eTARGET_NOISEGATE:	strcpy(pcTargetName,"NOISEGATE"); break;
			default:				strcpy(pcTargetName,"UNKNOWN"); break;
			}

			nIndex =  pCombo->AddString(pcTargetName);
			pCombo->SetItemDataPtr(nIndex, (void*) pnTargetList[i]);
		}
		pCombo->SetCurSel(0);
	}

	SetDlgItemText(IDC_CREATEPRESETHEADERS_STATUS,"");
}

void CCreatePresetHeaders::SetBrandModel(uint32 uiBrand, uint32 uiModel,char* pcBrand, char* pcModel)
{
	m_uiBrand = uiBrand;
	m_uiModel = uiModel;

	SetDlgItemText(IDC_CREATEPRESETHEADERS_BRAND,pcBrand);
	SetDlgItemText(IDC_CREATEPRESETHEADERS_MODEL,pcModel);

	UpdatePresetFileName();

}


void CCreatePresetHeaders::OnBnClickedCreatepresetheadersCreate()
{
	// TODO: Add your control notification handler code here
	int nSaveIndex;
	bool bAll = false;
	int nTarget;
	int nInstance;
	intfloat j;
	int nType;
	char pcTarget[200];
	int i;

	int nMartinTargetList[]  = 
	{
		//eTARGET_DEVICE,
		eTARGET_USER,				
		eTARGET_PROFILE,					
		eTARGET_THROW,					
		eTARGET_VOICING,					
		eTARGET_FIR,		
		eTARGET_XOVER,	
		eTARGET_AIRLOSS,	
		eTARGET_NOISEGATE,	
	};
	
	int nNum = sizeof(nMartinTargetList)/sizeof(int);


	if (!m_dlgParent) return;



	SetDlgItemText(IDC_CREATEPRESETHEADERS_STATUS,"Trying to Create...");

	nInstance = GetGetIndex();


	nTarget = GetTarget(pcTarget);
	nSaveIndex = GetSaveIndex();
	bAll = GetGenerateAllFlag();
	
	bool bStatus = true;
	if (!bAll)
	{
		bStatus = CreatePresetFile(nInstance, nSaveIndex, nTarget);
	}
	else
	{
		for (i = 0; i<nNum; i++)
		{
			bStatus &= CreatePresetFile(0, nSaveIndex, nMartinTargetList[i]);
		}

	}
	if (bStatus)

		SetDlgItemText(IDC_CREATEPRESETHEADERS_STATUS,"Success");
	else
		SetDlgItemText(IDC_CREATEPRESETHEADERS_STATUS,"Error with File");



}
bool CCreatePresetHeaders::CreatePresetFile(int nGetIndex, int nSaveIndex, int nTarget)
{
	// TODO: Add your control notification handler code here

	int nInstance;
	intfloat j;
	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;
	char pcBuffer[200];
	int i;
	intfloat jVal;
	void** pParameterAddress;
	char pcTarget[200] = {0};
	char pcTime[200] = {0};
	char pcBrand[200];
	char pcModel[200];
	char pcString[100];
	char pcStatus[500];

	char pcPresetFullFileName[400];

	CFile ZLogfile;

	if (!m_dlgParent) return false;


	time_t tNow;
	struct tm *newtime;

	time(&tNow);
	newtime = localtime(&tNow);
	strcpy(pcTime, asctime(newtime));

	GenerateFileName(m_pcPresetHeaderFileName,nTarget,nSaveIndex);
	sprintf(pcPresetFullFileName,"%s\\%s",m_pcFolderPath,m_pcPresetHeaderFileName);
	SetDlgItemText(IDC_CREATEPRESETHEADERS_FILENAME,m_pcPresetHeaderFileName);

	sprintf(pcStatus,"Trying to Create...%s",m_pcPresetHeaderFileName);
	SetDlgItemText(IDC_CREATEPRESETHEADERS_STATUS,pcStatus);


	int nMax = 1;
	switch(m_nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		nMax = m_dlgParent->m_ProductOLY.GetTargetInstances(m_uiBrand,m_uiModel,nTarget);
		//SetEQSelectList(m_dlgParent->m_ProductOLY.GetEQSelectList(),m_dlgParent->m_ProductOLY.GetEQSelectListSize());
		break;
	}

	// instances must be 0.. nMax-1
	if ((nGetIndex >= nMax) || (nGetIndex < 0))
	{
		MessageBox("Instances not in Range", "ERROR");
		SetDlgItemText(IDC_CREATEPRESETHEADERS_STATUS,"Instance out Range");

		return false;
	}

	sprintf(pcBuffer,"0..%d",nMax);
	SetDlgItemText(IDC_CREATEPRESETHEADERS_INDEX_RANGE,pcBuffer);

	ConvertTarget2Name(nTarget, pcTarget);


	if (ZLogfile.Open(pcPresetFullFileName,CFile::modeWrite | CFile::modeCreate)) //NoTruncate))
	{
		// put header in

		// get preset and loop thru the parameters
		nTableLength = 0;
		nXMLVersion;
		pParameterDesc = 0;
			m_dlgParent->GetParameterDescTable(m_nProductType, nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);
			
			sprintf(pcBuffer,"// Created %s\r\n",pcTime);
			ZLogfile.Write(pcBuffer,strlen(pcBuffer));

			sprintf(pcBuffer,"// %s %s\r\n", 	m_dlgParent->m_strProductName, m_dlgParent->m_strProductVersion);
			ZLogfile.Write(pcBuffer,strlen(pcBuffer));

			GetDlgItemText(IDC_CREATEPRESETHEADERS_BRAND,pcBrand, sizeof(pcBrand));
			GetDlgItemText(IDC_CREATEPRESETHEADERS_MODEL,pcModel, sizeof(pcModel));
			sprintf(pcBuffer,"// Device: %s %s\r\n", pcBrand,pcModel);
			ZLogfile.Write(pcBuffer,strlen(pcBuffer));


			sprintf(pcBuffer,"// Device Version: %s\r\n", 	m_dlgParent->m_pcFirmwareVersion);
			ZLogfile.Write(pcBuffer,strlen(pcBuffer));
	

			sprintf(pcBuffer,"// XML Version: %d\r\n", 	nXMLVersion);
			ZLogfile.Write(pcBuffer,strlen(pcBuffer));
	
			sprintf(pcBuffer,"\r\n");
			ZLogfile.Write(pcBuffer,strlen(pcBuffer));
			
			sprintf(pcBuffer,"#include \"OLYspeaker1_map.h\"\r\n");
			ZLogfile.Write(pcBuffer,strlen(pcBuffer));
	
			sprintf(pcBuffer,"\r\n");
			ZLogfile.Write(pcBuffer,strlen(pcBuffer));

			
			sprintf(pcBuffer,"#if (OLYspeaker1_XML_VERSION != %d)\r\n", 	nXMLVersion);
			ZLogfile.Write(pcBuffer,strlen(pcBuffer));
			sprintf(pcBuffer,"#error xml version out of date\n");
			ZLogfile.Write(pcBuffer,strlen(pcBuffer));
			sprintf(pcBuffer,"#else\n");
			ZLogfile.Write(pcBuffer,strlen(pcBuffer));
	
			sprintf(pcBuffer,"\nconst uint32_t %s_Voicing%d[]=\r\n",pcTarget,nSaveIndex);
			ZLogfile.Write(pcBuffer,strlen(pcBuffer));
			sprintf(pcBuffer,"{\r\n");
			ZLogfile.Write(pcBuffer,strlen(pcBuffer));


			//int nNumInstances = GetTargetInstances(m_uiBrand,m_uiModel, nTargetList[j].nTarget);

			//for (int k=1;k<=nNumInstances;k++)
			//{

				for(i=0;i<nTableLength;i++)
				{
					pParameterAddress = m_dlgParent->GetParamAddress(nTarget,i,nGetIndex+1);
					jVal.i = 0;
					if (pParameterAddress)
					{
						jVal.vp = *pParameterAddress;
					}

					sprintf(pcBuffer,"\t 0x%8.8X , // %d\t",jVal.ui, i);
					ZLogfile.Write(pcBuffer,strlen(pcBuffer));

					m_dlgParent->m_MandolinTextSummary.ConvertParameter2Text(&pParameterDesc[i],jVal,pcBuffer,false,0); //nGetIndex+1);
					ZLogfile.Write(pcBuffer,strlen(pcBuffer));


				}
			//}


			sprintf(pcBuffer,"};\r\n");
			ZLogfile.Write(pcBuffer,strlen(pcBuffer));

			sprintf(pcBuffer,"#endif\r\n");
			ZLogfile.Write(pcBuffer,strlen(pcBuffer));
	

			ZLogfile.Close();

			SetDlgItemText(IDC_CREATEPRESETHEADERS_STATUS,"Success, File Created");

			return true;

	}
	else
	{
		SetDlgItemText(IDC_CREATEPRESETHEADERS_STATUS,"Error with File");
	}
	return false;
}
void CCreatePresetHeaders::OnBnClickedCreatepresetheadersGethwswinfo()
{
	// TODO: Add your control notification handler code here
	m_dlgParent->OnBnClickedGetHwInfo();
	m_dlgParent->OnBnClickedGetSwInfo();
}

void CCreatePresetHeaders::OnBnClickedCreatepresetheadersSelect()
{
#if(0)
	// TODO: Add your control notification handler code here
		char szSelectFileTitle[_MAX_PATH];
		char szSelectFileFilter[256];
		BOOL bStatus;
		char szInputFileName[_MAX_PATH];

		strcpy(szInputFileName, m_pcPresetHeaderFileName);

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
			"Text files (*.h)",0,"*.h",0,
			"All Files",0,"*.*",0,0);

		if (NULL != strstr(szInputFileName,".txt")) {
		 		ofnSelectFileOpen.nFilterIndex = 1;
		}

		ofnSelectFileOpen.lStructSize = sizeof(ofnSelectFileOpen);
		ofnSelectFileOpen.hwndOwner = this->m_hWnd;
		bStatus = GetOpenFileName(&ofnSelectFileOpen);


  		if (bStatus)
		{
			strcpy(m_pcPresetHeaderFileName,ofnSelectFileOpen.lpstrFile); //strcpy(m_pcLogFileName, ofnSelectFileOpen.lpstrFile);
			SetDlgItemText(IDC_CREATEPRESETHEADERS_FILENAME, m_pcPresetHeaderFileName);
			//m_strLogFileName = ;
		}
#else

	BROWSEINFO bi = { 0 };
	bi.lpszTitle = _T("Pick a Preset Directory");
	LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );
	if ( pidl != 0 )
	{
		// get the name of the folder
		TCHAR path[MAX_PATH];
		if ( SHGetPathFromIDList ( pidl, path ) )
		{
			strcpy(m_pcFolderPath,path);
			//SetDlgItemText(IDC_OUTPUTDIR,path);
		}

		SetDlgItemText(IDC_CREATEPRESETHEADERS_DIRECTORYNAME,path);

		// free memory used
		IMalloc * imalloc = 0;
		if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
		{
			imalloc->Free ( pidl );
			imalloc->Release ( );
		}
	}


//	//m_strFolderPath = _T( "d:\\Windows" ); // Just for sample    
//    m_strDisplayName.Empty();
//    
//    CFolderDialog dlg(  _T( "Select Preset Directory" ), m_strFolderPath, this );
//    if( dlg.DoModal() == IDOK  )
//    {    
//        m_strFolderPath  = dlg.GetFolderPath();
//        m_strDisplayName = dlg.GetFolderDisplayName();
//        // Use folder path and display name here ...
//    }  
#endif

}
void CCreatePresetHeaders::OnCbnSelchangeCreatepresetheadersTarget()
{
	// TODO: Add your control notification handler code here

// TODO: Add your control notification handler code here
	int nType;
	CComboBox* pCombo;
	int nSel;

	int nTarget;
	int nInstance;
	intfloat j;
	char pcTarget[200];
	char pcBuffer[200];
	int i;
	if (!m_dlgParent) return;



	nTarget = GetTarget(pcTarget);

	int nMax = 1;
	switch(m_nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		nMax = m_dlgParent->m_ProductOLY.GetTargetInstances(m_uiBrand,m_uiModel,nTarget);
		//SetEQSelectList(m_dlgParent->m_ProductOLY.GetEQSelectList(),m_dlgParent->m_ProductOLY.GetEQSelectListSize());
		break;
	}
	sprintf(pcBuffer,"0..%d",nMax);
	SetDlgItemText(IDC_CREATEPRESETHEADERS_INDEX_RANGE,pcBuffer);
	SetDlgItemText(IDC_CREATEPRESETHEADERS_STATUS,"");


	int nSaveIndex = GetSaveIndex();
	GenerateFileName(m_pcPresetHeaderFileName,nTarget,nSaveIndex);
	SetDlgItemText(IDC_CREATEPRESETHEADERS_FILENAME, m_pcPresetHeaderFileName);

}
void CCreatePresetHeaders::GenerateFileName(char* pcFileName, int nTarget, int nSaveIndex)
{
	char pcLocalName[400];
	char pcTargetName[100];
	char pcModelName[100];
	int nLength;

	GetDlgItemText(IDC_CREATEPRESETHEADERS_MODEL,pcModelName,100);
	nLength = strlen(pcModelName);
	if (nLength > 0)
	{
		if (isspace(pcModelName[nLength-1])) 
			pcModelName[nLength-1] = 0;
	}


	switch(nTarget)
	{
		case eTARGET_DEVICE:	strcpy(pcTargetName,"DEVICE"); break;	
		case eTARGET_USER:		strcpy(pcTargetName,"USER"); break;				
		case eTARGET_PROFILE:	strcpy(pcTargetName,"PROFILE"); break;				
		case eTARGET_THROW:		strcpy(pcTargetName,"THROW"); break;				
		case eTARGET_VOICING:	strcpy(pcTargetName,"VOICING"); break;				
		case eTARGET_FIR:		strcpy(pcTargetName,"FIR"); break;		
		case eTARGET_XOVER:		strcpy(pcTargetName,"XOVER"); break;
		case eTARGET_AIRLOSS:	strcpy(pcTargetName,"AIRLOSS"); break;
		case eTARGET_NOISEGATE:	strcpy(pcTargetName,"NOISEGATE"); break;
		default:				strcpy(pcTargetName,"UNKNOWN"); break;
	}

	sprintf(pcLocalName,"%s_%s_data_factory_preset%d.h",pcModelName,pcTargetName,nSaveIndex);

	strcpy(pcFileName,pcLocalName);

}


int CCreatePresetHeaders::GetSaveIndex()
{
	intfloat jVal;
	int nType;
	char pcBuffer[100];

	// get the Save index
	GetDlgItemText(IDC_CREATEPRESETHEADERS_SAVEINDEX,pcBuffer,sizeof(pcBuffer));
	jVal = m_dlgParent->ConvertString2Value(pcBuffer,nType);
	if (nType != 0)
	{
		jVal.i = jVal.f;
	}
	return jVal.i;
}
int CCreatePresetHeaders::GetGetIndex()
{
	intfloat jVal;
	int nType;
	char pcBuffer[100];

	// get the Save index
	GetDlgItemText(IDC_CREATEPRESETHEADERS_INDEX,pcBuffer,sizeof(pcBuffer));
	jVal = m_dlgParent->ConvertString2Value(pcBuffer,nType);
	if (nType != 0)
	{
		jVal.i = jVal.f;
	}
	return jVal.i;
}


int CCreatePresetHeaders::GetTarget(char* pcTarget)
{
	int nTarget;
	CComboBox* pCombo;
	int nSel;


	nTarget = 0;
	pCombo = (CComboBox*) GetDlgItem(IDC_CREATEPRESETHEADERS_TARGET);
	if (pCombo)
	{
		nSel = pCombo->GetCurSel();
		pCombo->GetLBText(nSel,pcTarget);
		nTarget = pCombo->GetItemData(nSel);
	}

	return nTarget;
}

void CCreatePresetHeaders::ConvertTarget2Name(int nTarget, char* pcTarget)
{

	switch(nTarget)
	{
		case eTARGET_DEVICE:	strcpy(pcTarget,"DEVICE"); break;	
		case eTARGET_USER:		strcpy(pcTarget,"USER"); break;				
		case eTARGET_PROFILE:	strcpy(pcTarget,"PROFILE"); break;				
		case eTARGET_THROW:		strcpy(pcTarget,"THROW"); break;				
		case eTARGET_VOICING:	strcpy(pcTarget,"VOICING"); break;				
		case eTARGET_FIR:		strcpy(pcTarget,"FIR"); break;		
		case eTARGET_XOVER:		strcpy(pcTarget,"XOVER"); break;
		case eTARGET_AIRLOSS:	strcpy(pcTarget,"AIRLOSS"); break;
		case eTARGET_NOISEGATE:	strcpy(pcTarget,"NOISEGATE"); break;
		default:				strcpy(pcTarget,"UNKNOWN"); break;
	}
}
bool CCreatePresetHeaders::GetGenerateAllFlag()
{
	CButton* pButton = (CButton*) GetDlgItem(IDC_CREATEPRESETHEADERS_TARGETALL);
	if (!pButton) return false;

	return (pButton->GetCheck()==1);
}

void  CCreatePresetHeaders::UpdatePresetFileName()
{
	int nTarget;
	char pcTarget[200];
	int nSaveIndex;

	nTarget = GetTarget(pcTarget);
	nSaveIndex = GetSaveIndex();

	GenerateFileName(m_pcPresetHeaderFileName,nTarget,nSaveIndex);
	SetDlgItemText(IDC_CREATEPRESETHEADERS_FILENAME,m_pcPresetHeaderFileName);
}

void CCreatePresetHeaders::OnBnClickedCreatepresetheadersGetdevicestate()
{
	// TODO: Add your control notification handler code here
	 m_dlgParent->OnBnClickedGetstatefromdevice();
}

void CCreatePresetHeaders::OnBnClickedCreatepresetheadersGetdevicefirstate()
{
	// TODO: Add your control notification handler code here

	 m_dlgParent->OnBnClickedGetFIRstatefromdevice();
}
