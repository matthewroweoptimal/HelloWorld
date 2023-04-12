#pragma once
#include "SocketComm.h"
// CSocketSettings dialog
class CRUglyDlg;

class CSocketSettings : public CDialog
{
	DECLARE_DYNAMIC(CSocketSettings)

public:
	CSocketSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSocketSettings();

	CRUglyDlg* m_dlgParent;

	unsigned m_nIPAddress;
	int m_nPort;
	SocketType m_nType;
	void ShowIPAddress();


// Dialog Data
	enum { IDD = IDD_SOCKETSETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedSocketDisconnect();
};
