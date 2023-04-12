#include "stdafx.h"
#include "AcumaTypes.h"
//#include "winsock.h"
#include "winsock2.h"
#include <assert.h>
#include "WinGUI.h"

//#include "stdafx.h"
//
//#include "WinGUI.h"

//#include "AppDefines.h"
#define WIN


#include "MErrorCodes.h"
#include "SocketComm.h"




DWORD FAR PASCAL SocketCommWatchProc( SocketCommInfo*	pSocketCommInfo, LPSTR lpData );

int SocketCommWSAInit()
{

		WSAData theWSAData;
		WORD wVersionRequested = MAKEWORD( 2, 2 ); 
		int winSockResult = WSAStartup(wVersionRequested, &theWSAData);
		if ( winSockResult != 0 )
		{
			// Tell the user that we could not find a usable WinSock DLL.
			winSockResult = WSAGetLastError();
			return dataErr;
		}

		// Confirm that the WinSock DLL supports version 2.2.
		// Note that if the DLL supports versions greater
		// than 2.2 in addition to 2.2, it will still return
		// 2.2 in wVersion since that is the version we
		// requested.
		if ( LOBYTE( theWSAData.wVersion ) != 2 || HIBYTE( theWSAData.wVersion ) != 2 )
		{
			// Tell the user that we could not find a usable WinSock DLL.
			winSockResult = WSAGetLastError();
			WSACleanup();
			return dataErr;
		}

		return noErr;
}


SocketCommInfo*  CreateSocketCommInfo()
{
	SocketCommInfo*  pSocketCommInfo;
	
	// allocate memory for SocketCommInfo structure
	if (NULL == (pSocketCommInfo = (SocketCommInfo*) LocalAlloc(LPTR, sizeof(SocketCommInfo))))
		return ( NULL );


	// setup SocketComm info


	SocketCommWSAInit();


	pSocketCommInfo->m_pSocketComm = 0;
//	pSocketCommInfo->m_pGUID = NULL;
	pSocketCommInfo->fConnected = FALSE;
	pSocketCommInfo->UseMultithreaded = FALSE;
//	pSocketCommInfo->hOutputFile = NULL;			//	must be filled in by caller if wanted.
	pSocketCommInfo->cwndNotify = NULL;				//	must be filled in by caller if wanted.
	pSocketCommInfo->callbackHandler = NULL;


	//We are created and now the I/O threads can start using us
	//StartIOThreads();

	//TRACE2("WinSocketCommConnection::WinSocketCommConnection - done");


	// end setup

	return ( pSocketCommInfo) ;
	
} // end of CreateCInfo()

int  DestroySocketCommInfo(SocketCommInfo* pSocketCommInfo)
{
  	if (NULL != pSocketCommInfo)
	{

		// clean up the SocketComm Info
		//if (pSocketCommInfo->fConnected == TRUE)
			pSocketCommInfo->CloseSocketCommConnection();

		LocalFree(pSocketCommInfo);
		return ( (int) TRUE ) ;
	}
	else
	{
		return ( (int) -1);
	}
}	



// ====================================================================================

