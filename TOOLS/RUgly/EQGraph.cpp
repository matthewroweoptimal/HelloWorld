// EQGraph.cpp : implementation file
//

#include "stdafx.h"
#include "RUgly.h"
#include "EQGraph.h"

#include "RUglyDlg.h"
#include "ProductMap.h"
#include <math.h>
#include "Product_arch.h"

#include "resource.h"
// CEQGraph dialog

IMPLEMENT_DYNAMIC(CEQGraph, CDialog)


CEQGraph::CEQGraph(CWnd* pParent /*=NULL*/)
	: CDialog(CEQGraph::IDD, pParent)
{
	int i,j;
	m_dlgParent = NULL;
	m_nProductType = ePRODUCT_OLYSPEAKER;
	m_nBands = 0;

	for(j=0;j<EQGRAPH_MAX_EDITBOX;j++)
	{
		m_EditBoxDescList[j].nTarget = 0; 
		m_EditBoxDescList[j].nParamNum = 0;
		m_EditBoxDescList[j].nIDC = 0;
	}

	for(j=0;j<EQGRAPH_MAX_BANDS;j++)
	{
		m_ComboBandTypexDescList[j].nTarget = 0; 
		m_ComboBandTypexDescList[j].nParamNum = 0;
		m_ComboBandTypexDescList[j].nIDC = 0;
	}
	for(j=0;j<EQGRAPH_MAX_BANDS;j++)
	{
		m_fLocalType[j] = 3;
		m_fLocalFreq[j] = 1000;
		m_fLocalGain[j] = 0;
		m_fLocalQ[j] = 1;
	}

	m_fFreqMax = 20000;
	m_fFreqMin = 20;
	m_fFreqLogMax = log10(m_fFreqMax);
	m_fFreqLogMin = log10(m_fFreqMin);
	m_fLevelMin = -15;
	m_fLevelMax = 15;

	m_nNumPoints = EQGRAPH_PLOT_NUM_POINTS;
	m_nNumCoeffs = 0;
	for (i=0;i<m_nNumPoints; i++)
	{
		m_fFreqPoints[i] = pow(10,(i*1.0/(m_nNumPoints-1)) *(log10(m_fFreqMax)-log10(m_fFreqMin)) + log10(m_fFreqMin));

		if (m_fFreqPoints[i] > m_fFreqMax) m_fFreqPoints[i] = m_fFreqMax;
		if (m_fFreqPoints[i] < m_fFreqMin) m_fFreqPoints[i] = m_fFreqMin;

	}
	for (i=0;i<m_nNumPoints; i++)
	{
		m_fFreqResponseSum[i] = 0;
		m_fPhaseResponseSum[i] = 20;

	}
	for(j=0;j<EQGRAPH_MAX_BANDS;j++)
	{
		for (i=0;i<m_nNumPoints; i++)
		{
			m_fFreqResponseBand[j][i] = 0;
			m_fPhaseResponseBand[j][i] = 0;
		}
	}

	m_fSampleRate		= 48000;
	m_bCoeffsDirty = false;
	bWindowInitialized = false;

}

CEQGraph::~CEQGraph()
{

	if (bWindowInitialized )
	{
		penAxis.DeleteObject();
		penGrid.DeleteObject();
		penPlot.DeleteObject();
		penPlot2.DeleteObject();
	}
}

void CEQGraph::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEQGraph, CDialog)
	ON_CBN_SELCHANGE(IDC_EQGRAPH_TYPE_BAND1, &CEQGraph::OnCbnSelchangeEqgraphTypeBand1)
	ON_CBN_SELCHANGE(IDC_EQGRAPH_TYPE_BAND2, &CEQGraph::OnCbnSelchangeEqgraphTypeBand2)
	ON_CBN_SELCHANGE(IDC_EQGRAPH_TYPE_BAND3, &CEQGraph::OnCbnSelchangeEqgraphTypeBand3)
	ON_CBN_SELCHANGE(IDC_EQGRAPH_TYPE_BAND4, &CEQGraph::OnCbnSelchangeEqgraphTypeBand4)
	ON_CBN_SELCHANGE(IDC_EQGRAPH_TYPE_BAND5, &CEQGraph::OnCbnSelchangeEqgraphTypeBand5)
	ON_CBN_SELCHANGE(IDC_EQGRAPH_TYPE_BAND6, &CEQGraph::OnCbnSelchangeEqgraphTypeBand6)
	ON_CBN_SELCHANGE(IDC_EQGRAPH_TYPE_BAND7, &CEQGraph::OnCbnSelchangeEqgraphTypeBand7)
	ON_CBN_SELCHANGE(IDC_EQGRAPH_TYPE_BAND8, &CEQGraph::OnCbnSelchangeEqgraphTypeBand8)
	ON_CBN_SELCHANGE(IDC_EQGRAPH_TYPE_BAND9, &CEQGraph::OnCbnSelchangeEqgraphTypeBand9)
	ON_CBN_SELCHANGE(IDC_EQGRAPH_TYPE_BAND10, &CEQGraph::OnCbnSelchangeEqgraphTypeBand10)
	ON_CBN_SELCHANGE(IDC_EQGRAPH_TYPE_BAND11, &CEQGraph::OnCbnSelchangeEqgraphTypeBand11)
	ON_CBN_SELCHANGE(IDC_EQGRAPH_TYPE_BAND12, &CEQGraph::OnCbnSelchangeEqgraphTypeBand12)
	ON_CBN_SELCHANGE(IDC_EQGRAPH_SELECT, &CEQGraph::OnCbnSelchangeEqgraphSelect)
