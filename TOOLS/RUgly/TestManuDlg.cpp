// TestManuDlg.cpp : implementation file
//


#include "stdafx.h"
#include "RUgly.h"
#include "TestManuDlg.h"

#include "RUglyDlg.h"
#include "ProductMap.h"
#include <math.h>
#include "Product_arch.h"


#include "LOUD_defines.h"

// CTestManuDlg dialog

IMPLEMENT_DYNAMIC(CTestManuDlg, CDialog)

CTestManuDlg::CTestManuDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestManuDlg::IDD, pParent)
{
	int j;
	m_dlgParent = NULL;
	m_nProductType = ePRODUCT_OLYSPEAKER;

}

CTestManuDlg::~CTestManuDlg()
{
}

void CTestManuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTestManuDlg, CDialog)
	ON_BN_CLICKED(IDC_TESTMANU_SETFUNCGENOUTPUT, &CTestManuDlg::OnBnClickedTestmanuSetfuncgenoutput)
	ON_BN_CLICKED(IDC_TESTMANU_FUNCGEN, &CTestManuDlg::OnBnClickedTestmanuFuncgen)
	ON_BN_CLICKED(IDC_TESTMANU_SETRMSINPUTLEVEL, &CTestManuDlg::OnBnClickedTestmanuSetrmsinputlevel)
	ON_BN_CLICKED(IDC_TESTMANU_GETRMSINPUTLEVEL, &CTestManuDlg::OnBnClickedTestmanuGetrmsinputlevel)
	ON_BN_CLICKED(IDC_TESTMANU_GETRMSINPUTRANGE, &CTestManuDlg::OnBnClickedTestmanuGetrmsinputrange)
	ON_BN_CLICKED(IDC_TESTMANU_WRITEIO, &CTestManuDlg::OnBnClickedTestmanuWriteio)
	ON_BN_CLICKED(IDC_TESTMANU_READIO, &CTestManuDlg::OnBnClickedTestmanuReadio)
	ON_BN_CLICKED(IDC_TESTMANU_READAMPTEMP, &CTestManuDlg::OnBnClickedTestmanuReadamptemp)
	ON_BN_CLICKED(IDC_TESTMANU_GETHARDWAREINFO, &CTestManuDlg::OnBnClickedTestmanuGethardwareinfo)
	ON_CBN_SELCHANGE(IDC_TESTMANU_WRITEIO_SELECT, &CTestManuDlg::OnCbnSelchangeTestmanuWriteioSelect)
	ON_CBN_SELCHANGE(IDC_TESTMANU_READIO_SELECT, &CTestManuDlg::OnCbnSelchangeTestmanuReadioSelect)
	ON_BN_CLICKED(IDC_TESTMANU_GETSERIALNUMBER, &CTestManuDlg::OnBnClickedTestmanuGetserialnumber)
	ON_BN_CLICKED(IDC_TESTMANU_SETSERIALNUMBER, &CTestManuDlg::OnBnClickedTestmanuSetserialnumber)
	ON_BN_CLICKED(IDC_TESTMANU_RESTOREDEFAULTS, &CTestManuDlg::OnBnClickedTestmanuRestoredefaults)
	ON_BN_CLICKED(IDC_TESTMANU_VALIDATESPIFLASH, &CTestManuDlg::OnBnClickedTestmanuValidatespiflash)
	ON_BN_CLICKED(IDC_TESTMANU_GETIMONVALUE, &CTestManuDlg::OnBnClickedTestmanuGetimonvalue)
	ON_BN_CLICKED(IDC_TESTMANU_GETMACADDR, &CTestManuDlg::OnBnClickedTestmanuGetmacaddr)
	ON_BN_CLICKED(IDC_TESTMANU_SETBRANDMODEL, &CTestManuDlg::OnBnClickedTestmanuSetbrandmodel)
	ON_BN_CLICKED(IDC_TESTMANU_SETMACADDR, &CTestManuDlg::OnBnClickedTestmanuSetmacaddr)
	ON_BN_CLICKED(IDC_TESTMANU_REBOOT2, &CTestManuDlg::OnBnClickedTestmanuReboot2)
END_MESSAGE_MAP()


// CTestManuDlg message handlers
// ParameterAdjust message handlers

