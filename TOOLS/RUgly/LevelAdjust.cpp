// LevelAdjust.cpp : implementation file
//

#include "stdafx.h"
#include "RUgly.h"
#include "LevelAdjust.h"

#include "RUglyDlg.h"
#include "ProductMap.h"
#include <math.h>
#include "Product_arch.h"

//namespace OLY
//{
//#include "OLYspeaker1_lookup.h"
//}
//namespace REDLINE 
//{
//#include "REDLINEspeaker1_lookup.h"
//}

// ------------------------------------------

// CLevelAdjust dialog

IMPLEMENT_DYNAMIC(CLevelAdjust, CDialog)

CLevelAdjust::CLevelAdjust(CWnd* pParent /*=NULL*/)
	: CDialog(CLevelAdjust::IDD, pParent)
{
	int j;
	m_dlgParent = NULL;
	m_nProductType = ePRODUCT_OLYSPEAKER;

	m_nFaderAdjustByTouch = -1;
	for(j=0;j<MAX_LEVELADJUST_SWITCH;j++)
	{
		m_descSwitch[j].nTarget = 0; 
		m_descSwitch[j].nParamNum = 0;
		m_descSwitch[j].nIDC = 0;
	}

	for(j=0;j<MAX_LEVELADJUST_FADER;j++)
	{
		m_descFader[j].nTarget = 0; 
		m_descFader[j].nParamNum = 0;
		m_descFader[j].nIDC = 0;
	}

}

CLevelAdjust::~CLevelAdjust()
{
}

void CLevelAdjust::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FADER_INPUTGAIN1, m_Fader[0]);
	DDX_Control(pDX, IDC_FADER_OUTPUTGAIN1, m_Fader[1]);
	DDX_Control(pDX, IDC_FADER_OUTPUTGAIN2, m_Fader[2]);
	DDX_Control(pDX, IDC_FADER_OUTPUTGAIN3, m_Fader[3]);


}


BEGIN_MESSAGE_MAP(CLevelAdjust, CDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_INPUTMUTE1, IDC_CHECK_OUTPUTPHASE3, OnRangeButtonClick)
	ON_WM_VSCROLL()
	ON_EN_KILLFOCUS(IDC_EDIT_INPUTGAIN1, &CLevelAdjust::OnEnKillfocusEditInputgain1)
	ON_EN_KILLFOCUS(IDC_EDIT_OUTPUTGAIN1, &CLevelAdjust::OnEnKillfocusEditOutputgain1)
	ON_EN_KILLFOCUS(IDC_EDIT_OUTPUTGAIN2, &CLevelAdjust::OnEnKillfocusEditOutputgain2)
	ON_EN_KILLFOCUS(IDC_EDIT_OUTPUTGAIN3, &CLevelAdjust::OnEnKillfocusEditOutputgain3)
END_MESSAGE_MAP()



void CLevelAdjust::OnOK()
{

	CWnd* pwndCtrl = GetFocus();
	if (!pwndCtrl) 
		return;
	int ctrl_ID = pwndCtrl->GetDlgCtrlID();

	switch(ctrl_ID)
	{
	case IDC_EDIT_INPUTGAIN1: OnEnKillfocusEditInputgain1(); break;
	case IDC_EDIT_OUTPUTGAIN1: OnEnKillfocusEditOutputgain1(); break;
	case IDC_EDIT_OUTPUTGAIN2: OnEnKillfocusEditOutputgain2(); break;
	case IDC_EDIT_OUTPUTGAIN3: OnEnKillfocusEditOutputgain3(); break;
	}


}

// ParameterAdjust message handlers
BOOL CLevelAdjust::OnInitDialog()
{


	CDialog::OnInitDialog();



	SetupVScroll();

	return true;

}
void CLevelAdjust::SetupVScroll()
{
//	CScrollBar* pScrollBar;

	SCROLLINFO ScrollInfo;
	ScrollInfo.cbSize = sizeof(ScrollInfo);     // size of this structure
	ScrollInfo.fMask = SIF_ALL;                 // parameters to set
	ScrollInfo.nMin = 0;                        // minimum scrolling position
	ScrollInfo.nMax = 255;                      // maximum scrolling position
	ScrollInfo.nPage = 1;                      // the page size of the scroll box
	ScrollInfo.nPos = 255;                       // initial position of the scroll box
	ScrollInfo.nTrackPos = 0;                   // immediate position of a scroll box that the user is dragging

	for(int i=0;i<MAX_LEVELADJUST_FADER;i++)
	{

		m_Fader[i].SetScrollInfo(&ScrollInfo, false);

	}

}

