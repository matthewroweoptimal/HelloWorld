#ifndef _APPPRODCUTDEFS_REDLINE_H_
#define _APPPRODCUTDEFS_REDLINE_H_
#pragma once
class CRUglyDlg;
#include "LOUD_types.h"
#include "resource.h"
#include "afxcmn.h"

typedef struct {
	//uint32 utf8SystemLabel[MAX_SYSTEMLABEL];
	void* vpParameterBuffer[REDLINEspeaker1_SNAPSHOT_PARAMETER_MAX];
	void* vpGlobalParameterBuffer[REDLINEspeaker1_GLOBAL_PARAMETER_MAX];
	void* vpFIRParameterBuffer[REDLINEspeaker1_FIR_PARAMETER_MAX];
	//void* vpInfoParameterBuffer[INFO_PARAMETER_MAX];
	//char pcShowStringParameterBuffer[eLID_SNAPSHOT_FENCE][SNAPSHOT_DISPLAYNAME_MAX];
} tCurrentStateREDLINE;


class AppProductDefs_REDLINE 
{
public:
	AppProductDefs_REDLINE();
	~AppProductDefs_REDLINE();

	CRUglyDlg* m_dlgParent;

	// parameter maps and desc
	int GetParameterDescTable(int nTarget, tParameterDesc** ppParameterDesc, int* pTableLength, int* nXMLVersion);
	tParameterDesc* GetParameterDesc(int nTarget, int nParamNum);
	void**  GetParamAddress(int nTarget, int nParam, int nInstance = 0);
	tParameterDesc*  ConvertString2ParameterDesc(char* pName,int* nTarget, int* nPID);
	int GetMeterType(  int nTarget, int nParamNum);

	bool IsTargetFIR(int nTarget);
	bool IsTargetMeters(int nTarget);
	int GetMetersTarget();


	void  SetBrandModel(uint32 uiBrand, uint32 uiModel);


	// currnent state
	tCurrentStateREDLINE m_CurrentState;
	float m_gMeterLevel[REDLINEspeaker1_METER_MAX];
	void SetCurrentStateToDefaults();
	void SetCurrentStateToDefaultsFIR();
	void SetCurrentStateToRandom();
	void SetCurrentStateToRandomFIR();
	void SaveCurrentStateToFile(char* pcFileName);
	void SaveCurrentStateFIRToFile(char* pcFileName);
	void ShowCurrentState();
	void ShowCurrentStateFIR();

	int GetRandomTarget(int* nTarget, int* nInstance);

	// Get State from device
	int m_nGetStatePos;
	int m_nGetStateTargetPos;		// which target number
	int m_nGetStateRequestSize;
	void GetStateStart();
	void GetStateStop();
	void GetStateRequestNext();

	// Send State to Device
	int m_nSendStatePos;
	int m_nSendStateTargetPos;
	int m_nSendStateRequestSize;
	void SendStateStart();
	void SendStateStop();
	void SendStateSendNext();


	void ParameterAdjustSetComboBox(CComboBox** pCombo, int nNum);

	int GetLevelSwitchDescList(ControlDesc** pList);
	int GetLevelFaderDescList(ControlDesc** pList);
	int GetLevelEditBoxDescList(ControlDesc** pList);
	int GetLevelTitleList(tStringIDC** pList);


	
	int GetMetersDescList(ControlDesc** pList);
	int GetMetersTitleList(tStringIDC** pList);
};


#endif


