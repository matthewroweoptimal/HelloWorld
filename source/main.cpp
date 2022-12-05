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
#include "board.h"
#include "uart_ultimo.h"
#include "flash_params.h"

using namespace std;

//Table of IRQ handler priorities - collected in one place to allow system irq priority analysis
//The uart handlers run quite fast (8usec?) so effect on ethernet should be minimal
//Ultimo is most critical and given the highest priority
//default appears to be 0XC0, which is level 12
//These are priorites as used by NVIC_SetPriority -- which must be an even number greater than 2
//The NVIC_SetPriority level allowed are: 4,6,8,10,12,14 -> corresponding to MQX priority 2,3,4,5,6,7
#define SPI1_HANDLER_PRIORITY 	12U   // master so can be delayed
#define SPI2_HANDLER_PRIORITY 	12U   // master so can be delayed
#define I2C0_HANDLER_PRIORITY 	12U   // master so can be delayed
#define UART0_HANDLER_PRIORITY 	6U    //voicing  115k -not used by customer, so can keep low for test
//#define UART1_HANDLER_PRIORITY 	6U    //irda 14.4k
#define UART1_HANDLER_PRIORITY 	4U    //ultimo  115k  - this needs service within 86usec
//#define UART4_HANDLER_PRIORITY 	6U    //irda 14.4k
//#define UART5_HANDLER_PRIORITY 	6U    //irda 14.4k
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
    CLK_SetCoreClock(FREQ_160MHZ);

    /* Set both PCLK0 and PCLK1 as HCLK/2 */
    CLK->PCLKDIV = CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2;

    // SPI1_CONFIGURATION - This is  done in the DSPI_DRV_MasterInit
    /* >>>>>>> Select PCLK0 as the clock source of SPI1 */
    //CLK_SetModuleClock(SPI1_MODULE, CLK_CLKSEL2_SPI1SEL_PCLK0, MODULE_NoMsk);
    /* >>>>>>>>> Enable SPI1 peripheral clock */
    //CLK_EnableModuleClock(SPI1_MODULE);


    /* Enable IP clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_EnableModuleClock(EMAC_MODULE);
    /* Enable I2C0 peripheral clock */
    CLK_EnableModuleClock(I2C2_MODULE);
    CLK_EnableModuleClock(UART1_MODULE);
    CLK_EnableModuleClock(SPIM_MODULE);


    /* Select IP clock source */
    /*TODO these are actually activated in the individual init functions */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HXT, CLK_CLKDIV0_UART0(1));
    CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL1_UART1SEL_HXT, CLK_CLKDIV0_UART1(1));

    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HXT, 0);
    // Configure MDC clock rate to HCLK / (127 + 1) = 1.5 MHz if system is running at 192 MHz
    CLK_SetModuleClock(EMAC_MODULE, 0, CLK_CLKDIV3_EMAC(127));


    // SPI1_CONFIGURATION
    /* Configure SPI1 related multi-function pins. GPH[7:4] : SPI1_MISO, SPI1_MOSI, SPI1_CLK, SPI1_SS. */
    SYS->GPH_MFPL &= ~(SYS_GPH_MFPL_PH4MFP_Msk | SYS_GPH_MFPL_PH5MFP_Msk | SYS_GPH_MFPL_PH6MFP_Msk | SYS_GPH_MFPL_PH7MFP_Msk);
    SYS->GPH_MFPL |= (SYS_GPH_MFPL_PH4MFP_SPI1_MISO | SYS_GPH_MFPL_PH5MFP_SPI1_MOSI | SYS_GPH_MFPL_PH6MFP_SPI1_CLK | SYS_GPH_MFPL_PH7MFP_SPI1_SS);

    /* Enable SPI1 clock pin (PH6) schmitt trigger */
    PH->SMTEN |= GPIO_SMTEN_SMTEN6_Msk;
    /* Enable SPI1 I/O high slew rate (0xF0 for PH4-7) what about other PH pins?*/
    GPIO_SetSlewCtl(PH, 0xF0, GPIO_SLEWCTL_HIGH);

    /*enable some output GPIO pins for resets of DAC, ADC, SHARC, AMPS and the happy leds */
    GPIO_SetMode(PH, BIT0|BIT1|BIT2, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PE, BIT7, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PD, BIT3, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PG, BIT0|BIT1|BIT2|BIT3, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PF, BIT5|BIT6, GPIO_MODE_OUTPUT);

    //Set the inputs up
    GPIO_SetMode(PF, BIT7|BIT8, GPIO_MODE_QUASI);
    //GPIO_SetMode(PB, BIT0|BIT1, GPIO_MODE_QUASI);
    GPIO_SetMode(PF, BIT9|BIT10|BIT11, GPIO_MODE_QUASI);
    GPIO_SetMode(PG, BIT4, GPIO_MODE_QUASI);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    /* Enable SPIM module clock */


    /* Set GPB multi-function pins for UART0 RXD (PB.12) and TXD (PB.13) */
    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk);
    SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    /*LEDs (PH.0 & PH.1) */
    PH->MODE = (PH->MODE & ~(GPIO_MODE_MODE0_Msk | GPIO_MODE_MODE1_Msk)) |
               (GPIO_MODE_OUTPUT << GPIO_MODE_MODE0_Pos) |
               (GPIO_MODE_OUTPUT << GPIO_MODE_MODE1_Pos);  // Set to output mode

    /* Set GPB multi-function pins for UART1 TXD (PB.7), RXD (PB.6), CTS (PB.9) and RTS (PB.8)*/
    SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB6MFP_Msk | SYS_GPB_MFPL_PB7MFP_Msk);
    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB8MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk);

    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB6MFP_UART1_RXD | SYS_GPB_MFPL_PB7MFP_UART1_TXD);
    SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB8MFP_UART1_nRTS | SYS_GPB_MFPH_PB9MFP_UART1_nCTS);


    /* Set I2C0 multi-function pins */
    SYS->GPD_MFPL = (SYS->GPD_MFPL & ~(SYS_GPD_MFPL_PD0MFP_Msk | SYS_GPD_MFPL_PD1MFP_Msk)) |
                    (SYS_GPD_MFPL_PD0MFP_I2C2_SDA | SYS_GPD_MFPL_PD1MFP_I2C2_SCL);

    /* I2C pin enable schmitt trigger */
    PD->SMTEN |= GPIO_SMTEN_SMTEN0_Msk | GPIO_SMTEN_SMTEN1_Msk;

    /* Set PB.0 ~ PB.1 to input mode */
    PB->MODE &= ~(GPIO_MODE_MODE0_Msk | GPIO_MODE_MODE1_Msk);

    /* Configure the GPB0 ADC analog input pins.  */
    SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_EADC0_CH0 | SYS_GPB_MFPL_PB1MFP_EADC0_CH1);

    /* Disable the GPB0 and GPB12 digital input path to avoid the leakage current. */
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT1|BIT0);

    /*DISPLAY PINS
     * 	#define DISPLAY_BUT_IN				PB5

     *	#define DISPLAY_LED1  				PB3
     *	#define DISPLAY_LED2  				PB2
     *	#define DISPLAY_LED3  				PC12
     *	#define DISPLAY_LED4  				PC11
     *	#define DISPLAY_LED5  				PC10
     */

    /* Enable SPIM module clock */
    CLK_EnableModuleClock(SPIM_MODULE);

    /* Init SPIM multi-function pins, MOSI(PC.0), MISO(PC.1), CLK(PC.2), SS(PC.3), D3(PC.4), and D2(PC.5) */
    SYS->GPC_MFPL &= ~(SYS_GPC_MFPL_PC0MFP_Msk | SYS_GPC_MFPL_PC1MFP_Msk | SYS_GPC_MFPL_PC2MFP_Msk |
                       SYS_GPC_MFPL_PC3MFP_Msk | SYS_GPC_MFPL_PC4MFP_Msk | SYS_GPC_MFPL_PC5MFP_Msk);
    SYS->GPC_MFPL |= SYS_GPC_MFPL_PC0MFP_SPIM_MOSI | SYS_GPC_MFPL_PC1MFP_SPIM_MISO |
                     SYS_GPC_MFPL_PC2MFP_SPIM_CLK | SYS_GPC_MFPL_PC3MFP_SPIM_SS |
                     SYS_GPC_MFPL_PC4MFP_SPIM_D3 | SYS_GPC_MFPL_PC5MFP_SPIM_D2;
    PC->SMTEN |= GPIO_SMTEN_SMTEN2_Msk;

    /* Set SPIM I/O pins as high slew rate up to 80 MHz. */
    PC->SLEWCTL = (PC->SLEWCTL & 0xFFFFF000) |
                  (0x1<<GPIO_SLEWCTL_HSREN0_Pos) | (0x1<<GPIO_SLEWCTL_HSREN1_Pos) |
                  (0x1<<GPIO_SLEWCTL_HSREN2_Pos) | (0x1<<GPIO_SLEWCTL_HSREN3_Pos) |
                  (0x1<<GPIO_SLEWCTL_HSREN4_Pos) | (0x1<<GPIO_SLEWCTL_HSREN5_Pos);



    GPIO_SetMode(PB, BIT2|BIT3, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PB, BIT5, GPIO_MODE_QUASI);
    GPIO_SetMode(PC, BIT10|BIT11|BIT12, GPIO_MODE_OUTPUT);		//
    GPIO_SetMode(PH, BIT2, GPIO_MODE_OUTPUT);    				//sharc reset

    Gpio::setGpio(SHARC_RESET,LOW);
    Gpio::setGpio(ULTIMO_RESET,LOW);
    Gpio::setGpio(ADC_RESET,LOW);
    Gpio::setGpio(DAC_RESET,LOW);
    Gpio::setGpio(AMP1_MUTE,HIGH);
    Gpio::setGpio(AMP2_MUTE,HIGH);
    Gpio::setGpio(AMP_STANDBY,HIGH);
    Gpio::setGpio(AMP_BTL4R_CTL,HIGH);
    Gpio::setGpio(AMP1_TEMP_SEL,HIGH);
    Gpio::setGpio(AMP2_TEMP_SEL,HIGH);

    initEthernetHardware();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    /* some SPIM init */
    SPIM_SET_CLOCK_DIVIDER(1);        /* Set SPIM clock as HCLK divided by 2 */

    SPIM_SET_RXCLKDLY_RDDLYSEL(0);    /* Insert 0 delay cycle. Adjust the sampling clock of received data to latch the correct data. */
    SPIM_SET_RXCLKDLY_RDEDGE();       /* Use SPI input clock rising edge to sample received data. */

    SPIM_SET_DCNUM(8);                /* Set 8 dummy cycle. */


}
/*-----------------------------------------------------------*/



static void peripherals_init(void)
{
	/* DSP SPI1 boot config */
	NVIC_SetPriority(SPI1_IRQn, SPI1_HANDLER_PRIORITY);
//	OSA_InstallIntHandler(SPI0_IRQn, spi_sharc_IRQHandler);
	DSPI_DRV_MasterInit(FSL_SPI_SHARC, &spi_sharc_MasterState, &spi_sharc_MasterConfig_boot, &spi_sharc_bootConfig);

	//Init I2C for ADC - Does it need any init or do we just do the setup?

	/* Initialize Flash */
	flash_init();

	/* TODO : Accelerometer I2C NOT USED BY NUCDDL*/

	/* TODO : Voice UART NOT USED BY NUCDDL*/

    /* TODO : Ultimo UART */
    ULTIMO_UART_Init();

    /* TODO : FTM2 Encoder */

    /* TODO : FTM3 LCD Backlight */

    /* TODO : Init ADC for Amp Monitoring */
}
