// Stuff to handle use of the comm port

#include "stdafx.h"
//#include <Windows.h>
//#include <stdlib.h>
//#include <Windows.h>
//#include "windows.h"
#include "comstuff.h"
//#include "settings.h"

#define DEFAULT_SETTINGS_FILE "PortSettings.bin"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// prototypes



cpINFO*  CreateCPInfo()
{
	cpINFO*   pCPInfo ;
	
	// allocate memory for cpinfo structure
	if (NULL == (pCPInfo = (cpINFO*) LocalAlloc(LPTR, sizeof(cpINFO))))
		return ( NULL );

	HANDLE hSettingsFile = INVALID_HANDLE_VALUE;	
	
	// if no init file exists, put in some default settings
	if ((hSettingsFile = CreateFile(DEFAULT_SETTINGS_FILE, 
									GENERIC_READ, 
									0, 
									NULL, 
									OPEN_EXISTING, 
									FILE_ATTRIBUTE_NORMAL, 
									NULL )) == INVALID_HANDLE_VALUE) {
		pCPInfo->bPort = 1;
		pCPInfo->bByteSize = 8;
		pCPInfo->bFlowCtrl = FC_NONE;
		pCPInfo->bParity = NOPARITY;
		//pCPInfo->bParity = ODDPARITY;
		//pCPInfo->bParity = EVENPARITY;

		pCPInfo->bStopBits = ONESTOPBIT;
		pCPInfo->dwBaudRate = CBR_115200;
		//pCPInfo->dwBaudRate = CBR_19200;
	} else {	// read the settings from the file
		ReadFile(hSettingsFile, &pCPInfo->bPort, sizeof(pCPInfo->bPort),NULL, NULL);
		ReadFile(hSettingsFile, &pCPInfo->bByteSize, sizeof(pCPInfo->bByteSize),NULL, NULL);
		ReadFile(hSettingsFile, &pCPInfo->bFlowCtrl, sizeof(pCPInfo->bFlowCtrl),NULL, NULL);
		ReadFile(hSettingsFile, &pCPInfo->bParity, sizeof(pCPInfo->bParity),NULL, NULL);
		ReadFile(hSettingsFile, &pCPInfo->bStopBits, sizeof(pCPInfo->bStopBits),NULL, NULL);
		ReadFile(hSettingsFile, &pCPInfo->dwBaudRate, sizeof(pCPInfo->dwBaudRate),NULL, NULL);
		CloseHandle(hSettingsFile);
	}

	// fill in the rest of the structure
	pCPInfo->hCommDevice = 0;
	pCPInfo->fConnected = FALSE;
	pCPInfo->UseMultithreaded = FALSE;
//	pCPInfo->fXonXoff = FALSE;
//	pCPInfo->fUseCNReceive = TRUE;
//	pCPInfo->fDisplayErrors = TRUE;
	pCPInfo->hOutputFile = NULL;			//	must be filled in by caller if wanted.
	pCPInfo->cwndNotify = NULL;				//	must be filled in by caller if wanted.
	pCPInfo->callbackHandler = NULL;

	return ( pCPInfo) ;
	
} // end of CreateCInfo()

int  DestroyCPInfo(cpINFO*	pCPInfo)
{
  	if (NULL != pCPInfo)
		{
		if (pCPInfo->fConnected == TRUE)
			CloseConnection(pCPInfo);
		LocalFree(pCPInfo);
		return ( (int) TRUE ) ;
		}
	else
		{
		return ( (int) -1);
		}
}		



//---------------------------------------------------------------------------
//  int  WriteCommBlock( cpINFO*	pCPInfo,LPSTR lpszBlock)
//
//  Description:
//     Writes a block to the COM port from the specified block.
//
//  Parameters:
//
//     LPSTR lpszBlock
//        block used for storage
//
//
//---------------------------------------------------------------------------

int WriteCommBlock( cpINFO*	pCPInfo,LPSTR lpszBlock)
{
	DWORD nBytesWritten;

	WriteFile(pCPInfo->hCommDevice , lpszBlock, strlen(lpszBlock), &nBytesWritten, NULL);
	return nBytesWritten;
}

//---------------------------------------------------------------------------
//  int  WriteCommBlockBinary( cpINFO*	pCPInfo,LPSTR lpszBlock, int nBytes)
//
//  Description:
//     Writes a block to the COM port from the specified block.
//
//  Parameters:
//
//     LPSTR lpszBlock
//        block used for storage
//
//
//---------------------------------------------------------------------------

