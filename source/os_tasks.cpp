/*	This file is not shared with Secondary Boot loader build!!!!!
 	 Any changes made to the secondary boot loader version that are needed here must be manually copied.*/
/*
 * os_tasks.cpp
 *
 *  Created on: Sep 17, 2015
 *      Author: Kris.Simonsen
 */


#include "ConfigManager.h"
//#include "IRDA_Task.h"

#include "os_tasks.h"
#include "network.h"

extern "C" {
//#include "gpio_pins.h"
//#include "uart_voice.h"
//#include "uart_ultimo.h"
//#include "uart_irda.h"
#include "inputhandler.h"
#include "oly_logo.h"
#include <stdio.h>
#include "UltimoPort.h"
//#include "Discovery.h"
//#include <timer.h>
//#include "TimeKeeper.h"
//#include "SpeakerConfiguration.h"
//#include "math.h"
//#include "CurrentSense.h"
//#include "AUX_Spi_task.h"
//#include "pascal_spro2.h"


#if USE_CDD_UI
//#include "cddl_leds.h"
#endif
//#include "AmpMonitor.h"
//#include "Region.h"
//#include "MMA8653FC.h"
//#include "IRDAManager.h"
//#include "sys.h"
}

#include "MQX_To_FreeRTOS.h"
#define FSL_UART_ULTIMO 1


using namespace oly;

Config *	   		olyConfig;

UltimoPort *		g_pUltimoPort = 0;
//Discovery *			g_pDiscovery = 0;

bool logo_enabled = false;
bool network_initialized = false;	//Used to prevent network-based tasks from running until network is initialized
bool configtask_initialized = false;
const char *g_pszModelName = "";
extern uint32_t g_CurrentIpAddress;
bool g_bMeterComplete = true;
int orientation_counter = 0;
extern bool g_ScreenInTransition;

// -----------------------------------------------------------------------------------------------


#ifdef USE_MEMPOOL_MONITOR
typedef struct {
                int nId;
                int nStatus;  // 0 = free, 1 in used
                //MQX_TICK_STRUCT startTicks;
                mandolin_mqx_message_ptr pData;
} tMemPoolMonitor;

tMemPoolMonitor gMemPoolMonitor[MAX_MSG_COUNT];

bool MemPoolMonitor_Init()
{
	int i;
	for(i=0;i<MAX_MSG_COUNT;i++)
	{
		gMemPoolMonitor[i].nId = i;
		gMemPoolMonitor[i].nStatus = 0;
		gMemPoolMonitor[i].pData = 0;
	}
}
bool MemPoolMonitor_Alloc(mandolin_mqx_message_ptr pMem)
{
	// find the entry that matches
	int i;
	bool bFound = false;
	int nFirstBlank = -1;
	if (!pMem)
		return false;
	for(i=0;i<MAX_MSG_COUNT;i++)
	{
		if (gMemPoolMonitor[i].pData == pMem)
		{
			if (gMemPoolMonitor[i].nStatus == 0)
			{
				gMemPoolMonitor[i].nStatus = 1;			// mark as used
				return true;

			}
			else
			{
				// error... tried to alloc a memory pool that is allocated already
				return false;
			}
		}
		if ((nFirstBlank < 0) && (gMemPoolMonitor[i].pData == 0))
		{
			nFirstBlank = i;
		}
	}
	if (nFirstBlank >= 0)
	{
		// match a monitor slot with this memory
		gMemPoolMonitor[nFirstBlank].pData = pMem;
		gMemPoolMonitor[i].nStatus = 1;
		return true;

	}
	return false;
}

bool MemPoolMonitor_Free(mandolin_mqx_message_ptr pMem)
{
	int i;
	for(i=0;i<MAX_MSG_COUNT;i++)
	{
		if (gMemPoolMonitor[i].pData == pMem)
		{
			gMemPoolMonitor[i].nStatus = 0;
			return true;
		}
	}
	return false;
}

#endif
// -----------------------------------------------------------------------------------------------
#ifdef USE_MSGQ_MUTEX

MUTEX_STRUCT msgq_mutex;
MUTEX_ATTR_STRUCT msgq_mutex_attr;
int gMsgQAllocCount = 0;
int gMsgQAllocErrorCount = 0;
int gMsgQSendErrorCount = 0;

int	MsgQ_Init()
{

	 /* Initialize mutex attributes: */
	 //if (_mutatr_init(&mutexattr) != MQX_OK) {
	 //printf("Initializing mutex attributes failed.\n");
	 //_mqx_exit(0);
	 //}
//
	// /* Initialize the mutex: */
	 //if (_mutex_init(&print_mutex, &mutexattr) != MQX_OK) {
	 //printf("Initializing print mutex failed.\n");
	 //_mqx_exit(0);
	 //}


	_mutatr_init(&msgq_mutex_attr);
	//_mutatr_set_sched_protocol(&msgq_mutex_attr, (MUTEX_PRIO_INHERIT | MUTEX_PRIO_PROTECT));
	//_mutatr_set_wait_protocol(&msgq_mutex_attr, MUTEX_QUEUEING);
	_mutex_init(&msgq_mutex, &msgq_mutex_attr);


#ifdef USE_MEMPOOL_MONITOR
	MemPoolMonitor_Init();
#endif
}

int	MsgQ_Get_Count(_queue_id config_qid)
{
	int nCount;
	_mutex_lock(&msgq_mutex);
	nCount = _msgq_get_count(config_qid);
	_mutex_unlock(&msgq_mutex);
	return  nCount;
}
void* Msg_Alloc(_pool_id message_pool)
{
	void* pMsg;

	_mutex_lock(&msgq_mutex);
	if (gMsgQAllocCount >= (MAX_MSG_COUNT-1))
	{
		// for some reason if msgalloc every is full then the task stop working?
		_mutex_unlock(&msgq_mutex);
		return NULL;
	}
	pMsg  = _msg_alloc(message_pool);
	if (pMsg)
	{
		gMsgQAllocCount++;
#ifdef USE_MEMPOOL_MONITOR
		MemPoolMonitor_Alloc((mandolin_mqx_message_ptr)pMsg);
#endif
	}
	else
	{
		gMsgQAllocErrorCount++;
	}
	_mutex_unlock(&msgq_mutex);
	return pMsg;

}
void Msg_Free(void* pMsg)
{
	_mutex_lock(&msgq_mutex);

	 if (pMsg)
	 {
		 gMsgQAllocCount--;
#ifdef USE_MEMPOOL_MONITOR
		MemPoolMonitor_Free((mandolin_mqx_message_ptr)pMsg);
#endif

	 }
	_msg_free(pMsg);
	_mutex_unlock(&msgq_mutex);
	return;

}

 bool MsgQ_Send(void* pMsg)
 {
	 bool bSuccess;
	 _mutex_lock(&msgq_mutex);
	 bSuccess  = _msgq_send(pMsg);
	 if (!bSuccess)
	 {
		 gMsgQSendErrorCount++;
	 }

	_mutex_unlock(&msgq_mutex);
	return bSuccess;
 }

#endif

// -----------------------------------------------------------------------------------------------
#ifdef USE_TASK_DEBUG
tTaskDebug gTaskDebug[TASK_FENCE];

void TaskDebug_Init()
{
	int i;
	for(i=0;i<TASK_FENCE;i++)
	{
		//gTaskDebug[i].startTicks = 0;
		//gTaskDebug[i].endTicks = 0;
		gTaskDebug[i].nId = i;
		_time_get_elapsed_ticks(&gTaskDebug[i].startTicks);
		_time_get_elapsed_ticks(&gTaskDebug[i].endTicks);
		gTaskDebug[i].nCount = 0;
		gTaskDebug[i].nDiffTicks = 0;
		gTaskDebug[i].bInTask = false;
		gTaskDebug[i].nTaskPos = 0;
	}

}

