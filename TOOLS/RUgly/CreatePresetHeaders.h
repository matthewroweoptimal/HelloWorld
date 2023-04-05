#ifndef _CREATEPRESETHEADRES_H_
#define _CREATEPRESETHEADRES_H_
#pragma once
class CRUglyDlg;
#include "LOUD_types.h"
#include "resource.h"

extern "C"
{
#include "mandolin.h"
#include "ProductMap.h"

}
#include "MandolinComm.h"

// CCreatePresetHeaders dialog

class CCreatePresetHeaders : public CDialog
{
	DECLARE_DYNAMIC(CCreatePresetHeaders)

public:
	CCreatePresetHeaders(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCreatePresetHeaders();



	CRUglyDlg* m_dlgParent;
	int m_nProductType;
	void SetProductType(int nProductType);
	void SetBrandModel(uint32 uiBrand, uint32 uiModel,char* pcBrand=0, char* pcModel=0);
	
	uint32 m_uiBrand;
	uint32 m_uiModel;


	void GenerateFileName(char* pcFileName, int nTarget, int nSaveIndex);
	int GetTarget(char* pcTarget);
	int GetGetIndex();
	int GetSaveIndex();
	bool GetGenerateAllFlag();
	void ConvertTarget2Name(int nTarget, char* pcTarget);


	bool CreatePresetFile(int nGetIndex, int nSaveIndex, int nTarget);

	void UpdatePresetFileName();

	char m_pcPresetHeaderFileName[400];

    char   m_pcFolderPath[400];;
    //CString   m_strDisplayName;


// Dialog Data
	enum { IDD = IDD_CREATEPRESETHEADERS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCreatepresetheadersAll();
	afx_msg void OnBnClickedCreatepresetheadersCreate();
	afx_msg void OnBnClickedCreatepresetheadersGethwswinfo();
	afx_msg void OnBnClickedCreatepresetheadersSelect();
	afx_msg void OnCbnSelchangeCreatepresetheadersTarget();
	afx_msg void OnBnClickedCreatepresetheadersGetdevicestate();
	afx_msg void OnBnClickedCreatepresetheadersGetdevicefirstate();
};

#endif