END_MESSAGE_MAP()



// ------------------------------
// EQ types
// ------------------------------
char* EQGraph_EQTypes[] = {
"L-Pass",		//EQBDTYPE_LOW_PASS				( 0 )
"H-Pass",	//EQBDTYPE_HIGH_PASS				( 1 )
"Para",	//EQBDTYPE_PARAMETRIC				( 2 )
"L-Shelf2",	//EQBDTYPE_LOW_SHELF_2_ORDER		( 3 )
"H-Shelf2", //EQBDTYPE_HIGH_SHELF_2_ORDER		( 4 )
"L-Shelf1",	//EQBDTYPE_LOW_SHELF_1_ORDER		( 5 )
"H-Shelf1",	//EQBDTYPE_HIGH_SHELF_1_ORDER		( 6 )
"Notch",		//EQBDTYPE_NOTCH					( 7 )
"B-Pass",	//EQBDTYPE_BAND_PASS				( 8 )
"L-Pass1",	//EQBDTYPE_LOW_PASS_1_ORDER			( 9 )
"H-Pass1", //EQBDTYPE_HIGH_PASS_1_ORDER		( 10 )
"L-ShelfVQ",	//#define	EQBDTYPE_LOW_SHELF_VAR_Q		( 11 )
"H-ShelfVQ",//#define	EQBDTYPE_HIGH_SHELF_VAR_Q		( 12 )
"LinearP",		//#define	EQBDTYPE_LINEAR_PHASE			( 13 )
//EQBDTYPE_MAX					( 14 )
};

int EQGraph_IDC[4*EQGRAPH_MAX_BANDS] = 
{
	IDC_EQGRAPH_TYPE_BAND1,		// must be type, gain, freq q
	IDC_EQGRAPH_GAIN_BAND1,
	IDC_EQGRAPH_FREQ_BAND1,
	IDC_EQGRAPH_Q_BAND1,
	IDC_EQGRAPH_TYPE_BAND2,
	IDC_EQGRAPH_GAIN_BAND2,
	IDC_EQGRAPH_FREQ_BAND2,
	IDC_EQGRAPH_Q_BAND2,
	IDC_EQGRAPH_TYPE_BAND3,
	IDC_EQGRAPH_GAIN_BAND3,
	IDC_EQGRAPH_FREQ_BAND3,
	IDC_EQGRAPH_Q_BAND3,
	IDC_EQGRAPH_TYPE_BAND4,
	IDC_EQGRAPH_GAIN_BAND4,
	IDC_EQGRAPH_FREQ_BAND4,
	IDC_EQGRAPH_Q_BAND4,
	IDC_EQGRAPH_TYPE_BAND5,
	IDC_EQGRAPH_GAIN_BAND5,
	IDC_EQGRAPH_FREQ_BAND5,
	IDC_EQGRAPH_Q_BAND5,
	IDC_EQGRAPH_TYPE_BAND6,
	IDC_EQGRAPH_GAIN_BAND6,
	IDC_EQGRAPH_FREQ_BAND6,
	IDC_EQGRAPH_Q_BAND6,
	IDC_EQGRAPH_TYPE_BAND7,
	IDC_EQGRAPH_GAIN_BAND7,
	IDC_EQGRAPH_FREQ_BAND7,
	IDC_EQGRAPH_Q_BAND7,
	IDC_EQGRAPH_TYPE_BAND8,
	IDC_EQGRAPH_GAIN_BAND8,
	IDC_EQGRAPH_FREQ_BAND8,
	IDC_EQGRAPH_Q_BAND8,
	IDC_EQGRAPH_TYPE_BAND9,
	IDC_EQGRAPH_GAIN_BAND9,
	IDC_EQGRAPH_FREQ_BAND9,
	IDC_EQGRAPH_Q_BAND9,
	IDC_EQGRAPH_TYPE_BAND10,
	IDC_EQGRAPH_GAIN_BAND10,
	IDC_EQGRAPH_FREQ_BAND10,
	IDC_EQGRAPH_Q_BAND10,
	IDC_EQGRAPH_TYPE_BAND11,
	IDC_EQGRAPH_GAIN_BAND11,
	IDC_EQGRAPH_FREQ_BAND11,
	IDC_EQGRAPH_Q_BAND11,
	IDC_EQGRAPH_TYPE_BAND12,
	IDC_EQGRAPH_GAIN_BAND12,
	IDC_EQGRAPH_FREQ_BAND12,
	IDC_EQGRAPH_Q_BAND12,
};
// CEQGraph message handlers
// CEQGraph message handlers
BOOL CEQGraph::OnInitDialog()
{
	CComboBox* pCombo;
	CDialog::OnInitDialog();

	int i;
	int j;
	int nIndex;
	int nNumOfTypes;

	nNumOfTypes = sizeof(EQGraph_EQTypes) /sizeof(char*);

	for(i=0;i< sizeof(EQGraph_IDC)/sizeof(int);i++)
	{
		if ((i%4) == 0) // get type
		{
			pCombo = (CComboBox*) GetDlgItem(EQGraph_IDC[i]);
			if (pCombo)
			{ 
				for(j=0;j<nNumOfTypes;j++)
				{
					nIndex = pCombo->AddString(EQGraph_EQTypes[j]);
					pCombo->SetItemData(nIndex,j);
				}

			}
		}
	}



	SetProductType(m_nProductType);

	//	Colors:
	clrBackground = RGB(255,255,255);		// white
	clrPlot		  = RGB(255,0,0);	 		// red;
	clrAxis		  = RGB(0,0,0);
	clrGrid		  = RGB(40,40,60);
	clrPlot2	  = RGB(0,0,255);	 		// blue;


//	Pens:
	penAxis.CreatePen(PS_SOLID,3,clrAxis);
	penGrid.CreatePen(PS_DASH,1,clrGrid);
	penPlot.CreatePen(PS_SOLID,1,clrPlot);
	penPlot2.CreatePen(PS_SOLID,1,clrPlot2);

	bWindowInitialized = true;


	return true;

}

