// --------------------------------

#include "stdafx.h"
#include "LOUD_types.h"
extern "C" 
{
#include "mandolin.h"
}

#include "RUglyDlg.h"

#include <stdlib.h>
//#include "resource.h"


#include "FirmwareFile.h"



CFirmwareFile::CFirmwareFile()
{
	m_dlgParent = 0;
}

CFirmwareFile::~CFirmwareFile()
{
}


int CFirmwareFile::FirmwareFileInit(char* strInputTextFile, CString& strErrorMsg)
{
	CFile fReadFile;
	CString errorMsg;

	strErrorMsg.Empty();

	//	SetDlgItemText(IDC_FILEOPENFILENAME,strInputTextFile);

	if (!fReadFile.Open(strInputTextFile, CFile::modeRead))
	{
		errorMsg = "Can not open file: ";
		errorMsg += strInputTextFile;
		strErrorMsg = errorMsg;
		//CWnd::MessageBox(errorMsg, "Error", MB_OK | MB_ICONERROR);
		return -1;
	}
	else
	{
		fReadFile.Close();
		m_strFirmwareFileName = strInputTextFile;
		if (!FirmwareFileRead(strInputTextFile, strErrorMsg))
		{
			return -1;
		}

		return 0;
	}
}


bool CFirmwareFile::FirmwareFileRead(char* strInputTextFile, CString& strErrorMsg)
{
	// read the file and looks for errors
	// return true if ok
	bool bOk = true;
	CFile fReadFile;
	int nLineLength;

	if (!fReadFile.Open(strInputTextFile, CFile::modeRead))
	{
		strErrorMsg.Format("Error Opening file: %s", strInputTextFile);
		return false;
	}

	UINT readLength = fReadFile.Read(&m_regionHeader, sizeof(OLY_REGION));
	if (readLength < sizeof(OLY_REGION))
	{
		bOk = false;	// Header not complete
	}
	else
	{	// TODO : Check the Header looks good
		fReadFile.Close();
		if ((m_regionHeader.type != OLY_REGION_APPLICATION) ||
			(m_regionHeader.address != 0) ||
			(m_regionHeader.length < 1024) ||
			(m_regionHeader.vectorTable != 0) ||
			(m_regionHeader.stackPtr != 0x28000))
		{
			strErrorMsg.Format("Firmware Header Incorrect : %s", strInputTextFile);
			bOk = false;	// Header not as expected
		}
	}

	return bOk;
}

void CFirmwareFile::OnTimer(UINT nIDEvent)
{
	char pcBuffer[200];

	switch (nIDEvent)
	{
	case MSGTIMEOUT_TIMERID:
		// Message Timed out
		if (m_bFirmwareFileInProgress &&  m_bFirmwareFileWaitingAck )
		{
			TRACE("FW Timed Out\n");
		}
		break;

	case FIRMWAREFILE_TIMERID:
		if (m_bFirmwareFileInProgress)
		{
			CString strErrorMsg;
			bool bDone = false;
			if (!FirmwareSendNextChunk(m_strFirmwareFileName.GetBuffer(), m_nFirmwareFilePos, bDone, strErrorMsg))
			{
				m_dlgParent->SetDlgItemText(IDC_RESPONSESTATUS, strErrorMsg);
				bDone = true;
			}
			m_dlgParent->SetDlgItemText(IDC_RESPONSESTATUS, strErrorMsg);

			if (bDone)
			{
				m_dlgParent->KillTimer(FIRMWAREFILE_TIMERID);

				m_bFirmwareFileInProgress = false;
				m_bFirmwareFileWaitingAck = false;
			}
			else
				m_dlgParent->SetTimer(FIRMWAREFILE_TIMERID, 500, NULL);	// Every 0.5s process timer
		}
		break;
	}
}

