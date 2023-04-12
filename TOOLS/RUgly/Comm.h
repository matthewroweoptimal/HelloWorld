#ifndef COMM_h
#define COMM_h

#include "stdafx.h"
#include "AcumaTypes.h"
#include "MErrorCodes.h"

//#include "AppDefines.h"
//#include "Memory.h"


#define DSP_TRANSMIT_BUFSIZE (1024 * 40)
#define SURFACE_TRANSMIT_BUFSIZE (1024 * 40)


class Comm
{
protected:
	Bool myIsConnected;
	UInt	myCOMPort;
	UInt	myIPAddress;
public:
	Comm(void);
	virtual	~Comm(void);

	Bool 		IsConnected(void){ return myIsConnected;};

	virtual stdErr 	Disconnect(void) { return noErr;};
	virtual stdErr 	Connect(void)										= 0;

	virtual void 	SetIPAddress(UInt theAddress) { myIPAddress = theAddress;}
	virtual UInt	GetIPAddress(UInt theAddress) { return myIPAddress;}

	virtual void 	SetPortAddress(UInt theAddress) { myCOMPort = theAddress;}
	virtual UInt 	GetPortAddress(void) { return myCOMPort;}

	virtual Bool HasAnyData(void)									= 0;
	virtual stdErr	Read(char *theData, long *theSize)					= 0;
	virtual stdErr 	Write(char *theData, long size, Bool isASYNC)	= 0;

//	stdErr			WriteString(class String &theString, Bool isASYNC = true)
//	{
//		if(!theString.GetLength()) 
//			return noErr;
//		
//		return Write((char *) theString.GetData(), theString.GetLength(), isASYNC);
//	}
};


/*
Comm
	UartComm
	NetComm
		TCPComm
		UDPComm
*/
	
//stdErr SendCodeFile(String &thePathName, class Comm *thePort);

//#define MAXMESSAGESIZE			80
//#define HOTKEYMAX				70
//#define HOTKEYMIN				48

//extern long gHardwareBlocked;

#endif // COMM_h


