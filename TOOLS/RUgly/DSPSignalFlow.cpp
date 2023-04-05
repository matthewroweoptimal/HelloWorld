// DSPSignalFlow.cpp : implementation file
//

#include "stdafx.h"
#include "RUgly.h"
#include "DSPSignalFlow.h"


//#include "Product_arch.h"
#include "RUglyDlg.h"
//#include "ParameterAdjust.h"
//#include <stdlib.h>
//#include "resource.h"
//#include <math.h>


// CDSPSignalFlow dialog

IMPLEMENT_DYNAMIC(CDSPSignalFlow, CDialog)

CDSPSignalFlow::CDSPSignalFlow(CWnd* pParent /*=NULL*/)
	: CDialog(CDSPSignalFlow::IDD, pParent)
{

}

CDSPSignalFlow::~CDSPSignalFlow()
{
}

void CDSPSignalFlow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDSPSignalFlow, CDialog)
	ON_STN_CLICKED(IDC_DSPSIGNALFLOW_IMAGE, &CDSPSignalFlow::OnStnClickedDspsignalflowImage)
	ON_WM_LBUTTONDOWN()
	ON_CBN_SELCHANGE(IDC_DSPSIGNALFLOW_SELECT, &CDSPSignalFlow::OnCbnSelchangeDspsignalflowSelect)
END_MESSAGE_MAP()


// CDSPSignalFlow message handlers

void CDSPSignalFlow::SetProductType(int nProductType)
{
	m_nProductType = nProductType;
}

typedef struct 
{
	int		nHitZoneID;
	char	pcHitZone[50];
	int		x;
	int		y;
	int		width;
	int		height;
} tHitZone;

typedef enum {
	OLY_HITZONE_NONE = 0,

	OLY_HITZONE_GLOBAL_GAIN,	
	OLY_HITZONE_GLOBAL_LARGE_PEQ,

	OLY_HITZONE_HF_LARGE_PEQ,	
	OLY_HITZONE_HF_DYNEQ1,		
	OLY_HITZONE_HF_DYNEQ2,		
	OLY_HITZONE_HF_GAIN,		
	OLY_HITZONE_HF_LIMITERA,	
	OLY_HITZONE_HF_LIMITERB,	
	OLY_HITZONE_HF_SMALL_PEQ,	
	OLY_HITZONE_HF_DELAY,		

	OLY_HITZONE_MF_LARGE_PEQ,	
	OLY_HITZONE_MF_DYNEQ1,		
	OLY_HITZONE_MF_DYNEQ2,		
	OLY_HITZONE_MF_GAIN,		
	OLY_HITZONE_MF_LIMITERA,	
	OLY_HITZONE_MF_LIMITERB,	
	OLY_HITZONE_MF_SMALL_PEQ,	
	OLY_HITZONE_MF_DELAY,		

	OLY_HITZONE_LF_LARGE_PEQ,	
	OLY_HITZONE_LF_DYNEQ1,		
	OLY_HITZONE_LF_DYNEQ2,		
	OLY_HITZONE_LF_GAIN,		
	OLY_HITZONE_LF_LIMITERA,	
	OLY_HITZONE_LF_LIMITERB,	
	OLY_HITZONE_LF_SMALL_PEQ,	
	OLY_HITZONE_LF_DELAY,		

	OLY_HITZONE_GROUP_CROSSOVER,
	OLY_HITZONE_GROUP_PEQ,		
	OLY_HITZONE_GROUP_DELAY,	
	OLY_HITZONE_GROUP_LIMITER,	
	OLY_HITZONE_GROUP_LEVEL,	
	OLY_HITZONE_GROUP_POLARITY,

	OLY_HITZONE_THROW_SM_PEQ,	 	
	OLY_HITZONE_SPEAKER_PROFILE_PEQ,	 	
	OLY_HITZONE_AIR_CORRECTION,	
	OLY_HITZONE_CARDIOID_CONTROL,	
	OLY_HITZONE_USER_GAIN,		
	OLY_HITZONE_USER_CROSSOVER,	
	OLY_HITZONE_USER_PEQ,		
	OLY_HITZONE_USER_DELAY,		
	OLY_HITZONE_USER_LIMITER,	
	OLY_HITZONE_USER_POLARITY,	


	OLY_HITZONE_INPUT_MODE,
	OLY_HITZONE_INPUT_FUNCGEN,


	OLY_HITZONE_MAX
} eOLYHitZoneID;


