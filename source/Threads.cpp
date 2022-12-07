/*
 * Threads.cpp
 *
 *  Created on: 15 Feb 2022
 *      Author: Matthew.Rowe
 */

#include "Threads.h"

void Threads::StartThreads()
{
	// heartbeat timer thread
    _secondTimer = new SecondTimer();
    _secondTimer->Start();

    // This main thread only present to get the scheduler going and immediately suspends
    _semMainThreadComplete = xSemaphoreCreateBinary();
    _mainThread = new MainThread(MAIN_THREAD_STACKSIZE, MAIN_TASK_PRIORITY,_semMainThreadComplete);
    _mainThread->Start();

    _monitoring = new MonitoringTimer(MONITORING_TIMER_STACKSIZE, MONITORING_TASK_PRIORITY, MONITORING_TIMER_TICK);
    _monitoring->Start();

    _tcpThread = new TcpThread(TCPTHREAD_STACKSIZE, TCP_THREAD_PRIORITY,_semMainThreadComplete);
    _tcpThread->Start();

    _danteThread = new DanteThread(DANTETHREAD_STACKSIZE, DANTE_THREAD_PRIORITY,_semMainThreadComplete);
    _danteThread->Start();

    _guiThread = new GuiThread(GUITHREAD_STACKSIZE, GUI_THREAD_PRIORITY,_semMainThreadComplete);
    _guiThread->Start();



}

void Threads::StartScheduler()
{
	printf("Running FreeRTOS. Start scheduler...\n");
	_mainThread->StartScheduler();
}