#define OLY_TESTMANU_INPUTSELECT_ANALOG 0
#define OLY_TESTMANU_INPUTSELECT_AESL	1
#define OLY_TESTMANU_INPUTSELECT_AESR	2
#define OLY_TESTMANU_INPUTSELECT_DANTE	3
#define OLY_TESTMANU_INPUTSELECT_MIC	4
#define OLY_TESTMANU_INPUTSELECT_TONE	5


typedef struct {
	uint32   uiBrand;
	uint32 uiModel;
} tBrandModel;

BOOL CTestManuDlg::OnInitDialog()
{
	int i;
	int nSel;
	CComboBox* pCombo;

  

	char*	pcInputSelect[] = {
		"Analog",
		"Dante",
		"Pink",
		"Sine",
	};
	int	nInputSelect[] = {
	   TEST_INPUT_ANALOG,
       TEST_INPUT_DANTE,
       TEST_INPUT_PINK,
       TEST_INPUT_SINE,
	};

	BOOL bStatus = CDialog::OnInitDialog();

	pCombo = (CComboBox*) GetDlgItem(IDC_TESTMANU_RMSINPUT_SELECT);
	pCombo->ResetContent();
	for(i=0;i<sizeof(nInputSelect)/sizeof(int);i++)
	{
		nSel = pCombo->AddString(pcInputSelect[i]);
		pCombo->SetItemData(nSel,nInputSelect[i]);
	}

	char pcTestPinInput[OLYTESTPIN_I_FENCE][50] = {
	//   "AMP1_TEMP_VAC_MON", //                     // query raw ADC value (8-byte return message?)
 //      "AMP1_CH1_VOUT_MON", //
 //      "AMP1_CH2_VOUT_MON", //
 //      "AMP1_CH1_CLIP", //                                // query GPIO input logic
 //      "AMP1_CH2_CLIP", //
 //      "AMP1_PROTECT", //                                        // query GPIO output logic
 //      "AMP2_TEMP_VAC_MON", //                     // query raw ADC value (8-byte return message?)
 //      "AMP2_CH1_VOUT_MON", //
 //      "AMP2_CH2_VOUT_MON", //
 //      "AMP2_CH1_CLIP", //                                // query GPIO input logic
 //      "AMP2_CH2_CLIP", //
 //      "AMP2_PROTECT", //                                        // query GPIO output logic
 //      "AUX_SPI_MISO", //
 //      "IR_A_RX", //
 //      "IR_B_RX", //
//};
	"AMP1_CH1_CLIP",
	"AMP1_CH2_CLIP",
	"AMP1_PROTECT",
	"AMP2_CH1_CLIP",
	"AMP2_CH2_CLIP",
	"AMP2_PROTECT",
	"IR_A_RX",
	"IR_B_RX",
	"DISPLAY_D0",
	"DISPLAY_D1",
	};



	char pcTestPinOutput[OLYTESTPIN_O_FENCE][50] = {
 //      "AMP1_DISABLE_MUTE", //                     // set GPIO output logic
 //      "AMP1_BTL4R", //
 //      "AMP1_TEMP_VAC_SEL_CONTROL", //
 //      "AMP1_STANDBY", //
	//   "AMP2_DISABLE_MUTE", //                           // set GPIO output logic
 //      "AMP2_BTL4R", //
 //      "AMP2_TEMP_VAC_SEL_CONTROL", //
 //      "AMP2_STANDBY", //
 //      "AUX_SPI_CLK", //                                         // query raw ADC value (0-1023)
 //      "AUX_SPI_MOSI", //
 //      "AUX_SPI_SS", //
 //      "IR_A_TX", //                                             // send echo from IRDA A and receive on IRDA B
 //      "IR_B_TX", //
 //      "DISPLAY_D0", //                                   // set GPIO output logic
 //      "DISPLAY_D1", //
 //      "DISPLAY_D2", //
 //      "DISPLAY_D3", //
 //      "DISPLAY_D4", //
 //      "DISPLAY_D5", //
 //      "DISPLAY_D6", //
 //      "FAN_CONTROL", //
 //      "LOGOLYTESTPIN_O_LED_AMBER", //                               // set GPIO output logic
 //      "LOGOLYTESTPIN_O_LED_GREEN", //
 //      "LOGOLYTESTPIN_O_LED_RED", //
	//};
		"AMP1_DISABLE_MUTE",
		"AMP1_BTL4R",
		"AMP1_TEMP_VAC_SEL_CONTROL",
		"AMP1_STANDBY",
		"AMP2_DISABLE_MUTE",
		"AMP2_BTL4R",
		"AMP2_TEMP_VAC_SEL_CONTROL",
		"AMP2_STANDBY",
		"IR_A_TX",
		"IR_B_TX",
		"DISPLAY_D2",
		"DISPLAY_D3",
		"DISPLAY_D4",
		"DISPLAY_D5",
		"DISPLAY_D6",
		"FAN_CONTROL",
		"LOGO_LED_AMBER",
		"LOGO_LED_GREEN",
		"LOGO_LED_RED",
	};


	tBrandModel tBrandModelList[] = {

		//	brand				model,				
		{ LOUD_BRAND_MACKIE, MACKIE_MODEL_R8_AA,	},
		{ LOUD_BRAND_MACKIE, MACKIE_MODEL_R8_64,	},
		{ LOUD_BRAND_MACKIE, MACKIE_MODEL_R8_96,	},	
		{ LOUD_BRAND_MACKIE, MACKIE_MODEL_R12_64,	},
		{ LOUD_BRAND_MACKIE, MACKIE_MODEL_R12_96,	},
		{ LOUD_BRAND_MACKIE, MACKIE_MODEL_R12_SM,	},
		{ LOUD_BRAND_MACKIE, MACKIE_MODEL_R12_SW,	},
		{ LOUD_BRAND_MACKIE, MACKIE_MODEL_R18_SW,	},	


		{ LOUD_BRAND_EAW, EAW_MODEL_RSX208L,		},
		{ LOUD_BRAND_EAW, EAW_MODEL_RSX86,			},
		{ LOUD_BRAND_EAW, EAW_MODEL_RSX89,			},	
		{ LOUD_BRAND_EAW, EAW_MODEL_RSX126,			},
		{ LOUD_BRAND_EAW, EAW_MODEL_RSX129,			},
		{ LOUD_BRAND_EAW, EAW_MODEL_RSX12M,			},
		{ LOUD_BRAND_EAW, EAW_MODEL_RSX12,			},
		{ LOUD_BRAND_EAW, EAW_MODEL_RSX18,			},	
		{ LOUD_BRAND_EAW, EAW_MODEL_RSX218,			},
		{ LOUD_BRAND_EAW, EAW_MODEL_RSX18F, 		},
		{ LOUD_BRAND_EAW, EAW_MODEL_RSX212L, 		},

		{ LOUD_BRAND_MARTIN, MARTIN_MODEL_GENERIC,	},
		{ LOUD_BRAND_MARTIN, MARTIN_MODEL_CDDL8,	},
		{ LOUD_BRAND_MARTIN, MARTIN_MODEL_CDDL12,	},
		{ LOUD_BRAND_MARTIN, MARTIN_MODEL_CDDL15,	},	
		{ LOUD_BRAND_MARTIN, MARTIN_MODEL_CSXL118,	},
		{ LOUD_BRAND_MARTIN, MARTIN_MODEL_CSXL218,	},
	};
	char pcBrand[100];
	char pcModel[100];
	char pcBuffer[300];


	pCombo = (CComboBox*) GetDlgItem(IDC_TESTMANU_READIO_SELECT);
	if (pCombo)
	{
		pCombo->ResetContent();
		for(i=0;i<OLYTESTPIN_I_FENCE;i++)
		{
			pCombo->AddString(pcTestPinInput[i]);
		}

	}

	pCombo = (CComboBox*) GetDlgItem(IDC_TESTMANU_WRITEIO_SELECT);
	if (pCombo)
	{
		pCombo->ResetContent();
		for(i=0;i<OLYTESTPIN_O_FENCE;i++)
		{
			pCombo->AddString(pcTestPinOutput[i]);
		}

	}

	pCombo = (CComboBox*) GetDlgItem(IDC_TESTMANU_SETBRANDMODEL_SELECT);
	if (pCombo)
	{
		pCombo->ResetContent();
		for(i=0;i<sizeof(tBrandModelList)/sizeof(tBrandModel);i++)
		{

			m_dlgParent->ConvertBrandModel2Text(pcBrand, pcModel, tBrandModelList[i].uiBrand, tBrandModelList[i].uiModel);
			sprintf(pcBuffer,"%s-%s",pcBrand,pcModel);
			nSel = pCombo->AddString(pcBuffer);
			pCombo->SetItemData(nSel,((tBrandModelList[i].uiBrand&0x0ffff) << 16) + (tBrandModelList[i].uiModel&0x0ffff));
		}

	}
	return bStatus;

}
void CTestManuDlg::UpdateSetBrandModelSelect(uint32 uiBrand, uint32 uiModel)
{
	int nSel;
	uint32 uiValue = ((uiBrand & 0x0ffff) << 16) + (uiModel & 0x0ffff);
	CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_TESTMANU_SETBRANDMODEL_SELECT);

	char pcBrand[100];
	char pcModel[100];
	char pcBuffer[300];

	m_dlgParent->ConvertBrandModel2Text(pcBrand, pcModel, uiBrand,uiModel);
	sprintf(pcBuffer,"%s-%s",pcBrand,pcModel);
	
	if (pCombo)
	{
		pCombo->SelectString(-1,pcBuffer);
	}
}



