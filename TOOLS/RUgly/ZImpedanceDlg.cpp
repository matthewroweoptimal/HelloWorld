// ZImpedanceDlg.cpp : implementation file
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
#include "ZImpedanceDlg.h"
#include <stdlib.h>
#include "resource.h"
#include <math.h>

#include "ProductMap.h"

#define METER_DISPLAY_MIN -40
#define METER_DISPLAY_MAX 12
#define COMPRED_DISPLAY_MAX 20
#define COMPRED_DISPLAY_MIN 0

#define	Z_DISPLAY_MIN 0
//#define	Z_DISPLAY_MAX 30

void	DrawCompRed	(	int nIDC, float	fGainReduction	);
void	DrawMeter	(	int nIDC, float	fLevel	);
void DrawZImpedance (	int nIDC, float	fLevel, float fOpenThresh, float fShortThresh, float fZMax );


// CZImpedanceDlg dialog


CZImpedanceDlg*	pZWindow;
IMPLEMENT_DYNAMIC(CZImpedanceDlg, CDialog)

CZImpedanceDlg::CZImpedanceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CZImpedanceDlg::IDD, pParent)
{
	m_dlgParent = NULL;
	m_nProductType = ePRODUCT_OLYSPEAKER;
	m_bInProgress = false;

	m_uiSequence = 1;

	char pcLogfileName[50] = "RZLogFile.txt";
		
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

CZImpedanceDlg::~CZImpedanceDlg()
{
}

void CZImpedanceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ZIMPEDANCE_MESSAGERX, m_OutputTextMeters);

}


BEGIN_MESSAGE_MAP(CZImpedanceDlg, CDialog)
	ON_BN_CLICKED(IDC_ZIMPEDANCE_START, &CZImpedanceDlg::OnBnClickedZimpedanceStart)
	ON_BN_CLICKED(IDC_ZIMPEDANCE_STOP, &CZImpedanceDlg::OnBnClickedZimpedanceStop)
	ON_BN_CLICKED(IDC_ZIMPEDANCE_LOGFILESELECT, &CZImpedanceDlg::OnBnClickedZimpedanceLogfileselect)
	ON_BN_CLICKED(IDC_ZIMPEDANCE_LOGFILESCLEAR, &CZImpedanceDlg::OnBnClickedZimpedanceLogfilesclear)
	ON_BN_CLICKED(IDC_ZIMPEDANCE_CLEAR, &CZImpedanceDlg::OnBnClickedZimpedanceClear)
END_MESSAGE_MAP()


BOOL CZImpedanceDlg::OnInitDialog()
{
	bool bStatus;
	bStatus = CDialog::OnInitDialog();

	m_OutputTextMeters.SetScrollRange(SB_VERT,0,100,true);

	CHARFORMAT CharFormat;
	CharFormat.cbSize = sizeof(CharFormat);
    CharFormat.dwMask = CFM_COLOR | CFM_SIZE | CFM_CHARSET;
   //CharFormat.crTextColor;
    CharFormat.dwEffects = CFE_AUTOCOLOR;
    CharFormat.yHeight = 170;
    ::lstrcpy(CharFormat.szFaceName, "Arial");
	m_OutputTextMeters.SetDefaultCharFormat(CharFormat);


	SetDlgItemText(IDC_ZIMPEDANCE_LOGFILENAME, m_strLogFileName);

	pZWindow = this;


	return bStatus;
}

void CZImpedanceDlg::OnTimer(UINT nIDEvent)
{

	switch(nIDEvent)
	{
	case ZIMPEDANCE_TIMERID:
		m_mGetMeters.payload = m_pcGetMeterPayload;
		if (m_dlgParent)
		{
			m_dlgParent->m_MandolinComm.CreateParameterEditGetNeighborhood(&m_mGetMeters, m_uiSequence,m_dlgParent->GetMetersTarget(m_nProductType),1,0x0ff);
			m_dlgParent->TryToSendMandolinMessage(&m_mGetMeters, true);
			m_dlgParent->SetTimer(ZIMPEDANCE_TIMERID,500,NULL);
		}
		m_uiSequence++;
		if (m_uiSequence==0) m_uiSequence++;
		break;
	}
}

