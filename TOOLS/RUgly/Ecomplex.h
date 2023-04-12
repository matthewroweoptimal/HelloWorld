//
//			ecomplex.h	Copyright by David Gunness, 1996, 1997, 1998, 1999
//
//	Was derived from CObject, but since no CObject features have ever been used,
//	this was eliminated - 11/11/02  Saves runtime memory.
//
///////////////////////////////////////////////////////////////////////////////////
/*
						Complex Math for Engineering

	v0.0, 12/10/96, by David Gunness
	v0.1, 12/30/96 - changed name to ecomplex to avoid Microsoft complex class
	v0.2, 1/5/97 - changed primary operators to friends, allows promotion

	Supports expressions of complex numbers, like A = 3.3 + 2*B + ecomplex(1.0, 2.0),
	where A & B are complex numbers. Also provides .mag() & .phase() functions.

	Example:	Output = 20.0 * LOG10 ( (ecomplex(3.4, 2.3)).mag() );

*/
#ifndef Complex_Math
#define Complex_Math

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <ostream>
#include <istream>
#include <iostream>
#include <math.h>
#include <float.h>

using namespace std;


#undef AFX_DATA
#define AFX_DATA AFX_EXT_DATA

inline long round(long Value) 
{
	if (Value < 0)
		return long(Value - 0.5);
	else
		return long(Value + 0.5);
}

#ifndef pi
#define pi double( 4.0*atan(1.0) ) // double PI = 3.14159265358979323846 
#endif

#ifndef EXP
#define EXP double (exp(1.0));
#endif

#define log10e double( log10(exp(1.0)) ) 
#define loge10 double( log(10.0) )

// double pi(void) { return 4 * atan(1.0); }

enum ComplexFormat {RealImag=0, MagPhase, dBRadians, dBDegrees};


class ecomplex		// : public CObject
{

protected:
	// only needed for CObject-derived classes - DECLARE_SERIAL( ecomplex )

public:
	virtual void Serialize(CArchive& archive);

	double R;
	double X;
	
private:
	// format style for io streams
	static ComplexFormat ComplexFormatSetting;

public:
	inline double real() const { return R; }
	inline double imag() const { return X; }
	inline double mag() const { return sqrt(R*R+X*X); } 
	// uses square root function in math.h
	inline double phase() const { return atan2(X,R); }   
	// uses arctan function in math.h
 
	// Default Constructor (required for serialization)
	// ecomplex() { R = 0.0; X = 0.0; }

	// General Purpose Constructor definition - also serves as Default Constructor
	ecomplex(double Rv=0.0, double Xv=0.0) { R = Rv; X = Xv; }

	ecomplex(int n) { R = (double) n; X = 0.0; } 

	// Copy Constructor 
	ecomplex(const ecomplex& _ec) {	R = _ec.R; 	X = _ec.X; 	}

	// Assignment Operator 
	ecomplex& operator=(const ecomplex& _ec) { R = _ec.R; X = _ec.X; return *this; }

//
//	Implementing the primary operators as friend functions allows the 
//	compiler to automatically promote standard types to ecomplex arguments.
//
// Operator function prototype for '+'
	friend ecomplex operator+(const ecomplex& Var1, const ecomplex& Var2);
	friend ecomplex operator+(const double Var1, const ecomplex& Var2);

// Operator function prototype for '-' (subtraction)
	friend ecomplex operator-(const ecomplex& Var1, const ecomplex& Var2);
	
// Operator function for '-' (negation)
	ecomplex operator-() { return ecomplex(-R, -X); }

// Operator function prototype for '*'
	friend ecomplex operator*(const ecomplex& Var1, const ecomplex& Var2);
	
// Operator function prototype for '/'
	friend ecomplex operator/(const ecomplex& Var1, const ecomplex& Var2);

// Operator function prototype for '/'
	friend ecomplex operator/(const ecomplex& Var1, const double Var2);

	/*	The carat operator for exponentiation is eliminated, because the compiler doesn't 
	recognize its precedence correctly. A^2 * 4 is interpreted as A^(2*4)
	friend ecomplex operator^ (const ecomplex& Var1, const double Var2);
	friend ecomplex operator^ (const ecomplex& Var1, const ecomplex& Var2);
	*/
	friend ecomplex pow(const ecomplex& Var1, const double Var2);
	friend ecomplex pow(const ecomplex& Var1, const ecomplex& Var2);

// Operator function for '+='
	inline void operator+=(const ecomplex& Var2)
	{
		R += Var2.R;
		X += Var2.X;
	}

// Operator function for '-='
	inline void operator-=(const ecomplex& Var2)
	{
		R -= Var2.R;
		X -= Var2.X;
	}

// Operator function for '*='
	void operator*=(const ecomplex& Var2);
	
// Operator function for '/='
	void operator/=(const ecomplex& Var2);

/*
				Logical Test Operators on Complex Numbers
		>, >=, <, and <= compare the magnitudes of the complex numbers
			== tests for identical real and imaginary parts
		So, (1 + 0i) <= (0 + 1i) is true, but (1 + 0i) == (0 + 1i) is false!
		To test for equal magnitude use (A <= B) & (A >= B), or use A.mag() == B.mag()

*/

// Operator function for '>'
	bool operator>(ecomplex& cVar);
// Operator function for '>='
	bool operator>=(ecomplex& cVar);
// Operator function for '<'
	bool operator<(ecomplex& cVar);
// Operator function for '<='
	bool operator<=(ecomplex& cVar);
// Operator function for '=='
	bool operator==(ecomplex& cVar);
// Operator function for '!=' not equal to
	bool operator!=(ecomplex& cVar);
	friend ostream& operator<< ( ostream& os, const ecomplex& _ec); 
	//	insert operator - friend declaration not essential 
	//	since .R & .X are public.
	friend istream& operator>> ( istream& is, ecomplex& _ec);
	//	extract operator

	inline static void SetComplexFormat(ComplexFormat CF) { ComplexFormatSetting=CF; }
	inline static ComplexFormat GetComplexFormatSetting() { return ComplexFormatSetting; }
	
};	// end of class ecomplex definition

inline double abs(const ecomplex& Acomplex){ return Acomplex.mag(); }

inline ecomplex TimePhasor(const double time, const double frequency)
{
	if (time == 0.0) return ecomplex(1.0, 0.0);
	// time in s of delay, frequency in Hz (not radians - or natural frequency, w)
	double radians;
	radians = -2.0 * pi * time * frequency;
	return ecomplex(cos(radians), sin(radians))	 ;
}

inline ecomplex conjugate(const ecomplex _ec)
{
	return ecomplex(_ec.R, -_ec.X);
}

inline double dB(const ecomplex V) { return 20.0 * log10(abs(V)); }

inline ecomplex ec_abs(const ecomplex V) { return ecomplex(V.mag(),0); }

inline double fabs(const ecomplex ec) { return ec.mag(); }

inline ecomplex exp(const ecomplex& ec) { return ecomplex(exp(ec.R) * cos(ec.X), exp(ec.R) * sin(ec.X)); }

inline ecomplex log(const ecomplex& ec) { 
	double mag = ec.mag();
	return ecomplex(0 == mag ? 0 : log(mag), ec.phase()); }

inline ecomplex log10(const ecomplex& ec) { return log10e * log(ec); }

#undef AFX_DATA
#define AFX_DATA

#endif			// Complex_Math
