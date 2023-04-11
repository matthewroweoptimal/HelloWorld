#ifndef _FIRMWAREFILE_H_
#define _FIRMWAREFILE_H_
#pragma once


#include "stdafx.h"

extern "C"
{
#include "mandolin.h"
}

class CRUglyDlg;


class CFirmwareFile
{
public:
	CFirmwareFile();
	~CFirmwareFile();

	int FirmwareFileInit(char* strInputTextFile, CString& strErrorMsg);
	bool FirmwareFileRead(char* strInputTextFile, CString& strErrorMsg);

	void OnTimer(UINT nIDEvent);
	void OnStart();
	void OnStop();
	bool FirmwareSendNextChunk(char* strInputTextFile, int& nFilePos, bool& bDone, CString& strErrorMsg);


	CRUglyDlg*	m_dlgParent;
	CString		m_strFirmwareFileName;
	OLY_REGION	m_regionHeader;
	int			m_nFirmwareFilePos;
	int			m_totalLengthSent;
	bool		m_bStartFwTransfer;
	bool		m_bEndFwTransfer;
	bool		m_bFirmwareFileInProgress;
	bool		m_bFirmwareFileWaitingAck;
};

#endif