int WriteCommBlockBinary( cpINFO*	pCPInfo,LPSTR lpszBlock, int nBytes)
{
	DWORD nBytesWritten;

//	DWORD       dwEvtMask ;
//	dwEvtMask =  EV_TXEMPTY;
	if (nBytes > 0)
	{
//		EscapeCommFunction(pCPInfo->hCommDevice, CLRRTS);
		WriteFile(pCPInfo->hCommDevice , lpszBlock, nBytes, &nBytesWritten, NULL);
//		GetCommMask(pCPInfo->hCommDevice,&dwEvtMask);
//		while ((dwEvtMask & EV_TXEMPTY))
//		{
//			GetCommMask(pCPInfo->hCommDevice,&dwEvtMask);
//		}
//		EscapeCommFunction(pCPInfo->hCommDevice, SETRTS);
	}
	else
		nBytesWritten = 0;

	return nBytesWritten;
}


//---------------------------------------------------------------------------
//  int  ReadCommBlock( LPSTR lpszBlock, int nMaxLength )
//
//  Description:
//     Reads a block from the COM port and stuffs it into
//     the provided buffer.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     LPSTR lpszBlock
//        block used for storage
//
//     int nMaxLength
//        max length of block to read
//
//  Win-32 Porting Issues:
//     - ReadComm() has been replaced by ReadFile() in Win-32.
//     - Overlapped I/O has been implemented.
//
//---------------------------------------------------------------------------

void FreeCommData(void* lpData)
{
 	GlobalFree(lpData);
}

int  ReadCommBlock( cpINFO*	pCPInfo,LPSTR lpszBlock, int nMaxLength )
{
	int			fReadStat, fWriteStat ;
	COMSTAT		ComStat ;
	DWORD		dwErrorFlags;
	DWORD		dwLength;
	DWORD*		lpData;
	
	// only try to read number of bytes in queue
	ClearCommError( pCPInfo->hCommDevice , &dwErrorFlags, &ComStat ) ;
	if (dwErrorFlags != 0)
	{
		dwErrorFlags = 0;
	}
	dwLength = min( (DWORD) nMaxLength, ComStat.cbInQue ) ;

	if (dwLength > 0)
	{
		// read the stuff from the comm port buffer
		fReadStat = ReadFile( pCPInfo->hCommDevice , lpszBlock,
			dwLength, &dwLength, NULL ) ;
		
		if (fReadStat) // if data was received, log to file and/or notify window if setup to do so:
			{
				if (pCPInfo->bIgnoreRxData != TRUE)		//ignoring data?
				{
					if (pCPInfo->hOutputFile != NULL)	// can log data
						fWriteStat = WriteFile(pCPInfo->hOutputFile, lpszBlock, dwLength, &dwLength, NULL);

					if (pCPInfo->cwndNotify != NULL) 
						{
						lpData = (DWORD *) GlobalAlloc(GMEM_FIXED, dwLength);		// IMPORTANT : notify window must free this data!
						if (lpData != NULL)
							{
								memcpy (lpData, lpszBlock, dwLength );
								// NOTE: if using windows messaging, must pass pCPInfo->callbackObject somehow...(if using that method of processing)
								// you could always have a different windows message id for each port though.
								PostMessage(pCPInfo->cwndNotify,pCPInfo->msgID,dwLength,(LONG) lpData);
							}
						}

					else if (pCPInfo->callbackHandler != NULL)
						 pCPInfo->callbackHandler( dwLength, (char *) lpszBlock, pCPInfo->callbackObject );
				}
			}
		else
			{
				dwLength = 0;
			}
	 }
	
	return ( dwLength ) ;
} // end of ReadCommBlock()


//************************************************************************
//  DWORD FAR PASCAL CommWatchProc( LPSTR lpData )
//
//  Description:
//     A secondary thread that will watch for COMM events.
//
//  Parameters:
//     LPSTR lpData
//        32-bit pointer argument
//
//  Win-32 Porting Issues:
//     - Added this thread to watch the communications device and
//       post notifications to the associated window.
//
//************************************************************************