void CLevelAdjust::ShowParameter(int nTarget, int nParam, bool bAll)
{
	int i;
	CButton* pButton;
	tParameterDesc*	pParameterDesc;
	void** pParameterAddress;
		intfloat jValue;
	float fNormalized;

	if (m_dlgParent == 0) return;
	SCROLLINFO ScrollInfo;

	//CScrollBar* pScrollBar;


	// --------------------------------------------
	// update faders
	// --------------------------------------------
	for(i=0;i<MAX_LEVELADJUST_FADER;i++)
	{
		if (((nTarget == m_descFader[i].nTarget) && (nParam == m_descFader[i].nParamNum)) || (bAll))
		{
			pParameterDesc = m_dlgParent->GetParameterDesc(m_nProductType, m_descFader[i].nTarget,m_descFader[i].nParamNum);
			pParameterAddress = m_dlgParent->GetParamAddress(m_descFader[i].nTarget, m_descFader[i].nParamNum);

			jValue.ui = 0;
			if (pParameterDesc && pParameterAddress)
			{
				jValue.vp = *pParameterAddress;
			}
	
				m_Fader[i].GetScrollInfo(&ScrollInfo, false);
			if (m_nFaderAdjustByTouch != i)
			{
				fNormalized = m_dlgParent->NormalizeParameterValue( pParameterDesc, jValue);


/*				
				m_Fader[i].GetScrollInfo(&ScrollInfo, false);
			if (ScrollInfo.nMax != 255)
				{
					ScrollInfo.cbSize = sizeof(ScrollInfo);     // size of this structure
					ScrollInfo.fMask = SIF_ALL;                 // parameters to set
					ScrollInfo.nMin = 0;                        // minimum scrolling position
					ScrollInfo.nMax = 255;                      // maximum scrolling position
					ScrollInfo.nPage = 1;                      // the page size of the scroll box
					ScrollInfo.nPos = 255;                       // initial position of the scroll box
					ScrollInfo.nTrackPos = 0;                   // immediate position of a scroll box that the user is dragging
					m_Fader[i].SetScrollInfo(&ScrollInfo, false);
				}
*/		
				int nPos = 255 - (fNormalized*(255 - 0)+0);
				m_Fader[i].SetScrollPos(nPos,true);

				//if (m_descFader[i].nIDC != 0)
				//{
				//	pScrollBar = (CScrollBar*) GetDlgItem(m_descFader[i].nIDC);
				//	if (pScrollBar)
				//		pScrollBar->SetScrollPos(nPos);
				//}
			}

			// 0..1

			break;
		}
	}

	for(i=0;i<MAX_LEVELADJUST_SWITCH;i++)
	{
		if (((m_descSwitch[i].nTarget != 0) && (m_descSwitch[i].nParamNum != 0) && (m_descSwitch[i].nIDC != 0)) || bAll)
		{
			pButton = (CButton*) GetDlgItem(m_descSwitch[i].nIDC);

			if (pButton)
			{
				pParameterDesc = m_dlgParent->GetParameterDesc(m_nProductType, m_descSwitch[i].nTarget,m_descSwitch[i].nParamNum);
				pParameterAddress = m_dlgParent->GetParamAddress(m_descSwitch[i].nTarget, m_descSwitch[i].nParamNum);

				if (pParameterDesc && pParameterAddress)
				{
					jValue.vp = *pParameterAddress;
					pButton->SetCheck(jValue.ui != 0);

				}


			}
		}

	}


	CString strEdit;
	CEdit* pEditBox;
	char pcValue[200];
	for(i=0;i<MAX_LEVELADJUST_EDITBOX;i++)
	{
		if (((m_descEditBox[i].nTarget != 0) && (m_descEditBox[i].nParamNum != 0) && (m_descEditBox[i].nIDC != 0)) || bAll)
		{

			pEditBox = (CEdit*) GetDlgItem(m_descEditBox[i].nIDC);

			if (pEditBox)
			{
				pParameterDesc = m_dlgParent->GetParameterDesc(m_nProductType, m_descEditBox[i].nTarget,m_descEditBox[i].nParamNum);
				pParameterAddress = m_dlgParent->GetParamAddress(m_descEditBox[i].nTarget, m_descEditBox[i].nParamNum);

				if (pParameterDesc && pParameterAddress)
				{
					jValue.vp = *pParameterAddress;

					pcValue[0] = 0;
					switch(pParameterDesc->format)
					{
					case eFORMAT_BOOL:
						sprintf(pcValue,"%d\n", jValue.ui);
						break;
					case eFORMAT_UNSIGNED:
						sprintf(pcValue,"%d\n",  jValue.ui);
						break;
					case eFORMAT_SIGNED:
						sprintf(pcValue,"%d\n",  jValue.i);
						break;
					case eFORMAT_FLOAT:
						sprintf(pcValue,"%8.2f\n",  jValue.f);
						break;
					}
					SetDlgItemText(m_descEditBox[i].nIDC,pcValue);
				}
			}
		}
	}
}
// CLevelAdjust message handlers


