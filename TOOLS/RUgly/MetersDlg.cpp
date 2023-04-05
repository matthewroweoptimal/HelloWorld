// MetersDlg.cpp : implementation file
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
#include "MetersDlg.h"
#include <stdlib.h>
#include "resource.h"
#include <math.h>

#include "ProductMap.h"

#define METER_DISPLAY_MIN -40
#define METER_DISPLAY_MAX 12
#define COMPRED_DISPLAY_MAX 20
#define COMPRED_DISPLAY_MIN 0
#define DYNEQ_DISPLAY_MAX 20
#define DYNEQ_DISPLAY_MIN -20



void	Meters_DrawCompRed	(	int nIDC, float	fGainReduction	);
void	Meters_DrawMeter	(	int nIDC, float	fLevel	);


CMetersDlg*	pMeterWindow;
// CMetersDlg dialog

IMPLEMENT_DYNAMIC(CMetersDlg, CDialog)

CMetersDlg::CMetersDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMetersDlg::IDD, pParent)
{
	m_dlgParent = NULL;
	m_nProductType = ePRODUCT_OLYSPEAKER;
	m_bInProgress = false;

	m_uiSequence = 1;
	m_uiMeterRequestInterval = 500;
	
	
	void OnSetMeterSubscribeSelect(int Select);
	m_nMeterListNumPIDs = 0;
	m_pMeterList = 0; // id, target, pid, pid, pid
	m_nMeterListRate = 500; // msec 


	m_bMeterListInProgress = false;

}

CMetersDlg::~CMetersDlg()
{
}

void CMetersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMetersDlg, CDialog)
	ON_BN_CLICKED(IDCONNECT, &CCommSettings::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_METERS_START, &CMetersDlg::OnBnClickedMetersStart)
	ON_BN_CLICKED(IDC_METERS_STOP, &CMetersDlg::OnBnClickedMetersStop)
	ON_BN_CLICKED(IDC_UPDATE_INTERVAL, &CMetersDlg::OnBnClickedUpdateInterval)
	ON_BN_CLICKED(IDC_METERS_SUBSCRIBE_START, &CMetersDlg::OnBnClickedMetersSubscribeStart)
	ON_BN_CLICKED(IDC_METERS_SUBSCRIBE_STOP, &CMetersDlg::OnBnClickedMetersSubscribeStop)
END_MESSAGE_MAP()


// CMetersDlg message handlers
BOOL CMetersDlg::OnInitDialog()
{
	bool retcode;
	retcode = CDialog::OnInitDialog();

	pMeterWindow = this;


	OnSetMeterSubscribeSelect(1);

	return retcode;
}

void CMetersDlg::OnBnClickedOk()
{
}

// ------------------------------------------------------------------------
// Display
// ------------------------------------------------------------------------




void	Meters_DrawMeter	(	int nIDC, float	fLevel	)
{

	// theLevel ..METER_DISPLAY_MAX..METER_DISPLAY_MIN
	HDC		TheDC;
	HWND	DlgItem = 0;
	HGDIOBJ	OldBrush, Brush;
	HGDIOBJ	OldPen,   Pen;
	CRect	Zoom_XY;
	int		XGreen, XYellow, XRed;


	DlgItem	=	GetDlgItem	( *pMeterWindow,	nIDC	);

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

		ReleaseDC ( *pMeterWindow, TheDC );
	}
}

void	Meters_DrawCompRed	(	int nIDC, float	fGainReduction	)
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

	DlgItem	=	GetDlgItem	( *pMeterWindow,	nIDC	);

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

		ReleaseDC ( *pMeterWindow, TheDC );
	}
}

