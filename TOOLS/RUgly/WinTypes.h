//*****************************************************************************
//***      Copyright(C) 2003 Acuma Labs Inc. All Rights Reserved.           ***
//*****************************************************************************
//	File: WinTypes.h
//	Module: Windows Datatype definitions
//	
//	Description:
//		Common data type definitions and macros for the Win32 platform.
//		Use of these types facilitates portability.
//
//	Created: 30Oct2003  Don Goodeve
//	$Id:$
//*****************************************************************************

#ifndef WINTYPES_H
#define WINTYPES_H

//*** Global types
//*****************************************************************************
typedef char  SInt8;
typedef short SInt16;
typedef int   SInt32;
typedef int	  SInt;

typedef unsigned char  UInt8;
typedef unsigned short UInt16;
typedef unsigned int   UInt32;
typedef unsigned int   UInt;

typedef float  Float32;
typedef double Float64;

typedef struct {
	UInt uLSW;
	UInt uMSW;
}UInt64;

#ifdef __cplusplus   // Note - this is non-standard
typedef bool Bool;
#else
typedef int Bool;
#endif

// 'segment' keyword on Windows has no effect
#define segment(a)



#endif // WINTYPES_H
