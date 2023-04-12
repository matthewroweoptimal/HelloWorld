#include "stdafx.h"
#include "LOUD_types.h"
#include "LOUD_defines.h"
extern "C" 
{
#include "mandolin.h"
#include "ProductMap.h"
#include "crc.h"
}

//#include "Product_arch.h"
#include "RUglyDlg.h"
#include "AppProductDefs_OLY.h"
#include <stdlib.h>
#include "resource.h"
#include <math.h>

#include "OLY_meta.h"
#include "Product_arch.h"
#include "OLYspeaker1_pids.h"
#include "OLYspeaker1_lookup.h"
#include "OLYspeaker1_map.h"
extern "C" 
{
#include "OLYspeaker1_ParamTable.h"
}



AppProductDefs_OLY::AppProductDefs_OLY()
{
	m_dlgParent = NULL;
	m_nGetStatePos = 1;
	m_nGetStateTargetPos = 0;
	m_nGetStateTargetInstance = 1;
	m_nGetStateRequestSize = 20;
	m_nGetSingleTarget = eTARGET_INVALID;		// set to get all
	m_nSendStatePos = 1;
	m_nSendStateTargetPos = 0;
	m_nSendStateRequestSize = 20;
	m_nSendStateMode = 0; // normal 0, FIR 1


	m_nUserInstance = 1;
	m_nProfileInstance = 1;
	m_nThrowInstance = 1;
	m_nVoicingInstance = 1;
	m_nFIRInstance = 1;
	m_nXOverInstance = 1;

	m_uiBrand = LOUD_BRAND_MACKIE;
	m_uiModel = MACKIE_MODEL_R8_AA;
}

AppProductDefs_OLY::~AppProductDefs_OLY()
{
}




// ----------------------------------------------------------------------------------------------------
// parameter maps and descrioptors.
// ----------------------------------------------------------------------------------------------------
int AppProductDefs_OLY::GetParameterDescTable(int nTarget, tParameterDesc** ppParameterDesc, int* pTableLength, int* nXMLVersion)
{
	*ppParameterDesc = 0;
	*pTableLength = 0;
	*nXMLVersion = 0;
	//if (nProduct >= ePRODUCT_FENCE) return 0;
	if (nTarget >= eTARGET_FENCE) return 0;
	*ppParameterDesc = gOLYspeaker1ParameterTables[nTarget];
	*pTableLength = nOLYspeaker1ParameterTablesMax[nTarget];
	*nXMLVersion = OLYspeaker1_XML_VERSION;
	return *pTableLength;

}



tParameterDesc* AppProductDefs_OLY::GetParameterDesc(int nTarget, int nParamNum)
{
	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;

	GetParameterDescTable(nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);

	if (!pParameterDesc) return 0;

	if (nParamNum < 0)
	{
		return 0;
	}

	if (nParamNum >= nTableLength) return 0;

	return &pParameterDesc[nParamNum];


}
int AppProductDefs_OLY::GetMeterType(  int nTarget, int nParamNum)
{
	if (nParamNum < OLYspeaker1_METER_MAX)
	{
		return g_OLYspeaker1MeterTypeTable[nParamNum];
	}
	return eMETERTYPE_NORMAL;
}


#define OLYMETERLIST_STATUS_SIZE  31
unsigned int g_nOLYStatusMeterList[OLYMETERLIST_STATUS_SIZE+2] =  // id, target, pid, pid, pid
{
  0x1, 			// id number
  eTARGET_STATUS,
	/*    1 */ ePID_OLYspeaker1_STATUS_ANGLE,
	/*    2 */ ePID_OLYspeaker1_STATUS_AMP1_TEMP,
	/*    3 */ ePID_OLYspeaker1_STATUS_AMP2_TEMP,
	/*    4 */ ePID_OLYspeaker1_STATUS_AMP_FAULT,
	/*    5 */ ePID_OLYspeaker1_STATUS_LIMITING,
	/*    6 */ ePID_OLYspeaker1_STATUS_IDENTIFY,
	/*    7 */ ePID_OLYspeaker1_STATUS_FAN_ENABLED,
	/*    8 */ ePID_OLYspeaker1_STATUS_SELFTEST_MODE,
	/*    9 */ ePID_OLYspeaker1_STATUS_SELFTEST_SEQ,
	/*   10 */ ePID_OLYspeaker1_STATUS_LOGO_STATE,
	/*   11 */ ePID_OLYspeaker1_STATUS_CS_I_CH1,
	/*   12 */ ePID_OLYspeaker1_STATUS_CS_I_CH2,
	/*   13 */ ePID_OLYspeaker1_STATUS_CS_I_CH3,
	/*   14 */ ePID_OLYspeaker1_STATUS_CS_I_CH4,
	/*   15 */ ePID_OLYspeaker1_STATUS_CS_V_CH1,
	/*   16 */ ePID_OLYspeaker1_STATUS_CS_V_CH2,
	/*   17 */ ePID_OLYspeaker1_STATUS_CS_V_CH3,
	/*   18 */ ePID_OLYspeaker1_STATUS_CS_V_CH4,
	/*   19 */ ePID_OLYspeaker1_STATUS_CS_Z_CH1,
	/*   20 */ ePID_OLYspeaker1_STATUS_CS_Z_CH2,
	/*   21 */ ePID_OLYspeaker1_STATUS_CS_Z_CH3,
	/*   22 */ ePID_OLYspeaker1_STATUS_CS_Z_CH4,
	/*   23 */ ePID_OLYspeaker1_STATUS_CS_FAULT_CH1,
	/*   24 */ ePID_OLYspeaker1_STATUS_CS_FAULT_CH2,
	/*   25 */ ePID_OLYspeaker1_STATUS_CS_FAULT_CH3,
	/*   26 */ ePID_OLYspeaker1_STATUS_CS_FAULT_CH4,
	/*   27 */ ePID_OLYspeaker1_STATUS_ARRAY_MISMATCH,
	/*   28 */ ePID_OLYspeaker1_STATUS_ARRAY_SIZE,
	/*   29 */ ePID_OLYspeaker1_STATUS_ARRAY_INDEX,
	/*   30 */ ePID_OLYspeaker1_STATUS_ARRAY_ID_ABOVE,
	/*   31 */ ePID_OLYspeaker1_STATUS_ARRAY_ID_BELOW,
};



#define OLYMETERLIST_ALL_SIZE  35
unsigned int g_nOLYALLOutMeterList[OLYMETERLIST_ALL_SIZE+2] =  // id, target, pid, pid, pid
{
	0x2, 			// id number
	eTARGET_METERS,
	/*    1 */ eMID_OLYspeaker1_INPUT_IN,
	/*    2 */ eMID_OLYspeaker1_SPEAKER_IN,
	/*    3 */ eMID_OLYspeaker1_GROUP_SOFTLIMIT_IN,
	/*    4 */ eMID_OLYspeaker1_GROUP_SOFTLIMIT_GAINREDUCE,
	/*    5 */ eMID_OLYspeaker1_GROUP_SOFTLIMIT_OUT,
	/*    6 */ eMID_OLYspeaker1_HF_OUT,
	/*    7 */ eMID_OLYspeaker1_HF_SOFTLIMIT_IN,
	/*    8 */ eMID_OLYspeaker1_HF_SOFTLIMIT_GAINREDUCE,
	/*    9 */ eMID_OLYspeaker1_HF_SOFTLIMIT_OUT,
	/*   10 */ eMID_OLYspeaker1_HF_KLIMITER_IN,
	/*   11 */ eMID_OLYspeaker1_HF_KLIMITER_GAINREDUCE,
	/*   12 */ eMID_OLYspeaker1_HF_KLIMITER_OUT,
	/*   13 */ eMID_OLYspeaker1_MF_OUT,
	/*   14 */ eMID_OLYspeaker1_MF_SOFTLIMIT_IN,
	/*   15 */ eMID_OLYspeaker1_MF_SOFTLIMIT_GAINREDUCE,
	/*   16 */ eMID_OLYspeaker1_MF_SOFTLIMIT_OUT,
	/*   17 */ eMID_OLYspeaker1_MF_KLIMITER_IN,
	/*   18 */ eMID_OLYspeaker1_MF_KLIMITER_GAINREDUCE,
	/*   19 */ eMID_OLYspeaker1_MF_KLIMITER_OUT,
	/*   20 */ eMID_OLYspeaker1_LF_OUT,
	/*   21 */ eMID_OLYspeaker1_LF_SOFTLIMIT_IN,
	/*   22 */ eMID_OLYspeaker1_LF_SOFTLIMIT_GAINREDUCE,
	/*   23 */ eMID_OLYspeaker1_LF_SOFTLIMIT_OUT,
	/*   24 */ eMID_OLYspeaker1_LF_KLIMITER_IN,
	/*   25 */ eMID_OLYspeaker1_LF_KLIMITER_GAINREDUCE,
	/*   26 */ eMID_OLYspeaker1_LF_KLIMITER_OUT,
	/*   27 */ eMID_OLYspeaker1_HF_DYNEQ1_GAIN,
	/*   28 */ eMID_OLYspeaker1_HF_DYNEQ2_GAIN,
	/*   29 */ eMID_OLYspeaker1_MF_DYNEQ1_GAIN,
	/*   30 */ eMID_OLYspeaker1_MF_DYNEQ2_GAIN,
	/*   31 */ eMID_OLYspeaker1_LF_DYNEQ1_GAIN,
	/*   32 */ eMID_OLYspeaker1_LF_DYNEQ2_GAIN,
	/*   33 */ eMID_OLYspeaker1_INPUT_GATE_IN,
	/*   34 */ eMID_OLYspeaker1_INPUT_GATE_GAINREDUCE,
	/*   35 */ eMID_OLYspeaker1_INPUT_GATE_OUT,
};


unsigned int*	 AppProductDefs_OLY::GetMeterSubscriptionList( int nSelect, int* pnSize)
{
	// returns: id, target, pid, pid, pid
	*pnSize  = 0;
	switch (nSelect)
	{
	case 0:
		*pnSize = OLYMETERLIST_STATUS_SIZE;
		return g_nOLYStatusMeterList;
	case 1:
	default:
		*pnSize = OLYMETERLIST_ALL_SIZE;
		return g_nOLYALLOutMeterList;
	}

}


