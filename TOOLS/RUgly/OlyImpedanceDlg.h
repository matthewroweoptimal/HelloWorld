#ifndef _OLYIMPEDANCE_H_
#define _OLYIMPEDANCE_H_
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


typedef struct {
	int ANGLE;			//	/*    1 */ ePID_OLYspeaker1_STATUS_ANGLE,							int32				Angle;
	float AMP1_TEMP;		//	/*    2 */ ePID_OLYspeaker1_STATUS_AMP1_TEMP,					float32				Amp1_Temp;
	float AMP2_TEMP;		//	/*    3 */ ePID_OLYspeaker1_STATUS_AMP2_TEMP,					float32				Amp2_Temp;
	uint32 AMP_FAULT;		//	/*    4 */ ePID_OLYspeaker1_STATUS_AMP_FAULT,					bool32				Amp_Fault;
	uint32 LIMITING;		//	/*    5 */ ePID_OLYspeaker1_STATUS_LIMITING,					bool32				Limiting;
	uint32 IDENTIFY;			//	/*    6 */ ePID_OLYspeaker1_STATUS_IDENTIFY,					int32				Identify;
	uint32 FAN_ENABLED;     //	/*    7 */ ePID_OLYspeaker1_STATUS_FAN_ENABLED,					bool32				Fan_Enabled;
	uint32 SELFTEST_MODE;   //	/*    8 */ ePID_OLYspeaker1_STATUS_SELFTEST_MODE,				int32				SelfTest_Mode;
	uint32 SELFTEST_SEQ;    //	/*    9 */ ePID_OLYspeaker1_STATUS_SELFTEST_SEQ,				int32	 			SelfTest_Seq;
	uint32 LOGO_STATE;      //	/*   10 */ ePID_OLYspeaker1_STATUS_LOGO_STATE,					int32				Logo_State;
	float CS_I_CH1;			//	/*   11 */ ePID_OLYspeaker1_STATUS_CS_I_CH1,					float32				cs_I_ch1;
	float CS_I_CH2;			//	/*   12 */ ePID_OLYspeaker1_STATUS_CS_I_CH2,					float32				cs_I_ch2;
	float CS_I_CH3;			//	/*   13 */ ePID_OLYspeaker1_STATUS_CS_I_CH3,					float32				cs_I_ch3;
	float CS_I_CH4;			//	/*   14 */ ePID_OLYspeaker1_STATUS_CS_I_CH4,					float32				cs_I_ch4;
	float CS_V_CH1;			//	/*   15 */ ePID_OLYspeaker1_STATUS_CS_V_CH1,					float32				cs_V_ch1;
	float CS_V_CH2;			//	/*   16 */ ePID_OLYspeaker1_STATUS_CS_V_CH2,					float32				cs_V_ch2;
	float CS_V_CH3;			//	/*   17 */ ePID_OLYspeaker1_STATUS_CS_V_CH3,					float32				cs_V_ch3;
	float CS_V_CH4;			//	/*   18 */ ePID_OLYspeaker1_STATUS_CS_V_CH4,					float32				cs_V_ch4;
	float CS_Z_CH1;			//	/*   19 */ ePID_OLYspeaker1_STATUS_CS_Z_CH1,					float32				cs_Z_ch1;
	float CS_Z_CH2;			//  /*   20 */ ePID_OLYspeaker1_STATUS_CS_Z_CH2,					float32				cs_Z_ch2;
	float CS_Z_CH3;			//	/*   21 */ ePID_OLYspeaker1_STATUS_CS_Z_CH3,					float32				cs_Z_ch3;
	float CS_Z_CH4;			//	/*   22 */ ePID_OLYspeaker1_STATUS_CS_Z_CH4,					float32				cs_Z_ch4;
	uint32 CS_FAULT_CH1;    //	/*   23 */ ePID_OLYspeaker1_STATUS_CS_FAULT_CH1,				int32				cs_fault_ch1;
	uint32 CS_FAULT_CH2;    //	/*   24 */ ePID_OLYspeaker1_STATUS_CS_FAULT_CH2,				int32				cs_fault_ch2;
	uint32 CS_FAULT_CH3;    //	/*   25 */ ePID_OLYspeaker1_STATUS_CS_FAULT_CH3,				int32				cs_fault_ch3;
	uint32 CS_FAULT_CH4;	//	/*   26 */ ePID_OLYspeaker1_STATUS_CS_FAULT_CH4,				int32				cs_fault_ch4;
	uint32 ARRAY_MISMATCH;  //	/*   27 */ ePID_OLYspeaker1_STATUS_ARRAY_MISMATCH,				bool32				Array_Mismatch;
	uint32 ARRAY_SIZE;      //	/*   28 */ ePID_OLYspeaker1_STATUS_ARRAY_SIZE,					uint32_t			Array_Size;
	uint32 ARRAY_INDEX;     //	/*   29 */ ePID_OLYspeaker1_STATUS_ARRAY_INDEX,					uint32_t			Array_Index;
	int ARRAY_ID_ABOVE;     //	/*   30 */ ePID_OLYspeaker1_STATUS_ARRAY_ID_ABOVE,				uint32_t			Array_ID_Above;
	int ARRAY_ID_BELOW;		//	/*   31 */ ePID_OLYspeaker1_STATUS_ARRAY_ID_BELOW,				uint32_t			Array_ID_Below;
} tOLYStatusMeters;




// COlyImpedanceDlg dialog

class COlyImpedanceDlg : public CDialog
{
	DECLARE_DYNAMIC(COlyImpedanceDlg)

public:
	COlyImpedanceDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COlyImpedanceDlg();


	CRUglyDlg* m_dlgParent;
	int m_nProductType;

	bool m_bInProgress;

	mandolin_message m_mGetMeters;
	uint8  m_pcGetMeterPayload[100];
	uint8 m_uiSequence;
	void OnStatusDecode(mandolin_message* pInMsg);

	tOLYStatusMeters m_jMeters;
	void	UpdateStatusValues();


	//void UpdateOutputTextMeters(CString theText,bool bAppend, bool bForce);
	//CRichEditCtrl m_OutputTextMeters;
	void LogResultsToFile(tOLYStatusMeters* jMeters);

	CString m_strLogFileName;
	char m_pcLogFileName[400];


	void OnTimer(UINT nIDEvent);

// Dialog Data
	enum { IDD = IDD_OLYIMPEDANCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOlyimpedanceStart();
	afx_msg void OnBnClickedOlyimpedanceStop();
	afx_msg void OnBnClickedOlyimpedanceFileselect();
	afx_msg void OnBnClickedOlyimpedanceLogclear();
};
#endif