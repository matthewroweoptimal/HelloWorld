
// RUglyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RUgly.h"
#include "RUglyDlg.h"

#include "LOUD_defines.h"

#ifdef _DEBUG
//#include <vld.h>
#endif

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CRUglyDlg	*		pWindow;
// CAboutDlg dialog used for App About

const	UINT			wm_DataAvailable = RegisterWindowMessage( "DataAvailable" );
int gBaudRates[BAUDRATE_MAX] =
{
	CBR_4800   ,
	CBR_9600   ,
	CBR_14400  ,
	CBR_19200  ,
	CBR_38400  ,
	CBR_56000  ,
	CBR_57600  ,
	CBR_115200 ,
	CBR_128000 ,
	CBR_256000 ,
	921600
//	31250
};


class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

public:
	CString	m_strProductName;
	CString	m_strProductVersion;
	//CString	m_strProductDate;
	CString	m_strCompanyName;
	CString	m_strCopyright;


// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CAboutDlg::OnInitDialog()
{

	CDialog::OnInitDialog();

	SetDlgItemText(IDC_ABOUTPRODUCTNAME,m_strProductName);
	SetDlgItemText(IDC_ABOUTVERSION,m_strProductVersion);
	SetDlgItemText(IDC_ABOUTCOMPANY,m_strCompanyName);
	SetDlgItemText(IDC_ABOUTCOPYRIGHT,m_strCopyright);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CRUglyDlg dialog




CRUglyDlg::CRUglyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRUglyDlg::IDD, pParent)
{
	int i;

	m_hIcon = AfxGetApp()->LoadIcon(IDI_EAW);

	m_nProductType = ePRODUCT_OLYSPEAKER;

	m_nComType = COMTYPE_COM;
	m_bIsConnected = FALSE;
	pCPCommPort = NULL;
	bCommPortSemaphore = FALSE;

	pSocketCommInfo = NULL;

	m_ProductOLY.m_dlgParent = this;
	m_ProductOLY.SetCurrentStateToDefaults();
	m_ProductREDLINE.m_dlgParent = this;
	m_ProductREDLINE.SetCurrentStateToDefaults();


	m_nGetStatePos = 0;
	m_nGetStateRequestSize = 20;
	m_bGetStateStarted = false;


	m_nSendStatePos = 0;
	m_nSendStateRequestSize = 20;

	m_RedBrush.CreateSolidBrush(RGB(255,0,0));
	m_YellowBrush.CreateSolidBrush(RGB(255,255,0));
	m_BlueBrush.CreateSolidBrush(RGB(0,0,255));
	m_GreenBrush.CreateSolidBrush(RGB(0,255,0));

	m_uiMandolinVersion = 0;
	m_uiK64Version = 0;
	m_nMandolinMaxMsgSize = 2*1024;

	//m_uiCRCPC = 0;
	//m_uiCRCGovernor = 0;
	MandolinConnectStateSetState(MANDOLIN_CONNECT_NONE);
	m_bMandolinConnectStateStart = false;

}

CRUglyDlg::~CRUglyDlg()
{

	if (pCPCommPort != NULL)
	{
		pCPCommPort->bIgnoreRxData = TRUE;
		CloseConnection(pCPCommPort);
		DestroyCPInfo(pCPCommPort);
	}
	if (pSocketCommInfo != NULL)
	{
		CloseSocketComm();
		pSocketCommInfo->bIgnoreRxData = TRUE;
		DestroySocketCommInfo(pSocketCommInfo);
	}

	DeleteObject(m_RedBrush);
	DeleteObject(m_YellowBrush);
	DeleteObject(m_BlueBrush);
	DeleteObject(m_GreenBrush);

}

void CRUglyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT_OUTPUT, m_OutputTextBox);
	DDX_Control(pDX, IDC_RICHEDIT_INPUT, m_InputTextBox);
	DDX_Control(pDX, IDC_SCROLLBAR1, m_ScrollBar);
}

BEGIN_MESSAGE_MAP(CRUglyDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_VSCROLL()
	ON_WM_QUERYDRAGICON()
    ON_REGISTERED_MESSAGE( wm_DataAvailable, OnRxData )
	ON_COMMAND(IDM_CONNECTION_COMMPORT, &CRUglyDlg::OnCommSettings)
	ON_COMMAND(IDM_CONNECTION_SOCKET, &CRUglyDlg::OnSocketSettings)
	ON_COMMAND(IDM_HELP_ABOUT, &CRUglyDlg::OnHelpAbout)

 	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_PING, &CRUglyDlg::OnBnClickedPing)
	ON_BN_CLICKED(IDC_PARAMETERADJUST, &CRUglyDlg::OnBnClickedParameteradjust)
	ON_BN_CLICKED(IDC_DISPLAYMESSAGEFORMAT_RAW, &CRUglyDlg::OnBnClickedDisplaymessageformatRaw)
	ON_BN_CLICKED(IDC_DISPLAYMESSAGEFORMAT_INTERPRET, &CRUglyDlg::OnBnClickedDisplaymessageformatInterpret)
	ON_BN_CLICKED(IDC_FREEZE, &CRUglyDlg::OnBnClickedFreeze)
	ON_BN_CLICKED(IDC_SETTINGSFILE_SELECT, &CRUglyDlg::OnBnClickedSettingsfileSelect)
	ON_BN_CLICKED(IDC_SETTINGSFILE_SEND, &CRUglyDlg::OnBnClickedSettingsfileSend)
	ON_BN_CLICKED(IDC_SETTINGSFILE_STOP, &CRUglyDlg::OnBnClickedSettingsfileStop)
	ON_BN_CLICKED(IDC_SAVESTATE, &CRUglyDlg::OnBnClickedSavestate)
	ON_BN_CLICKED(IDC_SETTINGSFILE_LOAD, &CRUglyDlg::OnBnClickedSettingsfileLoad)
	ON_BN_CLICKED(IDC_PRODUCTTYPE_OLY, &CRUglyDlg::OnBnClickedProducttypeOly)
	ON_BN_CLICKED(IDC_PRODUCTTYPE_REDLINE, &CRUglyDlg::OnBnClickedProducttypeRedline)
	ON_BN_CLICKED(IDC_CLEARDISPLAY, &CRUglyDlg::OnBnClickedCleardisplay)
	ON_BN_CLICKED(IDC_SAVESTATEFIR, &CRUglyDlg::OnBnClickedSavestatefir)
	ON_BN_CLICKED(IDC_GETSTATEFROMDEVICE, &CRUglyDlg::OnBnClickedGetstatefromdevice)
	ON_BN_CLICKED(IDC_GETFIRSTATEFROMDEVICE, &CRUglyDlg::OnBnClickedGetFIRstatefromdevice)
	ON_BN_CLICKED(IDC_SNAPSHOTSTORE, &CRUglyDlg::OnBnClickedSnapshotstore)
	ON_BN_CLICKED(IDC_SNAPSHOTLOAD, &CRUglyDlg::OnBnClickedSnapshotload)
	ON_BN_CLICKED(IDC_ZIMPEDANCEDLG, &CRUglyDlg::OnBnClickedZimpedancedlg)
	ON_BN_CLICKED(IDC_CREATERANDOMSNAPSHOT, &CRUglyDlg::OnBnClickedCreaterandomsnapshot)
	ON_BN_CLICKED(IDC_SENDSTATETODEVICE, &CRUglyDlg::OnBnClickedSendstatetodevice)
	ON_BN_CLICKED(IDC_SHOWSTATE2, &CRUglyDlg::OnBnClickedShowstate2)
	ON_BN_CLICKED(IDC_SetStateToDefaults, &CRUglyDlg::OnBnClickedSetstatetodefaults)
	ON_BN_CLICKED(IDC_LEVELADUST, &CRUglyDlg::OnBnClickedLeveladust)
	ON_BN_CLICKED(IDC_METERS, &CRUglyDlg::OnBnClickedMeters)
	ON_BN_CLICKED(IDC_DSPSIGNALFLOW, &CRUglyDlg::OnBnClickedDspsignalflow)
	ON_BN_CLICKED(IDC_EQGRAPH, &CRUglyDlg::OnBnClickedEqgraph)
	ON_BN_CLICKED(IDC_MANDOLIN_CONNNECT, &CRUglyDlg::OnBnClickedMandolinConnnect)
	ON_BN_CLICKED(IDC_ACTIVEPRESET, &CRUglyDlg::OnBnClickedActivepreset)
	ON_BN_CLICKED(IDC_MANDOLIN_DISCONNNECT, &CRUglyDlg::OnBnClickedMandolinDisconnnect)
	ON_BN_CLICKED(IDC_PARAMETERTEST, &CRUglyDlg::OnBnClickedParametertest)
	ON_BN_CLICKED(IDC_SYNC_REQUEST, &CRUglyDlg::OnBnClickedSyncRequest)
	ON_BN_CLICKED(IDC_GET_HW_INFO, &CRUglyDlg::OnBnClickedGetHwInfo)
	ON_BN_CLICKED(IDC_GET_SW_INFO, &CRUglyDlg::OnBnClickedGetSwInfo)
	ON_BN_CLICKED(IDC_OLYIMPEDANCEDLG, &CRUglyDlg::OnBnClickedOlyimpedancedlg)
	ON_BN_CLICKED(IDC_CREATEPRESETHEADERS, &CRUglyDlg::OnBnClickedCreatepresetheaders)
	ON_BN_CLICKED(IDC_TESTMANU, &CRUglyDlg::OnBnClickedTestmanu)
	ON_BN_CLICKED(IDC_STORESUSEREDIT, &CRUglyDlg::OnBnClickedStoresuseredit)
	ON_BN_CLICKED(IDC_CANCELSUSEREDIT, &CRUglyDlg::OnBnClickedCancelsuseredit)
	ON_BN_CLICKED(IDC_IDENTIFY_ON, &CRUglyDlg::OnBnClickedIdentifyOn)
	ON_BN_CLICKED(IDC_IDENTIFY_OFF, &CRUglyDlg::OnBnClickedIdentifyOff)
	ON_BN_CLICKED(IDC_SOCKET_CONNECT, &CRUglyDlg::OnBnClickedSocketConnect)
	ON_BN_CLICKED(IDC_SOCKET_DISCONNECT, &CRUglyDlg::OnBnClickedSocketDisconnect)
	ON_BN_CLICKED(IDC_SELFTESTMODE, &CRUglyDlg::OnBnClickedSelftestmode)
	ON_BN_CLICKED(IDC_GETIPADDRESS, &CRUglyDlg::OnBnClickedGetipaddress)
	ON_BN_CLICKED(IDC_CREATERANDOMFIR, &CRUglyDlg::OnBnClickedCreaterandomfir)
	ON_BN_CLICKED(IDC_SENDSTATETODEVICESTOP, &CRUglyDlg::OnBnClickedSendstatetodevicestop)
	ON_BN_CLICKED(IDC_SENDSTATETODEVICEFIR, &CRUglyDlg::OnBnClickedSendstatetodevicefir)
	ON_BN_CLICKED(IDC_SetStateToDefaultsFIR, &CRUglyDlg::OnBnClickedSetstatetodefaultsfir)
	ON_BN_CLICKED(IDC_SHOWSTATEFIR, &CRUglyDlg::OnBnClickedShowstatefir)
	ON_BN_CLICKED(IDC_COMPUTESTATECRC, &CRUglyDlg::OnBnClickedComputestatecrc)
	ON_BN_CLICKED(IDC_COMPUTEFIRCRC, &CRUglyDlg::OnBnClickedComputefircrc)
	ON_BN_CLICKED(IDC_SCANNETWORK, &CRUglyDlg::OnBnClickedScannetwork)
	ON_BN_CLICKED(IDC_STARTAUTOOPTIMIZE, &CRUglyDlg::OnBnClickedStartautooptimize)
	ON_BN_CLICKED(IDC_RESETAUTOOPTIMIZE, &CRUglyDlg::OnBnClickedResetautooptimize)
	ON_BN_CLICKED(IDC_SEND_FW_FILE, &CRUglyDlg::OnBnClickedSendFwFile)
	ON_BN_CLICKED(IDC_SELECT_FW_FILE, &CRUglyDlg::OnBnClickedSelectFwFile)
END_MESSAGE_MAP()


// CRUglyDlg message handlers

BOOL CRUglyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	int i;

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	pWindow = this;

	
	srand( (unsigned)time( NULL ) );

	// history
	//	June 2, 2015	1.02	redline and oly tables
	//	June 3, 2015	1.03	get state, fir edits
	//	June 4, 2015	1.04	parameter adjust screen improved
	//	June 4, 2015	1.05	store and load, tried to put in sliders
	//	June 5, 2015	1.06	reads the meters for impedance
	//	June 8, 2015	1.07	meter graphs and logging.
	//	June 9, 2015	1.08	more meters, get state, send state, defaults, random
	//	June 10, 2015	1.09	Z impedance graphs, added Z average
	//	June 12, 2015	1.10	Z calcs in running meter output
	//	June 16, 2015	1.11	shows Z thresholds depending on model
	//	June 17, 2015	1.12	updated Z thresholds
	//	June 17, 2015	1.13	updated Z thresholds
	//	June 17, 2015	1.14	updated Z thresholds
	//	June 18, 2015	1.15	added levels screen, added Z thresholds.
	//	June 18, 2015	1.16	added the OLY tables, generalized the products
	//	June 26, 2015	1.17	added the OLY tables, generalized the products
	//	Mar 17, 2016	2.0.004	fixed meter subscription, added get timeout status
	//	Mar 18, 2016	2.0.005	fixed meter poll, get neighbour hood a numparms=0 is ok, added Mandolin Connect
	//	Mar 21, 2016	2.0.006	added Mandolin Connect and working around for the set param with no acks
	//	Mar 22, 2016	2.0.007	added multiple instance support, limited subscriptios to #1 and #2
	//	Mar 23, 2016	2.0.008	added Parameter Test
	//	Mar 30, 2016	2.0.009	Meter subscriptions are now showing, show versions and model
	//	Mar 30, 2016	2.0.010	update the xml files
	//	Apr	4,  2016	2.0.011	hotspot for user Xover
	//	Apr	12,  2016	2.0.012	parameter test logs, oly status dlg
	//	Apr	13,  2016	2.0.013	added create preset header
	//	Apr	13,  2016	2.0.014	new pid tables, added 4th channel to status, renamed LF/MF/HF to ch1/2/3
	//	Apr	21,  2016	2.0.015	added test dlg
	//	Apr	29,  2016	2.0.016	added the Test Manu
	//	May	 3,  2016	2.0.017	Test Manu changes, minor changes on create preset header
	//	May	 3,  2016	2.0.018	Set Application string sends back an Ack
	//	May	 9,  2016	2.0.019	fixed save state (instance numbering) added store/cancel user edits
	//	May	10,  2016	2.0.020	added the user save buttons, added the eaw speaker brand/model decode
	//	May	10,  2016	2.0.021	added the status parameters to edit buffer
	//	May	11,  2016	2.0.022	added the socket connect on the top screen and does not try to connect automatically on startup
	//	May	20,  2016	2.0.023	updated to the newest parameter maps (status angle is an integer)
	//	May	27,  2016	2.0.024	added self test modes and if oly shifts the store index
	//	Jun  3,  2016	2.0.025	added all select in the create preset header
	//	Jun  7,  2016	2.0.026	fixed FIR send file
	//	Jun  8,  2016	2.0.027	fixed FIR send file
	//	Jun  9,  2016	2.0.028	Added an xml check to the preset header create, added random FIR value create
	//	Jun  9,  2016	2.0.029	Added an xml check to the preset header create, added random FIR value create
	//	Jun  10,  2016	2.0.030	mikeb: added dynamic EQ meters
	//	Jun  17,  2016	2.0.030b added CRC checks and added more test message (can set the Mac address now)
	//	Jun  17,  2016	2.0.030d added the scan network
	//	Jun  17,  2016	2.0.031	mikeb: added segment for airloss filter parameters
	//	Jun  20,  2016	2.0.032 fixed the scan network, included airloss filters
	//	Jun  24,  2016	2.0.033 added auto and reset optimize
	//	Jun  30,  2016	2.0.034 does not open comm port
	//	Jun  30,  2016	2.0.035 saves scan network info, added some debug printing for new type Mike B added
	//	Jul  22,  2016  2.0.036 dsp error message moved from MANDOLIN_MSG_ERROR to MANDOLIN_MSG_TEST with sub id TEST_DSP_ERROR
	//	Aug  10,  2016  2.0.037 rebuilt for noise gate and lockout (device parameter)
	// 	May  31,  2017  2.0.038 added support for RSX218
	//	Nov	16, 2017 2.0.0.39	added support for RSX212L and RSX18F

	this->GetFileVersionInfo();
	m_strProductName = "RUgly";
	m_strProductVersion = "Version 2.0.039, Nov 16, 2017";
	//m_strProductDate = "";
	m_strCompanyName = "EAW";
	m_strCopyright = "2017";


	m_pcFirmwareVersion[0] = 0;

	//m_RedBrush.CreateSolidBrush(RGB(255,0,0));
	//m_YellowBrush.CreateSolidBrush(RGB(255,255,0));
	//m_BlueBrush.CreateSolidBrush(RGB(0,0,255));
	//m_GreenBrush.CreateSolidBrush(RGB(0,255,0));


	// -----------------------------------
	// input/output
	// -----------------------------------
	m_bFreezeDisplay = false;
	m_nInputTextBoxFilter = INPUTTEXTBOXFILTER_INTERPRET;

	OnShowInputFilter();
	OnShowFreeze();

		//-----------------------------------------------------
	//  Set up look of Input/Ouput textboxes
	//-----------------------------------------------------
	m_OutputTextBox.SetScrollRange(SB_VERT,0,100,true);

	CHARFORMAT CharFormat;
	CharFormat.cbSize = sizeof(CharFormat);
    CharFormat.dwMask = CFM_COLOR | CFM_SIZE | CFM_CHARSET;
   //CharFormat.crTextColor;
    CharFormat.dwEffects = CFE_AUTOCOLOR;
    CharFormat.yHeight = 170;
    ::lstrcpy(CharFormat.szFaceName, "Arial");

	m_InputTextBox.SetDefaultCharFormat(CharFormat);
	m_OutputTextBox.SetDefaultCharFormat(CharFormat);


	if ((pCPCommPort = CreateCPInfo()) == NULL) 				// no memory left for data structure
		{
		MessageBox ( "Out of memory", "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL );
	   	return FALSE;
	   	}
	if ((pSocketCommInfo = CreateSocketCommInfo()) == NULL) 				// no memory left for data structure
		{
		MessageBox ( "Out of memory", "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL );
	   	return FALSE;
	   	}

	m_bIsConnected = false;
	if (m_nComType == COMTYPE_COM)
	{
		//m_bIsConnected = InitComPort(m_nConnectionPortNumber,m_nConnectionBaud,m_nConnectionParity);
	} else if (m_nComType == COMTYPE_SOCKET) {
		// don't connect on startup
		//m_bIsConnected = InitSocketComm(m_nSocketIPAddress,m_nSocketPort,m_nSocketType);
	}

	ShowIPAddress();

	for(i=0;i<2;i++)
	{
		m_nParameterListId[i] = -1;
		m_nParameterListNum[i] = 0;
		m_nParameterListTarget[i] = 0;
	}

	// -----------------------------------
	// tick counter
	// -----------------------------------
	SetTimer(TICKCOUNT_TIMERID,200,NULL);

	// -----------------------------------
	// Mandolin init
	// -----------------------------------
	m_nTxSequence = 1;
	MANDOLIN_FIFO_init(&m_bfpRxFifo,MANDOLIN_FIFO_BYTES, m_uiRxFifoBuffer);


	m_MandolinTextSummary.m_nProductType = m_nProductType;
	m_MandolinTextSummary.m_dlgParent = this;


	m_dlgParameterAdjust.m_nProductType = m_nProductType;
	m_dlgParameterAdjust.m_dlgParent = this;
	//m_dlgParameterAdjust.m_nProductType = m_nProductType;
	m_dlgParameterAdjust.Create( IDD_PARAMETERADJUST );

	m_dlgLevelAdjust.m_dlgParent = this;
	m_dlgLevelAdjust.Create( IDD_LEVELADJUST );
	
	m_dlgActivePreset.m_dlgParent = this;
	m_dlgActivePreset.Create( IDD_ACTIVEPRESET );
	
	m_dlgParameterTest.m_dlgParent = this;
	m_dlgParameterTest.Create( IDD_PARAMETERTEST );

	SetDlgItemText(IDC_SETTINGSFILE_FILENAME,m_pcSettingsFileName); 
	m_SettingsFile.m_dlgParent = this;
	 
	m_firmwareFile.m_dlgParent = this;

	m_dlgMeters.m_dlgParent = this;
	m_dlgMeters.Create( IDD_METERDIALOG );

	m_dlgDSPSignalFlow.m_dlgParent = this;
	m_dlgDSPSignalFlow.Create( IDD_DSPSIGNALFLOW );
	m_dlgDSPSignalFlow.SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);

	m_dlgEQGraph.m_dlgParent = this;
	m_dlgEQGraph.Create( IDD_EQGRAPH );

	m_dlgZImpedance.m_dlgParent = this;
	m_dlgZImpedance.Create( IDD_ZIMPEDANCE );

	m_dlgOlyImpedance.m_dlgParent = this;
	m_dlgOlyImpedance.Create( IDD_OLYIMPEDANCE );


	m_dlgCreatePresetHeaders.m_dlgParent = this;
	m_dlgCreatePresetHeaders.Create( IDD_CREATEPRESETHEADERS );

	m_dlgTestManu.m_dlgParent = this;
	m_dlgTestManu.Create( IDD_TESTMANU );


	m_dlgScanNetwork.m_dlgParent = this;
	m_dlgScanNetwork.Create( IDD_SCANNETWORK );

	m_dlgScanNetwork.SetIPAddress(m_nScanNetworkIPAddress,m_nScanNetworkIPEnd);



	SetProducttype(ePRODUCT_OLYSPEAKER);
	ShowProducttype();	
	
	SetDlgItemText(IDC_SNAPSHOTNUMBER,"1");

	//SetScrollRange(SB_HORZ,0,255);
	//SetScrollRange(SB_VERT,0,255);

	SCROLLINFO ScrollInfo;
	ScrollInfo.cbSize = sizeof(ScrollInfo);     // size of this structure
	ScrollInfo.fMask = SIF_ALL;                 // parameters to set
	ScrollInfo.nMin = 0;                        // minimum scrolling position
	ScrollInfo.nMax = 255;                      // maximum scrolling position
	ScrollInfo.nPage = 1;                      // the page size of the scroll box
	ScrollInfo.nPos = 255;                       // initial position of the scroll box
	ScrollInfo.nTrackPos = 0;                   // immediate position of a scroll box that the user is dragging
	m_ScrollBar.SetScrollInfo(&ScrollInfo,false);
	

	int nSelftestMode[]=  {
		eTEST_MODE_DISABLED,
		eTEST_MODE_ENABLED,
		eTEST_MODE_MUTE_FROM_SOLO,
		eTEST_MODE_SEQUENCE,
		eTEST_MODE_CYCLE,
	};
	char* pcSelftestMode[]=  {
		"DISABLED",
		"ENABLED",
		"MUTE_FROM_SOLO",
		"SEQUENCE",
		"CYCLE",
	};

	CComboBox* pCombo;
	int nSel;
	pCombo = (CComboBox*) GetDlgItem(IDC_SELFTESTMODE_SELECT);
	if (pCombo)
	{
		pCombo->ResetContent();
		for(i=0;i<sizeof(nSelftestMode)/sizeof(int);i++)
		{
			//sprintf(pcString,"%d",i);
			nSel =  pCombo->AddString(pcSelftestMode[i]);
			pCombo->SetItemData(nSel,nSelftestMode[i]);
		}
		pCombo->SetCurSel(0);
	}
	ShowSofwareInfo();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRUglyDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
		if ((nSBCode == SB_ENDSCROLL) || (nSBCode == SB_THUMBPOSITION))
		return;

	if (pScrollBar == &m_ScrollBar)
	{
		SCROLLINFO ScrollInfo;
		pScrollBar->GetScrollInfo(&ScrollInfo);  // get information about the scroll

		if (ScrollInfo.nMax != 255)
		{
			ScrollInfo.cbSize = sizeof(ScrollInfo);     // size of this structure
			ScrollInfo.fMask = SIF_ALL;                 // parameters to set
			ScrollInfo.nMin = 0;                        // minimum scrolling position
			ScrollInfo.nMax = 255;                      // maximum scrolling position
			ScrollInfo.nPage = 1;                      // the page size of the scroll box
			ScrollInfo.nPos = 255;                       // initial position of the scroll box
			ScrollInfo.nTrackPos = 0;                   // immediate position of a scroll box that the user is dragging
			m_ScrollBar.SetScrollInfo(&ScrollInfo,false);

		}

		switch(nSBCode)
		{
		case SB_BOTTOM:         //Scrolls to the lower right.
			break;

		case SB_ENDSCROLL:      //Ends scroll.
			break;

		case SB_LINEDOWN:       //Scrolls down dependant on number type.
				pScrollBar->SetScrollPos(pScrollBar->GetScrollPos() + 1);
			break;

		case SB_LINEUP:         //Scrolls one line down dependant on number type.
			pScrollBar->SetScrollPos(pScrollBar->GetScrollPos() - 1);
			break;

		case SB_PAGEDOWN:       //Scrolls one page down dependant on number type.

			pScrollBar->SetScrollPos(pScrollBar->GetScrollPos() + 40);
			break;

		case SB_PAGEUP:         //Scrolls one page up dependant on number type.
			pScrollBar->SetScrollPos(pScrollBar->GetScrollPos() - 40);
			break;

		case SB_THUMBPOSITION:  //The user has dragged the scroll box (thumb) and released the mouse button. The nPos parameter indicates the position of the scroll box at the end of the drag operation.
			break;

		case SB_THUMBTRACK:     //The user is dragging the scroll box. This message is sent repeatedly until the user releases the mouse button. The nPos parameter indicates the position that the scroll box has been dragged to.
			pScrollBar->SetScrollPos(nPos);
			break;

		case SB_TOP:            //Scrolls to the upper left.
			break;
	  }



	}


}

void CRUglyDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRUglyDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRUglyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




int nRxDataMultiThread = 0;
int nRxData = 0;

int FAR PASCAL	OnRxDataMultiThreaded( int nMessageLength, char *theData, void *callbackObject)
{
	// in the callback comm port routine
	nRxDataMultiThread += nMessageLength;

//	if ( pWindow->m_bWaitingForMessage )
//		pWindow->OnRxData(nMessageLength,(LONG) theData);
//	else
//	{
		DWORD* lpData;
		lpData = (DWORD *) GlobalAlloc(GMEM_FIXED, nMessageLength);		// IMPORTANT : notify window must free this data!
		if (lpData != NULL)
		{
			memcpy (lpData, theData, nMessageLength );
			// NOTE: if using windows messaging, must pass pCPInfo->callbackObject somehow...(if using that method of processing)
			// you could always have a different windows message id for each port though.
			PostMessage(pWindow->m_hWnd,wm_DataAvailable,nMessageLength,(LONG) lpData);
		}

//	}

	return 0;
}


char pcRxDebugBuffer[1024*4];
int  nRxDebugBufferPointer = 0;

LRESULT CRUglyDlg::OnRxData( WPARAM wParam, LPARAM lParam)

{
	unsigned int i;
	char	*pcData = (char *) lParam;
	char	*szTemp;
	char	newChar[256];
	bool	bNewMessage;
	int nLength;
	char theBuff[100];
		char *c;
	

	CString	theText;

	nRxData += wParam;
	nLength = wParam;
//	nRxByteCount	+= wParam;

	for(i=0;i<wParam;i++)
	{
		pcRxDebugBuffer[nRxDebugBufferPointer++] = pcData[i];

		if ((pcData[i] & 0x0ff) == 0x0e5)
		{
			nRxDebugBufferPointer += 0;
		}
		if ( nRxDebugBufferPointer >= sizeof ( pcRxDebugBuffer ))
			nRxDebugBufferPointer = 0;
	}



	if ((!m_bFreezeDisplay) && (m_nInputTextBoxFilter == INPUTTEXTBOXFILTER_RAW))
	{
	//	theText = m_OutputTextBox.GetText();
		//if ((*theMessage & 0x0ff) == 0x0ab)
		//{
		c = pcData;
		for(i=0;i<nLength;i++)
		{
			if ((i != 0) && ((i%4)== 0))
			{
				if (i%16)
				{
					theText += " ";
				}
				else
				{
					theText += "\n";
				}
			}
			sprintf(theBuff,"%2.2X ",(int)(*c++ & 0x0ff));
			theText += theBuff;

		}
		UpdateInputText(theText,true);
	}
	
	if ( MANDOLIN_FIFO_is_full(&m_bfpRxFifo)) 
	{
		return 0;
	}

	MANDOLIN_FIFO_enqueue_bytes(&m_bfpRxFifo,(mandolin_byte_type*)pcData, wParam);
	while (MANDOLIN_handle_Process(&bmwsRxMsgWrapper, &m_bfpRxFifo));		// do more than 1 message
	//m_MandolinComm.RxMandolinBytes(pcData,nLength);

//	szTemp = (char *) malloc ( 4* wParam + 40 );
//
//	//KillTimer( MSGTIMEOUT_TIMERID );					// reset time out
//	//SetTimer( MSGTIMEOUT_TIMERID,  nMsgTimeOutRate, NULL);
//
//	szTemp[0] = 0;
//
//	if (bLogFileOpen)
//	{
//		cfCaptureRaw.Write((void*) pcData, wParam);
//
//	}
//
//	for (i=0 ; i< wParam; i++)
//	{
//		bNewMessage = (((pcData[i]&0x0ff) == 0x0a5) &&  (m_nRxBytesToGo == 0));
//
//		if (m_AsciiMode)
//		{
//			if ((m_nMessagePos%4)== 0)
//			{
//				if (m_nMessagePos%16)
//				{
//					strcat(szTemp," ");
//				}
//				else
//				{
//					strcat(szTemp,"\n");
//				}
//			}
//
//			sprintf (newChar,"%c ", (unsigned char) pcData[i]);
//			strcat(szTemp,newChar);
//		}
//		else
//		{
//			if ((m_nMessagePos%4)== 0)
//			{
//				if (m_nMessagePos%16)
//				{
//					strcat(szTemp," ");
//				}
//				else
//				{
//					strcat(szTemp,"\n");
//				}
//			}
//
//
//			sprintf (newChar,"%2.2X ",(unsigned char) pcData[i]);
//			strcat(szTemp,newChar); //sprintf (szTemp,"%s%s",szTemp, newChar);
////			sprintf (szTemp,"%s%s%s",szTemp, ( bNewMessage ? "\n" : " ") , newChar);
//		}
//
//
//
//		if (m_nRxBytesToGo ==0)
//		{
//			if (m_bWaitForLength == true)
//			{
//				if (m_nMessagePos < MAX_MESSAGE_SIZE)
// 					m_cMessage[m_nMessagePos++] = pcData[i];		// add to message
//				m_bWaitForLength = false;
//				m_nRxBytesToGo = (pcData[i]&0x0FF) * 4 + (BNHEADER_SIZE-2);		// *4, for words, offset for header
//			}
//			else if ( (pcData[i]&0x0FF) == 0x0a5)			// sync received
//			{
//				m_bWaitForLength = true;		// waiting for length
//				m_nMessagePos =0;
//				if (m_nMessagePos < MAX_MESSAGE_SIZE)
// 					m_cMessage[m_nMessagePos++] = pcData[i];		// add to message
//				m_nWaitingSyncPos = 0;	// got a sync
//				MessageReceivingLED(true);
//
//			}
//			else
//			{
//
//				if (m_nWaitingSyncPos < MAX_WAITINGFORSYNCMESSAGE_SIZE)
//				{
//					m_cWaitingSyncMessage[m_nWaitingSyncPos++] = pcData[i];		// waiting for sync
//				}
//
//			}
//		}
//		else
//		{
//			// m_nRxBytesToGo >= 0
//
//			if (m_nMessagePos < MAX_MESSAGE_SIZE)
// 				m_cMessage[m_nMessagePos++] = pcData[i];		// add to message
//
//			m_nRxBytesToGo--;
//			if (m_nRxBytesToGo == 0)		// finnished
//			{
//				// process new message?
//				m_bMessageReceived = true;
//				m_bExpectingResponse = false;
//				KillTimer( MSGTIMEOUT_TIMERID );
//				//KillTimer(PARAMEDITTEST_TIMERID);
//
//				// Make copy of message for ProcessMessage call. I have seen this
//				// data disappear at inadvertant moments...
//				UInt uMsgLength = (m_cMessage[1]  & 0x0ff)*4+BNHEADER_SIZE;
//				char *pMsgCpy = new char[uMsgLength];
//				memcpy(pMsgCpy, m_cMessage, uMsgLength);
//				ProcessMessage(pMsgCpy, (pMsgCpy[1]  & 0x0ff)*4+BNHEADER_SIZE);
//				delete []pMsgCpy;
//
//				MessageReceivingLED(false);
//				m_nMessagePos = 0;
//			}
//		}
//		if (m_nMessagePos >= MAX_MESSAGE_SIZE)
//			m_nMessagePos = MAX_MESSAGE_SIZE-1;
//		if (m_nWaitingSyncPos >= MAX_WAITINGFORSYNCMESSAGE_SIZE)
//			m_nWaitingSyncPos = MAX_WAITINGFORSYNCMESSAGE_SIZE-1;
//
//		if (bLogFileOpen)
//		{
//			nLogFileByteCount++;
//			if (!m_bLogFileWordPack)
//			{
//				if (bNewMessage)
//					cfCapture.Write("\n",1);
//
//				cfCapture.Write(newChar,strlen(newChar));
//			}
//			else
//			{
//				nLogFileWordPackVal += ( (pcData[i] & 0x0ff) << ( 8*nLogFileWordPackCount) );
//				nLogFileWordPackCount++;
//				if (nLogFileWordPackCount >= 4)
//				{
//					char buff[15];
//					sprintf(buff,"0x%8.8X\n",nLogFileWordPackVal);
//					nLogFileWordPackVal = 0;
//					nLogFileWordPackCount = 0;
//					cfCapture.Write(buff,strlen(buff));
//				}
//			}
//
//		}
//
//	}
//
//	// debug
//	if ((strlen(m_cInputText)+strlen(szTemp)) < 2047)
//		strcat(m_cInputText,szTemp);
//
//	m_strInputText += szTemp;
//
//	if (m_strInputText.GetLength() > 2048)
//	{
//		m_strInputText.Delete(0,300);
//	}
//	// end of debug
//
//	if (!m_FreezeDisplay)
//	{
//		CString theText;
//		GetDlgItemText(IDC_RICHEDIT_INPUT, theText);
//		int theLength = strlen(szTemp);
//		int theCurrentLength =  theText.GetLength();
//
//		if ( theCurrentLength > MAX_RX_DISPLAY )
//		{
//			if (theLength > theCurrentLength)
//				theLength = theCurrentLength;
//			theText.Delete(0, theLength);
//		}
//		theText += szTemp;
//		SetDlgItemText(IDC_RICHEDIT_INPUT, theText);
//		int nLine = m_InputTextBox.GetLineCount();
//		long lastChar = theText.GetLength();
//		m_InputTextBox.SetScrollPos(SB_VERT,nLine,true);
//		m_InputTextBox.SetSel(lastChar,lastChar);
//		m_InputTextBox.HideSelection( false, true);
//
//		if (nLine > 21 )			// number of lines
//		{
////			myTextBox.ScrollWindowEx(0,-30, NULL, NULL, NULL, NULL,  SW_INVALIDATE | SW_ERASE );
//		}
//	}
//
//
//	free(szTemp);
//

	if (lParam != NULL)
		GlobalFree((void *) lParam);			//MUST ALWAYS free data after receipt

	//if (bLogFileOpen)
	//{
	//	//UpdateData(false);	// to update the Log file count
	//	char buff[20];
	//	sprintf(buff,"%d",nLogFileByteCount);
	//	SetDlgItemText(IDC_LOGFILEINBYTECOUNT,buff);


	//}

	return 0;
}


void CRUglyDlg::OnTimer(UINT nIDEvent)
{

	char	theMessage[4096];
//	int		i;
//	float	fdbLevel;
	int		nLength;
	mandolin_message msgDownload;
	char	pPayloadValues[(1024+30)*4];
	//char	a1,a2,a3,a4;
	char pcBuffer[200];



    tick_count = (clock()*1000) / CLOCKS_PER_SEC;

	KillTimer (nIDEvent);

	switch (nIDEvent)
	{
	case  TICKCOUNT_TIMERID:

//		tick_count = (clock()*1000) / CLOCKS_PER_SEC);
		SetTimer(TICKCOUNT_TIMERID,200,NULL);
		break;

	case KEEPALIVE_TIMERID:
		OnBnClickedPing();
		SetTimer(KEEPALIVE_TIMERID, 1000*3, NULL);
		break;
	

	case METER_TIMERID:
			if (m_dlgMeters.m_bInProgress)
			{
				m_dlgMeters.OnTimer(nIDEvent);
			}
			break;
	
	case PARAMETERTEST_TIMERID:
		m_dlgParameterTest.OnTimer(nIDEvent);
		break;

	case EQGRAPH_COEFFSDIRTY_TIMERID:
		m_dlgEQGraph.OnTimer(nIDEvent);
		break;
	
	case PARAMETERADJUST_GETPARAMETER_TIMERID:
		m_dlgParameterAdjust.OnTimer(nIDEvent);
		break;

	//		UpdateData(TRUE);
	//		Getmeters();

	//		m_nMeterTime = GetMeterRate();
	//		if (m_nMeterTime>0)
	//		{
	//			SetDlgItemText(IDC_GETMETERS,"Stop Meters");
	//			SetTimer(METER_TIMERID,m_nMeterTime,NULL);
	//		}
	//		else
	//		{
	//			SetDlgItemText(IDC_GETMETERS,"Get Meters");
	//		}
	//		break;

		//case ECHOTEST_TIMERID:
		//		if (m_dlgEchoTest.m_bEchoTestStarted)
		//		{
		//			m_dlgEchoTest.m_nEchoTestTotalTimeouts++;
		//		}
		//		m_dlgEchoTest.m_nEchoTestTotalMessages++;
		//		m_dlgEchoTest.UpdateResults();
		//		m_dlgEchoTest.SendEchoTestMessage();

	
		//	break;


		case MSGTIMEOUT_TIMERID:

			m_bExpectingResponse = false;
			SetDlgItemText(IDC_RESPONSESTATUS,"Incomplete Message received (timed out)");

			if (m_SettingsFile.m_bSettingsFileInProgress)
			{
				m_SettingsFile.OnTimer(nIDEvent);
			}

		
	
			//m_RawResponseDisplay.Format("Incomplete Message received (timed out)");

//			UpdateData(FALSE);
			break;


		case	SETTINGSFILE_TIMERID:
			if (m_SettingsFile.m_bSettingsFileInProgress)
			{
				m_SettingsFile.OnTimer(nIDEvent);
			}
			break;

		case FIRMWAREFILE_TIMERID:
			if (m_firmwareFile.m_bFirmwareFileInProgress)
			{
				m_firmwareFile.OnTimer(nIDEvent);
			}
			break;

		case	ZIMPEDANCE_TIMERID:
			if (m_dlgZImpedance.m_bInProgress)
			{
				m_dlgZImpedance.OnTimer(nIDEvent);
			}
			break;
		
		case	OLYIMPEDANCE_TIMERID:
			if (m_dlgOlyImpedance.m_bInProgress)
			{
				m_dlgOlyImpedance.OnTimer(nIDEvent);
			}
			break;

		case	GETSTATE_TIMERID:
			{
				GetStateRequestNext();
			}
			break;
	
		case	SENDSTATE_TIMERID:
			{
				SendStateSendNext();
			}
			break;


		case MSGSEND_TIMERID:
#if 0
{
			if (!m_bDownloading)
				break;
			if (m_nSendFilePos >= m_nSendFileSize  )
			{
				m_nSendFilePos = 	m_nSendFileSize;
				nLength = 0;
			}
			else
			{
				nLength = GetNextMessage(theMessage);
			}
			
			int i,nPos;
			char drive[_MAX_DRIVE];
			char dir[_MAX_DIR];
			char fname[_MAX_FNAME];
			char ext[_MAX_EXT];
    		int nFileNameLength;

			_splitpath(m_strPostFileName.GetBuffer(0), drive, dir, fname, ext );
			m_strPostFileName.ReleaseBuffer();

			strcat(fname,ext);

			msgDownload.id =  MANDOLIN_MSG_POST_FILE;
			msgDownload.transport =  0;
			msgDownload.payload = pPayloadValues;

			// fill in the id
			i = 0;
			nPos = 0;
			pPayloadValues[nPos++] = m_nHALFileId & 0x0ff;
			pPayloadValues[nPos++] = (m_nHALFileId>>8) & 0x0ff;
			pPayloadValues[nPos++] = (m_nHALFileId >> 16) & 0x0ff;
			pPayloadValues[nPos++] = (m_nHALFileId >> 24) & 0x0ff;	// file type
			
			// reverse the file string
			for (i=0;i< (nPos/4);i++)
			{
				a1 = pPayloadValues[i*4 + 0];
				a2 = pPayloadValues[i*4 + 1];
				a3 = pPayloadValues[i*4 + 2];
				a4 = pPayloadValues[i*4 + 3];
			
				pPayloadValues[i*4 + 0] = a4;
				pPayloadValues[i*4 + 1] = a3;
				pPayloadValues[i*4 + 2] = a2;
				pPayloadValues[i*4 + 3] = a1;
			}

			// put in the file offset
			//	Payload:	FileID
			//		File size (in bytes) - 32-bit
			//		Fileoffset (in bytes) - 32-bit
			//		Bytes to Post - 32-bit (0 <= BtP <= page size)
			//		file data
			//	Reply:		Payload:  0 -> DELETE, 1 -> POST.

			pPayloadValues[nPos++] = (m_nSendFileSize >> 0) & 0x0ff;
			pPayloadValues[nPos++] = (m_nSendFileSize >> 8) & 0x0ff;
			pPayloadValues[nPos++] = (m_nSendFileSize >> 16) & 0x0ff;
			pPayloadValues[nPos++] = (m_nSendFileSize >> 24) & 0x0ff;

			pPayloadValues[nPos++] = (m_nSendFilePos >> 0) & 0x0ff;
			pPayloadValues[nPos++] = (m_nSendFilePos >> 8) & 0x0ff;
			pPayloadValues[nPos++] = (m_nSendFilePos >> 16) & 0x0ff;
			pPayloadValues[nPos++] = (m_nSendFilePos >> 24) & 0x0ff;

			pPayloadValues[nPos++] = ((nLength) >> 0) & 0x0ff;
			pPayloadValues[nPos++] = ((nLength) >> 8) & 0x0ff;
			pPayloadValues[nPos++] = ((nLength) >> 16) & 0x0ff;
			pPayloadValues[nPos++] = ((nLength) >> 24) & 0x0ff;

			// pad the Mesasge with 0 to make it word allign
			for(i=0;i< (int)((nLength+3)/4)*4-nLength;i++)
			{
				theMessage[nLength+i] = 0;
			}
			nLength += i;
			
			for(i=0;i<(nLength/4);i++)
			{
				pPayloadValues[nPos++] = theMessage[i*4+0] & 0x0ff;
				pPayloadValues[nPos++] = theMessage[i*4+1] & 0x0ff;
				pPayloadValues[nPos++] = theMessage[i*4+2] & 0x0ff;
				pPayloadValues[nPos++] = theMessage[i*4+3] & 0x0ff;
			}

			msgDownload.length = nPos/4;

			TryToSendMandolinMessage(&msgDownload, false);

	
			if (m_nSendFilePos >= m_nSendFileSize )
			{
				m_bDownloading = false;
				m_cfDownloadFile.Close();
				m_strResponseStatusDisplay.Format("Download Complete - Total Byte Size %d",m_nSendFileSize);
				SetDlgItemText(IDC_RESPONSESTATUS,m_strResponseStatusDisplay);
				if (m_dlgUpgrade.m_bUpgrade)
				{
					m_dlgUpgrade.OnSetUpgradeProgress(100);
					m_dlgUpgrade.OnSetStatus("File Send Complete");
				}
			}
			else
			{
				if (m_dlgUpgrade.m_bUpgrade)
				{
					if (!m_nSendFileSize)
					{
						m_dlgUpgrade.OnSetUpgradeProgress(0);
					}
					else
					{
						m_dlgUpgrade.OnSetUpgradeProgress(m_nSendFilePos*1.0/m_nSendFileSize*100);
					}
				}

				m_strResponseStatusDisplay.Format("Download %d bytes from offset %d of %d",nLength, m_nSendFilePos,m_nSendFileSize);
				SetDlgItemText(IDC_RESPONSESTATUS,m_strResponseStatusDisplay);
				m_nSendFilePos += nLength; // go to the next packet
				SetTimer (MSGSEND_TIMERID, 3000 , NULL);	// set timeout
		    }
		}
		break;
#endif // 0

	
	default:
			break;
	}
}


// ---------------------------------------------------------