void CFirmwareFile::OnStart()
{
	CString strErrorMsg;
	m_dlgParent->KillTimer(FIRMWAREFILE_TIMERID);
	m_bFirmwareFileInProgress = false;

	// TODO : Any tidy up of old transfer

	m_nFirmwareFilePos = 0;
	m_totalLengthSent = 0;
	m_bStartFwTransfer = true;
	m_bEndFwTransfer = false;
	m_bFirmwareFileInProgress = true;
	m_bFirmwareFileWaitingAck = false;
	m_dlgParent->SetTimer(FIRMWAREFILE_TIMERID, 1, NULL);  // start the sending of the firmware file
}

void CFirmwareFile::OnStop()
{
	m_dlgParent->KillTimer(FIRMWAREFILE_TIMERID);
	m_bFirmwareFileInProgress = false;
}

bool CFirmwareFile::FirmwareSendNextChunk(char* strInputTextFile, int& nFilePos, bool& bDone, CString& strErrorMsg)
{	CString errorMsg;
	bool bOk = true;
	CFile fReadFile;
	uint8 pcBinData[FW_UPGRADE_CHUNK_SIZE];
	uint32 fileId = 0x5555;

	bDone = false;



	if (m_bStartFwTransfer)
	{
		if (!fReadFile.Open(strInputTextFile, CFile::modeRead))
		{
			strErrorMsg.Format("Error Opening file: %s", strInputTextFile);
			return false;
		}
		fReadFile.Seek(0, CFile::begin);

		UINT readLength = fReadFile.Read(pcBinData, sizeof(OLY_REGION));
		if (readLength > 0)
		{	// Send OLY_REGION information in 'FILE OPEN' message to start firmware upgrade process
			TRACE("Sending POST FILE, OLY_REGION size %d\n", readLength);
			strErrorMsg.Format("Sending POST FILE, OLY_REGION size %d\n", readLength);

			m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
			m_dlgParent->m_MandolinComm.CreateFileOpenCDDLive(&m_dlgParent->m_msgTx, m_dlgParent->m_nTxSequence, (OLY_REGION*)pcBinData);
			m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);
			m_bStartFwTransfer = false;
			m_bFirmwareFileWaitingAck = true;
			fReadFile.Close();
		}
	}
	else if (!m_bEndFwTransfer)
	{	// Send ALL the FW chunks across - including the header which goes to SPI Flash FW region also
		if (!fReadFile.Open(strInputTextFile, CFile::modeRead))
		{
			strErrorMsg.Format("Error Opening file: %s", strInputTextFile);
			return false;
		}
		fReadFile.Seek(nFilePos, CFile::begin);

		UINT readLength = fReadFile.Read(pcBinData, FW_UPGRADE_CHUNK_SIZE);
		if (readLength > 0)
		{	// Send successive chunks of firmware data in 'FILE POST' message
			m_totalLengthSent += readLength;

			TRACE("Chunk : Read %d, total length = %d (%d kB)\n", readLength, m_totalLengthSent, m_totalLengthSent / 1024);
			strErrorMsg.Format("Chunk : Read %d, total length = %d (%d kB)\n", readLength, m_totalLengthSent, m_totalLengthSent / 1024);

			m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
			m_dlgParent->m_MandolinComm.CreateFilePost(&m_dlgParent->m_msgTx, m_dlgParent->m_nTxSequence, fileId, fReadFile.GetLength(), nFilePos, readLength, pcBinData);
			m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);
			nFilePos += readLength;	// Move the offset on to next chunk

			if (m_totalLengthSent >= m_regionHeader.length)
			{
				m_bEndFwTransfer = true;	// Finish up the transfer process
			}

			fReadFile.Close();
		}
	}
	else
	{	// Send a final 'FILE CLOSE' message to finish off the firmware upgrade process
		m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
		m_dlgParent->m_MandolinComm.CreateFileClose(&m_dlgParent->m_msgTx, m_dlgParent->m_nTxSequence, fileId);
		m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);
		m_bEndFwTransfer = false;
		bDone = true;
		strErrorMsg.Format(".... Finishing Transfer\n");
	}
}



