// ParameterAdjust.cpp : implementation file
//

#include "stdafx.h"
#include "LOUD_types.h"
extern "C" 
{
#include "mandolin.h"
}
#include "Product_arch.h"
#include "RUglyDlg.h"
#include "ParameterAdjust.h"
#include <stdlib.h>
#include "resource.h"
#include <math.h>

#include "ProductMap.h"
//#include "HAL_types.h"


// ParameterAdjust dialog

IMPLEMENT_DYNAMIC(ParameterAdjust, CDialog)

ParameterAdjust::ParameterAdjust(CWnd* pParent /*=NULL*/)
	: CDialog(ParameterAdjust::IDD, pParent)
{
	int i;
	m_dlgParent = NULL;
	m_nProductType = ePRODUCT_OLYSPEAKER;


	for(i=0;i<sizeof(m_nGetParameterTimeout)/sizeof(int);i++)
	{
		m_nGetParameterTimeout[i] = 0;
	}


}

ParameterAdjust::~ParameterAdjust()
{
}

void ParameterAdjust::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PARAMETERADJUST_SLIDER1, m_Slider[0]);
	DDX_Control(pDX, IDC_PARAMETERADJUST_SLIDER2, m_Slider[1]);
	DDX_Control(pDX, IDC_PARAMETERADJUST_SLIDER3, m_Slider[2]);
	DDX_Control(pDX, IDC_PARAMETERADJUST_SLIDER4, m_Slider[3]);
	DDX_Control(pDX, IDC_PARAMETERADJUST_SLIDER5, m_Slider[4]);
}


BEGIN_MESSAGE_MAP(ParameterAdjust, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_PARAMETERADJUST_SEND1, &ParameterAdjust::OnBnClickedParameteradjustSend1)
	ON_CBN_SELCHANGE(IDC_PARAMETERADJUST_NAMECOMBO1, &ParameterAdjust::OnCbnSelchangeParameteradjustNamecombo1)
	ON_CBN_SELCHANGE(IDC_PARAMETERADJUST_NAMECOMBO2, &ParameterAdjust::OnCbnSelchangeParameteradjustNamecombo2)
	ON_CBN_SELCHANGE(IDC_PARAMETERADJUST_NAMECOMBO3, &ParameterAdjust::OnCbnSelchangeParameteradjustNamecombo3)
	ON_CBN_SELCHANGE(IDC_PARAMETERADJUST_NAMECOMBO4, &ParameterAdjust::OnCbnSelchangeParameteradjustNamecombo4)
	ON_CBN_SELCHANGE(IDC_PARAMETERADJUST_NAMECOMBO5, &ParameterAdjust::OnCbnSelchangeParameteradjustNamecombo5)
	ON_BN_CLICKED(IDC_PARAMETERADJUST_SEND2, &ParameterAdjust::OnBnClickedParameteradjustSend2)
	ON_BN_CLICKED(IDC_PARAMETERADJUST_SEND3, &ParameterAdjust::OnBnClickedParameteradjustSend3)
	ON_BN_CLICKED(IDC_PARAMETERADJUST_SEND4, &ParameterAdjust::OnBnClickedParameteradjustSend4)
	ON_BN_CLICKED(IDC_PARAMETERADJUST_SEND5, &ParameterAdjust::OnBnClickedParameteradjustSend5)
	ON_BN_CLICKED(IDC_PARAMETERADJUST_GET1, &ParameterAdjust::OnBnClickedParameteradjustGet1)
	ON_BN_CLICKED(IDC_PARAMETERADJUST_GET2, &ParameterAdjust::OnBnClickedParameteradjustGet2)
	ON_BN_CLICKED(IDC_PARAMETERADJUST_GET3, &ParameterAdjust::OnBnClickedParameteradjustGet3)
	ON_BN_CLICKED(IDC_PARAMETERADJUST_GET4, &ParameterAdjust::OnBnClickedParameteradjustGet4)
	ON_BN_CLICKED(IDC_PARAMETERADJUST_GET5, &ParameterAdjust::OnBnClickedParameteradjustGet5)
	ON_BN_CLICKED(IDC_PARAMETERADJUST_PAGEUP, &ParameterAdjust::OnBnClickedParameteradjustPageup)
	ON_BN_CLICKED(IDC_PARAMETERADJUST_SINGLEUP, &ParameterAdjust::OnBnClickedParameteradjustSingleup)
	ON_BN_CLICKED(IDC_PARAMETERADJUST_SINGLEDOWN, &ParameterAdjust::OnBnClickedParameteradjustSingledown)
	ON_BN_CLICKED(IDC_PARAMETERADJUST_PAGEDOWN, &ParameterAdjust::OnBnClickedParameteradjustPagedown)
END_MESSAGE_MAP()


// ParameterAdjust message handlers
BOOL ParameterAdjust::OnInitDialog()
{
	int i;
	bool bRetCode;

	bRetCode = CDialog::OnInitDialog();

	SetProductType(m_nProductType);

	SetScroll(0,255);
	//SetScrollRange(SB_HORZ,0,255);

	nAdjustedBySlider = -1; 
	for(i=0;i<5;i++)
	{
		SetGetTimeoutFlag(i,"");
	}

	return bRetCode;

}

