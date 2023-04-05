#ifndef _ZIMPEDANCE_H_
#define _ZIMPEDANCE_H_
#pragma once
class CRUglyDlg;
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

// CZImpedanceDlg dialog
class CRUglyDlg;


typedef struct {

	float INPUT_IN;
	float HF_OUT;
	float HF_SOFTLIMIT_IN;
	float HF_SOFTLIMIT_GAINREDUCE;
	float HF_SOFTLIMIT_OUT;
	float MF_OUT;
	float MF_SOFTLIMIT_IN;
	float MF_SOFTLIMIT_GAINREDUCE;
	float MF_SOFTLIMIT_OUT;
	float HF_OUT_DAC;
	float MF_OUT_DAC;
	uint32  ampFault;
	float phCurrentSense0;
	float phCurrentSense1;
	float fVPeak0;
	float fVPeak1;
	float fImpedance0;
	float fImpedance1;
	float fImpedanceAverage0;
	float fImpedanceAverage1;
	uint32  uiTemperature;
	uint32 uiVersion;
} tREDLINEMeters;

class CZImpedanceDlg : public CDialog
{
	DECLARE_DYNAMIC(CZImpedanceDlg)

public:
	CZImpedanceDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CZImpedanceDlg();


	CRUglyDlg* m_dlgParent;
	int m_nProductType;

	bool m_bInProgress;

	mandolin_message m_mGetMeters;
	uint8  m_pcGetMeterPayload[100];
	uint8 m_uiSequence;
	void OnMeterDecode(mandolin_message* pInMsg);

	void UpdateOutputTextMeters(CString theText,bool bAppend, bool bForce);
	CRichEditCtrl m_OutputTextMeters;
	void LogResultsToFile(tREDLINEMeters* jMeters);

	CString m_strLogFileName;
	char m_pcLogFileName[400];

	void OnTimer(UINT nIDEvent);

// Dialog Data
	enum { IDD = IDD_ZIMPEDANCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedZimpedanceStart();
	afx_msg void OnBnClickedZimpedanceStop();
	afx_msg void OnBnClickedZimpedanceLogfileselect();
	afx_msg void OnBnClickedZimpedanceLogfilesclear();
	afx_msg void OnBnClickedZimpedanceClear();
};
#endif