void**  AppProductDefs_OLY::GetParamAddress(int nTarget, int nParam, int nInstance)
{
	void** pAddress = 0;
	intfloat j;


	
	switch (nTarget)
	{
	case eTARGET_DEVICE:
		if (nParam < OLYspeaker1_DEVICE_PARAMETER_MAX)
			pAddress = &m_CurrentState.vpDeviceParameterBuffer[nParam]; 

		break;

	case	eTARGET_STATUS:
		if (nParam < OLYspeaker1_STATUS_PARAMETER_MAX)
			pAddress = &m_CurrentState.vpStatusParameterBuffer[nParam]; 
		break;

	case eTARGET_USER:
		if (m_uiBrand == LOUD_BRAND_MARTIN)
		{
			nInstance = 0;
		}
		else if (nInstance == 0)
		{
			j.vp = m_CurrentState.vpDeviceParameterBuffer[ePID_OLYspeaker1_DEVICE_ACTIVE_USER];
			nInstance = j.i;

		}
		else
		{
			nInstance--;
		}
		if ((nInstance  < 0) ||(nInstance>=PRESET_MAX_USER)) 
			nInstance = 0;

		if (nParam < OLYspeaker1_USER_PARAMETER_MAX)
			pAddress =  &m_CurrentState.vpUserParameterBuffer[nInstance][nParam]; 
		break;

	case eTARGET_PROFILE:
		if (m_uiBrand == LOUD_BRAND_MARTIN)
		{
			nInstance = 0;
		}
		else if (nInstance == 0)
		{
			j.vp = m_CurrentState.vpDeviceParameterBuffer[ePID_OLYspeaker1_DEVICE_ACTIVE_PROFILE];
			nInstance = j.i;
		}
		else
		{
			nInstance--;
		}
		if ((nInstance  < 0) ||(nInstance>=PRESET_MAX_PROFILE)) 
			nInstance = 0;

		if (nParam < OLYspeaker1_PROFILE_PARAMETER_MAX)
			pAddress =  &m_CurrentState.vpProfileParameterBuffer[nInstance][nParam]; 
		break;

	case eTARGET_THROW:
		if (m_uiBrand == LOUD_BRAND_MARTIN)
		{
			nInstance = 0;
		}
		else if (nInstance == 0)
		{
			j.vp = m_CurrentState.vpDeviceParameterBuffer[ePID_OLYspeaker1_DEVICE_ACTIVE_THROW];
			nInstance = j.i;

		}
		else
		{
			nInstance--;
		}
		if ((nInstance  < 0) ||(nInstance>=PRESET_MAX_THROW)) 
			nInstance = 0;

		if (nParam < OLYspeaker1_THROW_PARAMETER_MAX)
			pAddress = &m_CurrentState.vpThrowParameterBuffer[nInstance][nParam]; 

		break;
	case eTARGET_VOICING:
		if (m_uiBrand == LOUD_BRAND_MARTIN)
		{
			nInstance = 0;
		}
		else if (nInstance == 0)
		{
			j.vp = m_CurrentState.vpDeviceParameterBuffer[ePID_OLYspeaker1_DEVICE_ACTIVE_VOICING];
			nInstance = j.i;

		}
		else
		{
			nInstance--;
		}
		if ((nInstance  < 0) ||(nInstance>=PRESET_MAX_VOICING)) 
			nInstance = 0;
		
		if (nParam < OLYspeaker1_VOICING_PARAMETER_MAX)
			pAddress =  &m_CurrentState.vpVoicingParameterBuffer[nInstance][nParam]; 
		break;


	case eTARGET_FIR:
		if (m_uiBrand == LOUD_BRAND_MARTIN)
		{
			nInstance = 0;
		}
		else if (nInstance == 0)
		{
			j.vp = m_CurrentState.vpDeviceParameterBuffer[ePID_OLYspeaker1_DEVICE_ACTIVE_FIR];
			nInstance = j.i;

		}
		else
		{
			nInstance--;
		}
		if ((nInstance  < 0) ||(nInstance>=PRESET_MAX_FIR)) 
			nInstance = 0;

		if (nParam < OLYspeaker1_FIR_PARAMETER_MAX)
			pAddress =  &m_CurrentState.vpFIRParameterBuffer[nInstance][nParam]; 
		break;

	case eTARGET_XOVER:
		if (m_uiBrand == LOUD_BRAND_MARTIN)
		{
			nInstance = 0;
		}
		else if (nInstance == 0)
		{
			j.vp = m_CurrentState.vpDeviceParameterBuffer[ePID_OLYspeaker1_DEVICE_ACTIVE_XOVER];
			nInstance = j.i;

		}
		else
		{
			nInstance--;
		}
		if ((nInstance  < 0) ||(nInstance>=PRESET_MAX_XOVER)) 
			nInstance = 0;

		if (nParam < OLYspeaker1_XOVER_PARAMETER_MAX)
			pAddress =  &m_CurrentState.vpXOverParameterBuffer[nInstance][nParam]; 
		break;

	case eTARGET_METERS:
		if (nParam < OLYspeaker1_METER_MAX)
			pAddress =  (void**) ((float*)&m_gMeterLevel[nParam]); 
		break;

	case eTARGET_AIRLOSS:
		if (nParam < OLYspeaker1_AIRLOSS_PARAMETER_MAX)
			pAddress = &m_CurrentState.vpAirlossParameterBuffer[nParam]; 

		break;

	case eTARGET_NOISEGATE:
		if (nParam < OLYspeaker1_NOISEGATE_PARAMETER_MAX)
			pAddress = &m_CurrentState.vpNoisegateParameterBuffer[nParam]; 

		break;

	default:
		break;
	}


	return pAddress;
}

tParameterDesc*  AppProductDefs_OLY::ConvertString2ParameterDesc(char* pName,int* nTarget, int* nPID)
{

	tParameterDesc* pParameterDesc = 0;
	int nTableLength = 0;
	int i;
	int nXMLVersion;

	*nTarget = 0;
	*nPID = 0;

		if (strstr(pName, "DEVICE_") == pName)
		{
			*nTarget = eTARGET_DEVICE;
			GetParameterDescTable(*nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);
		}
		else if (strstr(pName, "USER_") == pName)
		{
			*nTarget = eTARGET_USER;
			GetParameterDescTable(*nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);
		}
		else if (strstr(pName, "STATUS_") == pName)
		{
			*nTarget = eTARGET_STATUS;
			GetParameterDescTable(*nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);
		}
		else if (strstr(pName, "PROFILE_") == pName)
		{
			*nTarget = eTARGET_PROFILE;
			GetParameterDescTable(*nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);
		}
		else if (strstr(pName, "THROW_") == pName)
		{
			*nTarget = eTARGET_THROW;
			GetParameterDescTable(*nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);
		}
		else if (strstr(pName, "VOICING_") == pName)
		{
			*nTarget = eTARGET_VOICING;
			GetParameterDescTable(*nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);
		}
		else if (strstr(pName, "XOVER_") == pName)
		{
			*nTarget = eTARGET_XOVER;
			GetParameterDescTable(*nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);
		}
		else if (strstr(pName, "AIRLOSS_") == pName)
		{
			*nTarget = eTARGET_AIRLOSS;
			GetParameterDescTable(*nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);
		}
		else if (strstr(pName, "NOISEGATE_") == pName)
		{
			*nTarget = eTARGET_NOISEGATE;
			GetParameterDescTable(*nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);
		}

		if (strstr(pName, "FIR_") == pName)		// starts with FIR_
		{
			*nTarget = eTARGET_FIR;
			GetParameterDescTable(*nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);
		}


	
	if (!pParameterDesc) 
		return 0;
	for(i=0;i<nTableLength;i++)
	{
		if (strcmp(pName,pParameterDesc->pName) == 0)
		{
			*nPID = i;
			return pParameterDesc;
		}
		pParameterDesc++;
	}
	return 0;
}

bool AppProductDefs_OLY::IsTargetFIR(int nTarget)
{
	return (nTarget == eTARGET_FIR);
}
bool AppProductDefs_OLY::IsTargetMeters(int nTarget)
{
	return (nTarget == eTARGET_METERS);
}

int AppProductDefs_OLY::GetMetersTarget()
{
	return eTARGET_METERS;
}
int AppProductDefs_OLY::GetUserTarget()
{
	return eTARGET_USER;
}

// ----------------------------------------------------------------------------------------------------
// Current state
// ----------------------------------------------------------------------------------------------------
typedef struct {
		int nTarget;
		void** ppAddr;
		int nNumInstances;
} tTargetStateAddr;

typedef struct {
		int nTarget;
		int nNumParameters;
		int nNumInstances;
} tTargetInfo;

typedef struct {
	uint32 uiBrand;
	uint32 uiModel;

	int nUserInstance;
	int nProfileInstance;
	int nThrowInstance;
	int nVoicingInstance;
	int nFIRInstance;
	int nXOverInstance;


} tModelBrandInstance;


tModelBrandInstance gModelBrandInstance[] = {
	//	brand				model,						user,	profile,	throw,	voicing,	FIR,	XOver 
	{ LOUD_BRAND_MACKIE, MACKIE_MODEL_R8_AA,			1,		5,			4,		1,			1,		1		},
	{ LOUD_BRAND_MACKIE, MACKIE_MODEL_R8_64,			1,		5,			4,		1,			1,		8		},
	{ LOUD_BRAND_MACKIE, MACKIE_MODEL_R8_96,			1,		1,			5,		5,			1,		1		},	
	{ LOUD_BRAND_MACKIE, MACKIE_MODEL_R12_64,			1,		5,			4,		1,			1,		8		},
	{ LOUD_BRAND_MACKIE, MACKIE_MODEL_R12_96,			1,		5,			4,		1,			1,		8		},
	{ LOUD_BRAND_MACKIE, MACKIE_MODEL_R12_SM,			1,		5,			4,		1,			1,		8		},
	{ LOUD_BRAND_MACKIE, MACKIE_MODEL_R12_SW,			1,		5,			1,		4,			1,		8		},
	{ LOUD_BRAND_MACKIE, MACKIE_MODEL_R18_SW,			1,		5,			1,		4,			1,		8		},	


	{ LOUD_BRAND_EAW, EAW_MODEL_RSX208L,				1,		5,			4,		1,			1,		1		},
	{ LOUD_BRAND_EAW, EAW_MODEL_RSX86,					1,		5,			4,		1,			1,		8		},
	{ LOUD_BRAND_EAW, EAW_MODEL_RSX89,					1,		1,			5,		5,			1,		1		},	
	{ LOUD_BRAND_EAW, EAW_MODEL_RSX126,					1,		5,			4,		1,			1,		8		},
	{ LOUD_BRAND_EAW, EAW_MODEL_RSX129,					1,		5,			4,		1,			1,		8		},
	{ LOUD_BRAND_EAW, EAW_MODEL_RSX12M,					1,		5,			4,		1,			1,		8		},
	{ LOUD_BRAND_EAW, EAW_MODEL_RSX12,					1,		5,			1,		4,			1,		8		},
	{ LOUD_BRAND_EAW, EAW_MODEL_RSX18,					1,		5,			1,		4,			1,		8		},	
	{ LOUD_BRAND_EAW, EAW_MODEL_RSX218,					1,		5,			1,		4,			1,		8		},
	{ LOUD_BRAND_EAW, EAW_MODEL_RSX18F,					1,		5,			1,		4,			1,		8		},
	{ LOUD_BRAND_EAW, EAW_MODEL_RSX212L,				1,		5,			4,		1,			1,		1		},

	{ LOUD_BRAND_MARTIN, MARTIN_MODEL_GENERIC,			5,		1,			1,		1,			1,		1		},
	{ LOUD_BRAND_MARTIN, MARTIN_MODEL_CDDL8,			5,		1,			1,		1,			1,		1		},
	{ LOUD_BRAND_MARTIN, MARTIN_MODEL_CDDL12,			5,		1,			1,		1,			1,		1		},
	{ LOUD_BRAND_MARTIN, MARTIN_MODEL_CDDL15,			6,		1,			1,		1,			1,		1		},	
	{ LOUD_BRAND_MARTIN, MARTIN_MODEL_CSXL118,			6,		1,			1,		1,			1,		1		},
	{ LOUD_BRAND_MARTIN, MARTIN_MODEL_CSXL218,			6,		1,			1,		1,			1,		1		},
};



void  AppProductDefs_OLY::SetBrandModel(uint32 uiBrand, uint32 uiModel)
{
	int i;
	m_nUserInstance = 1;
	m_nProfileInstance = 1;
	m_nThrowInstance = 1;
	m_nVoicingInstance = 1;
	m_nFIRInstance = 1;
	m_nXOverInstance = 1;

	m_uiBrand = uiBrand;
	m_uiModel = uiModel;

	for(i=0;i<sizeof(gModelBrandInstance)/sizeof(tModelBrandInstance);i++)
	{
		if ((uiBrand == gModelBrandInstance[i].uiBrand) && (uiModel == gModelBrandInstance[i].uiModel))
		{
			m_nUserInstance = gModelBrandInstance[i].nUserInstance;
			m_nProfileInstance = gModelBrandInstance[i].nProfileInstance;
			m_nThrowInstance = gModelBrandInstance[i].nThrowInstance;
			m_nVoicingInstance = gModelBrandInstance[i].nVoicingInstance;
			m_nFIRInstance = gModelBrandInstance[i].nFIRInstance;
			m_nXOverInstance = gModelBrandInstance[i].nXOverInstance;
		}
	}
	return;

}
int AppProductDefs_OLY::GetTargetInstances(uint32 uiBrand, uint32 uiModel,int nTarget)
{
	for(int i=0;i<sizeof(gModelBrandInstance)/sizeof(tModelBrandInstance);i++)
	{
		if ((uiBrand == gModelBrandInstance[i].uiBrand) && (uiModel == gModelBrandInstance[i].uiModel))
		{
			switch(nTarget)
			{
			case eTARGET_DEVICE:	return 1;	
			case eTARGET_USER:		return gModelBrandInstance[i].nUserInstance;			
			case eTARGET_PROFILE:	return gModelBrandInstance[i].nProfileInstance;		
			case eTARGET_THROW:		return gModelBrandInstance[i].nThrowInstance;				
			case eTARGET_VOICING:	return gModelBrandInstance[i].nVoicingInstance;		
			case eTARGET_FIR:		return gModelBrandInstance[i].nFIRInstance;		
			case eTARGET_XOVER:		return gModelBrandInstance[i].nXOverInstance;	
			case eTARGET_AIRLOSS:	return 1;	
			case eTARGET_NOISEGATE:	return 1;	
			}
		}
	}
	return 1;

}