#define PARAMETERADJUST_RATE 100
void ParameterAdjust::OnTimer(UINT nIDEvent)
{
	int i;
	bool bHasTime = false;

	int nRate = PARAMETERADJUST_RATE;
	switch(nIDEvent)
	{
	case PARAMETERADJUST_GETPARAMETER_TIMERID:
		m_dlgParent->KillTimer(PARAMETERADJUST_GETPARAMETER_TIMERID);
		bHasTime = false;
		for(i=0;i<5;i++)
		{
			if (m_nGetParameterTimeout[i] > 0)
			{
				m_nGetParameterTimeout[i] -= nRate;
				if (m_nGetParameterTimeout[i] <= 0)
				{
					SetGetTimeoutFlag(i,"Timed Out");
					m_nGetParameterTimeout[i] = 0;
				}
				else
				{
					bHasTime = true;
				}
			}
		}
		if (bHasTime) 
			m_dlgParent->SetTimer(PARAMETERADJUST_GETPARAMETER_TIMERID,PARAMETERADJUST_RATE,NULL);

		break;
	}

}


void ParameterAdjust::OnOK()
{
	CWnd* pwndCtrl = GetFocus();
	if (!pwndCtrl) 
		return;
	int ctrl_ID = pwndCtrl->GetDlgCtrlID();

	switch(ctrl_ID)
	{
	case IDC_PARAMETERADJUST_VALUE1: OnBnClickedParameteradjustSend1(); break;
	case IDC_PARAMETERADJUST_VALUE2: OnBnClickedParameteradjustSend2(); break;
	case IDC_PARAMETERADJUST_VALUE3: OnBnClickedParameteradjustSend3(); break;
	case IDC_PARAMETERADJUST_VALUE4: OnBnClickedParameteradjustSend4(); break;
	case IDC_PARAMETERADJUST_VALUE5: OnBnClickedParameteradjustSend5(); break;
	}
	

}
void ParameterAdjust::SetScroll(int nMin, int nMax)
{
	int i;
	//int nIDCScrollName[] = { 
	//	IDC_PARAMETERADJUST_SLIDER1,
	//	IDC_PARAMETERADJUST_SLIDER2,
	//	IDC_PARAMETERADJUST_SLIDER3,
	//	IDC_PARAMETERADJUST_SLIDER4,
	//	IDC_PARAMETERADJUST_SLIDER5
	//};
	//CScrollBar* pScroll;
	SCROLLINFO ScrollInfo;
	ScrollInfo.cbSize = sizeof(ScrollInfo);     // size of this structure
	ScrollInfo.fMask = SIF_ALL;                 // parameters to set
	ScrollInfo.nMin = 0;                        // minimum scrolling position
	ScrollInfo.nMax = 255;                      // maximum scrolling position
	ScrollInfo.nPage = 1;                      // the page size of the scroll box
	ScrollInfo.nPos = 255;                       // initial position of the scroll box
	ScrollInfo.nTrackPos = 0;                   // immediate position of a scroll box that the user is dragging

	//SetScrollRange(SB_HORZ,0,255);
	for(i=0;i<5;i++)
	{

		m_Slider[i].SetScrollInfo(&ScrollInfo, false);

	//	pScroll = (CScrollBar*)GetDlgItem(nIDCScrollName[i]);
	//	if (pScroll)
	//	{
	//		pScroll->SetScrollRange(nMin,nMax);
			//((CSliderCtrl *)pScroll)->SetScrollRange(SB_HORZ,0,255,true);

			//ScrollInfo.nMax = nMin;
			//ScrollInfo.nMin = nMax;
		
			// pScroll->SetScrollInfo(&ScrollInfo);  // get information about the scroll

			// pScroll->GetScrollInfo(&ScrollInfo);  // get information about the scroll
	//	}
 
	}

	
}

void ParameterAdjust::SetProductType(int nProductType)
{

	CComboBox* pCombo[5];
	int nIDC[] = {
			IDC_PARAMETERADJUST_NAMECOMBO1,
			IDC_PARAMETERADJUST_NAMECOMBO2,
			IDC_PARAMETERADJUST_NAMECOMBO3,
			IDC_PARAMETERADJUST_NAMECOMBO4,
			IDC_PARAMETERADJUST_NAMECOMBO5,
	};

	int i;

	tParameterDesc* pParameterDesc;


	int nTableLength;
	int nXMLVersion;

	m_nProductType = nProductType;
	for (i=0;i<5;i++)
	{
		pCombo[i] = (CComboBox*) GetDlgItem(nIDC[i]);
	}


	for (i=0;i<5;i++)
	{
		if (pCombo[i]) pCombo[i]->ResetContent();
	}

	switch(m_nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		m_dlgParent->ParameterAdjustSetComboBox(m_nProductType,pCombo,5);
		break;
	};


}
void ParameterAdjust::SetGetTimeoutFlag(int nBox,char* pcString)
{
	switch(nBox)
	{
	case 0: SetDlgItemText(IDC_PARAMETERADJUST_GETTIMEOUT1,pcString); break;
	case 1: SetDlgItemText(IDC_PARAMETERADJUST_GETTIMEOUT2,pcString); break;
	case 2:	SetDlgItemText(IDC_PARAMETERADJUST_GETTIMEOUT3,pcString); break;
	case 3:	SetDlgItemText(IDC_PARAMETERADJUST_GETTIMEOUT4,pcString); break;
	case 4:	SetDlgItemText(IDC_PARAMETERADJUST_GETTIMEOUT5,pcString); break;
	}
}


