
// RUglyDlg.h : header file
//

#pragma once
#include "afxcmn.h"



#include "ComStuff.h"
#include "MandolinComm.h"
#include "ParameterAdjust.h"
#include "SettingsFile.h"
#include "FirmwareFile.h"
#include "MetersDlg.h"
#include "DSPSignalFlow.h"
#include "ZImpedanceDlg.h"
#include "OlyImpedanceDlg.h"
#include "CreatePresetHeaders.h"
#include "LevelAdjust.h"
#include "ActivePreset.h"
#include "ParameterTest.h"
#include "EQGraph.h"
#include "TestManuDlg.h"
#include "ScanNetwork.h"

#include "CommSettings.h"
#include "SocketSettings.h"
#include "Product_arch.h"
//#include "OLYSpeaker1_arch.h"
extern "C"
{

#include "ProductMap.h"
}
#include "OLY_MT_enums.h"
#include "MandolinCommOLYTestManu.h"
#include "MandolinTextSummary.h"
#include "afxwin.h"

#include "AppProductDefs_OLY.h"
#include "AppProductDefs_REDLINE.h"

typedef enum {
	MANDOLIN_AE_STORE_USER_EDIT = 0x40,		// CDD "store current to user" function
	MANDOLIN_AE_CANCEL_USER_EDIT = 0x41,	// CDD "cancel edit and revert to previous" function
	MANDOLIN_AE_SET_AVAILABLE = 0x42,		// OLY "logo indicate available for Rig"
	MANDOLIN_AE_SET_SELF_TEST_MODE = 0x43,	// Request to set OLY self test mode
	MANDOLIN_AE_START_AUTO_OPTIMIZE = 0x44,	// OLY start the auto-optimize process
	MANDOLIN_AE_RESET_AUTO_OPTIMIZE = 0x45	// OLY clear the auto-optimize settings
} oly_application_event;

// CRUglyDlg dialog

// equates
// --------------------------------------------------------------------------------------
// structures
// --------------------------------------------------------------------------------------

//typedef struct tCurrentState_tag {
//	//uint32 utf8SystemLabel[MAX_SYSTEMLABEL];
//	void* vpParameterBuffer[SNAPSHOT_PARAMETER_MAX];
//	void* vpGlobalParameterBuffer[GLOBAL_PARAMETER_MAX];
//	void* vpFIRParameterBuffer[FIR_PARAMETER_MAX];
//	//void* vpInfoParameterBuffer[INFO_PARAMETER_MAX];
//	//char pcShowStringParameterBuffer[eLID_SNAPSHOT_FENCE][SNAPSHOT_DISPLAYNAME_MAX];
//} tCurrentState;


typedef enum {
    COMM_UNKNOWN,
    COMM_RS232_UART,
    COMM_RS232_USB,
    COMM_RS485_REMOTE,
    COMM_LOGIC,
    COMM_ETHERNET_TCP
} comm_method;

typedef struct {
    bool32            reply;        // the message is an auto-generated reply and does not need (further) handling
    bool32            forward;      // forward the message to all (MANDOLIN capable) devices other than the sender
    comm_method       sender_type;
    int32             sender_ID;    // port, socket, or other device ID #
    mandolin_message *bmhsp;        // message header pointer
} mandolin_message_wrapper_s;




// equates

typedef enum {
	  BAUDRATE_4800  =0
	, BAUDRATE_9600
	, BAUDRATE_14400
	, BAUDRATE_19200
	, BAUDRATE_38400
	, BAUDRATE_56000
	, BAUDRATE_57600
	, BAUDRATE_115200
	, BAUDRATE_128000
	, BAUDRATE_256000
	, BAUDRATE_921600
//	, BAUDRATE_31250
	, BAUDRATE_MAX
} BaudRates;

extern int gBaudRates[BAUDRATE_MAX];

#define COMTYPE_NONE 0
#define COMTYPE_COM  1
#define COMTYPE_USB  2
#define COMTYPE_SOCKET  3
#define COMTYPE_CONMON  4

#define	SCALETYPE_LINEAR	0
#define	SCALETYPE_LOG		1


#define	MAX_RX_DISPLAY					(4096*15)

#define SETCHECK(a,b)			( ( (CButton *)		(GetDlgItem( (a) ) ) )->SetCheck(	(b)		  ) )
#define GETCHECK(a)				( ( (CButton *)		(GetDlgItem( (a) ) ) )->GetCheck(			  ) )

#define INPUTTEXTBOXFILTER_RAW			0	
#define INPUTTEXTBOXFILTER_DEVICE		1
#define INPUTTEXTBOXFILTER_INTERPRET	2

