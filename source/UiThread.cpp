/*
 * TcpThread.cpp
 *
 *  Created on: 21 Feb 2022
 *      Author: Matthew.Rowe
 */

#include "UiThread.h"
#include "os_tasks.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


UiThread::UiThread(uint16_t usStackDepth, UBaseType_t uxPriority, SemaphoreHandle_t& semMainThreadComplete)
     : Thread("Ui Config Thread", usStackDepth, uxPriority),
       _semMainThreadComplete(semMainThreadComplete)
{
}

void UiThread::Run()
{
	printf("Ui thread waiting on MainThread completing...\n");
	//--- Wait until MainThread complete ---
    xSemaphoreTake(_semMainThreadComplete, portMAX_DELAY);
    xSemaphoreGive(_semMainThreadComplete);					//this feels dodgy, any thread that picks up the semaphore gives again for other threads to start.

    printf("Ui thread running\n");

    UserEvents_Task(0);		//runs the code from original NXP version...

    Suspend();
}
