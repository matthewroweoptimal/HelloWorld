/*
 * Threads.cpp
 *
 *  Created on: 15 Feb 2022
 *      Author: Matthew.Rowe
 */

#include "Threads.h"

/* Priorities for application tasks. */
#define mainCREATOR_TASK_PRIORITY           ( tskIDLE_PRIORITY + 3UL )

void Threads::StartThreads()
{
	// heartbeat timer thread
    secondTimer = new SecondTimer();
    secondTimer->Start();

    // This main thread only present to get the scheduler going and immediately suspends
    mainThread = new MainThread(configMINIMAL_STACK_SIZE, mainCREATOR_TASK_PRIORITY);
    mainThread->Start();
}

void Threads::StartScheduler()
{
	printf("Running FreeRTOS. Start scheduler...\n");
	mainThread->StartScheduler();
}