typedef enum {
    MSGTIMEOUT_TIMERID = 1,
	TICKCOUNT_TIMERID,
	GETSTATE_TIMERID, 
	ZIMPEDANCE_TIMERID,
	SENDSTATE_TIMERID, 

	//SENDSTATE_TIMERID,
	KEEPALIVE_TIMERID,
	//DRAWALLMETERS_TIMERID,
	METER_TIMERID,
	//PARAMETERRANDOMTEST_TIMERID,
	//ECHOTEST_TIMERID,
	MSGSEND_TIMERID,
	//UPGRADE_TIMERID,
	SETTINGSFILE_TIMERID,
	FIRMWAREFILE_TIMERID,
	EQGRAPH_COEFFSDIRTY_TIMERID,
	PARAMETERADJUST_GETPARAMETER_TIMERID,
	PARAMETERTEST_TIMERID,

	OLYIMPEDANCE_TIMERID,
};

typedef enum {
	MANDOLIN_CONNECT_NONE = 0,
	MANDOLIN_CONNECT_SE_CONNECT_SEND,				// send the Connect msg
	MANDOLIN_CONNECT_SE_CONNECT_WAITINGACK,			// waiting ack from fw
	MANDOLIN_CONNECT_SS_MAX_MSG_WORDS_WAIT,			// fw will send MAX mesage size
	MANDOLIN_CONNECT_SS_MAX_MSG_WORDS_SEND,			// send a MAX message size	
	MANDOLIN_CONNECT_SS_MAX_MSG_WORDS_WAITINGACK,	// waitng ack from fw
	MANDOLIN_CONNECT_SE_CONNECT_WAIT,				// fw will send a Connect
	MANDOLIN_CONNECT_SE_SYNC_CURRENT_STATE_SEND,	// send a sync command
	MANDOLIN_CONNECT_SE_SYNC_CURRENT_STATE_WAITINGACK,	// waiting ack
	MANDOLIN_CONNECT_MAX,						
} tMandolinConnectState;

//typedef enum enumMandolinConnectState {
//	mcsDisconnected,			//	No CONNECT mesg received
//	mcsConnectReceived,			//	CONNECT message received,
//	mcsGetHardInfoSent,			//	GetHardInfo Sent, awaiting reply
//	mcsGetHardInfoReplyReceived,//	Reply for Get Hardware Info received,
//	mcsGetMaxMsgSent,			//	GetMaxMsg Sent, awaiting reply
//	mcsGetMaxMsgReplyReceived,	//	GetMaxMsg reply received, awaiting GetMaxMsg command
//	mcsGetMaxMsgReceived,		//	GetMaxMsg Received
//	mcsConnected				//	Connect message sent, awaiting Sync
//} MANDOLIN_CONNECT_STATE;


class CRUglyDlg : public CDialog
{
// Construction
public:
	CRUglyDlg(CWnd* pParent = NULL);	// standard constructor
	~CRUglyDlg();

// Dialog Data
	enum { IDD = IDD_RUGLY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCommSettings();
	afx_msg void OnSocketSettings();
	afx_msg void OnHelpAbout();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);


	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedPing();

public:
	CRichEditCtrl m_OutputTextBox;
	CRichEditCtrl m_InputTextBox;

	CString m_strResponseStatusDisplay;

	void GetFileVersionInfo();
	CString m_strProductName;
	CString m_strProductVersion;
	//CString m_strProductDate;
	CString m_strCompanyName;
	CString m_strCopyright;


	char	m_pcFirmwareVersion[200];

	int m_nProductType;
	void SetProducttype(int nProduct);
	void ShowProducttype();
	void ConvertBrandModel2Text(char* pcBrand, char* pcModel, uint32 uiBrand, uint32 uiModel);
	void SetBrandModel(uint32 uiBrand, uint32 uiModel);

	AppProductDefs_OLY		m_ProductOLY;
	AppProductDefs_REDLINE	m_ProductREDLINE;

	//tCurrentState m_CurrentState;
	//float m_gMeterLevel[METER_PARAMETER_MAX];


	int GetParameterDescTable(int nProduct, int nTarget, tParameterDesc** ppParameterDesc, int* pTableLength, int* nXMLVersion);
	tParameterDesc* GetParameterDesc(int nProductType, int nTarget, int nParamNum);
	int GetMeterType(int nProductType,  int nTarget, int nParamNum);
	unsigned int*	GetMeterSubscriptionList(int nProductType, int nSelect, int* pnSize);


