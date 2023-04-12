// DSPEffect.h: interface for the CEffect class.
//
//////////////////////////////////////////////////////////////////////

// COMPILER INFO
#if !defined(DSP_EFFECT)
#define DSP_EFFECT

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "tnt.h"

// DEFINE
#define AUDIO_TYPE float


// STRUCTS

#define COEFF_B0 0
#define COEFF_B1 1
#define COEFF_B2 2
#define COEFF_A1 3
#define COEFF_A2 4
#define COEFF_SIZE 5

typedef struct {
	float b0;
	float b1;
	float b2;
	float a1;
	float a2;
} BIQUAD_COEFFS;


/////////////////////////////////////////////////////////////////////////////
// CEffect

#define PARAMETER_MAX 5
#define DATA_MAX 20
class CEffect  
{

public:
	enum FilterType {THRU, dB6, BS12, BW12, LR12, BS18, BW18, BS24, LR24, BW24, BS30, BW30, BS36, BW36, LR36, BS42, BW42, BS48, BW48, LR48};
	enum nEQType { EQ_PARAMETRIC, EQ_6DBLOSHELF, EQ_6DBHISHELF, EQ_12DBLOSHELF, EQ_12DBHISHELF };


	//	The settings, coefficients, and previous results
public:
	float m_pParameters[PARAMETER_MAX];
	int	  m_nParameterNum;
	float m_pData[DATA_MAX];	// hold the coeffs b0,b1,b2,a1,a2, etc
	int	  m_nCoeffNum;

	//	Define the input(s) and output(s)
	long m_nFrameSize;
	int m_nInputs;
	AUDIO_TYPE** m_pInput;
	int m_nOutputs;
	AUDIO_TYPE** m_pOutput;

public:
	CEffect();
	virtual ~CEffect();
	virtual int GetReqs(int& nInputs, int& nOutputs);
	virtual int Configure(	AUDIO_TYPE** pOutputPtr,	// array of pointers to the input buffers
							AUDIO_TYPE** pInputPtr,	// array of pointers to the output buffers
							long nFrameSize);	// The length of the input & output buffers
	virtual int CalcCoefficients(float* pParameters, int nParamCount, float SampleRate = 48000.0f);
	virtual void Process();	// process the audio data
	virtual void ProcessReplacing();
	virtual float* GetData() { return m_pData; }
	virtual float* GetParameters() {return m_pParameters; }
};


/////////////////////////////////////////////////////////////////////////////
// CBiquad
class CBiquad : public CEffect
{
protected:
	float m_fLastInputs[2];
	float m_fLastOutputs[2];
public:
	CBiquad();
	virtual void ProcessReplacing();
};


/////////////////////////////////////////////////////////////////////////////
// CPEQ
enum PEQType {MZT, MZTi, BZT, ORFAN};
class CPEQ : public CBiquad
{
public:
	CPEQ();
	virtual int CalcCoefficients(float* pParameters, int nParamCount, float SampleRate = 48000.0f);
	int SetParams(float frequency, float boostDB, float Q, float SampleRate = 48000.0f);
	int m_nType; // MZT, BZT, MZTi, etc.
};


/////////////////////////////////////////////////////////////////////////////
// CShelf 

class CShelf : public CBiquad
{
public:
	CShelf();
	virtual int CalcCoefficientsOld(float* pParameters, int nParamCount, float SampleRate = 48000.0f);
	virtual int CalcCoefficients(float* pParameters, int nParamCount, float SampleRate = 48000.0f);
	int SetParams(float frequency, float boostDB, float fEQType, float SampleRate = 48000.0f);
};

/////////////////////////////////////////////////////////////////////////////
// CLowPass6dB
class CLowPass6dB : public CBiquad
{
public:
	CLowPass6dB();
	virtual int CalcCoefficients(float* pParameters, int nParamCount, float SampleRate = 48000.0f);
	int SetParams(float frequency, float SampleRate = 48000.0f);
};

/////////////////////////////////////////////////////////////////////////////
// CLowPass12dB
class CLowPass12dB : public CBiquad
{
public:
	CLowPass12dB();
	virtual int CalcCoefficients(float* pParameters, int nParamCount, float SampleRate = 48000.0f);
	int SetParams(float frequency, float Q, float SampleRate = 48000.0f);
};

/////////////////////////////////////////////////////////////////////////////
// CHighPass6dB
class CHighPass6dB : public CBiquad
{
public:
	CHighPass6dB();
	virtual int CalcCoefficients(float* pParameters, int nParamCount, float SampleRate = 48000.0f);
	int SetParams(float frequency, float SampleRate = 48000.0f);
};

/////////////////////////////////////////////////////////////////////////////
// CHighPass12dB
class CHighPass12dB : public CBiquad
{
public:
	CHighPass12dB();
	virtual int CalcCoefficients(float* pParameters, int nParamCount, float SampleRate = 48000.0f);
	int SetParams(float frequency, float Q, float SampleRate = 48000.0f);
};


/////////////////////////////////////////////////////////////////////////////
// CLowPass
class CLowPass : public CEffect
{
public:
	CLowPass();
	int SetParams(float frequency, int FilterType, float SampleRate = 48000.0f);
	
};

/////////////////////////////////////////////////////////////////////////////
// CHighPass
class CHighPass : public CEffect
{
public:
	CHighPass();
	int SetParams(float frequency, int FilterType, float SampleRate = 48000.0f);
};

/////////////////////////////////////////////////////////////////////////////
// CLinearPhaseFilter
class CLinearPhaseFilter : public CEffect
{
public:
	CLinearPhaseFilter();
	int SetParams(bool bEnable, float fFreq, float fPhase, int nOrder, float SampleRate = 48000.0f);
	int CalcCoefficients(float* pParameters, int nParamCount, float SampleRate = 48000.0f);

};


/////////////////////////////////////////////////////////////////////////////
// Freqz Function
// Calculates the Frequency response / Transfer function of a digital filter

void FreqZBiquads(float *Coeffs, int nBiquadNum, float SampFreq, float *FreqPoints, int NumPoints, float *FreqResp);
//void FreqZBiquads(float *Coeffs, int nBiquadNum, float SampFreq, float *FreqPoints, int NumPoints, float *FreqResp, float *PhaseResp);

void FreqPhaseZBiquads(float *Coeffs, int BiquadNum, float SampFreq, float *FreqPoints, int NumPoints, float *FreqResp, float *PhaseResp);

#endif