intfloat CTestManuDlg::GetDlgItemValue(int nIDC, int& nType)
{
	CString theString;
	//char tempBuff[40];
	intfloat j;
	j.i = 0;

	CString theString2;
	
	if (m_dlgParent)
	{
		GetDlgItemText(nIDC,theString);
		j = m_dlgParent->ConvertString2Value(theString,nType);
	}
	return j;

}


void CTestManuDlg::SetProductType(int nProductType)
{
	//int j = 0;
	//CButton* pButton;

	m_nProductType = nProductType;
}


void CTestManuDlg::SetBrandModel(uint32 uiBrand, uint32 uiModel,char* pcBrand, char* pcModel)
{
	char pcBuffer[300];
	m_uiBrand = uiBrand;
	m_uiModel = uiModel;

	sprintf(pcBuffer,"%s-%s",pcBrand,pcModel);
	SetDlgItemText(IDC_TESTMANU_BRANDMODEL,pcBuffer);

	sprintf(pcBuffer,"%d",uiBrand);
	SetDlgItemText(IDC_TESTMANU_BRAND,pcBuffer);
	sprintf(pcBuffer,"%d",uiModel);
	SetDlgItemText(IDC_TESTMANU_MODEL,pcBuffer);

	UpdateSetBrandModelSelect(uiBrand, uiModel);

}

