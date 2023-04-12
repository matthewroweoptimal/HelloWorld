// SocketSettings.cpp : implementation file
//

#include "stdafx.h"

#include "RUgly.h"
#include "RUglyDlg.h"

#include "SocketSettings.h"

#include "comstuff.h"

// CSocketSettings dialog

IMPLEMENT_DYNAMIC(CSocketSettings, CDialog)

CSocketSettings::CSocketSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CSocketSettings::IDD, pParent)
{


	m_dlgParent = 0;
	m_nIPAddress = 0;
	m_nPort = 30000;
	m_nType = TCPClient;
}

CSocketSettings::~CSocketSettings()
{
}

void CSocketSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSocketSettings, CDialog)
	ON_BN_CLICKED(IDCONNECT, &CSocketSettings::OnBnClickedConnect)
	ON_BN_CLICKED(ID_SOCKET_DISCONNECT, &CSocketSettings::OnBnClickedSocketDisconnect)
END_MESSAGE_MAP()


// CSocketSettings message handlers

BOOL CSocketSettings::OnInitDialog() 
{
	bool bStatus = CDialog::OnInitDialog();
	ShowIPAddress();

	return bStatus;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSocketSettings::ShowIPAddress()
{
	// TODO: Add extra initialization here
	CIPAddressCtrl* pctrIPAddr;
		
	pctrIPAddr = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS1);

	if (pctrIPAddr)
	{
		pctrIPAddr->SetAddress(m_nIPAddress);
	}

	SetDlgItemInt(IDC_IPADDRESSPORT,m_nPort);

	UpdateData(FALSE);

}


void CSocketSettings::OnBnClickedConnect()
{
	// TODO: Add your control notification handler code here
	CIPAddressCtrl* pctrIPAddr;
	unsigned char addr1,addr2,addr3,addr4;
		
	pctrIPAddr = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS1);
	m_nPort = GetDlgItemInt( IDC_IPADDRESSPORT );


	pctrIPAddr->GetAddress(addr1,addr2,addr3,addr4);

	m_nIPAddress = ((addr1 & 0x0ff)<<24) + ((addr2& 0x0ff)<<16) + ((addr3 & 0x0ff)<<8)+ (addr4 & 0x0ff);
	//m_nSocketIPAddress = (20<<24) + (0<<16) + (168<<8)+ (192);

	OnOK();

	if (m_dlgParent) {
		m_dlgParent->OnChangeSocketSettings(m_nIPAddress, m_nPort,m_nType) ;
	}
}

void CSocketSettings::OnBnClickedSocketDisconnect()
{
	// TODO: Add your control notification handler code here
	if (m_dlgParent) {
		m_dlgParent->CloseSocketComm() ;
	}

}