void CEQGraph::OnTimer(UINT nIDEvent)
{

	switch(nIDEvent)
	{
	case EQGRAPH_COEFFSDIRTY_TIMERID:
		m_dlgParent->KillTimer(EQGRAPH_COEFFSDIRTY_TIMERID);
		ReCalculateAllCoeffs();
		break;
	}

}



void CEQGraph::SetNumofBands(int nBands)
{
	char pcBuffer[200];
	int i,j;
	CWnd* pWnd;
	int nShow;

	m_nBands = nBands;
	sprintf(pcBuffer,"%d",m_nBands);
	SetDlgItemText(IDC_EQGRAPH_NUMOFBANDS,pcBuffer);

	for(i=0;i<EQGRAPH_MAX_BANDS;i++)
	{
		nShow = (i<m_nBands)?SW_SHOW:SW_HIDE;
		for (j=0;j<4;j++)
		{
			pWnd = GetDlgItem(EQGraph_IDC[i*4+j]);
			if (pWnd)
			{
				pWnd->ShowWindow(nShow);
			}
		}
	}
}

void CEQGraph::SetEditBoxControlDesc(char** pList)
{
	int i;
	int j;
	int nSize;
	int nMax;
	nMax = sizeof(m_EditBoxDescList)/sizeof(ControlDesc);
	int nTarget;
	int nPID;
	tParameterDesc* pParameterDesc;

	nSize = 0;
	while((pList[nSize][0] != 0) &&(nSize<4*EQGRAPH_MAX_BANDS))
	{
		nSize++;
	}
	SetNumofBands(nSize/4);
	j = 0;
	for(i=0;i<nSize;i++)
	{
		if ((i % 4) != 0)		// skip the type
		{
			if (j<nMax)
			{
				pParameterDesc = m_dlgParent->ConvertString2ParameterDesc(m_nProductType,pList[i],&nTarget,&nPID);
				m_EditBoxDescList[j].nTarget = nTarget;
				m_EditBoxDescList[j].nParamNum = nPID;
				m_EditBoxDescList[j].nIDC = EQGraph_IDC[i];
				j++;
			}
			else
			{
				break;
			}
		}
	}

	j = 0;
	nMax = sizeof(m_ComboBandTypexDescList)/sizeof(int);
	for(i=0;i<nSize;i++)
	{
		if ((i % 4) == 0)		// skip the type
		{
			if (j<nMax)
			{
				pParameterDesc = m_dlgParent->ConvertString2ParameterDesc(m_nProductType,pList[i],&nTarget,&nPID);
				m_ComboBandTypexDescList[j].nTarget = nTarget;
				m_ComboBandTypexDescList[j].nParamNum = nPID;
				m_ComboBandTypexDescList[j].nIDC = EQGraph_IDC[i];
				j++;
			}
			else
			{
				break;
			}
		}
	}
	ShowAll();

}



// -----------------------------------------------------------------------------
void CEQGraph::SetEQSelectList(EQGRAPH_EQSelectList* pEQSelectList, int nNum)
{
	int i;
	CComboBox* pCombo;
	int nIndex;

	pCombo = (CComboBox*) GetDlgItem(IDC_EQGRAPH_SELECT);
	if (pCombo)
	{
		pCombo->ResetContent();

		for(i=0;i<nNum;i++)
		{
			nIndex =  pCombo->AddString(pEQSelectList[i].pcName);
			pCombo->SetItemDataPtr(nIndex, (void*) pEQSelectList[i].pParameterNameList);
		}
		pCombo->SetCurSel(0);
		OnCbnSelchangeEQSelect();
	}

}