// CZImpedanceDlg message handlers
#define MAX_ZIMPEDANCE_METER (11+9)




void CZImpedanceDlg::OnMeterDecode(mandolin_message* pInMsg)
{
	
	uint32  flags;
    uint32 *in_ptr = (uint32 *) pInMsg->payload;
    
	tREDLINEMeters jMeters ={0};
	uint32* puiValue;;

    int nParameterNumber;
	int k;
	int nTarget;
	int nDirection;
	int nNumOfParameters;
		
	bool bMultiParameter = false;
   	bool bIsReply;
	int nMaxZImpedanceMeter = sizeof(tREDLINEMeters)/4 +1;
	
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

	if (nTarget != m_dlgParent->GetMetersTarget(m_nProductType))
	{
		return;
	}

	if (m_nProductType != ePRODUCT_REDLINESPEAKER1)
	{
		return;
	}

	puiValue = (uint32*) (void*)(&jMeters.INPUT_IN);
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

	sprintf(pcBuffer,"%6.4f",jMeters.phCurrentSense0);
	SetDlgItemText(IDC_ZIMPEDANCE_CURRENTCH1,pcBuffer);
	sprintf(pcBuffer,"%6.4f",jMeters.phCurrentSense1);
	SetDlgItemText(IDC_ZIMPEDANCE_CURRENTCH2,pcBuffer);
	sprintf(pcBuffer,"%6.4f",jMeters.fVPeak0);
	SetDlgItemText(IDC_ZIMPEDANCE_VCH1,pcBuffer);
	sprintf(pcBuffer,"%6.4f",jMeters.fVPeak1);
	SetDlgItemText(IDC_ZIMPEDANCE_VCH2,pcBuffer);

	sprintf(pcBuffer,"%6.4f",jMeters.fImpedance0);
	SetDlgItemText(IDC_ZIMPEDANCE_ZCH1,pcBuffer);
	sprintf(pcBuffer,"%6.4f",jMeters.fImpedance1);
	SetDlgItemText(IDC_ZIMPEDANCE_ZCH2,pcBuffer);


	sprintf(pcBuffer,"%6.4f",jMeters.fImpedanceAverage0);
	SetDlgItemText(IDC_ZIMPEDANCE_ZAVERAGECH1,pcBuffer);
	sprintf(pcBuffer,"%6.4f",jMeters.fImpedanceAverage1);
	SetDlgItemText(IDC_ZIMPEDANCE_ZAVERAGECH2,pcBuffer);



	sprintf(pcBuffer,"%8.8x",jMeters.ampFault);
	SetDlgItemText(IDC_ZIMPEDANCE_AMPSTATUS,pcBuffer);

#define AMP_FAULT_PROTECT1		0x01		//
#define AMP_FAULT_PROTECT2		0x02
#define AMP_FAULT_TEMP1			0x04		//
#define AMP_FAULT_TEMP2			0x08
#define AMP_FAULT_VAC			0x10		// 
#define AMP_FAULT_AMPSHORT_1	0x20
#define AMP_FAULT_AMPSHORT_2	0x04
#define AMP_FAULT_AMPOPEN_1		0x080
#define AMP_FAULT_AMPOPEN_2		0x100

	pcBuffer[0] = 0;
	if (jMeters.ampFault & AMP_FAULT_PROTECT1		) strcat(pcBuffer,"AMP_FAULT_PROTECT1 ");
	if (jMeters.ampFault & AMP_FAULT_PROTECT2		) strcat(pcBuffer,"AMP_FAULT_PROTECT2 ");
	if (jMeters.ampFault & AMP_FAULT_TEMP1			) strcat(pcBuffer,"AMP_FAULT_TEMP1 ");
	if (jMeters.ampFault & AMP_FAULT_TEMP2			) strcat(pcBuffer,"AMP_FAULT_TEMP2 ");
	if (jMeters.ampFault & AMP_FAULT_VAC			) strcat(pcBuffer,"AMP_FAULT_VAC ");
	if (jMeters.ampFault & AMP_FAULT_AMPSHORT_1		) strcat(pcBuffer,"AMP_FAULT_AMPSHORT_1 ");
	if (jMeters.ampFault & AMP_FAULT_AMPSHORT_2		) strcat(pcBuffer,"AMP_FAULT_AMPSHORT_2 ");
	if (jMeters.ampFault & AMP_FAULT_AMPOPEN_1		) strcat(pcBuffer,"AMP_FAULT_AMPOPEN_1 ");
	if (jMeters.ampFault & AMP_FAULT_AMPOPEN_2		) strcat(pcBuffer,"AMP_FAULT_AMPOPEN_2 ");

	SetDlgItemText(IDC_ZIMPEDANCE_AMPSTATUS2,pcBuffer);

	float fTemp;
	fTemp = ((signed short)(jMeters.uiTemperature & 0x0ffff))/2.0;
	sprintf(pcBuffer,"%5.2f",fTemp);
	SetDlgItemText(IDC_ZIMPEDANCE_TEMPERATURECH1,pcBuffer);


	fTemp = ((signed short)((jMeters.uiTemperature >> 16) & 0x0ffff))/2.0;
	sprintf(pcBuffer,"%5.2f",fTemp);
	SetDlgItemText(IDC_ZIMPEDANCE_TEMPERATURECH2,pcBuffer);


	int uiModel = ((jMeters.uiVersion >> 24)  & 0x0ff);
	int uiMajor = ((jMeters.uiVersion >> 16)  & 0x0ff);
	int uiMinor = ((jMeters.uiVersion >>  8)  & 0x0ff);
	int uiMicro = ((jMeters.uiVersion >>  0)  & 0x0ff);

	sprintf(pcBuffer,"%s: %2.2x.%2.2x.%2.2x",
		((uiModel == 1)?"RL12": (uiModel == 2)?"RL15" : (uiModel == 3)?"RL18S":"Unknown"),
		uiMajor,uiMinor, uiMicro);
	SetDlgItemText(IDC_ZIMPEDANCE_VERSION,pcBuffer);
	
#define  AMPSTAUTS_HF_MAX					42
#define  AMPSTAUTS_LF_MAX					42					

	float fHFOpenThreashold;
	float fHFShortThreashold;
	float fLFOpenThreashold;
	float fLFShortThreashold;

	switch(uiModel)
	{
	case 1: //RL12
		fHFOpenThreashold	=	16.0;	
		fHFShortThreashold	=	3.0;
		fLFOpenThreashold	=	38.0;
		fLFShortThreashold	=	1.5;
		break;
	case 2: //RL15
		fHFOpenThreashold	=	16.0;	
		fHFShortThreashold	=	3.0;
		fLFOpenThreashold	=	38.0;
		fLFShortThreashold	=	1.5;
		break;
	case 3: //RL18S
		fHFOpenThreashold	=	16.0;	
		fHFShortThreashold	=	3.0;
		fLFOpenThreashold	=	38.0;
		fLFShortThreashold	=	1.5;
		break;

	default: //unknown
		fHFOpenThreashold	=	16.0;	
		fHFShortThreashold	=	3.0;
		fLFOpenThreashold	=	38.0;
		fLFShortThreashold	=	1.5;
		break;
	}

	CString strOutput;
	strOutput.Format("Z < %5.1f Short", fLFShortThreashold);
	SetDlgItemText(IDC_ZIMPEDANCE_LFSHORTTHRESH,strOutput);
	strOutput.Format("Z > %5.1f Open", fLFOpenThreashold);
	SetDlgItemText(IDC_ZIMPEDANCE_LFOPENTHRESH,strOutput);
	strOutput.Format("Z < %5.1f Short", fHFShortThreashold);
	SetDlgItemText(IDC_ZIMPEDANCE_HFSHORTTHRESH,strOutput);
	strOutput.Format("Z > %5.1f Open", fHFOpenThreashold);
	SetDlgItemText(IDC_ZIMPEDANCE_HFOPENTHRESH,strOutput);

	strOutput.Format("%2.2d -",AMPSTAUTS_HF_MAX);
	SetDlgItemText(IDC_ZIMPEDANCE_HFMAX, strOutput);
	strOutput.Format("- %2.2d",AMPSTAUTS_LF_MAX);
	SetDlgItemText(IDC_ZIMPEDANCE_LFMAX, strOutput);

	strOutput.Format(" Input \t %5.3f \n HF Out \t %5.3f \n HF GR \t %5.3f \n LF Out \t %5.3f \n LF GR \t %5.3f \n HF DAC \t %5.3f \n LF DAC \t %5.3f",
	jMeters.INPUT_IN,
	jMeters.HF_OUT,
//	jMeters.HF_SOFTLIMIT_IN,
	jMeters.HF_SOFTLIMIT_GAINREDUCE,
//	jMeters.HF_SOFTLIMIT_OUT,
	jMeters.MF_OUT,
//	jMeters.MF_SOFTLIMIT_IN,
	jMeters.MF_SOFTLIMIT_GAINREDUCE,
//	jMeters.MF_SOFTLIMIT_OUT,
	jMeters.HF_OUT_DAC,
	jMeters.MF_OUT_DAC);

	UpdateOutputTextMeters(strOutput,true, true);

	strOutput.Format(" HF V \t %5.3f \n HF I \t %5.3f \n HF Z \t %5.3f \t(%5.3f) \n LF V \t %5.3f \n LF I \t %5.3f \n LF Z \t %5.3f \t(%5.3f) \n",
		jMeters.fVPeak1, jMeters.phCurrentSense1, jMeters.fImpedance1, jMeters.fImpedanceAverage1,
		jMeters.fVPeak0, jMeters.phCurrentSense0, jMeters.fImpedance0, jMeters.fImpedanceAverage0);
	UpdateOutputTextMeters(strOutput,true, true);

	//jMeters.INPUT_IN = -20;
	// updated the metes
	DrawMeter	(	IDC_LEVEL_INPUT1, jMeters.INPUT_IN	);
	DrawMeter	(	IDC_LEVEL_OUTPUTHF, jMeters.HF_OUT	);
	DrawCompRed	(	IDC_LEVEL_GAINREDHF, jMeters.HF_SOFTLIMIT_GAINREDUCE	);
	DrawMeter	(	IDC_LEVEL_OUTPUTLF, jMeters.MF_OUT	);
	DrawCompRed	(	IDC_LEVEL_GAINREDLF, jMeters.MF_SOFTLIMIT_GAINREDUCE	);


	DrawZImpedance(	IDC_LEVEL_ZLF, jMeters.fImpedance0, fLFOpenThreashold, fLFShortThreashold, AMPSTAUTS_LF_MAX	);
	DrawZImpedance(	IDC_LEVEL_ZAVERAGELF, jMeters.fImpedanceAverage0, fLFOpenThreashold, fLFShortThreashold, AMPSTAUTS_LF_MAX	);
	DrawZImpedance(	IDC_LEVEL_ZHF, jMeters.fImpedance1, fHFOpenThreashold, fHFShortThreashold, AMPSTAUTS_HF_MAX	);
	DrawZImpedance(	IDC_LEVEL_ZAVERAGEHF, jMeters.fImpedanceAverage1, fHFOpenThreashold, fHFShortThreashold, AMPSTAUTS_HF_MAX );


	LogResultsToFile(&jMeters);
//	return true;
}