DWORD FAR PASCAL CommWatchProc( cpINFO*	pCPInfo, LPSTR lpData )
{
	DWORD       dwEvtMask ;
	int        nLength ;
	BYTE       abIn[ MAXBLOCK + 1] ;
	
	DCB		dcb;

	GetCommState( pCPInfo->hCommDevice,	&dcb);

	if(!SetCommMask( pCPInfo->hCommDevice, EV_RXCHAR  | EV_ERR) )
//	if (!SetCommMask( pCPInfo->hCommDevice, EV_RXCHAR))
		return ( FALSE ) ;
	
	while ( pCPInfo->fConnected )
	{
		dwEvtMask = 0 ;
		GetCommMask(pCPInfo->hCommDevice , &dwEvtMask	 ) ;

//		dwEvtMask = EV_RXCHAR ;
//		WaitCommEvent( pCPInfo->hCommDevice, &dwEvtMask, NULL );
		
//		if ((dwEvtMask & EV_ERR) == EV_ERR)
//		{
//			nLength = 0;
//		}

		if ((dwEvtMask & EV_RXCHAR) == EV_RXCHAR) 
		{
			do
			{
				if (nLength = ReadCommBlock( pCPInfo, (LPSTR) abIn, MAXBLOCK ))
				{
					// write to file
				}
			}
			while ( nLength > 0 ) ;
		}
		else if (!dwEvtMask)
		{
			if ( pCPInfo->hOutputFile )
			{
				sprintf ((char*) abIn,"\nStat %X\n",dwEvtMask);
				WriteFile(pCPInfo->hOutputFile, abIn, strlen((char *) abIn), NULL, NULL);
			}

		}

		Sleep(4);
	}
	
	// CloseCommunication() will clear the fConnected flag, terminating the above loop.
	// clear information in structure (kind of a "we're done flag")
	
	pCPInfo->dwThreadID = 0 ;
	pCPInfo->hWatchThread = NULL ;
	
	return( TRUE ) ;
	
} // end of CommWatchProc()



//---------------------------------------------------------------------------
//  bool  OpenConnection()
//
//  Description:
//     Opens communication port specified in the TTYINFO struct.
//     It also sets the CommState and notifies the window via
//     the fConnected flag in the TTYINFO struct.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//  Win-32 Porting Issues:
//     - OpenComm() is not supported under Win-32.  Use CreateFile()
//       and setup for OVERLAPPED_IO.
//     - Win-32 has specific communication timeout parameters.
//     - Created the secondary thread for event notification.
//
//---------------------------------------------------------------------------


