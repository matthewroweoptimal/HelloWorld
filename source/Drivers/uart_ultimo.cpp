
/* MODULE uart_ultimo. */

#include "uart_ultimo.h"

extern uint32_t m_ServerQueue[];
uint8_t	uart_ultimo_rx_byte;

#ifdef ULTIMO_UART_EDMA

// Store runtime state structure for the eDMA driver
edma_state_t                uart_ultimo_State;
// Store runtime state structure for UART driver with EDMA
uart_edma_state_t           uart_ultimo_EdmaState;

// Config the eDMA driver
const edma_user_config_t          uart_ultimo_EdmaConfig = {
    .chnArbitration     = kEDMAChnArbitrationRoundrobin,
#if (FSL_FEATURE_EDMA_CHANNEL_GROUP_COUNT > 0x1U)
    .groupArbitration   = kEDMAGroupArbitrationFixedPriority,
    .groupPriority      = kEDMAGroup0PriorityLowGroup1PriorityHigh,
#endif
    .notHaltOnError     = false
};

// Config the UART driver
const uart_edma_user_config_t     uart_ultimo_Config = {
    .bitCountPerChar = kUart8BitsPerChar,
    .parityMode      = kUartParityDisabled,
    .stopBitCount    = kUartOneStopBit,
    .baudRate        = 115200U
};

#else	//	ULTIMO_UART_EDMA
/*! Driver state structure without DMA */
uart_state_t uart_ultimo_State;

/*! uart_ultimo configuration structure */
//const uart_user_config_t uart_ultimo_Config = {
//  .baudRate = 115200U,
//  .parityMode = kUartParityDisabled,
//  .stopBitCount = kUartOneStopBit,
//  .bitCountPerChar = kUart8BitsPerChar,
//};

const uart_user_config_t uart_ultimo_Config = {
	115200U,				// baudRate
	kUartParityDisabled,	// parityMode
	kUartOneStopBit,		// stopBitCount
	kUart8BitsPerChar		// bitCountPerChar
};


#endif	//	ULTIMO_UART_EDMA




  
/* Implementation of UART3 handler named in startup code. */         
#ifdef ULTIMO_UART_EDMA
extern void EDMA_DRV_IRQHandler(uint8_t channel);
void edmaRx_ultimo_IRQHandler(void)
{
	EDMA_DRV_IRQHandler(uart_ultimo_EdmaState.edmaUartRx.channel);
}

void edmaTx_ultimo_IRQHandler(void)
{
	EDMA_DRV_IRQHandler(uart_ultimo_EdmaState.edmaUartTx.channel);
}
#else	//	ULTIMO_UART_EDMA
extern void UART_DRV_IRQHandler(uint32_t instance);
void uart_ultimo_IRQHandler(void)
{
  UART_DRV_IRQHandler(FSL_UART_ULTIMO);
}
void uart_ultimo_rx_cb(uint32_t instance, void * uartState, bool bRxOverflow)
{
	int nCount = 0;

	_mqx_uint uiWord = (_mqx_uint)uart_ultimo_rx_byte;
	if (bRxOverflow)
	{
		uiWord |= 0xff000000;
	}
	//_mqx_uint rc = _lwmsgq_send((void *)m_ServerQueue, &uiWord, 0);		//	LWMSGQ_SEND_BLOCK_ON_FULL
	if (MQX_OK!=rc)
	{
//		printf("_lwmsgq_send Server is full1!\n");
	}
}
#endif	//	ULTIMO_UART_EDMA



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
