/**
* File     : tx_timer.c
* Created  : October 2014
* Updated  : 2014/10/08
* Author   : Chamira Perera
* Synopsis : HostCPU UHIP Tx timer implementation for windows
*
* Copyright 2005-2014, Audinate Pty Ltd and/or its licensors
* Confidential
* Audinate Copyright Header Version 1
*/

#include "uhip_hostcpu_tx_timer.h"
#include <sys/time.h>
//#include "mqx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "stdbool.h"

static TickType_t g_txUhipTickStart;
static unsigned int g_uiTxUhipMs = 0;
static int g_nTxTimerStarted = 0;

void uhip_hostcpu_tx_timer_init()
{
	memset(&g_txUhipTickStart, 0, sizeof(TickType_t));
	g_nTxTimerStarted = 0;
	g_uiTxUhipMs = 0;
}

void uhip_hostcpu_tx_timer_start(int num_milliseconds)
{
//	printf("UHIP:Start Tx Timer %dms\n", num_milliseconds);
	g_txUhipTickStart = xTaskGetTickCount();
	g_uiTxUhipMs = (unsigned int)num_milliseconds;
	g_nTxTimerStarted = 1;
}

void uhip_hostcpu_tx_timer_stop()
{
//	printf("UHIP:Stop Tx Timer\n");
	memset(&g_txUhipTickStart, 0, sizeof(TickType_t));
	g_uiTxUhipMs = 0;
	g_nTxTimerStarted = 0;
}

aud_bool_t uhip_hostcpu_tx_timer_is_running()
{
	bool bWrap=false;

	TickType_t ticks = xTaskGetTickCount();

	if (ticks<g_txUhipTickStart)
		bWrap = true;

	TickType_t nMSec = (ticks - g_txUhipTickStart) * 1000 / configTICK_RATE_HZ;

	return (g_nTxTimerStarted && (nMSec<g_uiTxUhipMs) && (!bWrap));
}

aud_bool_t uhip_hostcpu_tx_timer_has_fired()
{
	bool bWrap=false;

	TickType_t ticks = xTaskGetTickCount();

	if (ticks<g_txUhipTickStart)
		bWrap = true;

	TickType_t nMSec = (ticks - g_txUhipTickStart) * 1000 / configTICK_RATE_HZ;

	return (g_nTxTimerStarted && ((nMSec>=g_uiTxUhipMs) || (bWrap)));
}
