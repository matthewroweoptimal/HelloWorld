#pragma once
#include "LOUD_types.h"

extern "C"
{
#include "mandolin.h"
//#include "ProductMap.h"

}

// CTestManuDlg dialog
class CRUglyDlg;
class CTestManuDlg : public CDialog
{
	DECLARE_DYNAMIC(CTestManuDlg)

public:
	CTestManuDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTestManuDlg();


	CRUglyDlg* m_dlgParent;
	int m_nProductType;
	void SetProductType(int nProductType);

	uint32	m_uiBrand;
	uint32  m_uiModel;


	intfloat GetDlgItemValue(int nIDC, int& nType);


// Dialog Data
	enum { IDD = IDD_TESTMANU };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedTestmanuSetfuncgenoutput();
	virtual BOOL OnInitDialog();

	void ProcessMessageTest(mandolin_message* theMessage, int theLength);
	void UpdateSetBrandModelSelect(uint32 uiBrand, uint32 uiModel);
	void SetBrandModel(uint32 uiBrand, uint32 uiModel,char* pcBrand=0, char* pcModel=0);


	afx_msg void OnBnClickedTestmanuFuncgen();
	afx_msg void OnBnClickedTestmanuSetrmsinputlevel();
	afx_msg void OnBnClickedTestmanuGetrmsinputlevel();
	afx_msg void OnBnClickedTestmanuGetrmsinputrange();
	afx_msg void OnBnClickedTestmanuWriteio();
	afx_msg void OnBnClickedTestmanuReadio();
	afx_msg void OnBnClickedTestmanuReadamptemp();
	afx_msg void OnBnClickedTestmanuGethardwareinfo();
	afx_msg void OnCbnSelchangeTestmanuWriteioSelect();
	afx_msg void OnCbnSelchangeTestmanuReadioSelect();
	afx_msg void OnBnClickedTestmanuGetserialnumber();
	afx_msg void OnBnClickedTestmanuSetserialnumber();
	afx_msg void OnBnClickedTestmanuRestoredefaults();
	afx_msg void OnBnClickedTestmanuValidatespiflash();
	afx_msg void OnBnClickedTestmanuGetimonvalue();
	afx_msg void OnBnClickedTestmanuGetmacaddr();
	afx_msg void OnBnClickedTestmanuSetbrandmodel();
	afx_msg void OnBnClickedTestmanuSetmacaddr();
	afx_msg void OnBnClickedTestmanuReboot2();
};