void TaskDebug_TimeIn(int nId)
{
	_time_get_elapsed_ticks(&gTaskDebug[nId].startTicks);
	gTaskDebug[nId].nCount++;
	gTaskDebug[nId].bInTask = true;
	gTaskDebug[nId].nTaskPos = 0;

}
void TaskDebug_TimeOut(int nId)
{
	bool bWrap;
	_time_get_elapsed_ticks(&gTaskDebug[nId].endTicks);
	gTaskDebug[nId].bInTask = false;
	gTaskDebug[nId].nDiffTicks = _time_diff_milliseconds(&gTaskDebug[nId].endTicks, &gTaskDebug[nId].startTicks, &bWrap);
	gTaskDebug[nId].nTaskPos = 100;
}
void TaskDebug_SetPos(int nId, int nPos)
{
	gTaskDebug[nId].nTaskPos = nPos;
}

#endif

// -----------------------------------------------------------------------------------------------

/* Processes incoming mandolin messages */
void Config_Task(uint32_t button_state)
{
	_queue_id			 	 config_qid;
	mandolin_mqx_message_ptr msg_ptr = NULL;
	MQX_TICK_STRUCT startTicks;
	int nMaxMsg = 0;
	int nMsgs = 0;
	int nLastMsgsA = 0;
	int nLastMsgsB = 0;
	int nCount = 0;
	bool bTimeout;
	static int nMaxMsgCount = 0;

	printf("Config task created\n");
	init_message_pool();				// Had to move message pool creation to BEFORE Config object creation (as this opens message Qs)

	olyConfig = new Config();

//	g_pszModelName = Region::GetMandolinModelName(Region::GetSystemBrand(), Region::GetSystemModel());

	//set brightness to zero and later turn bright. This assumes the stored value is not used for anything.
//	olyConfig->SetLcdBrightness(0);

	/* Load stored DSP parameters */
//	olyConfig->LoadAllFromFlash();			// (Also inits codec)

//	if ((!olyConfig->IsInitialized()) || (!(button_state & event_EncoderSW)) || (!(button_state & event_SW2))) { 	// First time boot, flash not programmed or Encoder button held at boot
//		olyConfig->RestoreDefaults(true);
//	}
#if USE_CDD_UI
//	olyConfig->SetFanEnabled(true);  //enable fan for first 2 seconds on startup
#else
//	olyConfig->SetFanEnabled(false);
#endif

//	IQ - We need to setup olyParams and olyStoredParams to the CDD defaults (specified in voiceing.h). The user settable bits will need to be stored in flash. There are not many params that are user changable. Suggest we only use onboard flash!

//  IQ -  RestoreDefaults has been adjusted to just copy in the defaults. We do not look for saved changes, no interactino with flash at all!
	olyConfig->RestoreDefaults(true);


	MSGQ_INIT()

	config_qid = _msgq_open(CONFIG_QUEUE, 0);

	/* Init Ethernet driver */
	network_init();
	olyConfig->SetIP();
	network_initialized = true;

#if USE_OLY_UI
	olyConfig->SetInputSelectMode(eAUDIO_MODE_AUTO); // Enforce auto mode on oly
#endif

//	olyConfig->InitStatusParams();
//	olyConfig->InitUI();

	configtask_initialized = true;
	 _time_get_elapsed_ticks(&startTicks);


	while (1) {
		nMsgs = MSGQ_GET_COUNT(config_qid);
		if ((nMsgs>nMaxMsg))// || (nMsgs!=nLastMsgsA))
		{
			if (nMsgs>nMaxMsg)
				nMaxMsg = nMsgs;
			nLastMsgsA = nMsgs;
			if (nMaxMsg>=MAX_MSG_COUNT)
			{
//				printf("Message-A Count Peaked at %d, last = %d\n", nMaxMsg, nLastMsgsA);
				nMaxMsg = 0;
			}
		}
		bTimeout = false;

		#if USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
		{
			char tempString[25];
			uint8_t tempVar;
			static uint8_t nMsgs1 = 0xff;
			static uint8_t nMsgs2 = 0xff;
			tempVar = MSGQ_GET_COUNT(config_qid);
			if(nMsgs1 != tempVar)	{
				nMsgs1 = tempVar;
				sprintf(tempString, "Cfg-Q: %i", nMsgs1);
				olyConfig->olyUI.DebugLineOut(0, tempString);
			}
			tempVar = MSGQ_GET_COUNT(_msgq_get_id(0,TCP_QUEUE));
			if(nMsgs2 != tempVar)	{
				nMsgs2 = tempVar;
				sprintf(tempString, "TCP-Q: %i", nMsgs2);
				olyConfig->olyUI.DebugLineOut(1, tempString);
			}
		}
		#endif	// USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG

		msg_ptr = (mandolin_mqx_message_ptr)_msgq_receive(config_qid, CONNECT_MANAGEMENT_PERIOD);	// Block waiting for message
		bTimeout = (msg_ptr == 0);

		if (!msg_ptr)	// if null then here because of time out, so just check to see if anything is ready anyway so loop below works
		{
			msg_ptr = (mandolin_mqx_message_ptr)_msgq_poll(config_qid);  // returns NULL if no message ready
		}

		nCount = 0;
		TASKDEBUG_IN(TASK_CONFIG)
		nMsgs = MSGQ_GET_COUNT(config_qid);
		TASKDEBUG_POS(TASK_CONFIG,12)

		if (nMsgs > nMaxMsgCount)
		{
			nMaxMsgCount = nMsgs;
		}
		nCount = 0; // count to see how many messages are processed from the pool
		while(msg_ptr)	// do more than one message if more than one are ready
		{
			if (msg_ptr->sender_type == COMM_RS232_UART) {
				TASKDEBUG_POS(TASK_CONFIG,11)
#if !USES_FOUR_IRDA
//				olyConfig->MandolinHandle(&olyConfig->olyVoicingPort,msg_ptr);
#endif
			}
			else if (msg_ptr->sender_type == COMM_ETHERNET_TCP) {
				TASKDEBUG_POS(TASK_CONFIG,1)
				olyConfig->MandolinHandle(&olyConfig->olyNetworkPort,msg_ptr);
				#if USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
				{
					char temp[25];
						switch(msg_ptr->message.id)	{
						case MANDOLIN_MSG_PING:	sprintf(temp, "NetRX: Ping");	olyConfig->olyUI.DebugOut(temp);	break;
						case MANDOLIN_MSG_ADVERTIZE_HARDWARE_INFO:	sprintf(temp, "NetRX: Adv HW Info");	olyConfig->olyUI.DebugOut(temp);	break;
						case MANDOLIN_MSG_GET_HARDWARE_INFO:	sprintf(temp, "NetRX: Get HW Info");	olyConfig->olyUI.DebugOut(temp);	break;
						case MANDOLIN_MSG_GET_SOFTWARE_INFO:	sprintf(temp, "NetRX: Get SW Info");	olyConfig->olyUI.DebugOut(temp);	break;
						case MANDOLIN_MSG_REBOOT:	sprintf(temp, "NetRX: Reboot");	olyConfig->olyUI.DebugOut(temp);	break;
						case MANDOLIN_MSG_TRIGGER_SYSTEM_EVENT:	sprintf(temp, "NetRX: Trig Sys Evt");	olyConfig->olyUI.DebugOut(temp);	break;
						case MANDOLIN_MSG_TRIGGER_APPLICATION_EVENT:	sprintf(temp, "NetRX: Trig App Evt");	olyConfig->olyUI.DebugOut(temp);	break;
						case MANDOLIN_MSG_TEST:	sprintf(temp, "NetRX: Test");	olyConfig->olyUI.DebugOut(temp);	break;
						case MANDOLIN_MSG_GET_INFO:	sprintf(temp, "NetRX: Get Info");	olyConfig->olyUI.DebugOut(temp);	break;
						case MANDOLIN_MSG_FILE_OPEN:	sprintf(temp, "NetRX: File Open");	olyConfig->olyUI.DebugOut(temp);	break;
						case MANDOLIN_MSG_FILE_CLOSE:	sprintf(temp, "NetRX: File Close ");	olyConfig->olyUI.DebugOut(temp);	break;
						case MANDOLIN_MSG_GET_FILE:	sprintf(temp, "NetRX: Get File");	olyConfig->olyUI.DebugOut(temp);	break;
						case MANDOLIN_MSG_POST_FILE:	sprintf(temp, "NetRX: Post File");	olyConfig->olyUI.DebugOut(temp);	break;
						case MANDOLIN_MSG_GET_SYSTEM_STATUS:	sprintf(temp, "NetRX: Get Sys Sts");	olyConfig->olyUI.DebugOut(temp);	break;
						case MANDOLIN_MSG_SET_SYSTEM_STATE:	sprintf(temp, "NetRX: Set Sys Ste");	olyConfig->olyUI.DebugOut(temp);	break;
						case MANDOLIN_MSG_RESET_SYSTEM_STATE:	sprintf(temp, "NetRX: Rst Sys Ste");	olyConfig->olyUI.DebugOut(temp);	break;
						case MANDOLIN_MSG_SUBSCRIBE_SYSTEM_STATE:	sprintf(temp, "NetRX: Subs Sys Ste");	olyConfig->olyUI.DebugOut(temp);	break;
						case MANDOLIN_MSG_GET_APPLICATION_PARAMETER:	/*sprintf(temp, "NetRX: Get App Prm");	olyConfig->olyUI.DebugOut(temp);*/	break;
						case MANDOLIN_MSG_SET_APPLICATION_PARAMETER:	/*sprintf(temp, "NetRX: Set App Prm");	olyConfig->olyUI.DebugOut(temp);*/	break;
						case MANDOLIN_MSG_RESET_APPLICATION_PARAMETER:	sprintf(temp, "NetRX: Rst App Prm");	olyConfig->olyUI.DebugOut(temp);	break;
						case MANDOLIN_MSG_SUBSCRIBE_APPLICATION_PARAMETER:	sprintf(temp, "NetRX: Subs App Prm");	olyConfig->olyUI.DebugOut(temp);	break;
						case MANDOLIN_MSG_CREATE_PARAMETER_LIST:	sprintf(temp, "NetRX: Create Prm List");	olyConfig->olyUI.DebugOut(temp);	break;
						case MANDOLIN_MSG_GET_APPLICATION_STRING:	sprintf(temp, "NetRX: Get App Str");	olyConfig->olyUI.DebugOut(temp);	break;
						case MANDOLIN_MSG_SET_APPLICATION_STRING:	sprintf(temp, "NetRX: Set App Str");	olyConfig->olyUI.DebugOut(temp);	break;
						case MANDOLIN_MSG_RESET_APPLICATION_STRING:	sprintf(temp, "NetRX: Rst App Str");	olyConfig->olyUI.DebugOut(temp);	break;
						default:	sprintf(temp, "NetRX: Invalid");	olyConfig->olyUI.DebugOut(temp);	break;
						}
				}
				#endif	// USE_BACKPANEL_DEBUG & USE_DEBUG_MAN_MSG
			}
			else if (msg_ptr->sender_type == COMM_SPI ) {
				TASKDEBUG_POS(TASK_CONFIG,2)
//				olyConfig->MandolinHandle(&olyConfig->olyDspPort,msg_ptr);
			}
			TASKDEBUG_POS(TASK_CONFIG,3)

			MSG_FREE(config_qid, msg_ptr);

			msg_ptr = (mandolin_mqx_message_ptr)_msgq_poll(config_qid);  // returns NULL if no message ready
			nCount++;
		}
		if (bTimeout)
		{
#if(0)
			if(meterStartWait)
				meterStartWait--;
			else
			{
				TASKDEBUG_POS(TASK_CONFIG,13)

				TASKDEBUG_IN(TASK_METER)

				//	Add mechanism to not request meters from DSP until last meters received or 1 second of non-responsiveness
				if (g_bMeterComplete || (!s_meter_skip))
				{
					s_meter_skip = 1000/DSP_METER_RATE_MS;
					g_bMeterComplete = false;
					TASKDEBUG_POS(TASK_CONFIG,14)
					olyConfig->GetCurrentMeter();
				}
				else
					s_meter_skip--;

				TASKDEBUG_OUT(TASK_METER)
			}
#endif
		}

		if ((nMsgs>nMaxMsg) || (nMsgs!=nLastMsgsB))
		{
			if (nMsgs>nMaxMsg)
				nMaxMsg = nMsgs;
			nLastMsgsB = nMsgs;
			if (nMaxMsg>=MAX_MSG_COUNT)
			{
//				printf("Message-B Count Peaked at %d, last = %d\n", nMaxMsg, nLastMsgsB);
				nMaxMsg = 0;
			}
		}

		MQX_TICK_STRUCT ticks;
		bool bWrap;
		 _time_get_elapsed_ticks(&ticks);
		 int32_t nDiff = _time_diff_milliseconds(&ticks, &startTicks, &bWrap);

		if (nDiff>(CONNECT_MANAGEMENT_PERIOD*2))
		{
			olyConfig->Run(nDiff);
			startTicks = ticks;
		}
		else if (bWrap)
			startTicks = ticks;

		TASKDEBUG_OUT(TASK_CONFIG)
	}
}

