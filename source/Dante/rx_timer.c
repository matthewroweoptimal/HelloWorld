/**
* File     : rx_timer.c
* Created  : October 2014
* Updated  : 2014/10/08
* Author   : Chamira Perera
* Synopsis : HostCPU UHIP Rx timer implementation for Windows
*
* Copyright 2005-2014, Audinate Pty Ltd and/or its licensors
* Confidential
* Audinate Copyright Header Version 1
*/

#include "uhip_hostcpu_rx_timer.h"
#include <sys/time.h>
//#include "mqx.h"
//#include "MQX_To_FreeRTOS.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "stdbool.h"

//IQ - remangled these functions to use freertos TIMER functions.


static TickType_t g_rxUhipTickStart;
static unsigned int g_uiRxUhipMs = 0;
static int g_nRxTimerStarted = 0;

void uhip_hostcpu_rx_timer_init()
{
	memset(&g_rxUhipTickStart, 0, sizeof(TickType_t));
	g_nRxTimerStarted = 0;
	g_uiRxUhipMs = 0;
}

void uhip_hostcpu_rx_timer_start(int num_milliseconds)
{
	//printf("UHIP:Start Rx Timer %d ms\n", num_milliseconds);
	//_time_get_elapsed_ticks(&g_rxUhipTickStart);
	g_rxUhipTickStart = xTaskGetTickCount();
	g_uiRxUhipMs = (unsigned int)num_milliseconds;
	g_nRxTimerStarted = 1;
}

void uhip_hostcpu_rx_timer_stop()
{
	//printf("UHIP:Stop Rx Timer\n");
	memset(&g_rxUhipTickStart, 0, sizeof(TickType_t));
	g_uiRxUhipMs = 0;
	g_nRxTimerStarted = 0;
}

aud_bool_t uhip_hostcpu_rx_timer_is_running()
{
	bool bWrap=false;

	TickType_t ticks = xTaskGetTickCount();

	if (ticks<g_rxUhipTickStart)
		bWrap = true;

	TickType_t nMSec = (ticks - g_rxUhipTickStart) * 1000 / configTICK_RATE_HZ;

	return (g_nRxTimerStarted && (nMSec<g_uiRxUhipMs) && (!bWrap));
}

aud_bool_t uhip_hostcpu_rx_timer_has_fired()
{
	bool bWrap=false;

	TickType_t ticks = xTaskGetTickCount();

	if (ticks<g_rxUhipTickStart)
		bWrap = true;

	TickType_t nMSec = (ticks - g_rxUhipTickStart) * 1000 / configTICK_RATE_HZ;

	return (g_nRxTimerStarted && ((nMSec>=g_uiRxUhipMs) || (bWrap)));
}

