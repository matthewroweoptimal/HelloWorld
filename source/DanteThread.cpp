/*
 * TcpThread.cpp
 *
 *  Created on: 21 Feb 2022
 *      Author: Matthew.Rowe
 */

#include "DanteThread.h"
#include "os_tasks.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


DanteThread::DanteThread(uint16_t usStackDepth, UBaseType_t uxPriority, SemaphoreHandle_t& semMainThreadComplete)
     : Thread("Dante Config Thread", usStackDepth, uxPriority),
       _semMainThreadComplete(semMainThreadComplete)
{
}

void DanteThread::Run()
{
	printf("Dante thread waiting on MainThread completing...\n");
	//--- Wait until MainThread complete ---
    xSemaphoreTake(_semMainThreadComplete, portMAX_DELAY);
    printf("Dante thread running\n");

    Dante_Task(0);		//runs the code from original NXP version...

    Suspend();
}