#if 0	// EXCLUDE OTHER TASKS FOR THE MOMENT

/* Background task for refreshing the display */
void GUI_Task(uint32_t task_init_data)
{
#ifdef USE_OLY_UI
	olyConfig->olyUI.IdleTimer_Reset();
#endif

	printf("GUI task started\n");

#if USE_OLY_UI
	D4D_Poll();		// Call once to initialize the display before enabling backlight
//	olyConfig->SetLcdBrightness(LCD_BRIGHT_VALUE);
#endif // USE_OLY_UI

	while (1) {
		TASKDEBUG_IN(TASK_GUI)

		poll_inputs();
		poll_encoder();
		poll_logo(20);
#if USE_OLY_UI
		olyConfig->olyUI.UpdateActiveScreen();
		olyConfig->olyUI.UpdateStatusIndicator(GPIO_DRV_ReadPinInput(DEBUG_LED_A));
		g_ScreenInTransition = true;
		D4D_Poll();
		g_ScreenInTransition = false;
#endif // USE_OLY_UI

#if USE_CDD_UI && !MFG_TEST_MARTIN
		olyConfig->GetCurrentUIMeter();
		cddl_poll_led_blink(20);
#endif
		TASKDEBUG_POS(TASK_GUI,3)

		olyConfig->MandolinProcess(TASK_GUI);
		TASKDEBUG_OUT(TASK_GUI)

		_time_delay(20);
	}
}

#endif

/* Solely for monitoring the Ultimo UART. */
void Dante_Task(uint32_t task_init_data)
{
	printf("Dante task created\n");

	g_pUltimoPort = new UltimoPort(olyConfig);

	if (!g_pUltimoPort->Initialize(FSL_UART_ULTIMO))
	{
		printf("Dante task could not initialized!\n");
	}
	else
	{

		MQX_TICK_STRUCT startTicks;

		 _time_get_elapsed_ticks(&startTicks);


			MQX_TICK_STRUCT ticks;
			bool bWrap;
			TASKDEBUG_IN(TASK_DANTE)

			 _time_get_elapsed_ticks(&ticks);

			 int32_t nDiff = _time_diff_milliseconds(&ticks, &startTicks, &bWrap);

			if ((0!=nDiff) || bWrap)
			{
				g_pUltimoPort->Run(nDiff);
				startTicks = ticks;


#ifdef LOCAL_MQX_KERNEL_LOGGING
				/* Read data from kernel log */
//				printf("\nKernel log contains:\n");
				while (_klog_display()){
				}
#endif	//	LOCAL_MQX_KERNEL_LOGGING
			}
			TASKDEBUG_OUT(TASK_DANTE)

			_time_delay(100);
		}
	}
}