void ParameterAdjust::SetGetTimer(int nBox)
{
	if ((nBox >=0) && (nBox < 5))
		m_nGetParameterTimeout[nBox] = 1500;

	m_dlgParent->SetTimer(PARAMETERADJUST_GETPARAMETER_TIMERID,PARAMETERADJUST_RATE,NULL);
}
void ParameterAdjust::ClearGetTimer(int nBox)
{
	if ((nBox >=0) && (nBox < 5))
		m_nGetParameterTimeout[nBox] = 0;
}


void ParameterAdjust::OnBnClickedParameteradjustSend1()
{
	// TODO: Add your control notification handler code here

	char pcBuffer[200];
	char pcValue[200];
	int nSel;

	CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_PARAMETERADJUST_NAMECOMBO1);

	if(!pCombo) return;
	if (!m_dlgParent) return;

	nSel = pCombo->GetCurSel();
	pCombo->GetLBText(nSel,pcBuffer);
	GetDlgItemText(IDC_PARAMETERADJUST_VALUE1,pcValue, 199);
	SendParameterAdjust(pcBuffer,pcValue);
	SetDlgItemText(IDC_PARAMETERADJUST_VALUE1,pcValue);

}
void ParameterAdjust::OnBnClickedParameteradjustSend2()
{
	// TODO: Add your control notification handler code here
	char pcBuffer[200];
	char pcValue[200];
	int nSel;

	CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_PARAMETERADJUST_NAMECOMBO2);

	if(!pCombo) return;
	if (!m_dlgParent) return;

	nSel = pCombo->GetCurSel();
	pCombo->GetLBText(nSel,pcBuffer);
	GetDlgItemText(IDC_PARAMETERADJUST_VALUE2,pcValue, 199);
	SendParameterAdjust(pcBuffer,pcValue);
	SetDlgItemText(IDC_PARAMETERADJUST_VALUE2,pcValue);

}

void ParameterAdjust::OnBnClickedParameteradjustSend3()
{
	// TODO: Add your control notification handler code here
	char pcBuffer[200];
	char pcValue[200];
	int nSel;

	CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_PARAMETERADJUST_NAMECOMBO3);

	if(!pCombo) return;
	if (!m_dlgParent) return;

	nSel = pCombo->GetCurSel();
	pCombo->GetLBText(nSel,pcBuffer);
	GetDlgItemText(IDC_PARAMETERADJUST_VALUE3,pcValue, 199);
	SendParameterAdjust(pcBuffer,pcValue);
	SetDlgItemText(IDC_PARAMETERADJUST_VALUE3,pcValue);

}

void ParameterAdjust::OnBnClickedParameteradjustSend4()
{
	// TODO: Add your control notification handler code here
	char pcBuffer[200];
	char pcValue[200];
	int nSel;

	CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_PARAMETERADJUST_NAMECOMBO4);

	if(!pCombo) return;
	if (!m_dlgParent) return;

	nSel = pCombo->GetCurSel();
	pCombo->GetLBText(nSel,pcBuffer);
	GetDlgItemText(IDC_PARAMETERADJUST_VALUE4,pcValue, 199);
	SendParameterAdjust(pcBuffer,pcValue);
	SetDlgItemText(IDC_PARAMETERADJUST_VALUE4,pcValue);

}

void ParameterAdjust::OnBnClickedParameteradjustSend5()
{
	// TODO: Add your control notification handler code here
	char pcBuffer[200];
	char pcValue[200];
	int nSel;

	CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_PARAMETERADJUST_NAMECOMBO5);

	if(!pCombo) return;
	if (!m_dlgParent) return;

	nSel = pCombo->GetCurSel();
	pCombo->GetLBText(nSel,pcBuffer);
	GetDlgItemText(IDC_PARAMETERADJUST_VALUE5,pcValue, 199);
	SendParameterAdjust(pcBuffer,pcValue);
	SetDlgItemText(IDC_PARAMETERADJUST_VALUE5,pcValue);


}