void AppProductDefs_OLY::SetCurrentStateToDefaults() {
	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;
	int i;
	int k;
	intfloat jVal;
	void** pDest;
	int nInstances;

	tTargetStateAddr  nTargetList[] = {
		{eTARGET_DEVICE,		&m_CurrentState.vpDeviceParameterBuffer[0], 1},			
		{eTARGET_STATUS,		&m_CurrentState.vpStatusParameterBuffer[0], 1},			
		{eTARGET_USER,			&m_CurrentState.vpUserParameterBuffer[0][0],PRESET_MAX_USER},				
		{eTARGET_PROFILE,		&m_CurrentState.vpProfileParameterBuffer[0][0],PRESET_MAX_PROFILE},			
		{eTARGET_THROW,			&m_CurrentState.vpThrowParameterBuffer[0][0],PRESET_MAX_THROW},				
		{eTARGET_VOICING,		&m_CurrentState.vpVoicingParameterBuffer[0][0],PRESET_MAX_VOICING},			
		//{eTARGET_FIR,			&m_CurrentState.vpFIRParameterBuffer[0][0],PRESET_MAX_FIR},	
		{eTARGET_XOVER,			&m_CurrentState.vpXOverParameterBuffer[0][0],PRESET_MAX_XOVER},				
		{eTARGET_AIRLOSS,		&m_CurrentState.vpAirlossParameterBuffer[0],1},				
		{eTARGET_NOISEGATE,		&m_CurrentState.vpNoisegateParameterBuffer[0],1},				
	};
	

	for(int j=0; j< sizeof(nTargetList)/sizeof(tTargetStateAddr);j++)
	{

		GetParameterDescTable(nTargetList[j].nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);

		nInstances = nTargetList[j].nNumInstances;
		for(k=0;k<nInstances;k++)
		{
			for(i=0;i<nTableLength;i++)
			{
				pDest = (nTargetList[j].ppAddr+i) + k*nTableLength;
				*pDest = (void*) pParameterDesc[i].defValue;
			}
		}


	}

	// fix up the FIR since all coeffs are 0, must make the first one 1.0
//	jVal.f = 1.0;
//	for(i=0;i<PRESET_MAX_FIR;i++)
//	{
//
//		m_CurrentState.vpFIRParameterBuffer[i][ePID_OLYspeaker1_FIR_HF_FIR_COEF1_VALUE] = jVal.vp;
//		m_CurrentState.vpFIRParameterBuffer[i][ePID_OLYspeaker1_FIR_MF_FIR_COEF1_VALUE] = jVal.vp;
//		m_CurrentState.vpFIRParameterBuffer[i][ePID_OLYspeaker1_FIR_LF_FIR_COEF1_VALUE] = jVal.vp;
//	}

}

void AppProductDefs_OLY::SetCurrentStateToDefaultsFIR() {
	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;
	int i;
	int k;
	intfloat jVal;
	void** pDest;
	int nInstances;

	tTargetStateAddr  nTargetList[] = {
		//{eTARGET_DEVICE,		&m_CurrentState.vpDeviceParameterBuffer[0], 1},			
		//{eTARGET_STATUS,		&m_CurrentState.vpStatusParameterBuffer[0], 1},			
		//{eTARGET_USER,			&m_CurrentState.vpUserParameterBuffer[0][0],PRESET_MAX_USER},				
		//{eTARGET_PROFILE,		&m_CurrentState.vpProfileParameterBuffer[0][0],PRESET_MAX_PROFILE},			
		//{eTARGET_THROW,			&m_CurrentState.vpThrowParameterBuffer[0][0],PRESET_MAX_THROW},				
		//{eTARGET_VOICING,		&m_CurrentState.vpVoicingParameterBuffer[0][0],PRESET_MAX_VOICING},			
		{eTARGET_FIR,			&m_CurrentState.vpFIRParameterBuffer[0][0],PRESET_MAX_FIR},	
		//{eTARGET_XOVER,			&m_CurrentState.vpXOverParameterBuffer[0][0],PRESET_MAX_XOVER},				
	};
	

	for(int j=0; j< sizeof(nTargetList)/sizeof(tTargetStateAddr);j++)
	{

		GetParameterDescTable(nTargetList[j].nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);

		nInstances = nTargetList[j].nNumInstances;
		for(k=0;k<nInstances;k++)
		{
			for(i=0;i<nTableLength;i++)
			{
				pDest = (nTargetList[j].ppAddr+i) + k*nTableLength;
				*pDest = (void*) pParameterDesc[i].defValue;
			}
		}


	}

	// fix up the FIR since all coeffs are 0, must make the first one 1.0
	jVal.f = 1.0;
	for(i=0;i<PRESET_MAX_FIR;i++)
	{

		m_CurrentState.vpFIRParameterBuffer[i][ePID_OLYspeaker1_FIR_HF_FIR_COEF1_VALUE] = jVal.vp;
		m_CurrentState.vpFIRParameterBuffer[i][ePID_OLYspeaker1_FIR_MF_FIR_COEF1_VALUE] = jVal.vp;
		m_CurrentState.vpFIRParameterBuffer[i][ePID_OLYspeaker1_FIR_LF_FIR_COEF1_VALUE] = jVal.vp;
	}

}
uint32 AppProductDefs_OLY::ComputeCRC()
{
	// TODO: Add your control notification handler code here
	char pcBuffer[200];
	intfloat jVal;
	int i,k;
	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;
	void** pDest;
	int nInstances;
	uint32 uiCRCValue = CRC32_SEED;


	tTargetStateAddr  nTargetList[] = {
		//{eTARGET_DEVICE,		&m_CurrentState.vpDeviceParameterBuffer[0],1},			
		{eTARGET_USER,			&m_CurrentState.vpUserParameterBuffer[0][0],1},				
		{eTARGET_PROFILE,		&m_CurrentState.vpProfileParameterBuffer[0][0],5},			
		{eTARGET_THROW,			&m_CurrentState.vpThrowParameterBuffer[0][0],3},				
		{eTARGET_VOICING,		&m_CurrentState.vpVoicingParameterBuffer[0][0],4},			
		//{eTARGET_FIR,			&m_CurrentState.vpFIRParameterBuffer[0][0],1},	
		{eTARGET_XOVER,			&m_CurrentState.vpXOverParameterBuffer[0][0],1},				
	};


	CRC32_init();
	for(int j=0; j< sizeof(nTargetList)/sizeof(tTargetStateAddr);j++)
	{

		GetParameterDescTable(nTargetList[j].nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);
		//nInstances = nTargetList[j].nNumInstances;
		nInstances = GetTargetInstances(m_uiBrand,m_uiModel, nTargetList[j].nTarget);
		for(k=0;k<nInstances;k++)
		{

			uiCRCValue = CRC32_block_words(uiCRCValue,(uint32*)(void*)nTargetList[j].ppAddr, nTableLength);
		}
	}

	return uiCRCValue;
}
uint32 AppProductDefs_OLY::ComputeFIRCRC()
{
	// TODO: Add your control notification handler code here
	char pcBuffer[200];
	intfloat jVal;
	int i,k;
	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;
	void** pDest;
	int nInstances;
	uint32 uiCRCValue = CRC32_SEED;


	tTargetStateAddr  nTargetList[] = {
		//{eTARGET_DEVICE,		&m_CurrentState.vpDeviceParameterBuffer[0],1},			
		//{eTARGET_USER,			&m_CurrentState.vpUserParameterBuffer[0][0],1},				
		//{eTARGET_PROFILE,		&m_CurrentState.vpProfileParameterBuffer[0][0],5},			
		//{eTARGET_THROW,			&m_CurrentState.vpThrowParameterBuffer[0][0],3},				
		//{eTARGET_VOICING,		&m_CurrentState.vpVoicingParameterBuffer[0][0],4},			
		{eTARGET_FIR,			&m_CurrentState.vpFIRParameterBuffer[0][0],1},	
		//{eTARGET_XOVER,			&m_CurrentState.vpXOverParameterBuffer[0][0],1},				
	};


	CRC32_init();
	for(int j=0; j< sizeof(nTargetList)/sizeof(tTargetStateAddr);j++)
	{

		GetParameterDescTable(nTargetList[j].nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);
		//nInstances = nTargetList[j].nNumInstances;
		nInstances = GetTargetInstances(m_uiBrand,m_uiModel, nTargetList[j].nTarget);
		for(k=0;k<nInstances;k++)
		{

			uiCRCValue = CRC32_block_words(uiCRCValue,(uint32*)(void*)nTargetList[j].ppAddr, nTableLength);
		}
	}

	return uiCRCValue;
}
void AppProductDefs_OLY::SetCurrentStateToRandom()
{
	// TODO: Add your control notification handler code here
	char pcBuffer[200];
	intfloat jVal;
	int i,k;
	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;
	void** pDest;
	int nInstances;


	tTargetStateAddr  nTargetList[] = {
		//{eTARGET_DEVICE,		&m_CurrentState.vpDeviceParameterBuffer[0],1},			
		{eTARGET_USER,			&m_CurrentState.vpUserParameterBuffer[0][0],1},				
		{eTARGET_PROFILE,		&m_CurrentState.vpProfileParameterBuffer[0][0],5},			
		{eTARGET_THROW,			&m_CurrentState.vpThrowParameterBuffer[0][0],3},				
		{eTARGET_VOICING,		&m_CurrentState.vpVoicingParameterBuffer[0][0],4},			
		//{eTARGET_FIR,			&m_CurrentState.vpFIRParameterBuffer[0]},	
		{eTARGET_XOVER,			&m_CurrentState.vpXOverParameterBuffer[0][0],1},				
		{eTARGET_AIRLOSS,		&m_CurrentState.vpAirlossParameterBuffer[0],1},				
		{eTARGET_NOISEGATE,		&m_CurrentState.vpNoisegateParameterBuffer[0],1},				
	};


	for(int j=0; j< sizeof(nTargetList)/sizeof(tTargetStateAddr);j++)
	{

		GetParameterDescTable(nTargetList[j].nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);
		//nInstances = nTargetList[j].nNumInstances;
		nInstances = GetTargetInstances(m_uiBrand,m_uiModel, nTargetList[j].nTarget);
		for(k=0;k<nInstances;k++)
		{
			for(i=0;i<nTableLength;i++)
			{
				jVal  =  m_dlgParent->GetParameterRandomValue(&pParameterDesc[i],pcBuffer);
				//nTargetList[j].ppAddr[i] = jVal.vp;

				pDest = (nTargetList[j].ppAddr+i) + k*nTableLength;
				*pDest = jVal.vp;
			}
		}
	}

}

void AppProductDefs_OLY::SetCurrentStateToRandomFIR()
{
	// TODO: Add your control notification handler code here
	char pcBuffer[200];
	intfloat jVal;
	int i,k;
	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;
	void** pDest;
	int nInstances;


	tTargetStateAddr  nTargetList[] = {
		//{eTARGET_DEVICE,		&m_CurrentState.vpDeviceParameterBuffer[0],1},			
		//{eTARGET_USER,			&m_CurrentState.vpUserParameterBuffer[0][0],1},				
		//{eTARGET_PROFILE,		&m_CurrentState.vpProfileParameterBuffer[0][0],5},			
		//{eTARGET_THROW,			&m_CurrentState.vpThrowParameterBuffer[0][0],3},				
		//{eTARGET_VOICING,		&m_CurrentState.vpVoicingParameterBuffer[0][0],4},			
		{eTARGET_FIR,			&m_CurrentState.vpFIRParameterBuffer[0][0],1},	
		//{eTARGET_XOVER,			&m_CurrentState.vpXOverParameterBuffer[0][0],1},				
	};


	for(int j=0; j< sizeof(nTargetList)/sizeof(tTargetStateAddr);j++)
	{

		GetParameterDescTable(nTargetList[j].nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);
		//nInstances = nTargetList[j].nNumInstances;
		nInstances = GetTargetInstances(m_uiBrand,m_uiModel, nTargetList[j].nTarget);
		for(k=0;k<nInstances;k++)
		{
			for(i=0;i<nTableLength;i++)
			{
				jVal  =  m_dlgParent->GetParameterRandomValue(&pParameterDesc[i],pcBuffer);
				//nTargetList[j].ppAddr[i] = jVal.vp;

				pDest = (nTargetList[j].ppAddr+i) + k*nTableLength;
				*pDest = jVal.vp;
			}
		}
	}

}




int AppProductDefs_OLY::GetRandomTarget(int* nTarget, int* nInstance)
{
	// TODO: Add your control notification handler code here
	char pcBuffer[200];
	intfloat jVal;
	int i,k;
	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;
	void** pDest;
	double r;
	int nSize;
	int nPickTarget;
	int nPickInstance;

	tTargetInfo  nTargetList[] = {
		//{eTARGET_DEVICE,		OLYspeaker1_DEVICE_PARAMETER_MAX,1},			
		{eTARGET_USER,			OLYspeaker1_USER_PARAMETER_MAX,1},				
		{eTARGET_PROFILE,		OLYspeaker1_PROFILE_PARAMETER_MAX,1}, //5			
		{eTARGET_THROW,			OLYspeaker1_THROW_PARAMETER_MAX,1},	//4				
		{eTARGET_VOICING,		OLYspeaker1_VOICING_PARAMETER_MAX,1},		// 4 did not work?		
		//{eTARGET_FIR,			OLYspeaker1_FIR_PARAMETER_MAX,1},	
		{eTARGET_XOVER,			OLYspeaker1_XOVER_PARAMETER_MAX,1},				
		{eTARGET_AIRLOSS,		OLYspeaker1_AIRLOSS_PARAMETER_MAX,1},				
		{eTARGET_NOISEGATE,		OLYspeaker1_NOISEGATE_PARAMETER_MAX,1},				
	};
	
	r = (double)(rand()*1.0)/RAND_MAX;
	nSize = sizeof(nTargetList)/sizeof(tTargetInfo);
	nPickTarget = r*nSize;
	if (nPickTarget >= nSize) nPickTarget = nSize-1;


	*nTarget = nTargetList[nPickTarget].nTarget;


	*nInstance = 1;

	// model dependent instances
	nSize = GetTargetInstances(m_uiBrand,m_uiModel, *nTarget);
	//nSize = nTargetList[nPickTarget].nNumInstances;

	if (nSize > 1)
	{
		r = (double)(rand()*1.0)/RAND_MAX;

		nPickInstance = r*nSize;
		if (nPickInstance >= nSize) nPickInstance = nSize-1;

		//nPickInstance = 0; // set this for now  DEBUG

		*nInstance = nPickInstance+1;	// make 1..n
	}
	return  nTargetList[nPickTarget].nNumParameters;

}