void CTestManuDlg::ProcessMessageTest(mandolin_message* pMsg, int theLength)
{
    uint32 *in_ptr = (uint32 *) pMsg->payload;
	int nMsgLength;
	int nMsgId;
	char pcBuffer[300];
	int nPos;
	uint32 uiValue;
	intfloat jValue;
	int i;
	int nTestID;
	intfloat j;
	int nState;
	char pcError[100];
	uint8 macaddr[6];



	if (!in_ptr) return;
	
	nMsgLength = pMsg->length;
	nMsgId = pMsg->id;
   	if (nMsgId == MANDOLIN_MSG_NESTED)
	{
		nMsgId = ((in_ptr[0] >> 16 ) & 0x0ffff);
		in_ptr += 1;
		nMsgLength--;
	}

	nPos = 0;
	nTestID = in_ptr[nPos];
	nPos++;


	if(IS_MANDOLIN_NACK(pMsg->transport))
	{
	}
	else
	{
		switch (pMsg->id)
		{
		   case MANDOLIN_MSG_TEST:
			if (pMsg->transport & MANDOLIN_TRANSPORT_REPLY )
			{
				switch(nTestID)
				{
				case	TEST_CMD_GET_SERIAL_NUMBER:
					sprintf(pcBuffer,"Serial Num = 0x%8.8X", in_ptr[nPos]);
					SetDlgItemText(IDC_TESTMANU_GETSERIALNUMBER_VALUE,pcBuffer);
					//strcat(pDest,pcBuffer);
					break;

				case TEST_CMD_GET_MAC_ADDR:
					macaddr[0] =	((in_ptr[nPos] >>  8) & 0x0ff);
					macaddr[1] = 	((in_ptr[nPos] >>  0) & 0x0ff);
					macaddr[2] = 	((in_ptr[nPos+1] >> 24) & 0x0ff);
					macaddr[3] = 	((in_ptr[nPos+1] >> 16) & 0x0ff);
					macaddr[4] = 	((in_ptr[nPos+1] >>  8) & 0x0ff);
					macaddr[5] = 	((in_ptr[nPos+1] >>  0) & 0x0ff);



					sprintf(pcBuffer,"MAC Addr= %2.2X-%2.2X-%2.2X-%2.2X-%2.2X-%2.2X", 
							macaddr[0],
							macaddr[1],
							macaddr[2],
							macaddr[3],
							macaddr[4],
							macaddr[5]);
					//strcat(pDest,pcBuffer);
					SetDlgItemText(IDC_TESTMANU_GETMACADDR_VALUE,pcBuffer);

					sprintf(pcBuffer,"%2.2X", macaddr[4]);
					SetDlgItemText(IDC_TESTMANU_SETMACADDR_VALUE,pcBuffer);

					sprintf(pcBuffer,"%2.2X", macaddr[5]);
					SetDlgItemText(IDC_TESTMANU_SETMACADDR_VALUE2,pcBuffer);

					
					break;
				}

			}
			else
			{
			}
			break;

		}
	}
}
void CTestManuDlg::OnBnClickedTestmanuSetfuncgenoutput()
{
	// TODO: Add your control notification handler code here
		// TODO: Add your control notification handler code here
	char pcBuffer[200];
	intfloat nChannel;
	int nValueType;

	if (!m_dlgParent) return;

	nChannel = GetDlgItemValue(IDC_TESTMANU_SETFUNCGENOUTPUT_CH, nValueType);
	if (nValueType ==1)
	{
		nChannel.i = nChannel.f;
		sprintf(pcBuffer,"%d",nChannel.i);
		SetDlgItemText(IDC_TESTMANU_SETFUNCGENOUTPUT_CH,pcBuffer);
	}
	
	if ((nChannel.i<0) || (nChannel.i>4))
	{
		MessageBox("Channel must be 0..4","Test Manu DSP Error",MB_OK);
		return;
	}

	m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
	m_dlgParent->m_MandolinCommOLYTestManu.CreateTestDSPSetFuncGenOutput(&m_dlgParent->m_msgTx,m_dlgParent->m_nTxSequence,nChannel.i);
	m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);

}

