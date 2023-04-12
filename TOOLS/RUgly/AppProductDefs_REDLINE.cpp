#include "stdafx.h"
#include "LOUD_types.h"
extern "C" 
{
#include "mandolin.h"
#include "ProductMap.h"
}

//#include "Product_arch.h"
#include "RUglyDlg.h"
#include "AppProductDefs_REDLINE.h"
#include <stdlib.h>
#include "resource.h"
#include <math.h>

#include "REDLINE_meta.h"
#include "Product_arch.h"
#include "REDLINEspeaker1_pids.h"
#include "REDLINEspeaker1_lookup.h"
#include "REDLINEspeaker1_map.h"



AppProductDefs_REDLINE::AppProductDefs_REDLINE()
{
	m_dlgParent = NULL;
	m_nGetStatePos = 1;
	m_nGetStateTargetPos = 0;
	m_nGetStateRequestSize = 20;
	m_nSendStatePos = 1;
	m_nSendStateTargetPos = 0;
	m_nSendStateRequestSize = 20;
}
AppProductDefs_REDLINE::~AppProductDefs_REDLINE()
{
}




// ----------------------------------------------------------------------------------------------------
// parameter maps and descrioptors.
// ----------------------------------------------------------------------------------------------------
int AppProductDefs_REDLINE::GetParameterDescTable(int nTarget, tParameterDesc** ppParameterDesc, int* pTableLength, int* nXMLVersion)
{
	*ppParameterDesc = 0;
	*pTableLength = 0;
	*nXMLVersion = 0;
	//if (nProduct >= ePRODUCT_FENCE) return 0;
	if (nTarget >= eTARGET_FENCE) return 0;
	*ppParameterDesc = gREDLINEspeaker1ParameterTables[nTarget];
	*pTableLength = nREDLINEspeaker1ParameterTablesMax[nTarget];
	*nXMLVersion = REDLINEspeaker1_XML_VERSION;
	return *pTableLength;

}



tParameterDesc* AppProductDefs_REDLINE::GetParameterDesc(int nTarget, int nParamNum)
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

int AppProductDefs_REDLINE::GetMeterType(  int nTarget, int nParamNum)
{
	if (nParamNum < OLYspeaker1_METER_MAX)
	{
		return g_REDLINEspeaker1MeterTypeTable[nParamNum];
	}
	return eMETERTYPE_NORMAL;
}

void**  AppProductDefs_REDLINE::GetParamAddress(int nTarget, int nParam, int nInstance)
{
	void** pAddress = 0;

	switch (nTarget)
	{
	case eTARGET_SNAPSHOT:
		if (nParam < REDLINEspeaker1_SNAPSHOT_PARAMETER_MAX)
			pAddress = &m_CurrentState.vpParameterBuffer[nParam]; 

		break;
	case eTARGET_GLOBAL:
		if (nParam < REDLINEspeaker1_GLOBAL_PARAMETER_MAX)
			pAddress =  &m_CurrentState.vpGlobalParameterBuffer[nParam]; 
		break;

	case eTARGET_FIR:
		if (nParam < REDLINEspeaker1_FIR_PARAMETER_MAX)
			pAddress =  &m_CurrentState.vpFIRParameterBuffer[nParam]; 
		break;

	case eTARGET_METERS:
		if (nParam < REDLINEspeaker1_METER_MAX)
			pAddress =  (void**) ((float*)&m_gMeterLevel[nParam]); 
		break;

	default:
		break;
	}


	return pAddress;
}