#define OLYDSPSignalFlow_SIZE 26
tHitZone OLYDSPSignalFlow[OLYDSPSignalFlow_SIZE] =
{
      {OLY_HITZONE_GLOBAL_LARGE_PEQ,      "Global Large PEQ 10 band",   67,197,47, 46},

      {OLY_HITZONE_HF_LARGE_PEQ,    "HF Large PEQ 10 band", 188,67,47, 48},
      {OLY_HITZONE_HF_DYNEQ1,       "HF Dynamic EQ 1",            288,67,47, 47},
      {OLY_HITZONE_HF_DYNEQ2,       "HF Dynamic EQ 2",            341,67,47, 47},
      {OLY_HITZONE_HF_GAIN,         "HF GAIN",                    390,70,39, 39},
      {OLY_HITZONE_HF_LIMITERA,     "HF Limiter A",               440,66,47, 47},
      {OLY_HITZONE_HF_LIMITERB,     "HF Limiter B",               499,67,46, 47},
      {OLY_HITZONE_HF_SMALL_PEQ,    "HF Large PEQ 4 band",  546,67,46, 46},
      {OLY_HITZONE_HF_DELAY,        "HF Delay",                   600,66,49, 47},

      {OLY_HITZONE_MF_LARGE_PEQ,    "MF Large PEQ 10 band", 188,197,47, 48},
      {OLY_HITZONE_MF_DYNEQ1,       "MF Dynamic EQ 1",            287,197,49, 47},
      {OLY_HITZONE_MF_DYNEQ2,       "MF Dynamic EQ 2",            341,197,48, 48},
      {OLY_HITZONE_MF_GAIN,         "MF GAIN",                    392,200,34, 38},
      {OLY_HITZONE_MF_LIMITERA,     "MF Limiter A",               440,196,47, 47},
      {OLY_HITZONE_MF_LIMITERB,     "MF Limiter B",               499,197,46, 47},
      {OLY_HITZONE_MF_SMALL_PEQ,    "MF Large PEQ 4 band",  547,197,45, 46},
      {OLY_HITZONE_MF_DELAY,        "MF Delay",                   601,195,47, 48},

      {OLY_HITZONE_LF_LARGE_PEQ,    "LF Large PEQ 10 band", 189,327,45, 47},
      {OLY_HITZONE_LF_DYNEQ1,       "LF Dynamic EQ 1",            289,328,45, 46},
      {OLY_HITZONE_LF_DYNEQ2,       "LF Dynamic EQ 2",            341,327,47, 48},
      {OLY_HITZONE_LF_GAIN,         "LF GAIN",                    391,331,36, 37},
      {OLY_HITZONE_LF_LIMITERA,     "LF Limiter A",               440,326,47, 47},
      {OLY_HITZONE_LF_LIMITERB,     "LF Limiter B",               499,326,45, 48},
      {OLY_HITZONE_LF_SMALL_PEQ,    "LF Large PEQ 4 band",  546,327,46, 45},
      {OLY_HITZONE_LF_DELAY,        "LF Delay",                   601,326,47, 46},

};

