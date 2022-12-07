/*
 * Threads.h
 *
 *  Created on: 15 Feb 2022
 *      Author: Matthew.Rowe
 */

#ifndef THREADS_H_
#define THREADS_H_

#include "MainThread.h"
#include "SecondTimer.h"
#include "MonitoringTimer.h"
#include "TcpThread.h"
#include "DanteThread.h"
#include "GuiThread.h"
#include "lwipopts.h"	// For the LWIP TCP thread priority
// Defined in lwipopts.h :
//		#define TCPIP_THREAD_PRIO	            8
//		#define TCPIP_THREAD_STACKSIZE	        1024

/* Priorities for application tasks. */

#define MAIN_TASK_PRIORITY          	( tskIDLE_PRIORITY + 1UL )
#define MONITORING_TASK_PRIORITY        ( tskIDLE_PRIORITY + 2UL )
#define TCP_THREAD_PRIORITY				( TCPIP_THREAD_PRIO - 2 )
#define TCP_MANDOLIN_THREAD_PRIORITY	( TCPIP_THREAD_PRIO - 1 )
#define DANTE_THREAD_PRIORITY	( TCPIP_THREAD_PRIO - 4 )
#define GUI_THREAD_PRIORITY	( TCPIP_THREAD_PRIO - 3 )

#define MAIN_THREAD_STACKSIZE      (configMINIMAL_STACK_SIZE + 256)
#define MONITORING_TIMER_STACKSIZE (configMINIMAL_STACK_SIZE + 100)
#define TCPTHREAD_STACKSIZE 	   (1664)		// FreeRTOS stack size for TCP CONFIG thread - seen 1518 HWM
#define TCP_MANDOLIN_STACK_SIZE    (1664)    	// FreeRTOS stack size for MANDOLIN thread - seen 1580 HWM
#define DANTETHREAD_STACKSIZE    (1664)    	// FreeRTOS stack size for DANTE thread - seen ???? HWM
#define GUITHREAD_STACKSIZE    (1664)    	// FreeRTOS stack size for GUI thread - seen ???? HWM

#define MONITORING_TIMER_TICK  ( pdMS_TO_TICKS(500))


class Threads
{
public:
    void StartThreads();
    void StartScheduler();

private:
    /* Threads */
    MainThread 		*_mainThread;
    MonitoringTimer *_monitoring;
    TcpThread 		*_tcpThread;	// Effectively the 'CDDLive' Config Task.
    								// This receives messages posted by 'tcp_mandolin_thread' onto it's message Q (CONFIG_QUEUE)
    DanteThread 	*_danteThread; 	// This is Dante_Task
    GuiThread 		*_guiThread; 	// This is Gui_Task
    /* Timers */
    SecondTimer 	*_secondTimer;

    SemaphoreHandle_t _semMainThreadComplete;   // Signal that mainThread has completed it's setup work
};

#endif /* THREADS_H_ */