tParameterDesc*  AppProductDefs_REDLINE::ConvertString2ParameterDesc(char* pName,int* nTarget, int* nPID)
{

	tParameterDesc* pParameterDesc = 0;
	int nTableLength = 0;
	int i;
	int nXMLVersion;

	*nTarget = 0;
	*nPID = 0;

			if (strstr(pName, "SNAPSHOT_") != NULL)
		{
			*nTarget = eTARGET_SNAPSHOT;
			GetParameterDescTable(*nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);
			//pParameterDesc = g_tSnapshotParameterTable;
			//nTableLength = ANYA_SNAPSHOT_PARAMETER_MAX;
		}
		else if (strstr(pName, "GLOBAL_") != NULL)
		{
			*nTarget = eTARGET_GLOBAL;
			GetParameterDescTable(*nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);
			//pParameterDesc = g_tGlobalParameterTable;
			//nTableLength = ANYA_GLOBAL_PARAMETER_MAX;
		}
		if (strstr(pName, "FIR_") == pName)		// starts with FIR_
		{
			*nTarget = eTARGET_FIR;
			GetParameterDescTable(*nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);
			//pParameterDesc = g_tSnapshotParameterTable;
			//nTableLength = ANYA_SNAPSHOT_PARAMETER_MAX;
		}
		//else if (strstr(pName, "eMID_") != NULL)
		//{
		//	*nTarget = eTARGET_METERS;
		//	GetParameterDescTable(*nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);
		//	//pParameterDesc = g_tGlobalParameterTable;
		//	//nTableLength = ANYA_GLOBAL_PARAMETER_MAX;
		//}

	

	
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

bool AppProductDefs_REDLINE::IsTargetFIR(int nTarget)
{
	return (nTarget == eTARGET_FIR);
}
bool AppProductDefs_REDLINE::IsTargetMeters(int nTarget)
{
	return (nTarget == eTARGET_METERS);
}


int AppProductDefs_REDLINE::GetMetersTarget()
{
	return eTARGET_METERS;
}


// ----------------------------------------------------------------------------------------------------
// Current state
// ----------------------------------------------------------------------------------------------------
typedef struct {
		int nTarget;
		void** ppAddr;
} tTargetStateAddr;
typedef struct {
		int nTarget;
		int nNumParameters;
		int nNumInstances;
} tTargetInfo;

void  AppProductDefs_REDLINE::SetBrandModel(uint32 uiBrand, uint32 uiModel)
{
}

void AppProductDefs_REDLINE::SetCurrentStateToDefaults() {
	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;
	int i;
	
	tTargetStateAddr  nTargetList[] = {
		{eTARGET_SNAPSHOT, &m_CurrentState.vpParameterBuffer[0]},
		{eTARGET_GLOBAL, &m_CurrentState.vpGlobalParameterBuffer[0]},
		//{eTARGET_FIR, &m_CurrentState.vpFIRParameterBuffer[0]},
	};

	for(int j=0; j< sizeof(nTargetList)/sizeof(tTargetStateAddr);j++)
	{

		GetParameterDescTable(nTargetList[j].nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);

		for(i=0;i<nTableLength;i++)
		{
			nTargetList[j].ppAddr[i] = (void*) pParameterDesc[i].defValue;
		}
	}

}
void AppProductDefs_REDLINE::SetCurrentStateToDefaultsFIR() {
	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;
	int i;
	
	tTargetStateAddr  nTargetList[] = {
		//{eTARGET_SNAPSHOT, &m_CurrentState.vpParameterBuffer[0]},
		//{eTARGET_GLOBAL, &m_CurrentState.vpGlobalParameterBuffer[0]},
		{eTARGET_FIR, &m_CurrentState.vpFIRParameterBuffer[0]},
	};

	for(int j=0; j< sizeof(nTargetList)/sizeof(tTargetStateAddr);j++)
	{

		GetParameterDescTable(nTargetList[j].nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);

		for(i=0;i<nTableLength;i++)
		{
			nTargetList[j].ppAddr[i] = (void*) pParameterDesc[i].defValue;
		}
	}


}
void AppProductDefs_REDLINE::SetCurrentStateToRandom()
{
	// TODO: Add your control notification handler code here
	char pcBuffer[200];
	intfloat jVal;
	int i;
	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;
	tTargetStateAddr  nTargetList[] = {
		{eTARGET_SNAPSHOT, &m_CurrentState.vpParameterBuffer[0]},
		//{eTARGET_GLOBAL, &m_CurrentState.vpGlobalParameterBuffer[0]},
		//{eTARGET_FIR, &m_CurrentState.vpFIRParameterBuffer[0]},
	};

	for(int j=0; j< sizeof(nTargetList)/sizeof(tTargetStateAddr);j++)
	{

		GetParameterDescTable(nTargetList[j].nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);

		for(i=0;i<nTableLength;i++)
		{
			jVal  =  m_dlgParent->GetParameterRandomValue(&pParameterDesc[i],pcBuffer);
			nTargetList[j].ppAddr[i] = jVal.vp;
		}
	}

}

void AppProductDefs_REDLINE::SetCurrentStateToRandomFIR()
{
	// TODO: Add your control notification handler code here
	char pcBuffer[200];
	intfloat jVal;
	int i;
	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;
	tTargetStateAddr  nTargetList[] = {
		//{eTARGET_SNAPSHOT, &m_CurrentState.vpParameterBuffer[0]},
		//{eTARGET_GLOBAL, &m_CurrentState.vpGlobalParameterBuffer[0]},
		{eTARGET_FIR, &m_CurrentState.vpFIRParameterBuffer[0]},
	};

	for(int j=0; j< sizeof(nTargetList)/sizeof(tTargetStateAddr);j++)
	{

		GetParameterDescTable(nTargetList[j].nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);

		for(i=0;i<nTableLength;i++)
		{
			jVal  =  m_dlgParent->GetParameterRandomValue(&pParameterDesc[i],pcBuffer);
			nTargetList[j].ppAddr[i] = jVal.vp;
		}
	}

}
int AppProductDefs_REDLINE::GetRandomTarget(int* nTarget, int* nInstance)
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
		{eTARGET_SNAPSHOT, REDLINEspeaker1_SNAPSHOT_PARAMETER_MAX, 1},
		//{eTARGET_GLOBAL, &m_CurrentState.vpGlobalParameterBuffer[0]},
		//{eTARGET_FIR, &m_CurrentState.vpFIRParameterBuffer[0]},
	};
	
	r = (double)(rand()*1.0)/RAND_MAX;
	nSize = sizeof(nTargetList)/sizeof(tTargetStateAddr);
	nPickTarget = r*nSize;
	if (nPickTarget >= nSize) nPickTarget = nSize-1;

	*nTarget = nTargetList[nPickTarget].nTarget;

	*nInstance = 0;

	nSize = nTargetList[nPickTarget].nNumInstances;
	if (nSize > 1)
	{
		r = (double)(rand()*1.0)/RAND_MAX;

		nPickInstance = r*nSize;
		if (nPickInstance >= nSize) nPickInstance = nSize-1;
		*nInstance = nPickInstance;

	}
	return  nTargetList[nPickTarget].nNumParameters;

}
void AppProductDefs_REDLINE::ShowCurrentState()
{
	// TODO: Add your control notification handler code here
	char pcBuffer[200];
	intfloat jVal;
	int i;
	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;
	tTargetStateAddr  nTargetList[] = {
		{eTARGET_SNAPSHOT, &m_CurrentState.vpParameterBuffer[0]},
		//{eTARGET_GLOBAL, &m_CurrentState.vpGlobalParameterBuffer[0]},
		//{eTARGET_FIR, &m_CurrentState.vpFIRParameterBuffer[0]},
	};

	if (m_dlgParent)
	{
		
		m_dlgParent->UpdateOutputText("SHOW",true);


		for(int j=0; j< sizeof(nTargetList)/sizeof(tTargetStateAddr);j++)
		{

			GetParameterDescTable(nTargetList[j].nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);

			for(i=1;i<nTableLength;i++)
			{
				jVal.vp = nTargetList[j].ppAddr[i];
				m_dlgParent->m_MandolinTextSummary.ConvertParameter2Text(&pParameterDesc[i],jVal,pcBuffer,false);
				m_dlgParent->UpdateOutputText(pcBuffer,true,false,false);
			}
		}
		m_dlgParent->UpdateOutputText("end of SHOW",true);
	}


}