	int CalcScaleType(tParameterDesc* pParameterDesc);
	void**  GetParamAddress(int nTarget, int nParam, int nInstance = 0);
	int  GetParameterScaleType(tParameterDesc* pParameterDesc);
	float NormalizeParameterValue(tParameterDesc* pParameterDesc, intfloat theParamValue);
	void	ShowParameter(int nTarget, int nParam, bool bUpdate,bool bAll = false);
	void ShowMeter(int nTarget, int nParam, float fValue);
	void GetParameterValueString(char* pcValue,tParameterDesc*  pParameterDesc, int nTarget, int nParam);



	int		GetRandomTarget(int* nTarget, int* nInstance);
	intfloat  GetParameterRandomValue(tParameterDesc* pParameterDesc, char* pcBuffer);

	bool	IsTargetFIR(int m_nProductType,int nTarget);
	bool	IsTargetMeters(int m_nProductType,int nTarget);
	int		GetMetersTarget(int m_nProductType);

	// -------------------------------------
	// Input/Output windows
	// -------------------------------------

	void UpdateOutputText(CString theText, bool bAppend = true, bool bForce=false, bool bLineFeed = true);
	void UpdateInputText(CString theText, bool bAppend = true, bool bForce=false);
	bool m_bFreezeDisplay;
	int m_nInputTextBoxFilter;
	void	OnShowInputFilter();
	void	OnShowFreeze();
	

	// ---------------------------------------------------
	// Dialogs
	// ---------------------------------------------------
	ParameterAdjust m_dlgParameterAdjust;
	CLevelAdjust m_dlgLevelAdjust;
	CActivePreset m_dlgActivePreset;
	CParameterTest m_dlgParameterTest;
	
	CScanNetwork m_dlgScanNetwork;
	unsigned int m_nScanNetworkIPAddress;
	unsigned int m_nScanNetworkIPEnd;


	// ---------------------------------------------------
	// Display
	//------------------------------------------------------

	CBrush m_RedBrush;
	CBrush m_YellowBrush;
	CBrush m_BlueBrush;
	CBrush m_GreenBrush;

	int PostMessageBox( LPCTSTR lpszText, LPCTSTR lpszCaption = NULL, UINT nType = MB_OK );

	// ---------------------------------
	// &&& Comm Port Init
	// ---------------------------------

	afx_msg LRESULT OnRxData(WPARAM wParam, LPARAM lpData);

	cpINFO	*pCPCommPort;
//	int		m_nConnectionStopBits;
	int		m_nComType;
	bool	m_bIsConnected;
	int		m_nConnectionPortNumber;
	int		m_nConnectionBaud;
	int		m_nConnectionParity;

	bool	InitComPort(int nPort,int nBaud = BAUDRATE_115200, int theParity = NOPARITY);
	//void	OnSetComportConnectButtonText();
	void	OnChangeCommSettings(int nCommPort, int nCommBaudRate, int nCommParity);

	int		nMsgTimeOutRate;

	bool	bCommPortSemaphore;

	// ---------------------------------
	// &&& Socket Init
	// ---------------------------------
	SocketCommInfo* pSocketCommInfo;
	bool	InitSocketComm(UInt nIPAddress, int nPort,SocketType nSocketType = TCPServer );
	bool	OpenSocketComm(void);
	bool	CloseSocketComm(void);
	unsigned int m_nSocketIPAddress;
	int m_nSocketPort;
	SocketType m_nSocketType;
	void	OnChangeSocketSettings(unsigned int nIP, int nPort,SocketType nType);
	void	ShowIPAddress();


	// ---------------------------------
	// parameter list
	// ---------------------------------
	unsigned int m_nParameterList[2][PARAMETERLIST_MAX];
	int m_nParameterListTarget[2];
	int m_nParameterListId[2];
	int m_nParameterListNum[2];
	void SetParameterList(int nListNum, int nId, int nNumPIDs, int nTarget, unsigned int* pList);


	// -------------------------------------
	// Mandolin Handling
	// -------------------------------------

	CMandolinComm m_MandolinComm;
	CMandolinCommOLYTestManu m_MandolinCommOLYTestManu;
	CMandolinTextSummary	m_MandolinTextSummary;
	mandolin_message m_msgTx;
	uint8 m_msgTxPayload[MANDOLIN_MAX_PAYLOAD_DATA_WORDS*MANDOLIN_BYTES_PER_WORD];
	unsigned char m_pTxMsgBuffer[1024*3];
	int m_nTxSequence; 


	bool m_bMessageReceived;
	bool  TryToSendMandolinMessage(mandolin_message* pMsg, bool bExpectingResponse);
	bool  TryToSendMessage(char * theMessage , int nLength, bool bExpectingResponse);
	void  MessageSend( char * theMessage , int nLength);