#if 0
/* Handles User Interface Events */
void UserEvents_Task(uint32_t task_init_data)
{
	uint32_t 				ui_bits;
	uint32_t 				result;
	mandolin_mqx_message *	msg_ptr;
	MQX_TICK_STRUCT 		ticks;

	printf("UI task created\n");

	while (1) {
		_lwevent_wait_ticks(&user_event,0xFFFFFFFF,FALSE,0);
		ui_bits = _lwevent_get_signalled();

		TASKDEBUG_IN(TASK_USER)

#if	USE_CDD_UI
		if (ui_bits & event_SW2){
		  printf("SW2 event detected\n");
		  olyConfig->Switch2();
		  _lwevent_clear(&user_event, event_SW2);
		}
		else if (ui_bits & event_SW2_LONG){
		  printf("SW2 long event detected\n");
		  olyConfig->SW2_long();
		  _lwevent_clear(&user_event, event_SW2_LONG);
		}
		else if (ui_bits & event_SW2_LONG_LONG){
		  printf("SW2 long long event detected\n");
		  olyConfig->SW2_long_long();
		  _lwevent_clear(&user_event, event_SW2_LONG_LONG);
		}
#else
		if (ui_bits & event_SW2){
		  printf("SW2 event detected\n");
		  _lwevent_clear(&user_event, event_SW2);
		}
#endif // end USE_CC_UI


		else if (ui_bits & event_EncoderL) {
//			LogoMfgDemoCycleDec();
			olyConfig->EncoderL();
			_lwevent_clear(&user_event, event_EncoderL);
		}
		else if (ui_bits & event_EncoderR) {
//			LogoMfgDemoCycleInc();
			olyConfig->EncoderR();
			_lwevent_clear(&user_event, event_EncoderR);
		}
		else if (ui_bits & event_EncoderSW) {
			olyConfig->EncoderSW();

#if 0	//	Test Logo
		logo_enabled = !logo_enabled;

		if (!logo_enabled)
			olyConfig->UpdateLogoState(LOGO_OFF);
		else
			olyConfig->UpdateLogoState(logoMfgDemoSeq[logoMfgDemoIdx]);
#endif

#if 0	// Test routine to scroll between audio sources
		uint8_t src = olyConfig->GetInputSelectMode();
		src++;
		if( src > 3 )	src = 1;
		olyConfig->SetInputSelectMode((LOUD_audio_mode) src);
#endif

			_lwevent_clear(&user_event, event_EncoderSW);
		}

		else if ( ui_bits & event_IdentifyStop) {
			printf("Identify stopped by timeout\n");
			olyConfig->IdentifyStop();
			_lwevent_clear(&user_event, event_IdentifyStop);
		}

#if USE_OLY_UI
		else if ( ui_bits & event_EngageProfile) {
			// TODO: does this logic need to be model-specific since some models' UI objects aren't 1-to-1 (the FR boxes' UI says 'Profile' but it actually corresponds to the 'auto' screen under the hood)
			if (olyConfig->olyUI.GetCurrentScreen() == SCREEN_CONTOUR) {
				olyConfig->Controls_SetProfile(olyConfig->olyUI.Screen_Contour.GetHighlightedObject());
			} else if (olyConfig->olyUI.GetCurrentScreen() == SCREEN_CONFIG){
				olyConfig->Controls_SetXoverMode(olyConfig->olyUI.Screen_Config.GetHighlightedObject());
			} else if (olyConfig->olyUI.GetCurrentScreen() == SCREEN_POLARITY){
				olyConfig->Controls_SetPolarityInvert(olyConfig->olyUI.Screen_Polarity.GetHighlightedObject());
			}
			_lwevent_clear(&user_event, event_EngageProfile);
		}

#if OLY_UI_OPTILOGIC
		else if ( ui_bits & event_OptimizingDone) {
			olyConfig->olyUI.SetCurrentScreen(&olyConfig->olyUI.Screen_Auto, SCREEN_AUTO);
			olyConfig->olyUI.LongIdleTimer_Reset();
			if(olyConfig->olyUI.Screen_Auto.GetHighlightedObject() != OPTIMIZE_Option_Back)
				olyConfig->olyUI.Screen_Auto.ScrollDown();

			_lwevent_clear(&user_event, event_OptimizingDone);
		}
#endif // OLY_UI_OPTILOGIC

		else if (ui_bits & event_brightness_change){
			if(!g_ScreenInTransition)
				olyConfig->RampLcdBrightness(true, 0);
		}

#endif // USE_OLY_UI

		else {
			_lwevent_clear(&user_event, (0xFFFFFFFF));	// clear unhandled flags
		}

		TASKDEBUG_OUT(TASK_USER)

	}
}

/* Handles Amplifier events */
uint32 gSPIReadMessageNotRequiredCount=0;
void SysEvents_Task(uint32_t task_init_data)
{
	uint8_t input_bits;
	mandolin_mqx_message * pMsg;
	int nCount;
	uint32 uiSize;

	printf("SysEvents task created\n");

	// kick the DSP error timer at boot
	olyConfig->DSPErrorTimer_Reset();

	while (1) {
		_lwevent_wait_ticks(&sys_event,EVENTS_ALL,FALSE,0);
		input_bits = _lwevent_get_signalled();

		TASKDEBUG_IN(TASK_SYSEVENT)

		if (input_bits & event_amp_clip){
			TASKDEBUG_POS(TASK_SYSEVENT,1)

			printf("amp_clip event detected\n");
			_lwevent_clear(&sys_event, event_amp_clip);
		}
#if !MFG_TEST_EAW && !MFG_TEST_MARTIN
		else if (input_bits & event_amp_fault){
			TASKDEBUG_POS(TASK_SYSEVENT,2)

			olyConfig->OnAmpFaultStatusChanged(get_amp_fault_state());
			printf("Amp Fault = %d\n", get_amp_fault_state());
			_lwevent_clear(&sys_event, event_amp_fault);
		}
#endif //  !MFG_TEST_EAW && !MFG_TEST_MARTIN
		else if(input_bits & event_dsp_tx_ready )
		{
			TASKDEBUG_POS(TASK_SYSEVENT,3)
			_lwevent_clear( &sys_event, event_dsp_tx_ready );

			// DSP has pending message, read some bytes and process
//			while(GPIO_DRV_ReadPinInput(SHARC_SPI_READY)) {		// TODO while loop is disabled to prevent lockup when SPI driver crashes
			nCount = 0;
			uiSize = SHARC_BYTES_TO_READ_FIRST;
			while (GPIO_DRV_ReadPinInput(SHARC_SPI_READY) && (nCount<2)) {		// TODO while loop is disabled to prevent lockup when SPI driver crashes
				olyConfig->olyDspPort.ReadPendingMessage(uiSize);

				TASKDEBUG_POS(TASK_SYSEVENT,30)
				nCount++;
				uiSize = SHARC_BYTES_TO_READ_NEXT;
			}
			if (GPIO_DRV_ReadPinInput(SHARC_SPI_READY))		// if exited the loop but still ready to run
			{
				_lwevent_set( &sys_event, event_dsp_tx_ready );	// Event is cleared by interrupt handler's falling edge
			}

		}


		else if (input_bits & event_net_connect){
			TASKDEBUG_POS(TASK_SYSEVENT,4)

			olyConfig->OnNetworkConnectionChanged(true);
			printf("Network connect event detected\n");
			_lwevent_clear(&sys_event, event_net_connect);
		}

		else if (input_bits & event_net_disconnect){
			TASKDEBUG_POS(TASK_SYSEVENT,5)

			olyConfig->OnNetworkConnectionChanged(false);
			olyConfig->olyNetworkPort.KillAllSubscriptions();
			printf("Network disconnect event detected\n");
			_lwevent_clear(&sys_event, event_net_disconnect);
		}

		else if (input_bits & event_dsp_mute){

			olyConfig->OnDanteMuteChanged();
			_lwevent_clear(&sys_event, event_dsp_mute);
		}

#if USE_OLY_UI && !MFG_TEST_EAW && !MFG_TEST_MARTIN
		else if (input_bits & event_AlertScreenTimeout){
			if(olyConfig->GetAmpFaultStatus())	{
				olyConfig->olyUI.AlertTimer_Reset();
			}
			else {
				_lwevent_set(&timer_event, event_timer_backpanel_ui_timeout);
			}
			_lwevent_clear(&sys_event, event_AlertScreenTimeout);
		}

		else if (input_bits & event_DSPError){
			olyConfig->olyUI.ShowHWErrorScreen(HW_ERROR_DSP);
			olyConfig->DSPErrorTimer_Reset();
			_lwevent_clear(&sys_event, event_DSPError);
		}
#endif	// USE_OLY_UI

		else {
#if USE_OLY_UI
			_lwevent_clear(&sys_event, (EVENTS_ALL & !event_dsp_mute & !event_amp_clip & !event_amp_fault & !event_net_connect & !event_net_disconnect));	// clear unhandled flags
#else
			_lwevent_clear(&sys_event, (EVENTS_ALL & !event_amp_clip & !event_amp_fault & !event_net_connect & !event_net_disconnect
													& !event_AlertScreenTimeout & !event_DSPError));	// clear unhandled flags
#endif	// USE_OLY_UI
#if MFG_TEST_EAW || MFG_TEST_MARTIN
			_lwevent_clear(&sys_event, event_amp_fault);
#endif
			}

		TASKDEBUG_OUT(TASK_SYSEVENT)

	}
}

