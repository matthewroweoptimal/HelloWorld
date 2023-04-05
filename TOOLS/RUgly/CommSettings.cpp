// CommSettings.cpp : implementation file
//

#include "stdafx.h"

#include "RUgly.h"
#include "RUglyDlg.h"

#include "CommSettings.h"

#include "comstuff.h"

// CCommSettings dialog

IMPLEMENT_DYNAMIC(CCommSettings, CDialog)

CCommSettings::CCommSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CCommSettings::IDD, pParent)
{


	m_dlgParent = 0;
	m_nCommParity = NOPARITY; // none
	m_nCommBaudRate = CBR_115200;
	m_nCommPort = 1;
}

CCommSettings::~CCommSettings()
{
}

void CCommSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCommSettings, CDialog)
	ON_BN_CLICKED(IDCONNECT, &CCommSettings::OnBnClickedConnect)
END_MESSAGE_MAP()


// CCommSettings message handlers

BOOL CCommSettings::OnInitDialog() 
{
	int i;
	char buff[200];
	CDialog::OnInitDialog();
	int nBaudSel;

	CButton* pButtonParityNone = (CButton*) GetDlgItem(IDC_COMMSETTINGS_PARITYNONE);
	CButton* pButtonParityEven = (CButton*) GetDlgItem(IDC_COMMSETTINGS_PARITYEVEN);
	CButton* pButtonParityOdd = (CButton*) GetDlgItem(IDC_COMMSETTINGS_PARITYODD);

	

	// TODO: Add extra initialization here
	CComboBox *pB = (CComboBox*) GetDlgItem(IDC_COMMSETTINGS_COMMBAUDRATE);
	if (pB)
	{
		nBaudSel = 0;
		for(i=0;i<BAUDRATE_MAX;i++)
		{
			sprintf(buff,"%d",gBaudRates[i]);
			pB->AddString(buff);
			pB->SetItemData(i,gBaudRates[i]);
			if (m_nCommBaudRate == gBaudRates[i]) 
			{
				nBaudSel = i;
			}
		
		}

		pB->SetCurSel(nBaudSel);
	}
	SetDlgItemInt(IDC_COMMSETTINGS_COMMPORT, m_nCommPort);

	if(pButtonParityNone) pButtonParityNone->SetCheck(m_nCommParity==NOPARITY?1:0);
	if(pButtonParityEven) pButtonParityEven->SetCheck(m_nCommParity==EVENPARITY?1:0);
	if(pButtonParityOdd) pButtonParityOdd->SetCheck(m_nCommParity==ODDPARITY?1:0);

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCommSettings::OnBnClickedConnect()
{
	// TODO: Add your control notification handler code here
	int nCommParity = m_nCommParity;
	int nCommBaudRate = m_nCommBaudRate;
	int nCommPort;
	int		bTrans;
	CButton* pButtonParityNone = (CButton*) GetDlgItem(IDC_COMMSETTINGS_PARITYNONE);
	CButton* pButtonParityEven = (CButton*) GetDlgItem(IDC_COMMSETTINGS_PARITYEVEN);
	CButton* pButtonParityOdd = (CButton*) GetDlgItem(IDC_COMMSETTINGS_PARITYODD);
	CComboBox *pB = (CComboBox*) GetDlgItem(IDC_COMMSETTINGS_COMMBAUDRATE);


	OnOK();
	nCommPort = GetDlgItemInt( IDC_COMMSETTINGS_COMMPORT, &bTrans  );

	if (pButtonParityNone)  
	{
		if (pButtonParityNone->GetCheck()) nCommParity = NOPARITY;
	}
	if (pButtonParityEven)  
	{
		if (pButtonParityEven->GetCheck()) nCommParity = EVENPARITY;
	}
	if (pButtonParityOdd)  
	{
		if (pButtonParityOdd->GetCheck()) nCommParity = ODDPARITY;
	}

	if (pB)
	{
		int nSel;
		nSel = pB->GetCurSel();
		nCommBaudRate = pB->GetItemData(nSel);
	}

//	if ((nCommParity != m_nCommParity) || (nCommBaudRate != m_nCommBaudRate) || (m_nCommPort != nCommPort))
//	{
		m_nCommPort = nCommPort;
		m_nCommBaudRate = nCommBaudRate;
		m_nCommParity = nCommParity;
		if (m_dlgParent)
		{
			m_dlgParent->OnChangeCommSettings( nCommPort,  nCommBaudRate,  nCommParity) ;
		}
//	}

}
