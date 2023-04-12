#pragma once

// CCommSettings dialog
#include "MandolinComm.h"


class CRUglyDlg;

class CMandolinTextSummary 
{
public:

	CMandolinTextSummary();

	CRUglyDlg* m_dlgParent;
	int m_nProductType;

	#define PARAMETERLIST_MAX			  (200)
	unsigned int m_nParameterList[PARAMETERLIST_MAX];
	int m_nParameterListId;
	int m_nParameterListNum;


	
	bool MANDOLIN_handle_bHasSubId(int nMsgId);
	void ConvertParameter2Text(tParameterDesc* pParameterDesc, intfloat j, char* pcBuffer, bool bFormat, int nInstance=0, bool bHighPrec=false);

	void MANDOLIN_handle_Error2Text(char *pDest, int mError);

	void MANDOLIN_handle_MsgId2Text(char* pDest, int nId, int nSubId);


	void MANDOLIN_handle_MsgTextSummary_SET_APPLICATION_PARAMETER(char *pDest, mandolin_message* pMsg, int nMaxLength);
	void MANDOLIN_handle_MsgTextSummary_ERROR(char *pDest, mandolin_message* pMsg, int nMaxLength);
	void MANDOLIN_handle_MsgTextSummary_GET_SOFTWARE_INFO_REPLY(char *pDest, mandolin_message* pMsg, int nMaxLength);
	void MANDOLIN_handle_MsgTextSummary_GET_APPLICATION_PARAMETER_REPLY(char *pDest, mandolin_message* pMsg, int nMaxLength);
	void MANDOLIN_handle_MsgTextSummary_GET_SYSTEM_STATUS_REPLY(char *pDest, mandolin_message* pMsg, int nMaxLength);
	void MANDOLIN_handle_MsgTextSummary_CUSTOM_MICSWEEP_REPLY(char *pDest, mandolin_message* pMsg, int nMaxLength);
	void MANDOLIN_handle_MsgTextSummary_CUSTOM_REPLY(char *pDest, mandolin_message* pMsg, int nMaxLength);
	void MANDOLIN_handle_MsgTextSummary_TEST_REPLY(char *pDest, mandolin_message* pMsg, int nMaxLength);
	void MANDOLIN_handle_MsgTextSummary_TRIGGER_SYSTEM_EVENT(char *pDest, mandolin_message* pMsg, int nMaxLength);
	void MANDOLIN_handle_MsgTextSummary(char *pDest, mandolin_message* pMsg, int nMaxLength);



};

