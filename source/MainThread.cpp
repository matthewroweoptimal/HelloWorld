/*
 * MainThread.cpp
 *
 *  Created on: 17 Aug 2022
 *      Author: Stuart Campbell
 */
#include "MainThread.h"
#include "MQX_To_FreeRTOS.h"
#include "spi_sharc.h"
#include "oly_sharc_boot.h"


MainThread::MainThread(uint16_t usStackDepth, UBaseType_t uxPriority, SemaphoreHandle_t& semMainThreadComplete)
     : Thread("MainThread", usStackDepth, uxPriority),
       _semMainThreadComplete(semMainThreadComplete)
{
    printf("MainThread created\n");  
}

void MainThread::Run()
{
    printf("MainThread HW setup\n");
		
    // Below code needs to happen after the Scheduler is started as it relies on FreeRTOS functionality (delays, semaphores etc)
    // TODO : Possibly can move to MainThread and stop other threads running until completed ?
    
    /* TODO : Bring ultimo & DSP out of reset */
    //GPIO_DRV_SetPinOutput(ULTIMO_FACTORY);
    _time_delay(50);
    //GPIO_DRV_SetPinOutput(SHARC_RESET);
    
    /* Load DSP boot code */
    _time_delay(15);
    DSPI_DRV_MasterTransferBlocking(FSL_SPI_SHARC, OLY_sharc_boot, NULL, OLY_sharc_boot_size, 5000);
    
    /* Set DSP SPI driver to control mode */
    _time_delay(250);
    dsp_sharc_SetMode_Host();
    
    // Signal that other Threads can now start
    xSemaphoreGive(_semMainThreadComplete);
    printf("MainThread Complete\n");
    
    // go to sleep
    Suspend();
}
