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
#include "lwipopts.h"	// For the LWIP TCP thread priority
// Defined in lwipopts.h :
//		#define TCPIP_THREAD_PRIO	            8
//		#define TCPIP_THREAD_STACKSIZE	        1024

/* Priorities for application tasks. */

#define MAIN_TASK_PRIORITY          	( tskIDLE_PRIORITY + 1UL )
#define MONITORING_TASK_PRIORITY        ( tskIDLE_PRIORITY + 2UL )
#define TCP_THREAD_PRIORITY				( TCPIP_THREAD_PRIO - 2 )
//#define CONFIG_THREAD_PRIORITY			( tskIDLE_PRIORITY + 10UL )		// TODO : Review priorities. Higher has bigger priority
#define TCP_MANDOLIN_THREAD_PRIORITY	( TCPIP_THREAD_PRIO - 1 )

#define MONITORING_TIMER_STACKSIZE (configMINIMAL_STACK_SIZE + 100)
#define TCPTHREAD_STACKSIZE 	   (configMINIMAL_STACK_SIZE + 1536)
//#define CONFIG_THREAD_STACKSIZE	   (configMINIMAL_STACK_SIZE + 512)
#define TCP_MANDOLIN_STACK_SIZE    (configMINIMAL_STACK_SIZE + 1536)    	// FreeRTOS stack size for Mandolin TCP thread



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

    /* Timers */
    SecondTimer 	*_secondTimer;

};

#endif /* THREADS_H_ */
