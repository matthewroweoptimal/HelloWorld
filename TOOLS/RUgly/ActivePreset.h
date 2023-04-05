#pragma once
#include "LOUD_types.h"
#include "ProductMap.h"

// CActivePreset dialog
class CRUglyDlg;
class CActivePreset : public CDialog
{
	DECLARE_DYNAMIC(CActivePreset)

public:
	CActivePreset(CWnd* pParent = NULL);   // standard constructor
	virtual ~CActivePreset();


	CRUglyDlg* m_dlgParent;
	int m_nProductType;
	ControlDesc	m_ComboBox[3];

	void ShowParameter(int nTarget, int nParam, bool bAll = false);
	void SendParameterAdjust(int nTarget, int nParam, char* pcValue);
	void CActivePreset::ChangeComboSelPreset(int nCombo);


// Dialog Data
	enum { IDD = IDD_ACTIVEPRESET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	afx_msg void OnCbnSelchangeActivepresetProfile();
	afx_msg void OnCbnSelchangeActivepresetThrow();
	afx_msg void OnCbnSelchangeActivepresetVoicing();
};