void CEQGraph::SetEQSelect(char* pcName)
{
	CComboBox* pCombo;
	int nSel;

	pCombo = (CComboBox*) GetDlgItem(IDC_EQGRAPH_SELECT);
	if (pCombo)
	{
		nSel = pCombo->SelectString(-1, (LPCTSTR) pcName);
		if (nSel != -1)
		{
			pCombo->SetCurSel(nSel);
			OnCbnSelchangeEQSelect();
		}
	}

}
void CEQGraph::OnCbnSelchangeEQSelect()
{
	CComboBox* pCombo;
	int nSel;
	char** EQSelectParameterList;


	pCombo = (CComboBox*) GetDlgItem(IDC_EQGRAPH_SELECT);
	if (pCombo)
	{
		nSel = pCombo->GetCurSel();
		if (nSel != -1)
		{
			EQSelectParameterList = (char**) pCombo->GetItemDataPtr(nSel);
			if (EQSelectParameterList)
			{
				SetEditBoxControlDesc(EQSelectParameterList);	
				//SetEditBoxControlDesc(EQGraph_OLY_VOICING_HF_PEQ);
			}
		}
	}

}

void CEQGraph::SetProductType(int nProductType)
{

	switch(nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		m_dlgParent->m_ProductOLY.GetEQSelectList();
		SetEQSelectList(m_dlgParent->m_ProductOLY.GetEQSelectList(),m_dlgParent->m_ProductOLY.GetEQSelectListSize());
		break;
	}
	//SetEditBoxControlDesc(EQGraph_OLY_VOICING_HF_PEQ);	// go until empty string
	ShowAll();
	//CComboBox* pCombo[5];
	//int nIDC[] = {
	//		IDC_CEQGraph_NAMECOMBO1,
	//		IDC_CEQGraph_NAMECOMBO2,
	//		IDC_CEQGraph_NAMECOMBO3,
	//		IDC_CEQGraph_NAMECOMBO4,
	//		IDC_CEQGraph_NAMECOMBO5,
	//};

	//int i;

	//tParameterDesc* pParameterDesc;


	//int nTableLength;
	//int nXMLVersion;

	//m_nProductType = nProductType;
	//for (i=0;i<5;i++)
	//{
	//	pCombo[i] = (CComboBox*) GetDlgItem(nIDC[i]);
	//}


	//for (i=0;i<5;i++)
	//{
	//	if (pCombo[i]) pCombo[i]->ResetContent();
	//}

	//switch(m_nProductType)
	//{
	//case ePRODUCT_OLYSPEAKER:
	//	m_dlgParent->CEQGraphSetComboBox(m_nProductType,pCombo,5);
	//	break;
	//};


}

