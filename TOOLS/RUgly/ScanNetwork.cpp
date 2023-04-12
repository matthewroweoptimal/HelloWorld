// ScanNetwork.cpp : implementation file
//

#include "stdafx.h"
#include "RUgly.h"
#include "ScanNetwork.h"

#include "RUglyDlg.h"


#include "resource.h"

// CScanNetwork dialog

IMPLEMENT_DYNAMIC(CScanNetwork, CDialog)

CScanNetwork::CScanNetwork(CWnd* pParent /*=NULL*/)
	: CDialog(CScanNetwork::IDD, pParent)
{
	m_dlgParent = NULL;

	m_nFoundSpeakers = 0;

}

CScanNetwork::~CScanNetwork()
{
}

void CScanNetwork::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CScanNetwork, CDialog)
	ON_BN_CLICKED(IDC_SCANNETWORK_START, &CScanNetwork::OnBnClickedScannetworkStart)
	ON_BN_CLICKED(IDC_SCANNETWORK_FINDANDCONNECT, &CScanNetwork::OnBnClickedScannetworkFindandconnect)
	ON_BN_CLICKED(IDC_SCANNETWORK_CLEARARP, &CScanNetwork::OnBnClickedScannetworkCleararp)
END_MESSAGE_MAP()


void CScanNetwork::SetIPAddress(uint32 nIPAddress,uint32 nEnd)
{
	unsigned char addr1 = 10;
	unsigned char addr2 = 17;
	unsigned char addr3 = 1;
	unsigned char addr4 = 1;

	CIPAddressCtrl* pctrIPAddr = (CIPAddressCtrl*)GetDlgItem(IDC_SCANNETWORK_STARTIP);
	if (pctrIPAddr)
	{
		addr1 = (nIPAddress>>24)&0x0ff;
		addr2 = (nIPAddress>>16)&0x0ff;
		addr3 = (nIPAddress>>8)&0x0ff;
		addr4 = (nIPAddress>>0)&0x0ff;
		pctrIPAddr->SetAddress(addr1,addr2,addr3,addr4);

		m_dlgParent->m_nScanNetworkIPAddress = nIPAddress;

	}
	SetDlgItemInt(IDC_SCANNETWORK_ENDIP,nEnd);
	m_dlgParent->m_nScanNetworkIPEnd = nEnd;;
	

}


// CScanNetwork message handlers

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>

std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = _popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL)
                result += buffer;
        }
    } catch (...) {
        _pclose(pipe);
        throw;
    }
    _pclose(pipe);
    return result;
}


bool CScanNetwork::ReadLineFromBuffer(char* pDest, char* pSrc, int* pnSrcPos, int nMaxLineLength)
{
	int nSrcLength = strlen(pSrc);
	bool bDone = false;
	char c;
	int nCount = 0;
	int nPos;

	
	*pDest = 0;
	if (*pnSrcPos >= nSrcLength) return true;
	nPos = *pnSrcPos;

	while ((nPos < nSrcLength) && !bDone)
	{
		c = pSrc[nPos++];
		if (pSrc[nPos] == '\n')
		{
			bDone = true;
			*pDest = 0;
		}
		else
		{
			*pDest = c;
			pDest++;
			nCount++;
			if (nCount >= (nMaxLineLength+1)) 
			{
				bDone = true;
				*pDest = 0;
			}

		}

	}
	*pnSrcPos = nPos;

	return false;
	
}