/* Handles Timer Events */
void TimeKeeper_Task(uint32_t task_init_data)
{
	uint32_t timer_bits;
	uint32_t subscription_bits;
	uint8_t ix;

	MQX_TICK_STRUCT heartbeat_ticks;
	MQX_TICK_STRUCT flashWrite_ticks;
	MQX_TICK_STRUCT ampMonitoring_ticks;

	_timer_id heartbeat_timer;
	_timer_id flashWrite_timer;
	_timer_id ampMonitoring_timer;

	/* Create Heartbeat Timer */
	_time_init_ticks(&heartbeat_ticks, 0);
	_time_add_msec_to_ticks(&heartbeat_ticks, TIMEOUT_HEARTBEAT);
	heartbeat_timer = _timer_start_periodic_every_ticks(HeartbeatTimer, 0, TIMER_ELAPSED_TIME_MODE, &heartbeat_ticks);

	/* Create Amp Temperature Monitoring Timer */
	_time_init_ticks(&ampMonitoring_ticks, 0);
	_time_add_msec_to_ticks(&ampMonitoring_ticks, TIMEOUT_AMPMONITORING);
	ampMonitoring_timer = _timer_start_periodic_every_ticks(CheckAmps, 0, TIMER_ELAPSED_TIME_MODE, &ampMonitoring_ticks);

	/* Create Flash Timer */
	_time_init_ticks(&flashWrite_ticks, 0);
	_time_add_sec_to_ticks(&flashWrite_ticks, TIMEOUT_FLASHWRITE);

	printf("TimeKeeper task created\n");

	while(1)
	{
		_lwevent_wait_ticks(&timer_event,0xffffffff,FALSE,0);
		timer_bits = _lwevent_get_signalled();
		TASKDEBUG_IN(TASK_TIMER)
		if( timer_bits & event_timer_flashwrite_start ) {
			if( flashWrite_timer )	_timer_cancel( flashWrite_timer );
			flashWrite_timer = _timer_start_oneshot_after_ticks(TriggerFlashWrite, 0, TIMER_ELAPSED_TIME_MODE, &flashWrite_ticks);
			_lwevent_clear(&timer_event, event_timer_flashwrite_start);
		}

		//this is now handled by aux_spi_task
		else if( timer_bits & event_timer_flashwrite_expired ) {
		//	olyConfig->WriteParamsToFlash();
			_lwevent_set( &aux_spi_event, system_flash_write_event );
			_lwevent_clear(&timer_event, event_timer_flashwrite_expired);
		}

		else if ( timer_bits & event_timer_amp_temp_update) {
			olyConfig->OnAmpTempChanged(sPro2_amp1, olyConfig->GetCurrentAmpTemp(sPro2_amp1));
			olyConfig->OnAmpTempChanged(sPro2_amp2, olyConfig->GetCurrentAmpTemp(sPro2_amp2));
			olyConfig->UpdateCurrentTiltAngle();
			_lwevent_clear(&timer_event, event_timer_amp_temp_update);
		}

		else if ( timer_bits & event_timer_self_test_switch) {
			olyConfig->SelfTestSequence();
			_lwevent_clear(&timer_event, event_timer_self_test_switch);
		}

		else if ( timer_bits & event_timer_backpanel_ui_timeout) {
#ifdef USE_OLY_UI
			if(olyConfig->GetAmpFaultStatus())	{
				olyConfig->olyUI.ShowAlertScreen();
				olyConfig->olyUI.AlertTimer_Reset();
			}
			else {
				switch(olyConfig->GetDisplayUserMode())	{
				case eDISP_MODE_BRIGHT:
					#if USE_BACKPANEL_DEBUG
					olyConfig->olyUI.ShowStatusScreen();
					#else
					olyConfig->olyUI.ShowDefaultScreen();
					#endif	// USE_BACKPANEL_DEBUG
					break;
				case eDISP_MODE_OFF:
					olyConfig->olyUI.ShowMenuScreen();
					olyConfig->DimLcdBrightness(true);
					break;
				case eDISP_MODE_DIM:
					olyConfig->DimLcdBrightness(true);
					olyConfig->olyUI.ShowDefaultScreen();
					break;
				}
			}

			olyConfig->olyUI.Screen_Settings.CancelHiddenCount();
#endif
			_lwevent_clear(&timer_event, event_timer_backpanel_ui_timeout);
		}

		else if ( timer_bits & event_timer_subscription_Voicing1) {
#if !USES_FOUR_IRDA			
			olyConfig->OnParamSubscriptionEventVoicingList(1);
#endif
			_lwevent_clear(&timer_event, event_timer_subscription_Voicing1);
		}

		else if ( timer_bits & event_timer_subscription_Voicing2) {

#if !USES_FOUR_IRDA			
			olyConfig->OnParamSubscriptionEventVoicingList(2);
#endif
			_lwevent_clear(&timer_event, event_timer_subscription_Voicing2);
		}

		//	Leave space for 10 of these, one for each port
		else if ( timer_bits & event_timer_subscription_Network1) {
			olyConfig->OnParamSubscriptionEventNetworkList(1);

			_lwevent_clear(&timer_event, event_timer_subscription_Network1);
		}

		//	Leave space for 10 of these, one for each port
		else if ( timer_bits & event_timer_subscription_Network2) {
			olyConfig->OnParamSubscriptionEventNetworkList(2);

			_lwevent_clear(&timer_event, event_timer_subscription_Network2);
		}

		else
			_lwevent_clear(&timer_event, 0xffffffff);

		TASKDEBUG_OUT(TASK_TIMER)

	}
}

bool IsIpConnected()
{
	bool bIsIpConnected = network_initialized;
	//	if DHCP, verify received address
	bool bStaticIp = olyConfig && (!olyConfig->GetDhcp());
	if (bStaticIp)
	{
		if (!g_pUltimoPort)
			bIsIpConnected = false;
		else
		{
			uint16 linkFlags;
			if (!g_pUltimoPort->GetLinkFlags(&linkFlags))
				bIsIpConnected = false;
			else if (0==(linkFlags&1))
				bIsIpConnected = false;
		}
	}
	else if (0==g_CurrentIpAddress)
		bIsIpConnected = false;
	return(bIsIpConnected);
}


/* mDNS Responder */
void Discovery_Task(uint32_t task_init_data)
{
	printf("Discovery task created\n");

	g_pDiscovery = new Discovery(olyConfig->GetDiscoServiceName(), olyConfig && olyConfig->GetDhcp());

	if (!g_pDiscovery)
	{
		// error on creation task
		return;

	}
	MQX_TICK_STRUCT startTicks;

	while (1){

		MQX_TICK_STRUCT ticks;
		bool bWrap = false;

		TASKDEBUG_IN(TASK_DISCO)

		 _time_get_elapsed_ticks(&ticks);

		 int32_t nDiff = _time_diff_milliseconds(&ticks, &startTicks, &bWrap);

		if ((0!=nDiff) || bWrap)
		{
			TASKDEBUG_POS(TASK_DISCO,1)
			bool bIsIpConnected = IsIpConnected();
			TASKDEBUG_POS(TASK_DISCO,2)
			g_pDiscovery->Run(bIsIpConnected, nDiff);
			TASKDEBUG_POS(TASK_DISCO,0x60)
			startTicks = ticks;
		}
		TASKDEBUG_OUT(TASK_DISCO)

		_time_delay(100);
	}
}