void CEQGraph::OnOK()
{
	CWnd* pwndCtrl = GetFocus();
	if (!pwndCtrl) 
		return;
	int ctrl_ID = pwndCtrl->GetDlgCtrlID();

	int i=0;
	char pcText[200];
	while(i<3*EQGRAPH_MAX_BANDS)
	{
		if (ctrl_ID == m_EditBoxDescList[i].nIDC)
		{
			GetDlgItemText(ctrl_ID,pcText,200);
			SendParameterAdjust(m_EditBoxDescList[i].nTarget, m_EditBoxDescList[i].nParamNum, pcText);
			break;
		}
		i++;
	}
}
void CEQGraph::ShowParameter(int nTarget, int nParam, bool bAll)
{
	int i;
	CButton* pButton;
	tParameterDesc*	pParameterDesc;
	void** pParameterAddress;
	intfloat jValue;
	char pcValue[200];
	CComboBox* pCombo;

	if (m_dlgParent == 0) return;

	if (bAll)
	{
		ShowAll();
		return;
	}
	// --------------------------------------------
	// update text boxes
	// --------------------------------------------
	for(i=0;i<EQGRAPH_MAX_EDITBOX;i++)
	{
		if ((nTarget == m_EditBoxDescList[i].nTarget) && (nParam == m_EditBoxDescList[i].nParamNum))
		{
			pParameterDesc =  m_dlgParent->GetParameterDesc( m_dlgParent->m_nProductType, nTarget,nParam);
			if(pParameterDesc)
			{
				GetParameterValueString(pcValue, pParameterDesc, nTarget, nParam);
				SetDlgItemText(m_EditBoxDescList[i].nIDC,pcValue);
			}
			else
			{
				SetDlgItemText(m_EditBoxDescList[i].nIDC,"");
			}
		}
	}

	for(i=0;i<EQGRAPH_MAX_BANDS;i++)
	{
		if ((nTarget == m_ComboBandTypexDescList[i].nTarget) && (nParam == m_ComboBandTypexDescList[i].nParamNum))
		{
			pCombo = (CComboBox*) GetDlgItem(m_ComboBandTypexDescList[i].nIDC);

			if (pCombo)
			{
				pParameterDesc =  m_dlgParent->GetParameterDesc( m_dlgParent->m_nProductType, m_ComboBandTypexDescList[i].nTarget, m_ComboBandTypexDescList[i].nParamNum);
				pParameterAddress = m_dlgParent->GetParamAddress(m_ComboBandTypexDescList[i].nTarget, m_ComboBandTypexDescList[i].nParamNum);

				jValue.ui = 0;
				if (pParameterDesc && pParameterAddress)
				{
					jValue.vp = *pParameterAddress;
					pCombo->SetCurSel(jValue.ui);
				}
				else
				{
					pCombo->SetCurSel(2);

				}
	
			}
		}
	}
	UpdateLocalAll();


}
void CEQGraph::ShowAll()  // goes thru the Parameter Desc list, converts to strings and updates the edit boxes
{
	int i;
	tParameterDesc*	pParameterDesc;
	void** pParameterAddress;
	//intfloat jValue;
	char pcValue[200];
	CComboBox* pCombo;
	intfloat jValue;

	for(i=0;i<EQGRAPH_MAX_EDITBOX;i++)
	{
		if ((0 != m_EditBoxDescList[i].nTarget) && (0 != m_EditBoxDescList[i].nParamNum))
		{
			pParameterDesc =  m_dlgParent->GetParameterDesc( m_dlgParent->m_nProductType, m_EditBoxDescList[i].nTarget, m_EditBoxDescList[i].nParamNum);
			if(pParameterDesc)
			{
				GetParameterValueString(pcValue, pParameterDesc, m_EditBoxDescList[i].nTarget, m_EditBoxDescList[i].nParamNum);
				SetDlgItemText(m_EditBoxDescList[i].nIDC,pcValue);
			}
			else
			{
				SetDlgItemText(m_EditBoxDescList[i].nIDC,"");
			}
		}
	}

	for(i=0;i<EQGRAPH_MAX_BANDS;i++)
	{
		if ((0 != m_ComboBandTypexDescList[i].nTarget) && (0 != m_ComboBandTypexDescList[i].nParamNum))
		{
			pCombo = (CComboBox*) GetDlgItem(m_ComboBandTypexDescList[i].nIDC);

			if (pCombo)
			{
				pParameterDesc =  m_dlgParent->GetParameterDesc( m_dlgParent->m_nProductType, m_ComboBandTypexDescList[i].nTarget, m_ComboBandTypexDescList[i].nParamNum);
				pParameterAddress = m_dlgParent->GetParamAddress(m_ComboBandTypexDescList[i].nTarget, m_ComboBandTypexDescList[i].nParamNum);

				jValue.ui = 0;
				if (pParameterDesc && pParameterAddress)
				{
					jValue.vp = *pParameterAddress;
					pCombo->SetCurSel(jValue.ui);
				}
				else
				{
					pCombo->SetCurSel(2);

				}
	
			}
		}
	}

	UpdateLocalAll();

}
void CEQGraph::UpdateLocalAll()  // updates the local varaibles
{
	int i;
	tParameterDesc*	pParameterDesc;
	void** pParameterAddress;
	//intfloat jValue;
	char pcValue[200];
	CComboBox* pCombo;
	intfloat jValue;
	int nType;
	int  nBand;
	int nSel;

	for(i=0;i<sizeof(EQGraph_IDC)/sizeof(int);i++)
	{
		nBand = i/4;
		if (nBand >= EQGRAPH_MAX_BANDS)
			break;
		if ( (i%4) != 0) 
		{
			GetDlgItemText(EQGraph_IDC[i],pcValue,200);
			jValue = m_dlgParent->ConvertString2Value(pcValue, nType);	
			if (nType == 0) 
				jValue.f = jValue.i;		// convert float
			switch(i % 4)
			{
			case 0:
				break;
			case 1: // gain
				m_fLocalGain[nBand] =	jValue.f;
				break;
			case 2: // Freq
				m_fLocalFreq[nBand] =	jValue.f;
				break;
			case 3: // Q
				m_fLocalQ[nBand] =	jValue.f;
				break;
			default:
				break;
			}
		}
		else if ( (i%4) == 0) 
		{
			pCombo = (CComboBox*) GetDlgItem(EQGraph_IDC[i]);
			m_fLocalType[nBand] = 3;

			if (pCombo)
			{
				nSel = pCombo->GetCurSel();
				jValue.i = pCombo->GetItemData(nSel);
				m_fLocalType[nBand] = jValue.i;

			}
		}
	}
	SetCoeffsDirty();

}

void CEQGraph::SetCoeffsDirty()
{
	m_bCoeffsDirty = true;
	m_dlgParent->SetTimer(EQGRAPH_COEFFSDIRTY_TIMERID,500,NULL);
}

void CEQGraph::ReCalculateAllCoeffs()
{	
	int i,j;
	m_bCoeffsDirty = false;
	int nNumCoeffs;
	int nTotalNumCoeffs = 0;
	float* pfCoeffs;

	// initialize the coeffs

	for (j=0;j<EQGRAPH_MAX_BANDS;j++)
	{
		m_fCoeffs[j*5+COEFF_B0] = 1.0f;
		m_fCoeffs[j*5+COEFF_B1] = 0.0f;
		m_fCoeffs[j*5+COEFF_B2] = 0.0f;
		m_fCoeffs[j*5+COEFF_A1] = 0.0f;
		m_fCoeffs[j*5+COEFF_A2] = 0.0f;
	}

	for(i=0;i<m_nBands;i++)
	{
		CalculateEQCoeffs(m_fLocalType[i],m_fLocalFreq[i],m_fLocalGain[i],m_fLocalQ[i],&nNumCoeffs,&pfCoeffs);
		if (nNumCoeffs == 5)
		{
			for(j=0;j<nNumCoeffs;j++)
			{
				m_fCoeffs[i*5+j] = pfCoeffs[j];
			}
			nTotalNumCoeffs += nNumCoeffs;
		}
	}
	m_nNumCoeffs =	nTotalNumCoeffs; 
	CalculatePlots();
}
void CEQGraph::CalculatePlots()
{
	int i;
	FreqPhaseZBiquads(m_fCoeffs, m_nNumCoeffs/5, m_fSampleRate, m_fFreqPoints, m_nNumPoints, m_fFreqResponseSum,m_fPhaseResponseSum);
	for(i=0;i<m_nBands;i++)
	{
		FreqPhaseZBiquads(&m_fCoeffs[i*5], 1, m_fSampleRate, m_fFreqPoints, m_nNumPoints, m_fFreqResponseBand[i],m_fPhaseResponseBand[i]);
	}

	DrawPlots(IDC_EQGRAPH_PLOT);
}