bool   CRUglyDlg::InitComPort(int nPort, int nBaud, int theParity)
{

	bool bStatus = TRUE;


	//m_nConnectionParity = 0;
	if (pCPCommPort != NULL )
	{
		pCPCommPort->cwndNotify = this->m_hWnd;			// setup to call window back when data received
		//pCPCommPort->cwndNotify = NULL;
		pCPCommPort->msgID = wm_DataAvailable;
		pCPCommPort->bPort = (unsigned char) nPort;
		pCPCommPort->bIgnoreRxData = FALSE;
		pCPCommPort->UseMultithreaded = TRUE;

		if (theParity < 0)
		{
			pCPCommPort->bParity = NOPARITY;			// default
		}
		else
		{
			pCPCommPort->bParity = theParity;
		}

//		pCPCommPort->dwBaudRate = CBR_9600;
//		if ((nBaud >= BAUDRATE_MAX) || (nBaud < 0))
		if ((nBaud >= gBaudRates[BAUDRATE_MAX-1]) || (nBaud < 0))
		{
//			pCPCommPort->dwBaudRate = 921600;
			pCPCommPort->dwBaudRate = gBaudRates[BAUDRATE_MAX-1];
		}
		else
		{
//			pCPCommPort->dwBaudRate = gBaudRates[nBaud];
			pCPCommPort->dwBaudRate = nBaud;

		}

		if (0)
		{
			//pCPCommPort->bByteSize = 8;					// 4-8

			//pCPCommPort->bFlowCtrl = FC_NONE;			// default
			//pCPCommPort->bFlowCtrl = FC_HARDWARE;
			//pCPCommPort->bFlowCtrl = FC_SOFTWARE;

			//pCPCommPort->bParity = NOPARITY;			// default
			//pCPCommPort->bParity = ODDPARITY;
			pCPCommPort->bParity = EVENPARITY;
			//pCPCommPort->bParity = MARKPARITY;
			//pCPCommPort->bParity = SPACEPARITY;

			pCPCommPort->bStopBits = ONESTOPBIT;		// default
			//pCPCommPort->bStopBits = ONE5STOPBITS;
			//pCPCommPort->bStopBits = TWOSTOPBITS;


			//pCPCommPort->dwBaudRate = CBR_115200;
		}


		pCPCommPort->callbackHandler = (ComDataHandler) OnRxDataMultiThreaded;

		if (OpenConnection(pCPCommPort) == FALSE)
		{	// connection has failed to open
			char pcBuffer[100];
			sprintf(pcBuffer,"Could not open Comm Port # %d", pCPCommPort->bPort);
			MessageBox ( pcBuffer, "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL );
			bStatus = FALSE;
			//m_ResponseStatusDisplay.Format("Could not connect to port %d",nPort);
			SetDlgItemText(IDC_RESPONSESTATUS,"Comm Status: disconnected");
			SetDlgItemText(IDC_CONNECTIONSTATUS,"Comm Status: disconnected");
		}

		else
		{
			//m_ResponseStatusDisplay.Format("Connect :COM%d @%d",nPort,pCPCommPort->dwBaudRate  );
			char pcBuffer[100];
			sprintf(pcBuffer,"Comm Status: Connected to Com%d at %d", pCPCommPort->bPort,pCPCommPort->dwBaudRate);
			SetDlgItemText(IDC_RESPONSESTATUS,pcBuffer);
			SetDlgItemText(IDC_CONNECTIONSTATUS,pcBuffer);
		}
	}

	UpdateData(FALSE);

	return bStatus;

}
void CRUglyDlg::OnChangeCommSettings(int nCommPort, int nCommBaudRate, int nCommParity) 
{
/*		if ( bCommPortSemaphore )
		{
//			SetDlgItemInt(IDC_SETCOM, m_dlgParent->m_nConnectionPortNumber);
			MessageBox("WTF?.");
		}

		else
		{	
			bCommPortSemaphore = TRUE;	// must make this section of code a mutex *manually* (thank you, windows)

			if (m_nComType == COMTYPE_COM)
			{
				if ( (nCommPort != m_nConnectionPortNumber) || (nCommBaudRate != m_nConnectionBaud) ||
					(m_nConnectionParity != nCommParity) )
				{
					m_nConnectionPortNumber = nCommPort;
					m_nConnectionBaud = nCommBaudRate;
					m_nConnectionParity = nCommParity;
					if (m_bIsConnected)
						CloseConnection(pCPCommPort);
					m_bIsConnected = InitComPort(m_nConnectionPortNumber,m_nConnectionBaud,m_nConnectionParity);
					//OnSetComportConnectButtonText();

				}
			}
			bCommPortSemaphore = FALSE;
		}
*/
	if ( bCommPortSemaphore )
	{
//		SetDlgItemInt(IDC_SETCOM, m_dlgParent->m_nConnectionPortNumber);
		MessageBox("WTF?.");
	}
	else
	{	
		bCommPortSemaphore = TRUE;	// must make this section of code a mutex *manually* (thank you, windows)
		// if there's already a connection, shut it down (but only if things are changing) 
		if (m_bIsConnected) {
			if (m_nComType == COMTYPE_SOCKET) {
				CloseSocketComm();
				m_bIsConnected = false;
			} else if (m_nComType == COMTYPE_COM) {
				if ( (nCommPort != m_nConnectionPortNumber) || (nCommBaudRate != m_nConnectionBaud) ||
					(m_nConnectionParity != nCommParity) )
				{
					CloseConnection(pCPCommPort);
					m_bIsConnected = false;
				}
			}
		}
		// if it's not still connected (i.e. the settings have changed), then start the connection
		if (!m_bIsConnected) {
			m_nComType = COMTYPE_COM;
			m_nConnectionPortNumber = nCommPort;
			m_nConnectionBaud = nCommBaudRate;
			m_nConnectionParity = nCommParity;
			m_bIsConnected = InitComPort(m_nConnectionPortNumber,m_nConnectionBaud,m_nConnectionParity);
		}
		bCommPortSemaphore = FALSE;
	}
	ShowIPAddress();
}
void CRUglyDlg::OnChangeSocketSettings(unsigned int nIP, int nPort, SocketType nType)
{
	// if there's already a connection, shut it down (but only if things are changing) 
	if (m_bIsConnected) {
		if (m_nComType == COMTYPE_COM) {
			CloseConnection(pCPCommPort);
			m_bIsConnected = false;
		} else if (m_nComType == COMTYPE_SOCKET) {
//			if ((nIP != m_nSocketIPAddress) || (nPort != m_nSocketPort) || (nType != m_nSocketType)) {
				CloseSocketComm();
				m_bIsConnected = false;
//			}
		}
	}
	// if it's not still connected (i.e. the settings have changed), then start the connection
	if (!m_bIsConnected) {
		m_nComType = COMTYPE_SOCKET;
		m_nSocketIPAddress = nIP;
		m_nSocketPort = nPort;
		m_nSocketType = nType;
		m_bIsConnected = InitSocketComm(m_nSocketIPAddress, m_nSocketPort, m_nSocketType);
	}
	ShowIPAddress();

}

void CRUglyDlg::OnCommSettings()
{
	CCommSettings dlgCommSettings;

	dlgCommSettings.m_dlgParent = this;
	dlgCommSettings.m_nCommPort= m_nConnectionPortNumber;
	dlgCommSettings.m_nCommBaudRate = m_nConnectionBaud;
	dlgCommSettings.m_nCommParity = m_nConnectionParity;

	dlgCommSettings.DoModal();

}
void CRUglyDlg::OnSocketSettings()
{
	CSocketSettings dlgSocketSettings;

	dlgSocketSettings.m_dlgParent = this;
	dlgSocketSettings.m_nIPAddress = m_nSocketIPAddress;
	dlgSocketSettings.m_nPort = m_nSocketPort;
	dlgSocketSettings.m_nType = m_nSocketType;

	dlgSocketSettings.DoModal();

}
// ---------------------------------------------------------


bool   CRUglyDlg::InitSocketComm(UInt nIPAddress, int nPort, SocketType nSocketType)
{
	if (pSocketCommInfo != NULL)
	{

		pSocketCommInfo->cwndNotify = this->m_hWnd;			// setup to call window back when data received
		pSocketCommInfo->msgID = wm_DataAvailable;
		pSocketCommInfo->callbackHandler = (SocketCommDataHandler) OnRxDataMultiThreaded;
		pSocketCommInfo->bIgnoreRxData = FALSE;
		pSocketCommInfo->UseMultithreaded = TRUE;

		pSocketCommInfo->m_nIPAddress =	nIPAddress;
		pSocketCommInfo->m_nPort =	nPort;
		pSocketCommInfo->m_nSocketType = nSocketType;

		return OpenSocketComm();
	} else {
		return false;
	}
}

bool   CRUglyDlg::OpenSocketComm()
{
	bool bStatus = false;

	if (pSocketCommInfo != NULL)
	{
		if (pSocketCommInfo->OpenSocketCommConnection() == FALSE)
		{	// connection has failed to open
			char pcBuffer[100];
			//MessageBox ( "Connection to DSP Board failed", "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL );
			bStatus = FALSE;
//			m_ResponseStatusDisplay.Format("Could not connect to Socket");
			sprintf(pcBuffer,"Could not connect to %d.%d.%d.%d", (m_nSocketIPAddress>>24)&0xFF,(m_nSocketIPAddress>>16)&0xFF,(m_nSocketIPAddress>>8)&0xFF,m_nSocketIPAddress&0xFF);
			MessageBox ( pcBuffer, "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL );
			SetDlgItemText(IDC_RESPONSESTATUS,"Comm Status: disconnected");
			SetDlgItemText(IDC_CONNECTIONSTATUS,"Comm Status: disconnected");
		}
		else
		{
			char pcBuffer[100];
			bStatus = true;
//			m_ResponseStatusDisplay.Format("Connect :Socket");
			sprintf(pcBuffer,"Comm Status: Connected to %d.%d.%d.%d", (m_nSocketIPAddress>>24)&0xFF,(m_nSocketIPAddress>>16)&0xFF,(m_nSocketIPAddress>>8)&0xFF,m_nSocketIPAddress&0xFF);
			SetDlgItemText(IDC_RESPONSESTATUS,pcBuffer);
			SetDlgItemText(IDC_CONNECTIONSTATUS,pcBuffer);

		}
	}
	//OnSetComportConnectButtonText();

	UpdateData(FALSE);

	return bStatus;
}

bool   CRUglyDlg::CloseSocketComm()
{
	bool bStatus = false;
	if (pSocketCommInfo != NULL)
	{
		bStatus = true;
	 	pSocketCommInfo->CloseSocketCommConnection();
		SetDlgItemText(IDC_CONNECTIONSTATUS,"Comm Status: disconnected");
	}

	return bStatus;
}

void CRUglyDlg::OnHelpAbout()
{
	// TODO: Add your command handler code here
	CAboutDlg dlgAbout;

	dlgAbout.m_strProductName =    	m_strProductName;
	dlgAbout.m_strProductVersion = 	m_strProductVersion;
	dlgAbout.m_strCompanyName =    	m_strCompanyName;
	dlgAbout.m_strCopyright =      	m_strCopyright;

	dlgAbout.DoModal();

}

void CRUglyDlg::GetFileVersionInfo()
{
	DWORD dwVerHnd;
	DWORD dwVerInfoSize;
	char szFullPath[256];

	m_strProductName = "";
	m_strProductVersion = "";
	//m_strProductDate = "";
	m_strCompanyName = "";
	m_strCopyright = "";




	::GetModuleFileName(AfxGetApp()->m_hInstance, szFullPath, sizeof(szFullPath));
	dwVerInfoSize = ::GetFileVersionInfoSize(szFullPath, &dwVerHnd);

	if (dwVerInfoSize)
	{
		// If we were able to get the information, process it:
		HANDLE  hMem;
		LPVOID  lpvMem;
		char    szGetName[256];
	    LPVOID	lpBuffer;
		unsigned int dwBytes;

        //char	szVersion[256];
		//int     cchRoot;
		//int     i;
		hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
		lpvMem = GlobalLock(hMem);
		::GetFileVersionInfo(szFullPath, dwVerHnd, dwVerInfoSize, lpvMem);
		strcpy(szGetName,"\\");
		if (::VerQueryValue(lpvMem,szGetName,&lpBuffer,&dwBytes))
		{
			VS_FIXEDFILEINFO fileInfo;
			fileInfo = *(VS_FIXEDFILEINFO*)lpBuffer;

			//m_strProductDate.Format("%d%d",fileInfo.dwFileDateMS,fileInfo.dwFileDateLS);

			// --------------------------
			// ProductName
			// --------------------------
			lstrcpy(szGetName, "\\StringFileInfo\\040904b0\\");
			strcat(szGetName,"ProductName");
			if (::VerQueryValue(lpvMem,szGetName,&lpBuffer,&dwBytes))
			{
				m_strProductName.Format("%s",lpBuffer);
			}


			// --------------------------
			// ProductVersion
			// --------------------------
			lstrcpy(szGetName, "\\StringFileInfo\\040904b0\\");
			strcat(szGetName,"ProductVersion");
			if (::VerQueryValue(lpvMem,szGetName,&lpBuffer,&dwBytes))
			{
				m_strProductVersion.Format("%s",lpBuffer);
			}

			// --------------------------
			// CompanyName
			// --------------------------
			lstrcpy(szGetName, "\\StringFileInfo\\040904b0\\");
			strcat(szGetName,"CompanyName");
			if (::VerQueryValue(lpvMem,szGetName,&lpBuffer,&dwBytes))
			{
				m_strCompanyName.Format("%s",lpBuffer);
			}

			// --------------------------
			// Copyright
			// --------------------------
			lstrcpy(szGetName, "\\StringFileInfo\\040904b0\\");
			strcat(szGetName,"LegalCopyright");
			if (::VerQueryValue(lpvMem,szGetName,&lpBuffer,&dwBytes))
			{
				m_strCopyright.Format("%s",lpBuffer);
			}


		}

		GlobalUnlock(hMem);
		GlobalFree(hMem);
	}


}




// outpt text
void CRUglyDlg::UpdateOutputText(CString theText,bool bAppend, bool bForce, bool bLineFeed)
{
	if ((!m_bFreezeDisplay) || (bForce))
	{

		CString scText;
		GetDlgItemText(IDC_RICHEDIT_OUTPUT, scText);
		int theLength = strlen(theText);
		int theCurrentLength =  scText.GetLength();

		if ( (theCurrentLength+theLength+10) > MAX_RX_DISPLAY )
		{
//			scText.Delete(0, MAX_RX_DISPLAY-(theLength+10));
			scText.Delete(0, MAX_RX_DISPLAY*.25);
		}
		if (bLineFeed)
		{
			scText += "\n";
		}
		scText += theText;
		SetDlgItemText(IDC_RICHEDIT_OUTPUT, scText);
		int nLine = m_OutputTextBox.GetLineCount();
		long lastChar = scText.GetLength();
		//m_OutputTextBox.LineScroll(1);//, int nChars = 0 );
		m_OutputTextBox.SetSel(lastChar,lastChar);
		m_OutputTextBox.HideSelection( false, true);
	}
	UpdateData(FALSE); // *&&&&&&gm added 5/28
}

void CRUglyDlg::UpdateInputText(CString theText,bool bAppend, bool bForce)
{
	if ((!m_bFreezeDisplay) || (bForce))
	{

		CString scText;
		GetDlgItemText(IDC_RICHEDIT_INPUT, scText);
		int theLength = strlen(theText);
		int theCurrentLength =  scText.GetLength();

		if ( (theCurrentLength+theLength+10) > MAX_RX_DISPLAY )
		{
//			scText.Delete(0, MAX_RX_DISPLAY-(theLength+10));
			scText.Delete(0, MAX_RX_DISPLAY*.25);
		}
		scText += "\n" + theText;
		SetDlgItemText(IDC_RICHEDIT_INPUT, scText);
		int nLine = m_InputTextBox.GetLineCount();
		long lastChar = scText.GetLength();
		//m_InputTextBox.LineScroll(1);//, int nChars = 0 );
		m_InputTextBox.SetSel(lastChar,lastChar);
		m_InputTextBox.HideSelection( false, true);
	}
}

int CRUglyDlg::CopyMandolinMessage(mandolin_message* pDest, mandolin_message* pSrc)
{
	uint8* pDestPayload;
	uint8* pSrcPayload;
	pDest->sync = pSrc->sync;
	pDest->sequence = pSrc->sequence;
	pDest->length = pSrc->length;
	pDest->transport = pSrc->transport;
	pDest->id = pSrc->id;
	pDest->checksum = pSrc->checksum;
	if (!pSrc->payload)
	{
		pDest->payload = 0;
		return MANDOLIN_HEADER_WORDS*MANDOLIN_BYTES_PER_WORD;
	}
	pDestPayload = (uint8*)pDest->payload;
	pSrcPayload = (uint8*)pSrc->payload;
	for(int i=0;i<(pSrc->length*4);i++)
	{
		*pDestPayload++ = *pSrcPayload++;

	}
	return (MANDOLIN_HEADER_WORDS+pSrc->length+1)*MANDOLIN_BYTES_PER_WORD;

}
// ------------------------------------------------------------------------
// Message Sending
// ------------------------------------------------------------------------
// Create a standard ACK to the message enclosed in 'in' and enqueue it.
// Returns false on failure (of memory allocation), true otherwise.
bool  CRUglyDlg::Send_reply_ACK(mandolin_message* pMessage)
{
    
	mandolin_message mReplyMsg;
//#if(0)
	if (pMessage->transport & MANDOLIN_TRANSPORT_NOACK)
		return true;
//#endif

	mReplyMsg.id =  pMessage->id;
	mReplyMsg.length = 0;
    mReplyMsg.transport = pMessage->transport | MANDOLIN_TRANSPORT_REPLY;
    mReplyMsg.sequence = pMessage->sequence;

	return TryToSendMandolinMessage(&mReplyMsg, false);
 
}

bool  CRUglyDlg::Send_reply_withsubid_ACK(mandolin_message* pMessage, unsigned int uiSubId)
{
    
	mandolin_message mReplyMsg;
	unsigned int pPayload[2];

	if (pMessage->transport & MANDOLIN_TRANSPORT_NOACK)
		return true;


	mReplyMsg.id =  pMessage->id;
	mReplyMsg.length = 1;
	mReplyMsg.payload = pPayload;
	pPayload[0] = uiSubId;

    mReplyMsg.transport = pMessage->transport | MANDOLIN_TRANSPORT_REPLY;
    mReplyMsg.sequence = pMessage->sequence;

	return TryToSendMandolinMessage(&mReplyMsg, false);
 
}

bool  CRUglyDlg::Send_reply_withpayload(mandolin_message* pMessage, unsigned int* pPayload, int nLength)
{
    
	mandolin_message mReplyMsg;

	if (pMessage->transport & MANDOLIN_TRANSPORT_NOACK)
		return true;


	mReplyMsg.id =  pMessage->id;
	mReplyMsg.length = nLength;
	mReplyMsg.payload = pPayload;

    mReplyMsg.transport = pMessage->transport | MANDOLIN_TRANSPORT_REPLY;
    mReplyMsg.sequence = pMessage->sequence;

	return TryToSendMandolinMessage(&mReplyMsg, false);
 
}





bool  CRUglyDlg::TryToSendMandolinMessage(mandolin_message* pMsg, bool bExpectingResponse)
{
	char pcMessage[2000];

	bExpectingResponse = false;
	if (!IS_MANDOLIN_REPLY(pMsg->transport))
	{
		bExpectingResponse = true;

		pMsg->transport = pMsg->transport;
		pMsg->sequence = m_nTxSequence & 0x0ff;
		m_nTxSequence  = (m_nTxSequence+1) & 0x0ff;
		if (m_nTxSequence == 0) 
			m_nTxSequence++;
	} 
	else
	{
		pMsg->transport += 0;
	}

	MANDOLIN_MSG_pack(pMsg, pMsg->sequence); // calc checksums.
	int nLength = MANDOLIN_MSG_write(m_pTxMsgBuffer,pMsg);


	if ((m_nInputTextBoxFilter == INPUTTEXTBOXFILTER_INTERPRET) &&(!m_bFreezeDisplay))
	{
		m_MandolinTextSummary.MANDOLIN_handle_MsgTextSummary(pcMessage,pMsg, sizeof(pcMessage)-1);
		UpdateOutputText(pcMessage, true);
	}


	return (TryToSendMessage((char*)m_pTxMsgBuffer, nLength, bExpectingResponse));
}


bool  CRUglyDlg::TryToSendMessage(char * theMessage , int nLength, bool bExpectingResponse)
{

	bool	bReturnVal = TRUE;

	char theBuff[20];
	char *c;
	CString theText;
	int i;

	//if (bLogFileOpen)
	//{
	//	cfCaptureOutRaw.Write(theMessage, nLength);
	//}

#if(0)
	//debug
	bExpectingResponse = false;		// without this.. sending 2 messages fail
	//end debug
#endif

	// Convert the Message to String
#if(WAITFOR_RESPONSE)
	if (m_bExpectingResponse)
#else
	if (false) //(m_bExpectingResponse)	//let this pass for now... sending 2 edits back to back has issues.
#endif
	{
		m_strResponseStatusDisplay.Format("Blocked, waiting response");
		SetDlgItemText(IDC_RESPONSESTATUS,m_strResponseStatusDisplay);
		//UpdateData(FALSE);
		return false;
	}

	theText = "";
	//if (pcDeviceName != 0)
	//{
	//	theText += "[";
	//	theText += pcDeviceName;
	//	theText += "]\n";
	//}

	if ((!m_bFreezeDisplay) && (m_nInputTextBoxFilter != INPUTTEXTBOXFILTER_INTERPRET))
	{
	//	theText = m_OutputTextBox.GetText();
		//if ((*theMessage & 0x0ff) == 0x0ab)
		//{
		c = theMessage;
		for(i=0;i<nLength;i++)
		{
			if ((i != 0) && ((i%4)== 0))
			{
				if (i%16)
				{
					theText += " ";
				}
				else
				{
					theText += "\n";
				}
			}
			sprintf(theBuff,"%2.2X ",(int)(*c++ & 0x0ff));
			theText += theBuff;

		}

		UpdateOutputText(theText,true);


	}
	//if (bLogFileOpen)
	//{
	//	nLogFileOutByteCount += nLength;
	//	cfCaptureOut.Write("\n",1);
	//	cfCaptureOut.Write(theText,strlen(theText));

	//	// to update the Log file count
	//	char buff[20];
	//	sprintf(buff,"%d",nLogFileOutByteCount);
	//	SetDlgItemText(IDC_LOGFILEOUTBYTECOUNT,buff);
	//}

	if (!m_bIsConnected)
	{
		m_strResponseStatusDisplay.Format("Not connected to com port");
		bReturnVal = FALSE;
	}
	else
	{

		m_bMessageReceived = FALSE;
		if (bExpectingResponse)
		{
			m_nExpectingResponseSequence = (theMessage[MANDOLIN_BYTE_SEQUENCE] & 0x0ff);

			//if (m_bSettingsFileInProgress)
			//{
			//	m_nSettingsFileReplySequence = m_nExpectingResponseSequence;
			//}
			
#if(WAITFOR_RESPONSE)
			m_bExpectingResponse = true;
			SetTimer (MSGTIMEOUT_TIMERID, 500 , NULL);
#endif

		}
		//else
		//{
		//	m_bExpectingResponse = false;
		//}

		MessageSend( theMessage, nLength );

	}

	SetDlgItemText(IDC_RESPONSESTATUS,m_strResponseStatusDisplay);
	UpdateData(FALSE);

	return bReturnVal;

}

void CRUglyDlg::MessageSend( char * theMessage , int nLength)
{
	//if (m_bExpectingResponse)
	//{
	//	if (m_bParamEditTest)
	//	{
	//		KillTimer( PARAMEDITTEST_TIMERID );					// reset time out
	//		SetTimer(PARAMEDITTEST_TIMERID,2000,NULL);	// time out
	//	}
	//	else
	//	{
	//		KillTimer( MSGTIMEOUT_TIMERID );					// reset time out
	//		SetTimer( MSGTIMEOUT_TIMERID,  nMsgTimeOutRate, NULL);
	//	}
	//}

   	if ( ( theMessage != NULL)&& ( nLength != 0 ) )
	{
		if (m_nComType == COMTYPE_COM)
		{
			WriteCommBlockBinary(pCPCommPort, theMessage , nLength);

		}
		else if (m_nComType == COMTYPE_SOCKET)
		{
			if (pSocketCommInfo != NULL)
			{
				pSocketCommInfo->WriteBytes(theMessage,nLength);
			}
		}
//		else if (COMTYPE_CONMON == m_nComType)
//		{
//			char szText[256];
//			if (pcDeviceName == 0)
//			{
//				GetDlgItemText(IDC_EDIT_CONMON_DEVICE, szText, 256);
//			}
//			else
//			{
//				strcpy(szText,pcDeviceName);
//			}
//			
//			if (bControlMsg)
//				cmc_send_control_message(&cmc_data, szText, (const uint8*)theMessage, nLength);
//			else
//				cmc_send_status_message(&cmc_data, (const uint8*)theMessage, nLength);
//
//		}
//*/
	}

	//SetTimer(MSGTIMEOUT_TIMERID, nMsgTimeOutRate, NULL); // set time out

}
bool CRUglyDlg::MANDOLIN_handle_Process(mandolin_message_wrapper_s* bmwsp, mandolin_fifo *bfp)
{
	mandolin_parse_error mError;
	uint16 payload_length;
	char pcMessage[3000];


	if (MANDOLIN_FIFO_is_empty(bfp))
	{
		return false;
	}	

	mInMsg.payload = mInMsgPayload;
	
	bmwsp->bmhsp = &mInMsg;	
	 
	if (MANDOLIN_FIFO_parse(bfp, &mError))
	{
		payload_length = PACK_SHORT(MANDOLIN_FIFO_get_byte(bfp,MANDOLIN_BYTE_LENGTH_HI),MANDOLIN_FIFO_get_byte(bfp,MANDOLIN_BYTE_LENGTH_LO));

		if (payload_length > MANDOLIN_MAX_PAYLOAD_DATA_WORDS)
		{
			// dump the message
            MANDOLIN_FIFO_dump_bytes(bfp,MANDOLIN_HEADER_CHECKSUM_BYTES+payload_length*MANDOLIN_BYTES_PER_WORD);

		}
		else
		{
			if (MANDOLIN_MSG_read(bfp, &mInMsg, mInMsgPayload))
			{
	///*				if (m_nInputTextBoxFilter == INPUTTEXTBOXFILTER_DEVICE)
	//				{

	//					char* pC;
	//					uint8 uiMessage[(MANDOLIN_HEADER_WORDS + MANDOLIN_MAX_PAYLOAD_DATA_WORDS)*4];
	//					int nSize;
	//					int i;
	//					pC =  pcMessage;
	//					nSize = MANDOLIN_MSG_write(uiMessage, &mInMsg);
	//					for(i=0;i<nSize;i++)
	//					{
	//						sprintf(pC,"%2.2X ",uiMessage[i]);
	//						pC += 3;
	//						if (((i+1)%16) == 0)
	//						{
	//							sprintf(pC,"\n");
	//							UpdateInputText(pcMessage);
	//							pC = pcMessage;

	//						}
	//					}
	//					if (pC != pcMessage)
	//					{
	//						sprintf(pC,"\n");
	//						UpdateInputText(pcMessage);
	//					}
	//				}*/

				// fill in the wrapper args
				bmwsp->reply = IS_MANDOLIN_REPLY(mInMsg.transport);
				bmwsp->forward = false; 
			
				//if ((bmwsp->reply) && (m_bSettingsFileInProgress) && (mInMsg.sequence == m_nSettingsFileReplySequence))
				//{
				//	SetTimer(SETTINGSFILE_TIMERID,1,NULL); // send the next one
				//}

				if ((bmwsp->reply) && (m_bExpectingResponse) && (mInMsg.sequence == m_nExpectingResponseSequence))
				{
					m_bExpectingResponse = false;

					//KillTimer( MSGTIMEOUT_TIMERID );					// reset time out
				}

				if (m_nInputTextBoxFilter == INPUTTEXTBOXFILTER_INTERPRET)
				{
					m_MandolinTextSummary.MANDOLIN_handle_MsgTextSummary(pcMessage,&mInMsg, sizeof(pcMessage)-1);
					UpdateInputText(pcMessage);
				}
				ProcessMessage(bmwsp->bmhsp, bmwsp->bmhsp->length);
			  	//MANDOLIN_MSG_handle(bmwsp);    
			  	
			  	//MANDOLIN_MSG_clear(&mInMsg);	
			}
			return true;
		}
//		bfp->valid_header = 0; // reset to read the next one.
	}

	return false;
	

}

void CRUglyDlg::ProcessMessage(mandolin_message* theMessage, int theLength)
{
//	if (theLength < 8)
//		return;

	int i;

	int* pPayload;
	char pcBuffer[300];

	int theParam;
    uint32 *in_ptr = (uint32 *) theMessage->payload;
	//uint32 uiQueryNum;

	//if (m_dlgParameterEditTest.m_bTestRunning)
	//{
	//	m_dlgParameterEditTest.ProcessMessage(theMessage,theLength);
	//}

	//if ((m_dlgMicSweep.m_bGovSweepRunning) || (m_dlgMicSweep.m_bDSPSweepRunning))
	//{
	//	m_dlgMicSweep.ProcessMessage(theMessage,theLength);
	//}


	//if (m_dlgFileGet.IsWindowVisible()) 
	//{
	//	m_dlgFileGet.ProcessMessage(theMessage,theLength);
	//}

	//m_nNestedTargetId = 0;
	//if (theMessage->id == MANDOLIN_MSG_NESTED)
	//{
	//	if ((!theMessage->payload) || (theMessage->length<1))
	//	{
	//		return;
	//	}
	//	m_nNestedTargetId = in_ptr[0];
	//	theMessage->id = ((m_nNestedTargetId >> 16 ) & 0x0ffff);
	//	m_nNestedTargetId &= 0x0ffff;
	//
	//	theMessage->length--;
	//	if (theMessage->length>0)
	//	{
	//		theMessage->payload =  (void*)(&in_ptr[1]);
	//	}
	//	else
	//	{
	//		theMessage->length = 0;
	//		theMessage->payload = NULL;
	//	}
	//}

	mandolin_message mReplyMsg;
	int pReplyPayload[100];
	mReplyMsg.id =  theMessage->id;
	mReplyMsg.length = 0;
    mReplyMsg.transport = theMessage->transport | MANDOLIN_TRANSPORT_REPLY;
    mReplyMsg.sequence = theMessage->sequence;
	mReplyMsg.payload = (uint8*) pReplyPayload;

		// some state stuff
	//if ((theMessage->transport & MANDOLIN_TRANSPORT_REPLY ) && m_bSettingsFileInProgress) 
	//{
	//	if (theMessage->sequence == m_nSettingsFileReplySequence)
	//	{
	//		m_bSettingsFileWaitingAck = false;
	//		if (IS_MANDOLIN_NACK(theMessage->transport))
	//		{
	//			uint32 uiValue;
	//			sprintf(pcBuffer, "Setting File downlaod error: received a NACK response: 0x%x", m_nSettingsFileReplySequence);
	//			SetDlgItemText(IDC_RESPONSESTATUS,pcBuffer);
	//			uiValue = 0;
	//			if (theMessage->payload)
	//			{
	//				uiValue = MANDOLIN_PAYLOAD_GETUINT32(in_ptr,0);
	//			}
	//				
	//			m_nSettingsFileRetryCount++;

	//			if ((uiValue == MANDOLIN_ERROR_BUSY) &&(m_nSettingsFileRetryCount < 5))
	//			{
	//				m_bSettingsFileWaitingAck = true;
	//				TryToSendMandolinMessage(&m_msgSettingsFile, true);
	//				SetTimer(SETTINGSFILE_TIMERID,1000,NULL);
	//			}

	//			else
	//			{

	//				sprintf(pcBuffer, "Setting File downlaod error: stopped, too many NACKS or failed");
	//				SetDlgItemText(IDC_RESPONSESTATUS,pcBuffer);

	//				m_bSettingsFileInProgress = false;
	//				KillTimer(SETTINGSFILE_TIMERID);
	//			}

	//		}
	//		else
	//		{
	//			SetTimer(SETTINGSFILE_TIMERID,10,NULL); // send the next one
	//		}
	//	}
	//	else
	//	{
	//		// different ack
	//		SetDlgItemText(IDC_RESPONSESTATUS,"Setting File Download: ack but not correct sequence");

	//	}
	//}





	pPayload = (int*) theMessage->payload;




	// --------------------------------
	// message is valid
	// --------------------------------


//	if(m_bUpgradeWindowValid)
//	{
//		m_dUpgrade.ProcessMessage(theMessage,theLength);
//		return;
//	}


	i = 0;		// continue

	if(IS_MANDOLIN_NACK(theMessage->transport))
	{
		//// received a NACK
		//int nValue;
		//uint8* in_ptr;
		//if ((theMessage->payload) && (theMessage->length>0))
		//{
		//	in_ptr =  (uint8*) theMessage->payload;
		//	nValue =  ((in_ptr[3]&0x0ff) << 24) + ((in_ptr[2]&0x0ff) << 16) + ((in_ptr[1]&0x0ff) << 8) + ((in_ptr[0]&0x0ff) << 0);

		//}
		//m_strResponseStatusDisplay.Format("Received an NACK on seq: %d, msgid: %d, value: %d", theMessage->sequence, theMessage->id,nValue);
		//SetDlgItemText(IDC_RESPONSESTATUS,m_strResponseStatusDisplay); //UpdateData(FALSE);


		//switch (theMessage->id)
		//{
		//	case MANDOLIN_MSG_POST_FILE:

		//		//MessageBox("Post Has NACK","Post Error", MB_OK);
		//		break;


		//	case MANDOLIN_MSG_SET_APPLICATION_PARAMETER:

		//		break;


		//}



	}


	// check if in test mode
	switch (theMessage->id)
	{

	case MANDOLIN_MSG_PING:		// Ping message
//		DecodePing(theMessage);
		if (theMessage->transport & MANDOLIN_TRANSPORT_REPLY )
		{
		}
		else
		{
			Send_reply_ACK(theMessage);
		}
		break;


	case MANDOLIN_MSG_SET_SYSTEM_STATE:
		if (theMessage->transport & MANDOLIN_TRANSPORT_REPLY )
		{
		}
		else
		{
			//DecodeSetSystemState(theMessage,theLength);
		}
		break;

			

	case MANDOLIN_MSG_TRIGGER_SYSTEM_EVENT:
		if (theMessage->transport & MANDOLIN_TRANSPORT_REPLY )
		{
			switch(pPayload[0])
			{												
			case    MANDOLIN_SE_CONNECT:	

				if ((m_bMandolinConnectStateStart) &&	(m_nMandolinConnectStateState == MANDOLIN_CONNECT_SE_CONNECT_WAITINGACK))
				{
					MandolinConnectStateIncrementState();
					MandolinConnectStateDoState();
				}
				break;
			case    MANDOLIN_SE_DISCONNECT:				     break;
			case    MANDOLIN_SE_LOCK_CHANGES:			     break;
			case    MANDOLIN_SE_UNLOCK_CHANGES:			     break;
			case    MANDOLIN_SE_IDENTIFY:				     break;
			case    MANDOLIN_SE_SYNC_CURRENT_STATE:			 
				if ((m_bMandolinConnectStateStart) &&	(m_nMandolinConnectStateState == MANDOLIN_CONNECT_SE_SYNC_CURRENT_STATE_WAITINGACK))
				{
					MandolinConnectStateIncrementState();
					MandolinConnectStateDoState();
				}
				break;
			case    MANDOLIN_SE_BEGIN_UPDATE:			     break;
			case    MANDOLIN_SE_END_UPDATE:				     break;
			case    MANDOLIN_SE_PROCESS_FILES:
				// 1 status (fail 0, success 1, busy 2 with percent complete next byte)
				switch(pPayload[1])
				{
///*				case MANDOLIN_PROGRESS_FAIL:
//					KillTimer(UPGRADE_TIMERID);
//					if (m_dlgUpgrade.m_bUpgrade)
//					{
//						m_dlgUpgrade.OnSetStatus("Process file Fail");
//					}
//					break;
//	*/			//case MANDOLIN_PROGRESS_SUCCESS:
				//	KillTimer(UPGRADE_TIMERID);
				//	if (m_dlgUpgrade.m_bUpgrade)
				//	{
				//		m_dlgUpgrade.OnSetStatus("Process file PASS");
				//		m_dlgUpgrade.OnSetUpgradeProgress(100);

				//	}
				//	break;

	///*			case MANDOLIN_PROGRESS_BUSY:
	//				SetTimer(UPGRADE_TIMERID,500,NULL);
	//				if (m_dlgUpgrade.m_bUpgrade)
	//				{
	//					m_dlgUpgrade.OnSetUpgradeProgress(pPayload[2]);
	//				}
	//				break;*/


				}

				break;
			}

		}
		else
		{

			switch(pPayload[0])
			{												
			case    MANDOLIN_SE_CONNECT:				    
				Send_reply_withsubid_ACK(theMessage, pPayload[0]); 
				if ((m_bMandolinConnectStateStart) &&	(m_nMandolinConnectStateState == MANDOLIN_CONNECT_SE_CONNECT_WAIT))
				{
					MandolinConnectStateIncrementState();
					MandolinConnectStateDoState();
				}
				break;
			case    MANDOLIN_SE_DISCONNECT:				    Send_reply_withsubid_ACK(theMessage, pPayload[0]); break;
			case    MANDOLIN_SE_LOCK_CHANGES:			    Send_reply_withsubid_ACK(theMessage, pPayload[0]); break;
			case    MANDOLIN_SE_UNLOCK_CHANGES:			    Send_reply_withsubid_ACK(theMessage, pPayload[0]); break;
			case    MANDOLIN_SE_IDENTIFY:				    Send_reply_withsubid_ACK(theMessage, pPayload[0]); break;
			case    MANDOLIN_SE_SYNC_CURRENT_STATE:			break;//DecodeSyncCurrentState(theMessage,theLength); break;
			case    MANDOLIN_SE_BEGIN_UPDATE:			    Send_reply_withsubid_ACK(theMessage, pPayload[0]); break;
			case    MANDOLIN_SE_END_UPDATE:				    Send_reply_withsubid_ACK(theMessage, pPayload[0]); break;
			case    MANDOLIN_SE_PROCESS_FILES:			    Send_reply_withsubid_ACK(theMessage, pPayload[0]); break;
			}
		}
		
		break;



	case		MANDOLIN_MSG_ADVERTIZE_HARDWARE_INFO:
		if (theMessage->transport & MANDOLIN_TRANSPORT_REPLY )
		{
		}
		else
		{
			pReplyPayload[0] = MANDOLIN_ADVERTIZE_CONTINUE;
			mReplyMsg.length = 1;
			TryToSendMandolinMessage(&mReplyMsg,false);
		}
		break;


	case MANDOLIN_MSG_GET_SYSTEM_STATUS:
		if (theMessage->transport & MANDOLIN_TRANSPORT_REPLY )
		{
		   	DecodeGetSystemStatusReply(theMessage,theLength);
		}
		else
		{
		   	DecodeGetSystemStatus(theMessage,theLength);
		}
		break;

    case MANDOLIN_MSG_GET_HARDWARE_INFO:
		if (theMessage->transport & MANDOLIN_TRANSPORT_REPLY )
		{
			uint32 uiModel;
			uint32 uiBrand;
			char pDest[100];

			intfloat j;
			j.ui = pPayload[0];	// get brand and model
			uiBrand = (j.ui >> 28) & 0x0f;
			uiModel = (j.ui>>16)&0x0fff;

			// OLY has a bug in it.
			//switch (m_nProductType)
			//{
			//	case ePRODUCT_OLYSPEAKER:
			//		uiBrand = (j.ui >> 24) & 0x0ff;
			//		uiModel = (j.ui>>16)&0x0ff;
			//		break;
			//}


			SetBrandModel(uiBrand, uiModel);
		//		intfloat j;
		//		j.ui = pPayload[0];	// get brand and model
		//		switch((j.ui >> 28) & 0x0f)
		//		{
		//		case LOUD_BRAND_MACKIE:
		//			switch ((j.ui>>16)&0x0fff)
		//			{
		//			case MACKIE_MODEL_GENERIC:  break;
		//			case MACKIE_MODEL_DL1608:	break;
		//			case MACKIE_MODEL_DL806:	 break;
		//			default: break;

		//			}
		//			break;
		//		case LOUD_BRAND_EAW: 
		//			switch ((j.ui >> 16)&0x0fff)
		//			{
		//			case EAW_MODEL_GENERIC: break;
		//			case EAW_MODEL_DX1208:	break;
		//			case EAW_MODEL_ANYA:	OnBnClickedProducttypeAnya();break;
		//			case EAW_MODEL_OTTO:	OnBnClickedProducttypeOtto();break;
		//			case EAW_MODEL_ANNA:	OnBnClickedProducttypeAnna();break;
		//			default:  break;
		//			}
		//			break;
		//		default:
		//			break;

		//		}
		}
		else
		{
			// for the connection handshake.
			pReplyPayload[0] = 0;
			pReplyPayload[1] = 0;
			mReplyMsg.length = 2;
			TryToSendMandolinMessage(&mReplyMsg,false);

		}
		break;


    case MANDOLIN_MSG_GET_SOFTWARE_INFO:
		if (theMessage->transport & MANDOLIN_TRANSPORT_REPLY )
		{
		   	DecodeGetSoftwareInfoReply(theMessage,theLength);
			
		}
		else
		{
		}
		break;

	case		MANDOLIN_MSG_TEST:
			m_dlgTestManu.ProcessMessageTest(theMessage,theLength);
		break;

	case	MANDOLIN_MSG_FILE_OPEN:
		//if (theMessage->transport & MANDOLIN_TRANSPORT_REPLY )
		//{
		//	if (pPayload[0] == 1)
		//	{
		//		m_nHALFileId = pPayload[1];

		//		if (m_dlgUpgrade.m_bUpgrade)
		//		{
		//			m_dlgUpgrade.OnSetFileHandle(m_nHALFileId);
		//		}
		//	}
		//	else
		//	{
		//		m_nHALFileId = -1;

		//		if (m_dlgUpgrade.m_bUpgrade)
		//		{
		//			m_dlgUpgrade.OnSetFileHandle(m_nHALFileId);
		//			m_dlgUpgrade.OnSetStatus("File Open failed");
		//		}

		//	}
		//}
		//else
		//{
		//	
		//}
		break;

	case MANDOLIN_MSG_POST_FILE:
		//if (theMessage->transport & MANDOLIN_TRANSPORT_REPLY )
		//{
		//	KillTimer(MSGSEND_TIMERID);
		//	if (m_bDownloading)
		//	{
		//		// reply.. send the next one
		//		SetTimer(MSGSEND_TIMERID,1,NULL);
		//	}
		//}
		//else
		//{
		//	
		//}
		break;


	case MANDOLIN_MSG_GET_INFO:
		if (theMessage->transport & MANDOLIN_TRANSPORT_REPLY )
		{
			// 0 sub id
			// 1 file id
			// 2 file size
			// 3 status (0=fail, 1=success (with next word CRC), 2=busy (with next word percent)
			int nSubId = pPayload[0];
			int nFileId = pPayload[1];
			int nFileSize = pPayload[2];
			int nStatus = pPayload[3];
			int nPercent = pPayload[4];

			switch(nSubId)
			{	

			case	MANDOLIN_GI_FILE:	
				switch(nStatus)
				{
				case MANDOLIN_PROGRESS_FAIL:
	/*				KillTimer(UPGRADE_TIMERID);
					if (m_dlgUpgrade.m_bUpgrade)
					{
						m_dlgUpgrade.OnSetStatus("Get Info Fail");
					}*/
					break;
				case MANDOLIN_PROGRESS_SUCCESS:
					//KillTimer(UPGRADE_TIMERID);
					//if (m_dlgUpgrade.m_bUpgrade)
					//{
					//	KillTimer(UPGRADE_TIMERID);
					//	m_dlgUpgrade.OnSetDeviceCRC(pPayload[4]);
					//	m_dlgUpgrade.OnSetUpgradeProgress(100);


					//}
					break;

				case MANDOLIN_PROGRESS_BUSY:
	//				if (m_dlgUpgrade.m_bUpgrade)
	//				{
	//					m_dlgUpgrade.OnSetUpgradeProgress(pPayload[4]);
	//				}
					break;


				}
				break;
			case	MANDOLIN_GI_LIST:	break;
			case	MANDOLIN_GI_SNAPSHOT: break;
			case	MANDOLIN_GI_SHOW: break;

			}
		}
		else
		{
				// not reply			
		}

		break;

	case MANDOLIN_MSG_SET_APPLICATION_PARAMETER:
		if (theMessage->transport & MANDOLIN_TRANSPORT_REPLY )
		{
			ProcessMessageParameterSetReply(theMessage, theLength);
			// Ack on the Set
			// what if NACK
			// what if timeout
			//switch(MANDOLIN_EDIT_TARGET(pPayload[1]))
			//{
			//case eTARGET_CURRENT_STATE:
			//	DecodeSetParameterReply(theMessage, theLength);
			//	break;
			//case eTARGET_METERS:
			//	DecodeSetMetersReply(theMessage,theLength);
			//	break;
			//default:
			//	break;
			//}

		}
		else
		{
			if (pPayload == 0)
			{
				return;	// something wrong
			}
			ProcessMessageParameterSet(theMessage, theLength);

			theParam = pPayload[0];
			//DecodeSetParameter(theMessage,theLength);
			
		}

		break;


	case MANDOLIN_MSG_GET_APPLICATION_PARAMETER:
		theParam = pPayload[0];
		if (theMessage->transport & MANDOLIN_TRANSPORT_REPLY )
		{
			ProcessMessageParameterGetReply(theMessage, theLength);

			//DecodeGetParameterReply(theMessage, theLength);

			//switch(MANDOLIN_EDIT_TARGET(pPayload[1]))
			//{
			//case eTARGET_CURRENT_STATE:
			//	//DecodeGetParameterReply(theMessage, theLength);
			//	break;
			//case eTARGET_METERS:
			//	//DecodeGetMetersReply(theMessage,theLength);
			//	break;
			//default:
			//	break;
			//}

		}
		else
		{
			//switch(MANDOLIN_EDIT_TARGET(pPayload[1]))
			//{
			//case eTARGET_CURRENT_STATE:
			//	// DecodeGetParameter(theMessage,theLength);
			//	break;
			//case eTARGET_METERS:
			//	//DecodeGetMeters(theMessage,theLength);
			//	break;
			//default:
			//	break;
			//}

			
		}

		break;


	case MANDOLIN_MSG_SET_APPLICATION_STRING:
		theParam = pPayload[0];
		if (theMessage->transport & MANDOLIN_TRANSPORT_REPLY )
		{

		}
		else
		{
				ProcessMessageParameterStringSet(theMessage, theLength);
		}
		break;
	
	default:
		int i = 0;
		i += 0;
		break;

	}
}
bool CRUglyDlg::IsTargetFIR(int nProductType,int nTarget)
{
	switch (nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		return m_ProductOLY.IsTargetFIR(nTarget);

	case ePRODUCT_REDLINESPEAKER1:
		return m_ProductREDLINE.IsTargetFIR(nTarget);

	}
	return false;
}
bool CRUglyDlg::IsTargetMeters(int nProductType,int nTarget)
{
	switch (nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		return m_ProductOLY.IsTargetMeters(nTarget);

	case ePRODUCT_REDLINESPEAKER1:
		return m_ProductREDLINE.IsTargetMeters(nTarget);

	}
	return false;
}
int CRUglyDlg::GetMetersTarget(int nProductType)
{
	switch (nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		return m_ProductOLY.GetMetersTarget();

	case ePRODUCT_REDLINESPEAKER1:
		return m_ProductREDLINE.GetMetersTarget();
	}
	return 0;
}


void	CRUglyDlg::SetParameterList(int nListNum,int nId, int nNumPIDs, int nTarget, unsigned int* pList)
{
	int i = 0;
	if ((nListNum > 1) || (nListNum < 0)) return;
	
	m_nParameterListId[nListNum] = nId;
	if (nNumPIDs> PARAMETERLIST_MAX) 
		nNumPIDs= PARAMETERLIST_MAX;
	m_nParameterListNum[nListNum] = nNumPIDs;
	m_nParameterListTarget[nListNum] = nTarget;
	while((i<nNumPIDs) && pList)
	{
		m_nParameterList[nListNum][i] = pList[i];
		i++;
	}
}
void	CRUglyDlg::ProcessMessageParameterSetReply(mandolin_message* pInMsg, int theLength)
{
	bool bIsReply;

	bIsReply = IS_MANDOLIN_REPLY(pInMsg->transport);
	if (!bIsReply) return;

	if (m_dlgParameterTest.m_bTestInProgress)
	{
		m_dlgParameterTest.ProcessMessageParameterSetReply(pInMsg, theLength);
	}

}

void	CRUglyDlg::ProcessMessageParameterSet(mandolin_message* pInMsg, int theLength)
{

    uint32  flags;
    uint32 *in_ptr = (uint32 *) pInMsg->payload;
    
    bool valid_edit = false;
    

    int nParameterNumber;
	int k;
	int nDirection;
	int nNumOfParameters;

	int i;
	
	tParameterDesc* pParameterDesc;
	int nScaleType;
	int nTarget;
	void** pAddress;
	bool bUseParameterList = false;
	unsigned int *pParameterList;
	int nParameterListSize;
	int nInstance;

	bool bMultiParameter = false;

	intfloat j;
	bool bIsReply;

	bIsReply = IS_MANDOLIN_REPLY(pInMsg->transport);

	if (bIsReply) return;

  
    nParameterNumber = in_ptr[0];
	flags = in_ptr[1];
	nTarget = MANDOLIN_EDIT_TARGET(flags);
	nInstance = MANDOLIN_WILD_MAP(flags);

	if (nInstance == 2)
	{
		nInstance = 0;
	}
	if (MANDOLIN_WILD_TYPE(flags) == MANDOLIN_TYPE_LIST)
	{
		bUseParameterList = true;
		pParameterList = 0;
		// find the parameter list
		for(i=0;i<2;i++)
		{
			if ((nParameterNumber == m_nParameterListId[i])  && (m_nParameterListTarget[i] == nTarget))
			{
				pParameterList = m_nParameterList[i];
				nParameterListSize = m_nParameterListNum[i];
				break;
			}
		}
		if (!pParameterList)
		{
			return;
		}

		nDirection = MANDOLIN_DIRECTION_NEXT;
		nNumOfParameters = pInMsg->length-2;

	}
	else
	{
		nDirection = MANDOLIN_WILD_DIRECTION(flags);
		nNumOfParameters = pInMsg->length-2;
	}


	// --------------------------------
	//  checks to see if Multiparameter
	// --------------------------------
	if ( MANDOLIN_WILD_TYPE(flags) == MANDOLIN_TYPE_MULTIPARAMETER)
	{
		bMultiParameter = true;

		//return	MANDOLIN_handle_SET_MULTIPARAMETER(in);
	}




	for(k=0;k<nNumOfParameters;k++)
	{

		
		j.ui = in_ptr[k+2];

		if ((k & 0x01) && (bMultiParameter))
		{
			// 0=value, 1=nextid, 2=value, 3=next id
			nParameterNumber = in_ptr[k+2];

		}
		else
		{
			if (bUseParameterList)
			{
				/*
				if ((k>nParameterListSize) && (nTarget==eTARGET_METERS))
				{
					// get the rms
					nParameterNumber = eMID_FENCE+k-nParameterListSize;
				}
				else 
				*/
					nParameterNumber = pParameterList[k];
			}
			{

				// --------------------------------------------------------
				// decode the Parameter
				// --------------------------------------------------------

				pAddress = GetParamAddress(nTarget,nParameterNumber,nInstance);

				j.ui = in_ptr[k+2];
				if (pAddress)
				{
					*pAddress = j.vp;
				}

				if (IsTargetMeters(m_nProductType,nTarget))
				{
					valid_edit = true;
					ShowMeter(nTarget, nParameterNumber,j.f);
				}
				else
				{
					pParameterDesc = GetParameterDesc(m_nProductType, nTarget,nParameterNumber);
					if (!pParameterDesc)
					{
						//valid_edit = false;
						break;

					}
					valid_edit = true;

					nScaleType = CalcScaleType(pParameterDesc); 
					ShowParameter(nTarget, nParameterNumber, false);
				}


			}
			// --------------------------------------------------------
			// next parameter
			// --------------------------------------------------------


			if (nDirection == MANDOLIN_DIRECTION_PREVIOUS)
			{
				nParameterNumber--;
			}
			else
			{
				nParameterNumber++;
			}
		}
	}

	UpdateData(FALSE);


//	if (nTarget==eTARGET_METERS)
//	{
//
//#ifdef TEMPIMP_FROM_METERS
//		SetandShowTemperatureImpedanceValues();
//#endif
//
//
//
//		if (m_dlgMeterValues.IsWindowVisible())
//			m_dlgMeterValues.ShowValues();
//	}
//
	//DecodeGetParameterReply(theMessage,theLength);   // same format
	// Send back a reply

//BUG fix for now
	pInMsg->transport &= ~MANDOLIN_TRANSPORT_NOACK_MASK;	// there is a bug with the sync
	Send_reply_ACK(pInMsg);

}


void CRUglyDlg::ProcessMessageParameterGetReplyMeter(mandolin_message* pInMsg, int theLength)
{
	
	uint32  flags;
    uint32 *in_ptr = (uint32 *) pInMsg->payload;
    
    int nParameterNumber;
	int k;
	int nTarget;
	int nDirection;
	int nNumOfParameters;
		
	bool bMultiParameter = false;
   	bool bIsReply;
	intfloat j;
	
	bIsReply = IS_MANDOLIN_REPLY(pInMsg->transport);

	if (!bIsReply) return;
   
    nParameterNumber = in_ptr[0];
	flags = in_ptr[1];

	nDirection = MANDOLIN_WILD_DIRECTION(flags);
	nTarget = MANDOLIN_EDIT_TARGET(flags);

	nNumOfParameters = pInMsg->length-2;

	if (MANDOLIN_WILD_TYPE(flags) == MANDOLIN_TYPE_MULTIPARAMETER)
	{
		bMultiParameter = true;
	}


	if (nTarget != GetMetersTarget(m_nProductType))
	{
		return;
	}



	for(k=0;k<nNumOfParameters;k++)
	{

		if ((k & 0x01) && (bMultiParameter))
		{
			// 0=value, 1=nextid, 2=value, 3=next id
			nParameterNumber = in_ptr[k+2];

		}
		else
		{



			j.vp = GetParamAddress(nTarget, nParameterNumber);
			if (j.vp)
			{
				j.ui = in_ptr[k+2];

				ShowMeter(nTarget, nParameterNumber, j.f);

			}
	
			// --------------------------------------------------------
			// next parameter
			// --------------------------------------------------------


			if (nDirection == MANDOLIN_DIRECTION_PREVIOUS)
			{
				nParameterNumber--;
			}
			else
			{
				nParameterNumber++;
			}
		}
	}
}


void	CRUglyDlg::ProcessMessageParameterGetReply(mandolin_message* pInMsg, int theLength)
{
   uint32  flags;
    uint32 *in_ptr = (uint32 *) pInMsg->payload;
    
    bool valid_edit = false;
    

    int nParameterNumber;
	int k;
	int nDirection;
	int nNumOfParameters;
		
	tParameterDesc* pParameterDesc;
	int nScaleType;
	int nTarget;
	void** pAddress;
	int nInstance;


	bool bMultiParameter = false;


	intfloat j;
   	bool bIsReply;

	bIsReply = IS_MANDOLIN_REPLY(pInMsg->transport);

	if (!bIsReply) return;


	if (m_dlgParameterTest.m_bTestInProgress)
	{
		m_dlgParameterTest.ProcessMessageParameterGetReply(pInMsg, theLength);
	}
    nParameterNumber = in_ptr[0];
	flags = in_ptr[1];

	nDirection = MANDOLIN_WILD_DIRECTION(flags);
	nTarget = MANDOLIN_EDIT_TARGET(flags);
	nInstance = MANDOLIN_WILD_MAP(flags);

	nNumOfParameters = pInMsg->length-2;

	if (MANDOLIN_WILD_TYPE(flags) == MANDOLIN_TYPE_MULTIPARAMETER)
	{
		bMultiParameter = true;
	}


	if ((m_dlgZImpedance.m_bInProgress) && (nTarget == GetMetersTarget(m_nProductType)))
	{
		m_dlgZImpedance.OnMeterDecode(pInMsg);
	}
 
	//if ((m_dlgOlyImpedance.m_bInProgress) && (nTarget == eTARGET_STATUS) &&  (m_nProductType == ePRODUCT_OLYSPEAKER))
	if ((nTarget == eTARGET_STATUS) &&  (m_nProductType == ePRODUCT_OLYSPEAKER))
	{
		m_dlgOlyImpedance.OnStatusDecode(pInMsg);
	}

	if (nTarget == GetMetersTarget(m_nProductType))
	{
		return ProcessMessageParameterGetReplyMeter(pInMsg,theLength);
	}
 

	for(k=0;k<nNumOfParameters;k++)
	{

		if ((k & 0x01) && (bMultiParameter))
		{
			// 0=value, 1=nextid, 2=value, 3=next id
			nParameterNumber = in_ptr[k+2];

		}
		else
		{

			pParameterDesc = GetParameterDesc(m_nProductType, nTarget,nParameterNumber);
			if (!pParameterDesc)
			{
				//valid_edit = false;
				break;

			}

			// --------------------------------------------------------
			// decode the Parameter
			// --------------------------------------------------------

			j.ui = in_ptr[k+2];


			valid_edit = true;




			nScaleType = CalcScaleType(pParameterDesc); 


			//GetTypeEffectParam(nParameterNumber, theChan, theAux, theName, theAbr,
			//			theMax, theMin, theDefault, theNumericType, theScaleType);



			// --------------------------------------------------              	                                      
			//int theEditParam;

			//int nLength;
			//int i;
			//char tempBuff[200];
			//char * next;

			//nLength = m_strSendParam.GetLength();
			//m_strSendParam.Remove(' ');
			//i = 0;
			//if (nLength>0)	tempBuff[i++] = m_strSendParam.GetAt(0);
			//if (nLength > 1) tempBuff[i++] = m_strSendParam.GetAt(1);
			//tempBuff[i++] = 0;
			//theEditParam = strtol(tempBuff, &next, 16);

			//if (theEditParam == nParameterNumber)
			//{
			//	switch(theNumericType)
			//	{
			//	case NUMERICTYPE_FLOAT: 
			//		m_strParamValue.Format("%f",j.f); 
			//		break;
			//	case NUMERICTYPE_BOOL:
			//	case NUMERICTYPE_INT:
			//	case NUMERICTYPE_UINT:
			//	default:
			//		m_strParamValue.Format("%d",j.i);
			//		break;

			//	}
			//	SetDlgItemText(IDC_PARAMEDITVALUE,m_strParamValue);
			//}
			pAddress = GetParamAddress(nTarget,nParameterNumber,nInstance);

			if (pAddress)
			{
				*pAddress = j.vp;
			}

			ShowParameter(nTarget, nParameterNumber, false);

			// --------------------------------------------------------
			// next parameter
			// --------------------------------------------------------


			if (nDirection == MANDOLIN_DIRECTION_PREVIOUS)
			{
				nParameterNumber--;
			}
			else
			{
				nParameterNumber++;
			}
		}
	}

	UpdateData(FALSE);


//	return true;
}
void	CRUglyDlg::ProcessMessageParameterStringSet(mandolin_message* pInMsg, int theLength)
{
	//Payload:	String ID
	//		Parameter Flags
	//		String Data
	//Reply:	ACK.
	//
	//(SET MULTIPARAMETER)
	//
	//Payload:    StringID1
	//            Parameter Flags
	//            String Data1
	//            StringID2, String Data2,
	//            StringID3, String Data3,
	//            …
	//            StringIDN, String DataN
	//Reply:   	ACK.


	bool bIsReply = IS_MANDOLIN_REPLY(pInMsg->transport);
	if (bIsReply) return;


	//pInMsg->transport &= ~MANDOLIN_TRANSPORT_NOACK_MASK;	// just send back an ACK
	Send_reply_ACK(pInMsg);
}


//------------------------------------------------------------------------
// &&& Messages Decoding
//------------------------------------------------------------------------
void	CRUglyDlg::DecodeGetSystemStatusReply(mandolin_message* theMessage, int theLength)
{
	uint32 *in_ptr = (uint32 *) theMessage->payload;
	uint32 uiQueryNum;
	uint32 uiFlags;

	if ((theLength <= 0) || (in_ptr == 0))
	{
		return;
	}

	uiQueryNum = in_ptr[0] & 0x0ffff;
	uiFlags = (in_ptr[0]>>16) & 0x0ffff;


	switch(uiQueryNum)
	{

	case    MANDOLIN_SS_BOOT_CODE:
		//puiValue[nPayloadLength++] = MAIN_uiBootCode;
		//TryToSendMandolinMessage(&mMsg, false);
		break;

	case	MANDOLIN_SS_MAX_MSG_WORDS:
		m_nMandolinMaxMsgSize = in_ptr[1];

		if ((m_bMandolinConnectStateStart) &&	(m_nMandolinConnectStateState == MANDOLIN_CONNECT_SS_MAX_MSG_WORDS_WAITINGACK))
		{
			MandolinConnectStateIncrementState();
			MandolinConnectStateDoState();
		}
		// On sync?
		break;

	case    MANDOLIN_SS_SYSTEM_LABEL:
		//TryToSendMandolinMessage(&mMsg, false);
		break;

	case    MANDOLIN_SS_SYSTEM_IP_ADDRESS:

		break;


	case MANDOLIN_MSG_GET_SYSTEM_STATUS_CRC:  // CRC



		break;




	}

}


void	CRUglyDlg::DecodeGetSystemStatus(mandolin_message* theMessage, int theLength)
{
    uint32 *in_ptr = (uint32 *) theMessage->payload;
	uint32 uiQueryNum;


	mandolin_message mMsg;
	int pPayload[100];
	//int i;

	mMsg.id =  theMessage->id;
	mMsg.length = 1;
    mMsg.transport = theMessage->transport | MANDOLIN_TRANSPORT_REPLY;
    mMsg.sequence = theMessage->sequence;
	mMsg.payload = (uint8*) pPayload;


	if ((theLength <= 0) || (in_ptr == 0))
	{
		return;
	}

	uiQueryNum = in_ptr[0];

	pPayload[0] = in_ptr[0];


	switch(uiQueryNum)
	{

	case    MANDOLIN_SS_BOOT_CODE:
		//puiValue[nPayloadLength++] = MAIN_uiBootCode;
		TryToSendMandolinMessage(&mMsg, false);
		break;

	case	MANDOLIN_SS_MAX_MSG_WORDS:

		pPayload[1] = 1024*2;
		mMsg.length = 2;
		TryToSendMandolinMessage(&mMsg, false);

		if ((m_bMandolinConnectStateStart) &&	(m_nMandolinConnectStateState == MANDOLIN_CONNECT_SS_MAX_MSG_WORDS_WAIT))
		{
			MandolinConnectStateIncrementState();
			MandolinConnectStateDoState();
		}


		break;

	case    MANDOLIN_SS_SYSTEM_LABEL:
		TryToSendMandolinMessage(&mMsg, false);
		break;

	case    MANDOLIN_SS_SYSTEM_IP_ADDRESS:
		TryToSendMandolinMessage(&mMsg, false);
//		if (ZM_ValidateConnection())
//		{
//			g_nLocalIPAddress = ZM_GetIPAddress();
//		}
//		else
//		{
//			SocketDriver_GetIPAddress(g_szLocalIPAddress, &g_nLocalIPAddress); //&&&& not sure why I have to call this again
//		}
//		// uiSSPayload[nPayloadLength++] = g_nLocalIPAddress;		// network order
//		// host-ordered
//		puiValue[nPayloadLength++] = (((g_nLocalIPAddress >> 24) & 0x0ff) << 0) + 
//										(((g_nLocalIPAddress >> 16) & 0x0ff) << 8) +
//										(((g_nLocalIPAddress >>  8) & 0x0ff) << 16) +
//										(((g_nLocalIPAddress >>  0) & 0x0ff) << 24);
		break;


	case MANDOLIN_MSG_GET_SYSTEM_STATUS_CRC:  // CRC

//		uiParamterBufferCRCBlackfin = in_ptr[1];
//		// calculate the vpParameterBuffer CRC
//
//		// -----------------------
//		// debug
//		// -----------------------
//		vpParameterBuffer[0] = 0;
//  
//		for(int i=1;i<(PARAMETER_MAX-1);i++)		// setting 1,2,3,4 etc missed the last value
//		{
//			if (vpParameterBuffer[i] != (void*)(((((i-1)*4+3)&0x0ff)<<24)+((((i-1)*4+2)&0x0ff)<<16)+((((i-1)*4+1)&0x0ff)<<8)+((((i-1)*4+0)&0x0ff)<<0)))
//			{
//				break;
//			}
//		}
//
//		// -----------------------
//		// end of debug
//		// -----------------------
//
//
//		pPayload[1] = CurrentStateCalcCRC();
//		mMsg.length = 2;
//		TryToSendMandolinMessage(&mMsg, false);


		break;

//	case MANDOLIN_MSG_GET_SYSTEM_STATUS_SENDSTATETIME:  
//		puiValue[nPayloadLength++] = gSharcPortDesc.SendState_DiffMSec;
//		puiValue[nPayloadLength++] = gIPADPortDesc.SendState_DiffMSec;
//		for(nSocket=0;nSocket<SOCKET_CONNECTION_MAX;nSocket++)
//		{
//			puiValue[nPayloadLength++] = gSocketPortDesc[nSocket].SendState_DiffMSec;
//		}
//		TryToSendMandolinMessage(&mMsg, false);
//		break;




	// still need these
//	case MANDOLIN_MSG_GET_SYSTEM_STATUS_FIFODEPTH:
//
//		puiValue = (uint32*) mReplyMsgPayload;
//		puiValue[0] = uiSSPayload[0];
//		k = 1;
//
//		puiValue[k++] = (gSharcPortDesc.nType<<8) + gSharcPortDesc.nId;
//#if(DEBUG_FIFO_BUFFER)
//		puiValue[k++] = MANDOLIN_FIFO_high_watermark(&gSharcPortDesc.bfpRxFifo);
//		puiValue[k++] = MANDOLIN_FIFO_high_watermark(&gSharcPortDesc.bfpTxFifo);
//		puiValue[k++] = MANDOLIN_FIFO_high_watermark(&gSharcPortDesc.bfpTxReplyFifo);
//#else
//		puiValue[k++] = MANDOLIN_FIFO_bytes_full(&gSharcPortDesc.bfpRxFifo);     
//		puiValue[k++] = MANDOLIN_FIFO_bytes_full(&gSharcPortDesc.bfpTxFifo);     
//		puiValue[k++] = MANDOLIN_FIFO_bytes_full(&gSharcPortDesc.bfpTxReplyFifo);
//
//#endif

//
//		puiValue[k++] = (gIPADPortDesc.nType<<8) + gIPADPortDesc.nId;
//#if(DEBUG_FIFO_BUFFER)
//		puiValue[k++] = MANDOLIN_FIFO_high_watermark(&gIPADPortDesc.bfpRxFifo);
//		puiValue[k++] = MANDOLIN_FIFO_high_watermark(&gIPADPortDesc.bfpTxFifo);
//		puiValue[k++] = MANDOLIN_FIFO_high_watermark(&gIPADPortDesc.bfpTxReplyFifo);
//#else
//		puiValue[k++] = MANDOLIN_FIFO_bytes_full(&gIPADPortDesc.bfpRxFifo);     
//		puiValue[k++] = MANDOLIN_FIFO_bytes_full(&gIPADPortDesc.bfpTxFifo);     
//		puiValue[k++] = MANDOLIN_FIFO_bytes_full(&gIPADPortDesc.bfpTxReplyFifo);
//
//#endif
//
//
//		for(i=0;i<SOCKET_CONNECTION_MAX;i++)
//		{
//
//			puiValue[k++] = (gSocketPortDesc[i].nType<<8) + gSocketPortDesc[i].nId;
//#if(DEBUG_FIFO_BUFFER)
//			puiValue[k++] = MANDOLIN_FIFO_high_watermark(&gSocketPortDesc[i].bfpRxFifo);
//			puiValue[k++] = MANDOLIN_FIFO_high_watermark(&gSocketPortDesc[i].bfpTxFifo);
//			puiValue[k++] = MANDOLIN_FIFO_high_watermark(&gSocketPortDesc[i].bfpTxReplyFifo);
//#else
//			puiValue[k++] = MANDOLIN_FIFO_bytes_full(&gSocketPortDesc[i].bfpRxFifo);     
//			puiValue[k++] = MANDOLIN_FIFO_bytes_full(&gSocketPortDesc[i].bfpTxFifo);     
//			puiValue[k++] = MANDOLIN_FIFO_bytes_full(&gSocketPortDesc[i].bfpTxReplyFifo);
//
//#endif
//		}
//		nPayloadLength = k;
//
//
//		TryToSendMandolinMessage(&mMsg, false);
//		break;

	}



}

void	CRUglyDlg::DecodeGetSoftwareInfoReply(mandolin_message* theMessage, int theLength)
{
    uint32 *in_ptr = (uint32 *) theMessage->payload;
	if ((theLength <= 0) || (in_ptr == 0))
	{
		return;
	}

	m_uiMandolinVersion = in_ptr[0];
	if (in_ptr[1] == 2)
	{
		m_uiK64Version = in_ptr[2];
	}
	ShowSofwareInfo();

}


//-------------------------------------------------

void CRUglyDlg::OnBnClickedPing()
{
	// TODO: Add your control notification handler code here
	m_msgTx.payload = m_msgTxPayload;
	m_MandolinComm.CreatePingMsg(&m_msgTx, m_nTxSequence);
	TryToSendMandolinMessage(&m_msgTx, true);


	uint16	    checksum =          m_msgTx.sync      +          m_msgTx.sequence +
							((m_msgTx.length >> 8) & 0x0ff)   + (m_msgTx.length & 0x0ff) +
							m_msgTx.transport +          m_msgTx.id;
	uint16 checksum2 = ~checksum;

	checksum +=	((m_msgTx.checksum >> 8) & 0x0ff)   + (m_msgTx.checksum & 0x0ff);
	checksum++;
 					

}

void**  CRUglyDlg::GetParamAddress(int nTarget, int nParam, int nInstance)
{
	switch (m_nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		return m_ProductOLY.GetParamAddress(nTarget, nParam, nInstance);
	case ePRODUCT_REDLINESPEAKER1:
		return m_ProductREDLINE.GetParamAddress(nTarget, nParam, nInstance);
	}
	return 0;
}

void CRUglyDlg::GetParameterValueString(char* pcValue,tParameterDesc*  pParameterDesc, int nTarget, int nParam)
{
	intfloat j;
	void** pParameterAddress = GetParamAddress(nTarget, nParam);

	pcValue[0] = 0;
	if(!pParameterAddress) return;

	j.vp = *pParameterAddress;


	pcValue[0] = 0;
	switch(pParameterDesc->format)
	{
	case eFORMAT_BOOL:
		sprintf(pcValue,"%d\n", j.ui);
		break;
	case eFORMAT_UNSIGNED:
		sprintf(pcValue,"%d\n",  j.ui);
		break;
	case eFORMAT_SIGNED:
		sprintf(pcValue,"%d\n",  j.i);
		break;
	case eFORMAT_FLOAT:
		sprintf(pcValue,"%8.2f\n",  j.f);
		break;
	}
}
int  CRUglyDlg::GetParameterScaleType(tParameterDesc* pParameterDesc)
{
	if (!pParameterDesc) 
		return SCALETYPE_LINEAR;

	return SCALETYPE_LINEAR;

}

intfloat CRUglyDlg::ConvertString2Value(CString theString, int& nType)
{
	// returns nType 0=int, 1 = float
	char *next;
	int nLength;
	theString.MakeUpper();
	intfloat j;
	bool bInHex = false;


	nLength = theString.GetLength();
	int nDecimalPoint =  theString.Find(".",0);
	j.i = 0;
	nType = 0;


	if (nLength == 0)
		return j;

	if (nLength > 2)
	{
		if ( (theString.GetAt(0) == '0') &&
		     (theString.GetAt(1) == 'X'))
		{
			bInHex = true;
			if (nLength == 2) return j;
		}
	}
	if (-1 == nDecimalPoint)
	{
		// Integer
		if (bInHex)
		{
			j.i = strtoul(theString.GetBuffer(2),&next,16);
			theString.ReleaseBuffer();
		}	
		else
		{
			j.i = strtoul(theString.GetBuffer(0),&next,10);
			theString.ReleaseBuffer();
		}
		return j;
	}
	else
	{

		nType = 1;
		int nRetCode = sscanf(theString.GetBuffer(0),"%f",&j.f);
		theString.ReleaseBuffer();
		if ((nRetCode == EOF) || (nRetCode == 0))
		{
			j.f = 0;

		}
		return j;

	}
	return j; // int
}

intfloat CRUglyDlg::GetDlgItemValue(int nIDC, int& nType)
{
	CString theString;
	//char tempBuff[40];

	intfloat j;
	GetDlgItemText(nIDC,theString);

	j = ConvertString2Value(theString,nType);

	return j;

}
tParameterDesc*  CRUglyDlg::ConvertString2ParameterDesc(int nProductType,char* pName,int* nTarget, int* nPID)
{

	switch (nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		return m_ProductOLY.ConvertString2ParameterDesc(pName, nTarget, nPID);

	case ePRODUCT_REDLINESPEAKER1:
		return m_ProductREDLINE.ConvertString2ParameterDesc(pName, nTarget, nPID);

	}
	return 0;
}

int CRUglyDlg::CalcScaleType(tParameterDesc* pParameterDesc)
{
	if (!pParameterDesc)
		return SCALETYPE_LINEAR;
	if (pParameterDesc->paramEnum == ePARAM_FREQ)
			return SCALETYPE_LOG;
	return SCALETYPE_LINEAR;
}

float CRUglyDlg::NormalizeParameterValue(tParameterDesc* pParameterDesc, intfloat theParamValue)
{
	float fNormalizeValue = 0;
	int nValue;
	float theRangeValue = 0;
	intfloat theMax;
	intfloat theMin;

	theMax.f = 1.0;
	theMin.f = 0;

	if (!pParameterDesc) 
		return 0;

	theMax.ui = pParameterDesc->maxValue;
	theMin.ui = pParameterDesc->minValue;

	switch(pParameterDesc->format)
	{
	case eFORMAT_FLOAT:
		theRangeValue = theParamValue.f;
		break;

	case eFORMAT_BOOL:
		theRangeValue = ( (theParamValue.ui - 0.5)*(theMax.ui - theMin.ui) + theMin.ui );
		theMin.f = theMin.ui;
		theMax.f = theMax.ui;
		break;


	case eFORMAT_SIGNED:
		theMin.f = theMin.ui;
		theMax.f = theMax.ui;
		theRangeValue = ( theParamValue.i - 0.5 );
		break;

	case eFORMAT_UNSIGNED:
		theMin.f = theMin.ui;
		theMax.f = theMax.ui;
		theRangeValue = ( theParamValue.ui - 0.5 );
		break;
	}

	switch(CalcScaleType(pParameterDesc))
	{
	case SCALETYPE_LINEAR:
//		fNormalizeValue = (int) ( 1.0 - (1.0 * (theRangeValue - theMin.f)/(theMax.f - theMin.f)) );
		fNormalizeValue = (1.0 * (theRangeValue - theMin.f)/(theMax.f - theMin.f));
		break;
	case SCALETYPE_LOG:
		{
			int theNumSteps;
			theNumSteps = 1024;

			double df;
			if (theMin.f != 0)
			{
				df = ( pow ( (float) 10.0, (float)(log10(theMax.f/theMin.f) / (float) theNumSteps )) );
				//nValue = theNumSteps - ( log10(theRangeValue/theMin.f) / log10(df) );
				fNormalizeValue = ( log10(theRangeValue/theMin.f) / log10(df) )/theNumSteps;

				}
			else
			{
				fNormalizeValue = 0;
			}
		}
		break;
	}

	if (fNormalizeValue > 1.0) 
		fNormalizeValue = 1.0;
	if (fNormalizeValue < 0)
	{
		fNormalizeValue = 0;
	}
	return fNormalizeValue;
}

void	CRUglyDlg::ShowMeter(int nTarget, int nParam, float fValue)
{

	if ((m_dlgMeters.m_bInProgress) || (m_dlgMeters.m_bMeterListInProgress))
	{
		m_dlgMeters.ShowMeter(nTarget, nParam, fValue);
	}
 
}


void 	CRUglyDlg::ShowSofwareInfo()
{
	char pcString[200];

	uint32 uiMajor  = (m_uiK64Version >> 24) & 0x0ff;
	uint32 uiMinor =  (m_uiK64Version >> 16) & 0x0ff;
	uint32 uiSub	= (m_uiK64Version >> 8) & 0x0ff;
	uint32 uiBuild = (m_uiK64Version >> 0) & 0x0ff;


	sprintf(pcString,"%d.%d.%d", ((m_uiMandolinVersion>>24) & 0x0ff), ((m_uiMandolinVersion >> 16) & 0x0ff), m_uiMandolinVersion & 0x0ffff);
	SetDlgItemText(IDC_VERSION_MANDOLIN,pcString);
	sprintf(m_pcFirmwareVersion,"%d.%d.%d.%d", 
			uiMajor, 
			uiMinor, 
			uiSub,	
			uiBuild); 
	SetDlgItemText(IDC_VERSION_FIRMWARE,m_pcFirmwareVersion);

}

void	CRUglyDlg::OnGetSoftwareVersion()
{

	m_msgTx.payload = m_msgTxPayload;
	m_MandolinComm.CreateGetSoftwareInfo(&m_msgTx, m_nTxSequence);
	TryToSendMandolinMessage(&m_msgTx, true);
}
void	CRUglyDlg::OnSendSyncState()
{
	m_msgTx.payload = m_msgTxPayload;
	m_MandolinComm.CreateSyncState(&m_msgTx, m_nTxSequence);
	TryToSendMandolinMessage(&m_msgTx, true);
}
void	CRUglyDlg::OnSendConnect(int nConnectType)
{
	m_msgTx.payload = m_msgTxPayload;
	m_MandolinComm.CreateConnect(&m_msgTx, m_nTxSequence,nConnectType);
	TryToSendMandolinMessage(&m_msgTx, true);
}

void	CRUglyDlg::MandolinConnectStateSetState(int nState)
{
	m_nMandolinConnectStateState = nState;
}

void	CRUglyDlg::MandolinConnectStateIncrementState()
{
	m_nMandolinConnectStateState++;
	if (m_nMandolinConnectStateState >= MANDOLIN_CONNECT_MAX)
	{
		m_nMandolinConnectStateState = MANDOLIN_CONNECT_MAX;
	}


}
void	CRUglyDlg::MandolinConnectStateDoState()
{
	if (!m_bMandolinConnectStateStart) 
		return;

	if (m_nMandolinConnectStateState > MANDOLIN_CONNECT_MAX)
	{
		m_nMandolinConnectStateState = MANDOLIN_CONNECT_MAX;
	}

	switch(m_nMandolinConnectStateState)
	{
	case	MANDOLIN_CONNECT_NONE:
		break;
	case	MANDOLIN_CONNECT_SE_CONNECT_SEND:			// send the Connect msg
		MandolinConnectStateIncrementState();
		OnSendConnect(MANDOLIN_CONNECT_FULL);
		break;
	case	MANDOLIN_CONNECT_SE_CONNECT_WAITINGACK:	// waiting ack from fw
		break;
	case	MANDOLIN_CONNECT_SS_MAX_MSG_WORDS_WAIT:	//	fw will send MAX mesage size
		break;
	case	MANDOLIN_CONNECT_SS_MAX_MSG_WORDS_SEND:	//  send a MAX message size	
		OnGetSoftwareVersion();
		MandolinConnectStateIncrementState();
		OnGetSystemStatusMaxMessageSize();
		break;
	case	MANDOLIN_CONNECT_SS_MAX_MSG_WORDS_WAITINGACK:	//  waitng ack from fw
		break;
	case MANDOLIN_CONNECT_SE_CONNECT_WAIT:
		break;

	case	MANDOLIN_CONNECT_SE_SYNC_CURRENT_STATE_SEND:					//  send a sync command
		MandolinConnectStateIncrementState();
		OnSendSyncState();
		break;
	case	MANDOLIN_CONNECT_SE_SYNC_CURRENT_STATE_WAITINGACK:			// waiting ack
		break;

	case MANDOLIN_CONNECT_MAX:
	default:
		m_bMandolinConnectStateStart = false;
		break;
	}


}

void	CRUglyDlg::OnGetSystemStatusMaxMessageSize()
{

	m_msgTx.payload = m_msgTxPayload;
	m_MandolinComm.CreateGetMaxMessageSize(&m_msgTx, m_nTxSequence);
	TryToSendMandolinMessage(&m_msgTx, true);
}

void	CRUglyDlg::ShowParameter(int nTarget, int nParam, bool bUpdate, bool bAll)
{
	int i;
	tParameterDesc*	pParameterDesc = GetParameterDesc(m_nProductType, nTarget,nParam);
	intfloat j;

	if(!pParameterDesc) return;

	void** pParameterAddress = GetParamAddress(nTarget, nParam);
	if(!pParameterAddress) return;

	j.vp = *pParameterAddress;

	//// --------------------------------------------
	//// update faders
	//// --------------------------------------------
	//for(i=0;i<MAX_FADER;i++)
	//{
	//	if ((nTarget == m_descFader[i].nTarget) && (nParam == m_descFader[i].nParamNum))
	//	{

	//		UpdateVScrollBarValue(nTarget,nParam, j);	// does all that match
	//		break;
	//	}
	//}

	//// --------------------------------------------
	//// update switches
	//// --------------------------------------------
	//for(i=0;i<MAX_SWITCH;i++)
	//{
	//	if ((nTarget == m_descSwitch[i].nTarget) && (nParam == m_descSwitch[i].nParamNum))
	//	{
	//		CButton* pButton = (CButton*) GetDlgItem(m_descSwitch[i].nIDC);
	//		if (pButton)
	//		{
	//			pButton->SetCheck(j.ui != 0);
	//		}
	//	}
	//}



	//// --------------------------------------------
	//// update Meters
	//// --------------------------------------------

	//if (pParameterDesc->format != eFORMAT_FLOAT)
	//{
	//	SetMeter(nTarget,nParam,(float) j.ui);
	//}
	//else
	//{
	//	SetMeter(nTarget,nParam,j.f);
	//}


	//if (pParameterDesc->format != eFORMAT_FLOAT)
	//{
	//	SetCompRed(nTarget,nParam,(float) j.ui);
	//}
	//else
	//{
	//	SetCompRed(nTarget,nParam,j.f);
	//}

	//// combobox
	//if ((nTarget == eTARGET_SNAPSHOT) && (nParam == ePID_ANYA_SNAPSHOT_INPUT1_AUDIOSRC))
	//{
	//	CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_AUDIOSELECTCOMBO);
	//	if (pCombo)
	//	{
	//		pCombo->SetCurSel(j.i);
	//	}
	//}

	// --------------------------------------------
	// update parameter adjust
	// --------------------------------------------

	m_dlgParameterAdjust.ShowParameter(nTarget,nParam, bAll);

	m_dlgLevelAdjust.ShowParameter(nTarget,nParam, bAll);

	m_dlgEQGraph.ShowParameter(nTarget,nParam, bAll);

	m_dlgActivePreset.ShowParameter(nTarget,nParam, bAll);


}

void CRUglyDlg::SetProducttype(int nProduct)
{
	int i;

	m_nProductType = nProduct;

	//SetUpSwitches();
	//SetUpMeters();
	//SetUpVScrollBars();



	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;

	switch (m_nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		//m_ProductOLY.SetCurrentStateToDefaults(); //m_ProductOLY.SetCurrentStateToDefaults();
		break;
	case ePRODUCT_REDLINESPEAKER1:
		//m_ProductREDLINE.SetCurrentStateToDefaults(); //m_ProductOLY.SetCurrentStateToDefaults();
		break;
	}

	m_dlgParameterAdjust.SetProductType(m_nProductType);
	m_dlgLevelAdjust.SetProductType(m_nProductType);
	m_dlgMeters.SetProductType(m_nProductType);
	m_dlgDSPSignalFlow.SetProductType(m_nProductType);
	m_dlgEQGraph.SetProductType(m_nProductType);
	m_dlgParameterTest.SetProductType(m_nProductType);
	m_dlgCreatePresetHeaders.SetProductType(m_nProductType);
	m_dlgTestManu.SetProductType(m_nProductType);

	m_MandolinTextSummary.m_nProductType = m_nProductType;
}


 void CRUglyDlg::ConvertBrandModel2Text(char* pcBrand, char* pcModel, uint32 uiBrand, uint32 uiModel)
 {
	pcBrand[0] = 0;
	pcModel[0] = 0;
	switch(uiBrand)
	{
		case LOUD_BRAND_MACKIE:
			strcat(pcBrand,"Mackie");
			switch(uiModel)
			{
			case MACKIE_MODEL_GENERIC: strcat(pcModel,"GENERIC"); break;
			case MACKIE_MODEL_DL1608:	strcat(pcModel,"DL1608"); break;
			case MACKIE_MODEL_DL806:	strcat(pcModel,"DL806"); break;
			case     MACKIE_MODEL_DL1608_DL806:		strcat(pcModel,"DL1608_DL806:"); break;  // WhiteApp doesn't know...
			case     MACKIE_MODEL_DL3214:			strcat(pcModel,"DL3214"); break;
			case     MACKIE_MODEL_DL32R:			strcat(pcModel,"DL32R"); break;
			case     MACKIE_MODEL_DL24XX:			strcat(pcModel,"DL24XX"); break;
			case    MACKIE_MODEL_DL24XX_RACK:		strcat(pcModel,"DL24XX_RACK"); break;
			case    MACKIE_MODEL_DC16:				strcat(pcModel,"DC16"); break;
			/*  Loudspeakers */
			case    MACKIE_MODEL_R8_AA:				strcat(pcModel,"R8_AA"); break;
			case    MACKIE_MODEL_R8_64:				strcat(pcModel,"R8_64"); break;
			case    MACKIE_MODEL_R8_96:				strcat(pcModel,"R8_96"); break;
			case    MACKIE_MODEL_R12_64:			strcat(pcModel,"R12_64"); break;
			case    MACKIE_MODEL_R12_96:			strcat(pcModel,"R12_96"); break;
			case    MACKIE_MODEL_R12_SM:			strcat(pcModel,"R12_SM"); break;
			case    MACKIE_MODEL_R12_SW:			strcat(pcModel,"R12_SW"); break;
			case	MACKIE_MODEL_R18_SW:			strcat(pcModel,"R18_SW"); break;
			/*  Software Products */
			case     MACKIE_MODEL_MASTER_FADER:		strcat(pcModel,"MASTER_FADER"); break;
			case     MACKIE_MODEL_MY_FADER:			strcat(pcModel,"MY_FADER"); break;
			case     MACKIE_MODEL_MASTER_RIG:		strcat(pcModel,"MASTER_RIG"); break;

			default:						strcat(pcModel,"UNKNOWN"); break;
			}
			break;
		case LOUD_BRAND_EAW:
			strcat(pcBrand,"EAW");
			switch(uiModel)
			{
			case EAW_MODEL_GENERIC: strcat(pcModel,"GENERIC"); break;
			case EAW_MODEL_DX1208:	strcat(pcModel,"DX1208"); break;
			case EAW_MODEL_ANYA:	strcat(pcModel,"ANYA"); break;
			case EAW_MODEL_OTTO:	strcat(pcModel,"OTTO"); break;
			case EAW_MODEL_HALARCH: strcat(pcModel,"HALARCH"); break;
			case EAW_MODEL_ANNA:	strcat(pcModel,"ANNA"); break;

			case EAW_MODEL_RSX208L: strcat(pcModel,"RSX208L"); break;  // Radius 8" Articulated Array Module
			case EAW_MODEL_RSX86: strcat(pcModel,"RSX86"); break;   // Radius 8" Full-Range, 60x40 Horn
			case EAW_MODEL_RSX89: strcat(pcModel,"RSX89"); break;   // Radius 8" Full-Range, 90x60 Horn
			case EAW_MODEL_RSX126: strcat(pcModel,"RSX126"); break;   // Radius 12" Full-Range, 60x40 Horn
			case EAW_MODEL_RSX129: strcat(pcModel,"RSX129"); break;   // Radius 12" Full-Range, 90x60 Horn
			case EAW_MODEL_RSX12M: strcat(pcModel,"RSX12M"); break;   // Radius 12" Stage Monitor
			case EAW_MODEL_RSX12: strcat(pcModel,"RSX12"); break;   // Radius 12" Subwoofer
			case EAW_MODEL_RSX18: strcat(pcModel,"RSX18"); break;   // Radius 18" Subwoofer
			case EAW_MODEL_RSX218: strcat(pcModel, "RSX218"); break;   // Radius 2x18" Subwoofer
			case EAW_MODEL_RSX18F: strcat(pcModel, "RSX18F"); break;   // Radius 18" Subwoofer
			case EAW_MODEL_RSX212L: strcat(pcModel, "RSX212L"); break;   // Radius 2x12" Articulated Array Module
			default:						strcat(pcModel,"UNKNOWN"); break;
			}
			break;

		case LOUD_BRAND_MARTIN:
			strcat(pcBrand,"Martin");
			switch(uiModel)
			{
    		case MARTIN_MODEL_GENERIC:
			    strcat(pcModel,"GENERIC"); break;	
			case MARTIN_MODEL_GENERIC_NU:
			    strcat(pcModel,"GENERIC_NU"); break;
			/*  Loudspeakers */
			case MARTIN_MODEL_CDDL8:
				strcat(pcModel, "CDDL8"); break;
			case MARTIN_MODEL_CDDL8_NU:
				strcat(pcModel,"CDDL8_NU"); break;
			case MARTIN_MODEL_CDDL12:
				strcat(pcModel, "CDDL12"); break;
			case MARTIN_MODEL_CDDL12_NU:
				strcat(pcModel,"CDDL12_NU"); break;
			case MARTIN_MODEL_CDDL15:
				strcat(pcModel, "CDDL15"); break;
			case MARTIN_MODEL_CDDL15_NU:
				strcat(pcModel,"CDDL15_NU"); break;
			case MARTIN_MODEL_CSXL118:
				strcat(pcModel, "CSXL118"); break;
			case MARTIN_MODEL_CSXL118_NU:
				strcat(pcModel,"CSXL118_NU"); break;
			case MARTIN_MODEL_CSXL218:
				strcat(pcModel, "CSXL218"); break;
			case MARTIN_MODEL_CSXL218_NU:
				strcat(pcModel,"CSXL218_NU"); break;
			default:						strcat(pcModel,"UNKNOWN"); break;
			}
			break;

  		default:
			strcat(pcModel,"UNKNOWN"); break;
			break;
	}
 }
 void CRUglyDlg::SetBrandModel(uint32 uiBrand, uint32 uiModel)
 {
	char pcBrand[100];
	char pcModel[100];

	ConvertBrandModel2Text(pcBrand,pcModel, uiBrand,uiModel);

	SetDlgItemText(IDC_MODEL,pcModel);

		
	switch (m_nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		m_ProductOLY.SetBrandModel(uiBrand,uiModel);
		break;
	case ePRODUCT_REDLINESPEAKER1:
		m_ProductREDLINE.SetBrandModel(uiBrand,uiModel);
		//m_ProductREDLINE.SetCurrentStateToDefaults(); //m_ProductOLY.SetCurrentStateToDefaults();
		break;
	}
	m_dlgCreatePresetHeaders.SetBrandModel(uiBrand,uiModel,pcBrand,pcModel);
	m_dlgTestManu.SetBrandModel(uiBrand,uiModel,pcBrand,pcModel);



 }
// -------------------------------------

int  CRUglyDlg::SendParamEdit(int nTarget, int nParam, unsigned int uiValue)
{
	uint32 uiPID[1];
	intfloat jValue[1];

	uiPID[0] = nParam;
	jValue[0].ui = uiValue;

	m_msgTx.payload = m_msgTxPayload;
	m_MandolinComm.CreateParameterEditSet(&m_msgTx,m_nTxSequence,nTarget,uiPID,jValue,1,true);
	TryToSendMandolinMessage(&m_msgTx, true);


	return m_msgTx.sequence;	
}


int CRUglyDlg::SendParamEditMulti(int nTarget, uint32* nParam, intfloat* jValue, int nNumofParams)
{

	if (nNumofParams>100) return 0;
	m_msgTx.payload = m_msgTxPayload;
	m_MandolinComm.CreateParameterEditSet(&m_msgTx,m_nTxSequence,nTarget,nParam,jValue,nNumofParams,true);
	TryToSendMandolinMessage(&m_msgTx, true);

	return m_msgTx.sequence;	
}


int CRUglyDlg::SendParamEditGet(int nTarget,int nParam)
{
	uint32 uiPID[1];
	uiPID[0] = nParam;
	
	m_msgTx.payload = m_msgTxPayload;
	if (!IsTargetFIR(m_nProductType,nTarget))
	{
		m_MandolinComm.CreateParameterEditGetMulti(&m_msgTx,m_nTxSequence,nTarget,uiPID,1);
	}
	else
	{
		m_MandolinComm.CreateParameterEditGetNeighborhood(&m_msgTx,m_nTxSequence,nTarget,uiPID[0],1);
	}
	TryToSendMandolinMessage(&m_msgTx, true);
	return m_msgTx.sequence;
}

int CRUglyDlg::SendParamGetMulti(int nTarget, uint32* nParam, int nNumofParams)
{
	if (nNumofParams>100) return 0;
	m_msgTx.payload = m_msgTxPayload;
	m_MandolinComm.CreateParameterEditGetMulti(&m_msgTx,m_nTxSequence,nTarget,nParam,nNumofParams);
	TryToSendMandolinMessage(&m_msgTx, true);
	return m_msgTx.sequence;
}

void CRUglyDlg::OnBnClickedParameteradjust()
{

	//m_dlgParameterAdjust.SetScroll(0,255);
	m_dlgParameterAdjust.ShowWindow( SW_RESTORE  );
}

void CRUglyDlg::OnBnClickedLeveladust()
{
	// TODO: Add your control notification handler code here
	m_dlgLevelAdjust.ShowWindow( SW_RESTORE  );
}

void CRUglyDlg::OnBnClickedActivepreset()
{
	// TODO: Add your control notification handler code here
	m_dlgActivePreset.ShowWindow( SW_RESTORE  );

}
void CRUglyDlg::OnBnClickedDisplaymessageformatRaw()
{
	// TODO: Add your control notification handler code here
	m_nInputTextBoxFilter = INPUTTEXTBOXFILTER_RAW;
	OnShowInputFilter();
}

void CRUglyDlg::OnBnClickedDisplaymessageformatInterpret()
{
	// TODO: Add your control notification handler code here
	m_nInputTextBoxFilter = INPUTTEXTBOXFILTER_INTERPRET;
	OnShowInputFilter();
}
void CRUglyDlg::OnShowInputFilter()
{
	// TODO: Add your control notification handler code here
	CButton* pButtonRaw = (CButton*) GetDlgItem(IDC_DISPLAYMESSAGEFORMAT_RAW);
	//CButton* pButtonDevice = (CButton*) GetDlgItem(IDC_INPUT_FILTERDEVICE);
	CButton* pButtonInterpret = (CButton*) GetDlgItem(IDC_DISPLAYMESSAGEFORMAT_INTERPRET);

	if (pButtonRaw) pButtonRaw->SetCheck(0);
	//if (pButtonDevice) pButtonDevice->SetCheck(0);
	if (pButtonInterpret) pButtonInterpret->SetCheck(0);

	switch(m_nInputTextBoxFilter)
	{
	case INPUTTEXTBOXFILTER_RAW:		if (pButtonRaw)			pButtonRaw->SetCheck(1); break;
	//case INPUTTEXTBOXFILTER_DEVICE:		if(pButtonDevice)		pButtonDevice->SetCheck(1);break;
	case INPUTTEXTBOXFILTER_INTERPRET:	if (pButtonInterpret)	pButtonInterpret->SetCheck(1);break;
	}
}

void CRUglyDlg::OnBnClickedFreeze()
{
	// TODO: Add your control notification handler code here
	CButton* pButton;

	pButton = (CButton*) GetDlgItem(IDC_FREEZE);
	if (pButton)
	{
		m_bFreezeDisplay = (pButton->GetCheck() != 0);
	}
}
void CRUglyDlg::OnShowFreeze()
{
	CButton* pButton;

	pButton = (CButton*) GetDlgItem(IDC_FREEZE);
	if (pButton)
	{
		pButton->SetCheck(m_bFreezeDisplay?1:0);
	}

}
void CRUglyDlg::OnBnClickedSettingsfileSelect()
{
	// TODO: Add your command handler code here
		char szSelectFileTitle[_MAX_PATH];
		char szSelectFileFilter[256];
		BOOL bStatus;
		char szInputFileName[_MAX_PATH];




		strcpy(szInputFileName, m_SettingsFile.m_strSettingsFileName.GetBuffer());
		m_SettingsFile.m_strSettingsFileName.ReleaseBuffer();

		OPENFILENAME ofnSelectFileOpen = {  
		0,					//   DWORD        lStructSize;
		NULL,				//   HWND         hwndOwner;
		0,					//   HINSTANCE    hInstance;
		szSelectFileFilter,	//   LPCWSTR      lpstrFilter;
		NULL,				//   LPWSTR       lpstrCustomFilter;
		256,				//   DWORD        nMaxCustFilter;
		1,					//   DWORD        nFilterIndex;
		szInputFileName,//   LPWSTR       lpstrFile;
		256,				//   DWORD        nMaxFile;
		szSelectFileTitle,//  LPWSTR       lpstrFileTitle;
		256,				//   DWORD        nMaxFileTitle;
		NULL,//szSelectFileInitialDir,// LPCWSTR      lpstrInitialDir;
		"Select Input File",	//   LPCWSTR      lpstrTitle;
		0,					//   DWORD        Flags;
		0,					//   WORD         nFileOffset;
		0,					//   WORD         nFileExtension;
		"*.txt",			//   LPCWSTR      lpstrDefExt;
		0,					//   LPARAM       lCustData;
		NULL,				//   LPOFNHOOKPROC lpfnHook;
		NULL				//   LPCWSTR      lpTemplateName;
		};

		sprintf (szSelectFileFilter,
			"%s%c%s%c%s%c%s%c",
			"Text files (*.txt)",0,"*.txt",0,
			"All Files",0,"*.*",0,0);

		if (NULL != strstr(szInputFileName,".txt")) {
		 		ofnSelectFileOpen.nFilterIndex = 1;
		}

		ofnSelectFileOpen.lStructSize = sizeof(ofnSelectFileOpen);
		ofnSelectFileOpen.hwndOwner = this->m_hWnd;
		bStatus = GetOpenFileName(&ofnSelectFileOpen);
  		if (bStatus)
		{
			strcpy(m_pcSettingsFileName, ofnSelectFileOpen.lpstrFile);
			
			//m_SettingsFile.m_strSettingsFileName = ;
		}
		else
		{
			m_pcSettingsFileName[0] = 0;
			//m_SettingsFile.m_strSettingsFileName.Empty();
		}

		SetDlgItemText(IDC_SETTINGSFILE_FILENAME,m_pcSettingsFileName); 
		//OnSetFileDisplayNames();
		CString strErrorMsg;
		m_SettingsFile.SettingsFileInit(m_pcSettingsFileName, strErrorMsg);
		if (!strErrorMsg.IsEmpty())
		{
			UpdateOutputText(strErrorMsg); // SetDlgItemText(IDC_RICHEDIT_OUTPUT, strErrorMsg);
		}

}

void CRUglyDlg::OnBnClickedSettingsfileSend()
{
	// TODO: Add your control notification handler code here
	m_SettingsFile.m_nSettingsFileProductType = m_nProductType;
	m_SettingsFile.m_strSettingsFileName = m_pcSettingsFileName;
	m_SettingsFile.OnStart();
}

void CRUglyDlg::OnBnClickedSettingsfileStop()
{
	// TODO: Add your control notification handler code here
	m_SettingsFile.OnStop();
}

void CRUglyDlg::OnBnClickedSettingsfileLoad()
{
	// TODO: Add your control notification handler code here
	m_SettingsFile.m_nSettingsFileProductType = m_nProductType;
	m_SettingsFile.m_strSettingsFileName = m_pcSettingsFileName;
	m_SettingsFile.OnRead();
}


void CRUglyDlg::OnBnClickedSavestate()
{
	// TODO: Add your control notification handler code here

	// TODO: Add your control notification handler code here
	FILE *fpFactoryDefault = NULL;
	char pcFileName[_MAX_PATH] = "SaveState.txt";

	char pcString[200];
	tParameterDesc* pParameterDesc;
	void** pParameterValue;
	intfloat j;
	int i;
	char pcFormat[200];
	char szSelectFileTitle[_MAX_PATH];
	char szSelectFileFilter[256];
	CString strFileName;

	tParameterDesc* pParameterDescTable; 
	int nTableLength;
	int nXMLVersion;

	CFileDialog cDialog(false, "txt","SaveState.txt");

	if (false == cDialog.DoModal())
	{
		return;
	}
	
	switch(m_nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		break;
	case ePRODUCT_REDLINESPEAKER1:
		break;
	default:
		MessageBox("Invalid Product type","Save Error",0);
		return;
	}
	strFileName = cDialog.GetPathName();


	/*


	OPENFILENAME ofnSelectFileSave = {  
		0,					//   DWORD        lStructSize;
		NULL,				//   HWND         hwndOwner;
		0,					//   HINSTANCE    hInstance;
		szSelectFileFilter,	//   LPCWSTR      lpstrFilter;
		NULL,				//   LPWSTR       lpstrCustomFilter;
		256,				//   DWORD        nMaxCustFilter;
		0,					//   DWORD        nFilterIndex;
		pcFileName,			//   LPWSTR       lpstrFile;
		256,				//   DWORD        nMaxFile;
		szSelectFileTitle,//  LPWSTR       lpstrFileTitle;
		256,				//   DWORD        nMaxFileTitle;
		NULL,//szSelectFileInitialDir,// LPCWSTR      lpstrInitialDir;
		"Select Save State File",	//   LPCWSTR      lpstrTitle;
		0,					//   DWORD        Flags;
		0,					//   WORD         nFileOffset;
		0,					//   WORD         nFileExtension;
		"*.txt",			//   LPCWSTR      lpstrDefExt;
		0,					//   LPARAM       lCustData;
		NULL,				//   LPOFNHOOKPROC lpfnHook;
		NULL				//   LPCWSTR      lpTemplateName;
		};

		sprintf (szSelectFileFilter,
			"%s%c%s%c %s%c%s%c%s%c%s%c",
			"Text File (*.txt)",0,"*.txt",0,
			"All Files",0,"*.*",0,0);

	 
	GetSaveFileName(&ofnSelectFileSave); 
	 

	strcpy(pcFileName, ofnSelectFileSave.lpstrFile);
*/


	strcpy(pcFileName, strFileName.GetBuffer());
	strFileName.ReleaseBuffer();


		switch(m_nProductType)
		{
		case ePRODUCT_OLYSPEAKER:
			m_ProductOLY.SaveCurrentStateToFile(pcFileName);
			break;
		case ePRODUCT_REDLINESPEAKER1:
			m_ProductREDLINE.SaveCurrentStateToFile(pcFileName);
			break;
		default:
			break;

		}
	


}



void CRUglyDlg::OnBnClickedSavestatefir()
{
	// TODO: Add your control notification handler code here

	// TODO: Add your control notification handler code here
	FILE *fpFactoryDefault = NULL;
	char pcFileName[_MAX_PATH] = "SaveStateFIR.txt";

	char pcString[200];
	tParameterDesc* pParameterDesc;
	void** pParameterValue;
	intfloat j;
	int i;
	char pcFormat[200];
	//char szSelectFileTitle[_MAX_PATH];
	//char szSelectFileFilter[256];
	CString strFileName;

	//tParameterDesc* pParameterDescTable; 
	//int nTableLength;
	//int nXMLVersion;

	CFileDialog cDialog(false, "txt","SaveStateFIR.txt");

	if (false == cDialog.DoModal())
	{
		return;
	}
	
	switch(m_nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		break;
	case ePRODUCT_REDLINESPEAKER1:
		break;
	default:
		MessageBox("Invalid Product type","Save Error",0);
		return;
	}

	strFileName = cDialog.GetPathName();

	/*


	OPENFILENAME ofnSelectFileSave = {  
		0,					//   DWORD        lStructSize;
		NULL,				//   HWND         hwndOwner;
		0,					//   HINSTANCE    hInstance;
		szSelectFileFilter,	//   LPCWSTR      lpstrFilter;
		NULL,				//   LPWSTR       lpstrCustomFilter;
		256,				//   DWORD        nMaxCustFilter;
		0,					//   DWORD        nFilterIndex;
		pcFileName,			//   LPWSTR       lpstrFile;
		256,				//   DWORD        nMaxFile;
		szSelectFileTitle,//  LPWSTR       lpstrFileTitle;
		256,				//   DWORD        nMaxFileTitle;
		NULL,//szSelectFileInitialDir,// LPCWSTR      lpstrInitialDir;
		"Select Save State File",	//   LPCWSTR      lpstrTitle;
		0,					//   DWORD        Flags;
		0,					//   WORD         nFileOffset;
		0,					//   WORD         nFileExtension;
		"*.txt",			//   LPCWSTR      lpstrDefExt;
		0,					//   LPARAM       lCustData;
		NULL,				//   LPOFNHOOKPROC lpfnHook;
		NULL				//   LPCWSTR      lpTemplateName;
		};

		sprintf (szSelectFileFilter,
			"%s%c%s%c %s%c%s%c%s%c%s%c",
			"Text File (*.txt)",0,"*.txt",0,
			"All Files",0,"*.*",0,0);

	 
	GetSaveFileName(&ofnSelectFileSave); 
	 

	strcpy(pcFileName, ofnSelectFileSave.lpstrFile);
*/


	strcpy(pcFileName, strFileName.GetBuffer());
	strFileName.ReleaseBuffer();

	switch(m_nProductType)
		{
		case ePRODUCT_OLYSPEAKER:
			m_ProductOLY.SaveCurrentStateFIRToFile(pcFileName);
			break;
		case ePRODUCT_REDLINESPEAKER1:
			m_ProductREDLINE.SaveCurrentStateFIRToFile(pcFileName);
			break;
		default:
			break;
		}

}

void CRUglyDlg::OnBnClickedProducttypeOly()
{
	SetProducttype(ePRODUCT_OLYSPEAKER);
	ShowProducttype();

}
void CRUglyDlg::OnBnClickedProducttypeRedline()
{
	SetProducttype(ePRODUCT_REDLINESPEAKER1);
	ShowProducttype();
}

void CRUglyDlg::ShowProducttype()
{
	CButton* pButtonOly = (CButton*)GetDlgItem(IDC_PRODUCTTYPE_OLY);
	CButton* pButtonRedline = (CButton*)GetDlgItem(IDC_PRODUCTTYPE_REDLINE);
	if (pButtonOly)
	{
		pButtonOly->SetCheck(m_nProductType == ePRODUCT_OLYSPEAKER);
	}
	if (pButtonRedline)
	{
		pButtonRedline->SetCheck(m_nProductType == ePRODUCT_REDLINESPEAKER1);
	}


}

void CRUglyDlg::OnBnClickedCleardisplay()
{
	// TODO: Add your control notification handler code here
	SetDlgItemText(IDC_RICHEDIT_INPUT,"");
	SetDlgItemText(IDC_RICHEDIT_OUTPUT,"");
}


void CRUglyDlg::OnBnClickedGetstatefromdevice()
{
	// TODO: Add your control notification handler code here

	if (m_bGetStateStarted)
	{
		m_bGetStateStarted = false;
		SetDlgItemText(IDC_GETSTATEFROMDEVICE,"Get State from Device");
		switch (m_nProductType)
		{
		case ePRODUCT_OLYSPEAKER:
			return m_ProductOLY.GetStateStop();
		case ePRODUCT_REDLINESPEAKER1:
			return m_ProductREDLINE.GetStateStop();
		default:
			break;
		}
	}
	else
	{
		m_bGetStateStarted = true;
		SetDlgItemText(IDC_GETSTATEFROMDEVICE,"Stop State from Device");
		OnBnClickedGetHwInfo();
		switch (m_nProductType)
		{
		case ePRODUCT_OLYSPEAKER:
			return m_ProductOLY.GetStateStart(eTARGET_INVALID);
		case ePRODUCT_REDLINESPEAKER1:
			return m_ProductREDLINE.GetStateStart();
		default:
			break;
		}
	}

}

void CRUglyDlg::OnShowGetstatefromdevice()
{
	if (m_bGetStateStarted)
	{
		SetDlgItemText(IDC_GETSTATEFROMDEVICE,"Stop State from Device");
	}
	else
	{
		SetDlgItemText(IDC_GETSTATEFROMDEVICE,"Get State from Device");
	}
}

void CRUglyDlg::OnBnClickedGetFIRstatefromdevice()
{
	// TODO: Add your control notification handler code here
	switch (m_nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		return m_ProductOLY.GetStateStart(eTARGET_FIR);
	case ePRODUCT_REDLINESPEAKER1:
		return m_ProductREDLINE.GetStateStart();
	default:
		break;
	}

}




void CRUglyDlg::GetStateRequestNext()
{

	switch (m_nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		return m_ProductOLY.GetStateRequestNext();
	case ePRODUCT_REDLINESPEAKER1:
		return m_ProductREDLINE.GetStateRequestNext();
		break;
	default:
		break;
	}
}

void CRUglyDlg::OnBnClickedSendstatetodevice()
{

	switch (m_nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		return m_ProductOLY.SendStateStart();
	case ePRODUCT_REDLINESPEAKER1:
		return m_ProductREDLINE.SendStateStart();
	default:
		break;
	}
}

void CRUglyDlg::OnBnClickedSendstatetodevicefir()
{
	// TODO: Add your control notification handler code here
		switch (m_nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		return m_ProductOLY.SendStateStart(1);
	case ePRODUCT_REDLINESPEAKER1:
		return m_ProductREDLINE.SendStateStart();
	default:
		break;
	}
}


void CRUglyDlg::OnBnClickedSendstatetodevicestop()
{
	// TODO: Add your control notification handler code here
		switch (m_nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		return m_ProductOLY.SendStateStop();
	case ePRODUCT_REDLINESPEAKER1:
		return m_ProductREDLINE.SendStateStop();
	default:
		break;
	}
}


void CRUglyDlg::SendStateSendNext()
{
	switch (m_nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		return m_ProductOLY.SendStateSendNext();
	case ePRODUCT_REDLINESPEAKER1:
		return m_ProductREDLINE.SendStateSendNext();
	default:
		break;
	}

}
// --------------------------------------------------------------------------------------------------------

void  CRUglyDlg::ParameterAdjustSetComboBox(int nProductType, CComboBox** pCombo, int nNum)
{
	switch(nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		return m_ProductOLY.ParameterAdjustSetComboBox(pCombo,nNum);

	case ePRODUCT_REDLINESPEAKER1:
		return m_ProductREDLINE.ParameterAdjustSetComboBox(pCombo,nNum);

	default:
		break;
	}
	return;

}

// --------------------------------------------------------------------------------------------------------
int CRUglyDlg::GetLevelSwitchDescList(int nProductType, ControlDesc** pList)
{
	switch(nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		return m_ProductOLY.GetLevelSwitchDescList(pList);
	case ePRODUCT_REDLINESPEAKER1:
		return m_ProductREDLINE.GetLevelSwitchDescList(pList);
	default:
		break;
	}
	return 0;

}
int CRUglyDlg::GetLevelFaderDescList(int nProductType, ControlDesc** pList)
{
	switch(nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		return m_ProductOLY.GetLevelFaderDescList(pList);
	case ePRODUCT_REDLINESPEAKER1:
		return m_ProductREDLINE.GetLevelFaderDescList(pList);
	default:
		break;
	}
	return 0;

}
int CRUglyDlg::GetLevelEditBoxDescList(int nProductType, ControlDesc** pList)
{
	switch(nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		return m_ProductOLY.GetLevelEditBoxDescList(pList);
	case ePRODUCT_REDLINESPEAKER1:
		return m_ProductREDLINE.GetLevelEditBoxDescList(pList);
	default:
		break;
	}
	return 0;

}
int CRUglyDlg::GetLevelTitleList(int nProductType, tStringIDC** pList)
{
	switch(nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		return m_ProductOLY.GetLevelTitleList(pList);
		break;
	case ePRODUCT_REDLINESPEAKER1:
		return m_ProductREDLINE.GetLevelTitleList(pList);
	default:
		break;
	}
	return 0;

}

// -------------------------------------------------------------------------------------------

int CRUglyDlg::GetMetersDescList(int nProductType, ControlDesc** pList)
{
		switch(nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		return m_ProductOLY.GetMetersDescList(pList);
	case ePRODUCT_REDLINESPEAKER1:
		return m_ProductREDLINE.GetMetersDescList(pList);
	default:
		break;
	}
	return 0;

}
int CRUglyDlg::GetMetersTitleList(int nProductType, tStringIDC** pList)
{
		switch(nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		return m_ProductOLY.GetMetersTitleList(pList);
		break;
	case ePRODUCT_REDLINESPEAKER1:
		return m_ProductREDLINE.GetMetersTitleList(pList);
	default:
		break;
	}
	return 0;

}

// ----------------------------------------------------------------------------------------------

void CRUglyDlg::OnBnClickedSnapshotstore()
{
	// TODO: Add your control notification handler code here
	intfloat nSnapshot;
	int nType;
	char pcBuffer[200];
	int nUserTarget;

	nSnapshot = GetDlgItemValue(IDC_SNAPSHOTNUMBER,nType);
	if (nType != 0)
	{
		nSnapshot.i = nSnapshot.f;
	}
	if ((nSnapshot.i >= 1)  && (nSnapshot.i <= 3))
	{

		sprintf(pcBuffer,"%d",nSnapshot.i);
		SetDlgItemText(IDC_SNAPSHOTNUMBER,pcBuffer);

		switch (m_nProductType)
		{
		case ePRODUCT_OLYSPEAKER:
			nUserTarget = m_ProductOLY.GetUserTarget();
			nSnapshot.i = ((nSnapshot.i & 0x0f) << 24) + ((nUserTarget & 0x0ff) << 8);
			break;
		}

		m_msgTx.payload = m_msgTxPayload;
		m_MandolinComm.CreateSnapshotStore(&m_msgTx, m_nTxSequence,nSnapshot.i);
		TryToSendMandolinMessage(&m_msgTx, true);
	}

}



void CRUglyDlg::OnBnClickedSnapshotload()
{
	// TODO: Add your control notification handler code here
	intfloat nSnapshot;
	int nType;
	char pcBuffer[200];

	nSnapshot = GetDlgItemValue(IDC_SNAPSHOTNUMBER,nType);
	if (nType != 0)
	{
		nSnapshot.i = nSnapshot.f;
	}
	if ((nSnapshot.i >= 1)  && (nSnapshot.i <= 3))
	{
		sprintf(pcBuffer,"%d",nSnapshot.i);
		SetDlgItemText(IDC_SNAPSHOTNUMBER,pcBuffer);

		m_msgTx.payload = m_msgTxPayload;
		m_MandolinComm.CreateSnapshotRecall(&m_msgTx, m_nTxSequence,nSnapshot.i);
		TryToSendMandolinMessage(&m_msgTx, true);
	}

}

void CRUglyDlg::OnBnClickedZimpedancedlg()
{
	// TODO: Add your control notification handler code here
	m_dlgZImpedance.m_nProductType = m_nProductType;

	if (m_nProductType != ePRODUCT_REDLINESPEAKER1)
	{
		SetDlgItemText(IDC_RESPONSESTATUS,"ZImpedance Only available for Redline");
	}
	else
	{
		m_dlgZImpedance.ShowWindow( SW_RESTORE  );
	}

}


void CRUglyDlg::OnBnClickedOlyimpedancedlg()
{
	// TODO: Add your control notification handler code here
		// TODO: Add your control notification handler code here
	m_dlgOlyImpedance.m_nProductType = m_nProductType;

	if (m_nProductType != ePRODUCT_OLYSPEAKER)
	{
		SetDlgItemText(IDC_RESPONSESTATUS,"OLY Impedance Only available for OLY ");
	}
	else
	{
		m_dlgOlyImpedance.ShowWindow( SW_RESTORE  );
	}

}


void CRUglyDlg::OnBnClickedCreatepresetheaders()
{
	// TODO: Add your control notification handler code here
	m_dlgCreatePresetHeaders.m_nProductType = m_nProductType;

	if (m_nProductType != ePRODUCT_OLYSPEAKER)
	{
		SetDlgItemText(IDC_RESPONSESTATUS,"OLY Impedance Only available for OLY ");
	}
	else
	{
		m_dlgCreatePresetHeaders.ShowWindow( SW_RESTORE  );
	}

}

void CRUglyDlg::OnBnClickedTestmanu()
{
	// TODO: Add your control notification handler code here
	m_dlgTestManu.m_nProductType = m_nProductType;

	if (m_nProductType != ePRODUCT_OLYSPEAKER)
	{
		SetDlgItemText(IDC_RESPONSESTATUS,"OLY Test Manu Only available for OLY ");
	}
	else
	{
		m_dlgTestManu.ShowWindow( SW_RESTORE  );
	}

}


intfloat  CRUglyDlg::GetParameterRandomValue(tParameterDesc* pParameterDesc, char* pcBuffer)
{
	intfloat j;
	double r;
	intfloat jMin, jMax;

	j.i = 0;
	pcBuffer[0] = 0;
	if (!pParameterDesc) return j;

	r = (double)(rand()*1.0)/RAND_MAX;
	jMin.ui = pParameterDesc->minValue;
	jMax.ui = pParameterDesc->maxValue;
	switch(pParameterDesc->format)
	{
		case eFORMAT_BOOL:
		case eFORMAT_SIGNED:
			j.i = (jMax.i-jMin.i)*r + jMin.i;
			if (j.i > jMax.i) j.i = jMax.i;
			if (j.i < jMin.i) j.i = jMin.i;

			break;
		case eFORMAT_UNSIGNED:
			j.ui = (jMax.ui-jMin.ui)*r + jMin.ui;
			if (j.ui > jMax.ui) j.ui = jMax.ui;
			if (j.ui < jMin.ui) j.ui = jMin.ui;
			break;
		case eFORMAT_FLOAT:
			if ((pParameterDesc->paramEnum == ePARAM_FREQ) || (pParameterDesc->paramEnum == ePARAM_Q))
			{
					//df = ( pow ( (float) 10.0, log10(theMax/theMin) / (float) theNumSteps ) );
					//fValue = pow(df,nEffectSend)*theMin;
					double dfmax;
					double dfmin;
					double df;
					dfmax = log10(jMax.f);
					dfmin = log10(jMin.f);
					df = (dfmax-dfmin)*r+dfmin;  // random in the exponent
					j.f = pow(10,df);
			}
			else
			{
				j.f = (jMax.f-jMin.f)*r + jMin.f;
			}
			if (j.f > jMax.f) j.f = jMax.f;
			if (j.f < jMin.f) j.f = jMin.f;

			break;
	}
	m_MandolinTextSummary.ConvertParameter2Text(pParameterDesc, j, pcBuffer, true);
	return j;
}


void CRUglyDlg::OnBnClickedCreaterandomsnapshot()
{
	// TODO: Add your control notification handler code here
	char pcBuffer[200];
	intfloat j;
	int i;
	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;


	switch(m_nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		m_ProductOLY.SetCurrentStateToRandom();
		m_ProductOLY.ShowCurrentState();
		break;
	case ePRODUCT_REDLINESPEAKER1:
		m_ProductREDLINE.SetCurrentStateToRandom();
		m_ProductREDLINE.ShowCurrentState();
		break;
	default:
		break;
	}

	ShowParameter(2,1,true,true); // show all

}

void CRUglyDlg::OnBnClickedCreaterandomfir()
{
	// TODO: Add your control notification handler code here
		// TODO: Add your control notification handler code here
	char pcBuffer[200];
	intfloat j;
	int i;
	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;


	switch(m_nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		m_ProductOLY.SetCurrentStateToRandomFIR();
		m_ProductOLY.ShowCurrentStateFIR();
		break;
	case ePRODUCT_REDLINESPEAKER1:
		m_ProductREDLINE.SetCurrentStateToRandomFIR();
		m_ProductREDLINE.ShowCurrentStateFIR();
		break;
	default:
		break;
	}

	ShowParameter(2,1,true,true); // show all
}


int CRUglyDlg::GetRandomTarget(int* nTarget, int* nInstances)
{
	// returns the # of parameters in the target  nInstances are 1..n
	// TODO: Add your control notification handler code here


	switch(m_nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		return m_ProductOLY.GetRandomTarget(nTarget,nInstances);
	case ePRODUCT_REDLINESPEAKER1:
		return m_ProductREDLINE.GetRandomTarget(nTarget,nInstances);
		break;
	default:
		break;
	}

	return 0;
}

void CRUglyDlg::OnBnClickedShowstate2()
{
	switch(m_nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		m_ProductOLY.ShowCurrentState();
		break;
	case ePRODUCT_REDLINESPEAKER1:
		m_ProductREDLINE.ShowCurrentState();
		break;
	default:
		break;
	}

}

void CRUglyDlg::OnBnClickedShowstatefir()
{
	switch(m_nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		m_ProductOLY.ShowCurrentStateFIR();
		break;
	case ePRODUCT_REDLINESPEAKER1:
		m_ProductREDLINE.ShowCurrentStateFIR();
		break;
	default:
		break;
	}

}


void CRUglyDlg::OnBnClickedSetstatetodefaults()
{
	// TODO: Add your control notification handler code here
	switch(m_nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		m_ProductOLY.SetCurrentStateToDefaults();
		m_ProductOLY.ShowCurrentState();
		break;
	case ePRODUCT_REDLINESPEAKER1:
		m_ProductREDLINE.SetCurrentStateToDefaults();
		m_ProductREDLINE.ShowCurrentState();
		break;
	default:
		break;
	}

}

void CRUglyDlg::OnBnClickedSetstatetodefaultsfir()
{
	// TODO: Add your control notification handler code here
	switch(m_nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		m_ProductOLY.SetCurrentStateToDefaultsFIR();
		m_ProductOLY.ShowCurrentStateFIR();
		break;
	case ePRODUCT_REDLINESPEAKER1:
		m_ProductREDLINE.SetCurrentStateToDefaultsFIR();
		m_ProductREDLINE.ShowCurrentStateFIR();
		break;
	default:
		break;
	}

}


int CRUglyDlg::GetParameterDescTable(int nProduct, int nTarget, tParameterDesc** ppParameterDesc, int* pTableLength, int* nXMLVersion)
{
	switch(nProduct)
	{
	case ePRODUCT_OLYSPEAKER:
		return m_ProductOLY.GetParameterDescTable(nTarget, ppParameterDesc, pTableLength, nXMLVersion);
	case ePRODUCT_REDLINESPEAKER1:
		return m_ProductREDLINE.GetParameterDescTable(nTarget, ppParameterDesc, pTableLength, nXMLVersion);

	}

	return 0;

}



tParameterDesc* CRUglyDlg::GetParameterDesc(int nProductType, int nTarget, int nParamNum)
{

	switch(nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		return m_ProductOLY.GetParameterDesc( nTarget,  nParamNum);
	case ePRODUCT_REDLINESPEAKER1:
		return m_ProductREDLINE.GetParameterDesc( nTarget,  nParamNum);
	}
	return 0;
}

int CRUglyDlg::GetMeterType(int nProductType, int nTarget,  int nParamNum)
{

	switch(nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		return m_ProductOLY.GetMeterType(  nTarget, nParamNum);
	case ePRODUCT_REDLINESPEAKER1:
		return m_ProductREDLINE.GetMeterType(  nTarget,  nParamNum);
	default:
		return 0;
	}

}


unsigned int* CRUglyDlg::GetMeterSubscriptionList(int nProductType, int nSelect, int* pnSize)
{
	*pnSize = 0;
	switch(nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		return m_ProductOLY.GetMeterSubscriptionList( nSelect, pnSize);
	case ePRODUCT_REDLINESPEAKER1:
		//return m_ProductREDLINE.GetMeterSubscriptionList(  nSelect, pnSize);
		break;
	}
	return 0;
}

void CRUglyDlg::OnBnClickedMeters()
{
	// TODO: Add your control notification handler code here
	m_dlgMeters.ShowWindow( SW_RESTORE  );
}

void CRUglyDlg::OnBnClickedDspsignalflow()
{
	// TODO: Add your control notification handler code here
	m_dlgDSPSignalFlow.ShowWindow( SW_RESTORE  );
}

void CRUglyDlg::OnBnClickedEqgraph()
{
	// TODO: Add your control notification handler code here
	m_dlgEQGraph.ShowWindow( SW_RESTORE  );
}

void CRUglyDlg::OnBnClickedMandolinConnnect()
{
	//Firmware State Machine
	//mcsDisconnected,			//	No CONNECT mesg received
	//mcsConnectReceived,			//	CONNECT message received,
	//mcsGetHardInfoSent,			//	GetHardInfo Sent, awaiting reply
	//mcsGetHardInfoReplyReceived,//	Reply for Get Hardware Info received,
	//mcsGetMaxMsgSent,			//	GetMaxMsg Sent, awaiting reply
	//mcsGetMaxMsgReplyReceived,	//	GetMaxMsg reply received, awaiting GetMaxMsg command
	//mcsGetMaxMsgReceived,		//	GetMaxMsg Received
	//mcsConnected				//	Connect message sent, awaiting Sync

	// TODO: Add your control notification handler code here

	m_bMandolinConnectStateStart = true;
	
	MandolinConnectStateSetState(MANDOLIN_CONNECT_SE_CONNECT_SEND);
	MandolinConnectStateDoState();

	SetTimer(KEEPALIVE_TIMERID,5000,NULL);


}




void CRUglyDlg::OnBnClickedMandolinDisconnnect()
{
	// TODO: Add your control notification handler code here
	m_bMandolinConnectStateStart = false;
	KillTimer(KEEPALIVE_TIMERID);

	m_msgTx.payload = m_msgTxPayload;
	m_MandolinComm.CreateDisconnect(&m_msgTx, m_nTxSequence,MANDOLIN_DISCONNECT_USER_REQUEST);
	TryToSendMandolinMessage(&m_msgTx, true);


}

void CRUglyDlg::OnBnClickedParametertest()
{
	m_dlgParameterTest.ShowWindow( SW_RESTORE  );

}

void CRUglyDlg::OnBnClickedSyncRequest()
{
	// TODO: Add your control notification handler code here
	m_msgTx.payload = m_msgTxPayload;
	m_MandolinComm.CreateSyncState(&m_msgTx, m_nTxSequence);
	TryToSendMandolinMessage(&m_msgTx, true);

}

void CRUglyDlg::OnBnClickedGetHwInfo()
{
	// TODO: Add your control notification handler code here
	m_msgTx.payload = m_msgTxPayload;
	m_MandolinComm.CreateGetHardwareInfo(&m_msgTx, m_nTxSequence);
	TryToSendMandolinMessage(&m_msgTx, true);

}

void CRUglyDlg::OnBnClickedGetSwInfo()
{
	// TODO: Add your control notification handler code here
	m_msgTx.payload = m_msgTxPayload;
	m_MandolinComm.CreateGetSoftwareInfo(&m_msgTx, m_nTxSequence);
	TryToSendMandolinMessage(&m_msgTx, true);
}



void CRUglyDlg::OnBnClickedStoresuseredit()
{
	uint32 uiPayloadValues[2] = {MANDOLIN_AE_STORE_USER_EDIT,0};

	m_msgTx.payload = m_msgTxPayload;
	m_MandolinComm.CreateGeneric(&m_msgTx, m_nTxSequence,MANDOLIN_MSG_TRIGGER_APPLICATION_EVENT,1,uiPayloadValues);
	TryToSendMandolinMessage(&m_msgTx, true);
}

void CRUglyDlg::OnBnClickedCancelsuseredit()
{
	uint32 uiPayloadValues[2] = {MANDOLIN_AE_CANCEL_USER_EDIT,0};

	m_msgTx.payload = m_msgTxPayload;
	m_MandolinComm.CreateGeneric(&m_msgTx, m_nTxSequence,MANDOLIN_MSG_TRIGGER_APPLICATION_EVENT,1,uiPayloadValues);
	TryToSendMandolinMessage(&m_msgTx, true);
}

void CRUglyDlg::OnBnClickedIdentifyOn()
{
	// TODO: Add your control notification handler code here
	uint32 uiPayloadValues[2] = {MANDOLIN_AE_SET_AVAILABLE,10000};	// 10 sec

	m_msgTx.payload = m_msgTxPayload;
	m_MandolinComm.CreateGeneric(&m_msgTx, m_nTxSequence,MANDOLIN_MSG_TRIGGER_APPLICATION_EVENT,2,uiPayloadValues);
	TryToSendMandolinMessage(&m_msgTx, true);

}

void CRUglyDlg::OnBnClickedIdentifyOff()
{
	// TODO: Add your control notification handler code here
	uint32 uiPayloadValues[2] = {MANDOLIN_AE_SET_AVAILABLE,0};	// off

	m_msgTx.payload = m_msgTxPayload;
	m_MandolinComm.CreateGeneric(&m_msgTx, m_nTxSequence,MANDOLIN_MSG_TRIGGER_APPLICATION_EVENT,2,uiPayloadValues);
	TryToSendMandolinMessage(&m_msgTx, true);

}

//void CRUglyDlg::SetIPAddr(uint8 addr1, uint8 addr2, addr3, addr4)
//{
//	CIPAddressCtrl* pctrIPAddr = (CIPAddressCtrl*)GetDlgItem(IDC_SOCKET_IPADDRESS1);
//	if (pctrIPAddr)
//	{
//		pctrIPAddr->SetAddress(addr1,addr2,addr3,addr4);
//	}
//
//}

void CRUglyDlg::OnBnClickedSocketConnect()
{
	// TODO: Add your control notification handler code here
	CIPAddressCtrl* pctrIPAddr;
	unsigned char addr1,addr2,addr3,addr4;

	unsigned int nIPAddress;
	int nSocketPort;

		
	pctrIPAddr = (CIPAddressCtrl*)GetDlgItem(IDC_SOCKET_IPADDRESS1);
	nSocketPort = GetDlgItemInt( IDC_SOCKET_IPADDRESSPORT );

	if (pctrIPAddr)
	{
		SetDlgItemText(IDC_SOCKET_STATUS,"Trying to Connect");

		pctrIPAddr->GetAddress(addr1,addr2,addr3,addr4);

		nIPAddress = ((addr1 & 0x0ff)<<24) + ((addr2& 0x0ff)<<16) + ((addr3 & 0x0ff)<<8)+ (addr4 & 0x0ff);
		//m_nSocketIPAddress = (20<<24) + (0<<16) + (168<<8)+ (192);

		OnChangeSocketSettings(nIPAddress, nSocketPort,m_nSocketType);

		ShowIPAddress();

	}

}

void CRUglyDlg::OnBnClickedSocketDisconnect()
{
	// TODO: Add your control notification handler code here
	CloseSocketComm();
	m_bIsConnected = false;
	ShowIPAddress();
}


void CRUglyDlg::ShowIPAddress()
{
	CIPAddressCtrl* pctrIPAddr;
		
	pctrIPAddr = (CIPAddressCtrl*)GetDlgItem(IDC_SOCKET_IPADDRESS1);

	if (pctrIPAddr)
	{
		pctrIPAddr->SetAddress(m_nSocketIPAddress);
	}

	SetDlgItemInt(IDC_SOCKET_IPADDRESSPORT,m_nSocketPort);

	if(m_bIsConnected)
	{
		switch(m_nComType)
		{
		case COMTYPE_SOCKET: SetDlgItemText(IDC_SOCKET_STATUS,"Connected - Socket"); break;
		case	COMTYPE_COM: SetDlgItemText(IDC_SOCKET_STATUS,"Connected - Com"); break;
		default:			SetDlgItemText(IDC_SOCKET_STATUS,"Connected"); break;
		}

	}
	else
	{
		SetDlgItemText(IDC_SOCKET_STATUS,"Not Connected");
	}
	//m_dlgSocketSettings.ShowIPAddress();
	//UpdateData(FALSE);
}
//MANDOLIN_MSG_TRIGGER_APPLICATION_EVENT;
//MANDOLIN_AE_STORE_SNAPSHOT);

void CRUglyDlg::OnBnClickedSelftestmode()
{
	// TODO: Add your control notification handler code here

		// TODO: Add your control notification handler code here
	uint32 uiPayloadValues[2] = {MANDOLIN_AE_SET_SELF_TEST_MODE,eTEST_MODE_DISABLED};	

	int nSel = 0;
	int nMode = eTEST_MODE_DISABLED;

	CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_SELFTESTMODE_SELECT);
	if (pCombo)
	{
		nSel = pCombo->GetCurSel();
		//pCombo->GetLBText(nSel,pcTarget);
		nMode = pCombo->GetItemData(nSel);
	}
	uiPayloadValues[1] = nMode;

	m_msgTx.payload = m_msgTxPayload;
	m_MandolinComm.CreateGeneric(&m_msgTx, m_nTxSequence,MANDOLIN_MSG_TRIGGER_APPLICATION_EVENT,2,uiPayloadValues);
	TryToSendMandolinMessage(&m_msgTx, true);
}


void CRUglyDlg::OnBnClickedStartautooptimize()
{
	// TODO: Add your control notification handler code here
	uint32 uiPayloadValues[2] = {MANDOLIN_AE_START_AUTO_OPTIMIZE,0};	
	m_msgTx.payload = m_msgTxPayload;
	m_MandolinComm.CreateGeneric(&m_msgTx, m_nTxSequence,MANDOLIN_MSG_TRIGGER_APPLICATION_EVENT,1,uiPayloadValues);
	TryToSendMandolinMessage(&m_msgTx, true);


}

void CRUglyDlg::OnBnClickedResetautooptimize()
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	uint32 uiPayloadValues[2] = {MANDOLIN_AE_RESET_AUTO_OPTIMIZE,0};	
	m_msgTx.payload = m_msgTxPayload;
	m_MandolinComm.CreateGeneric(&m_msgTx, m_nTxSequence,MANDOLIN_MSG_TRIGGER_APPLICATION_EVENT,1,uiPayloadValues);
	TryToSendMandolinMessage(&m_msgTx, true);

}