void CTestManuDlg::OnBnClickedTestmanuFuncgen()
{
	// TODO: Add your control notification handler code here
		// TODO: Add your control notification handler code here
	char pcBuffer[200];
	intfloat jAmplitude;
	intfloat jFreq;
	int nValueType;

	jAmplitude = GetDlgItemValue(IDC_TESTMANU_FUNCGEN_AMPLITUDE, nValueType);
	if (nValueType == 0) jAmplitude.f = jAmplitude.i;
	if ((jAmplitude.f<-120) || (jAmplitude.f>10.0))
	{
		MessageBox("Amplitude must be -120..10.0","Test Manu DSP Error",MB_OK);
		return;
	}

	jFreq = GetDlgItemValue(IDC_TESTMANU_FUNCGEN_FREQ, nValueType);
	if (nValueType == 0) jFreq.f = jFreq.i;
	if ((jFreq.f<20) || (jFreq.f>5000.0))
	{
		MessageBox("Freq must be 20..5000.0","Test Manu DSP Error",MB_OK);
		return;
	}

	m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
	m_dlgParent->m_MandolinCommOLYTestManu.CreateTestDSPSetFuncGen(&m_dlgParent->m_msgTx,m_dlgParent->m_nTxSequence,0,jAmplitude.f,jFreq.f);
	m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);
}

void CTestManuDlg::OnBnClickedTestmanuSetrmsinputlevel()
{
	// TODO: Add your control notification handler code here
	
	CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_TESTMANU_RMSINPUT_SELECT);
	if (!pCombo) return;
	int nSel = pCombo->GetCurSel();

	if (nSel<0) return;

	int nValue = pCombo->GetItemData(nSel);


	
	m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
	m_dlgParent->m_MandolinCommOLYTestManu.CreateTestDSPSetRMSMeterInput(&m_dlgParent->m_msgTx,m_dlgParent->m_nTxSequence,nValue);
	m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);

}