void	CEQGraph::DrawPlots	(int nIDC)
{

	// theLevel ..METER_DISPLAY_MAX..METER_DISPLAY_MIN
	CWnd* pWnd;
	CRect rectPlot;
	CDC* pDC;
	int i;

	COLORREF cref[10];




	pWnd = GetDlgItem(nIDC);
	if (!pWnd) return;
	pWnd->GetClientRect(rectPlot);

	pDC = pWnd->GetDC();
	DrawGraphBackground(pDC,&rectPlot);


	for(i=0;i<m_nBands;i++)
	{
		DrawFreqResponse(pDC, &rectPlot, m_nNumPoints, m_fFreqPoints, m_fFreqResponseBand[i],PS_SOLID, 1, RGB(100,200,100));
	}
	DrawFreqResponse(pDC, &rectPlot, m_nNumPoints, m_fFreqPoints, m_fFreqResponseSum,PS_SOLID, 2, RGB(255,0,0));


	ReleaseDC(pDC);


/*
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
*/
}
void CEQGraph::DrawGraphBackground(CDC * dc, CRect* pRectPlot)
{
	CPen		*pOldPen;
	CPoint		pnt1,pnt2;
	float		x,y,yStep,xStep;
	int			i;
	char		szBuff[80];
	TEXTMETRIC	tm;
	BOOL		blPrintedDecade;
	CRect rectPlot;

	//float minLevel = m_fLevelMin;
	//float maxLevel = m_fLevelMax;
	//float maxFreq = m_fFreqMax;
	//float minFreq = m_fFreqMin;



	rectPlot = *pRectPlot;


	dc->GetTextMetrics(&tm);

// 	Draw background:

	dc->FillSolidRect( rectPlot, clrBackground );
// 	Draw axis:
	pOldPen = dc->SelectObject(&penAxis);
	pnt1 = rectPlot.TopLeft();
	pnt2 = rectPlot.BottomRight();
	pnt1.y = pnt2.y;					// pnt1 is the origin
	dc->MoveTo(pnt1);
	dc->LineTo(rectPlot.TopLeft());
	dc->MoveTo(pnt1);
	dc->LineTo(rectPlot.BottomRight());

//	Draw vertical grid:
	if (m_fLevelMax != m_fLevelMin)
	{
		dc->SelectObject(&penGrid);

		pnt1 = rectPlot.TopLeft();				// only x co-ords are used
		pnt2 = rectPlot.BottomRight();

		yStep = 5; //GetGridStep(minLevel,maxLevel);
//		x = floor(maxLevel/yStep) * yStep;		// max vertical grid level
//		y = floor(minLevel/yStep + 1.0) *yStep;	// min "
		x = m_fLevelMax;
		y = m_fLevelMin;


		while (y <= x)
		{
			pnt1.y = pnt2.y = rectPlot.top + nConvertYValue(y,m_fLevelMin,m_fLevelMax,rectPlot.Height());
			dc->MoveTo(pnt1);
			dc->LineTo(pnt2);

			sprintf(szBuff,"%3.3g",y);
			dc->SetBkMode(TRANSPARENT );
			dc->TextOut(pnt1.x-(strlen(szBuff)*tm.tmAveCharWidth),pnt1.y-(tm.tmHeight)/2,szBuff);


			y+= yStep;
		}

	}

//	Draw horizontal grid:
	if (m_fFreqMax != m_fFreqMin)
	{
		dc->SelectObject(&penGrid);

		pnt1 = rectPlot.TopLeft();				// only y co-ords are used
		pnt2 = rectPlot.BottomRight();
			x = floor (log10(m_fFreqMin));				// starting decade
			x = pow (10,x);							// starting frequency

			while (x < m_fFreqMax)
			{
				xStep = x;
				blPrintedDecade = FALSE;
				for (i=0;(i<9) && (x<m_fFreqMax);++i)
				{
					pnt1.x = pnt2.x = rectPlot.left + nConvertLogXValue( log10(x), m_fFreqLogMin, m_fFreqLogMax, rectPlot.Width());
					
					if (pnt1.x > 0)				// watch for out of range
					{
						dc->MoveTo(pnt1);
						dc->LineTo(pnt2);
						if ((blPrintedDecade == FALSE))
						{
							sprintf(szBuff,"%d",(int) x);
							dc->SetBkMode(TRANSPARENT );
							dc->TextOut(pnt2.x-(strlen(szBuff)*tm.tmAveCharWidth)/2,pnt2.y+5,szBuff);
							blPrintedDecade = TRUE;
						}

					}
					x +=xStep;
				}
			}
		
	}

	dc->SelectObject(pOldPen);

}

