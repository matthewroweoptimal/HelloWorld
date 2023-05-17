// LOUD_types.h

#ifndef __LOUD_TYPES_H__
#define __LOUD_TYPES_H__


// Compile Time Switches
#define BREAK_STANDARD_TYPES 0  // Set this switch to cause compile errors on every 'standard' type declaration.

// SIGNED TYPES
typedef int   int32;
typedef short int16;
typedef char  int8;

//typedef int   int32_t;
//typedef short int16_t;
//typedef char  int8_t;

// UNSIGNED TYPES
typedef unsigned int   uint32;
typedef unsigned short uint16;
typedef unsigned char  uint8;

//typedef unsigned int   uint32_t;
//typedef unsigned short uint16_t;
//typedef unsigned char  uint8_t;



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
typedef double float64;
typedef float  float32;


#if BREAK_STANDARD_TYPES
#define int      _FIX_ME_
#define short    _FIX_ME_
#define char     _FIX_ME_
#define unsigned _FIX_ME_
#define double   _FIX_ME_
#define float    _FIX_ME_
#endif


#endif // __LOUD_TYPES_H__