void CZImpedanceDlg::OnBnClickedZimpedanceStart()
{
	// TODO: Add your control notification handler code here
	m_bInProgress = true;
	if (m_dlgParent)
	{
		m_dlgParent->SetTimer(ZIMPEDANCE_TIMERID,500,NULL);
	}
}

void CZImpedanceDlg::OnBnClickedZimpedanceStop()
{
	// TODO: Add your control notification handler code here

	m_bInProgress = false;
	if (m_dlgParent)
	{
		m_dlgParent->KillTimer(ZIMPEDANCE_TIMERID);
	}
}

// ------------------------------------------------------------------------
// Display
// ------------------------------------------------------------------------




void	DrawMeter	(	int nIDC, float	fLevel	)
{

	// theLevel ..METER_DISPLAY_MAX..METER_DISPLAY_MIN
	HDC		TheDC;
	HWND	DlgItem = 0;
	HGDIOBJ	OldBrush, Brush;
	HGDIOBJ	OldPen,   Pen;
	CRect	Zoom_XY;
	int		XGreen, XYellow, XRed;


	DlgItem	=	GetDlgItem	( *pZWindow,	nIDC	);

	if (DlgItem != NULL)
	{

		TheDC		= GetDC ( DlgItem );
		GetClientRect (DlgItem, Zoom_XY);

		Pen			= CreatePen(PS_SOLID, 0, RGB(0,192,0));
		OldPen		= SelectObject( TheDC, Pen);
		Brush		= CreateSolidBrush( RGB(0,192,0));
		OldBrush	= SelectObject(TheDC, Brush);

		XGreen	= (int) floor ( (fLevel-METER_DISPLAY_MIN)/(METER_DISPLAY_MAX-METER_DISPLAY_MIN) * Zoom_XY.bottom - Zoom_XY.top + 0.5);
		XYellow	= (int) floor ( (fLevel-METER_DISPLAY_MIN)/(METER_DISPLAY_MAX-METER_DISPLAY_MIN) * Zoom_XY.bottom - Zoom_XY.top + 0.5);
		XRed	= (int) floor ( (fLevel-METER_DISPLAY_MIN)/(METER_DISPLAY_MAX-METER_DISPLAY_MIN) * Zoom_XY.bottom - Zoom_XY.top + 0.5);

		if (fLevel > -12) XGreen = (int) floor (( (-12.0-METER_DISPLAY_MIN)/(METER_DISPLAY_MAX-METER_DISPLAY_MIN) ) * Zoom_XY.bottom - Zoom_XY.top + 0.5);
		if (fLevel > 0) XYellow = (int) floor (( (0.0-METER_DISPLAY_MIN)/(METER_DISPLAY_MAX-METER_DISPLAY_MIN)) * Zoom_XY.bottom - Zoom_XY.top + 0.5);

		if		(XGreen <= Zoom_XY.top)		{	XGreen = Zoom_XY.top;		}
		else if (XGreen >= Zoom_XY.bottom)	{	XGreen = Zoom_XY.bottom;	}

		Rectangle (TheDC, Zoom_XY.left, Zoom_XY.bottom, Zoom_XY.right, Zoom_XY.bottom-XGreen);

		SelectObject (TheDC, OldBrush);
		DeleteObject (Brush);
		SelectObject (TheDC, OldPen);
		DeleteObject (Pen);

		Pen			= CreatePen(PS_SOLID, 0, RGB(255,255,0));
		OldPen		= SelectObject( TheDC, Pen);
		Brush		= CreateSolidBrush( RGB(255,255,0));
		OldBrush	= SelectObject(TheDC, Brush);

		if		(XYellow <= Zoom_XY.top)	{	XYellow = Zoom_XY.top;		}
		else if (XYellow >= Zoom_XY.bottom)	{	XYellow = Zoom_XY.bottom;	}

		Rectangle (TheDC, Zoom_XY.left, Zoom_XY.bottom-XGreen, Zoom_XY.right, Zoom_XY.bottom-XYellow);

		SelectObject (TheDC, OldBrush);
		DeleteObject (Brush);
		SelectObject (TheDC, OldPen);
		DeleteObject (Pen);

		Pen			= CreatePen(PS_SOLID, 0, RGB(255,0,0));
		OldPen		= SelectObject( TheDC, Pen);
		Brush		= CreateSolidBrush( RGB(255,0,0));
		OldBrush	= SelectObject(TheDC, Brush);

		if		(XRed <= Zoom_XY.top)		{	XRed = Zoom_XY.top;			}
		else if (XRed >= Zoom_XY.bottom)	{	XRed = Zoom_XY.bottom;		}

		Rectangle (TheDC, Zoom_XY.left, Zoom_XY.bottom-XYellow, Zoom_XY.right, Zoom_XY.bottom-XRed);

		SelectObject (TheDC, OldBrush);
		DeleteObject (Brush);
		SelectObject (TheDC, OldPen);
		DeleteObject (Pen);

		Pen			= CreatePen(PS_SOLID, 0, RGB(0,0,0));
		OldPen		= SelectObject( TheDC, Pen);
		Brush		= CreateSolidBrush( RGB(0,0,0));
		OldBrush	= SelectObject(TheDC, Brush);

		Rectangle (TheDC, Zoom_XY.left, Zoom_XY.bottom-XRed, Zoom_XY.right, Zoom_XY.top);

		SelectObject (TheDC, OldBrush);
		DeleteObject (Brush);
		SelectObject (TheDC, OldPen);
		DeleteObject (Pen);

		ReleaseDC ( *pZWindow, TheDC );
	}
}