	int		CopyMandolinMessage(mandolin_message* pDest, mandolin_message* pSrc);
	
	bool	Send_reply_ACK(mandolin_message* pMessage);
	bool	Send_reply_withsubid_ACK(mandolin_message* pMessage, unsigned int uiSubId);
	bool	Send_reply_withpayload(mandolin_message* pMessage, unsigned int* pPayload, int nLength);



	mandolin_message mInMsg;
	uint8 mInMsgPayload[MANDOLIN_MAX_PAYLOAD_DATA_WORDS*MANDOLIN_BYTES_PER_WORD];
	mandolin_message_wrapper_s bmwsRxMsgWrapper;
	int m_bExpectingResponse;
	int m_nExpectingResponseSequence;

	mandolin_fifo m_bfpRxFifo;
	uint8 m_uiRxFifoBuffer[MANDOLIN_FIFO_BYTES]; 
	bool MANDOLIN_handle_Process(mandolin_message_wrapper_s* bmwsp, mandolin_fifo *bfp);
	void	ProcessMessage(mandolin_message* theMessage, int theLength);
	void	ProcessMessageParameterSet(mandolin_message* theMessage, int theLength);
	void	ProcessMessageParameterSetReply(mandolin_message* pInMsg, int theLength);
	void	ProcessMessageParameterGetReply(mandolin_message* theMessage, int theLength);
	void	ProcessMessageParameterGetReplyMeter(mandolin_message* pInMsg, int theLength);
	void	ProcessMessageParameterStringSet(mandolin_message* theMessage, int theLength);
	void	DecodeGetSystemStatus(mandolin_message* theMessage, int theLength);
	void	DecodeGetSystemStatusReply(mandolin_message* theMessage, int theLength);

	void	DecodeGetSoftwareInfoReply(mandolin_message* theMessage, int theLength);


	uint32 m_nMandolinMaxMsgSize;
	uint32 m_uiMandolinVersion;
	uint32 m_uiK64Version;
	void	ShowSofwareInfo();
	void	OnGetSoftwareVersion();
	void	OnGetSystemStatusMaxMessageSize();
	void	OnSendSyncState();
	void	OnSendConnect(int nConnectType);

	int m_bMandolinConnectStateStart;	// if started
	int m_nMandolinConnectStateState;
	void MandolinConnectStateSetState(int nState);
	void MandolinConnectStateIncrementState();
	void MandolinConnectStateDoState();

	//mandolin_fifo bfpTxFifo;
	//uint8 uiTxFifoBuffer[MANDOLIN_FIFO_BYTES]; 

	int SendParamEdit(int nTarget,int nParam, unsigned int uiValue);
	int SendParamEditMulti(int nTarget, uint32* nParam, intfloat* jValue, int nNumofParams);
	//int SendRawMessage(char *pMsgBuffer, uint16 nLength, char* pcDeviceName = 0);
	int SendParamEditGet(int nTarget,int nParam);
	int SendParamGetMulti(int nTarget, uint32* nParam, int nNumofParams);


	// -------------------------------------
	// Parameter Tables
	// -------------------------------------
	intfloat ConvertString2Value(CString theString, int& nType);
	intfloat GetDlgItemValue(int nIDC, int& nType);
	tParameterDesc* ConvertString2ParameterDesc(int nProductType,char* pName,int* nTarget, int* nPID);

	// -------------------------------------
	// Setting Files
	// -------------------------------------
	CSettingsFile m_SettingsFile;
	char	m_pcSettingsFileName[_MAX_PATH];

	// -------------------------------------
	// Firmware File
	// -------------------------------------
	CFirmwareFile m_firmwareFile;
	char	m_pcFirmwareFileName[_MAX_PATH];


	// -------------------------------------
	// Meters
	// -------------------------------------
	CMetersDlg m_dlgMeters;

	// -------------------------------------
	// DSP Signal Flow
	// -------------------------------------
	CDSPSignalFlow m_dlgDSPSignalFlow;

	// -------------------------------------
	// EQ Graph
	// -------------------------------------
	CEQGraph m_dlgEQGraph;

	// -------------------------------------
	// Z Impedance  (for Redline)
	// -------------------------------------
	CZImpedanceDlg m_dlgZImpedance;


	// -------------------------------------
	// Oly Impedance
	// -------------------------------------
	COlyImpedanceDlg m_dlgOlyImpedance;


	// -------------------------------------
	// CREAte preset headers 
	// -------------------------------------
	CCreatePresetHeaders m_dlgCreatePresetHeaders;