void CRUglyDlg::OnBnClickedGetipaddress()
{
	uint32 uiPayloadValues[2] = {MANDOLIN_SS_SYSTEM_IP_ADDRESS,0};	

	m_msgTx.payload = m_msgTxPayload;
	m_MandolinComm.CreateGeneric(&m_msgTx, m_nTxSequence,MANDOLIN_MSG_GET_SYSTEM_STATUS,1,uiPayloadValues);
	TryToSendMandolinMessage(&m_msgTx, true);

}






void CRUglyDlg::OnBnClickedComputestatecrc()
{
	// TODO: Add your control notification handler code here
	uint32 uiCRC;
	char pcBuffer[100];
	switch(m_nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		uiCRC = m_ProductOLY.ComputeCRC();
		sprintf(pcBuffer,"%8.8X",uiCRC);
		SetDlgItemText(IDC_COMPUTESTATECRC_VALUE,pcBuffer);
		break;
	}

}

void CRUglyDlg::OnBnClickedComputefircrc()
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	uint32 uiCRC;
	char pcBuffer[100];
	switch(m_nProductType)
	{
	case ePRODUCT_OLYSPEAKER:
		uiCRC = m_ProductOLY.ComputeFIRCRC();
		sprintf(pcBuffer,"%8.8X",uiCRC);
		SetDlgItemText(IDC_COMPUTEFIRCRC_VALUE,pcBuffer);
		break;
	}
}

