/*
 * TcpThread.cpp
 *
 *  Created on: 21 Feb 2022
 *      Author: Matthew.Rowe
 */

#include "GuiThread.h"
#include "os_tasks.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


GuiThread::GuiThread(uint16_t usStackDepth, UBaseType_t uxPriority, SemaphoreHandle_t& semMainThreadComplete)
     : Thread("Gui Config Thread", usStackDepth, uxPriority),
       _semMainThreadComplete(semMainThreadComplete)
{
}

void GuiThread::Run()
{
	printf("Gui thread waiting on MainThread completing...\n");
	//--- Wait until MainThread complete ---
    xSemaphoreTake(_semMainThreadComplete, portMAX_DELAY);
    xSemaphoreGive(_semMainThreadComplete);					//this feels dodgy, any thread that picks up the semaphore gives again for other threads to start.

    printf("Gui thread running\n");

    GUI_Task(0);		//runs the code from original NXP version...

    Suspend();
}