void AppProductDefs_REDLINE::ShowCurrentStateFIR()
{
	// TODO: Add your control notification handler code here
	char pcBuffer[200];
	intfloat jVal;
	int i;
	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;
	tTargetStateAddr  nTargetList[] = {
		//{eTARGET_SNAPSHOT, &m_CurrentState.vpParameterBuffer[0]},
		//{eTARGET_GLOBAL, &m_CurrentState.vpGlobalParameterBuffer[0]},
		{eTARGET_FIR, &m_CurrentState.vpFIRParameterBuffer[0]},
	};

	if (m_dlgParent)
	{
		
		m_dlgParent->UpdateOutputText("SHOW",true);


		for(int j=0; j< sizeof(nTargetList)/sizeof(tTargetStateAddr);j++)
		{

			GetParameterDescTable(nTargetList[j].nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);

			for(i=1;i<nTableLength;i++)
			{
				jVal.vp = nTargetList[j].ppAddr[i];
				m_dlgParent->m_MandolinTextSummary.ConvertParameter2Text(&pParameterDesc[i],jVal,pcBuffer,false);
				m_dlgParent->UpdateOutputText(pcBuffer,true,false,false);
			}
		}
		m_dlgParent->UpdateOutputText("end of SHOW",true);
	}


}
void AppProductDefs_REDLINE::SaveCurrentStateToFile(char* pcFileName)
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

	tTargetStateAddr  nTargetList[] = {
		{eTARGET_SNAPSHOT, &m_CurrentState.vpParameterBuffer[0]},
		{eTARGET_GLOBAL, &m_CurrentState.vpGlobalParameterBuffer[0]},
		//{eTARGET_FIR, &m_CurrentState.vpFIRParameterBuffer[0]},
	};


	fpFactoryDefault = fopen(pcFileName,"w");


	// write the file header

	if (fpFactoryDefault)
	{

		strcpy(pcString,	"PRODUCT REDLINESPEAKER\n");
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

}


