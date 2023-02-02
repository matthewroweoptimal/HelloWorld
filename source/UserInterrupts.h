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
#include "spi_sharc.h"
#include "MQX_to_FreeRTOS.h"
#include "board.h"
#include "inputHandler.h"


extern "C" {
#include "uart_ultimo.h"

static uint32_t RTOS_RunTimeCounter; /* runtime counter, used for configGENERATE_RUNTIME_STATS */
int32_t gDebugCount =0;
int32_t gDebugCount1 =0;

void GPH_IRQHandler()
{

    /* To check if SHARC ready interrupt occurred */
    if(GPIO_GET_INT_FLAG(PH, BIT3))
    {
    	//printf("sharc ready int\n");
        GPIO_CLR_INT_FLAG(PH, BIT3);
        if(GPIO_DRV_ReadPinInput(SHARC_SPI_READY))
        {
        		gDebugCount++;
        		_lwevent_set_isr(&sys_event, event_dsp_tx_ready);
        }
        else
        {
        		gDebugCount1++;
        		_lwevent_clear_isr(&sys_event, event_dsp_tx_ready);
        }
    } else
    {
    	//printf("other PH ready int\n");
        PH->INTSRC = PH->INTSRC;
    }

}


void TMR0_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER0) == 1)
    {
        /* Clear Timer0 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER0);
        RTOS_RunTimeCounter++;
    }
}

void SPI1_IRQHandler(void)
{   // SHARC connected on SPI0
    DSPI_DRV_IRQHandler(FSL_SPI_SHARC);
}

void UART1_IRQHandler(void)
{   // ULTIMO connected on UART1
	ULTIMO_UART_IRQHandler();
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


void vApplicationMallocFailedHook(size_t requestedSize)
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
    printf("MallocFailedHook( %d bytes )\n", requestedSize);
#ifndef NDEBUG
    __BKPT(3);    // Stack will be available for examination under debugger
#endif
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
    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */
    printf("StackOverflowHook : TaskHandle %p (%s)\n", pxTask, pcTaskName );
#ifndef NDEBUG
    __BKPT(3);    // Stack will be available for examination under debugger
#endif
    taskDISABLE_INTERRUPTS();
    for( ;; );
}

}

#endif /* USERINTERRUPTS_H_ */