void CLevelAdjust::OnRangeButtonClick(UINT nID)
{
	int i;
	bool bFound = false;
	CButton* pButton;
	intfloat j;

	i = 0;
	while(i<MAX_LEVELADJUST_SWITCH)
	{
		if (m_descSwitch[i].nIDC == nID)
		{
			bFound = true;
			break;
		}
		i++;
	}
	if (bFound)
	{
		pButton = (CButton*) GetDlgItem(nID);
		if (pButton)
		{
			j.i = pButton->GetCheck();
		}
		tParameterDesc* pParameterDesc = m_dlgParent->GetParameterDesc(m_nProductType, m_descSwitch[i].nTarget, m_descSwitch[i].nParamNum);
		if (pParameterDesc)
		{
			void** pParameterAddress = m_dlgParent->GetParamAddress(m_descSwitch[i].nTarget, m_descSwitch[i].nParamNum);
			if(pParameterAddress)
			{
				*pParameterAddress = j.vp;
			}

			m_dlgParent->SendParamEdit(m_descSwitch[i].nTarget, m_descSwitch[i].nParamNum, j.ui);
			m_dlgParent->ShowParameter(m_descSwitch[i].nTarget, m_descSwitch[i].nParamNum, true);

//			m_strResponseStatusDisplay.Format("%s = %d", pParameterDesc->pName, nState);
//			SetDlgItemText(IDC_RESPONSESTATUS,m_strResponseStatusDisplay);
		}
	}

}

void CLevelAdjust::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if ((nSBCode == SB_ENDSCROLL) || (nSBCode == SB_THUMBPOSITION))
		return;

	CScrollBar Fader;
	int nFaderNum;
	int nPosition;
	int nNormalize;
	int nID = pScrollBar->GetDlgCtrlID();
	int i;
	int nNewPos;
	int nCurrentPos;
	nFaderNum = -1;

	if (!m_dlgParent)
		return;

	for(i=0;i<MAX_LEVELADJUST_FADER;i++)
	{
		if (*pScrollBar == m_Fader[i])
		//if (nID == m_descFader[i].nIDC)
		{
			nFaderNum = i;
			break;
		}
	}

	if (nFaderNum < 0) 
	  return;
			
	if (m_nFaderAdjustByTouch == i)
		return;

	if (( m_descFader[nFaderNum].nTarget == 0 ) || (m_descFader[nFaderNum].nParamNum == 0)) 
		return;

	tParameterDesc* pParameterDesc = m_dlgParent->GetParameterDesc(m_nProductType, m_descFader[nFaderNum].nTarget, m_descFader[nFaderNum].nParamNum);
  
	  if (!pParameterDesc) 
		  return;

	int nScaleType = m_dlgParent->GetParameterScaleType(pParameterDesc);
	int nFormat = pParameterDesc->format;
	int nTarget = m_descFader[nFaderNum].nTarget;
	int nParam = m_descFader[nFaderNum].nParamNum;

	SCROLLINFO ScrollInfo;
	pScrollBar->GetScrollInfo(&ScrollInfo);  // get information about the scroll

	//nPos =  pScrollBar->GetScrollPos();
	nCurrentPos =  pScrollBar->GetScrollPos();
	 //nNumericType = gCurrentStateTable[nFaderNum - 1].theNumericType;

	  switch(nSBCode)
	  {
		case SB_BOTTOM:         //Scrolls to the lower right.
			break;

		case SB_ENDSCROLL:      //Ends scroll.
			break;

		case SB_LINEDOWN:       //Scrolls down dependant on number type.
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

		case SB_LINEUP:         //Scrolls one line down dependant on number type.
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

		case SB_PAGEDOWN:       //Scrolls one page down dependant on number type.
			if(nFormat == eFORMAT_BOOL)
				nNewPos = nCurrentPos+255;
			else
				nNewPos = nCurrentPos+40;
			break;

		case SB_PAGEUP:         //Scrolls one page up dependant on number type.
			if(nFormat == eFORMAT_BOOL)
				nNewPos = nCurrentPos-255;
			else
				nNewPos = nCurrentPos-40;
			break;

		case SB_THUMBPOSITION:  //The user has dragged the scroll box (thumb) and released the mouse button. The nPos parameter indicates the position of the scroll box at the end of the drag operation.
			break;

		case SB_THUMBTRACK:     //The user is dragging the scroll box. This message is sent repeatedly until the user releases the mouse button. The nPos parameter indicates the position that the scroll box has been dragged to.
			nNewPos = nPos; //pScrollBar->SetScrollPos(nPos);
			break;

		case SB_TOP:            //Scrolls to the upper left.
			break;
	  }

	
	if (nNewPos < 0) nNewPos = 0;
	if (nNewPos > 255) nNewPos = 255;
	pScrollBar->SetScrollPos(nNewPos);
	nPosition = 255 - pScrollBar->GetScrollPos();
	//OnChangeFader(nFaderNum, nPosition);
	intfloat jMax, jMin, jVal;

	jMax.ui = pParameterDesc->maxValue;
	jMin.ui = pParameterDesc->minValue;