void AppProductDefs_OLY::ShowCurrentState()
{
	// TODO: Add your control notification handler code here
	char pcBuffer[200];
	intfloat jVal;
	int i;
	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;
	tTargetStateAddr  nTargetList[] = {
		{eTARGET_DEVICE,		&m_CurrentState.vpDeviceParameterBuffer[0], 1},			
		//{eTARGET_STATUS,		&m_CurrentState.vpStatusParameterBuffer[0], 1},			
		{eTARGET_USER,			&m_CurrentState.vpUserParameterBuffer[0][0],PRESET_MAX_USER},				
		{eTARGET_PROFILE,		&m_CurrentState.vpProfileParameterBuffer[0][0],PRESET_MAX_PROFILE},			
		{eTARGET_THROW,			&m_CurrentState.vpThrowParameterBuffer[0][0],PRESET_MAX_THROW},				
		{eTARGET_VOICING,		&m_CurrentState.vpVoicingParameterBuffer[0][0],PRESET_MAX_VOICING},			
		//{eTARGET_FIR,			&m_CurrentState.vpFIRParameterBuffer[0][0],PRESET_MAX_FIR},	
		{eTARGET_XOVER,			&m_CurrentState.vpXOverParameterBuffer[0][0],PRESET_MAX_XOVER},				
		{eTARGET_AIRLOSS,		&m_CurrentState.vpAirlossParameterBuffer[0],1},				
		{eTARGET_NOISEGATE,		&m_CurrentState.vpNoisegateParameterBuffer[0],1},				
	};


	if (m_dlgParent)
	{
		
		m_dlgParent->UpdateOutputText("SHOW",true);


		for(int j=0; j< sizeof(nTargetList)/sizeof(tTargetStateAddr);j++)
		{

			GetParameterDescTable(nTargetList[j].nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);


			//int nNumInstances = GetTargetInstances(m_uiBrand,m_uiModel, nTargetList[j].nTarget);

			//for (int k=1;k<=nNumInstances;k++)
			//{

				for(i=1;i<nTableLength;i++)
				{
					jVal.vp = nTargetList[j].ppAddr[i];
					m_dlgParent->m_MandolinTextSummary.ConvertParameter2Text(&pParameterDesc[i],jVal,pcBuffer,false);
					m_dlgParent->UpdateOutputText(pcBuffer,true,false,false);
				}
			//}
		}
		m_dlgParent->UpdateOutputText("end of SHOW",true);
	}


}


void AppProductDefs_OLY::ShowCurrentStateFIR()
{
	// TODO: Add your control notification handler code here
	char pcBuffer[200];
	intfloat jVal;
	int i;
	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;
	tTargetStateAddr  nTargetList[] = {
		//{eTARGET_DEVICE,		&m_CurrentState.vpDeviceParameterBuffer[0], 1},			
		//{eTARGET_STATUS,		&m_CurrentState.vpStatusParameterBuffer[0], 1},			
		//{eTARGET_USER,			&m_CurrentState.vpUserParameterBuffer[0][0],PRESET_MAX_USER},				
		//{eTARGET_PROFILE,		&m_CurrentState.vpProfileParameterBuffer[0][0],PRESET_MAX_PROFILE},			
		//{eTARGET_THROW,			&m_CurrentState.vpThrowParameterBuffer[0][0],PRESET_MAX_THROW},				
		//{eTARGET_VOICING,		&m_CurrentState.vpVoicingParameterBuffer[0][0],PRESET_MAX_VOICING},			
		{eTARGET_FIR,			&m_CurrentState.vpFIRParameterBuffer[0][0],PRESET_MAX_FIR},	
		//{eTARGET_XOVER,			&m_CurrentState.vpXOverParameterBuffer[0][0],PRESET_MAX_XOVER},				
	};


	if (m_dlgParent)
	{
		
		m_dlgParent->UpdateOutputText("SHOW",true);


		for(int j=0; j< sizeof(nTargetList)/sizeof(tTargetStateAddr);j++)
		{

			GetParameterDescTable(nTargetList[j].nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);


			//int nNumInstances = GetTargetInstances(m_uiBrand,m_uiModel, nTargetList[j].nTarget);

			//for (int k=1;k<=nNumInstances;k++)
			//{

				for(i=1;i<nTableLength;i++)
				{
					jVal.vp = nTargetList[j].ppAddr[i];
					m_dlgParent->m_MandolinTextSummary.ConvertParameter2Text(&pParameterDesc[i],jVal,pcBuffer,false);
					m_dlgParent->UpdateOutputText(pcBuffer,true,false,false);
				}
			//}
		}
		m_dlgParent->UpdateOutputText("end of SHOW",true);
	}


}

void AppProductDefs_OLY::SaveCurrentStateToFile(char* pcFileName)
{
	FILE *fpFactoryDefault = NULL;
//	char pcFileName[_MAX_PATH] = "SaveStateFIR.txt";

	char pcString[200];
	tParameterDesc* pParameterDesc;
	void** pParameterValue;
	intfloat jVal;
	int i;
	char pcFormat[200];
	//char szSelectFileTitle[_MAX_PATH];
	//char szSelectFileFilter[256];
	//CString strFileName;

	tParameterDesc* pParameterDescTable; 
	int nTableLength;
	int nXMLVersion;
	int nNumInstances;
	void** ppAddr;
	tTargetStateAddr  nTargetList[] = {
		{eTARGET_DEVICE,		&m_CurrentState.vpDeviceParameterBuffer[0], 1},			
		//{eTARGET_STATUS,		&m_CurrentState.vpStatusParameterBuffer[0], 1},			
		{eTARGET_USER,			&m_CurrentState.vpUserParameterBuffer[0][0],PRESET_MAX_USER},				
		{eTARGET_PROFILE,		&m_CurrentState.vpProfileParameterBuffer[0][0],PRESET_MAX_PROFILE},			
		{eTARGET_THROW,			&m_CurrentState.vpThrowParameterBuffer[0][0],PRESET_MAX_THROW},				
		{eTARGET_VOICING,		&m_CurrentState.vpVoicingParameterBuffer[0][0],PRESET_MAX_VOICING},			
		//{eTARGET_FIR,			&m_CurrentState.vpFIRParameterBuffer[0][0],PRESET_MAX_FIR},	
		{eTARGET_XOVER,			&m_CurrentState.vpXOverParameterBuffer[0][0],PRESET_MAX_XOVER},				
		{eTARGET_AIRLOSS,		&m_CurrentState.vpAirlossParameterBuffer[0],1},				
		{eTARGET_NOISEGATE,		&m_CurrentState.vpNoisegateParameterBuffer[0],1},				
	};


	fpFactoryDefault = fopen(pcFileName,"w");


	// write the file header

	if (fpFactoryDefault)
	{

		strcpy(pcString,	"PRODUCT OLYSPEAKER\n");
		//strcpy(pcString,	"PRODUCT REDLINESPEAKER\n");
			
		fwrite(&pcString,1,strlen(pcString),fpFactoryDefault);


		for(int j=0; j< sizeof(nTargetList)/sizeof(tTargetStateAddr);j++)
		{

			GetParameterDescTable(nTargetList[j].nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);
			
			nNumInstances = GetTargetInstances(m_uiBrand,m_uiModel, nTargetList[j].nTarget);
			if (m_uiBrand == LOUD_BRAND_MARTIN)
			{
				nNumInstances = 1;
			}

			for (int k=1;k<=nNumInstances;k++)
			{
				for(i=1;i<nTableLength;i++)
				{
					pcString[0] = 0;
					ppAddr = GetParamAddress(nTargetList[j].nTarget, i, k);
					if (ppAddr)
					{
						jVal.vp = *ppAddr;

						if (m_uiBrand == LOUD_BRAND_MARTIN)
							m_dlgParent->m_MandolinTextSummary.ConvertParameter2Text(&pParameterDesc[i],jVal,pcString,false,0,true);
						else
							m_dlgParent->m_MandolinTextSummary.ConvertParameter2Text(&pParameterDesc[i],jVal,pcString,false,k, true);

						// write the file header
						fwrite(&pcString,1,strlen(pcString),fpFactoryDefault);
					}
				}
			}
		}
		fclose(fpFactoryDefault);

	}

}


void AppProductDefs_OLY::SaveCurrentStateFIRToFile(char* pcFileName)
{
	FILE *fpFactoryDefault = NULL;
//	char pcFileName[_MAX_PATH] = "SaveStateFIR.txt";

	char pcString[200];
	tParameterDesc* pParameterDesc;
	void** pParameterValue;
	intfloat jVal;
	int i;
	int j;
	char pcFormat[200];
	//char szSelectFileTitle[_MAX_PATH];
	//char szSelectFileFilter[256];
	//CString strFileName;

	tParameterDesc* pParameterDescTable; 
	int nTableLength;
	int nXMLVersion;

	tTargetStateAddr  nTargetList[] = {
		{eTARGET_FIR,			&m_CurrentState.vpFIRParameterBuffer[0][0]},	
	};


	fpFactoryDefault = fopen(pcFileName,"w");


	// write the file header

	if (fpFactoryDefault)
	{

		strcpy(pcString,	"PRODUCT OLYSPEAKER\n");
		//strcpy(pcString,	"PRODUCT REDLINESPEAKER\n");
			
		fwrite(&pcString,1,strlen(pcString),fpFactoryDefault);
		for(int j=0; j< sizeof(nTargetList)/sizeof(tTargetStateAddr);j++)
		{

			GetParameterDescTable(nTargetList[j].nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);

			for(i=1;i<nTableLength;i++)
			{

				pcString[0] = 0;
				jVal.vp = nTargetList[j].ppAddr[i];
				m_dlgParent->m_MandolinTextSummary.ConvertParameter2Text(&pParameterDesc[i],jVal,pcString,false);
				// write the file header
				fwrite(&pcString,1,strlen(pcString),fpFactoryDefault);
			}
		}

		fclose(fpFactoryDefault);

	}

	//// dump it has hex valus
	//strcpy(pcFileName, strFileName.GetBuffer());
	//strcat(pcFileName,"2");

	//fpFactoryDefault = fopen(pcFileName,"w");
	//if (fpFactoryDefault)
	//{

	//	strcpy(pcString,	"PRODUCT OLYSPEAKER\n");
	//	//strcpy(pcString,	"PRODUCT REDLINESPEAKER\n");
	//	
	//	fwrite(&pcString,1,strlen(pcString),fpFactoryDefault);


	//	// do the Snapshot
	//	i=1;
	//	GetParameterDescTable(eTARGET_FIR,&pParameterDescTable,&nTableLength,&nXMLVersion);

	//	pParameterDesc = &pParameterDescTable[i];
	//	pParameterValue = &m_CurrentState.vpParameterBuffer[1];
	//	for(;i<nTableLength;i++)
	//	{
	//		pcString[0] = 0;
	//		j.vp = (void*) (*pParameterValue);
	//		sprintf(pcString,"0x%8.8x\n",j.ui);
	//		fwrite(&pcString,1,strlen(pcString),fpFactoryDefault);
	//		pParameterValue++;
	//	}
	//	fclose(fpFactoryDefault);
	//}
}



void AppProductDefs_OLY::GetStateStart(OLY_target nTarget)
{

	GetStateStop();

	m_dlgParent->m_bGetStateStarted = true;
	m_nGetStatePos = 1;
	m_nGetStateTargetPos = 0;
	m_nGetStateTargetInstance = 1;
	m_nGetStateRequestSize = 40;
	m_nGetSingleTarget = nTarget;
	
	GetStateRequestNext();


}

void AppProductDefs_OLY::GetStateStop()
{
	m_dlgParent->m_bGetStateStarted = false;
	if (m_dlgParent)
	{
		m_dlgParent->KillTimer(GETSTATE_TIMERID);
		m_dlgParent->OnShowGetstatefromdevice();
	}



}

