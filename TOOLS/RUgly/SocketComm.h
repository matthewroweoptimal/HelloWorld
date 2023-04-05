#ifndef _SOCKETCOMM_H
#define _SOCKETCOMM_H

#include "stdafx.h"
#include "AcumaTypes.h"

//#include "AppDefines.h"

#define WIN


typedef enum
{
	sleepStatus = 0,
	closedStatus,
	listeningStatus,
	startingLowerLayerStatus,
	startingStatus,
	authenticatingConnnectionStatus,
	connectedStatus,
	closingStatus,
	deletedStatus

	
} MixTCPStatus;

#include "Comm.h"


#ifdef SMX
#include "rtip.h"
#include "socket.h"
#endif


#ifdef WIN
#include <winsock.h>
#include <wsipx.h>
#include <nspapi.h>
#include <svcguid.h>
#endif //WIN


#ifdef MAC
#include <OpenTransport.h>
#include <OpenTptInternet.h>
typedef TEndpoint	*MixSocket;
#endif

#ifdef BEOS
#include <socket.h>
#endif


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// the berkeley socket class..
//#define MAX_MESSAGE_SIZE 1024

/*
Comm
	SocketComm
		TCPComm
		UDPComm
*/

class SocketComm;

typedef int (CALLBACK * SocketCommDataHandler)(
    unsigned int nMessageLength, char *theData, void *callbackObject
    );


typedef enum
{
	TCPClient = 0,
	TCPServer,
	UDP
} SocketType;


class SocketCommInfo 
{

public:


//	GUID* m_pGUID;
//	OVERLAPPED mReadOverlap;
//	OVERLAPPED mWriteOverlap;
//
//	HANDLE mQuitReadSem;
//	HANDLE mQuitWriteSem;

	BOOL				fConnected;

	DWORD				m_dwThreadID ;
	HANDLE				m_hWatchThread;


	HWND				cwndNotify;					// who to notify when data arrives (NULL for no notification
	UINT				msgID;						// Message # to send that window
	BOOL				bIgnoreRxData;
	BOOL				UseMultithreaded;
	SocketCommDataHandler		callbackHandler;
	void				*callbackObject;

	SocketComm* m_pSocketComm;
	UINT	m_nIPAddress;
	UINT	m_nPort;
	SocketType	m_nSocketType;
	
	void		CloseSocketCommConnection(void);
	bool		OpenSocketCommConnection(void);

	int			WriteBytes(char* buffer, DWORD bufLen);
	int			ReadSocketCommBlock( LPSTR lpszBlock, int nMaxLength );


};


SocketCommInfo*  CreateSocketCommInfo(void);
int  DestroySocketCommInfo(SocketCommInfo* pSocketCommInfo);


// --------------------------------------------------------------------------

class SocketComm 
{
protected:

	Bool		myIsConnected;
	UInt		myCOMPort;
	UInt		myIPAddress;

	long				mySocket;
	//long				myMessageIndex;
	//char				myMessageBuffer[MAX_MESSAGE_SIZE + 4];
public:

	virtual void 	SetIPAddress(UInt theAddress) { myIPAddress = theAddress;}
	virtual UInt	GetIPAddress(void) { return myIPAddress;}

	virtual void 	SetPortAddress(UInt theAddress) { myCOMPort = theAddress;}
	virtual UInt 	GetPortAddress(void) { return myCOMPort;}
	Bool 			IsConnected(void){ return myIsConnected;};


	MixTCPStatus		myTCPStatus;
	SocketComm			*myNextConnection;
	
	SocketComm(void);
	virtual ~SocketComm(void);

	virtual long		CreateSocket(void)										= 0;
	virtual stdErr		Write(char *data, long size, Bool isASYNC = true)	= 0;
	virtual stdErr		Read(char *data, long *size)							= 0;
	virtual Bool		IsUDP(void) = 0;
//	virtual stdErr		ProcessMessage(char* theMessage, int theLength)			= 0;
	stdErr				Connect(void);
	stdErr				Disconnect(void);
	stdErr				Reset(void);
//	stdErr				AddToList(void);
//	stdErr				RemoveFromList(void);
	stdErr				Close(void);
};

class UDPComm	: public SocketComm
{
public:
	long				CreateSocket(void);
	stdErr				Write(char *data, long size, Bool isASYNC = true);
	stdErr				Read(char *data, long *size);
	Bool				IsUDP(void) { return true;}
//	virtual stdErr		ProcessMessage(char* theMessage, int theLength)						= 0;
};

class TCPComm	: public SocketComm
{
public:
	long				CreateSocket(void);

	stdErr				Write(char *data, long size, Bool isASYNC = true);
	stdErr				Read(char *data, long *size);
	stdErr				Listen(long portNumber);
	Bool				IsUDP(void) { return false;}
//	virtual stdErr		ProcessMessage(char* theMessage, int theLength)						= 0;
};

/////////////////////////////////////////////////////////////////////////////////////////

void	ResetSocketConnections(long portNumber = 0);
long	CountSocketConnections(long portNumber = 0);
void	InitSocketCommunications(void);
stdErr	MixIPAddressToString(UInt address, char *theText);
stdErr	MixGetIPAddressFromString(char *theText, UInt *theAddress);

#define kMIXLINKPORT		6500
#define kBROADCASTPORT		6600

void ResetSocketConnections(long portNumber);

#endif