/*	Handles configuration of speaker parameters, currently only used by Dante link status */
void SpkrCnfg_Task( uint32_t task_init_data )
{
	uint32_t event_spkr_cnfg_bits;

	if( _event_create( (char*) "event_spkr_cnfg") != MQX_OK )	printf("Speaker Configuration event create error\n");
	if( _event_open( (char*) "event_spkr_cnfg", &event_spkr_cnfg_p) != MQX_OK )	printf("Speaker Configuration event open error\n");
	//olyConfig->SetInputSelectMode(eAUDIO_MODE_AUTO); I think this was causing the speaker to always default the auto.

	printf("Speaker Configuration task started\n");

	while( TRUE )
	{
		_event_wait_any(event_spkr_cnfg_p, EVENTS_ALL, 0);
		_event_get_value( event_spkr_cnfg_p, &event_spkr_cnfg_bits);
		TASKDEBUG_IN(TASK_SPKR_CNFG)

		if((LOUD_audio_mode)olyConfig->GetInputSelectMode() != eAUDIO_MODE_AUTO)
		{
			_event_clear( event_spkr_cnfg_p, EVENTS_ALL );
		}
		else if( event_spkr_cnfg_bits & event_select_audio_src_none )
		{
			if( olyConfig->GetAudioSource() != src_none )	olyConfig->SetAudioSource( src_none );
			_event_clear( event_spkr_cnfg_p, event_select_audio_src_none );
		}

		else if( event_spkr_cnfg_bits & event_select_audio_src_anlg )
		{
			if( olyConfig->GetAudioSource() != src_analog )	olyConfig->SetAudioSource( src_analog );
			_event_clear( event_spkr_cnfg_p, event_select_audio_src_anlg );
		}

		else if( event_spkr_cnfg_bits & event_select_audio_src_dnte )
		{
			if( olyConfig->GetAudioSource() != src_dante )	olyConfig->SetAudioSource( src_dante );
			_event_clear( event_spkr_cnfg_p, event_select_audio_src_dnte );
		}

		else if( event_spkr_cnfg_bits & event_select_audio_src_pink )
		{
			if( olyConfig->GetAudioSource() != src_pinknoise )	olyConfig->SetAudioSource( src_pinknoise );
			_event_clear( event_spkr_cnfg_p, event_select_audio_src_pink );
		}

		else _event_clear( event_spkr_cnfg_p, EVENTS_ALL );

		TASKDEBUG_OUT(TASK_SPKR_CNFG)

	}
}

/*	Handles meter polling, limiter LED control, and meter subscription */
#if(1)
void Meter_Task( uint32_t task_init_data )
{
	static int s_meter_skip = 0;
	_time_delay(METER_POLLING_WAIT_MS);		// Wait 5 seconds before starting metering

	printf("Periodic Polling started\n");

	while( TRUE )
	{
		TASKDEBUG_IN(TASK_METER)

		//	Add mechanism to not request meters from DSP until last meters received or 1 second of non-responsiveness
		if (g_bMeterComplete || (!s_meter_skip))
		{
			s_meter_skip = 1000/DSP_METER_RATE_MS;
			g_bMeterComplete = false;
			olyConfig->GetCurrentMeter();
		}
		else
			s_meter_skip--;
		TASKDEBUG_OUT(TASK_METER)

		_time_delay(DSP_METER_RATE_MS);
	}
}
#endif

/* Background task for measuring speaker impedance */
void AUX_spi_Task(uint32_t task_init_data)
{
	printf("AUX Spi task started\n");
	uint32_t timer_bits;

#if HAS_CURRENT_SENSE

	uint32_t int_result;
	bool impedance_changed;
	//initialize Mandolin output registers
	olyConfig->OnCurrentSenseImpedanceChanged(0);
	olyConfig->OnCurrentSenseImpedanceChanged(1);
	olyConfig->OnCurrentSenseImpedanceChanged(2);
#endif
	//load up cal constants into local C data structure for now for optimal speed
	_time_delay(1000);
	//olyConfig->CurrentSenseGetCalConst(current_sense_vi_cal);
	//olyConfig->CurrentSenseGetImpedanceLimits(current_sense_Z_limits);

	while ( TRUE ) {
		//_time_delay(1000);

		TASKDEBUG_IN(TASK_AUX_SPI)
		_lwevent_wait_ticks(&aux_spi_event,EVENTS_ALL,FALSE,0);
		timer_bits = _lwevent_get_signalled();
		if( timer_bits & system_flash_write_event ) {
			olyConfig->WriteParamsToFlash();
			_lwevent_clear(&aux_spi_event, system_flash_write_event);
			//use the next line to long term test flash
		//	_lwevent_set( &timer_event, event_timer_flashwrite_start );

		}

		if( timer_bits & current_sense_new_meter ) {
#if HAS_CURRENT_SENSE
#if HAS_LF
			//change DB back to fullscale for now - allows us to use shared meter
			int_result = (uint32_t)(exp10f(olyConfig->CurrentSenseGetOutputMeter(0)/20.0)* (float32)VOLTAGE_FULLSCALE_COUNT);
			impedance_changed = CurrentSenseProcessVoltage(0, int_result);
			if(impedance_changed) olyConfig->OnCurrentSenseImpedanceChanged(0);
#endif
#if HAS_MF
			int_result = (uint32_t)(exp10f(olyConfig->CurrentSenseGetOutputMeter(1)/20.0)* (float32)VOLTAGE_FULLSCALE_COUNT);
			impedance_changed = CurrentSenseProcessVoltage(1, int_result);
			if(impedance_changed) olyConfig->OnCurrentSenseImpedanceChanged(1);
#endif
#if HAS_LF2
			int_result = (uint32_t)(exp10f(olyConfig->CurrentSenseGetOutputMeter(0)/20.0)* (float32)VOLTAGE_FULLSCALE_COUNT);
			impedance_changed = CurrentSenseProcessVoltage(2, int_result);
			if(impedance_changed) olyConfig->OnCurrentSenseImpedanceChanged(2);
#elif HAS_HF
			int_result = (uint32_t)(exp10f(olyConfig->CurrentSenseGetOutputMeter(2)/20.0)* (float32)VOLTAGE_FULLSCALE_COUNT);
			impedance_changed = CurrentSenseProcessVoltage(2, int_result);
			if(impedance_changed) olyConfig->OnCurrentSenseImpedanceChanged(2);
#endif
#endif
			_lwevent_clear(&aux_spi_event, current_sense_new_meter);
		}

		TASKDEBUG_OUT(TASK_AUX_SPI)
	}
}






/*
 * Irda_Process_Task
 * Init IRDA task objects
 * process input rx data
 */
#define IRDA_PROCESS_TICKS_TIMEOUT_MS 10	//must be > 5
#define IRDA_PROCESS_TIMEOUT_TICKS (IRDA_PROCESS_TICKS_TIMEOUT_MS / 5) //system tick is 5 ms (5000us defined by BSP_ALARM_PERIOD)

