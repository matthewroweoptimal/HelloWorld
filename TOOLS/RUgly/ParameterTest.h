#pragma once


// CParameterTest dialog
class CRUglyDlg;
#include "LOUD_types.h"
#include "mandolin.h"
#include "ProductMap.h"

#define MAX_PARAMETERTEST_PARAMETERS  50
#define MAX_PARAMETERTEST_MAXONGOING  10

enum {
	PARAMETERTEST_STATUS_EMPTY = 0,
	PARAMETERTEST_STATUS_CREATED,
	PARAMETERTEST_STATUS_SENT,
};

typedef struct {
	int nStatus;

	int nSequenceSet;
	int nSequenceGet;
	int nTimeoutCounter; // count down
	
	int nNumParameters;	// 0 indicates not in use

	int nTarget;
	int nInstance;
	uint32  uiPID[MAX_PARAMETERTEST_PARAMETERS];
	intfloat jValues[MAX_PARAMETERTEST_PARAMETERS];


} tParameterTest;


class CParameterTest : public CDialog
{
	DECLARE_DYNAMIC(CParameterTest)

public:
	CParameterTest(CWnd* pParent = NULL);   // standard constructor
	virtual ~CParameterTest();

	CRUglyDlg* m_dlgParent;
	int m_nProductType;
	void SetProductType(int nProductType);


	void OnTimer(UINT nIDEvent);
	void SetParameterTestTimer();
	void ProcessTimer();

	uint32 uiStartTickCount;
	
	bool m_bTestInProgress;

	CString m_strLogFileName;
	char m_pcLogFileName[400];
	void LogEditsToFile(int nTarget, uint32 *PIDs, intfloat* jValues, int nNum, int nInstance);

	// ------------------
	// Message values
	// ------------------
	tParameterTest m_tTestMessages[MAX_PARAMETERTEST_MAXONGOING];
	void OnClearMessages();
	int FindEmptyMessageIndex();
	void OnFreeMessage(int nIndex);

	int OnGetOnGoing();
	int OnNewMessages();
	bool SendParameterTestMessage(int nIndex);

	void	ProcessMessageParameterSetReply(mandolin_message* pInMsg, int theLength);
	void	ProcessMessageParameterGetReply(mandolin_message* theMessage, int theLength);
	void CompareReceivedMessage(int nSequence,int nTarget,int nInstance,uint32* uiPID, intfloat* jValues, tParameterDesc** pParameterDesc,int nReceived);



	int m_nMaxNumParameters;
	int m_nMaxOnGoing;
	int m_nNumOnGoing;
	float m_fAverageOnGoing;
	int m_nAverageOnGoing;


	// ------------------
	// Results
	// ------------------


	int m_nNumTests;
	int m_nNumTimeouts;
	int m_nNumValueMismatch;
	int m_nNumInvalidSet;
	int m_nNumPasses;

	bool m_bStopOnFirstError; 


	void OnResetResults();
	void OnUpdateResults();




// Dialog Data
	enum { IDD = IDD_PARAMETERTEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedParametertestClear();
	afx_msg void OnBnClickedParametertestStart();
	afx_msg void OnBnClickedParametertestStop();
	afx_msg void OnBnClickedParametertestLogfileselect();
	afx_msg void OnBnClickedParametertestLogfileclear();
	afx_msg void OnBnClickedParametertestStoponfirsterror();
};