bool  SocketCommInfo::OpenSocketCommConnection()
{

	// build comm port string (COM1, COM2 etc)
	//wsprintf( sCommPortName, "%s%d", (LPSTR) sCommName, pCPInfo->bPort ) ;

	bool fRetVal;
	HANDLE	hSocketCommWatchThread ;
	DWORD	dwThreadID ;



	fRetVal = true;

	if (fConnected)
		CloseSocketCommConnection();

	m_pSocketComm = NULL;


	switch(m_nSocketType)
	{

	case TCPServer:
		m_pSocketComm = (SocketComm*) new TCPComm();
		if (m_pSocketComm)
		{
			m_pSocketComm->SetIPAddress(m_nIPAddress);
			m_pSocketComm->SetPortAddress(m_nPort);

			((TCPComm*)m_pSocketComm)->Listen(m_nPort);
		}
		break;
	case TCPClient:
		m_pSocketComm = (SocketComm*) new TCPComm();
		if (m_pSocketComm)
		{
			m_pSocketComm->SetIPAddress(m_nIPAddress);
			m_pSocketComm->SetPortAddress(m_nPort);

			((TCPComm*)m_pSocketComm)->Connect();
		}

		break;
	case UDP:
		m_pSocketComm = (SocketComm*) new UDPComm();
		if (m_pSocketComm)
		{
			m_pSocketComm->SetIPAddress(m_nIPAddress);
			m_pSocketComm->SetPortAddress(m_nPort);

			((UDPComm*)m_pSocketComm)->Connect();
		}


		break;
	}


	if (m_pSocketComm ==  NULL)
	{
		m_pSocketComm = NULL;
		return false;
	}


	// ---------------------------------------------
	if (NULL == (hSocketCommWatchThread =
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL,
							0,
							(LPTHREAD_START_ROUTINE) SocketCommWatchProc,
							(LPVOID) this,
							0, &dwThreadID )))
	{

		delete(m_pSocketComm);
		
		m_pSocketComm = NULL;

		CloseHandle(m_pSocketComm);
		fRetVal = false ;
	}
	else
	{

		fConnected = true;
		fRetVal = true ;
		m_dwThreadID = dwThreadID ;
		m_hWatchThread = hSocketCommWatchThread ;
		SetThreadPriority(m_hWatchThread,THREAD_PRIORITY_IDLE);				
			
  	}


	return ( fRetVal );
}


void SocketCommInfo::CloseSocketCommConnection()
{
	TRACE2("WinSocketCommConnection::CloseConnection - start");

	if (!fConnected)
	{
		TRACE2("WinSocketCommConnection::CloseConnection - done (not connected)");

		return;
	}


	if(m_pSocketComm != NULL)
	{
		delete(m_pSocketComm);
	}
	fConnected = false;

	// This is here because it fixes a crash that was happening on exit
	Sleep(1000);


	// Clean up all the Windows Objects
	if(m_pSocketComm != NULL)
	{
	//	if (FALSE == ::CloseHandle(m_pSocketComm))
	//		TRACE2EX("***WinSocketCommConnection::CloseConnection - problem closing the XD-2 HANDLE:", GetLastError());
	}


	//if(m_hWatchThread != NULL)
	//{
		::CloseHandle(m_hWatchThread);
	//}

	m_pSocketComm = NULL;
	m_hWatchThread = NULL;
	m_dwThreadID = 0 ;

	TRACE2("WinSocketCommConnection::CloseConnection - done");

}


int SocketCommInfo::WriteBytes(char* buffer, DWORD bufLen)
{

//	TRACE5EX("WinSocketCommConnection::WriteBytes - Everything looks good, bufLen:", bufLen);
	TRACE2("***WinSocketCommConnection::WriteBytes - it wrote");

	if (!m_pSocketComm) return 0;

	
	if (noErr != m_pSocketComm->Write(buffer,bufLen)) //->Read((char*) lpszBlock,(long*) &dwLength))
	{
		return 0;
	}
	

	return ((int) bufLen);
}




int  SocketCommInfo::ReadSocketCommBlock( LPSTR lpszBlock, int nMaxLength )
{
	//int			fReadStat, fWriteStat ;
	//COMSTAT		ComStat ;
	//DWORD		dwErrorFlags;
	DWORD		dwLength;
	DWORD*		lpData;
	

	dwLength = 0;

	if (m_pSocketComm == NULL)
	{
		return (dwLength);
	}

	if (!m_pSocketComm->IsConnected())
	{
		return (dwLength);
	}


	dwLength = nMaxLength;
	if (noErr != m_pSocketComm->Read((char*) lpszBlock,(long*) &dwLength))
	{
		dwLength = 0;
		return dwLength;
	}

	if ((bIgnoreRxData != TRUE) && (dwLength >0))		//ignoring data?
	{
		//if (hOutputFile != NULL)	// can log data
		//	fWriteStat = WriteFile(hOutputFile, lpszBlock, dwLength, &dwLength, NULL);

		if (cwndNotify != NULL) 
			{
			lpData = (DWORD *) GlobalAlloc(GMEM_FIXED, dwLength);		// IMPORTANT : notify window must free this data!
			if (lpData != NULL)
				{
					memcpy (lpData, lpszBlock, dwLength );
					// NOTE: if using windows messaging, must pass callbackObject somehow...(if using that method of processing)
					// you could always have a different windows message id for each port though.
					PostMessage(cwndNotify,msgID,dwLength,(LONG) lpData);
				}
			}

		else if (callbackHandler != NULL)
			 callbackHandler( dwLength, (char *) lpszBlock, callbackObject );
	}
	// 

	TRACE5EX("WinSocketCommConnection::ReadBytes - Everything looks good, bufLen:", *bufLen);

	
	return ( dwLength ) ;
} // end of ReadSocketCommBlock()