void	DrawCompRed	(	int nIDC, float	fGainReduction	)
{

	// theLevel ..METER_DISPLAY_MAX..METER_DISPLAY_MIN
	HDC		TheDC;
	HWND	DlgItem = 0;
	HGDIOBJ	OldBrush, Brush;
	HGDIOBJ	OldPen,   Pen;
	CRect	Zoom_XY;
	int		XRed;

	float		fCompMax = COMPRED_DISPLAY_MAX;
	float		fCompMin = COMPRED_DISPLAY_MIN;

	DlgItem	=	GetDlgItem	( *pZWindow,	nIDC	);

	if (DlgItem != NULL)
	{

		TheDC		= GetDC ( DlgItem );
		GetClientRect (DlgItem, Zoom_XY);


		XRed	= (int) floor ( (fGainReduction-fCompMin)/(fCompMax-fCompMin) * (Zoom_XY.bottom - Zoom_XY.top) + Zoom_XY.top+0.5);

		Pen			= CreatePen(PS_SOLID, 0, RGB(255,0,0));
		OldPen		= SelectObject( TheDC, Pen);
		Brush		= CreateSolidBrush( RGB(255,0,0));
		OldBrush	= SelectObject(TheDC, Brush);

		if		(XRed <= Zoom_XY.top)		{	XRed = Zoom_XY.top;			}
		else if (XRed >= Zoom_XY.bottom)	{	XRed = Zoom_XY.bottom;		}

		Rectangle (TheDC, Zoom_XY.left, Zoom_XY.top, Zoom_XY.right, Zoom_XY.top+XRed);

		SelectObject (TheDC, OldBrush);
		DeleteObject (Brush);
		SelectObject (TheDC, OldPen);
		DeleteObject (Pen);

		Pen			= CreatePen(PS_SOLID, 0, RGB(255,255,255));
		OldPen		= SelectObject( TheDC, Pen);
		Brush		= CreateSolidBrush( RGB(255,255,255));
		OldBrush	= SelectObject(TheDC, Brush);

		Rectangle (TheDC, Zoom_XY.left, Zoom_XY.top+XRed, Zoom_XY.right, Zoom_XY.bottom);

		SelectObject (TheDC, OldBrush);
		DeleteObject (Brush);
		SelectObject (TheDC, OldPen);
		DeleteObject (Pen);

		ReleaseDC ( *pZWindow, TheDC );
	}
}