void	CEQGraph::DrawFreqResponse(CDC * dc,CRect* pRectPlot, int nNumPoints, float* fFreqPoints, float* fFreqResponse,int nPenStyle, int nWidth, COLORREF crColor)
{
	CPen  penFreq;
	CPen		*pOldPen;
	CPoint		pnt1,pnt2;
	int			i;
	float		x,y;
	BOOL		blPlotStarted = FALSE;
	CRect		rectPlot;
	//float minLevel = m_fLevelMin;
	//float maxLevel = m_fLevelMax;
	//float maxFreq = m_fFreqMax;
	//float minFreq = m_fFreqMin;


	rectPlot = *pRectPlot;

	penFreq.CreatePen(nPenStyle,nWidth,crColor);
	pOldPen = dc->SelectObject(&penFreq);

	for (i=0;i<nNumPoints;++i)
	{
		x = fFreqPoints[i];
		y = fFreqResponse[i];


		pnt2.x = rectPlot.left + nConvertLogXValue( log10(x), m_fFreqLogMin, m_fFreqLogMax, rectPlot.Width());

		pnt2.y = rectPlot.top + nConvertYValue(y,m_fLevelMin,m_fLevelMax,rectPlot.Height()); 
		if ((pnt2.x>0) && (pnt2.y>0))
		{
			if (blPlotStarted == FALSE)
				dc->MoveTo(pnt2);
			else
				dc->LineTo(pnt2);
			blPlotStarted = TRUE;

		}

	}
	dc->SelectObject(&pOldPen);

	penFreq.DeleteObject();
}


int	CEQGraph::nConvertLogXValue(float xLog, float minLog, float maxLog, int nWidth)
{
	if (xLog < minLog)  xLog = minLog;
	if (xLog > maxLog)  xLog = maxLog;

	return ((int) ((float) nWidth * (xLog - minLog) / (maxLog - minLog)));
}


int	CEQGraph::nConvertYValue(float y, float minLevel, float maxLevel, int nHeight)
// returns negative if outside of graph area.
{
	CPoint	pnt;
	float x;

	if ((y < minLevel) || (y > maxLevel))
		return -1;

	x = (y-minLevel)/(maxLevel - minLevel);

	return ((int) nHeight - (x * (float) nHeight));
}

int	CEQGraph::CalculateEQCoeffs(int nEQType, float fFreq, float fGain, float fQ, int* pnCoeffsNum, float** ppfCoeffs)
{	 
	int j;
	// *ppfCoeffs = b0,b1,b2,a1,a2, etc

	*pnCoeffsNum = 5;

	switch(nEQType)
	{
		case EQBDTYPE_PARAMETRIC			:
			*pnCoeffsNum = m_effectPEQ.SetParams(fFreq, fGain, fQ, m_fSampleRate);
			*ppfCoeffs = m_effectPEQ.GetData();
			break;

		case EQBDTYPE_LOW_SHELF_2_ORDER		:
			*pnCoeffsNum = m_effectShelf.SetParams(fFreq,fGain,m_effectShelf.nEQType::EQ_12DBLOSHELF, m_fSampleRate);
			*ppfCoeffs = m_effectShelf.GetData();
			break;

		case EQBDTYPE_HIGH_SHELF_2_ORDER	:
			*pnCoeffsNum = m_effectShelf.SetParams(fFreq,fGain,m_effectShelf.nEQType::EQ_12DBHISHELF, m_fSampleRate);
			*ppfCoeffs = m_effectShelf.GetData();
			break;
		case EQBDTYPE_LOW_SHELF_1_ORDER		:
			*pnCoeffsNum = m_effectShelf.SetParams(fFreq,fGain,m_effectShelf.nEQType::EQ_6DBLOSHELF, m_fSampleRate);
			*ppfCoeffs = m_effectShelf.GetData();
			break;

		case EQBDTYPE_HIGH_SHELF_1_ORDER	:
			*pnCoeffsNum = m_effectShelf.SetParams(fFreq,fGain,m_effectShelf.nEQType::EQ_6DBHISHELF, m_fSampleRate);
			*ppfCoeffs = m_effectShelf.GetData();
			break;


		case EQBDTYPE_LOW_PASS				:	//2nd order
//			*pnCoeffsNum = m_effectLowPass24dB.SetParams(fFreq,CEffect::BW12); 
//			*ppfCoeffs = m_effectLowPass24dB.GetData();
			*pnCoeffsNum = m_effectLowPass.SetParams(fFreq,CEffect::BW12, m_fSampleRate); 
			*ppfCoeffs = m_effectLowPass.GetData();
			break;

		case EQBDTYPE_HIGH_PASS				:
//			*pnCoeffsNum = m_effectHighPass24dB.SetParams(fFreq,CEffect::BW12); 
//			*ppfCoeffs = m_effectLowPass24dB.GetData();
			*pnCoeffsNum = m_effectHighPass.SetParams(fFreq,CEffect::BW12, m_fSampleRate); 
			*ppfCoeffs = m_effectHighPass.GetData();
			break;

//			case EQBDTYPE_LOW_PASS_1_ORDER		:
//				*pnCoeffsNum = m_effectLowPass24dB.SetParams(fFreq,CEffect::dB6, m_fSampleRate); 
//				*ppfCoeffs = m_effectLowPass24dB.GetData(); 
//				break;

//			case EQBDTYPE_HIGH_PASS_1_ORDER		:
//				*pnCoeffsNum = m_effectHighPass24dB.SetParams(fFreq,CEffect::dB6, m_fSampleRate); 
//				*ppfCoeffs = m_effectLowPass24dB.GetData(); 
//				break;
		
//			case EQBDTYPE_LOW_SHELF_VAR_Q		:
//			case EQBDTYPE_HIGH_SHELF_VAR_Q		: 
//			case EQBDTYPE_NOTCH					:
//			case EQBDTYPE_BAND_PASS				:
		default:
			*pnCoeffsNum = 0;
			break;

	}
	return *pnCoeffsNum;

}

