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


MetersThread::MetersThread(uint16_t usStackDepth, UBaseType_t uxPriority, SemaphoreHandle_t& semMainThreadComplete)
     : Thread("Meter Thread", usStackDepth, uxPriority),
       _semMainThreadComplete(semMainThreadComplete)
{
}

void MetersThread::Run()
{
	printf("Meters thread waiting on MainThread completing...\n");
	//--- Wait until MainThread complete ---
    xSemaphoreTake(_semMainThreadComplete, portMAX_DELAY);
    xSemaphoreGive(_semMainThreadComplete);					//this feels dodgy, any thread that picks up the semaphore gives again for other threads to start.

    printf("Meters thread running\n");

    Meter_Task(0);		//runs the code from original NXP version...

    Suspend();
}

SysEventsThread::SysEventsThread(uint16_t usStackDepth, UBaseType_t uxPriority, SemaphoreHandle_t& semMainThreadComplete)
     : Thread("Sys Events Thread", usStackDepth, uxPriority),
       _semMainThreadComplete(semMainThreadComplete)
{
}

void SysEventsThread::Run()
{
	printf("Sys Events thread waiting on MainThread completing...\n");
	//--- Wait until MainThread complete ---
    xSemaphoreTake(_semMainThreadComplete, portMAX_DELAY);
    xSemaphoreGive(_semMainThreadComplete);					//this feels dodgy, any thread that picks up the semaphore gives again for other threads to start.

    printf("Sys Events thread running\n");

    SysEvents_Task(0);		//runs the code from original NXP version...

    Suspend();
}


TimerKeeperThread::TimerKeeperThread(uint16_t usStackDepth, UBaseType_t uxPriority, SemaphoreHandle_t& semMainThreadComplete)
     : Thread("Timer Keeper Thread", usStackDepth, uxPriority),
       _semMainThreadComplete(semMainThreadComplete)
{
}

void TimerKeeperThread::Run()
{
	printf("Timer Keeper thread waiting on MainThread completing...\n");
	//--- Wait until MainThread complete ---
    xSemaphoreTake(_semMainThreadComplete, portMAX_DELAY);
    xSemaphoreGive(_semMainThreadComplete);					//this feels dodgy, any thread that picks up the semaphore gives again for other threads to start.

    printf("Timer Keeper thread running\n");

    TimeKeeper_Task(0);		//runs the code from original NXP version...

    Suspend();
}

SpeakerConfigThread::SpeakerConfigThread(uint16_t usStackDepth, UBaseType_t uxPriority, SemaphoreHandle_t& semMainThreadComplete)
     : Thread("Speaker Config Thread", usStackDepth, uxPriority),
       _semMainThreadComplete(semMainThreadComplete)
{
}

void SpeakerConfigThread::Run()
{
	printf("Speaker Config thread waiting on MainThread completing...\n");
	//--- Wait until MainThread complete ---
    xSemaphoreTake(_semMainThreadComplete, portMAX_DELAY);
    xSemaphoreGive(_semMainThreadComplete);					//this feels dodgy, any thread that picks up the semaphore gives again for other threads to start.

    printf("Speaker Config thread running\n");

    SpkrCnfg_Task(0);		//runs the code from original NXP version...

    Suspend();
}