void CTestManuDlg::OnBnClickedTestmanuGetrmsinputlevel()
{
	// TODO: Add your control notification handler code here
	m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
	m_dlgParent->m_MandolinCommOLYTestManu.CreateTestDSPGetRMSMeterLevel(&m_dlgParent->m_msgTx,m_dlgParent->m_nTxSequence);
	m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);
}

void CTestManuDlg::OnBnClickedTestmanuGetrmsinputrange()
{
	// TODO: Add your control notification handler code here
	intfloat jMin, jMax;
	int nValueType;

	jMin = GetDlgItemValue(IDC_TESTMANU_GETRMSINPUTRANGE_MIN, nValueType);
	if (nValueType == 0) jMin.f = jMin.i;
	jMax = GetDlgItemValue(IDC_TESTMANU_GETRMSINPUTRANGE_MAX, nValueType);
	if (nValueType == 0) jMax.f = jMax.i;

	m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
	m_dlgParent->m_MandolinCommOLYTestManu.CreateTestDSPGetRMSMeterRange(&m_dlgParent->m_msgTx,m_dlgParent->m_nTxSequence,jMin.f,jMax.f);
	m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);
}

void CTestManuDlg::OnBnClickedTestmanuWriteio()
{
	// TODO: Add your control notification handler code here
	intfloat jAddr, jValue;
	int nValueType;
	char pcBuffer[100];

	jAddr = GetDlgItemValue(IDC_TESTMANU_WRITEIO_INDEX, nValueType);
	if (nValueType == 1) jAddr.i = jAddr.f;

	if ((jAddr.i<0) || (jAddr.i>>OLYTESTPIN_O_FENCE))
	{
		sprintf(pcBuffer,"Addr %d..%d",0,OLYTESTPIN_O_FENCE);
		MessageBox(pcBuffer,"Test Manu DSP Error",MB_OK);
		return;
	}

	jValue = GetDlgItemValue(IDC_TESTMANU_WRITEIO_VALUE, nValueType);
	if (nValueType == 1) jValue.i = jValue.f;
	if ((jValue.i<0) || (jValue.i>1))
	{
		MessageBox("Value 0..1","Test Manu DSP Error",MB_OK);
		return;
	}
	
	// ----------------------
	CComboBox* pCombo;
	int nSel;

	pCombo = (CComboBox*) GetDlgItem(IDC_TESTMANU_WRITEIO_SELECT);
	if (pCombo)
	{
		nSel = pCombo->SetCurSel(jAddr.i);
	}
	// ----------------------

	m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
	m_dlgParent->m_MandolinCommOLYTestManu.CreateTestDSPWriteIO(&m_dlgParent->m_msgTx,m_dlgParent->m_nTxSequence,jAddr.i,jValue.i);
	m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);
}

void CTestManuDlg::OnBnClickedTestmanuReadio()
{
	// TODO: Add your control notification handler code here
		intfloat jAddr;
	int nValueType;
	char pcBuffer[100];

	jAddr = GetDlgItemValue(IDC_TESTMANU_READIO_INDEX, nValueType);
	if (nValueType == 1) jAddr.i = jAddr.f;

	if ((jAddr.i<0) || (jAddr.i>>OLYTESTPIN_I_FENCE))
	{
		sprintf(pcBuffer,"Addr %d..%d",0,OLYTESTPIN_I_FENCE);
		MessageBox(pcBuffer,"Test Manu DSP Error",MB_OK);
		return;
	}
	
	// ----------------------
	CComboBox* pCombo;
	int nSel;

	pCombo = (CComboBox*) GetDlgItem(IDC_TESTMANU_READIO_SELECT);
	if (pCombo)
	{
		nSel = pCombo->SetCurSel(jAddr.i);
	}
	// ----------------------



	m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
	m_dlgParent->m_MandolinCommOLYTestManu.CreateTestDSPReadIO(&m_dlgParent->m_msgTx,m_dlgParent->m_nTxSequence,jAddr.i);
	m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);

}