void	Meters_DrawDynEQGain	(	int nIDC, float	fGain	)
{
	// theLevel ..METER_DISPLAY_MAX..METER_DISPLAY_MIN
	HDC		TheDC;
	HWND	DlgItem = 0;
	HGDIOBJ	OldBrush, Brush;
	HGDIOBJ	OldPen,   Pen;
	CRect	Zoom_XY;
	int		XRed;
	int		nZeroPosition;
	int		nMeterPosition;
	int		nMeterTop;
	int		nMeterBottom;
	float	fRange;

	float		fMax = DYNEQ_DISPLAY_MAX;
	float		fMin = DYNEQ_DISPLAY_MIN;

	DlgItem	=	GetDlgItem	( *pMeterWindow,	nIDC	);

	if (DlgItem != NULL)
	{

		TheDC		= GetDC ( DlgItem );
		GetClientRect (DlgItem, Zoom_XY);

		fRange = fMax - fMin;
		nZeroPosition = (int) floor ( (fMax/fRange) * (Zoom_XY.bottom - Zoom_XY.top) + Zoom_XY.top+0.5);
		nMeterPosition = (int) floor ( ((fMax - fGain)/fRange) * (Zoom_XY.bottom - Zoom_XY.top) + Zoom_XY.top+0.5);

		Pen			= CreatePen(PS_SOLID, 0, RGB(255,0,0));
		OldPen		= SelectObject( TheDC, Pen);
		Brush		= CreateSolidBrush( RGB(255,0,0));
		OldBrush	= SelectObject(TheDC, Brush);

		if		(nMeterPosition <= Zoom_XY.top)		{	nMeterPosition = Zoom_XY.top;			}
		else if (nMeterPosition >= Zoom_XY.bottom)	{	nMeterPosition = Zoom_XY.bottom;		}
		if		(nZeroPosition <= Zoom_XY.top)		{	nZeroPosition = Zoom_XY.top;			}
		else if (nZeroPosition >= Zoom_XY.bottom)	{	nZeroPosition = Zoom_XY.bottom;			}

		// draw the meter first
		if (nMeterPosition < nZeroPosition) {	// positive gain
			nMeterTop = nMeterPosition;
			nMeterBottom = nZeroPosition;
		} else {								// negative gain
			nMeterTop = nZeroPosition;
			nMeterBottom = nMeterPosition;
		}

		Rectangle (TheDC, Zoom_XY.left, Zoom_XY.top+nMeterTop, Zoom_XY.right, Zoom_XY.top+nMeterBottom);

		SelectObject (TheDC, OldBrush);
		DeleteObject (Brush);
		SelectObject (TheDC, OldPen);
		DeleteObject (Pen);

		// now draw the background in two parts
		Pen			= CreatePen(PS_SOLID, 0, RGB(255,255,255));
		OldPen		= SelectObject( TheDC, Pen);
		Brush		= CreateSolidBrush( RGB(255,255,255));
		OldBrush	= SelectObject(TheDC, Brush);

		Rectangle (TheDC, Zoom_XY.left, Zoom_XY.top, Zoom_XY.right, Zoom_XY.top+nMeterTop);
		Rectangle (TheDC, Zoom_XY.left, Zoom_XY.top+nMeterBottom, Zoom_XY.right, Zoom_XY.bottom);

		SelectObject (TheDC, OldBrush);
		DeleteObject (Brush);
		SelectObject (TheDC, OldPen);
		DeleteObject (Pen);

		ReleaseDC ( *pMeterWindow, TheDC );
	}
}

//----------------------------------------------------------------------------------------------------

void CMetersDlg::OnBnClickedMetersStart()
{
	// TODO: Add your control notification handler code here
	m_bInProgress = true;
	if (m_dlgParent)
	{
//		m_dlgParent->SetTimer(METER_TIMERID,500,NULL);
		m_dlgParent->SetTimer(METER_TIMERID,m_uiMeterRequestInterval,NULL);
	}
	SetDlgItemText(IDC_METERS_STATUS,"Started");

}

void CMetersDlg::OnBnClickedMetersStop()
{
	// TODO: Add your control notification handler code here

	m_bInProgress = false;
	if (m_dlgParent)
	{
		m_dlgParent->KillTimer(METER_TIMERID);
	}
	SetDlgItemText(IDC_METERS_STATUS,"Stopped");

}

#define METERS_PARAMETER_MAX 100

void	CMetersDlg::ShowMeter(int nTarget, int nParam, float fValue)
{
	char pcBuffer[100];

	for(int j=0;j<MAX_METERS_DESC;j++)
	{
		if ((m_descMeters[j].nTarget == nTarget) && (m_descMeters[j].nParamNum == nParam))
		{
			switch(m_descMeters[j].nMeterType)
			{
			case eMETERTYPE_NORMAL:
				Meters_DrawMeter(m_descMeters[j].nIDC, fValue);
				break;
			case eMETERTYPE_GATEGAINRED:
//				break;
			case eMETERTYPE_COMPGAINRED:
				Meters_DrawCompRed(m_descMeters[j].nIDC, fValue);
				break;
			case eMETERTYPE_DYNEQGAIN:
				Meters_DrawDynEQGain(m_descMeters[j].nIDC, fValue);
				break;

			}
			sprintf(pcBuffer,"%3.2f",fValue);
			SetDlgItemText(m_descMeters[j].nIDCValue,pcBuffer);
		}

	}

}