void AppProductDefs_REDLINE::SaveCurrentStateFIRToFile(char* pcFileName)
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
		{eTARGET_FIR, &m_CurrentState.vpFIRParameterBuffer[0]},
	};

	fpFactoryDefault = fopen(pcFileName,"w");


	// write the file header

	if (fpFactoryDefault)
	{

		strcpy(pcString,	"PRODUCT REDLINESPEAKER\n");
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

	//	strcpy(pcString,	"PRODUCT REDLINESPEAKER\n");
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



void AppProductDefs_REDLINE::GetStateStart()
{

	GetStateStop();

	m_dlgParent->m_bGetStateStarted = true;

	m_nGetStatePos = 1;
	m_nGetStateTargetPos = 0;
	m_nGetStateRequestSize = 20;

	
	GetStateRequestNext();


}

void AppProductDefs_REDLINE::GetStateStop()
{
	m_dlgParent->m_bGetStateStarted = false;
	if (m_dlgParent)
		m_dlgParent->KillTimer(GETSTATE_TIMERID);

}

void AppProductDefs_REDLINE::GetStateRequestNext()
{

	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;
	uint32 puiPID[100];
	int i,j;
	int nTargetList[] =
	{
		eTARGET_SNAPSHOT,
	};


	if (m_nGetStateTargetPos < sizeof(nTargetList)/sizeof(int))
	{
		GetParameterDescTable(nTargetList[m_nGetStateTargetPos],&pParameterDesc,&nTableLength,&nXMLVersion);

		if (!pParameterDesc) return;
		if (m_nGetStatePos >= nTableLength)
		{
			m_nGetStateTargetPos++;
			m_nGetStatePos = 1;
		}

		if ((m_nGetStatePos < nTableLength) && (m_nGetStateTargetPos < sizeof(nTargetList)/sizeof(int)))
		{
			i = m_nGetStatePos; 
			j = 0;
			while((m_nGetStatePos<nTableLength) && (j < 20))
			{
				puiPID[j++] = m_nGetStatePos++;

				}

			if (m_dlgParent)
			{
				m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
				m_dlgParent->m_MandolinComm.CreateParameterEditGetNeighborhood(&m_dlgParent->m_msgTx, m_dlgParent->m_nTxSequence,nTargetList[m_nGetStateTargetPos], puiPID[0],j);
				//m_MandolinComm.CreateParameterEditGet(&m_msgTx, m_nTxSequence,eTARGET_SNAPSHOT, puiPID,j);
				 m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);
				m_dlgParent->SetTimer(GETSTATE_TIMERID, 1000, NULL);
			}
		}
	}



}

