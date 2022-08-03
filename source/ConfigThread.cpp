/*
 * ConfigThread.cpp
 *
 *  Created on: 1 Aug 2022
 *      Author: Stuart Campbell
 */

#include "ConfigThread.h"
#include "os_tasks.h"



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CONFIG_THREAD_PRIO    ( tskIDLE_PRIORITY + 2UL )
#define CONFIG_THREAD_STACKSIZE  300
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/



ConfigThread::ConfigThread(uint16_t usStackDepth, UBaseType_t uxPriority) : Thread("Config Thread", usStackDepth, uxPriority)
{
}

void ConfigThread::Run()
{
	printf("Config thread startup\n");

	Config_Task( 0 );	// In CDD code os_tasks.cpp
}