void CTestManuDlg::OnBnClickedTestmanuReadamptemp()
{
	// TODO: Add your control notification handler code here
	char pcBuffer[200];
	intfloat nChannel;
	int nValueType;

	if (!m_dlgParent) return;


	nChannel = GetDlgItemValue(IDC_TESTMANU_READAMPTEMP_CH, nValueType);
	if (nValueType ==1)
	{
		nChannel.i = nChannel.f;
		sprintf(pcBuffer,"%d",nChannel.i);
		SetDlgItemText(IDC_TESTMANU_READAMPTEMP_CH,pcBuffer);
	}
	
	if ((nChannel.i<1) || (nChannel.i>2))
	{
		MessageBox("Amp must be 1..2","Test Manu DSP Error",MB_OK);
		return;
	}

	m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
	m_dlgParent->m_MandolinCommOLYTestManu.CreateTestDSPReadAmpTemperature(&m_dlgParent->m_msgTx,m_dlgParent->m_nTxSequence,nChannel.i);
	m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);

}

void CTestManuDlg::OnBnClickedTestmanuGethardwareinfo()
{
	// TODO: Add your control notification handler code here
	m_dlgParent->OnBnClickedGetHwInfo();
}

void CTestManuDlg::OnCbnSelchangeTestmanuWriteioSelect()
{
	// TODO: Add your control notification handler code here
	CComboBox* pCombo;
	int nSel;
	char pcBuffer[10];

	pCombo = (CComboBox*) GetDlgItem(IDC_TESTMANU_WRITEIO_SELECT);
	if (pCombo)
	{
		nSel = pCombo->GetCurSel();
		sprintf(pcBuffer,"%d",nSel);
		SetDlgItemText(IDC_TESTMANU_WRITEIO_INDEX,pcBuffer);
	}


}

void CTestManuDlg::OnCbnSelchangeTestmanuReadioSelect()
{
	// TODO: Add your control notification handler code here
	CComboBox* pCombo;
	int nSel;
	char pcBuffer[10];

	pCombo = (CComboBox*) GetDlgItem(IDC_TESTMANU_READIO_SELECT);
	if (pCombo)
	{
		nSel = pCombo->GetCurSel();
		sprintf(pcBuffer,"%d",nSel);
		SetDlgItemText(IDC_TESTMANU_READIO_INDEX,pcBuffer);
	}


}

void CTestManuDlg::OnBnClickedTestmanuGetserialnumber()
{
	// TODO: Add your control notification handler code here
		
	m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
	m_dlgParent->m_MandolinCommOLYTestManu.CreateTestDSPGetSerialNum(&m_dlgParent->m_msgTx,m_dlgParent->m_nTxSequence);
	m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);

}

void CTestManuDlg::OnBnClickedTestmanuSetserialnumber()
{
	// TODO: Add your control notification handler code here
	char pcBuffer[200];
	intfloat nSerial;
	int nValueType;

	if (!m_dlgParent) return;

	nSerial = GetDlgItemValue(IDC_TESTMANU_SETSERIALNUMBER_NUMBER, nValueType);
	if (nValueType ==1)
	{
		nSerial.ui = nSerial.f;
		sprintf(pcBuffer,"0x%8.8X",nSerial.ui);
		SetDlgItemText(IDC_TESTMANU_SETSERIALNUMBER_NUMBER,pcBuffer);
	}
	
	m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
	m_dlgParent->m_MandolinCommOLYTestManu.CreateTestDSPSetSerialNum(&m_dlgParent->m_msgTx,m_dlgParent->m_nTxSequence,nSerial.ui);
	m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);

	MessageBox("Requires a Power Cycle","Test Manu");

}

void CTestManuDlg::OnBnClickedTestmanuRestoredefaults()
{
	// TODO: Add your control notification handler code here
	m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
	m_dlgParent->m_MandolinCommOLYTestManu.CreateTestDSPRestoreDefaults(&m_dlgParent->m_msgTx,m_dlgParent->m_nTxSequence);
	m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);

}

void CTestManuDlg::OnBnClickedTestmanuGetmacaddr()
{
	// TODO: Add your control notification handler code here
	
	m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
	m_dlgParent->m_MandolinCommOLYTestManu.CreateTestGetMACAddr(&m_dlgParent->m_msgTx,m_dlgParent->m_nTxSequence);
	m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);

}


void CTestManuDlg::OnBnClickedTestmanuValidatespiflash()
{
	// TODO: Add your control notification handler code here
			
	m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
	m_dlgParent->m_MandolinCommOLYTestManu.CreateTestValidateSPIFlash(&m_dlgParent->m_msgTx,m_dlgParent->m_nTxSequence);
	m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);

}