bool  OpenConnection(cpINFO*	pCPInfo)
{

	char	sCommName[] = "COM";
	char	sCommPortName[5];
	HANDLE	hCommDevice;
	DCB		dcb;
	int		fRetVal;
	HANDLE	hCommWatchThread ;
	DWORD	dwThreadID ;

	// build comm port string (COM1, COM2 etc)
	wsprintf( sCommPortName, "%s%d", (LPSTR) sCommName, pCPInfo->bPort ) ;

	// create connection
		hCommDevice = CreateFile( sCommPortName,
					GENERIC_READ | GENERIC_WRITE , // read & write access
					0,						// exclusive access
					NULL,					// no security attrs
					OPEN_EXISTING,			// must use this when opening comm port
					FILE_ATTRIBUTE_NORMAL,	
					NULL );					// must be NULL
	
	// if connection failed, return
	if (hCommDevice == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	// save the handle to the comm port
	pCPInfo->hCommDevice = hCommDevice;

	// not sure about any of this stuff
    // get any early notifications
    //SetCommMask( hCommDevice, EV_RXCHAR);
    SetCommMask( hCommDevice, EV_RXCHAR | EV_ERR) ;

    // setup device buffers
    SetupComm( hCommDevice, COM_INPUT_BUFFER_SIZE , COM_OUTPUT_BUFFER_SIZE ) ;

    // purge any information in the buffer
    PurgeComm( hCommDevice, PURGE_TXABORT | PURGE_RXABORT |
                          PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
	
	// fill in the device control block with current settings of the Port
	GetCommState( hCommDevice,	&dcb);
	dcb.DCBlength = sizeof( DCB ) ;

	// set some stuff
	dcb.BaudRate = pCPInfo->dwBaudRate;
	dcb.ByteSize = pCPInfo->bByteSize;
	dcb.Parity = pCPInfo->bParity;
	dcb.StopBits = pCPInfo->bStopBits;
	
	// setup hardware flow control

	if (pCPInfo->bFlowCtrl == FC_HARDWARE) {
/*		bSet = (BYTE) ((pCPInfo->bFlowCtrl & FC_DTRDSR) != 0) ;
		dcb.fOutxDsrFlow = bSet ;
		if (bSet)
			dcb.fDtrControl = DTR_CONTROL_HANDSHAKE ;
		else
			dcb.fDtrControl = DTR_CONTROL_ENABLE ;
	
		bSet = (BYTE) ((pCPInfo->bFlowCtrl & FC_RTSCTS) != 0) ;
		dcb.fOutxCtsFlow = bSet ;
		if (bSet)
			dcb.fRtsControl = RTS_CONTROL_HANDSHAKE ;
		else
			dcb.fRtsControl = RTS_CONTROL_ENABLE ;*/
	} else if (pCPInfo->bFlowCtrl == FC_SOFTWARE) {
	// setup software flow control
/*		dcb.fInX = dcb.fOutX = TRUE ;
		dcb.XonChar = ASCII_XON ;
		dcb.XoffChar = ASCII_XOFF ;
		dcb.XonLim = 100 ;
		dcb.XoffLim = 100 ;
		dcb.fDtrControl = DTR_CONTROL_DISABLE;*/
	} else {	
	// try setting no hardware, no software flow control
		dcb.fDtrControl = DTR_CONTROL_DISABLE;	// no hardware FC
		dcb.fRtsControl = RTS_CONTROL_DISABLE;
		dcb.fOutX = FALSE;						// no software FC
		dcb.fInX = FALSE;
	}
	// other various settings
		
	dcb.fBinary = TRUE ;
	dcb.fParity = TRUE ;
	
	// set the comm state with the new values
	fRetVal = SetCommState( hCommDevice, &dcb ) ;

	if (fRetVal)
	{
		pCPInfo->fConnected = TRUE ;

		GetCommState( hCommDevice,	&dcb);			// just for the heck of it...see what's there.


		//pCPInfo->fConnected = FALSE ; // skip the create thread
		
		// /*
		// Create a secondary thread
		// to watch for an event.
		
		if (NULL == (hCommWatchThread =
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL,
							0,
							(LPTHREAD_START_ROUTINE) CommWatchProc,
							(LPVOID) pCPInfo,
							0, &dwThreadID )))
		{
			pCPInfo->fConnected = FALSE ;
			CloseHandle(pCPInfo->hCommDevice) ;
			fRetVal = FALSE ;
		}
		else
		{
			pCPInfo->dwThreadID = dwThreadID ;
			pCPInfo->hWatchThread = hCommWatchThread ;
			SetThreadPriority(hCommWatchThread,THREAD_PRIORITY_IDLE);				
			EscapeCommFunction( pCPInfo->hCommDevice, SETDTR );   // assert DTR
			
		}
		//*/
	}
	else
	{
		pCPInfo->fConnected = FALSE ;
		CloseHandle(pCPInfo->hCommDevice) ;
	}
	
	return ( (bool) (fRetVal != 0)  ) ;
	
} 

bool WaitForTXEmpty(cpINFO*	pCPInfo)
{
	bool bReturn = TRUE;

	if ( pCPInfo->fConnected == TRUE )
		FlushFileBuffers( pCPInfo->hCommDevice );
	else
		bReturn = FALSE;

	return bReturn;


}




//---------------------------------------------------------------------------
//  bool  CloseConnection()
//
//  Description:
//     Closes the connection to the port.  Resets the connect flag
//     in the TTYINFO struct.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//  Win-32 Porting Issues:
//     - Needed to stop secondary thread.  SetCommMask() will signal the
//       WaitCommEvent() event and the thread will halt when the
//       CONNECTED() flag is clear.
//     - Use new PurgeComm() API to clear communications driver before
//       closing device.
//		
//---------------------------------------------------------------------------

bool  CloseConnection(cpINFO*	pCPInfo)
{
	// set connected flag to FALSE
	
	if (pCPInfo == NULL)
		return TRUE;

	if (!pCPInfo->fConnected)
		return TRUE;

	pCPInfo->fConnected = FALSE ;
	pCPInfo->bIgnoreRxData = TRUE;		
	
	// disable event notification and wait for thread
	// to halt

	if (pCPInfo->hCommDevice)
	{
	
		SetCommMask( pCPInfo->hCommDevice, 0 ) ;
		
		// block until thread has been halted
		
	//	while(pCPInfo->dwThreadID != 0);   	this seems to give some real time problems when switching ports
	//										In any case, by setting bIgnoreRxData, no further interaction with the other thread will occur.
		
		// drop DTR, i dont know why
		
		EscapeCommFunction(pCPInfo->hCommDevice, CLRDTR ) ;
		
		// purge any outstanding reads/writes and close device handle
		
		PurgeComm( pCPInfo->hCommDevice, PURGE_TXABORT | PURGE_RXABORT |
			PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
		
		// close comm port
		CloseHandle( pCPInfo->hCommDevice ) ;
		pCPInfo->hCommDevice = 0;
	}


	if (pCPInfo->hWatchThread)
	{
		::CloseHandle(pCPInfo->hWatchThread);
		pCPInfo->hWatchThread = NULL;
	}

	 // close output file

	if (pCPInfo->hOutputFile)
	{
	 	if (!CloseHandle( pCPInfo->hOutputFile )) 
	 	{
	 		DWORD dwError = GetLastError();
	 	} 
	 	pCPInfo->hOutputFile = NULL; 
	}

	return ( TRUE ) ;

} // end of CloseConnection()



