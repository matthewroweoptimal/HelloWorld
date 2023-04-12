//				ecomplex.cpp	Copyright by David Gunness, 1997, 1998, 1999
//
///////////////////////////////////////////////////////////////////////////
#include <math.h>
#include "Ecomplex.h"
#include <ostream>
#include <istream>

ComplexFormat ecomplex::ComplexFormatSetting = RealImag;

// Operator function for '*='
void ecomplex::operator*=(const ecomplex& Var2)
{
	double SaveR = R;
	R = R*Var2.R - X*Var2.X; // Now R is changed
	X = SaveR*Var2.X + X*Var2.R;
}

// Operator function for '/='
void ecomplex::operator/=(const ecomplex& Var2)
{
	double denom = Var2.R * Var2.R + Var2.X * Var2.X;
	double SaveR = R;
	R = (R * Var2.R + X * Var2.X) / denom;; // Now R is changed
	X = (X * Var2.R - SaveR * Var2.X) / denom;
}
	
// Operator function for '>'
bool ecomplex::operator>(ecomplex& cVar)
{
	return (this->mag()) > (cVar.mag());
}

// Operator function for '>='
bool ecomplex::operator>=(ecomplex& cVar)
{
	return (this->mag()) >= (cVar.mag());
}

// Operator function for '<'
bool ecomplex::operator<(ecomplex& cVar)
{
	return (this->mag()) < (cVar.mag());
}

// Operator function for '<='
bool ecomplex::operator<=(ecomplex& cVar)
{
	return (this->mag()) <= (cVar.mag());
}

// Operator function for '=='
bool ecomplex::operator==(ecomplex& cVar)
{
	return ((R == cVar.R) & (X == cVar.X));
}

// Operator function for '!=' not equal to
bool ecomplex::operator!=(ecomplex& cVar)
{
	return ((R != cVar.R) || (X != cVar.X));
}
	
// IMPLEMENT_SERIAL( ecomplex, CObject, 1) // version 1

void ecomplex::Serialize(CArchive& archive)
{
	// call base function first
	// CObject::Serialize(archive);
	if ( archive.IsStoring() )
	{
		archive.Write(&R, sizeof(R));
		archive.Write(&X, sizeof(X));
	}
	else
	{
		archive.Read(&R, sizeof(R));
		archive.Read(&X, sizeof(X));
	}
}

//
//	Operator friend functions have to be explicitly declared as inline,
//	since they are defined outside the class definition.
//
//	Operator function for '+'
ecomplex operator+(const ecomplex& Var1, const ecomplex& Var2)
{
	return ecomplex(Var1.R + Var2.R, Var1.X + Var2.X);
}

ecomplex operator+(const double Var1, const ecomplex& Var2)
{
	return ecomplex(Var1 + Var2.R, Var2.X);
}

// Operator function for '-' (subtraction)
ecomplex operator-(const ecomplex& Var1, const ecomplex& Var2)
{
	return ecomplex(Var1.R - Var2.R, Var1.X - Var2.X);
}

// Operator function for '*'
ecomplex operator*(const ecomplex& Var1, const ecomplex& Var2)
{
	return ecomplex(Var1.R*Var2.R - Var1.X*Var2.X, Var1.R*Var2.X + Var1.X*Var2.R);
}

// Operator function for '/'
ecomplex operator/(const ecomplex& Var1, const ecomplex& Var2)
{
	double denom = Var2.R * Var2.R + Var2.X * Var2.X;
	return ecomplex((Var1.R * Var2.R + Var1.X * Var2.X) / denom, 
		(Var1.X * Var2.R - Var1.R * Var2.X) / denom);
}

ecomplex operator/(const ecomplex& Var1, const double Var2)
{
	return ecomplex(Var1.R/Var2, Var1.X/Var2);
}

/*	The carat operator for exponentiation is eliminated, because the compiler doesn't 
	recognize its precedence correctly. A^2 * 4 is interpreted as A^(2*4)

ecomplex operator^ (const ecomplex& Var1, const double Var2)
{
	double mag = pow(Var1.mag(), Var2);	
	double theta = Var2 * atan2(Var1.X, Var1.R);
	return ecomplex(mag * cos(theta), mag * sin(theta));
}

ecomplex operator^ (const ecomplex& Var1, const ecomplex& Var2)
{
	double theta = atan2(Var1.X, Var1.R);
	double ln_r = log(Var1.mag());
	double mag = exp(Var2.R*ln_r - Var2.X*theta);
	double A_Theta = Var2.R*theta;
	double B_lnR = Var2.X * ln_r;
	return mag * ecomplex(cos(A_Theta), sin(A_Theta)) * ecomplex(cos(B_lnR), sin(B_lnR));
}
*/

ecomplex pow(const ecomplex& Var1, double Var2)
{
	double mag = pow(Var1.mag(), Var2);	
	double theta = Var2 * atan2(Var1.X, Var1.R);
	return ecomplex(mag * cos(theta), mag * sin(theta));
}

ecomplex pow(const ecomplex& Var1, const ecomplex& Var2)
{
	double theta = atan2(Var1.X, Var1.R);
	double ln_r = log(Var1.mag());
	double mag = exp(Var2.R*ln_r - Var2.X*theta);
	double A_Theta = Var2.R*theta;
	double B_lnR = Var2.X * ln_r;
	return mag * ecomplex(cos(A_Theta), sin(A_Theta)) * ecomplex(cos(B_lnR), sin(B_lnR));
}


ostream& operator<< ( ostream& os, const ecomplex& _ec) 
{ 
	switch (ecomplex::GetComplexFormatSetting())
	{
	case (RealImag):
		os << _ec.R;
		if (_ec.X < 0)
			os << "-" << -_ec.X;
		else
			os << "+" << _ec.X;
		os << "i";
		break;
	case (MagPhase):
		os << _ec.mag();
		os << "\t";	// tab
		os << _ec.phase();
		break;
	case (dBRadians):
		os << 20.0 * log10(_ec.mag());
		os << "\t"; // tab
		os << _ec.phase();
		break;
	case (dBDegrees):
		os << 20.0 * log10(_ec.mag());
		os << "\t"; // tab
		os << _ec.phase() * 180.0/pi;
		break;
	default:
		os << "Invalid complex format setting.";
	}
	return os; 
}

istream& operator>> ( istream& is, ecomplex& _ec)
{
	is >> _ec.R >> _ec.X;
	// 2.34+3.45i works, 2.34 -3.45 works, 
	// "2.34,3.45", "2.1-3.2i", "2.1 -3.2i" do not work yet
	return is; // returns a reference to the stream so that cin << comp1 << comp2; works
	// Todo: adapt to negative imaginary part (interpret "-")
}