int CScanNetwork::GetToken(char* pcToken, char* pcSrc, int& nPos, int nSize)
{

	// returns length of token

	int	nLength = 0;
	
	*pcToken = 0;
	if (nPos >= nSize) 
		return nLength;


	// skip spaces
	while ( (isspace(pcSrc[nPos]) != 0) && (nPos<nSize))
	{
		nPos++;
	}
	
	// fill token
	while ( (isspace(pcSrc[nPos]) == 0) && (nPos<nSize) && 
		((nLength == 0) || (pcSrc[nPos] != ';')) )
	{
		pcToken[nLength++] = pcSrc[nPos++];
	}
	pcToken[nLength] = 0;

	if (nLength>1)
	{
		if ((pcToken[nLength-1] == ',') ||  (pcToken[nLength-1] == ':'))
		{
			pcToken[nLength-1] = 0;
		}
	}
	return nLength;

	
}
bool CScanNetwork::ParseArpLine(char* pcLine, uint32* IPAddr, unsigned char* macaddr)
{
	int nPos = 0;
	char pcToken[256];
	int nTokenLength;
	int ipaddr[6];
	unsigned long nIPValue;
	*IPAddr = 0;
	macaddr[0] = 0;
	macaddr[1] = 0;
	macaddr[2] = 0;
	macaddr[3] = 0;
	macaddr[4] = 0;
	macaddr[5] = 0;


	int nLineLength = strlen(pcLine);
	if (nLineLength == 0) return false;

	// get the IP address
	nTokenLength = GetToken(pcToken,pcLine,nPos,nLineLength);
	if (nTokenLength < 1) return false;

	// read the IP Address
	// num.num.num.num
	nIPValue = inet_addr(pcToken);
	*IPAddr = ((nIPValue & 0x0ff) << 24) + ((nIPValue & 0x0ff00) << 8) + ((nIPValue & 0x00ff0000) >> 8) + ((nIPValue & 0xff000000) >> 24);

	// get the Mac address
	nTokenLength = GetToken(pcToken,pcLine,nPos,nLineLength);
	if (nTokenLength < 1) return false;

	if( 6 == sscanf( pcToken, "%x-%x-%x-%x-%x-%x%c",
		&ipaddr[0], &ipaddr[1], &ipaddr[2],
		&ipaddr[3], &ipaddr[4], &ipaddr[5] ) )
	{
		/* convert to uint8_t */
		for(int i = 0; i < 6; ++i )
			macaddr[i] = (unsigned char) (ipaddr[i] & 0x0ff);
	}
	else
	{
		return false;
	}

	return true;

}

void CScanNetwork::AddStringToOutput(char* pcNewLine, bool bAppend)
{
	CString strTextOutput;

	GetDlgItemText(IDC_SCANNETWORK_OUTPUT,strTextOutput);

	if (!bAppend)//	if (strTextOutput.IsEmpty())
	{
		strTextOutput = pcNewLine;

	}
	else
	{
		strTextOutput += pcNewLine;
	}
	//strTextOutput += "\n";

	SetDlgItemText(IDC_SCANNETWORK_OUTPUT,strTextOutput);

}


