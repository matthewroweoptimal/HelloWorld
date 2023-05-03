/*
 * TcpThread.cpp
 *
 *  Created on: 21 Feb 2022
 *      Author: Matthew.Rowe
 */

#include "TcpThread.h"
#include "lwip/apps/mdns.h"
#include "network.h"
#include "os_tasks.h"
#include "Region.h"
#include "ConfigManager.h"
#include <cstring>

extern "C" {
 #include "otpFlash.h"
}


extern oly::Config *olyConfig;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MANDO_THREAD_PRIO    ( tskIDLE_PRIORITY + 2UL )
#define MANDO_THREAD_STACKSIZE  300
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


TcpThread::TcpThread(uint16_t usStackDepth, UBaseType_t uxPriority, SemaphoreHandle_t& semMainThreadComplete)
     : Thread("TCP Config Thread", usStackDepth, uxPriority),
       _semMainThreadComplete(semMainThreadComplete)
{
}

void TcpThread::Run()
{
	printf("TCP thread waiting on MainThread completing...\n");
	//--- Wait until MainThread complete ---
    xSemaphoreTake(_semMainThreadComplete, portMAX_DELAY);
    xSemaphoreGive(_semMainThreadComplete);						//this feels dodgy, any thread that picks up the semaphore gives again for other threads to start.
    printf("TCP thread running\n");

	Config_Task( 0 );	// In CDD code os_tasks.cpp, this ends up calling network_UseDHCP() which spawns TCP thread
	//--- Above will process messages until terminated ---

    Suspend();
}
