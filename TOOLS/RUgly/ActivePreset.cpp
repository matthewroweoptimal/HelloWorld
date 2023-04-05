// ActivePreset.cpp : implementation file
//

#include "stdafx.h"
#include "RUgly.h"
#include "ActivePreset.h"

#include "RUglyDlg.h"

#include "OLYspeaker1_pids.h"
// CActivePreset dialog

IMPLEMENT_DYNAMIC(CActivePreset, CDialog)

CActivePreset::CActivePreset(CWnd* pParent /*=NULL*/)
	: CDialog(CActivePreset::IDD, pParent)
{

	m_dlgParent = NULL;
	m_nProductType = ePRODUCT_OLYSPEAKER;


}

CActivePreset::~CActivePreset()
{
}

void CActivePreset::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CActivePreset, CDialog)
	ON_CBN_SELCHANGE(IDC_ACTIVEPRESET_PROFILE, &CActivePreset::OnCbnSelchangeActivepresetProfile)
	ON_CBN_SELCHANGE(IDC_ACTIVEPRESET_THROW, &CActivePreset::OnCbnSelchangeActivepresetThrow)
	ON_CBN_SELCHANGE(IDC_ACTIVEPRESET_VOICING, &CActivePreset::OnCbnSelchangeActivepresetVoicing)
END_MESSAGE_MAP()


// CActivePreset message handlers

BOOL CActivePreset::OnInitDialog()
{
	bool bRetVal;
	bRetVal = CDialog::OnInitDialog();
	char pcString[10];
	int i,k;
	int nIndex;

	CComboBox* pCombo;
	int nInstancesList[3] = {
		5, // profile
		3, // throw
		4, // voicing
	};

	ControlDesc nOLYInit[3] = {
	 
		{eTARGET_DEVICE,	ePID_OLYspeaker1_DEVICE_ACTIVE_PROFILE, IDC_ACTIVEPRESET_PROFILE},		// 	
		{eTARGET_DEVICE,	ePID_OLYspeaker1_DEVICE_ACTIVE_THROW, IDC_ACTIVEPRESET_THROW},	// 
		{eTARGET_DEVICE,	ePID_OLYspeaker1_DEVICE_ACTIVE_VOICING, IDC_ACTIVEPRESET_VOICING},		// 	
	};
	//int nIDC[3] =
	//{
	//	IDC_ACTIVEPRESET_PROFILE,
	//	IDC_ACTIVEPRESET_THROW,
	//	IDC_ACTIVEPRESET_VOICING,
	//};
	// initialize the combo box

	for(k=0;k<3;k++)
	{
		m_ComboBox[k] = nOLYInit[k];

		pCombo = (CComboBox*) GetDlgItem(m_ComboBox[k].nIDC);
		if (pCombo)
		{
			//pCombo->ResetContent();
			for(i=0;i<nInstancesList[k];i++)
			{
				sprintf(pcString,"%d",i);
				nIndex =  pCombo->AddString(pcString);
				pCombo->SetItemData(nIndex,i);
			}
			pCombo->SetCurSel(0);
		}
	}

	return bRetVal;
}


void CActivePreset::ShowParameter(int nTarget, int nParam, bool bAll)
{
	int i;
	CComboBox* pCombo;
	int nSel;
	tParameterDesc*	pParameterDesc; 
	intfloat j;
	char pcBuffer[200];
	char pcValue[200];
	//int nTarget2, nParam2;

	if (!m_dlgParent) return;
	pParameterDesc =  m_dlgParent->GetParameterDesc( m_dlgParent->m_nProductType, nTarget,nParam);
	if(!pParameterDesc) return;

	m_dlgParent->GetParameterValueString(pcValue, pParameterDesc, nTarget, nParam);

	for(i=0;i<3;i++)
	{

		if (((nTarget == m_ComboBox[i].nTarget) && (nParam == m_ComboBox[i].nParamNum)) || (bAll))
		{
			if (bAll)
			{
				pParameterDesc =  m_dlgParent->GetParameterDesc( m_dlgParent->m_nProductType, m_ComboBox[i].nTarget,m_ComboBox[i].nParamNum);
				if(pParameterDesc)
				{
					m_dlgParent->GetParameterValueString(pcValue, pParameterDesc, m_ComboBox[i].nTarget,m_ComboBox[i].nParamNum);
				}
				else
				{
					pcValue[0] = 0;
				}

			}
			pCombo = (CComboBox*) GetDlgItem(m_ComboBox[i].nIDC);

			if(pCombo)
			{
				pcBuffer[0] = 0;
				nSel = pCombo->GetCurSel();
				//pCombo->GetLBText(nSel,pcBuffer);
				SetDlgItemText(m_ComboBox[i].nIDC,pcValue);
			}
		}
	}

}

void CActivePreset::SendParameterAdjust(int nTarget, int nPID, char* pcValue)
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

		m_dlgParent->ShowParameter(nTarget,nPID,true, true);
	}

}


void CActivePreset::ChangeComboSelPreset(int nCombo)
{
	CComboBox* pCombo;
	int nSel;
	intfloat jValue;
	char pcBuffer[100];

	pCombo = (CComboBox*) GetDlgItem(m_ComboBox[nCombo].nIDC);

	if(pCombo)
	{
		nSel = pCombo->GetCurSel();
		jValue.i = pCombo->GetItemData(nSel);
		pCombo->GetLBText(nSel,pcBuffer);
		SendParameterAdjust(m_ComboBox[nCombo].nTarget, m_ComboBox[nCombo].nParamNum, pcBuffer);
	}

}



void CActivePreset::OnCbnSelchangeActivepresetProfile()
{
	// TODO: Add your control notification handler code here
	ChangeComboSelPreset(0);
}

void CActivePreset::OnCbnSelchangeActivepresetThrow()
{
	// TODO: Add your control notification handler code here
	ChangeComboSelPreset(1);
}

void CActivePreset::OnCbnSelchangeActivepresetVoicing()
{
	// TODO: Add your control notification handler code here
	ChangeComboSelPreset(2);
}