int CScanNetwork::RunCommandinProcess(char* pcCommand)
{
//  char szPath[] = "C:\\Program\\Windows Media Player\\wmplayer.exe";
	char pcBuffer[200];
  PROCESS_INFORMATION pif;  //Gives info on the thread and..
                           //..process for the new process
  STARTUPINFO si;          //Defines how to start the program
 
  ZeroMemory(&si,sizeof(si)); //Zero the STARTUPINFO struct
  si.cb = sizeof(si);         //Must set size of structure
 
  BOOL bRet = CreateProcess(
        NULL, //Path to executable file
        pcCommand,   //Command string - not needed here
        NULL,   //Process handle not inherited
        NULL,   //Thread handle not inherited
        FALSE,  //No inheritance of handles
        CREATE_NO_WINDOW, //0,      //No special flags
        NULL,   //Same environment block as this prog
        NULL,   //Current directory - no separate path
        &si,    //Pointer to STARTUPINFO
        &pif);   //Pointer to PROCESS_INFORMATION
 
  if(bRet == FALSE)
  {
	  sprintf(pcBuffer,"Unable to run command: %s", pcCommand);
    MessageBox(pcBuffer,"Scan Network",MB_OK);
    return 1;
  }
 
  CloseHandle(pif.hProcess);   //Close handle to process
  CloseHandle(pif.hThread);    //Close handle to thread
 
  return 0;
}
void CScanNetwork::OnBnClickedScannetworkStart()
{
	// TODO: Add your control notification handler code here
	CIPAddressCtrl* pctrIPAddr;
	unsigned char addr1,addr2,addr3,addr4;

	unsigned int nIPAddressStart = 0;
	int nIPAddressEnd = 0;

	char pcBuffer[100];
	char pcCommand[200];
	char pcCommandArg1[100];
	char pcCommandArg2[100];
	intptr_t processReturn;
    std::string result = "";
	char pcLine[100];
	int nMaxLine = sizeof(pcLine)-1;
	int nPos;

	char pcMACPrefix[] = "00-0f-f2-08";
	uint32 IPAddr;
	unsigned char macaddr[6];


//	zCommandNew = "\"D:\\Test\\Setup.exe\" -s -f1\"D:\\Test\\setup_install.iss\" -f2\"D:\\Test\\setup-log.log\"";
	int iCmdRet;// = system(zCommandNew);
		

	m_nFoundSpeakers = 0;

	pctrIPAddr = (CIPAddressCtrl*)GetDlgItem(IDC_SCANNETWORK_STARTIP);
	nIPAddressEnd = GetDlgItemInt( IDC_SCANNETWORK_ENDIP );

	if (pctrIPAddr)
	{
			pctrIPAddr->GetAddress(addr1,addr2,addr3,addr4);

			if (addr4 > nIPAddressEnd)
			{
				MessageBox("End Address not Valid","Scan Network");
				return;
			}
			nIPAddressStart = ((addr1 & 0x0ff)<<24) + ((addr2& 0x0ff)<<16) + ((addr3 & 0x0ff)<<8)+ (addr4 & 0x0ff);

			m_dlgParent->m_nScanNetworkIPAddress = nIPAddressStart;
			m_dlgParent->m_nScanNetworkIPEnd = nIPAddressEnd;;


	}
	int i;
	SetDlgItemText(IDC_SCANNETWORK_STATUS,"Pinging the addresses");
	for(i=addr4;i<nIPAddressEnd;i++)
	{
		sprintf(pcBuffer,"Pinging %d.%d.%d.%d\n",addr1,addr2,addr3,i);
		SetDlgItemText(IDC_SCANNETWORK_STATUS,pcBuffer);

		sprintf(pcCommand,"ping %d.%d.%d.%d -n 1\n",addr1,addr2,addr3,i);
		//iCmdRet = system(pcCommand);
		RunCommandinProcess(pcCommand);

		//sprintf(pcCommandArg1,"%d.%d.%d.%d",addr1,addr2,addr3,i);
		//processReturn = _spawnl (P_WAIT , "cmd", pcCommand,NULL);
		//processReturn = _spawnl (P_WAIT , "ping", pcCommandArg1, "-n","1",NULL);

	}

	SetDlgItemText(IDC_SCANNETWORK_STATUS,"Get the Mac Addr");

	result = exec("arp -a");
	//iCmdRet = system("arp -a);

	SetDlgItemText(IDC_SCANNETWORK_STATUS,"Done");

	result.c_str();

	int nSize = result.size();

	char *p;
	p = (char*)malloc(nSize+10);
	if (p)
	{
		strcpy(p,result.data());

		SetDlgItemText(IDC_SCANNETWORK_OUTPUT,"");

//		SetDlgItemText(IDC_SCANNETWORK_OUTPUT,p);

		nPos = 0;
		ReadLineFromBuffer(pcLine,p,&nPos,nMaxLine);	// First line is formating
		AddStringToOutput(" IP Address \tMAC Address \t type", false);

		while(!ReadLineFromBuffer(pcLine,p,&nPos,nMaxLine))
		{
			if (strstr(pcLine,pcMACPrefix) != NULL)
			{
				// found a speaker
				AddStringToOutput(pcLine, true);


				// format:   IPaddres     MacAddress
				if (ParseArpLine(pcLine, &IPAddr, macaddr))
				{

					if (m_nFoundSpeakers < MAX_SPEAKERS)
					{
						m_nFoundSpeakersIP[m_nFoundSpeakers] = IPAddr;
						m_nFoundSpeakersMACAddr[m_nFoundSpeakers]= ((macaddr[2] & 0x0ff)<<24) + ((macaddr[3] & 0x0ff)<<16) + ((macaddr[4] & 0x0ff)<<8) + (macaddr[5] & 0x0ff) ;	//last 4 chars only
						m_nFoundSpeakers++;
					}
				}

			}
		}
		free(p);
	}
	//std::string nextline;
	//std::getline(result,nextline,"\n");
	
	sprintf(pcBuffer,"Found %d speakers",m_nFoundSpeakers);
	SetDlgItemText(IDC_SCANNETWORK_STATUS,pcBuffer);


	


}



