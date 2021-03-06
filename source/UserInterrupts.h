/*
 * Userinterrupts.h
 *
 *  Created on: 18 Feb 2022
 *      Author: Matthew.Rowe
 */

#ifndef USERINTERRUPTS_H_
#define USERINTERRUPTS_H_

#include "task.h"
#include "timer.h"

extern "C" {

static uint32_t RTOS_RunTimeCounter; /* runtime counter, used for configGENERATE_RUNTIME_STATS */

void TMR0_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER0) == 1)
    {
        /* Clear Timer0 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER0);
        RTOS_RunTimeCounter++;
    }
}

void RTOS_AppConfigureTimerForRuntimeStats(void) {
  RTOS_RunTimeCounter = 0;

  // initialise microsecond timer
  TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1000000);
  TIMER_EnableInt(TIMER0);
  NVIC_EnableIRQ(TMR0_IRQn);
  TIMER_Start(TIMER0);
}

uint32_t RTOS_AppGetRuntimeCounterValueFromISR(void) {
  return RTOS_RunTimeCounter;
}


void vApplicationMallocFailedHook( void )
{
    /* vApplicationMallocFailedHook() will only be called if
    configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
    function that will get called if a call to pvPortMalloc() fails.
    pvPortMalloc() is called internally by the kernel whenever a task, queue,
    timer or semaphore is created.  It is also called by various parts of the
    demo application.  If heap_1.c or heap_2.c are used, then the size of the
    heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
    FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
    to query the size of free heap space that remains (although it does not
    provide information on how the remaining heap might be fragmented). */
    taskDISABLE_INTERRUPTS();
    for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
    /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
    to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
    task.  It is essential that code added to this hook function never attempts
    to block in any way (for example, call xQueueReceive() with a block time
    specified, or call vTaskDelay()).  If the application makes use of the
    vTaskDelete() API function (as this demo application does) then it is also
    important that vApplicationIdleHook() is permitted to return to its calling
    function, because it is the responsibility of the idle task to clean up
    memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/


void vApplicationStackOverflowHook( TaskHandle_t pxTask, signed char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */
    taskDISABLE_INTERRUPTS();
    for( ;; );
}

}

#endif /* USERINTERRUPTS_H_ */
