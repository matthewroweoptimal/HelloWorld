#pragma once


// CDSPSignalFlow dialog

#ifndef _DSPSIGNALFLOW_H_
#define _DSPSIGNALFLOW_H_
#pragma once

class CRUglyDlg;
#include "LOUD_types.h"
#include "resource.h"

extern "C"
{
#include "ProductMap.h"
}

class CDSPSignalFlow : public CDialog
{
	DECLARE_DYNAMIC(CDSPSignalFlow)

public:
	CDSPSignalFlow(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDSPSignalFlow();

public:
	CRUglyDlg* m_dlgParent;
	int m_nProductType;
	void SetProductType(int nProductType);

	int m_nImage;

	void InitImageSelect();
	void SetImageSelect(int nSel);

// Dialog Data
	enum { IDD = IDD_DSPSIGNALFLOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	afx_msg void OnStnClickedDspsignalflowImage();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCbnSelchangeDspsignalflowSelect();
};
#endif