#define OLYDSPSignalFlowGroup_SIZE 16
tHitZone OLYDSPSignalFlowGroup[OLYDSPSignalFlowGroup_SIZE] =
{
      {OLY_HITZONE_GROUP_CROSSOVER, "Group Cross Over",     30,102,62, 36},
      {OLY_HITZONE_GROUP_PEQ,             "Group 7 Band EQ",      93,96,47, 47},
      {OLY_HITZONE_GROUP_DELAY,           "Group Delay",          140,100,52, 37},
      {OLY_HITZONE_GROUP_LIMITER,         "Group limiter",  193,100,52, 37},
      {OLY_HITZONE_GROUP_LEVEL,           "Group Level",          249,100,52, 37},
      {OLY_HITZONE_GROUP_POLARITY,  "Group Polarity", 305,92,64, 53},

      {OLY_HITZONE_SPEAKER_PROFILE_PEQ,        "Speaker Profile PEQ",  401,97,47, 47},
      {OLY_HITZONE_THROW_SM_PEQ,                "Throw Small PEQ",      469,91,47, 48},
      {OLY_HITZONE_AIR_CORRECTION,        "Air Correction", 557,91,47, 47},
      {OLY_HITZONE_CARDIOID_CONTROL,            "Cardioid Control",     643,90,67, 57},
      {OLY_HITZONE_USER_GAIN,       "User Gain",            744,99,42, 38},
      {OLY_HITZONE_USER_CROSSOVER,        "User Crossover", 845,96,44, 48},
      {OLY_HITZONE_USER_PEQ,              "User 7 Band EQ", 890,97,43, 45},
      {OLY_HITZONE_USER_DELAY,            "User Delay",           934,97,44, 46},
      {OLY_HITZONE_USER_LIMITER,          "User Limiter",   980,97,45, 45},
      {OLY_HITZONE_USER_POLARITY,         "User Polarity",  1027,108,40, 24},
};

#define		OLYDSPSignalFlowInput_SIZE 2
tHitZone OLYDSPSignalFlowInput[OLYDSPSignalFlowInput_SIZE] =
{
      {OLY_HITZONE_INPUT_MODE, "Input Mode",     172,75,70, 70},
	  {OLY_HITZONE_INPUT_FUNCGEN,    "Func Generator",     5,78,159, 56},
};

//#define DSPSIGNALFLOW_GROUP	  1
//#define DSPSIGNALFLOW_VOICING 2
//#define DSPSIGNALFLOW_INPUT 3

typedef struct {
	char  pcName[100];
	int nIDC;
	tHitZone* pHitZone;
	int nZones;
}  tImageArray;

tImageArray gDSPSignalFlowImages[] = 
{
	"Group+Speaker", IDC_DSPSIGNALFLOWGROUP_IMAGE,OLYDSPSignalFlowGroup, OLYDSPSignalFlowGroup_SIZE,
	"Voicing", IDC_DSPSIGNALFLOW2_IMAGE,  OLYDSPSignalFlow, OLYDSPSignalFlow_SIZE,
	"Input", IDC_OLYDSPFLOWINPUT_IMAGE,OLYDSPSignalFlowInput, OLYDSPSignalFlowInput_SIZE,

};
void CDSPSignalFlow::OnStnClickedDspsignalflowImage()
{
	// TODO: Add your control notification handler code here
}
void CDSPSignalFlow::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd* pPic;
	CRect rectPic;
	CRect rectDlg;
	CRect rectClientPic;
	CRect rectClientDlg;
	int i;
	int nID;
	CPoint diagPoint;
	tHitZone* pHitZone;
	int nHitZoneNum;
		
	pPic = GetDlgItem(gDSPSignalFlowImages[m_nImage].nIDC);
	
	if (!pPic)
		return CDialog::OnLButtonDown(nFlags, point);
	pPic->GetWindowRect(&rectPic);
	ScreenToClient(&rectPic);


//	this->GetWindowRect(&rectDlg);
//	ScreenToClient(&rectDlg);