void	ParameterAdjust::ShowParameter(int nTarget, int nParam, bool bAll)
{

	int i;
	int nSel;
	tParameterDesc*	pParameterDesc; 
	intfloat j;
	char pcBuffer[200];
	char pcValue[200];
	CComboBox* pCombo;
	CScrollBar* pScroll;
	int nTarget2, nParam2;

	static int nIDCComboName[] = { 
		IDC_PARAMETERADJUST_NAMECOMBO1,
		IDC_PARAMETERADJUST_NAMECOMBO2,
		IDC_PARAMETERADJUST_NAMECOMBO3,
		IDC_PARAMETERADJUST_NAMECOMBO4,
		IDC_PARAMETERADJUST_NAMECOMBO5
	};

	static int nIDCValue[] = { 
		IDC_PARAMETERADJUST_VALUE1,
		IDC_PARAMETERADJUST_VALUE2,
		IDC_PARAMETERADJUST_VALUE3,
		IDC_PARAMETERADJUST_VALUE4,
		IDC_PARAMETERADJUST_VALUE5
	};

	static int nIDCScrollName[] = { 
		IDC_PARAMETERADJUST_SLIDER1,
		IDC_PARAMETERADJUST_SLIDER2,
		IDC_PARAMETERADJUST_SLIDER3,
		IDC_PARAMETERADJUST_SLIDER4,
		IDC_PARAMETERADJUST_SLIDER5
	};

	if (!m_dlgParent) return;
	pParameterDesc =  m_dlgParent->GetParameterDesc( m_dlgParent->m_nProductType, nTarget,nParam);
	if(!pParameterDesc) return;
	
	GetParameterValueString(pcValue, pParameterDesc, nTarget, nParam);
	void** pParameterAddress = m_dlgParent->GetParamAddress(nTarget, nParam);
	j.ui = 0;
	if(pParameterAddress != 0)
	{
		j.vp = *pParameterAddress;
	}
	float fNormalize;
	fNormalize = m_dlgParent->NormalizeParameterValue( pParameterDesc, j);
	int nPos = fNormalize*255;

	for(i=0;i<5;i++)
	{
		pCombo = (CComboBox*) GetDlgItem(nIDCComboName[i]);

		if(pCombo)
		{
			pcBuffer[0] = 0;
			nSel = pCombo->GetCurSel();
			if (nSel >= 0)
			{
				pCombo->GetLBText(nSel,pcBuffer);
				if ((strcmp(pcBuffer,pParameterDesc->pName) == 0) || bAll)
				{
					if (bAll)
					{
						pParameterDesc = m_dlgParent->ConvertString2ParameterDesc(m_nProductType,pcBuffer,&nTarget2,&nParam2);
						if(pParameterDesc)
						{
							GetParameterValueString(pcValue, pParameterDesc, nTarget2, nParam2);
						}
						else
						{
							pcValue[0] = 0;
						}


					}
					SetDlgItemText(nIDCValue[i],pcValue);
					m_Slider[i].SetScrollPos(nPos,true);
					ClearGetTimer(i);
					SetGetTimeoutFlag(i,"Received");	// received

				}
			}
		}
	}

	// slider


}
void ParameterAdjust::GetParameterValueString(char* pcValue,tParameterDesc*  pParameterDesc, int nTarget, int nParam)
{
	intfloat j;
	void** pParameterAddress = m_dlgParent->GetParamAddress(nTarget, nParam);

	pcValue[0] = 0;
	if(!pParameterAddress) return;

	j.vp = *pParameterAddress;


	pcValue[0] = 0;
	switch(pParameterDesc->format)
	{
	case eFORMAT_BOOL:
		sprintf(pcValue,"%d\n", j.ui);
		break;
	case eFORMAT_UNSIGNED:
		sprintf(pcValue,"%d\n",  j.ui);
		break;
	case eFORMAT_SIGNED:
		sprintf(pcValue,"%d\n",  j.i);
		break;
	case eFORMAT_FLOAT:
		sprintf(pcValue,"%8.2f\n",  j.f);
		break;
	}
}

void ParameterAdjust::SendParameterAdjustGet(char *pcName)
{
	intfloat nValue;
	int nTarget;
	int nPID;
	int nValueType;
	intfloat jMax;
	intfloat jMin;
	intfloat jDef;
	CButton* pButton;

	

	tParameterDesc* pParameterDesc = m_dlgParent->ConvertString2ParameterDesc(m_dlgParent->m_nProductType,pcName,&nTarget,&nPID);

	if (pParameterDesc)
	{
		m_dlgParent->SendParamEditGet(nTarget,nPID);

	
	}

}
void ParameterAdjust::SetComboSelectToParameter(int nCombo, char* pcParameterDesc) // set a combo box to the id
{
	// nCombo  1..5
	CComboBox* pCombo;
	int nSel;
	//char pcBuffer[200];
	//int nTarget;
	//int nPID;
	//char pcDefault[200];
	//char pcValue[200];
	//char pcDetails[200];

	int nIDC[] = {
			IDC_PARAMETERADJUST_NAMECOMBO1,
			IDC_PARAMETERADJUST_NAMECOMBO2,
			IDC_PARAMETERADJUST_NAMECOMBO3,
			IDC_PARAMETERADJUST_NAMECOMBO4,
			IDC_PARAMETERADJUST_NAMECOMBO5,
	};


	if ((nCombo < 1) || (nCombo > 5)) return;
	pCombo = (CComboBox*) GetDlgItem(nIDC[nCombo-1]);
	if (!pCombo) 
		return;


	nSel = pCombo->SelectString(0, (LPCTSTR) pcParameterDesc);
	OnCbnSelchangeParameteradjustName(nCombo);
	//pCombo->
/*
	nSel = pCombo->GetCurSel();
	pCombo->GetLBText(nSel,pcBuffer);
	tParameterDesc* pParameterDesc = m_dlgParent->ConvertString2ParameterDesc(m_dlgParent->m_nProductType,pcBuffer,&nTarget,&nPID);
	GetParameterDescDetailString(pParameterDesc ,pcDetails,pcDefault);
	GetParameterValueString(pcValue, pParameterDesc, nTarget, nPID);

	SetDlgItemText(IDC_PARAMETERADJUST_DETAILS5,pcDetails);
	if (pcValue[0] == 0)
	{
		SetDlgItemText(IDC_PARAMETERADJUST_VALUE5,pcDefault);
	}
	else
	{
		SetDlgItemText(IDC_PARAMETERADJUST_VALUE5,pcValue);
	}
	SendParameterAdjustGet(pcBuffer);
*/

}