void CRUglyDlg::OnBnClickedScannetwork()
{
	unsigned char addr1,addr2,addr3,addr4;
	uint32 nIPAddress;

	CIPAddressCtrl* pctrIPAddr = (CIPAddressCtrl*)GetDlgItem(IDC_SOCKET_IPADDRESS1);

	if (pctrIPAddr)
	{

		pctrIPAddr->GetAddress(addr1,addr2,addr3,addr4);

		nIPAddress = ((addr1 & 0x0ff)<<24) + ((addr2& 0x0ff)<<16) + ((addr3 & 0x0ff)<<8)+ 1; //(addr4 & 0x0ff);

		//m_dlgScanNetwork.SetIPAddress(nIPAddress,255);
	}
	// TODO: Add your control notification handler code here
	m_dlgScanNetwork.ShowWindow( SW_RESTORE  );

}



void CRUglyDlg::OnBnClickedSelectFwFile()
{
	// TODO: Select Firmware File to send to Connected Device
	char szSelectFileTitle[_MAX_PATH];
	char szSelectFileFilter[256];
	BOOL bStatus;
	char szInputFileName[_MAX_PATH];

	strcpy(szInputFileName, m_firmwareFile.m_strFirmwareFileName.GetBuffer());
	m_firmwareFile.m_strFirmwareFileName.ReleaseBuffer();

	OPENFILENAME ofnSelectFileOpen = {
	0,					//   DWORD        lStructSize;
	NULL,				//   HWND         hwndOwner;
	0,					//   HINSTANCE    hInstance;
	szSelectFileFilter,	//   LPCWSTR      lpstrFilter;
	NULL,				//   LPWSTR       lpstrCustomFilter;
	256,				//   DWORD        nMaxCustFilter;
	1,					//   DWORD        nFilterIndex;
	szInputFileName,	//   LPWSTR       lpstrFile;
	256,				//   DWORD        nMaxFile;
	szSelectFileTitle,	//  LPWSTR       lpstrFileTitle;
	256,				//   DWORD        nMaxFileTitle;
	NULL,				//szSelectFileInitialDir,// LPCWSTR      lpstrInitialDir;
	"Select Firmware File",	//   LPCWSTR      lpstrTitle;
	0,					//   DWORD        Flags;
	0,					//   WORD         nFileOffset;
	0,					//   WORD         nFileExtension;
	"*.img",			//   LPCWSTR      lpstrDefExt;
	0,					//   LPARAM       lCustData;
	NULL,				//   LPOFNHOOKPROC lpfnHook;
	NULL				//   LPCWSTR      lpTemplateName;
	};

	sprintf(szSelectFileFilter,
		"%s%c%s%c%s%c%s%c",
		"IMG files (*.img)", 0, "*.img", 0,
		"All Files", 0, "*.*", 0, 0);

	if (NULL != strstr(szInputFileName, ".img")) {
		ofnSelectFileOpen.nFilterIndex = 1;
	}

	ofnSelectFileOpen.lStructSize = sizeof(ofnSelectFileOpen);
	ofnSelectFileOpen.hwndOwner = this->m_hWnd;
	bStatus = GetOpenFileName(&ofnSelectFileOpen);
	if (bStatus)
	{
		strcpy(m_pcFirmwareFileName, ofnSelectFileOpen.lpstrFile);

		//m_SettingsFile.m_strSettingsFileName = ;
	}
	else
	{
		m_pcFirmwareFileName[0] = 0;
		//m_SettingsFile.m_strSettingsFileName.Empty();
	}

	char* pName = strrchr(m_pcFirmwareFileName, '\\' );
	if (pName)
		pName++;	// Skip '\' character to get to filename without path
	else
		pName = m_pcFirmwareFileName;

	SetDlgItemText(IDC_SELECT_FW_FILE, pName);
	//OnSetFileDisplayNames();

	CString strErrorMsg;
	m_firmwareFile.FirmwareFileInit(m_pcFirmwareFileName, strErrorMsg);
	if (!strErrorMsg.IsEmpty())
	{
		UpdateOutputText(strErrorMsg); // SetDlgItemText(IDC_RICHEDIT_OUTPUT, strErrorMsg);
	}
}


void CRUglyDlg::OnBnClickedSendFwFile()
{
	m_firmwareFile.OnStart();
}