//	bool bSend = false;

	switch (nFormat)
	{
	case eFORMAT_BOOL:
		if (nPosition > 128) 
			jVal.ui = 1;
		else
			jVal.ui = 0;
		//SendParamEdit(nTarget,nParam, jVal.ui);
		//m_strResponseStatusDisplay.Format("%s = %d", pParameterDesc->pName, jVal.ui);
		//SetDlgItemText(IDC_RESPONSESTATUS,m_strResponseStatusDisplay);

		break;
	case eFORMAT_UNSIGNED:
		jVal.ui = (nPosition/255.0)* (jMax.ui-jMin.ui)*1.0+jMin.ui;
		//SendParamEdit(nTarget,nParam, jVal.ui);
		//m_strResponseStatusDisplay.Format("%s = %d", pParameterDesc->pName, jVal.ui);
		//SetDlgItemText(IDC_RESPONSESTATUS,m_strResponseStatusDisplay);

		break;
	case eFORMAT_SIGNED:
		jVal.i = (nPosition/255.0)* (jMax.i-jMin.i)*1.0+jMin.i;
		//SendParamEdit(nTarget,nParam, jVal.ui);
		//m_strResponseStatusDisplay.Format("%s = %d", pParameterDesc->pName, jVal.i);
		//SetDlgItemText(IDC_RESPONSESTATUS,m_strResponseStatusDisplay);

		break;
	case eFORMAT_FLOAT:
		if (nScaleType == SCALETYPE_LOG)
		{
			if (jMin.f != 0)
			{
				double df = ( pow ( (float) 10.0, (float) (log10(jMax.f/jMin.f) / (float) 255 )) );
				jVal.f = pow(df,nPosition)*jMin.f;
			}
			else
			{
				jVal.f = 0;
			}

		}
		else
		{
			jVal.f = (nPosition/255.0)* (jMax.f-jMin.f)*1.0+jMin.f;
		}
		//SendParamEdit(nTarget,nParam, jVal.ui);
		//m_strResponseStatusDisplay.Format("%s = %f", pParameterDesc->pName, jVal.f);
		//SetDlgItemText(IDC_RESPONSESTATUS,m_strResponseStatusDisplay);

		break;

	}
	void** pParameterAddress = m_dlgParent->GetParamAddress(m_descFader[nFaderNum].nTarget, m_descFader[nFaderNum].nParamNum);
	if(pParameterAddress)
	{
		*pParameterAddress = jVal.vp;
	}

	m_dlgParent->SendParamEdit(m_descFader[nFaderNum].nTarget, m_descFader[nFaderNum].nParamNum, jVal.ui);

	m_nFaderAdjustByTouch = nFaderNum;

	m_dlgParent->ShowParameter(m_descFader[nFaderNum].nTarget, m_descFader[nFaderNum].nParamNum, true);

	m_nFaderAdjustByTouch = -1;

}