//************************************************************************
//  DWORD FAR PASCAL SocketCommWatchProc( LPSTR lpData )
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

#define MAXBLOCK        (1024*2)

DWORD FAR PASCAL SocketCommWatchProc( SocketCommInfo*	pSocketCommInfo, LPSTR lpData )
{
	BYTE       abIn[ MAXBLOCK + 1] ;
	int nLength;

	if (pSocketCommInfo == NULL)
	{
	}
	if (pSocketCommInfo != NULL)
	{

		while ( pSocketCommInfo->fConnected )
		{
			
			do
			{
				nLength = pSocketCommInfo->ReadSocketCommBlock( (LPSTR) abIn, MAXBLOCK );
				if (nLength > 0)
				{
					// write to file
				}
			}
			while ( nLength > 0 ) ;
//			if ( pSocketCommInfo->hOutputFile )
//			{
//				sprintf ((char*) abIn,"\nStat %X\n",dwEvtMask);
//				WriteFile(pSocketCommInfo->hOutputFile, abIn, strlen((char *) abIn), NULL, NULL);
//			}


			Sleep(4);
		}
	}
	
	// CloseCommunication() will clear the fConnected flag, terminating the above loop.
	// clear information in structure (kind of a "we're done flag")
	
	//pSocketCommInfo->m_dwThreadID = 0 ;
	//pSocketCommInfo->m_hWatchThread = NULL ;
	
	return( TRUE ) ;
	
} // end of SocketCommWatchProc()


// -------------------------------------------------------------------------------------

//SocketComm *gConnectionList = 0;
SocketComm::SocketComm(void)
{
    myIsConnected	= false;
	myCOMPort		= 0;
	myIPAddress		= 0;

	mySocket			= 0;
	myNextConnection	= 0;
	myTCPStatus 		= closedStatus;
	// myMessageIndex		= 0;
	//AddToList();
}

SocketComm::~SocketComm(void)
{
    if(myIsConnected)
        Disconnect();


	//RemoveFromList();
	Close();
}

//stdErr SocketComm::AddToList(void)
//{
//	myNextConnection 	= gConnectionList;
//	gConnectionList 	= this;
//	return noErr;
//}




//stdErr SocketComm::RemoveFromList(void)
//{
//	if(gConnectionList == this)
//	{
//		gConnectionList = myNextConnection;
//		return noErr;
//	}
//	SocketComm *theConnection = gConnectionList;
//	while(theConnection)
//	{
//		if(theConnection->myNextConnection == this)
//		{
//			theConnection->myNextConnection = this->myNextConnection;
//			return noErr;
//		}
//		theConnection = theConnection->myNextConnection;
//	}
//	return noErr;
//}