void	DrawZImpedance	(	int nIDC, float	fLevel, float fOpenThresh, float fShortThresh, float fZMax )
{

	// theLevel ..METER_DISPLAY_MAX..METER_DISPLAY_MIN
	HDC		TheDC;
	HWND	DlgItem = 0;
	HGDIOBJ	OldBrush, Brush;
	HGDIOBJ	OldPen,   Pen;
	CRect	Zoom_XY;
	int		XGreen, XYellow, XRed;
	int XBlack;


	DlgItem	=	GetDlgItem	( *pZWindow,	nIDC	);

	if (DlgItem != NULL)
	{

		TheDC		= GetDC ( DlgItem );
		GetClientRect (DlgItem, Zoom_XY);

	
		XGreen	= (int) floor ( (fLevel-Z_DISPLAY_MIN)/(fZMax-Z_DISPLAY_MIN) * Zoom_XY.bottom - Zoom_XY.top + 0.5);
		XYellow	= (int) floor ( (fLevel-Z_DISPLAY_MIN)/(fZMax-Z_DISPLAY_MIN) * Zoom_XY.bottom - Zoom_XY.top + 0.5);
		XRed	= (int) floor ( (fLevel-Z_DISPLAY_MIN)/(fZMax-Z_DISPLAY_MIN) * Zoom_XY.bottom - Zoom_XY.top + 0.5);
		XBlack = Zoom_XY.bottom;

		if (fLevel < 0)
		{
			XBlack = Zoom_XY.bottom;
		}

		if (fLevel < fShortThresh) 
			XGreen = (int) floor (( (0.5-Z_DISPLAY_MIN)/(fZMax-Z_DISPLAY_MIN) ) * Zoom_XY.bottom - Zoom_XY.top + 0.5);
		//if (fLevel > fThresh) 
		//	XYellow = (int) floor (( (0.0-Z_DISPLAY_MIN)/(Z_DISPLAY_MAX-Z_DISPLAY_MIN)) * Zoom_XY.bottom - Zoom_XY.top + 0.5);



		// Draw Middle Color
		if ((fLevel >= fShortThresh) && (fLevel <= fOpenThresh))
		{
	

			Pen			= CreatePen(PS_SOLID, 0, RGB(255,255,0));
			OldPen		= SelectObject( TheDC, Pen);
			Brush		= CreateSolidBrush( RGB(255,255,0));
			OldBrush	= SelectObject(TheDC, Brush);

			if		(XYellow <= Zoom_XY.top)	{	XYellow = Zoom_XY.top;		}
			else if (XYellow >= Zoom_XY.bottom)	{	XYellow = Zoom_XY.bottom;	}


			Rectangle (TheDC, Zoom_XY.left, Zoom_XY.bottom, Zoom_XY.right, Zoom_XY.bottom-XYellow);
			XBlack = Zoom_XY.bottom-XYellow;

			SelectObject (TheDC, OldBrush);
			DeleteObject (Brush);
			SelectObject (TheDC, OldPen);
			DeleteObject (Pen);
		}


		if ((fLevel <fShortThresh) && (fLevel >= 0))
		{

			//Pen			= CreatePen(PS_SOLID, 0, RGB(0,192,0));
			Pen			= CreatePen(PS_SOLID, 0, RGB(255,0,0));
			OldPen		= SelectObject( TheDC, Pen);
			//Brush		= CreateSolidBrush( RGB(0,192,0));
			Brush		= CreateSolidBrush( RGB(255,0,0));
			OldBrush	= SelectObject(TheDC, Brush);

			
			// Draw Green
			if		(XGreen <= Zoom_XY.top)		{	XGreen = Zoom_XY.top;		}
			else if (XGreen >= Zoom_XY.bottom)	{	XGreen = Zoom_XY.bottom;	}

			Rectangle (TheDC, Zoom_XY.left, Zoom_XY.bottom, Zoom_XY.right, Zoom_XY.bottom-XGreen);
			XBlack = Zoom_XY.bottom-XGreen;

			SelectObject (TheDC, OldBrush);
			DeleteObject (Brush);
			SelectObject (TheDC, OldPen);
			DeleteObject (Pen);

		}

		if (fLevel > fOpenThresh)
		{
			// Draw Red
			Pen			= CreatePen(PS_SOLID, 0, RGB(255,0,0));
			OldPen		= SelectObject( TheDC, Pen);
			Brush		= CreateSolidBrush( RGB(255,0,0));
			OldBrush	= SelectObject(TheDC, Brush);

			if		(XRed <= Zoom_XY.top)		{	XRed = Zoom_XY.top;			}
			else if (XRed >= Zoom_XY.bottom)	{	XRed = Zoom_XY.bottom;		}

			Rectangle (TheDC, Zoom_XY.left, Zoom_XY.bottom, Zoom_XY.right, Zoom_XY.bottom-XRed);
			XBlack = Zoom_XY.bottom-XRed;

			SelectObject (TheDC, OldBrush);
			DeleteObject (Brush);
			SelectObject (TheDC, OldPen);
			DeleteObject (Pen);
		}

		// Draw Black
		Pen			= CreatePen(PS_SOLID, 0, RGB(0,0,0));
		OldPen		= SelectObject( TheDC, Pen);
		Brush		= CreateSolidBrush( RGB(0,0,0));
		OldBrush	= SelectObject(TheDC, Brush);

		Rectangle (TheDC, Zoom_XY.left, XBlack, Zoom_XY.right, Zoom_XY.top);

		SelectObject (TheDC, OldBrush);
		DeleteObject (Brush);
		SelectObject (TheDC, OldPen);
		DeleteObject (Pen);

		ReleaseDC ( *pZWindow, TheDC );
	}
}

