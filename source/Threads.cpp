/*
 * Threads.cpp
 *
 *  Created on: 15 Feb 2022
 *      Author: Matthew.Rowe
 */

#include "Threads.h"

/* Priorities for application tasks. */
#define MONITORING_TASK_PRIORITY        ( tskIDLE_PRIORITY + 2UL )
#define MAIN_TASK_PRIORITY          	( tskIDLE_PRIORITY + 3UL )

#define MONITORING_TIMER_STACKSIZE (configMINIMAL_STACK_SIZE + 100)

#define MONITORING_TIMER_TICK  ( pdMS_TO_TICKS(500))


void Threads::StartThreads()
{
	// heartbeat timer thread
    secondTimer = new SecondTimer();
    secondTimer->Start();

    // This main thread only present to get the scheduler going and immediately suspends
    mainThread = new MainThread(configMINIMAL_STACK_SIZE, MAIN_TASK_PRIORITY);
    mainThread->Start();

    monitoring = new MonitoringTimer(MONITORING_TIMER_STACKSIZE, MONITORING_TASK_PRIORITY, MONITORING_TIMER_TICK);
    monitoring->Start();
}

void Threads::StartScheduler()
{
	printf("Running FreeRTOS. Start scheduler...\n");
	mainThread->StartScheduler();
}