void CEQGraph::GetParameterValueString(char* pcValue,tParameterDesc*  pParameterDesc, int nTarget, int nParam)
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
void CEQGraph::SendParameterAdjust(int nTarget, int nPID, char *pcValue)
{
	intfloat nValue;
	int nValueType;
	intfloat jMax;
	intfloat jMin;
	intfloat jDef;


	tParameterDesc* pParameterDesc = m_dlgParent->GetParameterDesc(m_nProductType, nTarget,nPID);

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

void CEQGraph::OnCbnSelchangeEqgraphType(int nBand)
{
	CComboBox* pCombo;
	int i;
	int nSel;
	intfloat nValue;

	i = nBand-1;
	if ((i < 0) || (i >= EQGRAPH_MAX_BANDS ))
		return;
	if ((m_ComboBandTypexDescList[i].nTarget != 0) && (m_ComboBandTypexDescList[i].nParamNum != 0))
	{
		pCombo = (CComboBox*) GetDlgItem(m_ComboBandTypexDescList[i].nIDC);
		nSel = pCombo->GetCurSel();
		nValue.ui = pCombo->GetItemData(nSel);
		m_dlgParent->SendParamEdit(m_ComboBandTypexDescList[i].nTarget ,m_ComboBandTypexDescList[i].nParamNum,nValue.ui);
		void** pParameterAddress = m_dlgParent->GetParamAddress(m_ComboBandTypexDescList[i].nTarget, m_ComboBandTypexDescList[i].nParamNum);
		if(!pParameterAddress) return;
		*pParameterAddress = nValue.vp;

		m_dlgParent->ShowParameter(m_ComboBandTypexDescList[i].nTarget,m_ComboBandTypexDescList[i].nParamNum,true);
	}
}
void CEQGraph::OnCbnSelchangeEqgraphTypeBand1()
{
	// TODO: Add your control notification handler code here
	OnCbnSelchangeEqgraphType(1);
}
void CEQGraph::OnCbnSelchangeEqgraphTypeBand2()
{
	// TODO: Add your control notification handler code here
	OnCbnSelchangeEqgraphType(2);
}
void CEQGraph::OnCbnSelchangeEqgraphTypeBand3()
{
	// TODO: Add your control notification handler code here
	OnCbnSelchangeEqgraphType(3);
}
void CEQGraph::OnCbnSelchangeEqgraphTypeBand4()
{
	// TODO: Add your control notification handler code here
	OnCbnSelchangeEqgraphType(4);
}
void CEQGraph::OnCbnSelchangeEqgraphTypeBand5()
{
	// TODO: Add your control notification handler code here
	OnCbnSelchangeEqgraphType(5);
}
void CEQGraph::OnCbnSelchangeEqgraphTypeBand6()
{
	// TODO: Add your control notification handler code here
	OnCbnSelchangeEqgraphType(6);
}
void CEQGraph::OnCbnSelchangeEqgraphTypeBand7()
{
	// TODO: Add your control notification handler code here
	OnCbnSelchangeEqgraphType(7);
}
void CEQGraph::OnCbnSelchangeEqgraphTypeBand8()
{
	// TODO: Add your control notification handler code here
	OnCbnSelchangeEqgraphType(8);
}
void CEQGraph::OnCbnSelchangeEqgraphTypeBand9()
{
	// TODO: Add your control notification handler code here
	OnCbnSelchangeEqgraphType(9);
}
void CEQGraph::OnCbnSelchangeEqgraphTypeBand10()
{
	// TODO: Add your control notification handler code here
	OnCbnSelchangeEqgraphType(10);
}
void CEQGraph::OnCbnSelchangeEqgraphTypeBand11()
{
	// TODO: Add your control notification handler code here
	OnCbnSelchangeEqgraphType(11);
}
void CEQGraph::OnCbnSelchangeEqgraphTypeBand12()
{
	// TODO: Add your control notification handler code here
	OnCbnSelchangeEqgraphType(12);
}
void CEQGraph::OnCbnSelchangeEqgraphSelect()
{
	// TODO: Add your control notification handler code here
	OnCbnSelchangeEQSelect();
}
