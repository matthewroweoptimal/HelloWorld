//
//	DSPEffect.cpp: implementation of the CEffect class.
//
//	CEffect is a digital filter which implements a gain control.  It 
//	serves as the parent class for a family of digital filters.
//
//////////////////////////////////////////////////////////////////////

// INCLUDE
#include <afx.h>
#include "DSPEffect.h"
#include <math.h>
#include "ecomplex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// CEffect
//////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constructor
CEffect::CEffect()
{
	//	Initialize the input(s) and output(s)
	m_nFrameSize = 1;
	m_nInputs = 1;
	m_pInput = 0;
	m_nOutputs = 1;
	m_pOutput = 0;

	m_nCoeffNum = 0;
	m_nParameterNum = 0;

}

///////////////////////////////////////////////////////////////////////////////
// Destructor
CEffect::~CEffect()
{
}

///////////////////////////////////////////////////////////////////////////////
// GetReqs
int CEffect::GetReqs(int& nInputs, int&nOutputs)
{
	nInputs = m_nInputs;
	nOutputs = m_nOutputs;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Configure
int CEffect::Configure(	AUDIO_TYPE** pOutputPtr,	// array of pointers to the input buffers
						AUDIO_TYPE** pInputPtr,		// array of pointers to the output buffers
						long nFrameSize)			// The length of the input & output buffers
{
	//	Memory for audio inputs & outputs, as well as arrays of pointers to them - should have 
	//	been allocated by the calling program
	m_nFrameSize = nFrameSize;
	m_pInput = pInputPtr;
	m_pOutput = pOutputPtr;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// CalcCoefficients
int CEffect::CalcCoefficients(float* pParameters, int nParamCount, float SampleRate)
{

	// fills in m_pData = parameter list back
	ASSERT(nParamCount <= PARAMETER_MAX);
	m_nParameterNum = nParamCount;

	memcpy(m_pParameters, pParameters, nParamCount * sizeof(float));

	ASSERT(nParamCount <= DATA_MAX);
	m_nCoeffNum = nParamCount;
	memcpy(m_pData, pParameters, nParamCount * sizeof(float));

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Process
void CEffect::Process()	// process the audio data
{
	//	For this prototype, just copy the input data to the output
	// memcpy(m_pOutput[0], m_pInput[0], m_nFrameSize * sizeof(AUDIO_TYPE));

	for (int i = 0; i < m_nFrameSize; i++)
	{
		m_pOutput[0][i] += m_pData[COEFF_B0] * m_pInput[0][i]; 
	}
	
}

void CEffect::ProcessReplacing()
{	
	//	For this prototype, just add the input data to the output
	// memcpy(m_pOutput[0], m_pInput[0], m_nFrameSize * sizeof(AUDIO_TYPE));

	//	Zero the output arrays
	for (int i = 0; i < m_nOutputs; i++)
		memset(&m_pData[i], 0, m_nFrameSize);
	//	Then, accumulate the results into it
	Process();
}


//////////////////////////////////////////////////////////////////////
// CBiquad
//////////////////////////////////////////////////////////////////////
//
//	Generic Biquad Effect
//
//	The first five floats in the parameter array are b0, b1, b2, a1, a2
//	This can be inherited by any filter implemented as a biquad.  Just
//	override the CalcCoefficients function.
//
//	Note that a separate instance is required for each stream to be processed.
//	Two previous results & inputs are saved across calls to ProcessReplacing().
//

///////////////////////////////////////////////////////////////////////////////
// Constructor
CBiquad::CBiquad()
	: CEffect()
{
	m_fLastInputs[0] = 0.0f;
	m_fLastInputs[1] = 0.0f;
	m_fLastOutputs[0] = 0.0f;
	m_fLastOutputs[1] = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
// Process
void CBiquad::ProcessReplacing()
{
	//	To process a frame, we need two previous inputs and two previous outputs
	//b0 b1 b2, a1, a2
	m_pOutput[0][0] = m_pData[COEFF_B0] * m_pInput[0][0] + m_pData[COEFF_B1] * m_fLastInputs[0] + m_pData[COEFF_B2] * m_fLastInputs[1] 
		- m_pData[COEFF_A1] * m_fLastOutputs[0] - m_pData[COEFF_A2] * m_fLastOutputs[1];
	
	m_pOutput[0][1] = m_pData[COEFF_B0] * m_pInput[0][1] + m_pData[COEFF_B1] * m_pInput[0][0] + m_pData[COEFF_B2] * m_fLastInputs[0] 
		- m_pData[COEFF_A1] * m_pOutput[0][0] - m_pData[COEFF_A2] * m_fLastOutputs[0];

	//	The third point through the last point (frame size - 1)
	for (int i = 2; i < m_nFrameSize; i++)
	{
		m_pOutput[0][i] = m_pData[COEFF_B0] * m_pInput[0][i] + m_pData[COEFF_B1] * m_pInput[0][i-1] + m_pData[COEFF_B2] * m_pInput[0][i-2] 
		- m_pData[COEFF_A1] * m_pOutput[0][i-1] - m_pData[COEFF_A2] * m_pOutput[0][i-2]; 
	}
	
	// Save the last two inpust and outputs for the next call to this function
	m_fLastInputs[0] = m_pInput[0][m_nFrameSize-1];
	m_fLastInputs[1] = m_pInput[0][m_nFrameSize-2];
	m_fLastOutputs[0] = m_pOutput[0][m_nFrameSize-1];
	m_fLastOutputs[1] = m_pOutput[0][m_nFrameSize-2];
}



///////////////////////////////////////////////////////////////////////////////
// Constructor
CPEQ::CPEQ()
	: CBiquad()
{
	m_nCoeffNum = 5;
	m_pData[COEFF_B0] = 1.0f;
	m_pData[COEFF_B1] = 0.0f;
	m_pData[COEFF_B2] = 0.0f;
	m_pData[COEFF_A1] = 0.0f;
	m_pData[COEFF_A2] = 0.0f;

	m_nType = MZTi;
}

///////////////////////////////////////////////////////////////////////////////
// CalcCoefficients	

#define PEQ_PARAMETER_FREQ 0 
#define PEQ_PARAMETER_GAIN 1 
#define PEQ_PARAMETER_Q 2 
int CPEQ::CalcCoefficients(float* pParameters, int nParamCount, float SampleRate)
{
	// pParameters
	//frequency, 	boostDB. Q;

	// fills in m_pData = b0,b1,b2,a1,a2
	ASSERT(nParamCount == 3);
	if (pParameters[PEQ_PARAMETER_GAIN] == 0.0)	// boostDB is 0, so return a unity biquad
	{
		m_pData[COEFF_B0] = 1.0;	// b0
		m_pData[COEFF_B1] = 0.0;	// b1
		m_pData[COEFF_B2] = 0.0;	// b2
		m_pData[COEFF_A1] = 0.0;	// a1
		m_pData[COEFF_A2] = 0.0;	// a2
		return 5;
	}
	double w0 = 2.0 * pi * pParameters[PEQ_PARAMETER_FREQ];					// pParameters[0] = frequency in Hz
	BOOL bCut = (pParameters[PEQ_PARAMETER_GAIN] < 0.0);
	double g0 = pow(10.0, fabs(pParameters[PEQ_PARAMETER_GAIN])/20.0);		// Calculate parameters of boost filter
	double Q;
	double T = 1.0 / SampleRate;
	double w0T = w0*T;
	if (g0 > 2.0)
		Q = pParameters[PEQ_PARAMETER_Q] * g0 / sqrt(g0*g0 - 2.0);
	else if (g0 > 0.5)
		Q = pParameters[PEQ_PARAMETER_Q] * sqrt(g0);
	else
		Q = pParameters[PEQ_PARAMETER_Q] * g0 / sqrt(1 - 2.0 * g0 * g0);	   // should never happen in a boost filter

	if (Q >= 0.5)
	{
		// a1
		m_pData[COEFF_A1] = (float) -2.0 * (float)exp( -w0T*0.5 / Q ) * (float)cos(sqrt(1.0 - pow(0.5/Q, 2.0)) * w0T);
	}
	else
	{
		// a1
		m_pData[COEFF_A1] = (float) -( exp(-w0T*((0.5/Q) + sqrt(pow(0.5/Q, 2.0) - 1.0))) + exp(-w0T*((0.5/Q) - sqrt(pow(0.5/Q, 2.0) - 1.0))) );
		//                          --------------------------------------------        --------------------------------------------
	}
	// a2
	m_pData[COEFF_A2] = (float) exp(-w0T / Q);

	//	Calculate the ideal and IIR responses at 1/3 and 2/3 Nyquist
	//	Ideal Response
	ecomplex s1 = ecomplex(0.0, (SampleRate/6.0) / pParameters[PEQ_PARAMETER_FREQ] );	// ratio of 1/3 Nyquist to center frequency
	ecomplex s2 = ecomplex(0.0, (SampleRate/3.0) / pParameters[PEQ_PARAMETER_FREQ] );	// 2/3 Nyquist
	ecomplex Ha1 = (s1 * s1 + g0 * s1 / Q + 1.0) / (s1 * s1 + s1 / Q + 1);	
	ecomplex Ha2 = (s2 * s2 + g0 * s2 / Q + 1.0) / (s2 * s2 + s2 / Q + 1);
	//	Digital IIR Response
	double IIR1 = 1.0 / sqrt((double)(1 + m_pData[COEFF_A1] - m_pData[COEFF_A2] + pow((double)m_pData[COEFF_A1], (double)2.0) + m_pData[COEFF_A1] * m_pData[COEFF_A2] + pow((double)m_pData[COEFF_A2], (double)2.0)));
	double IIR2 = 1.0 / sqrt(1 - m_pData[COEFF_A1] - m_pData[COEFF_A2] + pow((double)m_pData[COEFF_A1], (double)2.0) - m_pData[COEFF_A1] * m_pData[COEFF_A2] + pow((double)m_pData[COEFF_A2], (double)2.0));
	//	Mag Response for 3-Point Curve Fit of FIR part
	double h0 = 1 + m_pData[COEFF_A1] + m_pData[COEFF_A2]; // m_pData[COEFF_B0] + m_pData[COEFF_B1] + m_pData[COEFF_B2];
	double h1 = fabs(Ha1)/IIR1; 
	double h2 = fabs(Ha2)/IIR2; 
	//	Two-Zero Curve Fit
	m_pData[COEFF_B1] = (float) (h0 - sqrt(h0*h0 - 2*h1*h1 + 2*h2*h2) ) / 2.0;
	m_pData[COEFF_B2] = (float) (3 * (h0 - m_pData[COEFF_B1]) - sqrt(-3.0 * h0 * h0 + 12 * h1 * h1 - 6 * h0 * m_pData[COEFF_B1] - 3 * m_pData[COEFF_B1] * m_pData[COEFF_B1]) ) / 6.0;
	m_pData[COEFF_B0] = (float) h0 - m_pData[COEFF_B1] - m_pData[COEFF_B2];

	if (bCut)			 // If cut filter, invert the biquad
	{
		float a[2];	
		a[0] = m_pData[COEFF_B1] / m_pData[COEFF_B0];
		a[1] = m_pData[COEFF_B2] / m_pData[COEFF_B0];	 		
		m_pData[COEFF_B0] = 1/m_pData[COEFF_B0]; 	
		m_pData[COEFF_B1] = m_pData[COEFF_A1] * m_pData[COEFF_B0];
		m_pData[COEFF_B2] = m_pData[COEFF_A2] * m_pData[COEFF_B0];
		m_pData[COEFF_A1] = a[0];
		m_pData[COEFF_A2] = a[1];
	}
	 
	return 5;
}

///////////////////////////////////////////////////////////////////////////////
// SetParams
int CPEQ::SetParams(float frequency, float boostDB, float Q, float SampleRate)
{
	// fills in m_pData = b0, b1, b2,a1, a2
	float Params[3];
	Params[PEQ_PARAMETER_FREQ] = frequency;
	Params[PEQ_PARAMETER_GAIN] = boostDB;
	Params[PEQ_PARAMETER_Q] = Q;
	return CalcCoefficients(&Params[0], 3, SampleRate);
}



//////////////////////////////////////////////////////////////////////
// CShelf
//////////////////////////////////////////////////////////////////////
//
//	1st or 2nd-Order Shelving Filter
//
//	enum nEQType { EQ_PARAMETRIC, EQ_6DBLOSHELF, EQ_6DBHISHELF, EQ_12DBLOSHELF, EQ_12DBHISHELF };
//	
//	The first five floats in the parameter array are b0, b1, b2, a1, a2

CShelf::CShelf()
	: CBiquad()
{
	m_pData[COEFF_B0] = 1.0f;
	m_pData[COEFF_B1] = 0.0f;
	m_pData[COEFF_B2] = 0.0f;
	m_pData[COEFF_A1] = 0.0f;
	m_pData[COEFF_A2] = 0.0f;
}

#define SHELF_PARAMS_FREQ		0
#define SHELF_PARAMS_BOOSTDB	1
#define SHELF_PARAMS_EQTYPE		2


int CShelf::CalcCoefficientsOld(float* pParameters, int nParamCount, float SampleRate)
{
	// fills in m_pData = b0, b1, b2, a1, a2
	ASSERT(nParamCount == 3);
	float w0 = 2.0f * float(pi) * pParameters[SHELF_PARAMS_FREQ];
	float g = (float) pow(10.0, fabs(pParameters[SHELF_PARAMS_BOOSTDB])/20.0);	// Boost or cut, always positive
	float w1;
	float T = 1.0f / SampleRate;
	float w0T = w0*T;
	float w1T;

	int nType = (int) pParameters[2];

	if ((nType == EQ_12DBHISHELF) || (nType == EQ_12DBLOSHELF))	// 2nd-Order Shelf
	{
		float Q = 0.786f;
		if (nType == EQ_12DBHISHELF)
			w1 = w0 / (float) sqrt(g);
		else
			w1 = w0 * (float) sqrt(g);
		w1T = w1*T;
		
		if (Q >= 0.5)
		{
			// a1
			m_pData[COEFF_A1] = (float) -2.0 * (float)exp( -w0T*0.5 / Q ) * (float)cos(sqrt(1.0 - pow(0.5/Q, 2.0)) * w0T);
			// b1
			m_pData[COEFF_B1] = (float) -2.0 * (float)exp( -w1T*0.5 / Q ) * (float)cos(sqrt(1.0 - pow(0.5/Q, 2.0)) * w1T);
		}
		else
		{
			// a1
			m_pData[COEFF_A1] = (float) -( exp(-w0T*((0.5/Q) + sqrt(pow(0.5/Q, 2.0) - 1.0))) + exp(-w0T*((0.5/Q) - sqrt(pow(0.5/Q, 2.0) - 1.0))) );
			// b1                      --------------------------------------------        --------------------------------------------
			m_pData[COEFF_B1] = (float) -( exp(-w1T*((0.5/Q) + sqrt(pow(0.5/Q, 2.0) - 1.0))) + exp(-w1T*((0.5/Q) - sqrt(pow(0.5/Q, 2.0) - 1.0))) );
		}
		// a2
		m_pData[COEFF_A2] = (float) exp(-w0T / Q);
		// b2
		m_pData[COEFF_B2] = (float) exp(-w1T / Q);
	}
	else	//	1st-Order Shelf
	{
		if (nType == EQ_6DBHISHELF)
			w1 = w0 / (float) g;
		else
			w1 = w0 * (float) g;
		w1T = w1*T;

		// a1
		m_pData[COEFF_A1] = (float) -exp(-w0T);
		// b1
		m_pData[COEFF_B1] = (float) -exp(-w1T);
	}

	//	Invert the filter if it is a cut filter
	if (pParameters[SHELF_PARAMS_BOOSTDB] < 0.0) // -dB is a cut
	{
		float a1 = m_pData[COEFF_A1];
		float a2 = m_pData[COEFF_A2];
		m_pData[COEFF_A1] = m_pData[COEFF_B1]; // b1 -> a1 
		m_pData[COEFF_A2] = m_pData[COEFF_B2]; // b2 -> a2
		m_pData[COEFF_B1] = a1;
		m_pData[COEFF_B2] = a2;
	}

	//	Calculate required makeup gain to give correct results at DC
	
	if ((nType == EQ_12DBHISHELF) || (nType == EQ_6DBHISHELF))	// gain should be 1.0 at DC
		g = (1.0f + m_pData[COEFF_A1] + m_pData[COEFF_A2]) / (1.0f + m_pData[COEFF_B1] + m_pData[COEFF_B2]);
	else
	{	//  gain should be boost or 1/boost at DC
		if (pParameters[SHELF_PARAMS_BOOSTDB] < 0.0f)	// determine if it is a cut filter
			g = 1.0f/g;
		g = g * (1.0f + m_pData[COEFF_A1] + m_pData[COEFF_A2]) / (1.0f + m_pData[COEFF_B1] + m_pData[COEFF_B2]);
	}

	// Apply gain
	m_pData[COEFF_B0] = g;		// b0
	m_pData[COEFF_B1] *= g;	// b1
	m_pData[COEFF_B2] *= g;	// b2

	return 5;
}

int CShelf::CalcCoefficients(float* pParameters, int nParamCount, float SampleRate)
{
	// fills in m_pData = b0, b1, b2, a1, a2
	ASSERT(nParamCount == 3);
	double w0 = 2.0f * float(pi) * pParameters[SHELF_PARAMS_FREQ];
	double g = pow(10.0, fabs(pParameters[SHELF_PARAMS_BOOSTDB])/20.0);	// Boost or cut, always positive
	double w1;
	double T = 1.0f / SampleRate;
	double w0T = w0*T;
	double w1T;
	double a1,a2,b1,b2;
	
	int nType = (int) pParameters[2];

	if ((nType == EQ_12DBHISHELF) || (nType == EQ_12DBLOSHELF))	// 2nd-Order Shelf
	{
		float Q = 0.786f;
		if (nType == EQ_12DBHISHELF)
			w1 = w0 / sqrt(g);
		else
			w1 = w0 * sqrt(g);
		w1T = w1*T;
		
		if (Q >= 0.5)
		{
			a1 = -2.0 * exp( -w0T*0.5 / Q ) * cos(sqrt(1.0 - pow(0.5/Q, 2.0)) * w0T);
			b1 = -2.0 * exp( -w1T*0.5 / Q ) * cos(sqrt(1.0 - pow(0.5/Q, 2.0)) * w1T);
		}
		else
		{
			a1 = -( exp(-w0T*((0.5/Q) + sqrt(pow(0.5/Q, 2.0) - 1.0))) + exp(-w0T*((0.5/Q) - sqrt(pow(0.5/Q, 2.0) - 1.0))) );
			b1 = -( exp(-w1T*((0.5/Q) + sqrt(pow(0.5/Q, 2.0) - 1.0))) + exp(-w1T*((0.5/Q) - sqrt(pow(0.5/Q, 2.0) - 1.0))) );
		}
		a2 = exp(-w0T / Q);
		b2 = exp(-w1T / Q);
	}
	else	//	1st-Order Shelf
	{
		if (nType == EQ_6DBHISHELF)
			w1 = w0 / g;
		else
			w1 = w0 * g;
		w1T = w1*T;

		a1 = -exp(-w0T);
		b1 = -exp(-w1T);
		a2 = 0.0;
		b2 = 0.0;
	}

	//	Invert the filter if it is a cut filter
	if (pParameters[SHELF_PARAMS_BOOSTDB] < 0.0) // -dB is a cut
	{	// swap a and b 
		double temp = a1;
		a1 = b1;
		b1 = temp;
		temp = a2;
		a2 = b2;
		b2 = temp;
	}

	//	Calculate required makeup gain to give correct results at DC
	
	if ((nType == EQ_12DBHISHELF) || (nType == EQ_6DBHISHELF)) {	// gain should be 1.0 at DC
		g = (1.0f + a1 + a2) / (1.0f + b1 + b2);
	} else
	{	//  gain should be boost or 1/boost at DC
		if (pParameters[SHELF_PARAMS_BOOSTDB] < 0.0f)	// determine if it is a cut filter
			g = 1.0f/g;
		g = g * (1.0f + a1 + a2) / (1.0f + b1 + b2);
	}

	// Apply gain
	m_pData[COEFF_B0] = (float) g;		// b0
	m_pData[COEFF_B1] = (float) g*b1;	// b1
	m_pData[COEFF_B2] = (float) g*b2;	// b2
	m_pData[COEFF_A1] = (float) a1;	// a1
	m_pData[COEFF_A2] = (float) a2;	// a2


	return 5;
}

int CShelf::SetParams(float frequency, float boostDB, float fEQType, float SampleRate)
{
	float Params[3];
	Params[SHELF_PARAMS_FREQ	] = frequency;
	Params[SHELF_PARAMS_BOOSTDB] = boostDB;
	Params[SHELF_PARAMS_EQTYPE	] = fEQType;
	return CalcCoefficients(&Params[0], 3, SampleRate);
}

//////////////////////////////////////////////////////////////////////
// CLowPass6dB
//////////////////////////////////////////////////////////////////////
//
//	1st-Order Lowpass Filter
//

///////////////////////////////////////////////////////////////////////////////
// Constructor
CLowPass6dB::CLowPass6dB()
	: CBiquad()
{

}

///////////////////////////////////////////////////////////////////////////////
// CalcCoefficients
int CLowPass6dB::CalcCoefficients(float* pParameters, int nParamCount, float SampleRate)
{	
	//  1/(s+1.0);

	// fills in m_pData   //  b0, b1, b2, a1, a2
	ASSERT(nParamCount == 1);
	double f = pParameters[0];
	double w0 = 2.0 * 3.1415927 * f;
	double T = 1.0 / SampleRate;
	double w0T = w0*T;
	double ga,gz;
	double a1,b0;

	// a1
	a1 = -exp(-w0T);
	m_pData[COEFF_A1] = a1;
	// a2
	m_pData[COEFF_A2] = 0.0f;
	// compute overall gain = 1 + a1
//	m_pData[COEFF_B0] = 1.0f + a1;
	//	ga is the gain of the ideal analog filter at 1/2-Nyquist (subst j(Fnyq/2)/Fc for s (Fc is res freq))
	ga = fabs(1.0f/sqrt(1.0f + pow(pi/(2*w0T), 2.0)));
	//	gz is the natural gain of the IIR filter at 1/2-Nyquist
	gz = fabs((1.0f+a1)/sqrt(1.0f + exp(-2.0*w0T)));
	// compute new b0
	b0 = (1.0f + a1) * (1.0 + sqrt(2.0 * pow(ga / gz, 2.0) - 1)) / 2.0f;
	m_pData[COEFF_B0] = (float) b0;
	// b1 = 1 + a1 - b0
	m_pData[COEFF_B1] = (float)(1.0f + a1 - b0);
	// b2 = 0
	m_pData[COEFF_B2] = 0.0f;
	return 5;
}

///////////////////////////////////////////////////////////////////////////////
// SetParams
int CLowPass6dB::SetParams(float frequency, float SampleRate)
{
	float Params[1];
	Params[0] = frequency;
	return CalcCoefficients(&Params[0], 1, SampleRate);		// returns the number of coeffs
}
//////////////////////////////////////////////////////////////////////
// CLowPass12dB
//////////////////////////////////////////////////////////////////////
//
//	2nd-Order Lowpass Filter
//

///////////////////////////////////////////////////////////////////////////////
// Constructor
CLowPass12dB::CLowPass12dB()
	: CBiquad()
{

}
int CLowPass12dB::CalcCoefficients(float* pParameters, int nParamCount, float SampleRate)
{	

	// fills in m_pData   //  b0, b1, b2, a1, a2
	ASSERT(nParamCount == 2);
	double f = pParameters[0];
	double w0 = 2.0 * 3.1415927 * f;
	double Q = pParameters[1];
	double AnalogFreq = f;
	double AnalogQ = Q;
	//	If the resonance is above Nyquist, approximate it by placing a lower Q resonance at Nyquist.
	//	Then let the two-zero correction match the target response.
	if (f > SampleRate/2.0)
	{
		double ratio = f / (SampleRate/2.0); // Ratio of center frequency to Nyquist
		f = SampleRate / 2.0;
		Q = pow(Q, 1/(ratio*ratio));
		// adjust w0 as well
		w0 = 2.0 * 3.1415927 * f;
	}

	double T = 1.0 / SampleRate;
	double w0T = w0*T;
	double a1,a2;

	if (Q >= 0.5)	// if (1/2Q)^2 < 1 then we have complex roots
	{
		a1 = ( -2.0 * exp( -w0T*0.5 / Q ) * cos(sqrt(1.0 - pow(0.5/Q, 2.0)) * w0T) );
		m_pData[COEFF_A1] = (float) a1;
	}
	else
	{
		a1 = -( exp(-w0T*((0.5/Q) + sqrt(pow(0.5/Q, 2.0) - 1.0))) + exp(-w0T*((0.5/Q) - sqrt(pow(0.5/Q, 2.0) - 1.0))) );
		m_pData[COEFF_A1] = (float) a1;
		//                          --------------------------------------------        --------------------------------------------
	}
	a2 = exp(-w0T / Q);
	m_pData[COEFF_A2] = a2;

	//	Two-Zero De-Aliasing Filter
	// Analog Filter at 1/3 & 2/3 Nyquist
	ecomplex s = ecomplex(0.0, SampleRate/(6.0*AnalogFreq)); 
	ecomplex Ha = 1.0 / (pow(s, 2.0) + s/AnalogQ + 1.0);
	double ga1 = fabs(Ha);
	s = ecomplex(0.0, SampleRate/(3.0*AnalogFreq)); 
	Ha = 1.0 / (pow(s, 2.0) + s/AnalogQ + 1.0);
	double ga2 = fabs(Ha);
	// Digital Filter at 1/3 & 2/3 Nyquist
	double gd0 = 1 / (1 + a1 + a2);
	double gd1 = 1.0 / sqrt(1.0 + a1 - a2 + pow(a1, 2.0) + a1*a2 + pow(a2, 2.0)); 
	double gd2 = 1.0 / sqrt(1.0 - a1 - a2 + pow(a1, 2.0) - a1*a2 + pow(a2, 2.0)); 
	double H[3];
	double b1,b2;
	H[0] = 1.0 / gd0;
	H[1] = ga1 / gd1;
	H[2] = ga2 / gd2;
	b1 = ( H[0] - sqrt(pow(H[0], 2.0) - 2.0 * pow(H[1], 2.0) + 2.0 * pow(H[2], 2.0)) ) / 2.0;
	m_pData[COEFF_B1] = (float) b1;
	b2 = ( 3.0 * (H[0] - b1) - sqrt(-3.0 * pow(H[0], 2.0) + 12.0 * pow(H[1], 2.0) - 6.0 * H[0] * b1 - 3.0 * pow(b1, 2.0)) ) / 6.0;
	m_pData[COEFF_B2] = (float) b2;
	m_pData[COEFF_B0] = (float) (H[0] - b1 - b2);
	// TRACE("Bs = %f, %f, %f\n", m_pData[COEFF_B0], m_pData[COEFF_B1], m_pData[COEFF_B2]);
	// TRACE("As = %f, %f\n", m_pData[COEFF_A1], m_pData[COEFF_A2]);

	
	return 5;
}

///////////////////////////////////////////////////////////////////////////////
// SetParams
int CLowPass12dB::SetParams(float frequency, float Q, float SampleRate)
{
	float Params[2];
	Params[0] = frequency;
	Params[1] = Q;
	return CalcCoefficients(&Params[0], 2, SampleRate);		// returns the number of coeffs
}

//////////////////////////////////////////////////////////////////////
// CHighPass6dB
//////////////////////////////////////////////////////////////////////
//
//	1st-Order Highpass Filter
//

///////////////////////////////////////////////////////////////////////////////
// Constructor
CHighPass6dB::CHighPass6dB()
	: CBiquad()
{

}

///////////////////////////////////////////////////////////////////////////////
// CalcCoefficients
int CHighPass6dB::CalcCoefficients(float* pParameters, int nParamCount, float SampleRate)
{	
	//  s/(s+1.0);

	// fills in m_pData   //  b0, b1, b2, a1, a2
	ASSERT(nParamCount == 1);
	double f = pParameters[0];
	double w0 = 2.0 * 3.1415927 * f;
	double T = 1.0 / SampleRate;
	double w0T = w0*T;
	double ga,gz;
	double a1;

	// a1
	a1 = (float)-exp(-w0T);
	m_pData[COEFF_A1] = (float)a1;
	// a2
	m_pData[COEFF_A2] = 0.0f;
	// compute gain compensation to match the gain to that of the analog filter at the filter's 
	//	resonant frequency (for 1st order use b0 = 1, b1 = -1, b2 = 0)
	m_pData[COEFF_B0] = 1.0f;		// b0
	m_pData[COEFF_B1] = -1.0f;		// b1
	m_pData[COEFF_B2] = 0.0f;		// b2
	
	// j/(j+1) is the response of the analog filter at its resonant freq (s = j)
	// H(w0T) is the response of the digital filter
	// gain comp = |(j/(j+1))/H(w0T)|
	// H(w0T) = (b0 + b1*e(jw0T))/(1 + a1*e(jw0T))
	// using e(jwT) = cos(wT) + jsin(wT)
	// computing (j/(j+1))/H(w0T) we get:
	//	for H(w0T) alone, we get
	//						num.re	=	1 - cos(w0T)
	//						num.im	=	-sin(w0T)
	//						den.re	=	1+a1cos(w0T)
	//						den.im	=	a1sin(w0T)
	// |1/H(w0T)| = sqrt((den.re^2+den.im^2)/(num.re^2+num.im^2)
	// |j/(j+1)/H(w0T)| = 1/sqrt(2) * |1/H(w0T)|

	//	ga is the gain of the ideal analog filter at its resonant frequency
	ga = fabs(1.0 / sqrt(2.0));
	//	gz is the natural gain of the IIR filter at the resonant freq
	double denr,deni,numr,numi;
	denr = 1 + a1*cos(w0T);
	deni = a1*sin(w0T);
	numr = 1-cos(w0T);
	numi = -sin(w0T);
	gz = sqrt(((numr*numr) + (numi*numi))/((denr*denr) + (deni*deni)));
	// gain compensation = ga / gz
	double gaincomp = ga / gz;
	// b0 = gain comp * b0 = gain comp (since b0 = 1)
	m_pData[COEFF_B0] = (float)(m_pData[COEFF_B0] * gaincomp);
	// adjust b1 = b1 * gain comp = -b0 (since b1 = -1)
	m_pData[COEFF_B1] = (float)(m_pData[COEFF_B1] * gaincomp);
	
	return 5;
}

///////////////////////////////////////////////////////////////////////////////
// SetParams
int CHighPass6dB::SetParams(float frequency, float SampleRate)
{
	float Params[1];
	Params[0] = frequency;
	return CalcCoefficients(&Params[0], 1, SampleRate);		// returns the number of coeffs
}
//////////////////////////////////////////////////////////////////////
// CHighPass12dB
//////////////////////////////////////////////////////////////////////
//
//	2nd-Order Highpass Filter
//
///////////////////////////////////////////////////////////////////////////////
// Constructor
CHighPass12dB::CHighPass12dB()
	: CBiquad()
{

}

///////////////////////////////////////////////////////////////////////////////
// CalcCoefficients
int CHighPass12dB::CalcCoefficients(float* pParameters, int nParamCount, float SampleRate)
{	
	// fills in m_pData = b0, b1, b2, a1, a2
	ASSERT(nParamCount == 2);
	double f = pParameters[0];
	double w0 = 2.0 * 3.1415927 * f;
	double Q = pParameters[1];
	//	If the resonance is above Nyquist, approximate it by placing a lower Q resonance at Nyquist.
	//	Then let the two-zero correction match the target response.
/* don't do this for the hpf
	if (f > SampleRate/2.0)
	{
		double ratio = f / (SampleRate/2.0); // Ratio of center frequency to Nyquist
		f = SampleRate / 2.0;
		Q = pow(Q, 1/(ratio*ratio));
	}
*/
	double T = 1.0 / SampleRate;
	double w0T = w0*T;
	double a1,a2;

	if (Q >= 0.5)
	{
		// a1
		a1 = ( -2.0 * exp( -w0T*0.5 / Q ) * cos(sqrt(1.0 - pow(0.5/Q, 2.0)) * w0T) );
	}
	else
	{
		// a1
		a1 = -( exp(-w0T*((0.5/Q) + sqrt(pow(0.5/Q, 2.0) - 1.0))) + exp(-w0T*((0.5/Q) - sqrt(pow(0.5/Q, 2.0) - 1.0))) );
		//                  --------------------------------------------        --------------------------------------------
	}
	// a1
	m_pData[COEFF_A1] = (float) a1;
	// a2
	a2 = exp(-w0T / Q);
	m_pData[COEFF_A2] = (float) a2;

	// compute gain compensation to match the gain to that of the analog filter at the filter's 
	//	resonant frequency (for 2nd order use b0=1,b1=-2,b2=1)
	m_pData[COEFF_B0] = 1.0;	// b0
	m_pData[COEFF_B1] = -2.0;	// b1
	m_pData[COEFF_B2] = 1.0;	// b2

	// jQ is the response of the analog filter at its resonant freq (s = j)
	// H(w0T) is the response of the digital filter
	// gain comp = |jQ/H(w0T)|
	// H(w0T) = (b0 + b1*e(jw0T) + b2*e(2jw0T))/(1 + a1*e(jw0T) + a2*e(2jw0T)),
	// using e(jwT) = cos(wT) + jsin(wT)
	// computing jQ/H(w0T) we get:
	//						num.re	=	-Q(a1sin(w0T)+a2sin(2w0T))
	//						num.im	=	Q(1+a1cos(w0T)+a2cos(2w0T))
	//						den.re	=	b0+b1cos(w0T)+b2cos(2w0T) 
	//						den.im	=	b2sin(2w0T)+b1sin(w0T)
	// |jQ/H(w0T)| = sqrt((num.re^2+num.im^2)/(den.re^2+den.im^2)
	// gain compensation calculation
	double denr,deni,numr,numi;
	numr = -Q*(a1*sin(w0T) + a2*sin(2.0*w0T));
	numi = Q*(1+a1*cos(w0T)+a2*cos(2.0*w0T));
	denr = 1 - 2.0*cos(w0T)+cos(2.0*w0T);		// knowing b0 = 1, b1 = -2, b2 = 1
	deni = - 2.0*sin(w0T) + sin(2.0*w0T);
	// gain compensation = sqrt((numr^2 + numi^2)/(denr^2 + deni^2))
	double gaincomp = sqrt(((numr*numr) + (numi*numi))/((denr*denr) + (deni*deni)));
	// adjust b0, b1, b2 by gain compensation
	m_pData[COEFF_B0] = (float) (m_pData[COEFF_B0] * gaincomp);	// b0
	m_pData[COEFF_B1] = (float) (m_pData[COEFF_B1] * gaincomp);	// b1
	m_pData[COEFF_B2] = (float) (m_pData[COEFF_B2] * gaincomp);	// b2

	return 5;
}

///////////////////////////////////////////////////////////////////////////////
// SetParams
int CHighPass12dB::SetParams(float frequency, float Q, float SampleRate)
{
	float Params[2];
	Params[0] = frequency;
	Params[1] = Q;
	return CalcCoefficients(&Params[0], 2, SampleRate);	// returns the number of coeffs
}


/////////////////////////////////////////////////////////////////////////////
// CLowPass

CLowPass::CLowPass()
{
	m_nCoeffNum = 20;

}

int CLowPass::SetParams(float frequency, int FilterType, float SampleRate)
{
	// fills in m_pData = array of  b0,b1,b2,a1,a2  b0,b1,b2,a1,a2 etc
	BIQUAD_COEFFS bc[4];
	float *pCoeff = (float*) &bc[0];
	int i;
	
	for(i=0;i<20;i++)
		pCoeff[i] = 0.0f;
	
	// Pass audio, even if nothing is changed
	bc[0].b0 = 1.0f;
	bc[1].b0 = 1.0f;
	bc[2].b0 = 1.0f;
	bc[3].b0 = 1.0f;
	
	CLowPass6dB		LP6;
	CLowPass12dB	LP12;

	double w0T = 2 * pi * frequency / SampleRate;


	m_nParameterNum = 2;
	m_pParameters[0] = frequency;
	m_pParameters[1] = (float) FilterType;

	int nType = (int) FilterType;
	switch (nType) 
	{
	case dB6:
		LP6.SetParams((float)frequency, SampleRate);
		memcpy(&bc[0], LP6.m_pData, 5*sizeof(float));
		break;
	case BS12:
		LP12.SetParams((float)frequency * 1.27362602f, 0.577346337f, SampleRate);
		memcpy(&bc[0], LP12.m_pData, 5*sizeof(float));
		break;
	case BW12:
		LP12.SetParams((float)frequency, 1.0f/float(sqrt(2.0)), SampleRate);
		memcpy(&bc[0], LP12.m_pData, 5*sizeof(float));
		break;
	case LR12:
		LP12.SetParams((float)frequency, 0.5f, SampleRate);
		memcpy(&bc[0], LP12.m_pData, 5*sizeof(float));
		break;
	case BS18:
		// The first biquad is 1st order	
		LP6.SetParams((float)frequency * 1.327f, SampleRate);
		memcpy(&bc[1], LP6.m_pData, 5*sizeof(float));
		// Load second biquad
		LP12.SetParams((float)frequency * 1.452376349f, 0.691015486f, SampleRate);
		memcpy(&bc[0], LP12.m_pData, 5*sizeof(float));
		break;
	case BW18:
		// The first biquad is 1st order
		LP6.SetParams((float)frequency, SampleRate);
		memcpy(&bc[1], LP6.m_pData, 5*sizeof(float));
		// Load second biquad
		LP12.SetParams((float)frequency, 1.0f, SampleRate);
		memcpy(&bc[0], LP12.m_pData, 5*sizeof(float));
		break;
	case BS24:
		// Load first biquad
		LP12.SetParams((float)frequency * 1.419240138f, 0.521932972f, SampleRate);
		memcpy(&bc[0], LP12.m_pData, 5*sizeof(float));
		// Load second biquad
		LP12.SetParams((float)frequency * 1.591243869f, 0.805529953f, SampleRate);
		memcpy(&bc[1], LP12.m_pData, 5*sizeof(float));
		break;
	case LR24:
		// Load first biquad
		LP12.SetParams((float)frequency * 1.0f, float(sqrt(0.5)), SampleRate);
		memcpy(&bc[1], LP12.m_pData, 5*sizeof(float));
		// Second one is identical
		memcpy(&bc[0], LP12.m_pData, 5*sizeof(float));
		break;
	case BW24:
		// Load first biquad
		LP12.SetParams((float)frequency, 0.5411961f, SampleRate);
		memcpy(&bc[1], LP12.m_pData, 5*sizeof(float));
		// Load second biquad
		LP12.SetParams((float)frequency, 1.306562965f, SampleRate);
		memcpy(&bc[0], LP12.m_pData, 5*sizeof(float));
		break;
	case CEffect::BS30:
		// Load first biquad
		LP6.SetParams((float)frequency * 1.5069f, SampleRate);
		memcpy(&bc[2], LP6.m_pData, 5*sizeof(float));
		// Load second biquad
		LP12.SetParams((float)frequency * 1.76072364f, 0.91647077f, SampleRate);
		memcpy(&bc[1], LP12.m_pData, 5*sizeof(float));
		// Load third biquad
		LP12.SetParams((float)frequency * 1.5611041f, 0.5635348f, SampleRate);
		memcpy(&bc[0], LP12.m_pData, 5*sizeof(float));
		break;
	case CEffect::BW30:
		// The first biquad is 1st order
		LP6.SetParams((float)frequency, SampleRate);
		memcpy(&bc[2], LP6.m_pData, 5*sizeof(float));
		// Load second biquad
		LP12.SetParams((float)frequency, 0.61803399f, SampleRate);
		memcpy(&bc[1], LP12.m_pData, 5*sizeof(float));
		// Load third biquad
		LP12.SetParams((float)frequency, 1.61803399f, SampleRate);
		memcpy(&bc[0], LP12.m_pData, 5*sizeof(float));
		break;
	case CEffect::BS36:
		// Load first biquad
		LP12.SetParams((float)frequency * 1.90713063f, 1.02335835f, SampleRate);
		memcpy(&bc[2], LP12.m_pData, 5*sizeof(float));
		// Load second biquad
		LP12.SetParams((float)frequency * 1.69129957f, 0.61119528f, SampleRate);
		memcpy(&bc[1], LP12.m_pData, 5*sizeof(float));
		// Load third biquad
		LP12.SetParams((float)frequency * 1.60596885f, 0.5103174f, SampleRate);
		memcpy(&bc[0], LP12.m_pData, 5*sizeof(float));
		break;
	case CEffect::BW36:
		// Load first biquad
		LP12.SetParams((float)frequency, 0.51763809f, SampleRate);
		memcpy(&bc[2], LP12.m_pData, 5*sizeof(float));
		// Load second biquad
		LP12.SetParams((float)frequency, 0.70710678f, SampleRate);
		memcpy(&bc[1], LP12.m_pData, 5*sizeof(float));
		// Load third biquad
		LP12.SetParams((float)frequency, 1.93185165f, SampleRate);
		memcpy(&bc[0], LP12.m_pData, 5*sizeof(float));
		break;
	case CEffect::LR36:
		// Load first biquad
		LP12.SetParams((float)frequency * 1.0f, 0.5f, SampleRate);
		memcpy(&bc[2], LP12.m_pData, 5*sizeof(float));
		// Second one is identical
		LP12.SetParams((float)frequency * 1.0f, 1.0f, SampleRate);
		memcpy(&bc[1], LP12.m_pData, 5*sizeof(float));
		// Third one is identical to 2nd
		memcpy(&bc[0], LP12.m_pData, 5*sizeof(float));
		break;
	case CEffect::BS42:
		// The first biquad is 1st order	
		LP6.SetParams((float)frequency * 1.6853f, SampleRate);
		memcpy(&bc[3], LP6.m_pData, 5*sizeof(float));
		// Load second biquad
		LP12.SetParams((float)frequency * 2.05066682f, 1.12624496f, SampleRate);
		memcpy(&bc[2], LP12.m_pData, 5*sizeof(float));
		// Load third biquad
		LP12.SetParams((float)frequency * 1.82349976f, 0.66083198f, SampleRate);
		memcpy(&bc[1], LP12.m_pData, 5*sizeof(float));
		// Load fourth biquad
		LP12.SetParams((float)frequency * 1.7173809f, 0.53235614f, SampleRate);
		memcpy(&bc[0], LP12.m_pData, 5*sizeof(float));
		break;
	case CEffect::BW42:
		// The first biquad is 1st order
		LP6.SetParams((float)frequency, SampleRate);
		memcpy(&bc[3], LP6.m_pData, 5*sizeof(float));
		// Load second biquad
		LP12.SetParams((float)frequency, 0.55495813f, SampleRate);
		memcpy(&bc[2], LP12.m_pData, 5*sizeof(float));
		// Load third biquad
		LP12.SetParams((float)frequency, 0.80193774f, SampleRate);
		memcpy(&bc[1], LP12.m_pData, 5*sizeof(float));
		// Load fourth biquad
		LP12.SetParams((float)frequency, 2.2469796f, SampleRate);
		memcpy(&bc[0], LP12.m_pData, 5*sizeof(float));
		break;
	case CEffect::BS48:
		// Load first biquad
		LP12.SetParams((float)frequency * 1.78382258f, 0.50599154f, SampleRate);
		memcpy(&bc[0], LP12.m_pData, 5*sizeof(float));
		// Load second biquad
		LP12.SetParams((float)frequency * 2.19534499f, 1.22576493f, SampleRate);
		memcpy(&bc[1], LP12.m_pData, 5*sizeof(float));
		// Load third biquad
		LP12.SetParams((float)frequency * 1.95913725f, 0.71086257f, SampleRate);
		memcpy(&bc[2], LP12.m_pData, 5*sizeof(float));
		// Load fourth biquad
		LP12.SetParams((float)frequency * 1.8376495f, 0.55961067f, SampleRate);
		memcpy(&bc[3], LP12.m_pData, 5*sizeof(float));
		break;
	case CEffect::BW48:
		// Load first biquad
		LP12.SetParams((float)frequency, 2.56291545f, SampleRate);
		memcpy(&bc[0], LP12.m_pData, 5*sizeof(float));
		// Load second biquad
		LP12.SetParams((float)frequency, 0.89997622f, SampleRate);
		memcpy(&bc[1], LP12.m_pData, 5*sizeof(float));
		// Load third biquad
		LP12.SetParams((float)frequency, 0.60134489f, SampleRate);
		memcpy(&bc[2], LP12.m_pData, 5*sizeof(float));
		// Load fourth biquad
		LP12.SetParams((float)frequency, 0.50979558f, SampleRate);
		memcpy(&bc[3], LP12.m_pData, 5*sizeof(float));
		break;
	case CEffect::LR48:
		// Load first biquad
		LP12.SetParams((float)frequency * 1.0f, 0.5411961f, SampleRate);
		memcpy(&bc[3], LP12.m_pData, 5*sizeof(float));
		// Second one is identical
		memcpy(&bc[2], LP12.m_pData, 5*sizeof(float));
		// Third one
		LP12.SetParams((float)frequency * 1.0f, 1.30656296f, SampleRate);
		memcpy(&bc[1], LP12.m_pData, 5*sizeof(float));
		// Fourth is identical
		memcpy(&bc[0], LP12.m_pData, 5*sizeof(float));
		break;
	default:	// including THRU
		bc[0].b0 = 1.0f;
		bc[1].b0 = 1.0f;
		bc[2].b0 = 1.0f;
		bc[3].b0 = 1.0f;
	}

	// status:
	//    pCoeffs = array of b0,b1,b2,a1,a2, b0,b1,b2,a1,a2 etc
	for(i=0;i<20;i++)
		m_pData[i] = pCoeff[i];

	return 20;
}

/////////////////////////////////////////////////////////////////////////////
// CHighPass

CHighPass::CHighPass()
{
	m_nCoeffNum = 20;
}

int CHighPass::SetParams(float frequency, int FilterType, float SampleRate)
{
	BIQUAD_COEFFS bc[4];
	float *pCoeff = (float*) &bc[0];
	int i;
	for(i=0;i<20;i++)
		pCoeff[i] = 0.0f;
	// Pass audio, even if nothing is changed
	bc[0].b0 = 1.0f;
	bc[1].b0 = 1.0f;
	bc[2].b0 = 1.0f;
	bc[3].b0 = 1.0f;

	CHighPass6dB HP6;
	CHighPass12dB HP12;

	double w0T = 2 * pi * frequency / SampleRate;

	int nType = (int) FilterType;

	switch(nType)
	{
	case dB6:
		HP6.SetParams((float)frequency, SampleRate);
		memcpy(&bc[0], HP6.m_pData, 5*sizeof(float));
		break;
	case BS12:
		HP12.SetParams((float)frequency / 1.27362602f, 0.577346337f, SampleRate);
		memcpy(&bc[0], HP12.m_pData, 5*sizeof(float));
		break;
	case BW12:
		HP12.SetParams((float)frequency, 1.0f/float(sqrt(2.0)), SampleRate);
		memcpy(&bc[0], HP12.m_pData, 5*sizeof(float));
		break;
	case LR12:
		HP12.SetParams((float)frequency, 0.5f, SampleRate);
		memcpy(&bc[0], HP12.m_pData, 5*sizeof(float));
		break;
	case BS18:
		// The first biquad is 1st order	
		HP6.SetParams((float)frequency / 1.327f, SampleRate);
		memcpy(&bc[1], HP6.m_pData, 5*sizeof(float));
		// Load second biquad
		HP12.SetParams((float)frequency / 1.452376349f, 0.691015486f, SampleRate);
		memcpy(&bc[0], HP12.m_pData, 5*sizeof(float));
		break;
	case BW18:
		// The first biquad is 1st order
		HP6.SetParams((float)frequency, SampleRate);
		memcpy(&bc[1], HP6.m_pData, 5*sizeof(float));
		// Load second biquad
		HP12.SetParams((float)frequency, 1.0f, SampleRate);
		memcpy(&bc[0], HP12.m_pData, 5*sizeof(float));
		break;
	case BS24:
		// Load first biquad
		HP12.SetParams((float)frequency / 1.419240138f, 0.521932972f, SampleRate);
		memcpy(&bc[0], HP12.m_pData, 5*sizeof(float));
		// Load second biquad
		HP12.SetParams((float)frequency / 1.591243869f, 0.805529953f, SampleRate);
		memcpy(&bc[1], HP12.m_pData, 5*sizeof(float));
		break;
	case LR24:
		// Load first biquad
		HP12.SetParams((float)frequency * 1.0f, float(sqrt(0.5)), SampleRate);
		memcpy(&bc[0], HP12.m_pData, 5*sizeof(float));
		// Second one is identical
		memcpy(&bc[1], HP12.m_pData, 5*sizeof(float));
		break;
	case BW24:
		// Load first biquad
		HP12.SetParams((float)frequency, 0.5411961f, SampleRate);
		memcpy(&bc[1], HP12.m_pData, 5*sizeof(float));
		// Load second biquad
		HP12.SetParams((float)frequency, 1.306562965f, SampleRate);
		memcpy(&bc[0], HP12.m_pData, 5*sizeof(float));
		break;
	case CEffect::BS30:
		// Load first biquad
		HP6.SetParams((float)frequency / 1.5069f, SampleRate);
		memcpy(&bc[2], HP6.m_pData, 5*sizeof(float));
		// Load second biquad
		HP12.SetParams((float)frequency / 1.76072364f, 0.91647077f, SampleRate);
		memcpy(&bc[1], HP12.m_pData, 5*sizeof(float));
		// Load third biquad
		HP12.SetParams((float)frequency / 1.5611041f, 0.5635348f, SampleRate);
		memcpy(&bc[0], HP12.m_pData, 5*sizeof(float));
		break;
	case CEffect::BW30:
		// The first biquad is 1st order
		HP6.SetParams((float)frequency, SampleRate);
		memcpy(&bc[2], HP6.m_pData, 5*sizeof(float));
		// Load second biquad
		HP12.SetParams((float)frequency, 0.61803399f, SampleRate);
		memcpy(&bc[1], HP12.m_pData, 5*sizeof(float));
		// Load third biquad
		HP12.SetParams((float)frequency, 1.61803399f, SampleRate);
		memcpy(&bc[0], HP12.m_pData, 5*sizeof(float));
		break;
	case CEffect::BS36:
		// Load first biquad
		HP12.SetParams((float)frequency / 1.90713063f, 1.02335835f, SampleRate);
		memcpy(&bc[2], HP12.m_pData, 5*sizeof(float));
		// Load second biquad
		HP12.SetParams((float)frequency / 1.69129957f, 0.61119528f, SampleRate);
		memcpy(&bc[1], HP12.m_pData, 5*sizeof(float));
		// Load third biquad
		HP12.SetParams((float)frequency / 1.60596885f, 0.5103174f, SampleRate);
		memcpy(&bc[0], HP12.m_pData, 5*sizeof(float));
		break;
	case CEffect::BW36:
		// Load first biquad
		HP12.SetParams((float)frequency, 0.51763809f, SampleRate);
		memcpy(&bc[2], HP12.m_pData, 5*sizeof(float));
		// Load second biquad
		HP12.SetParams((float)frequency, 0.70710678f, SampleRate);
		memcpy(&bc[1], HP12.m_pData, 5*sizeof(float));
		// Load third biquad
		HP12.SetParams((float)frequency, 1.93185165f, SampleRate);
		memcpy(&bc[0], HP12.m_pData, 5*sizeof(float));
		break;
	case CEffect::LR36:
		// Load first biquad
		HP12.SetParams((float)frequency * 1.0f, 0.5f, SampleRate);
		memcpy(&bc[2], HP12.m_pData, 5*sizeof(float));
		// Second one is identical
		HP12.SetParams((float)frequency * 1.0f, 1.0f, SampleRate);
		memcpy(&bc[0], HP12.m_pData, 5*sizeof(float));
		// Third one is identical to 2nd
		memcpy(&bc[1], HP12.m_pData, 5*sizeof(float));
		break;
	case CEffect::BS42:
		// The first biquad is 1st order	
		HP6.SetParams((float)frequency / 1.6853f, SampleRate);
		memcpy(&bc[3], HP6.m_pData, 5*sizeof(float));
		// Load second biquad
		HP12.SetParams((float)frequency / 2.05066682f, 1.12624496f, SampleRate);
		memcpy(&bc[2], HP12.m_pData, 5*sizeof(float));
		// Load third biquad
		HP12.SetParams((float)frequency / 1.82349976f, 0.66083198f, SampleRate);
		memcpy(&bc[1], HP12.m_pData, 5*sizeof(float));
		// Load fourth biquad
		HP12.SetParams((float)frequency / 1.7173809f, 0.53235614f, SampleRate);
		memcpy(&bc[0], HP12.m_pData, 5*sizeof(float));
		break;
	case CEffect::BW42:
		// The first biquad is 1st order
		HP6.SetParams((float)frequency, SampleRate);
		memcpy(&bc[3], HP6.m_pData, 5*sizeof(float));
		// Load second biquad
		HP12.SetParams((float)frequency, 0.55495813f, SampleRate);
		memcpy(&bc[2], HP12.m_pData, 5*sizeof(float));
		// Load third biquad
		HP12.SetParams((float)frequency, 0.80193774f, SampleRate);
		memcpy(&bc[1], HP12.m_pData, 5*sizeof(float));
		// Load fourth biquad
		HP12.SetParams((float)frequency, 2.2469796f, SampleRate);
		memcpy(&bc[0], HP12.m_pData, 5*sizeof(float));
		break;
	case CEffect::BS48:
		// Load first biquad
		HP12.SetParams((float)frequency / 1.78382258f, 0.50599154f, SampleRate);
		memcpy(&bc[0], HP12.m_pData, 5*sizeof(float));
		// Load second biquad
		HP12.SetParams((float)frequency / 2.19534499f, 1.22576493f, SampleRate);
		memcpy(&bc[1], HP12.m_pData, 5*sizeof(float));
		// Load third biquad
		HP12.SetParams((float)frequency / 1.95913725f, 0.71086257f, SampleRate);
		memcpy(&bc[2], HP12.m_pData, 5*sizeof(float));
		// Load fourth biquad
		HP12.SetParams((float)frequency / 1.8376495f, 0.55961067f, SampleRate);
		memcpy(&bc[3], HP12.m_pData, 5*sizeof(float));
		break;
	case CEffect::BW48:
		// Load first biquad
		HP12.SetParams((float)frequency, 2.56291545f, SampleRate);
		memcpy(&bc[0], HP12.m_pData, 5*sizeof(float));
		// Load second biquad
		HP12.SetParams((float)frequency, 0.89997622f, SampleRate);
		memcpy(&bc[1], HP12.m_pData, 5*sizeof(float));
		// Load third biquad
		HP12.SetParams((float)frequency, 0.60134489f, SampleRate);
		memcpy(&bc[2], HP12.m_pData, 5*sizeof(float));
		// Load fourth biquad
		HP12.SetParams((float)frequency, 0.50979558f, SampleRate);
		memcpy(&bc[3], HP12.m_pData, 5*sizeof(float));
		break;
	case CEffect::LR48:
		// Load first biquad
		HP12.SetParams((float)frequency * 1.0f, 0.5411961f, SampleRate);
		memcpy(&bc[2], HP12.m_pData, 5*sizeof(float));
		// Second one is identical
		memcpy(&bc[3], HP12.m_pData, 5*sizeof(float));
		// Third one
		HP12.SetParams((float)frequency * 1.0f, 1.30656296f, SampleRate);
		memcpy(&bc[0], HP12.m_pData, 5*sizeof(float));
		// Fourth is identical
		memcpy(&bc[1], HP12.m_pData, 5*sizeof(float));
		break;
	default:	// including THRU
		bc[0].b0 = 1.0f;
		bc[1].b0 = 1.0f;
		bc[2].b0 = 1.0f;
		bc[3].b0 = 1.0f;
	}

	// status:
	//    pCoeffs = array of b0,b1,b2,a1,a2, b0,b1,b2,a1,a2 etc
	for(i=0;i<20;i++)
		m_pData[i] = pCoeff[i];

	return 20;
}

/////////////////////////////////////////////////////////////////////////////
// CLinearPhaseFilter

CLinearPhaseFilter::CLinearPhaseFilter()
{

	int i;
	BIQUAD_COEFFS bc[2];
	float *pCoeff = (float*) &bc[0];

	m_nCoeffNum = 10;
	for(i=0;i<m_nCoeffNum;i++)
		pCoeff[i] = 0.0f;
	// Pass audio, even if nothing is changed
	bc[0].b0 = 1.0f;
	bc[1].b0 = 1.0f;


	for(i=0;i<m_nCoeffNum;i++)
		m_pData[i] = pCoeff[i];

}


int CLinearPhaseFilter::SetParams(bool bEnable, float fFreq, float fPhase, int nOrder, float SampleRate)
{
	float Params[4];


	Params[0] = (bEnable?1.0:0);
	Params[1] = fFreq;
	Params[2] = fPhase;
	Params[3] = nOrder;

	m_nCoeffNum = CalcCoefficients(&Params[0], 4, SampleRate);	// returns the number of coeffs and fills m_pData

	return m_nCoeffNum; 
}


// -------------------------------------------------------------------------------
//	CLinearPhaseFilter::CalcCoefficients
//	Purpose		:	Compute the 2 biquad allpass
//	Comments	:	Calculates 2, 3, or 4 1st order allpass filters and
//					combines them into 2 biquads.
//					
//					Outer allpass filters are computed from the center
//					frequency with the width parameter.  Inner filters
//					are computed according to the width and the number of
//					allpass filters.
//						eg. 	2	F1,	F2,  n/a, n/a	(F1, F2 determined by BW)
//								3	F1,	F2,  Fc,  n/a   (F1, F2 determined by BW)
//								4	F1, F2,  F3,  F4    (F1, F2 determined by BW, F3, F4 evenly spaced between F1, F2)
//								
//					Once each Frequency has been determined a very simple
//					calculation is used to compute the coefficients.
//						ie.		wx = 2*pi*Fx/Fs
//								B1 = -e^(wx)
//								A1 = 1/B1
//								G  = A1
//								
//					Then the filters are combined with the simple relation
//					
//						B2 = B1a * B1b
//						B1 = B1a + B1b
//						A2 = A1a * A1b
//						A1 = A1a + A1b
//						G  = Ga  * Gb
//					
//					The Width table provides a ratio of Fx/Fc to space the allpass properly.
//					The slope parameter is used to move Fc based on the phase.
//						eg.	Fc    = 38 Hz
//							Phase = 90 degrees
//							Order = 3
//							
//							Fc	= 38 * e^(90*0.0139437)
//								= 38 * e^(1.254933)
//								= 38 * 3.50760
//							Fc	= 133.289
//
//							Phase @ 38 Hz = 177.965 (nominal -270) => 92.035 degrees
//					
//
//-----------------------------------------------------------------


typedef struct tLinearPhaseFilterWidthtable_tag
	{
		float f1;
		float f2;
		float f3;
		float f4;
		float fSlope;
} tLinearPhaseFilterWidthtable;

tLinearPhaseFilterWidthtable gLinearPhaseFilterWidthtable[3] = {
	//	F1			F2			F3			F4			Slope
	{0.206725,	4.83735,	0.00000,	0.00000,	0.0186279},	//	2nd order	  23.4
	{0.0953463,	1.00000,	10.4881,	0.00000,	0.0131779},	//	3rd order	 110
	{0.0162221,	0.253145,	3.95031,	61.6441,	0.0153633} //	4th order	3800

};	

int CLinearPhaseFilter::CalcCoefficients(float* pParameters, int nParamCount, float fSampleRate)
{
	int i;
	BIQUAD_COEFFS bc[2];			// {b0, b1, b2, a1, a0}, {b0, b1, b2, a1, a0}
	float *pCoeff = (float*) &bc[0];
	bool bEnable =  (pParameters[1] != 0)?true:false;
	float fFreq = pParameters[1]; 
	float fPhase = pParameters[2];;
	int nOrder = pParameters[3];
	float fPhaseInversion;
	double fFreqRatio;
	double fNewFc;
	double f[4], a1[4],b1[4],g[4];	
	double final_a2,final_a1,final_b2,final_b1,final_g;
	double fPIFS;


	nOrder = nOrder -2;
	if (nOrder < 0) nOrder= 0;
	if (nOrder > 2) nOrder= 2;

	m_nCoeffNum = 10;


	// ------------------------------
	// init
	// ------------------------------
	// Pass audio, even if nothing is changed
	for(i=0;i<m_nCoeffNum;i++)
		pCoeff[i] = 0.0f;
	bc[0].b0 = 1.0f;		
	bc[1].b0 = 1.0f;
	// Disable_Linear_Phase_Biquad_Coefs   (should be gain=1.0, a1,a2,b1,b2 = 0

	// ------------------------------
	// Calculate the Coeffs
	// ------------------------------
	if (bEnable)
	{

		//f2	=	90.0;
		//f8	=	180.0;
		//comp(f4,f2),	f1	=	m5;
		//if gt f4 = f4 - f8,		f1	=	f8;
		//f2	=	-90.0;
		//f8	=	-180.0;
		//comp(f4,f2);
		//if le f4 = f4 - f8,		f1	=	f8;

		fPhaseInversion = 0;
		if (fPhase > 90)
		{
			fPhase = fPhase-180;
			fPhaseInversion = 180;
		}

		if (fPhase <= -90)
		{
			fPhase = fPhase+180;
			fPhaseInversion = -180;
		}



		//r0	=	dm(FILTER_TYPE,i5);
		//r1	=	fext r0 by FILTER_TYPE_ORDER;
		//r2	=	2;
		//r1	=	r1	- r2;
		//r2	=	5;
		//r1	=	r1	* r2 (UUI);
		//i4	=	WidthTable;
		//m4	=	r1;
		//						modify(i4,m4);
		//						pushi(i4,3);							//	save address of conversion parms


		// have gLinearPhaseFilterWidthtable[nOrder] = i4

		//						
		////	F4		Phase
		////	I4		Conversion parms
		//
		//call	MA_Exp (db);
		//						f0	=	dm(4,i4);						//	get frequency shift coef
		//f4	=	f4	* f0;

		fPhase = fPhase*gLinearPhaseFilterWidthtable[nOrder].fSlope;	// (f4)
		fFreqRatio = exp((double)fPhase);								// (f0)


		//					f4	=	dm(FILTER_FC_CURRENT,i5);
		// calculate new Fc
		fNewFc = fFreq*fFreqRatio;		// 	f8	=	f4	* f0;

		
		//								f0	=	dm(0,i4);
		//f0	=	f8	* f0,		f1	=	dm(1,i4);						//	calc F1
		//f1	=	f8	* f1,		f2	=	dm(2,i4);						//	calc F2
		//f2	=	f8	* f2,		f4	=	dm(3,i4);						//	calc F3
		//f4	=	f8	* f4;												//	calc F4

		f[0] = fNewFc*gLinearPhaseFilterWidthtable[nOrder].f1;		// register f0
		f[1] = fNewFc*gLinearPhaseFilterWidthtable[nOrder].f2;		// register f1
		f[2] = fNewFc*gLinearPhaseFilterWidthtable[nOrder].f3;		// register f2
		f[3] = fNewFc*gLinearPhaseFilterWidthtable[nOrder].f4;		// register f4



		//								f8	=	pm(MATH_TABLE + PI_DIV_FS);
		//f0	=	f0	* f8;												//	pi*F1/Fs
		//f1	=	f1	* f8;                                               //	pi*F2/Fs
		//f2	=	f2	* f8;												//	pi*F3/Fs
		//f4	=	f4	* f8;												//	pi*F4/Fs
		//
		//f0	=	f0	+ f0;												//	2*pi*F1/Fs
		//f1	=	f1	+ f1,		pushr(f0,5);							//	2*pi*F2/Fs			save 2*pi*F1/Fs
		//f2	=	f2	+ f2,		pushr(f1,6);							//	2*pi*F3/Fs			save 2*pi*F2/Fs
		//f4	=	f4	+ f4,		pushr(f2,7);							//	2*pi*F4/Fs			save 2*pi*F3/Fs
		//						pushr(f4,8);							//						save 2*pi*F4/Fs

		fPIFS = (double)pi/(double)fSampleRate; 
		f[0] = 2*f[0]*fPIFS;		// register f0
		f[1] = 2*f[1]*fPIFS;		// register f1
		f[2] = 2*f[2]*fPIFS;		// register f2
		f[3] = 2*f[3]*fPIFS;		// register f4

		//	we now have the 4 frequencies on the stack
		//	A1	=	-exp(2*pi*Fx/Fs)		F0
		//	B1	=	1/A1					F1
		//	G	=	A1						F2

	

							//pushr(f0,9);							//	F1		A1
							//pushr(f1,10);							//			B1
							//pushr(f2,11);							//			G
	
							//pushr(f0,12);							//	F2		A1
							//pushr(f1,13);							//			B1
							//pushr(f2,14);							//			G
	
							//pushr(f0,15);							//	F3		A1
							//pushr(f1,16);							//			B1
							//pushr(f2,17);							//			G
	
							//pushr(f0,18);							//	F4		A1
							//pushr(f1,19);							//			B1
							//pushr(f2,20);							//			G

		
		//			r0	=	r0	xor r0,		r1	=	m5;					(r0=0,r1=0,f2=1.0)
		//	f2	=	1.0;

		//f4	=	pass f4;												//	check 2*piF4/Fs
		//if eq jump clp_4_done;											//	if 0, filter 4 is done
		//call	MA_Exp;													//	compute exp(2*pi*F4/Fs)
		//call	MA_Inv (db);											//  compute A1 = 1/B1
		//f8	=	-f0;													//	B1	=	-exp(2*piF4/Fs)
		//						pushr(f8,19);							//	save B1
		//						pushr(f0,18);							//	save A1
		//						pushr(f0,20);							//	save G



		for(i=0;i<4;i++)
		{
			a1[i] = 0;
			b1[i] = 0;
			g[i] = 1.0;

			if (f[i] != 0)
			{
				b1[i] = -exp((double)f[i]);
				a1[i] = 1/b1[i];
				g[i] = a1[i];
			}

		}



		//								popr(f0,9);								// get filter 1 A1
		//						popr(f1,12);							// get filter 2 A1
		//f2	=	f0	* f1,		popr(f12,10);							// biquad 1 A2 = filter 1 A1 * filter 2 A1, get filter 1 B1
		//f1	=	f0	+ f1,		popr(f0,13);							// biquad 1 A1 = filter 1 A1 + filter 2 A1, get filter 2 B1
		//f8	=	f0	* f12;												// biquad 1 B2 = filter 1 B1 * filter 2 B1
		//f4	=	f0	+ f12,		popr(f0,11);							// biquad 1 B1 = filter 1 B1 + filter 2 B1, get filter 1 G
		//						popr(f12,14);							// get filter 2 G
		//f0	=	f0	* f12,		popr(f12,2);							// biquad 1 G = filter 1 G * filter 2 G, get phase invert flag
		//call	Set_Biquad_Coefs (db);									// set biquad coefs
		//f12	=	pass f12;												// check phase invert flag
		//if ne f0 = -f0;													// invert G if phase invert flag is set



		//call	Set_Biquad_Coefs (db);									// set biquad coefs


		final_a2 = a1[0]*a1[1];
		final_a1 = a1[0]+a1[1];
		final_b2 = b1[0]*b1[1];
		final_b1 = b1[0]+b1[1];

		final_g = g[0]*g[1];	
		if (fPhaseInversion != 0) final_g = 0-final_g;
	//	we now have
	//	F0		Gain
	//	F1		A1
	//	F2		A2
	//	F4		B1
	//	F8		B2
		//call	Set_Biquad_Coefs (db);									// set biquad coefs
		bc[0].a1 = final_a1;
		bc[0].a2 = final_a2;
		bc[0].b1 = final_b1*final_g;
		bc[0].b2 = final_b2*final_g;
		bc[0].b0 = final_g;



		


	//								pushr(f0,15);							//	F3		A1
	//						pushr(f1,16);							//			B1
	//						pushr(f2,17);							//			G
	//
	//						pushr(f0,18);							//	F4		A1
	//						pushr(f1,19);							//			B1
	//						pushr(f2,20);							//			G


	//								popr(f0,15);	a1						// same for biquad 2
	//						popr(f1,18);			a1
	//f2	=	f0	* f1,		popr(f12,16);		b1
	//f1	=	f0	+ f1,		popr(f0,19);		b1
	//f8	=	f0	* f12;
	//f4	=	f0	+ f12,		popr(f0,17);		g
	//call	Set_Biquad_Coefs (db);
	//						popr(f12,20);
	//f0	=	f0	* f12;						g


		// Do the same for 2nd biquad
		final_a2 = a1[2]*a1[3];
		final_a1 = a1[2]+a1[3];
		final_b2 = b1[2]*b1[3];
		final_b1 = b1[2]+b1[3];
		final_g = g[2]*g[3];	
		//if (fPhaseInversion != 0) final_g = 0-final_g;
	//	we now have
	//	F0		Gain
	//	F1		A1
	//	F2		A2
	//	F4		B1
	//	F8		B2
		//call	Set_Biquad_Coefs (db);									// set biquad coefs
		bc[1].a1 = final_a1;
		bc[1].a2 = final_a2;
		bc[1].b1 = final_b1*final_g;
		bc[1].b2 = final_b2*final_g;
		bc[1].b0 = final_g;



	}
	// ------------------------------
	// move pCoeff data to m_pData
	// ------------------------------
	for(i=0;i<m_nCoeffNum;i++)
		m_pData[i] = pCoeff[i];

	return m_nCoeffNum;

}

void FreqZPoly(float *Poly, int OrderPoly, float SampFreq, float *FreqPoints, int NumPoints, float *FreqResp)
{
	int i,j;
	float realPart, imagPart, magnitude;

	for(i=0;i<NumPoints;i++)
	{
		realPart = Poly[0];
		imagPart = 0;
		for(j=0;j<OrderPoly;j++) 
		{
			realPart = realPart + Poly[1+j] * (float) cos(2.0 * pi * FreqPoints[i] * (j+1) / SampFreq);
			imagPart = imagPart + Poly[1+j] * (float) sin(2.0 * pi * FreqPoints[i] * (j+1) / SampFreq);
		}
		magnitude = realPart * realPart + imagPart * imagPart;
		FreqResp[i] *= (float) sqrt(magnitude);			// multiply what you have before
	}
}

void FreqZBiquads(float *Coeffs, int BiquadNum, float SampFreq, float *FreqPoints, int NumPoints, float *FreqResp)
{
	// 1-8th order as Biquads  
	// coeffs are in the order b0,b1,b2,a1,a2 for each 2 orders

	int i,k;
	float *fHB, *fHA;
	float Bz[3],Az[3];

	fHB = new float [NumPoints];
	fHA = new float [NumPoints];

	for(i=0;i<NumPoints;i++)
	{
		fHB[i] = 1.0;
		fHA[i] = 1.0;
	}
	for(k=0; k< BiquadNum; k++)
	{
		Bz[0] = Coeffs[k*5+0];	// b0
		Bz[1] = Coeffs[k*5+1];	// b1
		Bz[2] = Coeffs[k*5+2];	// b2
		Az[0] = 1.0f;			// a0
		Az[1] = Coeffs[k*5+3];	// a1
		Az[2] = Coeffs[k*5+4];	// a2
		 
		FreqZPoly(Bz,2,SampFreq,FreqPoints,NumPoints,fHB);
		FreqZPoly(Az,2,SampFreq,FreqPoints,NumPoints,fHA);
	}
	for(i=0;i<NumPoints;i++)
	{
		FreqResp[i] = 20.0f * (float) log10(fHB[i]/fHA[i]);	// convert to db scale
	}

	delete [] fHB;
	delete [] fHA;
}

void FreqPhaseZPoly(float *PolyNum, float *PolyDen, int OrderPoly, float SampFreq, float *FreqPoints, int NumPoints, float *FreqResp, float* PhaseResp)
{
	int i,j;
	float realPartNum, imagPartNum;
	float realPartDen, imagPartDen;
	double realPhase,imagPhase; 

	float magnitudeNum;
	float magnitudeDen;

	for(i=0;i<NumPoints;i++)
	{
		realPartNum = PolyNum[0];
		imagPartNum = 0;
		for(j=0;j<OrderPoly;j++) 
		{
			realPartNum = realPartNum + PolyNum[1+j] * (float) cos(2.0 * pi * FreqPoints[i] * (j+1) / SampFreq);
			imagPartNum = imagPartNum + PolyNum[1+j] * (float) sin(2.0 * pi * FreqPoints[i] * (j+1) / SampFreq);
		}
		imagPartNum = -imagPartNum;
		magnitudeNum = realPartNum * realPartNum + imagPartNum * imagPartNum;

		realPartDen = PolyDen[0];
		imagPartDen = 0;
		for(j=0;j<OrderPoly;j++) 
		{
			realPartDen = realPartDen + PolyDen[1+j] * (float) cos(2.0 * pi * FreqPoints[i] * (j+1) / SampFreq);
			imagPartDen = imagPartDen + PolyDen[1+j] * (float) sin(2.0 * pi * FreqPoints[i] * (j+1) / SampFreq);
		}
		imagPartDen = -imagPartDen;
		magnitudeDen = realPartDen * realPartDen + imagPartDen * imagPartDen;

		FreqResp[i] += 20.0f * (float) log10((float) sqrt(magnitudeNum)/(float) sqrt(magnitudeDen));			// multiply what you have before

		realPhase =  ((realPartNum*realPartDen)+(imagPartNum*imagPartDen))/((realPartDen*realPartDen)+(imagPartDen*imagPartDen));//((Num.re*den.re)+(num.im*den.im))/((den.re*den.re)+(den.im*den.im))
		imagPhase =  (((imagPartNum*realPartDen)-(realPartNum*imagPartDen))/((realPartDen*realPartDen)+(imagPartDen*imagPartDen))); //((Num.im*Den.re)-(Num.re*Dem.im))/((Den.re*Den.Re)+(Den.im*Den.im))
		PhaseResp[i] += (atan2(imagPhase,realPhase)*180/pi);

		if (PhaseResp[i] < 0) 
			PhaseResp[i] += 360;

	}
}




void FreqPhaseZBiquads(float *Coeffs, int BiquadNum, float SampFreq, float *FreqPoints, int NumPoints, float *FreqResp, float *PhaseResp)
{
	// 1-8th order as Biquads  
	// coeffs are in the order b0,b1,b2,a1,a2 for each 2 orders

	int i,k;
	//float *fH;
	float Bz[3],Az[3];

	//fH = new float [NumPoints];
	//fHA = new float [NumPoints];

	for(i=0;i<NumPoints;i++)
	{
		FreqResp[i] = 0;
		PhaseResp[i] = 0;
	}
	for(k=0; k< BiquadNum; k++)
	{
		Bz[0] = Coeffs[k*5+0];	// b0
		Bz[1] = Coeffs[k*5+1];	// b1
		Bz[2] = Coeffs[k*5+2];	// b2
		Az[0] = 1.0f;			// a0
		Az[1] = Coeffs[k*5+3];	// a1
		Az[2] = Coeffs[k*5+4];	// a2
		 
		FreqPhaseZPoly(Bz,Az,2,SampFreq,FreqPoints,NumPoints,FreqResp, PhaseResp);
	}

	for (i=0;i<NumPoints; i++)
	{
		//while (PhaseResp[i] >= 360) PhaseResp[i] -= 360;
		//while (PhaseResp[i] < 0) PhaseResp[i] += 360;
	}


	//delete [] fH;
}


