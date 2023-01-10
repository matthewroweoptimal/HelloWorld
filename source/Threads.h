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
#include "UiThread.h"
#include "lwipopts.h"	// For the LWIP TCP thread priority
// Defined in lwipopts.h :
//		#define TCPIP_THREAD_PRIO	            8
//		#define TCPIP_THREAD_STACKSIZE	        1024

/* Priorities for application tasks.
 * From original
 * #define TASK_CONFIG_PRIORITY		    (7+10)
 * #define TASK_GUI_PRIORITY			(7+15)
 * #define TASK_DANTE_PRIORITY			(7+14)
 * #define TASK_SYSEVENT_PRIORITY		(7+11)
 * #define TASK_USER_PRIORITY			(7+16)
 * #define TASK_TIMER_PRIORITY			(7+20)	This task should have the lowest priority
 * #define TASK_DISCO_PRIORITY			(7+16)
 * #define TASK_SPKR_CNFG_PRIORITY		(7+14)
 * #define TASK_METER_PRIORITY			(7+19)
 * #define TASK_AUX_SPI_PRIORITY        (7+19)
 * #define TASK_IRDA_PROCESS_PRIORITY	(7+14)
 * #define TASK_IRDA_TX_PRIORITY		(7+13)

 * */

#define MAIN_TASK_PRIORITY          	( tskIDLE_PRIORITY + 1UL )		//1
#define MONITORING_TASK_PRIORITY        ( tskIDLE_PRIORITY + 2UL )		//2
#define TCP_THREAD_PRIORITY				( TCPIP_THREAD_PRIO - 2 )		//6
#define TCP_MANDOLIN_THREAD_PRIORITY	( TCPIP_THREAD_PRIO - 1 )		//7
#define DANTE_THREAD_PRIORITY	( TCPIP_THREAD_PRIO - 4 )				//4
#define GUI_THREAD_PRIORITY	( TCPIP_THREAD_PRIO - 5 )					//5
#define UI_THREAD_PRIORITY	( TCPIP_THREAD_PRIO - 6 )					//3
#define METERS_THREAD_PRIORITY	( TCPIP_THREAD_PRIO - 6 )					//3


#define MAIN_THREAD_STACKSIZE      	(configMINIMAL_STACK_SIZE + 256)
#define MONITORING_TIMER_STACKSIZE 	(configMINIMAL_STACK_SIZE + 100)
#define TCPTHREAD_STACKSIZE 	   	(configMINIMAL_STACK_SIZE + 256)		// IQ - HWM 844 bytes so only 211 words - STuart-> FreeRTOS stack size for TCP CONFIG thread - seen 1518 HWM
#define TCP_MANDOLIN_STACK_SIZE    	(configMINIMAL_STACK_SIZE + 256)    	// IQ - This thread stack size is specified differently and in bytes - STuart-> FreeRTOS stack size for MANDOLIN thread - seen 1580 HWM
#define DANTETHREAD_STACKSIZE    	(configMINIMAL_STACK_SIZE + 256)    	// FreeRTOS stack size for DANTE thread - seen ???? HWM
#define GUITHREAD_STACKSIZE    		(configMINIMAL_STACK_SIZE + 256)    	// FreeRTOS stack size for GUI thread - seen ???? HWM
#define UITHREAD_STACKSIZE    		(configMINIMAL_STACK_SIZE + 256)    	// FreeRTOS stack size for GUI thread - seen ???? HWM
#define METERSTHREAD_STACKSIZE  	(configMINIMAL_STACK_SIZE + 256)    	// FreeRTOS stack size for GUI thread - seen ???? HWM


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
    UiThread 		*_uiThread; 	// This is ui_Task
    MetersThread 	*_metersThread; 	// This is Meter_Task

    /* Timers */
    SecondTimer 	*_secondTimer;

    SemaphoreHandle_t _semMainThreadComplete;   // Signal that mainThread has completed it's setup work
};

#endif /* THREADS_H_ */
