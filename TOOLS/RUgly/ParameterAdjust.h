#ifndef _PARAMETERADJUST_H_
#define _PARAMETERADJUST_H_
#pragma once
class CRUglyDlg;
#include "LOUD_types.h"
#include "resource.h"

extern "C"
{
#include "ProductMap.h"
}

// ParameterAdjust dialog

#include "afxcmn.h"
class ParameterAdjust : public CDialog
{
	DECLARE_DYNAMIC(ParameterAdjust)

public:
	ParameterAdjust(CWnd* pParent = NULL);   // standard constructor
	virtual ~ParameterAdjust();
	virtual void OnOK();
	void OnTimer(UINT nIDEvent);

	CRUglyDlg* m_dlgParent;
	int m_nProductType;
	
	void	SetProductType(int nProductType);

	void	SendParameterAdjust(char *pcName, char* pcValue);
	void	GetParameterDescDetailString(tParameterDesc* pParameterDesc ,char* pcDetails,char* pcDefault);
	void	SendParameterAdjustGet(char *pcName);
	void	GetParameterValueString(char* pcValue, tParameterDesc* pParameterDesc, int nTarget, int nParam);
	void	ShowParameter(int nTarget, int nParam, bool bAll = false);
	int		nAdjustedBySlider; // semiphore for the show.
	void	SetScroll(int nMin, int nMax);
	void	SetComboSelectToParameter(int nCombo, char* pcParameterDesc); // set a combo box to the id
	void	OnCbnSelchangeParameteradjustName(int nCombo);
	void	AdjustComboByOffset(int nOffset);

	void	SetGetTimeoutFlag(int nBox,char* pcString);
	//void	ClearGetTimeoutFlag(int nBox);
	void	SetGetTimer(int nBox);
	void	ClearGetTimer(int nBox);
	int		m_nGetParameterTimeout[5];



// Dialog Data
	enum { IDD = IDD_PARAMETERADJUST};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedParameteradjustSend1();
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeParameteradjustNamecombo1();
	afx_msg void OnCbnSelchangeParameteradjustNamecombo2();
	afx_msg void OnCbnSelchangeParameteradjustNamecombo3();
	afx_msg void OnCbnSelchangeParameteradjustNamecombo4();
	afx_msg void OnCbnSelchangeParameteradjustNamecombo5();
	afx_msg void OnBnClickedParameteradjustSend2();
	afx_msg void OnBnClickedParameteradjustSend3();
	afx_msg void OnBnClickedParameteradjustSend4();
	afx_msg void OnBnClickedParameteradjustSend5();
	afx_msg void OnBnClickedParameteradjustGet1();
	afx_msg void OnBnClickedParameteradjustGet2();
	afx_msg void OnBnClickedParameteradjustGet3();
	afx_msg void OnBnClickedParameteradjustGet4();
	afx_msg void OnBnClickedParameteradjustGet5();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CScrollBar m_Slider[5];
	afx_msg void OnBnClickedParameteradjustPageup();
	afx_msg void OnBnClickedParameteradjustSingleup();
	afx_msg void OnBnClickedParameteradjustSingledown();
	afx_msg void OnBnClickedParameteradjustPagedown();
};

#endif
