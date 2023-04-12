#pragma once



class CRUglyDlg;
#include "LOUD_types.h"
#include "resource.h"


// CScanNetwork dialog
#define MAX_SPEAKERS 300
class CScanNetwork : public CDialog
{
	DECLARE_DYNAMIC(CScanNetwork)

public:
	CScanNetwork(CWnd* pParent = NULL);   // standard constructor
	virtual ~CScanNetwork();

	CRUglyDlg* m_dlgParent;
	int m_nFoundSpeakers;
	uint32 m_nFoundSpeakersIP[MAX_SPEAKERS];
	uint32 m_nFoundSpeakersMACAddr[MAX_SPEAKERS];	//last 4 chars only


	int RunCommandinProcess(char* pcCommand);


	void SetIPAddress(uint32 nIPAddress,uint32 nEnd);

	bool ReadLineFromBuffer(char* pDest, char* pSrc, int* pnSrcPos, int nMaxLineLength);
	bool ParseArpLine(char* pcLine, uint32* IPAddr, unsigned char* macaddr);
	int GetToken(char* pcToken, char* pcSrc, int& nPos, int nSize);

	void AddStringToOutput(char* pcNewLine, bool bAppend=true);



// Dialog Data
	enum { IDD = IDD_SCANNETWORK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedScannetworkStart();
	afx_msg void OnBnClickedScannetworkFindandconnect();
	afx_msg void OnBnClickedScannetworkCleararp();
};