void ParameterAdjust::SendParameterAdjust(char *pcName, char* pcValue)
{
	intfloat nValue;
	int nTarget;
	int nPID;
	int nValueType;
	intfloat jMax;
	intfloat jMin;
	intfloat jDef;

	tParameterDesc* pParameterDesc = m_dlgParent->ConvertString2ParameterDesc(m_nProductType,pcName,&nTarget,&nPID);

	if (pParameterDesc)
	{
		jMax.ui = pParameterDesc->maxValue;
		jMin.ui = pParameterDesc->minValue;
		jDef.ui = pParameterDesc->defValue;


		nValue = m_dlgParent->ConvertString2Value(pcValue,nValueType);

		switch(pParameterDesc->format)
		{
		case eFORMAT_FLOAT:
			if (nValueType == 0) 
				nValue.f = nValue.i;
			if(nValue.f < jMin.f) nValue.f = jMin.f;
			if(nValue.f > jMax.f) nValue.f = jMax.f;
			sprintf(pcValue,"%f",nValue.f);
			m_dlgParent->SendParamEdit(nTarget,nPID,nValue.ui);
			break;
		case eFORMAT_SIGNED:

			if (nValueType == 1)
				nValue.i = nValue.f;
			if(nValue.i < jMin.i) nValue.i = jMin.i;
			if(nValue.i > jMax.i) nValue.i = jMax.i;
			sprintf(pcValue,"%d",nValue.i);

			 m_dlgParent->SendParamEdit(nTarget,nPID,nValue.ui);
			break;

		case eFORMAT_UNSIGNED:
		case eFORMAT_BOOL:
			if (nValueType == 1)
				nValue.i = nValue.f;
			if(nValue.ui < jMin.ui) nValue.ui = jMin.ui;
			if(nValue.ui > jMax.ui) nValue.ui = jMax.ui;
			sprintf(pcValue,"%u",nValue.ui);

			 m_dlgParent->SendParamEdit(nTarget,nPID,nValue.ui);
			break;
		default:
			break;

		}
		void** pParameterAddress = m_dlgParent->GetParamAddress(nTarget, nPID);
		if(!pParameterAddress) return;
		*pParameterAddress = nValue.vp;

		m_dlgParent->ShowParameter(nTarget,nPID,true);
	}

}
void ParameterAdjust::GetParameterDescDetailString(tParameterDesc* pParameterDesc ,char* pcDetails, char* pcDefault)
{
	intfloat jMax;
	intfloat jMin;
	intfloat jDef;

	pcDetails[0] = 0;
	if (pParameterDesc)
	{
		jMax.ui = pParameterDesc->maxValue;
		jMin.ui = pParameterDesc->minValue;
		jDef.ui = pParameterDesc->defValue;

		switch (pParameterDesc->format)
		{
		case eFORMAT_BOOL:
			sprintf(pcDetails,"type=BOOL, min=0 (false), max=1 (true), def=%s",(jDef.ui)?"true":"false");
			sprintf(pcDefault,"%d",jDef.i);
			break;
			break;
		case eFORMAT_UNSIGNED:
			sprintf(pcDetails,"type=UNSIGNED, min=%u, max=%u, def=%u", jMin.ui, jMax.ui,jDef.ui);
			sprintf(pcDefault,"%u",jDef.ui);
			break;
		case eFORMAT_SIGNED:
			sprintf(pcDetails,"type=SIGNED, min=%d, max=%d, def=%d", jMin.i, jMax.i,jDef.i);
			sprintf(pcDefault,"%d",jDef.i);
			break;
		case eFORMAT_FLOAT:
			int nScaleType = m_dlgParent->GetParameterScaleType(pParameterDesc);
			if (nScaleType == SCALETYPE_LOG)
			{
				sprintf(pcDetails,"type=LOG, min=%f, max=%f, def=%f", jMin.f, jMax.f,jDef.f);
			}
			else
			{
				sprintf(pcDetails,"type=FLOAT, min=%f, max=%f, def=%f", jMin.f, jMax.f,jDef.f);
			}
				sprintf(pcDefault,"%5.2f",jDef.f);

			break;

		}
	}

}
void ParameterAdjust::AdjustComboByOffset(int nOffset)
{
	// nOffset is pos or neg
	CComboBox* pCombo;
	int nSel;
	int nMax;
	int i;

	int nIDCName[] = {
			IDC_PARAMETERADJUST_NAMECOMBO1,
			IDC_PARAMETERADJUST_NAMECOMBO2,
			IDC_PARAMETERADJUST_NAMECOMBO3,
			IDC_PARAMETERADJUST_NAMECOMBO4,
			IDC_PARAMETERADJUST_NAMECOMBO5,
	};

	for(i=0;i<5;i++)
	{	
		pCombo = (CComboBox*) GetDlgItem(nIDCName[i]);
		if(pCombo)
		{
			nSel = pCombo->GetCurSel();
			nSel += nOffset;
			nMax = pCombo->GetCount();
			if (nSel<0) nSel = 0;
			if (nSel >=  pCombo->GetCount())
			{
				nSel = nMax-1;
				if (nSel<0) nSel = 0;
			}
			pCombo->SetCurSel(nSel);
			OnCbnSelchangeParameteradjustName(i+1);

		}

	}

}