void CZImpedanceDlg::UpdateOutputTextMeters(CString theText,bool bAppend, bool bForce)
{
	bool m_bFreezeDisplay = false;
	if ((!m_bFreezeDisplay) || (bForce))
	{

		CString scText;
		GetDlgItemText(IDC_ZIMPEDANCE_MESSAGERX, scText);
		int theLength = strlen(theText);
		int theCurrentLength =  scText.GetLength();

		long lastChar = scText.GetLength();
		m_OutputTextMeters.SetSel(lastChar,lastChar);
		m_OutputTextMeters.HideSelection( false, true);

		if ( (theCurrentLength+theLength+10) > MAX_RX_DISPLAY )
		{
//			scText.Delete(0, MAX_RX_DISPLAY-(theLength+10));
			scText.Delete(0, MAX_RX_DISPLAY*.25);
		}
		scText += "\n" + theText;
		SetDlgItemText(IDC_ZIMPEDANCE_MESSAGERX, scText);
		int nLine = m_OutputTextMeters.GetLineCount();
		//long lastChar = scText.GetLength();
		//m_OutputTextBox.LineScroll(1);//, int nChars = 0 );
		//m_OutputTextMeters.HideSelection( false, true);
	}
	UpdateData(FALSE); // *&&&&&&gm added 5/28
}
void CZImpedanceDlg::OnBnClickedZimpedanceLogfileselect()
{
	// TODO: Add your control notification handler code here

	// TODO: Add your command handler code here
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
			SetDlgItemText(IDC_ZIMPEDANCE_LOGFILENAME, m_strLogFileName);
			//m_strLogFileName = ;
		}
}

