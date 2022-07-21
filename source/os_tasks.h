/*
 * os_tasks.h
 *
 *  Created on: Sep 17, 2015
 *      Author: Kris.Simonsen
 */

#ifndef __os_tasks_H
#define __os_tasks_H

#ifdef __cplusplus
extern "C" {
#endif

#include "MQX_To_FreeRTOS.h"
#include "timer.h"


#define TASK_MAIN		  1
#define TASK_CONFIG 	  2
#define TASK_GUI		  3
#define TASK_DANTE		  4
#define TASK_SYSEVENT 	  6
#define TASK_USER   	  7
#define TASK_TIMER		  8
#define TASK_DISCO		  9
#define TASK_SPKR_CNFG	  10
#define TASK_METER	  	  11
#define TASK_AUX_SPI      12
#define TASK_IRDA_PROCESS 16
#define TASK_IRDA_TX_A    18
#define TASK_IRDA_TX_B    19
#define TASK_IRDA_TX_C    20
#define TASK_IRDA_TX_D    21
#define	TASK_NETWORK	  22		// added for the TCP task
#define	TASK_SPISHARC	  23		// added for the TCP task
#define TASK_FENCE		  24		// added the last one

#define TASK_CONFIG_PRIORITY		(7+10)
#define TASK_GUI_PRIORITY			(7+15)
#define TASK_DANTE_PRIORITY			(7+14)
#define TASK_SYSEVENT_PRIORITY		(7+11)
#define TASK_USER_PRIORITY			(7+16)
#define TASK_TIMER_PRIORITY			(7+20)	/* This task should have the lowest priority */
#define TASK_DISCO_PRIORITY			(7+16)
#define TASK_SPKR_CNFG_PRIORITY		(7+14)
#define TASK_METER_PRIORITY			(7+19)
#define TASK_AUX_SPI_PRIORITY       (7+19)
#define TASK_IRDA_PROCESS_PRIORITY	(7+14)
#define TASK_IRDA_TX_PRIORITY		(7+13)

#define SCREEN_ORIENTATION_POLL		(3000)		// in ms

void Config_Task(uint32_t task_init_data);
void SysEvents_Task(uint32_t task_init_data);
void GUI_Task(uint32_t task_init_data);
void UartIrda_A_Task(uint32_t task_init_data);
void UartIrda_B_Task(uint32_t task_init_data);
void UartIrda_C_Task(uint32_t task_init_data);
void UartIrda_D_Task(uint32_t task_init_data);
void Dante_Task(uint32_t task_init_data);
void UserEvents_Task(uint32_t task_init_data);
void TimeKeeper_Task(uint32_t task_init_data);
void Discovery_Task(uint32_t task_init_data);
void SpkrCnfg_Task(uint32_t task_init_data);
void Meter_Task(uint32_t task_init_data);
void AUX_spi_Task(uint32_t task_init_data);
void IRDA_Transmit_A_Task(uint32_t task_init_data);
void IRDA_Transmit_B_Task(uint32_t task_init_data);
void IRDA_Transmit_C_Task(uint32_t task_init_data);
void IRDA_Transmit_D_Task(uint32_t task_init_data);

#ifdef DEBUG
#define USE_MSGQ_MUTEX
#define USE_MEMPOOL_MONITOR
#endif


#ifdef USE_MSGQ_MUTEX

extern int	MsgQ_Init();
extern int	MsgQ_Get_Count(_queue_id config_qid);
extern void* Msg_Alloc(_pool_id message_pool);
extern void Msg_Free(void* pMsg);
extern bool MsgQ_Send(void* pMsg);

#define  MSGQ_INIT() 				MsgQ_Init();
#define  MSGQ_GET_COUNT(config_qid) MsgQ_Get_Count(config_qid)
#define  MSG_ALLOC(message_pool) 	Msg_Alloc(message_pool)
#define  MSG_FREE(pMsg) 			Msg_Free((void*) pMsg)
#define  MSGQ_SEND(pMsg)			MsgQ_Send((void*) pMsg)


#else
#define MSGQ_INIT()
#define MSGQ_GET_COUNT(config_qid)    _msgq_get_count(config_qid)
#define MSG_ALLOC(message_pool) 	 _msg_alloc(message_pool)
#define MSG_FREE(pMsg)				 _msg_free(pMsg)
#define MSGQ_SEND(pMsg)				 _msgq_send(pMsg)

#endif

//#define USE_TASK_DEBUG

#ifdef USE_TASK_DEBUG
typedef struct {
	int nId;
	MQX_TICK_STRUCT startTicks;
	MQX_TICK_STRUCT endTicks;
	int32_t nDiffTicks;
	bool	bInTask;
	unsigned int 	nCount;
	int		nTaskPos;
} tTaskDebug;

extern void TaskDebug_Init();
extern void TaskDebug_TimeIn(int nId);
extern void TaskDebug_TimeOut(int nId);
extern void TaskDebug_SetPos(int nId, int nPos);

#define TASKDEBUG_INIT()  TaskDebug_Init();
#define TASKDEBUG_IN(id)  TaskDebug_TimeIn(id);
#define TASKDEBUG_OUT(id)  TaskDebug_TimeOut(id);
#define TASKDEBUG_POS(id,pos)  TaskDebug_SetPos(id,pos);
#else
#define TASKDEBUG_INIT()
#define TASKDEBUG_IN(id)
#define TASKDEBUG_OUT(id)
#define TASKDEBUG_POS(id,pos)
#endif

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* __os_tasks_H */