void ParameterAdjust::OnBnClickedParameteradjustPageup()
{
	// TODO: Add your control notification handler code here
	AdjustComboByOffset(-5);
}

void ParameterAdjust::OnBnClickedParameteradjustSingleup()
{
	// TODO: Add your control notification handler code here
	AdjustComboByOffset(-1);
}

void ParameterAdjust::OnBnClickedParameteradjustSingledown()
{
	// TODO: Add your control notification handler code here
	AdjustComboByOffset(1);
}

void ParameterAdjust::OnBnClickedParameteradjustPagedown()
{
	// TODO: Add your control notification handler code here
	AdjustComboByOffset(5);
}

void ParameterAdjust::OnCbnSelchangeParameteradjustName(int nCombo)
{
	// nCombo  1..5
	int nTarget;
	int nPID;
	char pcBuffer[200];
	int nSel;
	char pcDetails[200];
	char pcDefault[200];
	char pcValue[200];
	CComboBox* pCombo;

	int nIDCName[] = {
			IDC_PARAMETERADJUST_NAMECOMBO1,
			IDC_PARAMETERADJUST_NAMECOMBO2,
			IDC_PARAMETERADJUST_NAMECOMBO3,
			IDC_PARAMETERADJUST_NAMECOMBO4,
			IDC_PARAMETERADJUST_NAMECOMBO5,
	};
	int nIDCDetails[] = {
			IDC_PARAMETERADJUST_DETAILS1,
			IDC_PARAMETERADJUST_DETAILS2,
			IDC_PARAMETERADJUST_DETAILS3,
			IDC_PARAMETERADJUST_DETAILS4,
			IDC_PARAMETERADJUST_DETAILS5,
	};
	int nIDCValue[] = {
			IDC_PARAMETERADJUST_VALUE1,
			IDC_PARAMETERADJUST_VALUE2,
			IDC_PARAMETERADJUST_VALUE3,
			IDC_PARAMETERADJUST_VALUE4,
			IDC_PARAMETERADJUST_VALUE5,
	};
	if ((nCombo<1) || (nCombo > 5)) return;
	nCombo--;
	
	pCombo = (CComboBox*) GetDlgItem(nIDCName[nCombo]);
	if(!pCombo)
	{
		return;
	}


	
	nSel = pCombo->GetCurSel();
	pCombo->GetLBText(nSel,pcBuffer);
	tParameterDesc* pParameterDesc = m_dlgParent->ConvertString2ParameterDesc(m_dlgParent->m_nProductType,pcBuffer,&nTarget,&nPID);
	GetParameterDescDetailString(pParameterDesc ,pcDetails,pcDefault);
	GetParameterValueString(pcValue, pParameterDesc, nTarget, nPID);

	SetDlgItemText(nIDCDetails[nCombo],pcDetails);
	if (pcValue[0] == 0)
	{
		SetDlgItemText(nIDCValue[nCombo],pcDefault);
	}
	else
	{
		SetDlgItemText(nIDCValue[nCombo],pcValue);
	}
	SendParameterAdjustGet(pcBuffer);

}

void ParameterAdjust::OnCbnSelchangeParameteradjustNamecombo1()
{
	// TODO: Add your control notification handler code here
	OnCbnSelchangeParameteradjustName(1);


}

void ParameterAdjust::OnCbnSelchangeParameteradjustNamecombo2()
{
	// TODO: Add your control notification handler code here
	OnCbnSelchangeParameteradjustName(2);


}

void ParameterAdjust::OnCbnSelchangeParameteradjustNamecombo3()
{
	// TODO: Add your control notification handler code here
	OnCbnSelchangeParameteradjustName(3);


}



void ParameterAdjust::OnCbnSelchangeParameteradjustNamecombo4()
{
	// TODO: Add your control notification handler code here
	OnCbnSelchangeParameteradjustName(4);
	

}

void ParameterAdjust::OnCbnSelchangeParameteradjustNamecombo5()
{
	// TODO: Add your control notification handler code here
	OnCbnSelchangeParameteradjustName(5);
}