void AppProductDefs_OLY::GetStateRequestNext()
{

	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;
	uint32 puiPID[120];
	int i,j;
	int nTargetList[] =
	{
		eTARGET_DEVICE,	
		eTARGET_STATUS,	
		eTARGET_USER,		
		eTARGET_PROFILE,	
		eTARGET_THROW,		
		eTARGET_VOICING,	
		//eTARGET_FIR,
		eTARGET_XOVER,		
		eTARGET_AIRLOSS,		
		eTARGET_NOISEGATE,				
	};
	int nTarget;
	int nMaxInstance;


	if (m_nGetStateTargetPos < sizeof(nTargetList)/sizeof(int))
	{
		if (m_nGetSingleTarget == eTARGET_INVALID) {		// does the all case
			nTarget = nTargetList[m_nGetStateTargetPos];
		} else {
			nTarget = m_nGetSingleTarget;
		}
		GetParameterDescTable(nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);

		if (!pParameterDesc) return;

		//&&&&gm
		// GetTargetInstances(uint32 uiBrand, uint32 uiModel,int nTarget);
		// instance m_nGetStateTargetPos
		nMaxInstance = GetTargetInstances(m_uiBrand,m_uiModel,nTarget);


		if (m_nGetStatePos >= nTableLength)
		{


			m_nGetStatePos = 1;		// reset length

			// &&& check instances.
			if (m_nGetStateTargetInstance < nMaxInstance)
			{
				m_nGetStateTargetInstance++;	// go to the next instance
			}
			else
			{
				m_nGetStateTargetInstance = 1;
				m_nGetStateTargetPos++;
			}
			if (m_nGetSingleTarget != eTARGET_INVALID) {	// only doing one specific target
				m_nGetStateTargetPos = sizeof(nTargetList)/sizeof(int);	// force to end
			}
		}

		// need to put in an instance

		if ((m_nGetStatePos < nTableLength) && (m_nGetStateTargetPos < sizeof(nTargetList)/sizeof(int)))
		{
			nTarget = nTargetList[m_nGetStateTargetPos];

			i = m_nGetStatePos; 
			j = 0;
			while((m_nGetStatePos<nTableLength) && (j < 20))	// can use this to get bigger messages?
			{
				puiPID[j++] = m_nGetStatePos++;
			}

			if (m_dlgParent)
			{
				m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
				if (m_nGetSingleTarget == eTARGET_INVALID) {	
					//m_dlgParent->m_MandolinComm.CreateParameterEditGetNeighborhood(&m_dlgParent->m_msgTx, m_dlgParent->m_nTxSequence,nTargetList[m_nGetStateTargetPos], puiPID[0],j);
					m_dlgParent->m_MandolinComm.CreateParameterEditGetMulti(&m_dlgParent->m_msgTx, m_dlgParent->m_nTxSequence,nTarget, puiPID,j, m_nGetStateTargetInstance);
				} else { // only doing one specific target
					m_dlgParent->m_MandolinComm.CreateParameterEditGetNeighborhood(&m_dlgParent->m_msgTx, m_dlgParent->m_nTxSequence,m_nGetSingleTarget, puiPID[0],j, m_nGetStateTargetInstance);
					//m_dlgParent->m_MandolinComm.CreateParameterEditGetMulti(&m_dlgParent->m_msgTx, m_dlgParent->m_nTxSequence,m_nGetSingleTarget, puiPID,j);
				}
				//m_MandolinComm.CreateParameterEditGet(&m_msgTx, m_nTxSequence,eTARGET_SNAPSHOT, puiPID,j);
				 m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);
				m_dlgParent->SetTimer(GETSTATE_TIMERID, 200, NULL);
			}
		}
		else
		{
			GetStateStop();
		}
	}



}

void AppProductDefs_OLY::SendStateStart(int nMode)
{
	// TODO: Add your control notification handler code here
	SendStateStop();

	m_nSendStatePos = 1;
	m_nSendStateTargetPos = 0;
	m_nSendStateMode = nMode; // normal 0, FIR 1
	m_nSendStateRequestSize = 20;
	SendStateSendNext();
}
void AppProductDefs_OLY::SendStateStop()
{
	if (m_dlgParent)
		m_dlgParent->KillTimer(SENDSTATE_TIMERID);
}

void AppProductDefs_OLY::SendStateSendNext()
{


	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;
	uint32 puiPID[100];
	intfloat jValues[100];
	int i,j;
	int nMaxPos;
	tTargetStateAddr  nTargetList[] = {
		//{eTARGET_STATUS,		&m_CurrentState.vpStatusParameterBuffer[0], 1},			
		{eTARGET_USER,			&m_CurrentState.vpUserParameterBuffer[0][0],PRESET_MAX_USER},				
		{eTARGET_PROFILE,		&m_CurrentState.vpProfileParameterBuffer[0][0],PRESET_MAX_PROFILE},			
		{eTARGET_THROW,			&m_CurrentState.vpThrowParameterBuffer[0][0],PRESET_MAX_THROW},				
		{eTARGET_VOICING,		&m_CurrentState.vpVoicingParameterBuffer[0][0],PRESET_MAX_VOICING},			
		//{eTARGET_FIR,			&m_CurrentState.vpFIRParameterBuffer[0][0],PRESET_MAX_FIR},	
		{eTARGET_XOVER,			&m_CurrentState.vpXOverParameterBuffer[0][0],PRESET_MAX_XOVER},		
		{eTARGET_AIRLOSS,		&m_CurrentState.vpAirlossParameterBuffer[0],1},		
		{eTARGET_NOISEGATE,		&m_CurrentState.vpNoisegateParameterBuffer[0],1},				
		{eTARGET_DEVICE,		&m_CurrentState.vpDeviceParameterBuffer[0], 1},					// needs to last because active throw send current throws back
	};

	tTargetStateAddr  nTargetListFIR[] = {
		{eTARGET_FIR,			&m_CurrentState.vpFIRParameterBuffer[0][0],PRESET_MAX_FIR},	
	};
	tTargetStateAddr* pCurrentTargetList;

	if (m_nSendStateMode == 0)
	{
		nMaxPos = sizeof(nTargetList)/sizeof(tTargetStateAddr);
		pCurrentTargetList = nTargetList;
	}
	else
	{
		nMaxPos = sizeof(nTargetListFIR)/sizeof(tTargetStateAddr);
		pCurrentTargetList = nTargetListFIR;
	}

	if (m_nSendStateTargetPos < nMaxPos)
	{
		GetParameterDescTable((pCurrentTargetList + m_nSendStateTargetPos)->nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);

		if (!pParameterDesc) return;

		if (m_nSendStatePos >= nTableLength)
		{
			m_nSendStateTargetPos++;
			m_nSendStatePos = 1;
			// re get this stuff since it will have changed
			GetParameterDescTable((pCurrentTargetList + m_nSendStateTargetPos)->nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);
		}

		if ((m_nSendStatePos < nTableLength) && (m_nSendStateTargetPos < nMaxPos))
		{
			i = m_nSendStatePos; 
			j = 0;
			while((m_nSendStatePos<nTableLength) && (j < 40))
			{
				puiPID[j] = m_nSendStatePos;
				jValues[j].vp = (pCurrentTargetList + m_nSendStateTargetPos)->ppAddr[m_nSendStatePos];
				m_nSendStatePos++;
				j++;
			}

			if (m_dlgParent)
			{
				m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
		
				m_dlgParent->m_MandolinComm.CreateParameterEditSet(&m_dlgParent->m_msgTx, m_dlgParent->m_nTxSequence,(pCurrentTargetList + m_nSendStateTargetPos)->nTarget,puiPID, jValues ,j,false);
				m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);
				m_dlgParent->SetTimer(SENDSTATE_TIMERID, 300, NULL);
			}
		}
	}

}



void AppProductDefs_OLY::ParameterAdjustSetComboBox(CComboBox** pCombo,int nNum)
{
	int i;
	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;
	int nIndex;
	int nTargetList[] =
	{
		eTARGET_DEVICE,	
		eTARGET_STATUS,	
		eTARGET_USER,		
		eTARGET_PROFILE,	
		eTARGET_THROW,		
		eTARGET_VOICING,	
		eTARGET_XOVER,		
		eTARGET_AIRLOSS,		
		eTARGET_NOISEGATE,				
		eTARGET_FIR,
	};


	for(int j=0;j<sizeof(nTargetList)/sizeof(int);j++)
	{


		GetParameterDescTable(nTargetList[j],&pParameterDesc,&nTableLength,&nXMLVersion);
		if (pParameterDesc)
		{
			for(int n=0;n<nTableLength;n++)
			{
//				if ((strncmp(pParameterDesc->pName, "FIR",3) != 0) && (strstr(pParameterDesc->pName, "NULL") == 0))  // starts with FIR
				if ((strstr(pParameterDesc->pName, "NULL") == 0))
				{
					for(i=0;i<nNum;i++)
					{
						if (pCombo[i]) 
						{
							nIndex = pCombo[i]->AddString(pParameterDesc->pName);
							//pCombo->SetItemData(nSel,n)
						}
					}
				}
				pParameterDesc++;
			}
		}
	}

}


// Level Adjust

ControlDesc gOLYLevelSwitchDescList[] = {
	{eTARGET_USER,		GetParamID_SpeakerMute(1), IDC_CHECK_INPUTMUTE1},		// 	
	{eTARGET_USER,		GetParamID_SpeakerInvert(1), IDC_CHECK_INPUTPHASE1},	// 
	{eTARGET_VOICING,	GetParamID_HfMute(1), IDC_CHECK_OUTPUTMUTE1},		// 	
	{eTARGET_VOICING,	0, IDC_CHECK_OUTPUTPHASE1},	// 
	{eTARGET_VOICING,	GetParamID_MfMute(1), IDC_CHECK_OUTPUTMUTE2},		// 	
	{eTARGET_VOICING,	0, IDC_CHECK_OUTPUTPHASE2},	// 
	{eTARGET_VOICING,	GetParamID_LfMute(1), IDC_CHECK_OUTPUTMUTE3},		// 	
	{eTARGET_VOICING,	0, IDC_CHECK_OUTPUTPHASE3},	// 
};

ControlDesc gOLYLevelFaderDescList[] = {
	{eTARGET_USER,		GetParamID_SpeakerFader(1), IDC_FADER_INPUTGAIN1},		// 	
	{eTARGET_VOICING,	GetParamID_HfFader(1), IDC_FADER_OUTPUTGAIN1},	// 
	{eTARGET_VOICING,	GetParamID_MfFader(1), IDC_FADER_OUTPUTGAIN2},	// 
	{eTARGET_VOICING,	GetParamID_LfFader(1), IDC_FADER_OUTPUTGAIN3},	// 
};
ControlDesc gOLYLevelEditBoxDescList[] = {
	{eTARGET_USER,		GetParamID_SpeakerFader(1), IDC_EDIT_INPUTGAIN1},		// 	
	{eTARGET_VOICING,	GetParamID_HfFader(1), IDC_EDIT_OUTPUTGAIN1},	// 
	{eTARGET_VOICING,	GetParamID_MfFader(1), IDC_EDIT_OUTPUTGAIN2},	// 
	{eTARGET_VOICING,	GetParamID_LfFader(1), IDC_EDIT_OUTPUTGAIN3},	// 
};



tStringIDC gOLYTitleList[] = {
	{"IN1",		IDC_LEVELADJUST_TITLE_INPUT1 }, 
	{"HF",		IDC_LEVELADJUST_TITLE_OUTPUT1},
	{"MF",		IDC_LEVELADJUST_TITLE_OUTPUT2},
	{"LF",		IDC_LEVELADJUST_TITLE_OUTPUT3},
};

int AppProductDefs_OLY::GetLevelSwitchDescList(ControlDesc** pList)
{
	*pList = gOLYLevelSwitchDescList;
	return sizeof(gOLYLevelSwitchDescList)/sizeof(ControlDesc);
}

int AppProductDefs_OLY::GetLevelFaderDescList(ControlDesc** pList)
{
	*pList = gOLYLevelFaderDescList;
	return sizeof(gOLYLevelFaderDescList)/sizeof(ControlDesc);
}

int AppProductDefs_OLY::GetLevelEditBoxDescList(ControlDesc** pList)
{
	*pList = gOLYLevelEditBoxDescList;
	return sizeof(gOLYLevelEditBoxDescList)/sizeof(ControlDesc);
}

int AppProductDefs_OLY::GetLevelTitleList(tStringIDC** pList)
{
	*pList = gOLYTitleList;
	return sizeof(gOLYTitleList)/sizeof(tStringIDC);
}


