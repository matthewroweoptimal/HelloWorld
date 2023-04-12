#pragma once


// CEQGraph dialog

#ifndef _EQGRAPH_H_
#define _EQGRAPH_H_
#pragma once
class CRUglyDlg;
#include "LOUD_types.h"
#include "resource.h"
#include "DSPEffect.h"
#include "ProductMap.h"



#define EQBDTYPE_LOW_PASS				( 0 )
#define EQBDTYPE_HIGH_PASS				( 1 )
#define EQBDTYPE_PARAMETRIC				( 2 )
#define EQBDTYPE_LOW_SHELF_2_ORDER		( 3 )
#define EQBDTYPE_HIGH_SHELF_2_ORDER		( 4 )
#define EQBDTYPE_LOW_SHELF_1_ORDER		( 5 )
#define EQBDTYPE_HIGH_SHELF_1_ORDER		( 6 )
//#define	EQBDTYPE_NOTCH					( 7 )
//#define EQBDTYPE_BAND_PASS				( 8 )
//#define EQBDTYPE_LOW_PASS_1_ORDER		( 9 )
//#define EQBDTYPE_HIGH_PASS_1_ORDER		( 10 )
//#define	EQBDTYPE_LOW_SHELF_VAR_Q		( 11 )
//#define	EQBDTYPE_HIGH_SHELF_VAR_Q		( 12 )

//typedef struct 
//{
//	char*  pcName;
//	char**	pParameterNameList;
//} EQGRAPH_EQSelectList;

extern "C"
{
#include "ProductMap.h"
}

#define EQGRAPH_MAX_BANDS	12
#define EQGRAPH_MAX_EDITBOX  (3*EQGRAPH_MAX_BANDS)

#define EQGRAPH_PLOT_NUM_POINTS	1024
class CEQGraph : public CDialog
{
	DECLARE_DYNAMIC(CEQGraph)

public:
	CEQGraph(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEQGraph();

	void OnTimer(UINT nIDEvent);


	CRUglyDlg* m_dlgParent;

	bool bWindowInitialized;

	int m_nProductType;
	void SetNumofBands(int nBands);
	int	m_nBands;
	virtual void OnOK();
	void SetProductType(int nProductType);
	void SetEQSelectList(EQGRAPH_EQSelectList* pEQSelectList, int nNum);
	void SetEQSelect(char* pcName);
	void OnCbnSelchangeEQSelect();

	void ShowParameter(int nTarget, int nParam, bool bAll = false);
	void ShowAll();
	void GetParameterValueString(char* pcValue,tParameterDesc*  pParameterDesc, int nTarget, int nParam);

	void SendParameterAdjust(int nTarget, int nParameterNum, char *pcValue);
	void SetEditBoxControlDesc(char** pList);
	ControlDesc		m_EditBoxDescList[EQGRAPH_MAX_EDITBOX];		// freq, q, gain
	ControlDesc		m_ComboBandTypexDescList[EQGRAPH_MAX_BANDS];		// type

	void OnCbnSelchangeEqgraphType(int nBand);

	void SetCoeffsDirty();
	bool m_bCoeffsDirty;
	void ReCalculateAllCoeffs();
	void CalculatePlots(); 
	void	DrawPlots	(int nIDC);
	void	DrawGraphBackground(CDC * dc, CRect* pRectPlot);
	void	DrawFreqResponse(CDC * dc, CRect* pRectPlot, int nNumPoints, float* fFreqPoints, float* fFreqResponse,int nPenStyle, int nWidth, COLORREF crColor);


	int	 nConvertLogXValue(float xLog, float minLog, float maxLog, int nWidth);
	int	nConvertYValue(float y, float minLevel, float maxLevel, int nHeight);


	void UpdateLocalAll();  // updates the local varaibles
	float m_fLocalFreq[EQGRAPH_MAX_BANDS];
	float m_fLocalGain[EQGRAPH_MAX_BANDS];
	float m_fLocalQ[EQGRAPH_MAX_BANDS];
	int m_fLocalType[EQGRAPH_MAX_BANDS];
	float	m_fCoeffs[EQGRAPH_MAX_BANDS*5];  // b0,b1,b2,a1,a2, b0,b1,b2,a1,a2 etc
	int		m_nNumCoeffs;



	CPEQ 		 		m_effectPEQ 		 ;   
	CShelf		 		m_effectShelf		 ;   
	CLowPass			m_effectLowPass;    
	CHighPass			m_effectHighPass;		   	  // 4 biquads	 (8th order)
	CLinearPhaseFilter	m_effectLinearPhaseFilter;		   	  // 2 biquads
	float m_fSampleRate;
	int   CalculateFilterCoeffs(int nFilterType, int nFilterSlope, int nLPFHPF, float fFreq, int* pnCoeffsNum, float** ppfCoeffs);
	int   CalculateEQCoeffs(int nEQType, float fFreq, float fGain, float fQ, int* pnCoeffsNum, float** ppfCoeffs);
	int	CalculateLinearPhaseFilterCoeffs(bool bEnable, float fFreq, float fPhase, int nOrder, int* pnCoeffsNum, float** ppfCoeffs);


	float m_fFreqMax;
	float m_fFreqMin;
	float m_fFreqLogMax;
	float m_fFreqLogMin;

	float m_fLevelMin;
	float m_fLevelMax;

	int m_nNumPoints;
	float m_fFreqPoints[EQGRAPH_PLOT_NUM_POINTS];
	float m_fFreqResponseBand[EQGRAPH_MAX_BANDS][EQGRAPH_PLOT_NUM_POINTS];	// per band
	float m_fFreqResponseSum[EQGRAPH_PLOT_NUM_POINTS];	// summation
	float m_fPhaseResponseBand[EQGRAPH_MAX_BANDS][EQGRAPH_PLOT_NUM_POINTS];	// per band
	float m_fPhaseResponseSum[EQGRAPH_PLOT_NUM_POINTS];	// summation


//		CRect		rectPlot;
	COLORREF	clrBackground;
	COLORREF	clrPlot;
	COLORREF	clrPlot2;
	COLORREF	clrAxis;		 
	COLORREF	clrGrid;

	CPen		penAxis;
	CPen		penGrid;
	CPen		penPlot;
	CPen		penPlot2;




// Dialog Data
	enum { IDD = IDD_EQGRAPH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	
	afx_msg void OnCbnSelchangeEqgraphTypeBand1();
	afx_msg void OnCbnSelchangeEqgraphTypeBand2();
	afx_msg void OnCbnSelchangeEqgraphTypeBand3();
	afx_msg void OnCbnSelchangeEqgraphTypeBand4();
	afx_msg void OnCbnSelchangeEqgraphTypeBand5();
	afx_msg void OnCbnSelchangeEqgraphTypeBand6();
	afx_msg void OnCbnSelchangeEqgraphTypeBand7();
	afx_msg void OnCbnSelchangeEqgraphTypeBand8();
	afx_msg void OnCbnSelchangeEqgraphTypeBand9();
	afx_msg void OnCbnSelchangeEqgraphTypeBand10();
	afx_msg void OnCbnSelchangeEqgraphTypeBand11();
	afx_msg void OnCbnSelchangeEqgraphTypeBand12();
	afx_msg void OnCbnSelchangeEqgraphSelect();
};

#endif