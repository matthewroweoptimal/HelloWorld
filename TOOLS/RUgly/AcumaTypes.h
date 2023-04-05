//*****************************************************************************
//***      Copyright(C) 2003 Acuma Labs Inc. All Rights Reserved.           ***
//*****************************************************************************
//	File: AcumaTypes.h
//	Module: Machine data type definitions.
//	
//	Description:
//		Common data type definitions and macros supporting portable
//		C code.
//
//	Created: 11Nov2003  Don Goodeve
//	$Id:$
//*****************************************************************************

#ifndef ACUMATYPES_H
#define ACUMATYPES_H

//*** Includes
//*****************************************************************************
#ifdef SHARC
#	include "SHARCTypes.h"
#else
#	ifdef _WIN32
#		include "WinTypes.h"
#	else
#		error("AcumaTypes.h: Undefined platform. Must be one of _WIN32, SHARC, ")
#	endif
#endif

#include "AcumaProject.h"


//*** Macros
//*****************************************************************************
#define K		(1024)
#define M		(K*K)
#undef  NULL
#define NULL	0
#ifndef TRUE
#define TRUE	(1==1)
#define FALSE	(!TRUE)
#endif

#define ARRAYSIZE(x) (sizeof(x)/sizeof(x[0]))

// Macro: SAFESET
// Sets the value indirected through a pointer IF the pointer is non-NULL.
//-----------------------------------------------------------------------------
#define SAFESET(pVar,value)		{if(pVar!=NULL) {*pVar=(value);}}


// Macro: UINTSIZE
// Returns the number of UINTs required to hold a structure of the given type.
//-----------------------------------------------------------------------------
#define UINTSIZE(a)		((sizeof(a)+(sizeof(UInt)-1))/sizeof(UInt))


#endif // ACUMATYPES_H 