void ParameterAdjust::OnBnClickedParameteradjustGet1()
{
	// TODO: Add your control notification handler code here
	char pcBuffer[200];
	char pcValue[200];
	int nSel;

	CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_PARAMETERADJUST_NAMECOMBO1);

	if(!pCombo) return;
	if (!m_dlgParent) return;

	nSel = pCombo->GetCurSel();
	pCombo->GetLBText(nSel,pcBuffer);
	SendParameterAdjustGet(pcBuffer);
	SetGetTimeoutFlag(0,"Getting");
	SetGetTimer(0);

}

void ParameterAdjust::OnBnClickedParameteradjustGet2()
{
	// TODO: Add your control notification handler code here
		char pcBuffer[200];
	char pcValue[200];
	int nSel;

	CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_PARAMETERADJUST_NAMECOMBO2);

	if(!pCombo) return;
	if (!m_dlgParent) return;

	nSel = pCombo->GetCurSel();
	pCombo->GetLBText(nSel,pcBuffer);
	SendParameterAdjustGet(pcBuffer);
	SetGetTimeoutFlag(1,"Getting");
	SetGetTimer(1);

}

void ParameterAdjust::OnBnClickedParameteradjustGet3()
{
	// TODO: Add your control notification handler code here
		char pcBuffer[200];
	char pcValue[200];
	int nSel;

	CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_PARAMETERADJUST_NAMECOMBO3);

	if(!pCombo) return;
	if (!m_dlgParent) return;

	nSel = pCombo->GetCurSel();
	pCombo->GetLBText(nSel,pcBuffer);
	SendParameterAdjustGet(pcBuffer);
	SetGetTimeoutFlag(2,"Getting");
	SetGetTimer(2);

}

void ParameterAdjust::OnBnClickedParameteradjustGet4()
{
	// TODO: Add your control notification handler code here
		char pcBuffer[200];
	char pcValue[200];
	int nSel;

	CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_PARAMETERADJUST_NAMECOMBO4);

	if(!pCombo) return;
	if (!m_dlgParent) return;

	nSel = pCombo->GetCurSel();
	pCombo->GetLBText(nSel,pcBuffer);
	SendParameterAdjustGet(pcBuffer);
	SetGetTimeoutFlag(3,"Getting");
	SetGetTimer(3);

}

void ParameterAdjust::OnBnClickedParameteradjustGet5()
{
	// TODO: Add your control notification handler code here
		char pcBuffer[200];
	char pcValue[200];
	int nSel;

	CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_PARAMETERADJUST_NAMECOMBO5);

	if(!pCombo) return;
	if (!m_dlgParent) return;

	nSel = pCombo->GetCurSel();
	pCombo->GetLBText(nSel,pcBuffer);
	SendParameterAdjustGet(pcBuffer);
	SetGetTimeoutFlag(4,"Getting");
	SetGetTimer(4);

}
	
void ParameterAdjust::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nID = pScrollBar->GetDlgCtrlID();
    int nNewPos = nPos; //((CSliderCtrl *)pScrollBar)->GetPos();
    //CWnd *ACWnd = GetDlgItem(nID);

	static int nIDCComboName[] = { 
		IDC_PARAMETERADJUST_NAMECOMBO1,
		IDC_PARAMETERADJUST_NAMECOMBO2,
		IDC_PARAMETERADJUST_NAMECOMBO3,
		IDC_PARAMETERADJUST_NAMECOMBO4,
		IDC_PARAMETERADJUST_NAMECOMBO5
	};

	int nPanNum = -1;
	switch(nID)
	{
	case IDC_PARAMETERADJUST_SLIDER1: nPanNum = 0; break;
	case IDC_PARAMETERADJUST_SLIDER2: nPanNum = 1; break;
	case IDC_PARAMETERADJUST_SLIDER3: nPanNum = 2; break;
	case IDC_PARAMETERADJUST_SLIDER4: nPanNum = 3; break;
	case IDC_PARAMETERADJUST_SLIDER5: nPanNum = 4; break;
	default:
		return CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	}
	intfloat jMax;
	intfloat jMin;
	intfloat jDef;
	//CButton* pButton;

	int nSel;
	CComboBox* pCombo;
	char pcName[200];
	int nTarget;
	int nPID;
	
	int minpos = 0;
   int maxpos = 255;
   //GetScrollRange(SB_HORZ, &minpos, &maxpos); 
   //maxpos = GetScrollLimit(SB_HORZ);
   // pScrollBar->SetScrollRange(0,255);

   //pScrollBar->GetScrollRange(&minpos,&maxpos);

	SCROLLINFO ScrollInfo;
	pScrollBar->GetScrollInfo(&ScrollInfo);  // get information about the scroll
 
	int theParam = 1;
	float	nNormalize;


	nNewPos = nPos;
	if (nNewPos > 0)
	{
		nNewPos += 0;
	}
	if ((nSBCode == SB_ENDSCROLL) || (nSBCode == SB_THUMBPOSITION))
		return;


 
	if(!m_dlgParent)
		return CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	pCombo = (CComboBox*) GetDlgItem(nIDCComboName[nPanNum]);

	if(!pCombo) return;

	nSel = pCombo->GetCurSel();
	pCombo->GetLBText(nSel,pcName);
	tParameterDesc* pParameterDesc = m_dlgParent->ConvertString2ParameterDesc(m_dlgParent->m_nProductType,pcName,&nTarget,&nPID);
	if (!pParameterDesc)
		return;

	int nFormat = pParameterDesc->format;
		
	jMax.ui = pParameterDesc->maxValue;
	jMin.ui  = pParameterDesc->minValue;