void Irda_Process_Task(uint32_t task_init_data)
{
	uint32_t irda_process_events;
	_mqx_uint result;
	int32_t time;
	uart_Irda_A_enabled = true;
	uart_Irda_B_enabled = true;
#if USES_FOUR_IRDA
	uart_Irda_C_enabled = true;
	uart_Irda_D_enabled = true;
#endif

	IRDAManager_hw_position_init();

	while(TRUE)
	{
		result = _lwevent_wait_ticks(&irda_lwevent,0xffffffff,FALSE, IRDA_PROCESS_TIMEOUT_TICKS);
		TASKDEBUG_IN(TASK_IRDA_PROCESS)
		if( result == MQX_OK )
		{
			irda_process_events = _lwevent_get_signalled();

/* IRDA message processing events */
			if(irda_process_events & irda_event_new_irda_A)
			{
				_lwevent_clear(&irda_lwevent, irda_event_new_irda_A);
				irdaTopPortTask->pIrdaUartPort->ProcessRxFifo(TASK_IRDA_PROCESS); 		//checks irda message, if valid, pass to irda manager mandolin fifo for processing
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
//				olyConfig->olyUI.DebugOut((char*)"New Top");
#endif	// USE_BACKPANEL_DEBUG
			}

			if(irda_process_events & irda_event_new_irda_B)
			{
				_lwevent_clear(&irda_lwevent, irda_event_new_irda_B);
				irdaBottomPortTask->pIrdaUartPort->ProcessRxFifo(TASK_IRDA_PROCESS); 	//checks irda message, if valid, pass to irda manager mandolin fifo for processing
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
//				olyConfig->olyUI.DebugOut((char*)"New Bottom");
#endif	// USE_BACKPANEL_DEBUG
			}

			if(irda_process_events & irda_event_new_irda_C)
			{
				_lwevent_clear(&irda_lwevent, irda_event_new_irda_C);
				irdaTopRearPortTask->pIrdaUartPort->ProcessRxFifo(TASK_IRDA_PROCESS); 	//checks irda message, if valid, pass to irda manager mandolin fifo for processing
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
//				olyConfig->olyUI.DebugOut((char*)"New Top Rear");
#endif	// USE_BACKPANEL_DEBUG
			}

			if(irda_process_events & irda_event_new_irda_D)
			{
				_lwevent_clear(&irda_lwevent, irda_event_new_irda_D);
				irdaBottomRearPortTask->pIrdaUartPort->ProcessRxFifo(TASK_IRDA_PROCESS); 	//checks irda message, if valid, pass to irda manager mandolin fifo for processing
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
//				olyConfig->olyUI.DebugOut((char*)"New Bottom Rear");
#endif	// USE_BACKPANEL_DEBUG
			}

/* IRDA link timer events */
			if(irda_process_events & irda_event_irdaA_timerCB)
			{
				_lwevent_clear(&irda_lwevent, irda_event_irdaA_timerCB);
				olyConfig->olyIrdaPortA.port_task->IRDA_TimerCB();		//link time callback
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
//				olyConfig->olyUI.DebugOut((char*)"Link Top");
#endif	// USE_BACKPANEL_DEBUG
			}

			if(irda_process_events & irda_event_irdaB_timerCB)
			{
				_lwevent_clear(&irda_lwevent, irda_event_irdaB_timerCB);
				olyConfig->olyIrdaPortB.port_task->IRDA_TimerCB();		//link timer callback
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
//				olyConfig->olyUI.DebugOut((char*)"Link Bottom");
#endif	// USE_BACKPANEL_DEBUG
			}

			if(irda_process_events & irda_event_irdaC_timerCB)
			{
				_lwevent_clear(&irda_lwevent, irda_event_irdaC_timerCB);
				olyConfig->olyIrdaPortC.port_task->IRDA_TimerCB();		//link time callback
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
//				olyConfig->olyUI.DebugOut((char*)"Link Top Rear");
#endif	// USE_BACKPANEL_DEBUG
			}

			if(irda_process_events & irda_event_irdaD_timerCB)
			{
				_lwevent_clear(&irda_lwevent, irda_event_irdaD_timerCB);
				olyConfig->olyIrdaPortD.port_task->IRDA_TimerCB();		//link timer callback
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
//				olyConfig->olyUI.DebugOut((char*)"Link Bottom Rear");
#endif	// USE_BACKPANEL_DEBUG
			}
/* array status timer CB */
			if(irda_process_events & irda_event_irdaA_array_status_timerCB)
			{
				_lwevent_clear(&irda_lwevent, irda_event_irdaA_array_status_timerCB);
				olyConfig->olyIrdaPortA.port_task->IRDA_TimerArrayStatus_CB();
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
//				olyConfig->olyUI.DebugOut((char*)"Status Timeout Top");
#endif	// USE_BACKPANEL_DEBUG
			}

			if(irda_process_events & irda_event_irdaB_array_status_timerCB)
			{
				_lwevent_clear(&irda_lwevent, irda_event_irdaB_array_status_timerCB);
				olyConfig->olyIrdaPortB.port_task->IRDA_TimerArrayStatus_CB();
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
//				olyConfig->olyUI.DebugOut((char*)"Status Timeout Bottom");
#endif	// USE_BACKPANEL_DEBUG
			}

			if(irda_process_events & irda_event_irdaC_array_status_timerCB)
			{
				_lwevent_clear(&irda_lwevent, irda_event_irdaC_array_status_timerCB);
				olyConfig->olyIrdaPortC.port_task->IRDA_TimerArrayStatus_CB();
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
//				olyConfig->olyUI.DebugOut((char*)"Status Timeout Top Rear");
#endif	// USE_BACKPANEL_DEBUG
			}

			if(irda_process_events & irda_event_irdaD_array_status_timerCB)
			{
				_lwevent_clear(&irda_lwevent, irda_event_irdaD_array_status_timerCB);
				olyConfig->olyIrdaPortD.port_task->IRDA_TimerArrayStatus_CB();
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
//				olyConfig->olyUI.DebugOut((char*)"Status Timeout Bottom Rear");
#endif	// USE_BACKPANEL_DEBUG
			}

/* IRDA notify events */
			if(irda_process_events & irda_event_neighbour_hw_status_update)
			{
				_lwevent_clear(&irda_lwevent, irda_event_neighbour_hw_status_update);
				olyConfig->UpdateNeighbourHardwareStatus();
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
//				olyConfig->olyUI.DebugOut((char*)"Neighbor HW Update");
#endif	// USE_BACKPANEL_DEBUG
			}

			if(irda_process_events & irda_event_array_status_update)
			{
				_lwevent_clear(&irda_lwevent, irda_event_array_status_update);
				int8_t size, position, optSize, optPosition, optSubSize, optSubPosition, optOrientation, arrayMismatch;
				float deviceHeightOffset;
				uint32_t active_profile;
				bool arrayReady;
				IRDA_GetArrayStatus(&size, &position, &deviceHeightOffset, &optSize, &optPosition, &optSubSize, &optSubPosition, &optOrientation, &arrayMismatch, &arrayReady);
				olyConfig->UpdateArrayStatus(size, position, deviceHeightOffset, optSize, optPosition, optSubSize, optSubPosition, optOrientation, arrayMismatch, arrayReady);
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
				olyConfig->olyUI.DebugOut((char*)"Array Status Update");
#endif	// USE_BACKPANEL_DEBUG
			}

/* IRDA handle optimize from IRDA message event */
			if(irda_process_events & irda_event_array_optimize_handle)
			{
				_lwevent_clear(&irda_lwevent, irda_event_array_optimize_handle);
				array_opt_vals_t opt_vals;
				IRDA_GetOptMsgValues(&opt_vals);
				olyConfig->Controls_AutoOptimize(opt_vals.OptMax, opt_vals.OptMin, opt_vals.OptHeight, optimize_source_irda, opt_vals.irda_Active_Profile_instance);
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
				olyConfig->olyUI.DebugOut((char*)"Optimize Handle");
#endif	// USE_BACKPANEL_DEBUG
			}

/* IRDA clear optimize from IRDA message event */
			if(irda_process_events & irda_event_array_optimize_clear)
			{
				_lwevent_clear(&irda_lwevent, irda_event_array_optimize_clear);
				olyConfig->ClearOptimization();
			}

/* IRDA send optimize over IRDA event */
			if(irda_process_events & irda_event_array_optimize_send)
			{
				_lwevent_clear(&irda_lwevent, irda_event_array_optimize_send);
				IRDATask_AutoOptimizeStartFromTask();
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
				olyConfig->olyUI.DebugOut((char*)"Optimize Send");
#endif	// USE_BACKPANEL_DEBUG
			}

/* optimize message timeout events */
			if(irda_process_events & irda_event_irdaA_optimize_timerCB)
			{
				_lwevent_clear(&irda_lwevent, irda_event_irdaA_optimize_timerCB);
				olyConfig->olyIrdaPortA.port_task->IRDA_OptimizeArray_CB();
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
				olyConfig->olyUI.DebugOut((char*)"Optimize CB TF");
#endif	// USE_BACKPANEL_DEBUG
			}

			if(irda_process_events & irda_event_irdaB_optimize_timerCB)
			{
				_lwevent_clear(&irda_lwevent, irda_event_irdaB_optimize_timerCB);
				olyConfig->olyIrdaPortB.port_task->IRDA_OptimizeArray_CB();
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
				olyConfig->olyUI.DebugOut((char*)"Optimize CB BF");
#endif	// USE_BACKPANEL_DEBUG
			}

			if(irda_process_events & irda_event_irdaC_optimize_timerCB)
			{
				_lwevent_clear(&irda_lwevent, irda_event_irdaC_optimize_timerCB);
				olyConfig->olyIrdaPortC.port_task->IRDA_OptimizeArray_CB();
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
				olyConfig->olyUI.DebugOut((char*)"Optimize CB TR");
#endif	// USE_BACKPANEL_DEBUG
			}

			if(irda_process_events & irda_event_irdaD_optimize_timerCB)
			{
				_lwevent_clear(&irda_lwevent, irda_event_irdaD_optimize_timerCB);
				olyConfig->olyIrdaPortD.port_task->IRDA_OptimizeArray_CB();
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
				olyConfig->olyUI.DebugOut((char*)"Optimize CB BR");
#endif	// USE_BACKPANEL_DEBUG
			}
		}
		else
		{
			irdaTopPortTask->pIrdaUartPort->ProcessRxFifo(TASK_IRDA_PROCESS); 		//checks irda message, if valid, pass to irda manager mandolin fifo for processing
			irdaBottomPortTask->pIrdaUartPort->ProcessRxFifo(TASK_IRDA_PROCESS); 	//checks irda message, if valid, pass to irda manager mandolin fifo for processing
			irdaTopRearPortTask->pIrdaUartPort->ProcessRxFifo(TASK_IRDA_PROCESS);
			irdaBottomRearPortTask->pIrdaUartPort->ProcessRxFifo(TASK_IRDA_PROCESS);
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
//			olyConfig->olyUI.DebugOut((char*)"Process FIFO");
#endif	// USE_BACKPANEL_DEBUG
		}
		TASKDEBUG_OUT(TASK_IRDA_PROCESS)
	}
}