void CTestManuDlg::OnBnClickedTestmanuGetimonvalue()
{
	char pcBuffer[200];
	intfloat nChannel;
	int nValueType;

	// TODO: Add your control notification handler code here
	nChannel = GetDlgItemValue(IDC_TESTMANU_GETIMONVALUE_CHANNEL, nValueType);
	if (nValueType ==1)
	{
		nChannel.i = nChannel.f;
		sprintf(pcBuffer,"%d",nChannel.i);
		SetDlgItemText(IDC_TESTMANU_READAMPTEMP_CH,pcBuffer);
	}
	if ((nChannel.i < 0) || (nChannel.i > 2))
	{
		MessageBox("Channel must be 0,1, or 2","Test Manu Error",MB_OK);
		return;
	}


	m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
	m_dlgParent->m_MandolinCommOLYTestManu.CreateTestGetIMonValue(&m_dlgParent->m_msgTx,m_dlgParent->m_nTxSequence,nChannel.i);
	m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);

}


void CTestManuDlg::OnBnClickedTestmanuSetbrandmodel()
{
	// TODO: Add your control notification handler code here

	CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_TESTMANU_SETBRANDMODEL_SELECT);
	uint32 uiValue;
	uint32 uiBrand = 0;
	uint32 uiModel = 0;
	int nSel;

	if(pCombo)
	{
		nSel = pCombo->GetCurSel();
		uiValue = pCombo->GetItemData(nSel);

		uiBrand = (uiValue >> 16 ) & 0x0ffff;
		uiModel = uiValue & 0x0ffff;


		m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
		m_dlgParent->m_MandolinCommOLYTestManu.CreateTestSetBrandModel(&m_dlgParent->m_msgTx,m_dlgParent->m_nTxSequence,uiBrand,uiModel);
		m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);
	}

	MessageBox("Requires a Power Cycle","Test Manu");

	//OnBnClickedTestmanuGethardwareinfo();

}

void CTestManuDlg::OnBnClickedTestmanuSetmacaddr()
{
	// TODO: Add your control notification handler code here
	uint8 macaddr[6] = {0x00, 0x0f, 0xf2, 0x08, 0xff, 0xff};
	uint32 uiSerialNumber = 0xffffffff;
	uint32 uiBrand = 0xffffffff;
	uint32 uiModel = 0xffffffff;
	uint32 uiHardwareRev = 0x0ffff;
	uint32 uiValue;
	
	char *next;
	intfloat nSerial;
	int nValueType;

	if (!m_dlgParent) return;

	char pcBuffer[100];

	GetDlgItemText(IDC_TESTMANU_SETMACADDR_VALUE,pcBuffer,sizeof(pcBuffer)-1);
	uiValue = strtoul(pcBuffer, &next,16);
	macaddr[4]= uiValue & 0x0ff;

	GetDlgItemText(IDC_TESTMANU_SETMACADDR_VALUE2,pcBuffer,sizeof(pcBuffer)-1);
	uiValue = strtoul(pcBuffer, &next,16);
	macaddr[5]= uiValue & 0x0ff;

	//nSerial = GetDlgItemValue(IDC_TESTMANU_SETMACADDR_VALUE, nValueType);
	//if (nValueType ==1)
	//{
	//	nSerial.ui = nSerial.f;
	//	sprintf(pcBuffer,"0x%8.8X",nSerial.ui);
	//	SetDlgItemText(IDC_TESTMANU_SETSERIALNUMBER_NUMBER,pcBuffer);
	//}



	m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
	m_dlgParent->m_MandolinCommOLYTestManu.CreateTestSetIdentity(&m_dlgParent->m_msgTx,m_dlgParent->m_nTxSequence,macaddr,uiBrand,uiModel,uiHardwareRev,uiSerialNumber);
	m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);

	MessageBox("Requires a Power Cycle","Test Manu");

}

void CTestManuDlg::OnBnClickedTestmanuReboot2()
{
	uint32 uiRebootCode = OLY_APPID_MAIN;

	// TODO: Add your control notification handler code here
	m_dlgParent->m_msgTx.payload = m_dlgParent->m_msgTxPayload;
	m_dlgParent->m_MandolinComm.CreateReboot(&m_dlgParent->m_msgTx,m_dlgParent->m_nTxSequence,uiRebootCode);
	m_dlgParent->TryToSendMandolinMessage(&m_dlgParent->m_msgTx, true);

}