void CMetersDlg::OnTimer(UINT nIDEvent)
{

	switch(nIDEvent)
	{
	case METER_TIMERID:
		m_mGetMeters.payload = m_pcGetMeterPayload;
		if (m_dlgParent)
		{
#if(1)
			m_dlgParent->m_MandolinComm.CreateParameterEditGetNeighborhood(&m_mGetMeters, m_uiSequence,m_dlgParent->GetMetersTarget(m_nProductType),1,0);
#else
// use this to force a message out for testing meter subscription intervals
			intfloat value;
			value.f = 0;
			m_dlgParent->m_MandolinComm.CreateParameterEditSetNeighborhood(&m_mGetMeters, m_uiSequence,m_dlgParent->GetMetersTarget(m_nProductType),1,&value,1,true,0);
#endif
			m_dlgParent->TryToSendMandolinMessage(&m_mGetMeters, true);
			m_dlgParent->SetTimer(METER_TIMERID,m_uiMeterRequestInterval,NULL);
		}
		m_uiSequence++;
		if (m_uiSequence==0) m_uiSequence++;
		break;
	}
}

void CMetersDlg::SetProductType(int nProductType)
{
	static int j = 0;
	CButton* pButton;

	// set the switch states
	m_nProductType = nProductType;
	ControlDesc* pDescList;
	tStringIDC* pTitleList;

	int nIDCLevel[MAX_METERS_DESC] = 
	{
		IDC_METERS_LEVEL1,
		IDC_METERS_LEVEL2,
		IDC_METERS_LEVEL3,
		IDC_METERS_LEVEL4,
		IDC_METERS_LEVEL5,
		IDC_METERS_LEVEL6,
		IDC_METERS_LEVEL7,
		IDC_METERS_LEVEL8,
		IDC_METERS_LEVEL9,
		IDC_METERS_LEVEL10,
		IDC_METERS_LEVEL11,
		IDC_METERS_LEVEL12,
		IDC_METERS_LEVEL13,
		IDC_METERS_LEVEL14,
		IDC_METERS_LEVEL15,
		IDC_METERS_LEVEL16,
		IDC_METERS_LEVEL17,
		IDC_METERS_LEVEL18,
		IDC_METERS_LEVEL19,
		IDC_METERS_LEVEL20,
		IDC_METERS_LEVEL21,
		IDC_METERS_LEVEL22,
		IDC_METERS_LEVEL23,
		IDC_METERS_LEVEL24,
		IDC_METERS_LEVEL25,
		IDC_METERS_LEVEL26,
		IDC_METERS_LEVEL27,
		IDC_METERS_LEVEL28,
		IDC_METERS_LEVEL29,
		IDC_METERS_LEVEL30,
		IDC_METERS_LEVEL31,

	};
	int nIDCMin[MAX_METERS_DESC] = 
	{
		IDC_METERS_MIN1,
		IDC_METERS_MIN2,
		IDC_METERS_MIN3,
		IDC_METERS_MIN4,
		IDC_METERS_MIN5,
		IDC_METERS_MIN6,
		IDC_METERS_MIN7,
		IDC_METERS_MIN8,
		IDC_METERS_MIN9,
		IDC_METERS_MIN10,
		IDC_METERS_MIN11,
		IDC_METERS_MIN12,
		IDC_METERS_MIN13,
		IDC_METERS_MIN14,
		IDC_METERS_MIN15,
		IDC_METERS_MIN16,
		IDC_METERS_MIN17,
		IDC_METERS_MIN18,
		IDC_METERS_MIN19,
		IDC_METERS_MIN20,
		IDC_METERS_MIN21,
		IDC_METERS_MIN22,
		IDC_METERS_MIN23,
		IDC_METERS_MIN24,
		IDC_METERS_MIN25,
		IDC_METERS_MIN26,
		IDC_METERS_MIN27,
		IDC_METERS_MIN28,
		IDC_METERS_MIN29,
		IDC_METERS_MIN30,
		IDC_METERS_MIN31,

	};
	int nIDCMax[MAX_METERS_DESC] = 
	{
		IDC_METERS_MAX1,
		IDC_METERS_MAX2,
		IDC_METERS_MAX3,
		IDC_METERS_MAX4,
		IDC_METERS_MAX5,
		IDC_METERS_MAX6,
		IDC_METERS_MAX7,
		IDC_METERS_MAX8,
		IDC_METERS_MAX9,
		IDC_METERS_MAX10,
		IDC_METERS_MAX11,
		IDC_METERS_MAX12,
		IDC_METERS_MAX13,
		IDC_METERS_MAX14,
		IDC_METERS_MAX15,
		IDC_METERS_MAX16,
		IDC_METERS_MAX17,
		IDC_METERS_MAX18,
		IDC_METERS_MAX19,
		IDC_METERS_MAX20,
		IDC_METERS_MAX21,
		IDC_METERS_MAX22,
		IDC_METERS_MAX23,
		IDC_METERS_MAX24,
		IDC_METERS_MAX25,
		IDC_METERS_MAX26,
		IDC_METERS_MAX27,
		IDC_METERS_MAX28,
		IDC_METERS_MAX29,
		IDC_METERS_MAX30,
		IDC_METERS_MAX31,

	};
	int nIDCTitle[MAX_METERS_DESC] = 
	{
		IDC_METERS_TITLE1,
		IDC_METERS_TITLE2,
		IDC_METERS_TITLE3,
		IDC_METERS_TITLE4,
		IDC_METERS_TITLE5,
		IDC_METERS_TITLE6,
		IDC_METERS_TITLE7,
		IDC_METERS_TITLE8,
		IDC_METERS_TITLE9,
		IDC_METERS_TITLE10,
		IDC_METERS_TITLE11,
		IDC_METERS_TITLE12,
		IDC_METERS_TITLE13,
		IDC_METERS_TITLE14,
		IDC_METERS_TITLE15,
		IDC_METERS_TITLE16,
		IDC_METERS_TITLE17,
		IDC_METERS_TITLE18,
		IDC_METERS_TITLE19,
		IDC_METERS_TITLE20,
		IDC_METERS_TITLE21,
		IDC_METERS_TITLE22,
		IDC_METERS_TITLE23,
		IDC_METERS_TITLE24,
		IDC_METERS_TITLE25,
		IDC_METERS_TITLE26,
		IDC_METERS_TITLE27,
		IDC_METERS_TITLE28,
		IDC_METERS_TITLE29,
		IDC_METERS_TITLE30,
		IDC_METERS_TITLE31,

	};
	int nLength;


	nLength = m_dlgParent->GetMetersTitleList(m_nProductType,&pTitleList);

	for(j=0;j<nLength;j++)
	{
		SetDlgItemText(pTitleList[j].nIDC, pTitleList[j].str);
	}

	nLength = m_dlgParent->GetMetersDescList(m_nProductType,&pDescList);

	// faders
	for(j=0;j<nLength;j++)
	{
		m_descMeters[j].nTarget = pDescList[j].nTarget; 
		m_descMeters[j].nParamNum = pDescList[j].nParamNum;
		m_descMeters[j].nIDC = pDescList[j].nIDC;
		m_descMeters[j].nIDCValue = pDescList[j].nIDCValue;
		m_descMeters[j].nMeterType = m_dlgParent->GetMeterType(m_nProductType, pDescList[j].nTarget, pDescList[j].nParamNum);
	}
	for(;j<MAX_METERS_DESC;j++)
	{
		m_descMeters[j].nTarget = 0; 
		m_descMeters[j].nParamNum = 0;
		m_descMeters[j].nIDC = nIDCLevel[j];
		m_descMeters[j].nIDCValue = m_descMeters[0].nIDCValue;
	}

	
	CWnd* pWnd;
	CWnd* pWndMin;
	CWnd* pWndMax;
	CWnd* pWndTitle;
	char pcBuffer[100];
	float fMin,fMax;

	for(j=0;j<MAX_METERS_DESC;j++)
	{
		pWnd =  GetDlgItem(m_descMeters[j].nIDC);
		pWndMax =  GetDlgItem(nIDCMax[j]);
		pWndMin =  GetDlgItem(nIDCMin[j]);
		pWndTitle = GetDlgItem(nIDCTitle[j]);
		if ((m_descMeters[j].nIDC != 0)&& (m_descMeters[j].nTarget != 0) && (m_descMeters[j].nParamNum != 0))
		{
			if (pWnd) pWnd->ShowWindow(SW_SHOW);
			if (pWndMin) pWndMin->ShowWindow(SW_SHOW);
			if (pWndMax) pWndMax->ShowWindow(SW_SHOW);
			if (pWndTitle) pWndTitle->ShowWindow(SW_SHOW);

			switch(m_descMeters[j].nMeterType)
			{
				case eMETERTYPE_NORMAL:
					fMin = METER_DISPLAY_MIN;
					fMax = METER_DISPLAY_MAX;
					sprintf(pcBuffer,"%3.1f",fMin);
//					SetDlgItemText(nIDCMin[j],pcBuffer);
					SetDlgItemText(nIDCMin[j],"");
					sprintf(pcBuffer,"%3.1f",fMax);
//					SetDlgItemText(nIDCMax[j],pcBuffer);
					SetDlgItemText(nIDCMax[j],"");
					ShowMeter(m_descMeters[j].nTarget, m_descMeters[j].nParamNum, fMin);
					SetDlgItemText(m_descMeters[j].nIDCValue,"-120");
					break;
				case eMETERTYPE_GATEGAINRED:
					SetDlgItemText(nIDCMin[j],"");
					SetDlgItemText(nIDCMax[j],"");
					ShowMeter(m_descMeters[j].nTarget, m_descMeters[j].nParamNum, -120);
					break;
				case eMETERTYPE_COMPGAINRED:
					fMin = COMPRED_DISPLAY_MAX;
					fMax = COMPRED_DISPLAY_MIN;
					sprintf(pcBuffer,"%3.1f",fMin);
//					SetDlgItemText(nIDCMin[j],pcBuffer);
					SetDlgItemText(nIDCMin[j],"");
					sprintf(pcBuffer,"%3.1f",fMax);
//					SetDlgItemText(nIDCMax[j],pcBuffer);
					SetDlgItemText(nIDCMax[j],"");
					ShowMeter(m_descMeters[j].nTarget, m_descMeters[j].nParamNum, fMax);
					SetDlgItemText(m_descMeters[j].nIDCValue,"0.0");
					break;
				case eMETERTYPE_DYNEQGAIN:
					fMin = DYNEQ_DISPLAY_MIN;
					fMax = DYNEQ_DISPLAY_MAX;
					sprintf(pcBuffer,"%3.1f",fMin);
					SetDlgItemText(nIDCMin[j],pcBuffer);
//					SetDlgItemText(nIDCMin[j],"");
					sprintf(pcBuffer,"%3.1f",fMax);
					SetDlgItemText(nIDCMax[j],pcBuffer);
//					SetDlgItemText(nIDCMax[j],"");
					ShowMeter(m_descMeters[j].nTarget, m_descMeters[j].nParamNum, fMax);
					SetDlgItemText(m_descMeters[j].nIDCValue,"0.0");
					break;
						
			}

		}
		else
		{
			if (pWnd) pWnd->ShowWindow(SW_HIDE);
			if (pWndMin) pWndMin->ShowWindow(SW_HIDE);
			if (pWndMax) pWndMax->ShowWindow(SW_HIDE);
			if (pWndTitle) pWndTitle->ShowWindow(SW_HIDE);

		}

		
		
	}
	SetDlgItemText(IDC_METER_REQUEST_INTERVAL,"500");
	SetDlgItemText(IDC_METER_SUBSCRIBE_RATE,"500");


}

