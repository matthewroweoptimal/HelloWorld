#ifndef _APPPRODCUTDEFS_OLY_H_
#define _APPPRODCUTDEFS_OLY_H_
#pragma once
class CRUglyDlg;
#include "LOUD_types.h"
#include "resource.h"
#include "afxcmn.h"

#define PRESET_MAX_USER		6
#define PRESET_MAX_PROFILE	5
#define PRESET_MAX_THROW	4
#define PRESET_MAX_XOVER	8
#define PRESET_MAX_VOICING	4
#define PRESET_MAX_FIR		1

typedef struct tCurrentState_tag {
	void* vpDeviceParameterBuffer[OLYspeaker1_DEVICE_PARAMETER_MAX];							// eTARGET_DEVICE,
	void* vpStatusParameterBuffer[OLYspeaker1_STATUS_PARAMETER_MAX];							// eTARGET_STATUS,
	void* vpUserParameterBuffer[PRESET_MAX_USER][OLYspeaker1_USER_PARAMETER_MAX];				// eTARGET_USER,
	void* vpProfileParameterBuffer[PRESET_MAX_PROFILE][OLYspeaker1_PROFILE_PARAMETER_MAX];		// eTARGET_PROFILE,
	void* vpThrowParameterBuffer[PRESET_MAX_THROW][OLYspeaker1_THROW_PARAMETER_MAX];			// eTARGET_THROW,
	void* vpVoicingParameterBuffer[PRESET_MAX_VOICING][OLYspeaker1_VOICING_PARAMETER_MAX];		// eTARGET_VOICING,
	void* vpFIRParameterBuffer[PRESET_MAX_FIR][OLYspeaker1_FIR_PARAMETER_MAX];					// eTARGET_FIR
	void* vpXOverParameterBuffer[PRESET_MAX_XOVER][OLYspeaker1_XOVER_PARAMETER_MAX];			// eTARGET_XOVER,
	void* vpAirlossParameterBuffer[OLYspeaker1_AIRLOSS_PARAMETER_MAX];							// eTARGET_AIRLOSS,
	void* vpNoisegateParameterBuffer[OLYspeaker1_NOISEGATE_PARAMETER_MAX];						// eTARGET_NOISEGATE,
} tCurrentStateOLY;


class AppProductDefs_OLY 
{
public:
	AppProductDefs_OLY();
	~AppProductDefs_OLY();

	CRUglyDlg* m_dlgParent;

	uint32 m_uiBrand;
	uint32 m_uiModel;

	int m_nUserInstance;
	int m_nProfileInstance;
	int m_nThrowInstance;
	int m_nVoicingInstance;
	int m_nFIRInstance;
	int m_nXOverInstance;

	// parameter maps and desc
	int GetParameterDescTable(int nTarget, tParameterDesc** ppParameterDesc, int* pTableLength, int* nXMLVersion);
	tParameterDesc* GetParameterDesc(int nTarget, int nParamNum);
	void**  GetParamAddress(int nTarget, int nParam, int nInstance = 0);
	tParameterDesc*  ConvertString2ParameterDesc(char* pName,int* nTarget, int* nPID);
	int GetMeterType(  int nTarget, int nParamNum);
	unsigned int*	GetMeterSubscriptionList( int nSelect, int* pnSize);


	void  SetBrandModel(uint32 uiBrand, uint32 uiModel);
	int	 GetTargetInstances(uint32 uiBrand, uint32 uiModel,int nTarget);

	bool IsTargetFIR(int nTarget);
	bool IsTargetMeters(int nTarget);
	int GetMetersTarget();
	int GetUserTarget();


	// currnent state
	tCurrentStateOLY m_CurrentState;
	float m_gMeterLevel[OLYspeaker1_METER_MAX];
	void SetCurrentStateToDefaults();
	void SetCurrentStateToDefaultsFIR();
	void SetCurrentStateToRandom();
	void SetCurrentStateToRandomFIR();
	void SaveCurrentStateToFile(char* pcFileName);
	void SaveCurrentStateFIRToFile(char* pcFileName);
	void ShowCurrentState();
	void ShowCurrentStateFIR();

	int GetRandomTarget(int* nTarget, int* nInstance);
	uint32	ComputeCRC();
	uint32	ComputeFIRCRC();


	// Get State from device
	int m_nGetStatePos;
	int m_nGetStateTargetPos;		// which target number
	int m_nGetStateTargetInstance;		// which instance in target number
	int m_nGetStateRequestSize;
	OLY_target m_nGetSingleTarget;			// eTARGET_INVALID for all targets, target for specific target
	void GetStateStart(OLY_target nTarget);
	void GetStateStop();
	void GetStateRequestNext();

	// Send State to Device
	int m_nSendStatePos;
	int m_nSendStateTargetPos;
	int m_nSendStateRequestSize;
	int m_nSendStateMode;	// 0 = normal,  1 = FIR

	void SendStateStart(int nMode = 0);	//  0 = normal, 1 = FIR
	void SendStateStop();
	void SendStateSendNext();


	void ParameterAdjustSetComboBox(CComboBox** pCombo, int nNum);

	int GetLevelSwitchDescList(ControlDesc** pList);
	int GetLevelFaderDescList(ControlDesc** pList);
	int GetLevelEditBoxDescList(ControlDesc** pList);
	int GetLevelTitleList(tStringIDC** pList);


	int GetMetersDescList(ControlDesc** pList);
	int GetMetersTitleList(tStringIDC** pList);

	EQGRAPH_EQSelectList*	GetEQSelectList();
	int						GetEQSelectListSize();

	int	GetPresetHeaderList(int* nList, int nMax);


	
};


#endif