	// -------------------------------------
	// Test Manufacturing
	// -------------------------------------
	CTestManuDlg m_dlgTestManu;

	// -------------------------------------
	// Parameter Tables
	// -------------------------------------

	bool m_bGetStateStarted;
	int	m_nGetStatePos;
	int m_nGetStateRequestSize;
	void GetStateRequestNext();
	void OnShowGetstatefromdevice();


	int	m_nSendStatePos;
	int m_nSendStateRequestSize;
	void SendStateSendNext();


	// -------------------------------------
	// Parameter Adjust
	// -------------------------------------
	void ParameterAdjustSetComboBox(int nProductType, CComboBox** pCombo, int nNum);

	// -------------------------------------
	// Level Adjust interface
	// -------------------------------------

	int GetLevelSwitchDescList(int nProductType, ControlDesc** pList);
	int GetLevelFaderDescList(int nProductType, ControlDesc** pList);
	int GetLevelEditBoxDescList(int nProductType, ControlDesc** pList);
	int GetLevelTitleList(int nProductType, tStringIDC** pList);

	// -------------------------------------
	// Meter Dialog
	// -------------------------------------

	int GetMetersDescList(int nProductType, ControlDesc** pList);
	int GetMetersTitleList(int nProductType, tStringIDC** pList);

	afx_msg void OnBnClickedParameteradjust();
	afx_msg void OnBnClickedDisplaymessageformatRaw();
	afx_msg void OnBnClickedDisplaymessageformatInterpret();
	afx_msg void OnBnClickedFreeze();
	afx_msg void OnBnClickedSettingsfileSelect();
	afx_msg void OnBnClickedSettingsfileSend();
	afx_msg void OnBnClickedSettingsfileStop();
	afx_msg void OnBnClickedSavestate();
	afx_msg void OnBnClickedSettingsfileLoad();
	afx_msg void OnBnClickedProducttypeOly();
	afx_msg void OnBnClickedProducttypeRedline();
	afx_msg void OnBnClickedCleardisplay();
	afx_msg void OnBnClickedSavestatefir();
	afx_msg void OnBnClickedGetstatefromdevice();
	afx_msg void OnBnClickedGetFIRstatefromdevice();
	afx_msg void OnBnClickedSnapshotstore();
	afx_msg void OnBnClickedSnapshotload();
	afx_msg void OnBnClickedZimpedancedlg();
	afx_msg void OnBnClickedCreaterandomsnapshot();
	afx_msg void OnBnClickedSendstatetodevice();
	afx_msg void OnBnClickedShowstate2();
	afx_msg void OnBnClickedSetstatetodefaults();
	afx_msg void OnBnClickedLeveladust();
	CScrollBar m_ScrollBar;
	afx_msg void OnBnClickedMeters();
	afx_msg void OnBnClickedDspsignalflow();
	afx_msg void OnBnClickedEqgraph();
	afx_msg void OnBnClickedMandolinConnnect();
	afx_msg void OnBnClickedActivepreset();
	afx_msg void OnBnClickedMandolinDisconnnect();
	afx_msg void OnBnClickedParametertest();
	afx_msg void OnBnClickedSyncRequest();
	afx_msg void OnBnClickedGetHwInfo();
	afx_msg void OnBnClickedGetSwInfo();
	afx_msg void OnBnClickedOlyimpedancedlg();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedCreatepresetheaders();
	afx_msg void OnBnClickedTestmanu();
	afx_msg void OnBnClickedStoresuseredit();
	afx_msg void OnBnClickedCancelsuseredit();
	afx_msg void OnBnClickedIdentifyOn();
	afx_msg void OnBnClickedIdentifyOff();
	afx_msg void OnBnClickedSocketConnect();
	afx_msg void OnBnClickedSocketDisconnect();
	afx_msg void OnBnClickedSelftestmode();
	afx_msg void OnBnClickedGetipaddress();
	afx_msg void OnBnClickedCreaterandomfir();
	afx_msg void OnBnClickedSendstatetodevicestop();
	afx_msg void OnBnClickedSendstatetodevicefir();
	afx_msg void OnBnClickedSetstatetodefaultsfir();
	afx_msg void OnBnClickedShowstatefir();
	afx_msg void OnBnClickedComputestatecrc();
	afx_msg void OnBnClickedComputefircrc();
	afx_msg void OnBnClickedScannetwork();
	afx_msg void OnBnClickedStartautooptimize();
	afx_msg void OnBnClickedResetautooptimize();
	afx_msg void OnBnClickedSelectFwFile();
	afx_msg void OnBnClickedSendFwFile();
};
