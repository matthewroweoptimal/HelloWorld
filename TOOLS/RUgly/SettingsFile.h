#ifndef _SETTINGSFILE_H_
#define _SETTINGSFILE_H_
#pragma once


#include "stdafx.h"

#include "LOUD_types.h"
extern "C"
{
#include "mandolin.h"
}

class CRUglyDlg;

#define SETTINGSFILE_RAWDATA_MAX (32)  // 32 per line
typedef enum {
	SETTINGSFILECOMMANDTYPE_NONE = 0,
	SETTINGSFILECOMMANDTYPE_PARAMETEREDIT,
	SETTINGSFILECOMMANDTYPE_PRODUCT,
	SETTINGSFILECOMMANDTYPE_SPEAKER,
	SETTINGSFILECOMMANDTYPE_MAP,
	SETTINGSFILECOMMANDTYPE_STORE,
	SETTINGSFILECOMMANDTYPE_RECALL,
	SETTINGSFILECOMMANDTYPE_PAUSE,
	SETTINGSFILECOMMANDTYPE_RAWDATA,
	SETTINGSFILECOMMANDTYPE_PROMPT,
} tSettingsFileCommandType;
typedef struct
{
	tSettingsFileCommandType nType;
	int nPIDTarget;
	int nPIDInstance;
	int nPID;
	int nArg1ValueType;  // 0 = int, 1 = float, 2=string
	int nArg2ValueType;  // 0 = int, 1 = float, 2=string
	char pcArg1[256];
	char pcArg2[256];
	intfloat jArg1;
	intfloat jArg2;
	char pcRawData[SETTINGSFILE_RAWDATA_MAX];
	int nRawDataPos;
} tSettingFileCommandLine;




class CSettingsFile
{
public:
	CSettingsFile();
	~CSettingsFile();

	CRUglyDlg* m_dlgParent;


	CString	m_strSettingsFileName;
	int m_nSettingsFilePos;
	int m_bSettingsFileInProgress;
	bool m_bSettingsFileWaitingAck;
	int m_nSettingsFileReplySequence;
	int m_nSettingsFileRetryCount;

	bool m_bSettingsFileDontWaitForAcks;	// for reads to happen or if device does not send acks
	bool m_bSettingsFileBlockSends;			// for reads to happen

	mandolin_message m_msgSettingsFile;
	uint8 m_uiSettingsFilePayload[MANDOLIN_BYTES_PER_WORD*MANDOLIN_MAX_MESSAGE_WORDS];
	//char m_pcSettingsFileConmonDeviceName[100];
	int m_nSettingsFileProductType;


	 int SettingsFileInit(char* strInputTextFile, CString& strErrorMsg);

	 int GetLine(CFile* fReadFile, char* pcNewLine, bool *bEndFile, int nMaxLen);
	int GetToken(char* pcToken, char* pcSrc, int& nPos, int nSize);
	bool SettingsFileParseLine(char* pcNewLine, int nLineLength, tSettingFileCommandLine* pCommand, CString& strError);
	bool SettingsFileSendNext(char* strInputTextFile, int& nFilePos, bool& bDone, CString& strErrorMsg);

	bool SettingsFileRead(char* strInputTextFile, CString& strErrorMsg);
	int SendParamEditMulti(int nTarget, uint32* nParam, intfloat* jValue, int nNumofParams, int nInstance=0);
	int SendParamEditMultiFIR(int nTarget, uint32* nParam, intfloat* jValue, int nNumofParams, int nInstance=0);
	int  SendRawMessage(char *pMsgBuffer, uint16 nLength);
	 void OnTimer(UINT nIDEvent);


	 void OnStart();
	 void OnStop();
	 void OnRead();

};

#endif