// -------------------------------------------------------------------------------------
/*
ControlDesc gOLYMetersDescList[] = {
	{eTARGET_METERS, GetMeterID_InputIn(1),					IDC_METERS_LEVEL1},		// 	
	{eTARGET_METERS, GetMeterID_HfOut(1),					IDC_METERS_LEVEL2},	// 
	{eTARGET_METERS, GetMeterID_HfSoftlimitGainreduce(1),	IDC_METERS_LEVEL3},	// 
	{eTARGET_METERS, GetMeterID_MfOut(1),					IDC_METERS_LEVEL4},	// 
	{eTARGET_METERS, GetMeterID_MfSoftlimitGainreduce(1),	IDC_METERS_LEVEL5},	// 
	{eTARGET_METERS, GetMeterID_LfOut(1),					IDC_METERS_LEVEL6},	// 
	{eTARGET_METERS, GetMeterID_LfKlimiterGainreduce(1),	IDC_METERS_LEVEL7},	// 
};

tStringIDC gOLYMeterTitleList[] = {
	{"IN",		IDC_METERS_TITLE1}, 
	{"HF",		IDC_METERS_TITLE2},
	{"HFGR",	IDC_METERS_TITLE3},
	{"MF",		IDC_METERS_TITLE4},
	{"MFGR",	IDC_METERS_TITLE5},
	{"LF",		IDC_METERS_TITLE6},
	{"LFGR",	IDC_METERS_TITLE7},
};
*/
ControlDesc gOLYMetersDescList[] = {
	{eTARGET_METERS, eMID_OLYspeaker1_INPUT_IN,						IDC_METERS_LEVEL1,	IDC_VALUE_METER1},		// 	
	{eTARGET_METERS, eMID_OLYspeaker1_SPEAKER_IN,					IDC_METERS_LEVEL2,	IDC_VALUE_METER2},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_GROUP_SOFTLIMIT_GAINREDUCE,	IDC_METERS_LEVEL3,	IDC_VALUE_METER3},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_GROUP_SOFTLIMIT_OUT,			IDC_METERS_LEVEL4,	IDC_VALUE_METER4},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_HF_DYNEQ1_GAIN,				IDC_METERS_LEVEL5,	IDC_VALUE_METER5},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_HF_DYNEQ2_GAIN,				IDC_METERS_LEVEL6,	IDC_VALUE_METER6},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_HF_SOFTLIMIT_IN,				IDC_METERS_LEVEL7,	IDC_VALUE_METER7},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_HF_SOFTLIMIT_GAINREDUCE,		IDC_METERS_LEVEL8,	IDC_VALUE_METER8},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_HF_SOFTLIMIT_OUT,				IDC_METERS_LEVEL9,	IDC_VALUE_METER9},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_HF_KLIMITER_GAINREDUCE,		IDC_METERS_LEVEL10,	IDC_VALUE_METER10},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_HF_KLIMITER_OUT,				IDC_METERS_LEVEL11,	IDC_VALUE_METER11},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_HF_OUT,						IDC_METERS_LEVEL12,	IDC_VALUE_METER12},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_MF_DYNEQ1_GAIN,				IDC_METERS_LEVEL13,	IDC_VALUE_METER13},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_MF_DYNEQ2_GAIN,				IDC_METERS_LEVEL14,	IDC_VALUE_METER14},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_MF_SOFTLIMIT_IN,				IDC_METERS_LEVEL15,	IDC_VALUE_METER15},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_MF_SOFTLIMIT_GAINREDUCE,		IDC_METERS_LEVEL16,	IDC_VALUE_METER16},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_MF_SOFTLIMIT_OUT,				IDC_METERS_LEVEL17,	IDC_VALUE_METER17},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_MF_KLIMITER_GAINREDUCE,		IDC_METERS_LEVEL18,	IDC_VALUE_METER18},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_MF_KLIMITER_OUT,				IDC_METERS_LEVEL19,	IDC_VALUE_METER19},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_MF_OUT,						IDC_METERS_LEVEL20,	IDC_VALUE_METER20},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_LF_DYNEQ1_GAIN,				IDC_METERS_LEVEL21,	IDC_VALUE_METER21},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_LF_DYNEQ2_GAIN,				IDC_METERS_LEVEL22,	IDC_VALUE_METER22},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_LF_SOFTLIMIT_IN,				IDC_METERS_LEVEL23,	IDC_VALUE_METER23},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_LF_SOFTLIMIT_GAINREDUCE,		IDC_METERS_LEVEL24,	IDC_VALUE_METER24},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_LF_SOFTLIMIT_OUT,				IDC_METERS_LEVEL25,	IDC_VALUE_METER25},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_LF_KLIMITER_GAINREDUCE,		IDC_METERS_LEVEL26,	IDC_VALUE_METER26},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_LF_KLIMITER_OUT,				IDC_METERS_LEVEL27,	IDC_VALUE_METER27},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_LF_OUT,						IDC_METERS_LEVEL28,	IDC_VALUE_METER28},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_INPUT_GATE_IN,				IDC_METERS_LEVEL29,	IDC_VALUE_METER29},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_INPUT_GATE_GAINREDUCE,		IDC_METERS_LEVEL30,	IDC_VALUE_METER30},	// 
	{eTARGET_METERS, eMID_OLYspeaker1_INPUT_GATE_OUT,				IDC_METERS_LEVEL31,	IDC_VALUE_METER31},	// 
};

tStringIDC gOLYMeterTitleList[] = {
	{"In",		IDC_METERS_TITLE1}, 
	{"User",	IDC_METERS_TITLE2},
	{"GR",		IDC_METERS_TITLE3},
	{"Out",		IDC_METERS_TITLE4},
	{"Dyn1",	IDC_METERS_TITLE5},
	{"Dyn2",	IDC_METERS_TITLE6},
	{"LimIn",	IDC_METERS_TITLE7},
	{"LimGR",	IDC_METERS_TITLE8},
	{"LimO",	IDC_METERS_TITLE9},
	{"KLmGR",	IDC_METERS_TITLE10},
	{"KLmO",	IDC_METERS_TITLE11},
	{"Out",		IDC_METERS_TITLE12},
	{"Dyn1",	IDC_METERS_TITLE13},
	{"Dyn2",	IDC_METERS_TITLE14},
	{"LimIn",	IDC_METERS_TITLE15},
	{"LimGR",	IDC_METERS_TITLE16},
	{"LimO",	IDC_METERS_TITLE17},
	{"KLmGR",	IDC_METERS_TITLE18},
	{"KLmO",	IDC_METERS_TITLE19},
	{"Out",		IDC_METERS_TITLE20},
	{"Dyn1",	IDC_METERS_TITLE21},
	{"Dyn2",	IDC_METERS_TITLE22},
	{"LimIn",	IDC_METERS_TITLE23},
	{"LimGR",	IDC_METERS_TITLE24},
	{"LimO",	IDC_METERS_TITLE25},
	{"KLmGR",	IDC_METERS_TITLE26},
	{"KLmO",	IDC_METERS_TITLE27},
	{"Out",		IDC_METERS_TITLE28},
	{"GtIn",	IDC_METERS_TITLE29},
	{"GtGR",	IDC_METERS_TITLE30},
	{"GtO",		IDC_METERS_TITLE31},
};

int AppProductDefs_OLY::GetMetersDescList(ControlDesc** pList)
{
	*pList = gOLYMetersDescList;
	return sizeof(gOLYMetersDescList)/sizeof(ControlDesc);
}
int AppProductDefs_OLY::GetMetersTitleList(tStringIDC** pList)
{
	*pList = gOLYMeterTitleList;
	return sizeof(gOLYMeterTitleList)/sizeof(tStringIDC);
}


// ------------------------------------------------------------
// OLY Defines
// ------------------------------------------------------------

// ------------------------------
// Parameter mapping
// ------------------------------

char* EQGraph_OLY_VOICING_HF_PEQ_POST[4*EQGRAPH_MAX_BANDS+1] = {
"VOICING_HF_PEQ_POST_BAND1_TYPE",  
"VOICING_HF_PEQ_POST_BAND1_GAIN",  
"VOICING_HF_PEQ_POST_BAND1_FREQ",  
"VOICING_HF_PEQ_POST_BAND1_Q",     
"VOICING_HF_PEQ_POST_BAND2_TYPE",  
"VOICING_HF_PEQ_POST_BAND2_GAIN",  
"VOICING_HF_PEQ_POST_BAND2_FREQ",  
"VOICING_HF_PEQ_POST_BAND2_Q",     
"VOICING_HF_PEQ_POST_BAND3_TYPE",  
"VOICING_HF_PEQ_POST_BAND3_GAIN",  
"VOICING_HF_PEQ_POST_BAND3_FREQ",  
"VOICING_HF_PEQ_POST_BAND3_Q",     
"VOICING_HF_PEQ_POST_BAND4_TYPE",  
"VOICING_HF_PEQ_POST_BAND4_GAIN",  
"VOICING_HF_PEQ_POST_BAND4_FREQ",  
"VOICING_HF_PEQ_POST_BAND4_Q", 
""
};

char* EQGraph_OLY_VOICING_MF_PEQ_POST[4*EQGRAPH_MAX_BANDS+1] = {
"VOICING_MF_PEQ_POST_BAND1_TYPE",  
"VOICING_MF_PEQ_POST_BAND1_GAIN",  
"VOICING_MF_PEQ_POST_BAND1_FREQ",  
"VOICING_MF_PEQ_POST_BAND1_Q",     
"VOICING_MF_PEQ_POST_BAND2_TYPE",  
"VOICING_MF_PEQ_POST_BAND2_GAIN",  
"VOICING_MF_PEQ_POST_BAND2_FREQ",  
"VOICING_MF_PEQ_POST_BAND2_Q",     
"VOICING_MF_PEQ_POST_BAND3_TYPE",  
"VOICING_MF_PEQ_POST_BAND3_GAIN",  
"VOICING_MF_PEQ_POST_BAND3_FREQ",  
"VOICING_MF_PEQ_POST_BAND3_Q",     
"VOICING_MF_PEQ_POST_BAND4_TYPE",  
"VOICING_MF_PEQ_POST_BAND4_GAIN",  
"VOICING_MF_PEQ_POST_BAND4_FREQ",  
"VOICING_MF_PEQ_POST_BAND4_Q", 
""
};

char* EQGraph_OLY_VOICING_LF_PEQ_POST[4*EQGRAPH_MAX_BANDS+1] = {
"VOICING_LF_PEQ_POST_BAND1_TYPE",  
"VOICING_LF_PEQ_POST_BAND1_GAIN",  
"VOICING_LF_PEQ_POST_BAND1_FREQ",  
"VOICING_LF_PEQ_POST_BAND1_Q",     
"VOICING_LF_PEQ_POST_BAND2_TYPE",  
"VOICING_LF_PEQ_POST_BAND2_GAIN",  
"VOICING_LF_PEQ_POST_BAND2_FREQ",  
"VOICING_LF_PEQ_POST_BAND2_Q",     
"VOICING_LF_PEQ_POST_BAND3_TYPE",  
"VOICING_LF_PEQ_POST_BAND3_GAIN",  
"VOICING_LF_PEQ_POST_BAND3_FREQ",  
"VOICING_LF_PEQ_POST_BAND3_Q",     
"VOICING_LF_PEQ_POST_BAND4_TYPE",  
"VOICING_LF_PEQ_POST_BAND4_GAIN",  
"VOICING_LF_PEQ_POST_BAND4_FREQ",  
"VOICING_LF_PEQ_POST_BAND4_Q", 
""
};

char* EQGraph_OLY_VOICING_GLOBAL_PEQ[4*EQGRAPH_MAX_BANDS+1] = {
	"VOICING_GLOBAL_PEQ_BAND1_TYPE",  
	"VOICING_GLOBAL_PEQ_BAND1_GAIN",  
	"VOICING_GLOBAL_PEQ_BAND1_FREQ",  
	"VOICING_GLOBAL_PEQ_BAND1_Q",     
	"VOICING_GLOBAL_PEQ_BAND2_TYPE",  
	"VOICING_GLOBAL_PEQ_BAND2_GAIN",  
	"VOICING_GLOBAL_PEQ_BAND2_FREQ",  
	"VOICING_GLOBAL_PEQ_BAND2_Q",     
	"VOICING_GLOBAL_PEQ_BAND3_TYPE",  
	"VOICING_GLOBAL_PEQ_BAND3_GAIN",  
	"VOICING_GLOBAL_PEQ_BAND3_FREQ",  
	"VOICING_GLOBAL_PEQ_BAND3_Q",     
	"VOICING_GLOBAL_PEQ_BAND4_TYPE",  
	"VOICING_GLOBAL_PEQ_BAND4_GAIN",  
	"VOICING_GLOBAL_PEQ_BAND4_FREQ",  
	"VOICING_GLOBAL_PEQ_BAND4_Q",     
	"VOICING_GLOBAL_PEQ_BAND5_TYPE",  
	"VOICING_GLOBAL_PEQ_BAND5_GAIN",  
	"VOICING_GLOBAL_PEQ_BAND5_FREQ",  
	"VOICING_GLOBAL_PEQ_BAND5_Q",     
	"VOICING_GLOBAL_PEQ_BAND6_TYPE",  
	"VOICING_GLOBAL_PEQ_BAND6_GAIN",  
	"VOICING_GLOBAL_PEQ_BAND6_FREQ",  
	"VOICING_GLOBAL_PEQ_BAND6_Q",     
	"VOICING_GLOBAL_PEQ_BAND7_TYPE",  
	"VOICING_GLOBAL_PEQ_BAND7_GAIN",  
	"VOICING_GLOBAL_PEQ_BAND7_FREQ",  
	"VOICING_GLOBAL_PEQ_BAND7_Q",     
	"VOICING_GLOBAL_PEQ_BAND8_TYPE",  
	"VOICING_GLOBAL_PEQ_BAND8_GAIN",  
	"VOICING_GLOBAL_PEQ_BAND8_FREQ",  
	"VOICING_GLOBAL_PEQ_BAND8_Q",     
	"VOICING_GLOBAL_PEQ_BAND9_TYPE",  
	"VOICING_GLOBAL_PEQ_BAND9_GAIN",  
	"VOICING_GLOBAL_PEQ_BAND9_FREQ",  
	"VOICING_GLOBAL_PEQ_BAND9_Q",     
	"VOICING_GLOBAL_PEQ_BAND10_TYPE", 
	"VOICING_GLOBAL_PEQ_BAND10_GAIN", 
	"VOICING_GLOBAL_PEQ_BAND10_FREQ", 
	"VOICING_GLOBAL_PEQ_BAND10_Q",    
	"VOICING_GLOBAL_PEQ_BAND11_TYPE", 
	"VOICING_GLOBAL_PEQ_BAND11_GAIN", 
	"VOICING_GLOBAL_PEQ_BAND11_FREQ", 
	"VOICING_GLOBAL_PEQ_BAND11_Q",    
	"VOICING_GLOBAL_PEQ_BAND12_TYPE", 
	"VOICING_GLOBAL_PEQ_BAND12_GAIN", 
	"VOICING_GLOBAL_PEQ_BAND12_FREQ", 
	"VOICING_GLOBAL_PEQ_BAND12_Q",  
	""
};

