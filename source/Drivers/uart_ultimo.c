
/* MODULE uart_ultimo. */

#include "uart_ultimo.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t g_u8RecData[ULT_RX_BUF_SIZE]  = {0};
volatile uint32_t g_u32comRbytes 	= 0;
volatile uint32_t g_u32comRhead  	= 0;
volatile uint32_t g_u32comRtail  	= 0;
volatile int32_t  g_bWait         	= 0;
volatile int32_t  g_intCounter	 	= 0;

//initialisation of the ultimo UART

void ULTIMO_UART_Init()
{
    /* Configure UART1 and set UART1 baud rate */
    UART_Open(UART1, 115200U);

    UART_SetTimeoutCnt(UART1, 10); // Set Rx Time-out counter

    // Set RX FIFO Interrupt Trigger Level
    UART1->FIFO &= ~ UART_FIFO_RFITL_Msk;
    UART1->FIFO |= UART_FIFO_RFITL_8BYTES;

    /* Enable UART RDA/Time-out interrupt - This will fire interrupt if FIFO is 8 byte full or there is something present and the timeout goes */
    NVIC_EnableIRQ(UART1_IRQn);
    UART_EnableInt(UART1, (UART_INTEN_RDAIEN_Msk | UART_INTEN_RXTOIEN_Msk));
}

/*---------------------------------------------------------------------------------------------------------*/
/* UART Callback function                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/

void ULTIMO_UART_IRQHandler()
{
    uint8_t u8InByte = 0xFF;
    uint32_t u32IntSts = UART1->INTSTS;
    bool isFirst = true;

    g_intCounter++;

    /*if the interrupt was either due to RDA or RXTO
     */

    if (u32IntSts & (UART_INTEN_RDAIEN_Msk | UART_INTEN_RXTOIEN_Msk)) {

    	/* Get all the input characters */
    	while(UART_GET_RX_EMPTY(UART1) == 0)
    	{
            /* Get the character from UART Buffer */
    		u8InByte = UART_READ(UART1);

    		/* Check if buffer full - TODO what happends when RX buffer overflows?*/
    		if(g_u32comRbytes < ULT_RX_BUF_SIZE)
    		{
    			/* Enqueue the character */
    			g_u8RecData[g_u32comRtail] = u8InByte;
    			g_u32comRtail = (g_u32comRtail == (ULT_RX_BUF_SIZE - 1)) ? 0 : (g_u32comRtail + 1);
    			g_u32comRbytes++;
    		}

    	}
    }

    //IQ - I do not understand the purpose of this!?!
    if(UART1->FIFOSTS & (UART_FIFOSTS_BIF_Msk | UART_FIFOSTS_FEF_Msk | UART_FIFOSTS_PEF_Msk | UART_FIFOSTS_RXOVIF_Msk))
    {
        UART1->FIFOSTS = (UART_FIFOSTS_BIF_Msk | UART_FIFOSTS_FEF_Msk | UART_FIFOSTS_PEF_Msk | UART_FIFOSTS_RXOVIF_Msk);
    }
}


/* END uart_ultimo. */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