void AppProductDefs_REDLINE::SendStateStart()
{
	// TODO: Add your control notification handler code here
	SendStateStop();

	m_nSendStatePos = 1;
	m_nSendStateTargetPos = 0;
	m_nSendStateRequestSize = 20;
	SendStateSendNext();
}
void AppProductDefs_REDLINE::SendStateStop()
{
	if (m_dlgParent)
		m_dlgParent->KillTimer(SENDSTATE_TIMERID);
}

void AppProductDefs_REDLINE::SendStateSendNext()
{


	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;
	uint32 puiPID[100];
	intfloat jValues[100];
	int i,j;
	tTargetStateAddr  nTargetList[] = {
		{eTARGET_SNAPSHOT, &m_CurrentState.vpParameterBuffer[0]},
	};


	if (m_nSendStateTargetPos < sizeof(nTargetList)/sizeof(int))
	{
		GetParameterDescTable(nTargetList[m_nSendStateTargetPos].nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);

		if (!pParameterDesc) return;

		if (m_nSendStatePos >= nTableLength)
		{
			m_nSendStateTargetPos++;
			m_nSendStatePos = 1;
		}

		if ((m_nSendStatePos < nTableLength) && (m_nSendStateTargetPos < sizeof(nTargetList)/sizeof(int)))
		{
			i = m_nSendStatePos; 
			j = 0;
			while((m_nSendStatePos<nTableLength) && (j < 40))
			{
				puiPID[j] = m_nSendStatePos;
				jValues[j].vp = nTargetList[m_nSendStateTargetPos].ppAddr[m_nSendStatePos];
				m_nSendStatePos++;
				j++;
			}

			if (m_dlgParent)
			{
				m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
		
				m_dlgParent->m_MandolinComm.CreateParameterEditSet(&m_dlgParent->m_msgTx, m_dlgParent->m_nTxSequence,nTargetList[m_nSendStateTargetPos].nTarget,puiPID, jValues ,j,false);
				m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);
				m_nSendStatePos += j;
				m_dlgParent->SetTimer(SENDSTATE_TIMERID, 1000, NULL);
			}
		}
	}

}