char* EQGraph_OLY_VOICING_HF_PEQ[4*EQGRAPH_MAX_BANDS+1] = {
//	"VOICING_HF_PEQ_ENABLE",    
	"VOICING_HF_PEQ_BAND1_TYPE",		// must be type, gain, freq q
	"VOICING_HF_PEQ_BAND1_GAIN",
	"VOICING_HF_PEQ_BAND1_FREQ",
	"VOICING_HF_PEQ_BAND1_Q",   
	"VOICING_HF_PEQ_BAND2_TYPE",
	"VOICING_HF_PEQ_BAND2_GAIN",
	"VOICING_HF_PEQ_BAND2_FREQ",
	"VOICING_HF_PEQ_BAND2_Q",   
	"VOICING_HF_PEQ_BAND3_TYPE",
	"VOICING_HF_PEQ_BAND3_GAIN",
	"VOICING_HF_PEQ_BAND3_FREQ",
	"VOICING_HF_PEQ_BAND3_Q",   
	"VOICING_HF_PEQ_BAND4_TYPE",
	"VOICING_HF_PEQ_BAND4_GAIN",
	"VOICING_HF_PEQ_BAND4_FREQ",
	"VOICING_HF_PEQ_BAND4_Q",   
	"VOICING_HF_PEQ_BAND5_TYPE",
	"VOICING_HF_PEQ_BAND5_GAIN",
	"VOICING_HF_PEQ_BAND5_FREQ",
	"VOICING_HF_PEQ_BAND5_Q",   
	"VOICING_HF_PEQ_BAND6_TYPE",
	"VOICING_HF_PEQ_BAND6_GAIN",
	"VOICING_HF_PEQ_BAND6_FREQ",
	"VOICING_HF_PEQ_BAND6_Q",   
	"VOICING_HF_PEQ_BAND7_TYPE",
	"VOICING_HF_PEQ_BAND7_GAIN",
	"VOICING_HF_PEQ_BAND7_FREQ",
	"VOICING_HF_PEQ_BAND7_Q",   
	"VOICING_HF_PEQ_BAND8_TYPE",
	"VOICING_HF_PEQ_BAND8_GAIN",
	"VOICING_HF_PEQ_BAND8_FREQ",
	"VOICING_HF_PEQ_BAND8_Q",   
	"VOICING_HF_PEQ_BAND9_TYPE",
	"VOICING_HF_PEQ_BAND9_GAIN",
	"VOICING_HF_PEQ_BAND9_FREQ",
	"VOICING_HF_PEQ_BAND9_Q",   
	"VOICING_HF_PEQ_BAND10_TYPE",
	"VOICING_HF_PEQ_BAND10_GAIN",
	"VOICING_HF_PEQ_BAND10_FREQ",
	"VOICING_HF_PEQ_BAND10_Q",  
	"VOICING_HF_PEQ_BAND11_TYPE",
	"VOICING_HF_PEQ_BAND11_GAIN",
	"VOICING_HF_PEQ_BAND11_FREQ",
	"VOICING_HF_PEQ_BAND11_Q",  
	"VOICING_HF_PEQ_BAND12_TYPE",
	"VOICING_HF_PEQ_BAND12_GAIN",
	"VOICING_HF_PEQ_BAND12_FREQ",
	"VOICING_HF_PEQ_BAND12_Q", 
	"",
};

char* EQGraph_OLY_VOICING_MF_PEQ[4*EQGRAPH_MAX_BANDS+1] = {
//	"VOICING_MF_PEQ_ENABLE",    
	"VOICING_MF_PEQ_BAND1_TYPE",		// must be type, gain, freq q
	"VOICING_MF_PEQ_BAND1_GAIN",
	"VOICING_MF_PEQ_BAND1_FREQ",
	"VOICING_MF_PEQ_BAND1_Q",   
	"VOICING_MF_PEQ_BAND2_TYPE",
	"VOICING_MF_PEQ_BAND2_GAIN",
	"VOICING_MF_PEQ_BAND2_FREQ",
	"VOICING_MF_PEQ_BAND2_Q",   
	"VOICING_MF_PEQ_BAND3_TYPE",
	"VOICING_MF_PEQ_BAND3_GAIN",
	"VOICING_MF_PEQ_BAND3_FREQ",
	"VOICING_MF_PEQ_BAND3_Q",   
	"VOICING_MF_PEQ_BAND4_TYPE",
	"VOICING_MF_PEQ_BAND4_GAIN",
	"VOICING_MF_PEQ_BAND4_FREQ",
	"VOICING_MF_PEQ_BAND4_Q",   
	"VOICING_MF_PEQ_BAND5_TYPE",
	"VOICING_MF_PEQ_BAND5_GAIN",
	"VOICING_MF_PEQ_BAND5_FREQ",
	"VOICING_MF_PEQ_BAND5_Q",   
	"VOICING_MF_PEQ_BAND6_TYPE",
	"VOICING_MF_PEQ_BAND6_GAIN",
	"VOICING_MF_PEQ_BAND6_FREQ",
	"VOICING_MF_PEQ_BAND6_Q",   
	"VOICING_MF_PEQ_BAND7_TYPE",
	"VOICING_MF_PEQ_BAND7_GAIN",
	"VOICING_MF_PEQ_BAND7_FREQ",
	"VOICING_MF_PEQ_BAND7_Q",   
	"VOICING_MF_PEQ_BAND8_TYPE",
	"VOICING_MF_PEQ_BAND8_GAIN",
	"VOICING_MF_PEQ_BAND8_FREQ",
	"VOICING_MF_PEQ_BAND8_Q",   
	"VOICING_MF_PEQ_BAND9_TYPE",
	"VOICING_MF_PEQ_BAND9_GAIN",
	"VOICING_MF_PEQ_BAND9_FREQ",
	"VOICING_MF_PEQ_BAND9_Q",   
	"VOICING_MF_PEQ_BAND10_TYPE",
	"VOICING_MF_PEQ_BAND10_GAIN",
	"VOICING_MF_PEQ_BAND10_FREQ",
	"VOICING_MF_PEQ_BAND10_Q",  
	"VOICING_MF_PEQ_BAND11_TYPE",
	"VOICING_MF_PEQ_BAND11_GAIN",
	"VOICING_MF_PEQ_BAND11_FREQ",
	"VOICING_MF_PEQ_BAND11_Q",  
	"VOICING_MF_PEQ_BAND12_TYPE",
	"VOICING_MF_PEQ_BAND12_GAIN",
	"VOICING_MF_PEQ_BAND12_FREQ",
	"VOICING_MF_PEQ_BAND12_Q", 
	"",
};


char* EQGraph_OLY_VOICING_LF_PEQ[4*EQGRAPH_MAX_BANDS+1] = {
//	"VOICING_LF_PEQ_ENABLE",    
	"VOICING_LF_PEQ_BAND1_TYPE",		// must be type, gain, freq q
	"VOICING_LF_PEQ_BAND1_GAIN",
	"VOICING_LF_PEQ_BAND1_FREQ",
	"VOICING_LF_PEQ_BAND1_Q",   
	"VOICING_LF_PEQ_BAND2_TYPE",
	"VOICING_LF_PEQ_BAND2_GAIN",
	"VOICING_LF_PEQ_BAND2_FREQ",
	"VOICING_LF_PEQ_BAND2_Q",   
	"VOICING_LF_PEQ_BAND3_TYPE",
	"VOICING_LF_PEQ_BAND3_GAIN",
	"VOICING_LF_PEQ_BAND3_FREQ",
	"VOICING_LF_PEQ_BAND3_Q",   
	"VOICING_LF_PEQ_BAND4_TYPE",
	"VOICING_LF_PEQ_BAND4_GAIN",
	"VOICING_LF_PEQ_BAND4_FREQ",
	"VOICING_LF_PEQ_BAND4_Q",   
	"VOICING_LF_PEQ_BAND5_TYPE",
	"VOICING_LF_PEQ_BAND5_GAIN",
	"VOICING_LF_PEQ_BAND5_FREQ",
	"VOICING_LF_PEQ_BAND5_Q",   
	"VOICING_LF_PEQ_BAND6_TYPE",
	"VOICING_LF_PEQ_BAND6_GAIN",
	"VOICING_LF_PEQ_BAND6_FREQ",
	"VOICING_LF_PEQ_BAND6_Q",   
	"VOICING_LF_PEQ_BAND7_TYPE",
	"VOICING_LF_PEQ_BAND7_GAIN",
	"VOICING_LF_PEQ_BAND7_FREQ",
	"VOICING_LF_PEQ_BAND7_Q",   
	"VOICING_LF_PEQ_BAND8_TYPE",
	"VOICING_LF_PEQ_BAND8_GAIN",
	"VOICING_LF_PEQ_BAND8_FREQ",
	"VOICING_LF_PEQ_BAND8_Q",   
	"VOICING_LF_PEQ_BAND9_TYPE",
	"VOICING_LF_PEQ_BAND9_GAIN",
	"VOICING_LF_PEQ_BAND9_FREQ",
	"VOICING_LF_PEQ_BAND9_Q",   
	"VOICING_LF_PEQ_BAND10_TYPE",
	"VOICING_LF_PEQ_BAND10_GAIN",
	"VOICING_LF_PEQ_BAND10_FREQ",
	"VOICING_LF_PEQ_BAND10_Q",  
	"VOICING_LF_PEQ_BAND11_TYPE",
	"VOICING_LF_PEQ_BAND11_GAIN",
	"VOICING_LF_PEQ_BAND11_FREQ",
	"VOICING_LF_PEQ_BAND11_Q",  
	"VOICING_LF_PEQ_BAND12_TYPE",
	"VOICING_LF_PEQ_BAND12_GAIN",
	"VOICING_LF_PEQ_BAND12_FREQ",
	"VOICING_LF_PEQ_BAND12_Q", 
	"",
};


char* EQGraph_OLY_USER_GROUP_PEQ[7*EQGRAPH_MAX_BANDS+1] = {
"USER_GROUP_PEQ_BAND1_TYPE",  
"USER_GROUP_PEQ_BAND1_GAIN",  
"USER_GROUP_PEQ_BAND1_FREQ",  
"USER_GROUP_PEQ_BAND1_Q",     
"USER_GROUP_PEQ_BAND2_TYPE",  
"USER_GROUP_PEQ_BAND2_GAIN",  
"USER_GROUP_PEQ_BAND2_FREQ",  
"USER_GROUP_PEQ_BAND2_Q",     
"USER_GROUP_PEQ_BAND3_TYPE",  
"USER_GROUP_PEQ_BAND3_GAIN",  
"USER_GROUP_PEQ_BAND3_FREQ",  
"USER_GROUP_PEQ_BAND3_Q",     
"USER_GROUP_PEQ_BAND4_TYPE",  
"USER_GROUP_PEQ_BAND4_GAIN",  
"USER_GROUP_PEQ_BAND4_FREQ",  
"USER_GROUP_PEQ_BAND4_Q", 
"USER_GROUP_PEQ_BAND5_TYPE",  
"USER_GROUP_PEQ_BAND5_GAIN",  
"USER_GROUP_PEQ_BAND5_FREQ",  
"USER_GROUP_PEQ_BAND5_Q",
"USER_GROUP_PEQ_BAND6_TYPE",  
"USER_GROUP_PEQ_BAND6_GAIN",  
"USER_GROUP_PEQ_BAND6_FREQ",  
"USER_GROUP_PEQ_BAND6_Q",
"USER_GROUP_PEQ_BAND7_TYPE",  
"USER_GROUP_PEQ_BAND7_GAIN",  
"USER_GROUP_PEQ_BAND7_FREQ",  
"USER_GROUP_PEQ_BAND7_Q",
""
};