/* Processes irda tx messages */
void IRDA_Transmit_A_Task(uint32_t task_init_data)
{
	_queue_id			 	irda_qid;
	irda_mqx_message_ptr 	msg_ptr = NULL;

	//open message pool, only this task can read from this queue
	olyConfig->olyIrdaPortA.irda_tx_message_pool = init_irda_message_pool();

	if (!olyConfig->olyIrdaPortA.irda_tx_message_pool) {
		volatile int error = _task_errno;
		printf("IRDA_Transmit_A_Task pool alloc failed\n");
		_task_set_error(MQX_OK);
		while(1);
	}

	//configure the TARGET_QUEUE
	irda_qid = _msgq_open(IRDA_A_QUEUE, 0);
	olyConfig->olyIrdaPortA.port_qid = irda_qid;

	while (1) {

		msg_ptr = (irda_mqx_message_ptr)_msgq_receive(irda_qid, 0);	// Block waiting for message
		TASKDEBUG_IN(TASK_IRDA_TX_A)

		if (msg_ptr)
		{
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
			olyConfig->olyUI.DebugOut((char*)"Msg TX Top");
#endif	// USE_BACKPANEL_DEBUG
			_lwsem_wait(&olyConfig->olyIrdaPortA.irda_tx_queue_port_sem);			//block while port is CTS_IDLE
			olyConfig->olyIrdaPortA.WriteMessage(&msg_ptr->message);
			_lwsem_post(&olyConfig->olyIrdaPortA.irda_tx_queue_port_sem);
			_msg_free(msg_ptr);
		}

		TASKDEBUG_IN(TASK_IRDA_TX_A)

	}
}

void IRDA_Transmit_B_Task(uint32_t task_init_data)
{
	_queue_id			 	irda_qid;
	irda_mqx_message_ptr 	msg_ptr = NULL;

	//open message pool, only this task can read from this queue
	olyConfig->olyIrdaPortB.irda_tx_message_pool = init_irda_message_pool();

	if (!olyConfig->olyIrdaPortB.irda_tx_message_pool) {
		volatile int error = _task_errno;
		printf("IRDA_Transmit_B_Task pool alloc failed\n");
		_task_set_error(MQX_OK);
		while(1);
	}

	//configure the TARGET_QUEUE
	irda_qid = _msgq_open(IRDA_B_QUEUE, 0);
	olyConfig->olyIrdaPortB.port_qid = irda_qid;

	while (1) {

		msg_ptr = (irda_mqx_message_ptr)_msgq_receive(irda_qid, 0);	// Block waiting for message
		TASKDEBUG_IN(TASK_IRDA_TX_B)
		if (msg_ptr)
		{
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
			olyConfig->olyUI.DebugOut((char*)"Msg TX Bottom");
#endif	// USE_BACKPANEL_DEBUG
			_lwsem_wait(&olyConfig->olyIrdaPortB.irda_tx_queue_port_sem);			//block while port is CTS_IDLE
			olyConfig->olyIrdaPortB.WriteMessage(&msg_ptr->message);
			_lwsem_post(&olyConfig->olyIrdaPortB.irda_tx_queue_port_sem);
			_msg_free(msg_ptr);
		}
		TASKDEBUG_OUT(TASK_IRDA_TX_B)

	}
}

void IRDA_Transmit_C_Task(uint32_t task_init_data)
{
	_queue_id			 	irda_qid;
	irda_mqx_message_ptr 	msg_ptr = NULL;

	//open message pool, only this task can read from this queue
	olyConfig->olyIrdaPortC.irda_tx_message_pool = init_irda_message_pool();

	if (!olyConfig->olyIrdaPortC.irda_tx_message_pool) {
		volatile int error = _task_errno;
		printf("IRDA_Transmit_C_Task pool alloc failed\n");
		_task_set_error(MQX_OK);
		while(1);
	}

	//configure the TARGET_QUEUE
	irda_qid = _msgq_open(IRDA_C_QUEUE, 0);
	olyConfig->olyIrdaPortC.port_qid = irda_qid;

	while (1) {

		msg_ptr = (irda_mqx_message_ptr)_msgq_receive(irda_qid, 0);	// Block waiting for message
		TASKDEBUG_IN(TASK_IRDA_TX_C)
		if (msg_ptr)
		{
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
			olyConfig->olyUI.DebugOut((char*)"Msg TX TR");
#endif	// USE_BACKPANEL_DEBUG
			_lwsem_wait(&olyConfig->olyIrdaPortC.irda_tx_queue_port_sem);			//block while port is CTS_IDLE
			olyConfig->olyIrdaPortC.WriteMessage(&msg_ptr->message);
			_lwsem_post(&olyConfig->olyIrdaPortC.irda_tx_queue_port_sem);
			_msg_free(msg_ptr);
		}
		TASKDEBUG_OUT(TASK_IRDA_TX_C)

	}
}

void IRDA_Transmit_D_Task(uint32_t task_init_data)
{
	_queue_id			 	irda_qid;
	irda_mqx_message_ptr 	msg_ptr = NULL;

	//open message pool, only this task can read from this queue
	olyConfig->olyIrdaPortD.irda_tx_message_pool = init_irda_message_pool();

	if (!olyConfig->olyIrdaPortD.irda_tx_message_pool) {
		volatile int error = _task_errno;
		printf("IRDA_Transmit_D_Task pool alloc failed\n");
		_task_set_error(MQX_OK);
		while(1);
	}

	//configure the TARGET_QUEUE
	irda_qid = _msgq_open(IRDA_D_QUEUE, 0);
	olyConfig->olyIrdaPortD.port_qid = irda_qid;

	while (1) {

		msg_ptr = (irda_mqx_message_ptr)_msgq_receive(irda_qid, 0);	// Block waiting for message
		TASKDEBUG_IN(TASK_IRDA_TX_D)
		if (msg_ptr)
		{
#if USE_BACKPANEL_DEBUG & USE_DEBUG_OPTIMIZE
			olyConfig->olyUI.DebugOut((char*)"Msg TX BR");
#endif	// USE_BACKPANEL_DEBUG
			_lwsem_wait(&olyConfig->olyIrdaPortD.irda_tx_queue_port_sem);			//block while port is CTS_IDLE
			olyConfig->olyIrdaPortD.WriteMessage(&msg_ptr->message);
			_lwsem_post(&olyConfig->olyIrdaPortD.irda_tx_queue_port_sem);
			_msg_free(msg_ptr);
		}
		TASKDEBUG_OUT(TASK_IRDA_TX_D)

	}
}

#endif // EXCLUDE OTHER TASKS FOR THE MOMENT

