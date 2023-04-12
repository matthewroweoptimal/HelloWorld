// LOUD_types.h

#ifndef __LOUD_TYPES_H__
#define __LOUD_TYPES_H__


// Compile Time Switches
#define BREAK_STANDARD_TYPES 0  // Set this switch to cause compile errors on every 'standard' type declaration.


#if defined(__ADSP21000__) // SHARC

//#message COMPILER: SHARC ADSP-21xxx

#elif defined(__ADSPBLACKFIN__) // Blackfin

//#message COMPILER: BLACKFIN ADSP-BFxxx

#elif defined(__C51__) || defined(__CX51__) // Keil C51 8051 Compiler

//#message COMPILER: Keil C51 DP8051
#include <cytypes.h>

#endif


#if defined(JUCE_INCLUDE)

// JUCE uses the same naming convention to define its own types!

#elif defined(__ADSP21000__)

// All SHARC types are 32-bit!

// SIGNED TYPES
typedef int   int32;
#define short _FIX_ME_
#define char  _FIX_ME_

// UNSIGNED TYPES
typedef unsigned int   uint32;

#elif defined(__C51__) || defined(__CX51__) // Keil C51 8051 Compiler

// USE: #include <cytypes.h>

#else

// SIGNED TYPES
typedef int   int32;
typedef short int16;
typedef char  int8;

// UNSIGNED TYPES
typedef unsigned int   uint32;
typedef unsigned short uint16;
typedef unsigned char  uint8;

#endif


// BOOLEAN TYPES
// NOTE:  There are two boolean types defined and used in LOUD types!

//   1) The simplest, 'bool', is used exclusively for signalling success or failure,
//   and is essentially one bit wide, so its actual precision is unimportant.
#if !defined(JUCE_INCLUDE) && !defined(VISUAL_DSP) && !defined(__cplusplus) && !defined(__bool_true_false_are_defined)
// Only needed in older versions of C that don't define a Boolean type!
typedef enum {
    false,
    true
} bool;
#endif

//   2) The other, 'bool32' is used when dealing with 32-bit data which takes on Boolean values.
//   This one MUST be 32-bits wide.
//   WARNING:  Users should be very careful not to use 'bool' - which is generally only 8-bits wide - for this purpose!
typedef int bool32;


// FLOATING POINT TYPES
#if defined(__ADSP21000__)
#define double _FIX_ME_
typedef float  float32;
#else
typedef double float64;
typedef float  float32;
#endif


#if BREAK_STANDARD_TYPES
#define int      _FIX_ME_
#define short    _FIX_ME_
#define char     _FIX_ME_
#define unsigned _FIX_ME_
#define double   _FIX_ME_
#define float    _FIX_ME_
#endif


// Prototypes
void LOUD_check_types(void);    // Users of LOUD types should implement a simple assert() based check to verify bit width assumptions!

typedef union
{
	float f;
	void* vp;
	int i;
	unsigned int ui;
} intfloat;
#endif // __LOUD_TYPES_H__