void CScanNetwork::OnBnClickedScannetworkFindandconnect()
{
	// TODO: Add your control notification handler code here
	uint8 macaddr[6] = {0x00, 0x0f, 0xf2, 0x08, 0xff, 0xff};
	uint32 uiMACtoFind;
	uint32 uiValue;
	char *next;
	unsigned char addr1 = 10;
	unsigned char addr2 = 17;
	unsigned char addr3 = 1;
	unsigned char addr4 = 1;

	if (!m_dlgParent) return;

	char pcBuffer[100];

	//OnBnClickedScannetworkStart();

	if (m_nFoundSpeakers == 0)
	{
		sprintf(pcBuffer,"%d found, Can not connect",m_nFoundSpeakers);
		SetDlgItemText(IDC_SCANNETWORK_STATUS,pcBuffer);
	}



	GetDlgItemText(IDC_SCANNETWORK_MACADDR_VALUE1,pcBuffer,sizeof(pcBuffer)-1);
	uiValue = strtoul(pcBuffer, &next,16);
	macaddr[4]= uiValue & 0x0ff;

	GetDlgItemText(IDC_SCANNETWORK_MACADDR_VALUE2,pcBuffer,sizeof(pcBuffer)-1);
	uiValue = strtoul(pcBuffer, &next,16);
	macaddr[5]= uiValue & 0x0ff;

	bool bfound = false;
	int i = 0;

	
	uiMACtoFind	= ((macaddr[2] & 0x0ff)<<24) + ((macaddr[3] & 0x0ff)<<16) + ((macaddr[4] & 0x0ff)<<8) + (macaddr[5] & 0x0ff) ;	//last 4 chars only

	while((i<m_nFoundSpeakers) && (!bfound))
	{

		if (m_nFoundSpeakersMACAddr[i]== uiMACtoFind)
		{
			bfound = true;

		}
		else
		{
			i++;
		}
	}

	if (!bfound)
	{
		sprintf(pcBuffer,"MAC Address not found");
		SetDlgItemText(IDC_SCANNETWORK_STATUS,pcBuffer);
		return;
	}

	CIPAddressCtrl* pctrIPAddr = (CIPAddressCtrl*)m_dlgParent->GetDlgItem(IDC_SOCKET_IPADDRESS1);
	if (pctrIPAddr)
	{

		addr1 = (m_nFoundSpeakersIP[i]>>24)&0x0ff;
		addr2 = (m_nFoundSpeakersIP[i]>>16)&0x0ff;
		addr3 = (m_nFoundSpeakersIP[i]>>8)&0x0ff;
		addr4 = (m_nFoundSpeakersIP[i]>>0)&0x0ff;

		sprintf(pcBuffer,"Found Match:  %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x  %d.%d.%d.%d",
			macaddr[0],macaddr[1],macaddr[2], macaddr[3],macaddr[4],macaddr[5],
			addr1,addr2,addr3,addr4	);

		SetDlgItemText(IDC_SCANNETWORK_STATUS,pcBuffer);

	
		pctrIPAddr->SetAddress(addr1,addr2,addr3,addr4);
		m_dlgParent->m_nSocketIPAddress = m_nFoundSpeakersIP[i];
	
		m_dlgParent->OnBnClickedSocketDisconnect();
		m_dlgParent->OnBnClickedSocketConnect();

	}
	

}

void CScanNetwork::OnBnClickedScannetworkCleararp()
{
	// TODO: Add your control notification handler code here
	char pcCommand[] = "netsh interface ip delete arpcache";
	HWND pHWND;


	pHWND = GetSafeHwnd();
	ShellExecute(pHWND, "runas", pcCommand, 0, 0, SW_SHOWNORMAL);
	
}
