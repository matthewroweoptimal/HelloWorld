/*
 * FreeRTOS Kernel V10.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#include <stdio.h>
/* Kernel includes. */
#include "FreeRTOS.h"
#include "UserInterrupts.h"

/* Hardware and starter kit includes. */
#include "NuMicro.h"
#include "Threads.h"
#include "InputHandler.h"
#include "TimeKeeper.h"
#include "spi_sharc.h"

using namespace std;

//Table of IRQ handler priorities - collected in one place to allow system irq priority analysis
//The uart handlers run quite fast (8usec?) so effect on ethernet should be minimal
//Ultimo is most critical and given the highest priority
//default appears to be 0XC0, which is level 12
//These are priorites as used by NVIC_SetPriority -- which must be an even number greater than 2
//The NVIC_SetPriority level allowed are: 4,6,8,10,12,14 -> corresponding to MQX priority 2,3,4,5,6,7
#define SPI0_HANDLER_PRIORITY 	12U   // master so can be delayed
#define SPI2_HANDLER_PRIORITY 	12U   // master so can be delayed
#define I2C0_HANDLER_PRIORITY 	12U   // master so can be delayed
#define UART0_HANDLER_PRIORITY 	6U    //voicing  115k -not used by customer, so can keep low for test
#define UART1_HANDLER_PRIORITY 	6U    //irda 14.4k
#define UART3_HANDLER_PRIORITY 	4U    //ultimo  115k  - this needs service within 86usec
#define UART4_HANDLER_PRIORITY 	6U    //irda 14.4k
#define UART5_HANDLER_PRIORITY 	6U    //irda 14.4k
#define FTM2_HANDLER_PRIORITY 	12U
#define FTM3_HANDLER_PRIORITY 	12U
#define ETHERNET_HANDLER_PRIORITY 8U  //transfer this manually to ethernet driver

/*-----------------------------------------------------------*/
/*
 * Set up the hardware ready to run this demo.
 */
static void prvSetupHardware( void );
static void peripherals_init(void);
/*-----------------------------------------------------------*/

// Map new / delete operators to use the FreeRTOS heap.
void * operator new( size_t size )
{
//	printf( "new( %d )\n", size );
    return pvPortMalloc(size);
}

void * operator new[]( size_t size )
{
	printf( "new[]( %d )\n", size );
    return pvPortMalloc( size );
}

void operator delete( void * ptr )
{
    vPortFree( ptr );
}

void operator delete[]( void * ptr )
{
    vPortFree( ptr );
}

int main(void)
{
    /* Configure the hardware */
    prvSetupHardware();
    peripherals_init();

	 /* Create OS Events */
	_lwevent_create(&sys_event,0);
	_lwevent_create(&user_event,0);
	_lwevent_create(&timer_event,0);
//	_lwevent_create(&aux_spi_event,0);
//	_lwevent_create(&irda_lwevent, 0);

    printf("\nCreating Threads\n");
    Threads *threads = new Threads();
    threads->StartThreads();
    threads->StartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for( ;; );
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Set XT1_OUT(PF.2) and XT1_IN(PF.3) to input mode */
    PF->MODE &= ~(GPIO_MODE_MODE2_Msk | GPIO_MODE_MODE3_Msk);

    /* Enable External XTAL (4~24 MHz) */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);

    /* Waiting for 12MHz clock ready */
    CLK_WaitClockReady( CLK_STATUS_HXTSTB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(FREQ_192MHZ);

    /* Set both PCLK0 and PCLK1 as HCLK/2 */
    CLK->PCLKDIV = CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2;

#if 1 // SPI0_CONFIGURATION    
    /* >>>>>>> Select PCLK0 as the clock source of SPI0 */
    CLK_SetModuleClock(SPI0_MODULE, CLK_CLKSEL2_SPI0SEL_PCLK1, MODULE_NoMsk); 
    /* >>>>>>>>> Enable SPI0 peripheral clock */
    CLK_EnableModuleClock(SPI0_MODULE);    
#endif   

    /* Enable IP clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_EnableModuleClock(EMAC_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HXT, CLK_CLKDIV0_UART0(1));
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HXT, 0);
    // Configure MDC clock rate to HCLK / (127 + 1) = 1.5 MHz if system is running at 192 MHz
    CLK_SetModuleClock(EMAC_MODULE, 0, CLK_CLKDIV3_EMAC(127));

#if 1 // SPI0_CONFIGURATION     
    /* >>>>>>>>> Setup SPI0 multi-function pins */
    SYS->GPA_MFPL |= SYS_GPA_MFPL_PA0MFP_SPI0_MOSI | SYS_GPA_MFPL_PA1MFP_SPI0_MISO | SYS_GPA_MFPL_PA2MFP_SPI0_CLK | SYS_GPA_MFPL_PA3MFP_SPI0_SS;

    /* >>>>>>>>> Enable SPI0 clock pin (PA2) schmitt trigger */
    PA->SMTEN |= GPIO_SMTEN_SMTEN2_Msk;

    /* >>>>>>>>> Enable SPI0 I/O high slew rate */
    GPIO_SetSlewCtl(PA, 0xF, GPIO_SLEWCTL_HIGH);
#endif
        
    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();


    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk);
    SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    PH->MODE = (PH->MODE & ~(GPIO_MODE_MODE0_Msk | GPIO_MODE_MODE1_Msk | GPIO_MODE_MODE2_Msk)) |
               (GPIO_MODE_OUTPUT << GPIO_MODE_MODE0_Pos) |
               (GPIO_MODE_OUTPUT << GPIO_MODE_MODE1_Pos) |
               (GPIO_MODE_OUTPUT << GPIO_MODE_MODE2_Pos);  // Set to output mode

    initEthernetHardware();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);
}
/*-----------------------------------------------------------*/

static void peripherals_init(void)
{
	/* DSP SPI0 boot config */
	NVIC_SetPriority(SPI0_IRQn, SPI0_HANDLER_PRIORITY);
//	OSA_InstallIntHandler(SPI0_IRQn, spi_sharc_IRQHandler);
	DSPI_DRV_MasterInit(FSL_SPI_SHARC, &spi_sharc_MasterState, &spi_sharc_MasterConfig_boot, &spi_sharc_bootConfig);

	/* TODO : DSP SPI2 boot config */

	/* TODO : Accelerometer I2C */

	/* TODO : Voice UART */

    /* TODO : Ultimo UART */

    /* TODO : FTM2 Encoder */

    /* TODO : FTM3 LCD Backlight */

    /* TODO : Init ADC for Amp Monitoring */
}