void CLevelAdjust::SetProductType(int nProductType)
{
	int j = 0;
	CButton* pButton;

	// set the switch states
	m_nProductType = nProductType;
	ControlDesc* pDescList;
	tStringIDC* pTitleList;

	int nLength;


	nLength = m_dlgParent->GetLevelTitleList(m_nProductType,&pTitleList);

	for(j=0;j<nLength;j++)
	{
		SetDlgItemText(pTitleList[j].nIDC, pTitleList[j].str);
	}

	nLength = m_dlgParent->GetLevelFaderDescList(m_nProductType,&pDescList);

	// faders
	for(j=0;j<nLength;j++)
	{
		m_descFader[j].nTarget = pDescList[j].nTarget; 
		m_descFader[j].nParamNum = pDescList[j].nParamNum;
		m_descFader[j].nIDC = pDescList[j].nIDC;
	}
	for(;j<MAX_LEVELADJUST_FADER;j++)
	{
		m_descFader[j].nTarget = 0; 
		m_descFader[j].nParamNum = 0;
		m_descFader[j].nIDC = 0;
	}

	
	nLength = m_dlgParent->GetLevelSwitchDescList(m_nProductType,&pDescList);

	// switches
	for(j=0;j<nLength;j++)
	{
		m_descSwitch[j].nTarget = pDescList[j].nTarget; 
		m_descSwitch[j].nParamNum = pDescList[j].nParamNum;
		m_descSwitch[j].nIDC = pDescList[j].nIDC;
	}
	for(;j<MAX_LEVELADJUST_SWITCH;j++)
	{
		m_descSwitch[j].nTarget = 0; 
		m_descSwitch[j].nParamNum = 0;
		m_descSwitch[j].nIDC = 0;
	}
	

	// Show and hide windows
	for(j=0;j<MAX_LEVELADJUST_FADER;j++)
	{
		if (m_descFader[j].nIDC != 0)
		{
			pButton = (CButton*) GetDlgItem(m_descFader[j].nIDC);

			if (pButton)
			{
				if ((m_descFader[j].nTarget != 0) && (m_descFader[j].nParamNum != 0))
				{
					pButton->ShowWindow(SW_SHOW);
					ShowParameter(m_descFader[j].nTarget, m_descFader[j].nParamNum);
				}
				else
				{
					pButton->ShowWindow(SW_HIDE);
				}

			}
		}
	}

	CWnd* pWnd;
	for(j=0;j<MAX_LEVELADJUST_EDITBOX;j++)
	{
		if (m_descEditBox[j].nIDC != 0)
		{
			pWnd =  GetDlgItem(m_descEditBox[j].nIDC);

			if (pWnd)
			{
				if ((m_descEditBox[j].nTarget != 0) && (m_descEditBox[j].nParamNum != 0))
				{
					pWnd->ShowWindow(SW_SHOW);
					ShowParameter(m_descEditBox[j].nTarget, m_descEditBox[j].nParamNum);
				}
				else
				{
					pWnd->ShowWindow(SW_HIDE);
				}

			}
		}
	}

	for(j=0;j<MAX_LEVELADJUST_SWITCH;j++)
	{
		if (m_descSwitch[j].nIDC != 0)
		{
			pButton = (CButton*) GetDlgItem(m_descSwitch[j].nIDC);

			if (pButton)
			{
				if ((m_descSwitch[j].nTarget != 0) && (m_descSwitch[j].nParamNum != 0))
				{
					pButton->ShowWindow(SW_SHOW);
					ShowParameter(m_descSwitch[j].nTarget, m_descSwitch[j].nParamNum);
				}
				else
				{
					pButton->ShowWindow(SW_HIDE);
				}

			}
		}
	}

}
void CLevelAdjust::SendParameterAdjust(int nTarget, int nPID, char* pcValue)
{
	intfloat nValue;
	int nValueType;
	intfloat jMax;
	intfloat jMin;
	intfloat jDef;

	tParameterDesc* pParameterDesc = m_dlgParent->GetParameterDesc(m_nProductType,nTarget,nPID);
	void** pParameterAddress = m_dlgParent->GetParamAddress(nTarget,nPID);
	if (pParameterDesc && pParameterAddress)
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

void CLevelAdjust::OnEnKillfocusEditInputgain1()
{
	// TODO: Add your control notification handler code here
	char pcValue[300];
	int i = 0;

	if ((m_descEditBox[i].nIDC != 0) && (m_descEditBox[i].nTarget != 0) && (m_descEditBox[i].nParamNum != 0))
	{
		GetDlgItemText(m_descEditBox[i].nIDC,pcValue, 199);
		SendParameterAdjust(m_descEditBox[i].nTarget,m_descEditBox[i].nParamNum, pcValue);
	}

}
void CLevelAdjust::OnEnKillfocusEditOutputgain1()
{
	// TODO: Add your control notification handler code here
	char pcValue[300];
	int i = 1;

	if ((m_descEditBox[i].nIDC != 0) && (m_descEditBox[i].nTarget != 0) && (m_descEditBox[i].nParamNum != 0))
	{
		GetDlgItemText(m_descEditBox[i].nIDC,pcValue, 199);
		SendParameterAdjust(m_descEditBox[i].nTarget,m_descEditBox[i].nParamNum, pcValue);
	}
}
void CLevelAdjust::OnEnKillfocusEditOutputgain2()
{
	// TODO: Add your control notification handler code here
}
void CLevelAdjust::OnEnKillfocusEditOutputgain3()
{
	// TODO: Add your control notification handler code here
}