void CZImpedanceDlg::OnBnClickedZimpedanceLogfilesclear()
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


void CZImpedanceDlg::LogResultsToFile(tREDLINEMeters* jMeters)
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
	CButton* pButton = (CButton*) GetDlgItem(IDC_ZIMPEDANCE_LOGFILEENABLE);

	if (pButton)
	{
		bEnable = (pButton->GetCheck() != 0);
	}
	if (!bEnable) return;

	float fTemp0;
	fTemp0 = ((signed char)(jMeters->uiTemperature & 0x0ff))/2.0;

	float fTemp1;
	fTemp1 = ((signed char)((jMeters->uiTemperature >> 8) & 0x0ff))/2.0;


	if (ZLogfile.Open(m_pcLogFileName,CFile::modeWrite | CFile::modeNoTruncate | CFile::modeCreate)) //NoTruncate))
	{
		int pos = ZLogfile.SeekToEnd();
		if (pos == 0)
		{
			sprintf(pcBuffer,"time,version,fault,\t temp0, Current0, Vpeak0, Z0,  \t temp1, Current1, Vpeak1, Z1\r\n"); 
			ZLogfile.Write(pcBuffer, strlen(pcBuffer));
		}
		time(&tNow);
		newtime = localtime(&tNow);
		strcpy(pcLogRecordWithTime, asctime(newtime));
		sprintf(pcLogRecord,", 0x%8.8X, %8.8x,    %5.3f, %5.3f, %5.3f, %5.3f,     %5.3f, %5.3f, %5.3f ,%5.3f\r\n",
		(long) jMeters->uiVersion,
		jMeters->ampFault,
		fTemp0,  jMeters->phCurrentSense0,  jMeters->fVPeak0, jMeters->fImpedance0, 
		fTemp1,  jMeters->phCurrentSense1,  jMeters->fVPeak1, jMeters->fImpedance1);

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
		//dZLogOld = dZLog;

		ZLogfile.Close();
	}



	return;
}	


void CZImpedanceDlg::OnBnClickedZimpedanceClear()
{
	// TODO: Add your control notification handler code here
	SetDlgItemText(IDC_ZIMPEDANCE_MESSAGERX,"");

}
