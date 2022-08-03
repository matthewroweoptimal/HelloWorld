/*
 * Threads.cpp
 *
 *  Created on: 15 Feb 2022
 *      Author: Matthew.Rowe
 */

#include "Threads.h"

/* Priorities for application tasks. */

#define MAIN_TASK_PRIORITY          	( tskIDLE_PRIORITY + 1UL )
#define MONITORING_TASK_PRIORITY        ( tskIDLE_PRIORITY + 2UL )
#define TCP_THREAD_PRIORITY				( tskIDLE_PRIORITY + 5UL )
#define CONFIG_THREAD_PRIORITY			( tskIDLE_PRIORITY + 10UL )		// TODO : Review priorities. Higher has bigger priority

#define MONITORING_TIMER_STACKSIZE (configMINIMAL_STACK_SIZE + 100)
#define TCPTHREAD_STACKSIZE 	   (configMINIMAL_STACK_SIZE + 512)
#define CONFIG_THREAD_STACKSIZE	   (configMINIMAL_STACK_SIZE + 512)

#define MONITORING_TIMER_TICK  ( pdMS_TO_TICKS(500))


void Threads::StartThreads()
{
	// heartbeat timer thread
    _secondTimer = new SecondTimer();
    _secondTimer->Start();

    // This main thread only present to get the scheduler going and immediately suspends
    _mainThread = new MainThread(configMINIMAL_STACK_SIZE, MAIN_TASK_PRIORITY);
    _mainThread->Start();

    _monitoring = new MonitoringTimer(MONITORING_TIMER_STACKSIZE, MONITORING_TASK_PRIORITY, MONITORING_TIMER_TICK);
    _monitoring->Start();

    _tcpThread = new TcpThread(TCPTHREAD_STACKSIZE, TCP_THREAD_PRIORITY);
    _tcpThread->Start();

    _configThread = new ConfigThread(CONFIG_THREAD_STACKSIZE, CONFIG_THREAD_PRIORITY);
    _configThread->Start();
}

void Threads::StartScheduler()
{
	printf("Running FreeRTOS. Start scheduler...\n");
	_mainThread->StartScheduler();
}
