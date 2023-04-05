#pragma once

#include "LOUD_types.h"


typedef struct {
	int nTarget;
	int nParamNum;
	int nIDC;	// the controller ID
//	*tParameterDesc pParameterDesc;
} LevelAdjustControlDesc;			// attach a Description to a control object (


#define MAX_LEVELADJUST_FADER	4
#define MAX_LEVELADJUST_EDITBOX	4
#define MAX_LEVELADJUST_SWITCH	8

// CLevelAdjust dialog
class CRUglyDlg;
class CLevelAdjust : public CDialog
{
	DECLARE_DYNAMIC(CLevelAdjust)

public:
	CLevelAdjust(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLevelAdjust();
	virtual void OnOK();

	CRUglyDlg* m_dlgParent;
	int m_nProductType;
	void SetProductType(int nProductType);

	void SetupVScroll();

	int m_nFaderAdjustByTouch; // semiphore
	void ShowParameter(int nTarget, int nParam, bool bAll = false);
	void SendParameterAdjust(int nTarget, int nParam, char* pcValue);


//	CScrollBar  m_Fader[MAX_FADER];
	CScrollBar  m_Fader[MAX_LEVELADJUST_FADER];
	LevelAdjustControlDesc m_descFader[MAX_LEVELADJUST_FADER];
	LevelAdjustControlDesc m_descSwitch[MAX_LEVELADJUST_SWITCH];
	LevelAdjustControlDesc m_descEditBox[MAX_LEVELADJUST_EDITBOX];

// Dialog Data
	enum { IDD = IDD_LEVELADJUST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRangeButtonClick(UINT nID);
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);


	afx_msg void OnEnKillfocusEditInputgain1();
	afx_msg void OnEnKillfocusEditOutputgain1();
	afx_msg void OnEnKillfocusEditOutputgain2();
	afx_msg void OnEnKillfocusEditOutputgain3();
};