stdErr	SocketComm::Connect(void)
{
	myTCPStatus 		= startingStatus;
	myIsConnected		= false;

	UInt destinationIPAddress	= GetIPAddress();
	if(!destinationIPAddress)
	{
		return dataErr;
	}

	int iValRet;

	mySocket = CreateSocket();
	if(!mySocket)
	{
#ifdef WIN
		iValRet = WSAGetLastError();
#endif
		return dataErr;
	}

	sockaddr_in saBroadcast;
	saBroadcast.sin_family = AF_INET;
	saBroadcast.sin_port = htons((short)GetPortAddress());
#if defined(WIN) || defined(BEOS)
	saBroadcast.sin_addr.s_addr = htonl(INADDR_ANY);
#else
	saBroadcast.sin_addr		= htonl(INADDR_ANY);
#endif
	memset(saBroadcast.sin_zero, 0, sizeof(saBroadcast.sin_zero));
	iValRet = 0; //bind(mySocket, (sockaddr *)&saBroadcast, sizeof(saBroadcast));
	if(iValRet < 0)
	{
#ifdef WIN
		iValRet = WSAGetLastError();
#endif
		Close();
		return dataErr;
	}


	sockaddr_in saTarget;
	saTarget.sin_family = AF_INET;
	saTarget.sin_port = htons((short)GetPortAddress());
#ifdef WIN
	saTarget.sin_addr.s_addr = htonl(destinationIPAddress);
#else
	saTarget.sin_addr		= htonl(destinationIPAddress);
#endif
	memset(saTarget.sin_zero, 0, sizeof(saTarget.sin_zero));

	iValRet = connect(mySocket, (struct sockaddr *)&saTarget, sizeof(saTarget));
	if(iValRet != 0)
	{	
		iValRet = WSAGetLastError();
		Close();
		return dataErr;
	}


	myTCPStatus		= connectedStatus;
	//gSystemProfile.myTCPConnections.SetValue(CountSocketConnections(0));
	myIsConnected	= true;
	return noErr;
}

stdErr	SocketComm::Reset(void)
{
	if(mySocket)
	{
		assert(mySocket != 0);
		myTCPStatus = closingStatus;
		closesocket(mySocket);
	}
	myTCPStatus		= closedStatus;
	myIsConnected	= false;
	mySocket		= 0;
//	gSystemProfile.myTCPConnections.SetValue(CountSocketConnections());
	return noErr;
}

stdErr	SocketComm::Close(void)
{
	int closeErr;
	if(mySocket == 0)
		return noErr;

	// To be sure of a graceful close
	closeErr = shutdown(mySocket, 1);	// Stop sending.. now we receive til no more or timeout

	myTCPStatus		= closingStatus;
	closeErr		= closesocket(mySocket);
	mySocket		= 0;
	myIsConnected	= false;
//	gSystemProfile.myTCPConnections.SetValue(CountSocketConnections());
	return noErr;
}

stdErr SocketComm::Disconnect(void)
{
    return Reset();
}

//stdErr UDPComm::ProcessMessage(char *theMessage, int theLength)
//{
//	return noErr;
//}

////////////////////////////////////////////////////////////////////////////////
/// UDP Connection Object
////////////////////////////////////////////////////////////////////////////////////
long UDPComm::CreateSocket(void)
{
	if(mySocket)
		return mySocket;

	mySocket = socket(AF_INET, SOCK_DGRAM, 0);
	if(mySocket < 0)
	{
		// Do error
		Close();
		return 0;	// Error
	}
	int iValRet;
	unsigned long	flag = 1;
	// change to non-blocking IO.
	flag = 1;
	iValRet = ioctlsocket(mySocket, FIONBIO, &flag);
	assert(iValRet != 0);
	if(iValRet != 0)
	{
#ifdef WIN
		iValRet = WSAGetLastError();
#endif
		Close();
		return 0;
	}

#if defined(WIN)
	int theOption = true;
	iValRet = setsockopt(mySocket, SOL_SOCKET, SO_BROADCAST, (char *) &theOption, sizeof(int));
	if(iValRet != 0)
	{
#ifdef WIN
		iValRet = WSAGetLastError();
#endif
		Close();
		return 0;
	}
#endif

	UInt8 fReUse = TRUE;
    iValRet = setsockopt(mySocket, SOL_SOCKET, SO_REUSEADDR, (char *)&fReUse, sizeof(int));
	if(iValRet < 0)
	{
#ifdef WIN
		iValRet = WSAGetLastError();
#endif
		Close();
		return 0;
	}
	
	return mySocket;
}