// ------------------------------------------------------------------------------------------------------
void AppProductDefs_REDLINE::ParameterAdjustSetComboBox(CComboBox** pCombo,int nNum)
{
	int i;
	int nIndex;
	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;
	int nTargetList[] =
	{
		eTARGET_SNAPSHOT,
		eTARGET_GLOBAL,
		eTARGET_METERS,
	};

	for(int j=0;j<sizeof(nTargetList)/sizeof(int);j++)
	{


		GetParameterDescTable(nTargetList[j],&pParameterDesc,&nTableLength,&nXMLVersion);
		for(int n=0;n<nTableLength;n++)
		{
			if ((strstr(pParameterDesc->pName, "FIR") == 0) && (strstr(pParameterDesc->pName, "NULL") == 0))
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

// ------------------------------------------------------------------------------------------------------

// Level Adjust

ControlDesc gREDLINELevelSwitchDescList[] = {
	{eTARGET_SNAPSHOT, 0, IDC_CHECK_INPUTMUTE1},		// 	
	{eTARGET_SNAPSHOT, 0, IDC_CHECK_INPUTPHASE1},	// 
	{eTARGET_SNAPSHOT, REDLINE_GetParamID_HfMute(1), IDC_CHECK_OUTPUTMUTE1},		// 	
	{eTARGET_SNAPSHOT, REDLINE_GetParamID_HfInvert(1), IDC_CHECK_OUTPUTPHASE1},	// 
	{eTARGET_SNAPSHOT, REDLINE_GetParamID_MfMute(1), IDC_CHECK_OUTPUTMUTE2},		// 	
	{eTARGET_SNAPSHOT, REDLINE_GetParamID_MfInvert(1), IDC_CHECK_OUTPUTPHASE2},	// 
	{eTARGET_SNAPSHOT, 0, IDC_CHECK_OUTPUTMUTE3},		// 	
	{eTARGET_SNAPSHOT, 0, IDC_CHECK_OUTPUTPHASE3},	// 
};



ControlDesc gREDLINELevelFaderDescList[] = {
	{eTARGET_SNAPSHOT, 0, IDC_FADER_INPUTGAIN1},		// 	
	{eTARGET_SNAPSHOT, REDLINE_GetParamID_HfFader(1), IDC_FADER_OUTPUTGAIN1},	// 
	{eTARGET_SNAPSHOT, REDLINE_GetParamID_MfFader(1), IDC_FADER_OUTPUTGAIN2},	// 
	{eTARGET_SNAPSHOT,	0, IDC_FADER_OUTPUTGAIN3},	// 
};

ControlDesc gREDLINELevelEditBoxDescList[] = {
	{eTARGET_SNAPSHOT, 0, IDC_EDIT_INPUTGAIN1},		// 	
	{eTARGET_SNAPSHOT, REDLINE_GetParamID_HfFader(1), IDC_EDIT_OUTPUTGAIN1},	// 
	{eTARGET_SNAPSHOT, REDLINE_GetParamID_MfFader(1), IDC_EDIT_OUTPUTGAIN2},	// 
	{eTARGET_SNAPSHOT, 0, IDC_EDIT_OUTPUTGAIN3},	// 
};


tStringIDC gREDLINETitleList[] = {
	{"",		IDC_LEVELADJUST_TITLE_INPUT1}, 
	{"HF",		IDC_LEVELADJUST_TITLE_OUTPUT1},
	{"LF",		IDC_LEVELADJUST_TITLE_OUTPUT2},
	{"",		IDC_LEVELADJUST_TITLE_OUTPUT3},
};

int AppProductDefs_REDLINE::GetLevelSwitchDescList(ControlDesc** pList)
{
	*pList = gREDLINELevelSwitchDescList;
	return sizeof(gREDLINELevelSwitchDescList)/sizeof(ControlDesc);
}

int AppProductDefs_REDLINE::GetLevelFaderDescList(ControlDesc** pList)
{
	*pList = gREDLINELevelFaderDescList;
	return sizeof(gREDLINELevelFaderDescList)/sizeof(ControlDesc);
}

int AppProductDefs_REDLINE::GetLevelEditBoxDescList(ControlDesc** pList)
{
	*pList = gREDLINELevelEditBoxDescList;
	return sizeof(gREDLINELevelEditBoxDescList)/sizeof(ControlDesc);
}

int AppProductDefs_REDLINE::GetLevelTitleList(tStringIDC** pList)
{
	*pList = gREDLINETitleList;
	return sizeof(gREDLINETitleList)/sizeof(tStringIDC);
}
// ---------------------------------------------------------------------------------------------
ControlDesc gREDLINEMetersDescList[] = {
	{eTARGET_METERS, GetMeterID_InputIn(1),					IDC_METERS_LEVEL1},		// 	
	{eTARGET_METERS, GetMeterID_HfOut(1),					IDC_METERS_LEVEL2},	// 
	{eTARGET_METERS, GetMeterID_HfSoftlimitGainreduce(1),	IDC_METERS_LEVEL3},	// 
	{eTARGET_METERS, GetMeterID_MfOut(1),					IDC_METERS_LEVEL4},	// 
	{eTARGET_METERS, GetMeterID_MfSoftlimitGainreduce(1),	IDC_METERS_LEVEL5},	// 
};

tStringIDC gREDLINEMetersTitleList[] = {
	{"IN",		IDC_METERS_TITLE1}, 
	{"HF",		IDC_METERS_TITLE2},
	{"HFGR",	IDC_METERS_TITLE3},
	{"LF",		IDC_METERS_TITLE4},
	{"LFGR",	IDC_METERS_TITLE5},
};

int AppProductDefs_REDLINE::GetMetersDescList(ControlDesc** pList)
{
	*pList = gREDLINEMetersDescList;
	return sizeof(gREDLINEMetersDescList)/sizeof(ControlDesc);
}
int AppProductDefs_REDLINE::GetMetersTitleList(tStringIDC** pList)
{
	*pList = gREDLINEMetersTitleList;
	return sizeof(gREDLINEMetersTitleList)/sizeof(tStringIDC);
}