//pScrollBar->SetScrollPos(200);
	nNewPos = nPos;

	int nCurrentPos = pScrollBar->GetScrollPos();
	nNewPos = nPos;



  switch(nSBCode)
  {
    case SB_LEFT:         //Scrolls to the lower right.
		break;

    case SB_ENDSCROLL:      //Ends scroll.
		break;

    case SB_LINELEFT:       //Scrolls down dependant on number type.
			if(nFormat == eFORMAT_BOOL)
			{
				nNewPos = nCurrentPos - 255;
			}
			else if( ((nFormat == eFORMAT_UNSIGNED) || (nFormat == eFORMAT_SIGNED))&& (pParameterDesc->maxValue < 255) )
			{
				nNormalize = (pParameterDesc->maxValue -pParameterDesc->minValue);
				nNewPos = nCurrentPos - 255.0/(nNormalize*1.0);
			}
			else
			{
				nNewPos = nCurrentPos-1;
			}
		break;

    case SB_LINERIGHT:         //Scrolls one line down dependant on number type.
			if(nFormat == eFORMAT_BOOL)
			{
				nNewPos = nCurrentPos + 255;
			}
			else if( ((nFormat == eFORMAT_UNSIGNED) || (nFormat == eFORMAT_SIGNED))&& (pParameterDesc->maxValue < 255) )
			{
				nNormalize = (pParameterDesc->maxValue -pParameterDesc->minValue);

				nNewPos = nCurrentPos + 255.0/(nNormalize*1.0);
			}
			else
			{
				nNewPos = nCurrentPos+1;
			}
		break;

    case SB_PAGELEFT:       //Scrolls one page down dependant on number type.
			if(nFormat == eFORMAT_BOOL)
				nNewPos = nCurrentPos-255;
			else
				nNewPos = nCurrentPos-40;
			break;

	

    case SB_PAGERIGHT:         //Scrolls one page up dependant on number type.
		if(nFormat == eFORMAT_BOOL)
				nNewPos = nCurrentPos+255;
			else
				nNewPos = nCurrentPos+40;
			break;

	

    case SB_THUMBPOSITION:  //The user has dragged the scroll box (thumb) and released the mouse button. The nNewPos parameter indicates the position of the scroll box at the end of the drag operation.
		nNewPos = nPos; //pScrollBar->SetScrollPos(nPos);
		break;

    case SB_THUMBTRACK:     //The user is dragging the scroll box. This message is sent repeatedly until the user releases the mouse button. The nNewPos parameter indicates the position that the scroll box has been dragged to.
		break;

    case SB_RIGHT:            //Scrolls to the upper left.
		break;
  }

  
  	if (nNewPos < 0) nNewPos = 0;
	if (nNewPos > 255) nNewPos = 255;
	pScrollBar->SetScrollPos(nNewPos);

	int	nNewPosition = pScrollBar->GetScrollPos();

	int nScaleType= m_dlgParent->CalcScaleType(pParameterDesc);
	intfloat jVal;

	switch (nFormat)
	{
	case eFORMAT_BOOL:
		if (nNewPosition > 128) 
			jVal.ui = 1;
		else
			jVal.ui = 0;

		break;
	case eFORMAT_UNSIGNED:
		jVal.ui = (nNewPosition/255.0)* (jMax.ui-jMin.ui)*1.0+jMin.ui;

		break;
	case eFORMAT_SIGNED:
		jVal.i = (nNewPosition/255.0)* (jMax.i-jMin.i)*1.0+jMin.i;
		break;
	case eFORMAT_FLOAT:
		if (nScaleType == SCALETYPE_LOG)
		{
			if (jMin.f != 0)
			{
				double df = ( pow ( (float) 10.0, (float) (log10(jMax.f/jMin.f) / (float) 255 )) );
				jVal.f = pow(df,nNewPosition)*jMin.f;
			}
			else
			{
				jVal.f = 0;
			}

		}
		else
		{
			jVal.f = (nNewPosition/255.0)* (jMax.f-jMin.f)*1.0+jMin.f;
		}


		break;

	}

	void** pParameterAddress = m_dlgParent->GetParamAddress(nTarget,nPID);
	if(pParameterAddress)
	{
		*pParameterAddress = jVal.vp;
	}

	m_dlgParent->SendParamEdit(nTarget,nPID, jVal.ui);


	nAdjustedBySlider = nPanNum;
	m_dlgParent->ShowParameter(nTarget,nPID, true);
	nAdjustedBySlider = -1; 

}