stdErr UDPComm::Read(char *theBuffer, long *theReadSize)
{
	if(!theReadSize)
		return dataErr;
	
	if(!myIsConnected || !mySocket)
		return dataErr;

	struct sockaddr_in saSource;
	saSource.sin_family		= AF_INET;
#if defined(WIN) || defined(BEOS)
	saSource.sin_addr.s_addr = INADDR_BROADCAST;
#else
	saSource.sin_addr		= INADDR_BROADCAST;
#endif
	saSource.sin_port		= htons((unsigned short) GetPortAddress());

	// The flag -> MSG_PEEK  can be used to see how many bytes are
	// available without actually extracting them.
	long flags = 0;
	int theSize = sizeof(saSource);
	int receivedCount = recvfrom(mySocket, theBuffer, *theReadSize, flags,
		(struct sockaddr *) &saSource, &theSize);
	if(receivedCount < 0)
	{
#ifdef WIN
		int iValRet = WSAGetLastError();
		if(iValRet == WSAEMSGSIZE)
		{
			// This means message length was greater than *theReadSize,
			// and the excess data in this packet was lost!!!
			return dataErr;
		}
		else if(iValRet == WSAEWOULDBLOCK)
		{
			// This error is ok, we have turned off blocking so now it
			// just means that there is no data available at the moment.
			*theReadSize = 0;
			return noErr;
		}
		else
#endif
			return dataErr;
	}

	*theReadSize = receivedCount;
	return noErr;
}

stdErr UDPComm::Write(char *theBuffer, long theSize, Bool isASYNC)
{
	if(!myIsConnected || !mySocket)
		return dataErr;

	struct sockaddr_in		theRequestedBindAddress;	
	theRequestedBindAddress.sin_family		= AF_INET;
#if defined(WIN) || defined(BEOS)
	theRequestedBindAddress.sin_addr.s_addr = INADDR_BROADCAST;
#else
	theRequestedBindAddress.sin_addr		= INADDR_BROADCAST;
#endif
	theRequestedBindAddress.sin_port		= htons((unsigned short) GetPortAddress());
	
	long flags = 0;
	long sentCount = sendto(mySocket, theBuffer, theSize, flags, (struct sockaddr *) &theRequestedBindAddress, sizeof(theRequestedBindAddress));
	if(sentCount < 0)
	{
#ifdef WIN
		long theError = WSAGetLastError();
		assert(theError != 0);
#endif
		return dataErr;
	}
	return noErr;
}


////////////////////////////////////////////////////////////////////////////////
/// TCP Connection Object
////////////////////////////////////////////////////////////////////////////////////
long TCPComm::CreateSocket(void)
{
	if(mySocket)
		return mySocket;

	mySocket = socket( AF_INET, SOCK_STREAM, 0);
	assert(mySocket >= 0);
	UInt temp 		= UInt(-1);
	if(mySocket < 0)
	{
		Close();
		return 0;	// Error
	}
	int theOption = true;
	if(setsockopt(mySocket, SOL_SOCKET, SO_KEEPALIVE, (char *) &theOption, sizeof(int)))
	{
		// Error!
		theOption = false;
	}
	struct linger
	{
		int	l_onoff;
		int l_linger;
	} theLinger;
	
	theLinger.l_onoff	= 0;
	theLinger.l_linger	= 30;
	if(setsockopt(mySocket, SOL_SOCKET, SO_LINGER, (char *) &theLinger, sizeof(theLinger)))
	{
		// Error!
	}
	return mySocket;
}