//	rectPic.OffsetRect(-rectDlg.left, -rectDlg.top); // make relative to just the dlg

	pPic->GetClientRect(&rectClientPic);
	this->GetClientRect(&rectClientDlg);
	if ((point.y >= rectPic.top) && (point.y <= rectPic.bottom) &&
		(point.x >= rectPic.left) && (point.x <= rectPic.right) ) //(	point.y < (rectPlot.bottom + 45) )
	{
		diagPoint.x = point.x - rectPic.left;	// make co-ord relative to Pic
		diagPoint.y = point.y - rectPic.top;

		// inside the pic
		nID = OLY_HITZONE_NONE;

		pHitZone = gDSPSignalFlowImages[m_nImage].pHitZone;
		nHitZoneNum = gDSPSignalFlowImages[m_nImage].nZones;
		if (nHitZoneNum == 0) return;
		for(i=0;i<nHitZoneNum;i++)
		{
			// check if inside hit zone
			if ((diagPoint.y >= pHitZone[i].y) && (diagPoint.y <= (pHitZone[i].y+pHitZone[i].height)) &&
				(diagPoint.x >= pHitZone[i].x) && (diagPoint.x <= (pHitZone[i].x+pHitZone[i].width)) ) 
			{
				nID = pHitZone[i].nHitZoneID;
				break;
			}
 
		}

		switch(nID)
		{

		case OLY_HITZONE_GLOBAL_GAIN:
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"USER_SPEAKER_FADER");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"USER_SPEAKER_MUTE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"USER_SPEAKER_INVERT");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"USER_SPEAKER_PEQ_ENABLE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"USER_SPEAKER_PEQ_BAND1_TYPE");
			break;

		case OLY_HITZONE_GLOBAL_LARGE_PEQ:
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_GLOBAL_PEQ_ENABLE");    
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_GLOBAL_PEQ_BAND1_TYPE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_GLOBAL_PEQ_BAND1_GAIN");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_GLOBAL_PEQ_BAND1_FREQ");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_GLOBAL_PEQ_BAND1_Q");
			m_dlgParent->m_dlgEQGraph.SetEQSelect("VOICING_GLOBAL_PEQ");
			break;


		case OLY_HITZONE_HF_LARGE_PEQ:
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_HF_PEQ_ENABLE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_HF_PEQ_BAND1_TYPE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_HF_PEQ_BAND1_GAIN");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_HF_PEQ_BAND1_FREQ");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_HF_PEQ_BAND1_Q");
			m_dlgParent->m_dlgEQGraph.SetEQSelect("VOICING_HF_PEQ");
			break;
		case OLY_HITZONE_HF_DYNEQ1:		
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_HF_DYNEQ1_DETECTEN");  
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_HF_DYNEQ1_DETECTTYPE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_HF_DYNEQ1_DETECTGAIN");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_HF_DYNEQ1_DETECTFREQ");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_HF_DYNEQ1_DETECTQ");   
			break;
		case OLY_HITZONE_HF_DYNEQ2:		
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_HF_DYNEQ2_DETECTEN");  
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_HF_DYNEQ2_DETECTTYPE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_HF_DYNEQ2_DETECTGAIN");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_HF_DYNEQ2_DETECTFREQ");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_HF_DYNEQ2_DETECTQ");   
			break;
		case OLY_HITZONE_HF_GAIN:	
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_HF_FADER");     
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_HF_MUTE");      
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_HF_INVERT");    
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_HF_FADER_POST");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_HF_MUTE_POST"); 
			break;
		case OLY_HITZONE_HF_LIMITERA:	
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_HF_SOFTLIMIT_ENABLE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_HF_SOFTLIMIT_GAIN");  
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_HF_SOFTLIMIT_THRESH");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_HF_SOFTLIMIT_ATTACK");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_HF_SOFTLIMIT_RELEASE");
			break;
		case OLY_HITZONE_HF_LIMITERB:	
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_HF_KLIMITER_PEAKENABLE");  
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_HF_KLIMITER_PEAKATTACK");  
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_HF_KLIMITER_GAINRELEASE"); 
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_HF_KLIMITER_LOOKAHEAD");   
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_HF_KLIMITER_MAXPEAK");     
			break;		
		case OLY_HITZONE_HF_SMALL_PEQ:	
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_HF_PEQ_POST_ENABLE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_HF_PEQ_POST_BAND1_TYPE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_HF_PEQ_POST_BAND1_GAIN");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_HF_PEQ_POST_BAND1_FREQ");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_HF_PEQ_POST_BAND1_Q");
			m_dlgParent->m_dlgEQGraph.SetEQSelect("VOICING_HF_PEQ_POST");
			break;		
		case OLY_HITZONE_HF_DELAY:		
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_HF_DELAY_TIME");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_MF_FUNCGENLEVELOFFSET");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_MF_FADER");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_MF_MUTE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_MF_INVERT");
			break;

			// ---------------------------------------------
		case OLY_HITZONE_MF_LARGE_PEQ:
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_MF_PEQ_ENABLE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_MF_PEQ_BAND1_TYPE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_MF_PEQ_BAND1_GAIN");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_MF_PEQ_BAND1_FREQ");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_MF_PEQ_BAND1_Q");
			m_dlgParent->m_dlgEQGraph.SetEQSelect("VOICING_MF_PEQ");
			break;
		case OLY_HITZONE_MF_DYNEQ1:		
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_MF_DYNEQ1_DETECTEN");  
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_MF_DYNEQ1_DETECTTYPE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_MF_DYNEQ1_DETECTGAIN");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_MF_DYNEQ1_DETECTFREQ");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_MF_DYNEQ1_DETECTQ");   
			break;
		case OLY_HITZONE_MF_DYNEQ2:		
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_MF_DYNEQ2_DETECTEN");  
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_MF_DYNEQ2_DETECTTYPE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_MF_DYNEQ2_DETECTGAIN");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_MF_DYNEQ2_DETECTFREQ");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_MF_DYNEQ2_DETECTQ");   
			break;
		case OLY_HITZONE_MF_GAIN:	
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_MF_FADER");     
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_MF_MUTE");      
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_MF_INVERT");    
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_MF_FADER_POST");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_MF_MUTE_POST"); 
			break;
		case OLY_HITZONE_MF_LIMITERA:	
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_MF_SOFTLIMIT_ENABLE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_MF_SOFTLIMIT_GAIN");  
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_MF_SOFTLIMIT_THRESH");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_MF_SOFTLIMIT_ATTACK");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_MF_SOFTLIMIT_RELEASE");
			break;
		case OLY_HITZONE_MF_LIMITERB:	
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_MF_KLIMITER_PEAKENABLE");  
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_MF_KLIMITER_PEAKATTACK");  
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_MF_KLIMITER_GAINRELEASE"); 
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_MF_KLIMITER_LOOKAHEAD");   
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_MF_KLIMITER_MAXPEAK");     
			break;		
		case OLY_HITZONE_MF_SMALL_PEQ:	
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_MF_PEQ_POST_ENABLE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_MF_PEQ_POST_BAND1_TYPE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_MF_PEQ_POST_BAND1_GAIN");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_MF_PEQ_POST_BAND1_FREQ");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_MF_PEQ_POST_BAND1_Q");
			m_dlgParent->m_dlgEQGraph.SetEQSelect("VOICING_MF_PEQ_POST");
			break;		
		case OLY_HITZONE_MF_DELAY:		
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_MF_DELAY_TIME");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_LF_FUNCGENLEVELOFFSET");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_LF_FADER");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_LF_MUTE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_LF_INVERT");
			break;
			
			// ---------------------------------------------
		case OLY_HITZONE_LF_LARGE_PEQ:
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_LF_PEQ_ENABLE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_LF_PEQ_BAND1_TYPE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_LF_PEQ_BAND1_GAIN");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_LF_PEQ_BAND1_FREQ");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_LF_PEQ_BAND1_Q");
			m_dlgParent->m_dlgEQGraph.SetEQSelect("VOICING_LF_PEQ");
			break;
		case OLY_HITZONE_LF_DYNEQ1:		
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_LF_DYNEQ1_DETECTEN");  
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_LF_DYNEQ1_DETECTTYPE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_LF_DYNEQ1_DETECTGAIN");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_LF_DYNEQ1_DETECTFREQ");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_LF_DYNEQ1_DETECTQ");   
			break;
		case OLY_HITZONE_LF_DYNEQ2:		
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_LF_DYNEQ2_DETECTEN");  
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_LF_DYNEQ2_DETECTTYPE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_LF_DYNEQ2_DETECTGAIN");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_LF_DYNEQ2_DETECTFREQ");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_LF_DYNEQ2_DETECTQ");   
			break;
		case OLY_HITZONE_LF_GAIN:	
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_LF_FADER");     
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_LF_MUTE");      
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_LF_INVERT");    
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_LF_FADER_POST");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_LF_MUTE_POST"); 
			break;
		case OLY_HITZONE_LF_LIMITERA:	
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_LF_SOFTLIMIT_ENABLE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_LF_SOFTLIMIT_GAIN");  
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_LF_SOFTLIMIT_THRESH");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_LF_SOFTLIMIT_ATTACK");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_LF_SOFTLIMIT_RELEASE");
			break;
		case OLY_HITZONE_LF_LIMITERB:	
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_LF_KLIMITER_PEAKENABLE");  
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_LF_KLIMITER_PEAKATTACK");  
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_LF_KLIMITER_GAINRELEASE"); 
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_LF_KLIMITER_LOOKAHEAD");   
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_LF_KLIMITER_MAXPEAK");     
			break;		
		case OLY_HITZONE_LF_SMALL_PEQ:	
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_LF_PEQ_POST_ENABLE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_LF_PEQ_POST_BAND1_TYPE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_LF_PEQ_POST_BAND1_GAIN");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_LF_PEQ_POST_BAND1_FREQ");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_LF_PEQ_POST_BAND1_Q");
			m_dlgParent->m_dlgEQGraph.SetEQSelect("VOICING_LF_PEQ_POST");
			break;		
		case OLY_HITZONE_LF_DELAY:		
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_LF_DELAY_TIME");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"VOICING_AMP1_ROUTING");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"VOICING_AMP2_ROUTING");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"VOICING_AMP3_ROUTING");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"VOICING_AMP4_ROUTING");
			break;
			

		// ----------------------------------------

		case OLY_HITZONE_GROUP_CROSSOVER:
			//m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"VOICING_LF_DELAY_TIME");
			break;

		case OLY_HITZONE_GROUP_PEQ:
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"USER_GROUP_PEQ_ENABLE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"USER_GROUP_PEQ_BAND1_TYPE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"USER_GROUP_PEQ_BAND1_GAIN");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"USER_GROUP_PEQ_BAND1_FREQ");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"USER_GROUP_PEQ_BAND1_Q");
			m_dlgParent->m_dlgEQGraph.SetEQSelect("USER_GROUP_PEQ");
			break;

		case OLY_HITZONE_GROUP_DELAY:
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"USER_GROUP_DELAY_ENABLE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"USER_GROUP_DELAY_TIME");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"ePID_OLYspeaker1_USER_GROUP_SOFTLIMIT_ENABLE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"ePID_OLYspeaker1_USER_GROUP_SOFTLIMIT_GAIN");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"ePID_OLYspeaker1_USER_GROUP_SOFTLIMIT_THRESH");	
			break;


		case	OLY_HITZONE_GROUP_LIMITER:
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"USER_GROUP_SOFTLIMIT_ENABLE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"USER_GROUP_SOFTLIMIT_GAIN");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"USER_GROUP_SOFTLIMIT_THRESH");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"USER_GROUP_SOFTLIMIT_ATTACK");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"USER_GROUP_SOFTLIMIT_RELEASE");
			break;


		case OLY_HITZONE_GROUP_LEVEL:
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"USER_GROUP_FADER");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"USER_GROUP_MUTE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"USER_GROUP_PEQ_ENABLE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"USER_GROUP_PEQ_BAND1_TYPE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"USER_GROUP_PEQ_BAND1_GAIN");
			break;

		case OLY_HITZONE_GROUP_POLARITY:
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"USER_GROUP_FADER");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"USER_GROUP_MUTE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"USER_GROUP_PEQ_ENABLE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"USER_GROUP_PEQ_BAND1_TYPE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"USER_GROUP_PEQ_BAND1_GAIN");
			break;



		case	OLY_HITZONE_THROW_SM_PEQ:
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"THROW_THROW_PEQ_ENABLE");	
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"THROW_THROW_PEQ_BAND1_TYPE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"THROW_THROW_PEQ_BAND1_GAIN");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"THROW_THROW_PEQ_BAND1_FREQ");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(54,"THROW_THROW_PEQ_BAND1_Q");
			m_dlgParent->m_dlgEQGraph.SetEQSelect("SPEAKER_THROW_PEQ");
			break;

		case	OLY_HITZONE_SPEAKER_PROFILE_PEQ:
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"PROFILE_PROFILE_PEQ_ENABLE");			
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"PROFILE_PROFILE_PEQ_BAND1_TYPE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"PROFILE_PROFILE_PEQ_BAND1_GAIN");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"PROFILE_PROFILE_PEQ_BAND1_FREQ");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"PROFILE_PROFILE_PEQ_BAND1_Q");
			m_dlgParent->m_dlgEQGraph.SetEQSelect("SPEAKER_PROFILE_PEQ");
			break;

		case	OLY_HITZONE_AIR_CORRECTION:
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"AIRLOSS_EQ_ENABLE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"AIRLOSS_EQ_FREQ");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"AIRLOSS_EQ_FREQ");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"AIRLOSS_EQ_FREQ");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"AIRLOSS_EQ_FREQ");	
			//m_dlgParent->m_dlgEQGraph.SetEQSelect("AIRLOSS_PEQ");
			break;

		case	OLY_HITZONE_CARDIOID_CONTROL:	
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"USER_GROUP_FADER");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"USER_GROUP_MUTE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"USER_GROUP_PEQ_ENABLE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"USER_GROUP_PEQ_BAND1_TYPE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"USER_GROUP_PEQ_BAND1_GAIN");
			break;

		case	OLY_HITZONE_USER_GAIN:	
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"USER_SPEAKER_FADER");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"USER_SPEAKER_MUTE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"USER_SPEAKER_INVERT");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"USER_SPEAKER_PEQ_ENABLE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"USER_SPEAKER_PEQ_BAND1_TYPE");			
			break;
	
		case	OLY_HITZONE_USER_CROSSOVER:
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"XOVER_SPEAKER_XOVER_TYPE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"XOVER_SPEAKER_XOVER_ENABLE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"XOVER_SPEAKER_XOVER_FREQ");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"XOVER_SPEAKER_XOVER_SLOPE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"XOVER_SPEAKER_XOVER_RESPONSE");			
			//m_dlgParent->m_dlgEQGraph.SetEQSelect("USER_SPEAKER_XOVER_PEQ");
			break;


		case	OLY_HITZONE_USER_PEQ:		
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"USER_SPEAKER_PEQ_ENABLE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"USER_SPEAKER_PEQ_BAND1_TYPE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"USER_SPEAKER_PEQ_BAND1_GAIN");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"USER_SPEAKER_PEQ_BAND1_FREQ");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"USER_SPEAKER_PEQ_BAND1_Q");			
			m_dlgParent->m_dlgEQGraph.SetEQSelect("USER_SPEAKER_PEQ");
			break;


		case	OLY_HITZONE_USER_DELAY:	
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"USER_SPEAKER_DELAY_ENABLE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"USER_SPEAKER_DELAY_TIME");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"USER_GROUP_FADER");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"USER_GROUP_MUTE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"USER_GROUP_PEQ_ENABLE");			
			break;

		case	OLY_HITZONE_USER_POLARITY:
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"USER_SPEAKER_FADER");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"USER_SPEAKER_MUTE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"USER_SPEAKER_INVERT");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"USER_SPEAKER_PEQ_ENABLE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"USER_SPEAKER_PEQ_BAND1_TYPE");			
			break;			break;


		case OLY_HITZONE_INPUT_MODE:
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"DEVICE_INPUT_MODE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"DEVICE_LCD_BRIGHTNESS");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"DEVICE_FAN_THRESHOLD");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"DEVICE_FAN_HYSTERESIS");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"DEVICE_CS_I_CAL_FS_LF");	
			break;

		case OLY_HITZONE_INPUT_FUNCGEN:
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(1,"USER_INPUT_FUNCGEN_ENABLE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(2,"USER_INPUT_FUNCGEN_FADER");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(3,"USER_INPUT_FUNCGEN_TYPE");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(4,"USER_INPUT_FUNCGEN_FREQ");
			m_dlgParent->m_dlgParameterAdjust.SetComboSelectToParameter(5,"");			
			break;
		}
		return;
			
	}  
	else
		CDialog::OnLButtonDown(nFlags, point);
}

