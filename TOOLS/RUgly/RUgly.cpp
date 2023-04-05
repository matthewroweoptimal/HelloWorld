
// RUgly.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "RUgly.h"
#include "RUglyDlg.h"


#include "ComStuff.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



static char PCProfile[] = "RUglyVariables";



// CRUglyApp

BEGIN_MESSAGE_MAP(CRUglyApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CRUglyApp construction

CRUglyApp::CRUglyApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CRUglyApp object

CRUglyApp theApp;


// CRUglyApp initialization

BOOL CRUglyApp::InitInstance()
{
//TODO: call AfxInitRichEdit2() to initialize richedit2 library.
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	if (!AfxInitRichEdit2())
	{
		AfxMessageBox("Failed to Initialize Rich Edit2");
		return FALSE;
	}
	//if (!AfxSocketInit())
	//{
	//	AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
	//	return FALSE;
	//}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CRUglyDlg dlg;
	m_pMainWnd = &dlg;

	dlg.m_nComType = GetProfileInt(PCProfile, "CommType", COMTYPE_COM);
	dlg.m_nConnectionPortNumber = GetProfileInt(PCProfile, "CommPortNumber", 1);
	dlg.m_nConnectionBaud = GetProfileInt(PCProfile,"CommPortBaud",BAUDRATE_115200);
	dlg.m_nConnectionParity = GetProfileInt(PCProfile,"CommPortParity",0);
	strcpy(dlg.m_pcSettingsFileName, GetProfileString(PCProfile,"SettingsFile",""));

	dlg.m_nSocketIPAddress	= GetProfileInt(PCProfile,"Socket Address",(10<<24) + (17<<16) + (1<<8)+ (29));
	dlg.m_nSocketPort		= GetProfileInt(PCProfile,"Socket Port",50001);
	dlg.m_nSocketType = TCPClient;


	dlg.m_nScanNetworkIPAddress	= GetProfileInt(PCProfile,"Scan Network Address",(10<<24) + (17<<16) + (1<<8)+ (1));
	dlg.m_nScanNetworkIPEnd	= GetProfileInt(PCProfile,"Scan Network Address End",255);

	INT_PTR nResponse = dlg.DoModal();

	WriteProfileInt(PCProfile, "CommType", dlg.m_nComType);
	WriteProfileInt(PCProfile,"CommPortNumber",dlg.m_nConnectionPortNumber);
	WriteProfileInt(PCProfile,"CommPortBaud",dlg.m_nConnectionBaud);
	WriteProfileInt(PCProfile,"CommPortParity",dlg.m_nConnectionParity );
	WriteProfileString(PCProfile,"SettingsFile",dlg.m_pcSettingsFileName);

	WriteProfileInt(PCProfile,"Socket Address",dlg.m_nSocketIPAddress);
	WriteProfileInt(PCProfile,"Socket Port",dlg.m_nSocketPort);

	WriteProfileInt(PCProfile,"Scan Network Address",dlg.m_nScanNetworkIPAddress);
	WriteProfileInt(PCProfile,"Scan Network Address End",dlg.m_nScanNetworkIPEnd);


	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