char* EQGraph_OLY_SPEAKER_PROFILE_PEQ[8*EQGRAPH_MAX_BANDS+1] = {
	"PROFILE_PROFILE_PEQ_BAND1_TYPE",      
	"PROFILE_PROFILE_PEQ_BAND1_GAIN",      
	"PROFILE_PROFILE_PEQ_BAND1_FREQ",      
	"PROFILE_PROFILE_PEQ_BAND1_Q",         
	"PROFILE_PROFILE_PEQ_BAND2_TYPE",      
	"PROFILE_PROFILE_PEQ_BAND2_GAIN",      
	"PROFILE_PROFILE_PEQ_BAND2_FREQ",      
	"PROFILE_PROFILE_PEQ_BAND2_Q",         
	"PROFILE_PROFILE_PEQ_BAND3_TYPE",      
	"PROFILE_PROFILE_PEQ_BAND3_GAIN",      
	"PROFILE_PROFILE_PEQ_BAND3_FREQ",      
	"PROFILE_PROFILE_PEQ_BAND3_Q",         
	"PROFILE_PROFILE_PEQ_BAND4_TYPE",      
	"PROFILE_PROFILE_PEQ_BAND4_GAIN",      
	"PROFILE_PROFILE_PEQ_BAND4_FREQ",      
	"PROFILE_PROFILE_PEQ_BAND4_Q",         
	"PROFILE_PROFILE_PEQ_BAND5_TYPE",      
	"PROFILE_PROFILE_PEQ_BAND5_GAIN",      
	"PROFILE_PROFILE_PEQ_BAND5_FREQ",      
	"PROFILE_PROFILE_PEQ_BAND5_Q",         
	"PROFILE_PROFILE_PEQ_BAND6_TYPE",      
	"PROFILE_PROFILE_PEQ_BAND6_GAIN",      
	"PROFILE_PROFILE_PEQ_BAND6_FREQ",      
	"PROFILE_PROFILE_PEQ_BAND6_Q",         
	"PROFILE_PROFILE_PEQ_BAND7_TYPE",      
	"PROFILE_PROFILE_PEQ_BAND7_GAIN",      
	"PROFILE_PROFILE_PEQ_BAND7_FREQ",      
	"PROFILE_PROFILE_PEQ_BAND7_Q",         
	"PROFILE_PROFILE_PEQ_BAND8_TYPE",      
	"PROFILE_PROFILE_PEQ_BAND8_GAIN",      
	"PROFILE_PROFILE_PEQ_BAND8_FREQ",      
	"PROFILE_PROFILE_PEQ_BAND8_Q",         
	""
};

char* EQGraph_OLY_THROW_PEQ[8*EQGRAPH_MAX_BANDS+1] = {
	"THROW_THROW_PEQ_BAND1_TYPE",          
	"THROW_THROW_PEQ_BAND1_GAIN",          
	"THROW_THROW_PEQ_BAND1_FREQ",          
	"THROW_THROW_PEQ_BAND1_Q",             
	"THROW_THROW_PEQ_BAND2_TYPE",          
	"THROW_THROW_PEQ_BAND2_GAIN",          
	"THROW_THROW_PEQ_BAND2_FREQ",          
	"THROW_THROW_PEQ_BAND2_Q",             
	"THROW_THROW_PEQ_BAND3_TYPE",          
	"THROW_THROW_PEQ_BAND3_GAIN",          
	"THROW_THROW_PEQ_BAND3_FREQ",          
	"THROW_THROW_PEQ_BAND3_Q",             
	"THROW_THROW_PEQ_BAND4_TYPE",          
	"THROW_THROW_PEQ_BAND4_GAIN",          
	"THROW_THROW_PEQ_BAND4_FREQ",          
	"THROW_THROW_PEQ_BAND4_Q",             
	""
};

//char* EQGraph_OLY_AIRLOSS_PEQ[1*EQGRAPH_MAX_BANDS+1] = {
//"USER_SPEAKER_PEQ_BAND8_TYPE",         
//"USER_SPEAKER_PEQ_BAND8_GAIN",         
//"USER_SPEAKER_PEQ_BAND8_FREQ",         
//"USER_SPEAKER_PEQ_BAND8_Q",   
//""
//};

char* EQGraph_OLY_USER_SPEAKER_XOVER_PEQ[4*EQGRAPH_MAX_BANDS+1] = {
"USER_SPEAKER_XOVER_BAND1_TYPE",       
"USER_SPEAKER_XOVER_BAND1_GAIN",       
"USER_SPEAKER_XOVER_BAND1_FREQ",       
"USER_SPEAKER_XOVER_BAND1_Q",          
"USER_SPEAKER_XOVER_BAND2_TYPE",       
"USER_SPEAKER_XOVER_BAND2_GAIN",       
"USER_SPEAKER_XOVER_BAND2_FREQ",       
"USER_SPEAKER_XOVER_BAND2_Q",          
"USER_SPEAKER_XOVER_BAND3_TYPE",       
"USER_SPEAKER_XOVER_BAND3_GAIN",       
"USER_SPEAKER_XOVER_BAND3_FREQ",       
"USER_SPEAKER_XOVER_BAND3_Q",          
"USER_SPEAKER_XOVER_BAND4_TYPE",       
"USER_SPEAKER_XOVER_BAND4_GAIN",       
"USER_SPEAKER_XOVER_BAND4_FREQ",       
"USER_SPEAKER_XOVER_BAND4_Q",          
""
};    

// /*    1 */ NAMESIN("XOVER_SPEAKER_XOVER_TYPE")            INFO(eTYPE_SPEAKER,  1, eEFFECT_XOVER,      1, eQUAL_NULL,   0, ePARAM_TYPE,         ePID_OLYspeaker1_XOVER_INVALID,                   ePID_OLYspeaker1_XOVER_INVALID)                   eFORMAT_SIGNED,             0,          12,           1 }, // eq, hi_pass 
//{ /*    2 */ NAMESIN("XOVER_SPEAKER_XOVER_ENABLE")          INFO(eTYPE_SPEAKER,  1, eEFFECT_XOVER,      1, eQUAL_NULL,   0, ePARAM_ENABLE,       ePID_OLYspeaker1_XOVER_INVALID,                   ePID_OLYspeaker1_XOVER_INVALID)                   eFORMAT_BOOL,               0,           1,           0 },
//{ /*    3 */ NAMESIN("XOVER_SPEAKER_XOVER_FREQ")            INFO(eTYPE_SPEAKER,  1, eEFFECT_XOVER,      1, eQUAL_NULL,   0, ePARAM_FREQ,         ePID_OLYspeaker1_XOVER_INVALID,                   ePID_OLYspeaker1_XOVER_INVALID)                   eFORMAT_FLOAT,     0x41A00000,  0x469C4000,  0x41A00000 }, // 20.000000,	20000.000000,	20.000000
//{ /*    4 */ NAMESIN("XOVER_SPEAKER_XOVER_SLOPE")           INFO(eTYPE_SPEAKER,  1, eEFFECT_XOVER,      1, eQUAL_NULL,   0, ePARAM_SLOPE,        ePID_OLYspeaker1_XOVER_INVALID,                   ePID_OLYspeaker1_XOVER_INVALID)                   eFORMAT_SIGNED,             0,           7,           2 }, // slope, slope_18 
//{ /*    5 */ NAMESIN("XOVER_SPEAKER_XOVER_RESPONSE")        INFO(eTYPE_SPEAKER,  1, eEFFECT_XOVER,      1, eQUAL_NULL,   0, ePARAM_RESPONSE,     ePID_OLYspeaker1_XOVER_INVALID,                   ePID_OLYspeaker1_XOVER_INVALID)                   eFORMAT_SIGNED,             0,           2,           0 }, // response, butterworth 
//{ /*    6 */ NAMESIN("XOVER_SPEAKER_XOVER_HPFOUTENABLE")    INFO(eTYPE_SPEAKER,  1, eEFFECT_XOVER,      1, eQUAL_NULL,   0, ePARAM_HPFOUTENABLE, ePID_OLYspeaker1_XOVER_INVALID,                   ePID_OLYspeaker1_XOVER_INVALID)                   eFORMAT_BOOL,               0,           1,           0 },


char* EQGraph_OLY_USER_SPEAKER_PEQ[4*EQGRAPH_MAX_BANDS+1] = {
"USER_SPEAKER_PEQ_BAND1_TYPE",         
"USER_SPEAKER_PEQ_BAND1_GAIN",         
"USER_SPEAKER_PEQ_BAND1_FREQ",         
"USER_SPEAKER_PEQ_BAND1_Q",            
"USER_SPEAKER_PEQ_BAND2_TYPE",         
"USER_SPEAKER_PEQ_BAND2_GAIN",         
"USER_SPEAKER_PEQ_BAND2_FREQ",         
"USER_SPEAKER_PEQ_BAND2_Q",            
"USER_SPEAKER_PEQ_BAND3_TYPE",         
"USER_SPEAKER_PEQ_BAND3_GAIN",         
"USER_SPEAKER_PEQ_BAND3_FREQ",         
"USER_SPEAKER_PEQ_BAND3_Q",            
"USER_SPEAKER_PEQ_BAND4_TYPE",         
"USER_SPEAKER_PEQ_BAND4_GAIN",         
"USER_SPEAKER_PEQ_BAND4_FREQ",         
"USER_SPEAKER_PEQ_BAND4_Q",            
"USER_SPEAKER_PEQ_BAND5_TYPE",         
"USER_SPEAKER_PEQ_BAND5_GAIN",         
"USER_SPEAKER_PEQ_BAND5_FREQ",         
"USER_SPEAKER_PEQ_BAND5_Q",            
"USER_SPEAKER_PEQ_BAND6_TYPE",         
"USER_SPEAKER_PEQ_BAND6_GAIN",         
"USER_SPEAKER_PEQ_BAND6_FREQ",         
"USER_SPEAKER_PEQ_BAND6_Q",            
"USER_SPEAKER_PEQ_BAND7_TYPE",         
"USER_SPEAKER_PEQ_BAND7_GAIN",         
"USER_SPEAKER_PEQ_BAND7_FREQ",         
"USER_SPEAKER_PEQ_BAND7_Q",            
""
};   


// Sets the drop down box

#define OLYEQSelectList_SIZE	11
EQGRAPH_EQSelectList OLYEQSelectList[OLYEQSelectList_SIZE] = {
"VOICING_GLOBAL_PEQ",EQGraph_OLY_VOICING_GLOBAL_PEQ,
"VOICING_HF_PEQ", EQGraph_OLY_VOICING_HF_PEQ, 
"VOICING_MF_PEQ", EQGraph_OLY_VOICING_MF_PEQ, 
"VOICING_LF_PEQ", EQGraph_OLY_VOICING_LF_PEQ, 
"VOICING_HF_PEQ_POST", EQGraph_OLY_VOICING_HF_PEQ_POST, 
"VOICING_MF_PEQ_POST", EQGraph_OLY_VOICING_MF_PEQ_POST, 
"VOICING_LF_PEQ_POST", EQGraph_OLY_VOICING_LF_PEQ_POST, 
"USER_GROUP_PEQ",	EQGraph_OLY_USER_GROUP_PEQ,
"SPEAKER_PROFILE_PEQ", EQGraph_OLY_SPEAKER_PROFILE_PEQ,
"SPEAKER_THROW_PEQ",EQGraph_OLY_THROW_PEQ,
//"AIRLOSS_PEQ", EQGraph_OLY_AIRLOSS_PEQ,
//"USER_SPEAKER_XOVER_PEQ", EQGraph_OLY_USER_SPEAKER_XOVER_PEQ,
"USER_SPEAKER_PEQ", EQGraph_OLY_USER_SPEAKER_PEQ,
};

EQGRAPH_EQSelectList*	AppProductDefs_OLY::GetEQSelectList()
{
	return OLYEQSelectList;
}
int	AppProductDefs_OLY::GetEQSelectListSize()
{
	return OLYEQSelectList_SIZE;
}


int	AppProductDefs_OLY::GetPresetHeaderList(int* nList, int nMax)
{

	int i;
	int nNum;
	int nTargetList[] = {
		eTARGET_DEVICE,	
		eTARGET_USER,				
		eTARGET_PROFILE,				
		eTARGET_THROW,				
		eTARGET_VOICING,				
		eTARGET_FIR,		
		eTARGET_XOVER,				
		eTARGET_AIRLOSS,				
		eTARGET_NOISEGATE,				
	};

	nNum = sizeof(nTargetList)/sizeof(int);
	if (nMax < nNum) nNum = nMax;

	for(i=0;i<nNum;i++)
	{
		nList[i] = nTargetList[i];
	}
	return nNum;

}
