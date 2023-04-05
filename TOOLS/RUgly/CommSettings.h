#pragma once
// CCommSettings dialog
class CRUglyDlg;

class CCommSettings : public CDialog
{
	DECLARE_DYNAMIC(CCommSettings)

public:
	CCommSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCommSettings();

	CRUglyDlg* m_dlgParent;

	int m_nCommParity;
	int m_nCommBaudRate;
	int m_nCommPort;

// Dialog Data
	enum { IDD = IDD_COMMSETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedConnect();
};