stdErr	TCPComm::Listen(long portNumber)
{
	assert(myIsConnected == false);
	
	myIsConnected		= false;	
	SetPortAddress(portNumber);
	UInt ourIPAddress	= myIPAddress; //GetOurIPAddress();


	short				bindResult, listenResult;
	struct sockaddr_in	theRequestedBindAddress;

	int iValRet;
	mySocket = CreateSocket();
	if (!mySocket)
	{

		iValRet = WSAGetLastError();
		return dataErr;
	}


	theRequestedBindAddress.sin_family		= AF_INET;
#if defined(WIN) || defined(BEOS)
	theRequestedBindAddress.sin_addr.s_addr = ourIPAddress;
#else
	theRequestedBindAddress.sin_addr		= ourIPAddress;
#endif
	theRequestedBindAddress.sin_port		= htons((unsigned short) GetPortAddress());

	bindResult = bind(mySocket, (struct sockaddr *) &theRequestedBindAddress, sizeof(theRequestedBindAddress));
	if(bindResult != noErr)
	{
		// Do error
		int error = WSAGetLastError();
		Close();
		return (stdErr) 1;	// Error
	}
	myTCPStatus		= listeningStatus;
	listenResult	= listen(mySocket, 2);
	if(listenResult != 0)
	{
		// Do error
		//error = WSAGetLastError();
		Close();
		return (stdErr) 1;	// Error
	}
	if(myTCPStatus == listeningStatus) // could have been closed by another task.
	{

		int	length			= sizeof(theRequestedBindAddress);
		long oldSocket		= mySocket;
		mySocket			= accept(mySocket, (struct sockaddr *) &theRequestedBindAddress, &length);
		Sleep(200);	//		MixDelay(200);
		if(!mySocket || myTCPStatus != listeningStatus)
			return dataErr;

		closesocket(oldSocket);
		myTCPStatus 		= connectedStatus; //authenticatingConnnectionStatus;
		myIsConnected		= true;
//		gSystemProfile.myTCPConnections.SetValue(CountSocketConnections());
	}
	return noErr;
}

stdErr	TCPComm::Write(char *data, long size, Bool isASYNC)
{
	if(!myIsConnected)
		return dataErr;

	long	charactersSent;

	charactersSent = send(mySocket, (char *) data, size, 0);
	if(charactersSent < 0) // we could not reach our target, so we might be disconnected..
	{
		Close();
		return dataErr;
	}
//	gSystemProfile.myTCPBytesOut.ReadInputDelta(charactersSent);
	return noErr;
}

stdErr TCPComm::Read(char *data, long *size)
{
	if(!myIsConnected || !size)
		return dataErr;

	UInt maxToRead = *size;
	*size			= 0;

	unsigned long	readAmount = *size;
	int err = ioctlsocket(mySocket, FIONREAD, &readAmount);
	if(err != 0)
	{	
		readAmount = 0;
		// There are many errors that are just fine such as when a block winsock call is in progress
#ifdef WIN
		long	error = WSAGetLastError();
		*size = 0;	// !!!
		if(error == WSAECONNABORTED || error == WSAECONNRESET || error == WSAENOTCONN)
		{
			assert(error != 0);
			Close();
			return dataErr;
		}
		return noErr;
#endif
		Close();
		return dataErr;
	}
	if(!readAmount)
	{
		return noErr;
	}
	if(maxToRead < readAmount)
		readAmount = maxToRead;

	assert(readAmount > 0);
	readAmount = recv(mySocket, (char *) data, readAmount, 0);
	if(readAmount == -1)
	{
		// Error
		assert(readAmount != -1);
		*size = 0;
		Close();
		return dataErr;
	}
	else
	{
		*size = readAmount;
//		gSystemProfile.myTCPBytesIn.ReadInputDelta(readAmount);
	}
	return noErr;
}


stdErr MixGetIPAddressFromString(char* theText, UInt *theLong)
{
	UInt curPos = 0;
//	UInt8 *theByte = (uchar *) theLong;
	short numIndex = 0;
	int i;


	curPos = strcspn(theText,".");
	theText[curPos] = 0;
	sscanf(theText,"%d",&i);
	theText += curPos;

	curPos = strcspn(theText,".");
	theText[curPos] = 0;
	sscanf(theText,"%d",&i);
	theText += curPos;

	curPos = strcspn(theText,".");
	theText[curPos] = 0;
	sscanf(theText,"%d",&i);
	theText += curPos;

	curPos = strcspn(theText,".");
	theText[curPos] = 0;
	sscanf(theText,"%d",&i);
	theText += curPos;

	return noErr;
}

stdErr MixIPAddressToString(UInt address, char *theText)
{
	unsigned char *theByte = (unsigned char *) &address;
	sprintf(theText,"%d.%d.%d.%d",theByte[0],theByte[1],theByte[2],theByte[3]);
	return noErr;
}