void CMetersDlg::OnBnClickedUpdateInterval()
{
	// TODO: Add your control notification handler code here
	char pcBuffer[100];
	float fTemp;
	// get the current interval setting
	GetDlgItemText(IDC_METER_REQUEST_INTERVAL,pcBuffer,100);
	// convert it to an int
	fTemp = atof(pcBuffer);
	// range check
	if (fTemp < 10.0) {
		fTemp = 10.0;
	} else if (fTemp > 100000.00) {
		fTemp = 100000.0;
	}
	m_uiMeterRequestInterval = (int)fTemp;
}


void CMetersDlg::OnSetMeterSubscribeSelect(int nSelect)
{

	CButton* pButtonStatus;
	CButton* pButtonAll;


	pButtonStatus = (CButton*) GetDlgItem(IDC_METER_SUBSCRIBE_LIST_STATUS);
	pButtonAll = (CButton*)GetDlgItem(IDC_METER_SUBSCRIBE_LIST_ALL);

	if (pButtonStatus)	pButtonStatus->SetCheck(nSelect == 0);
	if (pButtonAll)		pButtonAll->SetCheck(nSelect == 1);
}

void CMetersDlg::OnBnClickedMetersSubscribeStart()
{
	// TODO: Add your control notification handler code here
	int nRate;
	int nListSelect;
	intfloat jValue;
	int nType;
	CButton* pButtonStatus;
	CButton* pButtonAll;
	char pcString[200];


	if (m_bMeterListInProgress)
	{
		OnBnClickedMetersSubscribeStop();
	}
	m_bMeterListInProgress = false;
	GetDlgItemText(IDC_METER_SUBSCRIBE_RATE,pcString,sizeof(pcString)-1);
	jValue = m_dlgParent->ConvertString2Value(pcString, nType);

	if (nType != 0)
	{
		jValue.i = jValue.f;
	}
	pButtonStatus = (CButton*) GetDlgItem(IDC_METER_SUBSCRIBE_LIST_STATUS);
	pButtonAll = (CButton*)GetDlgItem(IDC_METER_SUBSCRIBE_LIST_ALL);

	nListSelect = 1;
	if (pButtonStatus)
	{
		if (pButtonStatus->GetCheck() != 0)
		{
			nListSelect = 0;
		}
	}
	if (pButtonAll)
	{
		if (pButtonAll->GetCheck() != 0)
		{
			nListSelect = 1;
		}
	}
	m_pMeterList =  m_dlgParent->GetMeterSubscriptionList(m_nProductType,nListSelect,&m_nMeterListNumPIDs);		// id, target, pid, pid, pid


	m_nMeterListRate = jValue.i; // msec 


	if ((m_nMeterListNumPIDs > 0) && (m_pMeterList))
	{
		int nListTarget = (int) m_pMeterList[1];
		int nListId = (int)  m_pMeterList[0];
		unsigned int* pList = &m_pMeterList[2];
	
		if ((nListId== 1) || (nListId == 2)) // meter
		{
			m_dlgParent->SetParameterList(nListId-1,nListId, m_nMeterListNumPIDs, nListTarget, pList);		// parent has 2 version of the list
		}

		m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
		m_dlgParent->m_MandolinComm.CreateParameterList(&m_dlgParent->m_msgTx, m_dlgParent->m_nTxSequence, nListId, pList, m_nMeterListNumPIDs );
		m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);


		m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
		m_dlgParent->m_MandolinComm.CreateSubscribeApplicationParameter(&m_dlgParent->m_msgTx, m_dlgParent->m_nTxSequence, m_nMeterListRate, nListId, nListTarget, true );
		m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);

		m_bMeterListInProgress = true;
		sprintf(pcString,"Started - %s - %d", (nListSelect==0)?"Status":"All Meters",m_nMeterListRate);
		SetDlgItemText(IDC_METERS_SUBSCRIBE_STATUS,pcString);
	}
	else
	{
		m_dlgParent->SetParameterList(0,-1, 0, 0, 0);		// only has one copy of the list

		//m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
		//m_dlgParent->m_MandolinComm.CreateSubscribeApplicationParameter(&m_dlgParent->m_msgTx, m_dlgParent->m_nTxSequence, 0, nListId, nListTarget );
		//m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);

	}

}

void CMetersDlg::OnBnClickedMetersSubscribeStop()
{
	// TODO: Add your control notification handler code here
	if ((m_pMeterList) && (m_nMeterListNumPIDs>0) && m_bMeterListInProgress)
	{

		int nListTarget = (int) m_pMeterList[1];
		int nListId = (int)  m_pMeterList[0];
		unsigned int* pList = &m_pMeterList[2];

		m_bMeterListInProgress = false;
		m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
		m_dlgParent->m_MandolinComm.CreateSubscribeApplicationParameter(&m_dlgParent->m_msgTx, m_dlgParent->m_nTxSequence, 0, nListId, nListTarget );
		m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);

		SetDlgItemText(IDC_METERS_SUBSCRIBE_STATUS,"Stopped");
	}


}