void CDSPSignalFlow::OnCbnSelchangeDspsignalflowSelect()
{
	// TODO: Add your control notification handler code here
	int nSel;
	CComboBox* pCombo;

	pCombo = (CComboBox*) GetDlgItem(IDC_DSPSIGNALFLOW_SELECT);
	if (pCombo)
	{
		nSel = pCombo->GetCurSel();
		SetImageSelect(nSel);
		//pCombo->SetCurSel(nSel);
		//OnCbnSelchangeEQSelect();
	}
}

BOOL CDSPSignalFlow::OnInitDialog()
{
	//CComboBox* pCombo;
	BOOL bResult = CDialog::OnInitDialog();
	m_nImage = 0;
	InitImageSelect();

	return bResult;
}



// -----------------------------------------------------------------------------
void CDSPSignalFlow::InitImageSelect()
{
	int i;
	CComboBox* pCombo;
	int nIndex;
	int nNum;

	pCombo = (CComboBox*) GetDlgItem(IDC_DSPSIGNALFLOW_SELECT);
	if (pCombo)
	{
		pCombo->ResetContent();
		nNum = sizeof(gDSPSignalFlowImages)/sizeof(tImageArray);
		for(i=0;i<nNum;i++)
		{
			nIndex =  pCombo->AddString(gDSPSignalFlowImages[i].pcName);
			pCombo->SetItemData(nIndex,i);
		}
		//pCombo->SetCurSel(0);
		SetImageSelect(0);
	}

}


void CDSPSignalFlow::SetImageSelect(int nSel)
{
	CComboBox* pCombo;
	CWnd* pWnd;
	int i, nNum;

	pCombo = (CComboBox*) GetDlgItem(IDC_DSPSIGNALFLOW_SELECT);
	if (pCombo)
	{
		pCombo->SetCurSel(nSel);
	}


	nNum = sizeof(gDSPSignalFlowImages)/sizeof(tImageArray);
	for(i=0;i<nNum;i++)
	{
		pWnd = GetDlgItem(gDSPSignalFlowImages[i].nIDC);
		if (pWnd)
		{
			pWnd->ShowWindow(i==nSel?SW_SHOW:SW_HIDE);
		}
	}
	
	m_nImage = nSel;

}