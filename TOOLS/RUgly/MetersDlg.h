#pragma once
#include "LOUD_types.h"
#include "resource.h"

extern "C"
{
#include "ProductMap.h"
}


#pragma once

#include "LOUD_types.h"
#include "resource.h"


extern "C"
{
#include "mandolin.h"
#include "ProductMap.h"

}
#include "MandolinComm.h"
#include "afxwin.h"

// CMetersDlg dialog

class CRUglyDlg;


typedef struct {
	int nTarget;
	int nParamNum;
	int nIDC;	// the controller ID
	int nMeterType;
	int nIDCValue;	// for meters, this is the printout of the actual value
//	*tParameterDesc pParameterDesc;
} MetersControlDesc;			// attach a Description to a control object (

#define MAX_METERS_DESC 31

class CMetersDlg : public CDialog
{
	DECLARE_DYNAMIC(CMetersDlg)

public:
	CMetersDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMetersDlg();

	CRUglyDlg* m_dlgParent;
	int m_nProductType;
	void SetProductType(int nProductType);

	bool m_bInProgress;

	void OnSetMeterSubscribeSelect(int Select);


	void ShowMeter(int nTarget, int nParam, float fValue);
	MetersControlDesc m_descMeters[MAX_METERS_DESC];

	mandolin_message m_mGetMeters;
	uint8  m_pcGetMeterPayload[100];
	uint8 m_uiSequence;
	
	unsigned int m_uiMeterRequestInterval;


	int m_nMeterListNumPIDs;
	unsigned int* m_pMeterList; // id, target, pid, pid, pid
	int m_nMeterListRate; // msec 
	bool m_bMeterListInProgress;

	void OnTimer(UINT nIDEvent);

// Dialog Data
	enum { IDD = IDD_METERDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedMetersStart();
	afx_msg void OnBnClickedMetersStop();
	afx_msg void OnBnClickedUpdateInterval();
	afx_msg void OnBnClickedMetersSubscribeStart();
	afx_msg void OnBnClickedMetersSubscribeStop();
};
