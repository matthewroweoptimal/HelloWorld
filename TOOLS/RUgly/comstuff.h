// ComStuff.h : header file
//

#if !defined(COMMSTUFF_H)
#define COMMSTUFF_H

#ifndef _UGLY
#include "WinIO.h"
#endif

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

typedef int (CALLBACK * ComDataHandler)(
    unsigned int nMessageLength, char *theData, void *callbackObject
    );

// data structures

typedef struct tagcpINFO
{
	HANDLE				hCommDevice;
	HANDLE				hOutputFile;				// if NULL, will not log data
	BYTE				bPort;
	BOOL				fConnected, fXonXoff, fLocalEcho, fNewLine, fAutoWrap,fUseCNReceive, fDisplayErrors;
	BYTE				bByteSize, bFlowCtrl, bParity, bStopBits ;
	DWORD				dwBaudRate ;
	HANDLE				hPostEvent, hWatchThread, hWatchEvent ;
	HWND				hTermWnd ;
	DWORD				dwThreadID ;
	HWND				cwndNotify;					// who to notify when data arrives (NULL for no notification
	UINT				msgID;						// Message # to send that window
	BOOL				bIgnoreRxData;
	BOOL				UseMultithreaded;
	ComDataHandler		callbackHandler;
	void				*callbackObject;


} cpINFO;

#define MAXBLOCK        (1024*2)

#define MAXLEN_TEMPSTR  81

#define RXQUEUE         4096
#define TXQUEUE         4096

#define	COM_OUTPUT_BUFFER_SIZE	(1024*64)
#define	COM_INPUT_BUFFER_SIZE	(1024*64)

// Flow control flags

#define FC_HARDWARE     0x01
#define FC_SOFTWARE     0x02
#define FC_NONE		    0x04

// ascii definitions

#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13

cpINFO* CreateCPInfo();
bool WaitForTXEmpty(cpINFO*	pCPInfo);
int DestroyCPInfo(cpINFO*	pCPInfo);
int NEAR ReadCommBlock( cpINFO*	pCPInfo,LPSTR lpszBlock, int nMaxLength );
int WriteCommBlock( cpINFO*	pCPInfo,LPSTR lpszBlock);
int WriteCommBlockBinary( cpINFO*	pCPInfo,LPSTR lpszBlock, int nBytes);
bool OpenConnection(cpINFO*	pCPInfo);
bool CloseConnection(cpINFO*	pCPInfo);



#endif