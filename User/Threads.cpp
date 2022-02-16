/*
 * Threads.cpp
 *
 *  Created on: 15 Feb 2022
 *      Author: Matthew.Rowe
 */

#include "threads.h"
#include "queue.hpp"

/* Priorities for the demo application tasks. */
#define mainFLASH_TASK_PRIORITY             ( tskIDLE_PRIORITY + 1UL )
#define mainQUEUE_POLL_PRIORITY             ( tskIDLE_PRIORITY + 2UL )
#define mainSEM_TEST_PRIORITY               ( tskIDLE_PRIORITY + 1UL )
#define mainBLOCK_Q_PRIORITY                ( tskIDLE_PRIORITY + 2UL )
#define mainCREATOR_TASK_PRIORITY           ( tskIDLE_PRIORITY + 3UL )
#define mainFLOP_TASK_PRIORITY              ( tskIDLE_PRIORITY )
#define mainCHECK_TASK_PRIORITY             ( tskIDLE_PRIORITY + 3UL )

#define mainCHECK_TASK_STACK_SIZE           ( configMINIMAL_STACK_SIZE )



void Threads::StartThreads()
{
    secondTimer = new SecondTimer();
    secondTimer->Start();

    mainThread = new MainThread(mainCHECK_TASK_STACK_SIZE, mainCREATOR_TASK_PRIORITY);
    mainThread->Start();
}

void Threads::StartScheduler()
{
	printf("running FreeRTOS. Start scheduler...\n");
	mainThread->StartScheduler();
}



