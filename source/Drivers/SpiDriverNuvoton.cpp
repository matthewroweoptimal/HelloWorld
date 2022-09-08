/*
 * This module is based on the Freescale driver from CDDLive project in file fsl_dspi_master_driver.c
 * It has been modified for the Nuvoton M480 processor, taking into account the Nuvoton
 * example project OpenNuvoton\SampleCode\StdDriver\SPI_MasterFIFOMode\
 *
 *
 * -----------------------------------------------------------------------------------
 *
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "SpiDriverNuvoton.h"
#include <cstring>

//#define DEBUG_SPI	1   // Define for Debug output (Note will affect ISR timing, but gives an idea of SPI transactions)

/*!
 * @brief Table to save DSPI IRQ enum numbers defined in CMSIS files.
 *
 * This is used by DSPI master and slave init functions to enable or disable DSPI interrupts.
 * This table is indexed by the module instance number and returns DSPI IRQ numbers.
 */
const IRQn_Type g_dspiIrqId[SPI_INSTANCE_COUNT] = { SPI0_IRQn, SPI1_IRQn, SPI2_IRQn, SPI3_IRQn };

/* Pointer to runtime state structure.*/
dspi_master_state_t* g_dspiStatePtr[SPI_INSTANCE_COUNT] = {NULL, NULL, NULL, NULL};

/*! @brief Table of SPI FIFO sizes per instance. */
// The transmitting and receiving threshold can be configured by setting TXTH (SPIx_FIFOCTL[30:28])
// and RXTH (SPIx_FIFOCTL[26:24]). When the count of valid data stored in transmit FIFO buffer is less
// than or equal to TXTH (SPIx_FIFOCTL[30:28]) setting, TXTHIF (SPIx_STATUS[18]) will be set to 1.
// When the count of valid data stored in receive FIFO buffer is larger than RXTH
// (SPIx_FIFOCTL[26:24]) setting, RXTHIF (SPIx_STATUS[10]) will be set to 1.
//
// 8 is the Max FIFO size for 8 to 16 bit width transfers, so we use this for max efficiency.
// Note : The TXTH and RXTH values set are actually 1 less than the sizes below (0..7)
const uint32_t g_dspiFifoSize[SPI_INSTANCE_COUNT] = { 8, 8, 8, 8 };



static void DSPI_DRV_MasterIRQHandler(uint32_t instance);
static dspi_status_t DSPI_DRV_MasterStartTransfer(uint32_t instance);
static void DSPI_DRV_MasterFillupTxFifo(uint32_t instance);
static void DSPI_DRV_MasterCompleteTransfer(uint32_t instance);

static SPI_T *getSpiInstance(uint32_t instance)
{
    switch (instance)
    {
        case 0: return ((SPI_T *) SPI0_BASE);    break;
        case 1: return ((SPI_T *) SPI1_BASE);    break;
        case 2: return ((SPI_T *) SPI2_BASE);    break;
        case 3: return ((SPI_T *) SPI3_BASE);    break;   
        default: configASSERT(0);                break;
    }
    return NULL;
}

static uint32_t getSpiModuleInstance(uint32_t instance)
{
    switch (instance)
    {
        case 0: return SPI0_MODULE;    break;
        case 1: return SPI1_MODULE;    break;
        case 2: return SPI2_MODULE;    break;
        case 3: return SPI3_MODULE;    break;   
        default: configASSERT(0);      break;
    }
    return SPI0_MODULE;;
}


/*!
 * @brief The function DSPI_DRV_IRQHandler passes IRQ control to either the master or
 * slave driver.
 *
 * The address of the IRQ handlers are checked to make sure they are non-zero before
 * they are called. If the IRQ handler's address is zero, it means that driver was
 * not present in the link (because the IRQ handlers are marked as weak). This would
 * actually be a program error, because it means the master/slave config for the IRQ
 * was set incorrectly.
 */
void DSPI_DRV_IRQHandler(uint32_t instance)
{   // Only Master mode supported for CDDLive
    configASSERT(instance < SPI_INSTANCE_COUNT);
    /* Master mode.*/
    DSPI_DRV_MasterIRQHandler(instance);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : DSPI_DRV_MasterInit
 * Description   : Initialize a DSPI instance for master mode operation.
 * This function uses a CPU interrupt driven method for transferring data.
 * This function will initialize the run-time state structure to keep track of the on-going
 * transfers, ungate the clock to the DSPI module, reset the DSPI module, initialize the module
 * to user defined settings and default settings, configure the IRQ state structure and enable
 * the module-level interrupt to the core, and enable the DSPI module.
 * The CTAR parameter is special in that it allows the user to have different SPI devices
 * connected to the same DSPI module instance in conjunction with different peripheral chip
 * selects. Each CTAR contains the bus attributes associated with that particular SPI device.
 * For simplicity and for most use cases where only one SPI device is connected per DSPI module
 * instance, it is recommended to use CTAR0.
 * The following is an example of how to set up the dspi_master_state_t and the
 * dspi_master_user_config_t parameters and how to call the DSPI_DRV_MasterInit function by passing
 * in these parameters:
 *   dspi_master_state_t dspiMasterState; <- the user simply allocates memory for this struct
 *   uint32_t calculatedBaudRate;
 *   dspi_master_user_config_t userConfig; <- the user fills out members for this struct
 *   userConfig.isChipSelectContinuous = false;
 *   userConfig.isSckContinuous = false;
 *   userConfig.pcsPolarity = kDspiPcs_ActiveLow;
 *   userConfig.whichCtar = kDspiCtar0;
 *   userConfig.whichPcs = kDspiPcs0;
 *   DSPI_DRV_MasterInit(masterInstance, &dspiMasterState, &userConfig);
 *
 *END**************************************************************************/
dspi_status_t DSPI_DRV_MasterInit(uint32_t instance, dspi_master_state_t * dspiState, const dspi_master_user_config_t * userConfig,
									const dspi_device_t * busConfig)
{
    uint32_t dspiSourceClock;
    dspi_status_t errorCode = kStatus_DSPI_Success;
    SPI_T *pSPI_INSTANCE = getSpiInstance(instance);
    uint32_t SPI_MODULE = getSpiModuleInstance(instance);

    /* Clear the run-time state struct for this instance.*/
    memset(dspiState, 0, sizeof(* dspiState));

    /* Note, remember to first enable clocks to the DSPI module before making any register accesses
     * Enable clock for DSPI
     */
    /* >>>>>>> Select PCLK0 as the clock source of SPI0 */
    CLK_SetModuleClock(SPI_MODULE, CLK_CLKSEL2_SPI0SEL_PCLK1, MODULE_NoMsk);    // NUVOTON M480
    /* >>>>>>>>> Enable SPI0 peripheral clock */
    CLK_EnableModuleClock(SPI_MODULE);                                          // NUVOTON M480 
         
    /* Configure the run-time state struct with the data command parameters*/
//    dspiState->whichCtar = userConfig->whichCtar;  /* set the dspiState struct CTAR*/
//    dspiState->whichPcs = userConfig->whichPcs;  /* set the dspiState struct whichPcs*/
    dspiState->isChipSelectContinuous = userConfig->isChipSelectContinuous; /* continuous PCS*/
    dspiState->irqSync = xSemaphoreCreateBinary();  // Create semaphore the IRQ will use to signal us to unblock.
    
    dspiState->bitsPerFrame = busConfig->dataBusConfig.bitsPerFrame; /* update dspiState bits/frame */
    
    /* Save runtime structure pointers to irq handler can point to the correct state structure*/
    g_dspiStatePtr[instance] = dspiState;
        
    
    /* Configure clock polarity and phase */
    uint32_t u32SPIMode = SPI_MODE_0;
    if (busConfig->dataBusConfig.clkPolarity == kDspiClockPolarity_ActiveLow)
    {
        if (busConfig->dataBusConfig.clkPhase == kDspiClockPhase_SecondEdge)
            u32SPIMode = SPI_MODE_0;    // CLKPOL=0; RXNEG=0; TXNEG=1  : Clock idle @ LOW state, Rx data on positive edge, Tx data on negative edge
        else
            u32SPIMode = SPI_MODE_1;    // CLKPOL=0; RXNEG=1; TXNEG=0  : Clock idle @ LOW state, Rx data on negative edge, Tx data on positive edge
    }
    else
    {
        if (busConfig->dataBusConfig.clkPhase == kDspiClockPhase_SecondEdge)
            u32SPIMode = SPI_MODE_3;    // CLKPOL=1; RXNEG=0; TXNEG=1  : Clock idle @ HIGH state, Rx data on positive edge, Tx data on negative edge
        else
            u32SPIMode = SPI_MODE_2;    // CLKPOL=1; RXNEG=1; TXNEG=0  : Clock idle @ HIGH state, Rx data on negative edge, Tx data on positive edge        
    }
    
    /* Configure as a MASTER, clock polarity and phase, transaction width, etc */
    uint32_t actualSpiHz = SPI_Open(pSPI_INSTANCE, SPI_MASTER, u32SPIMode, dspiState->bitsPerFrame, busConfig->bitsPerSec);

    /* Enable the automatic hardware slave select function. Select the SS pin and configure polarity. */
    uint32_t slaveSelectPolarity = SPI_SS_ACTIVE_HIGH;
    if ( userConfig->pcsPolarity == kDspiPcs_ActiveLow )
    {
        slaveSelectPolarity = SPI_SS_ACTIVE_LOW;
    }
    SPI_EnableAutoSS(pSPI_INSTANCE, SPI_SS, slaveSelectPolarity);
    
    SPI_SetFIFO(pSPI_INSTANCE, g_dspiFifoSize[instance]-1, g_dspiFifoSize[instance]-1);
        

    if (busConfig->dataBusConfig.direction == kDspiMsbFirst)
    {
    	SPI_SET_MSB_FIRST(pSPI_INSTANCE);
    } else
    {
    	SPI_SET_LSB_FIRST(pSPI_INSTANCE);
    }


    /* Initialize the DSPI module with user config */
    // TODO : Could take into account (?) : .isSckContinuous, .isChipSelectContinuous,






    return errorCode;

}

/*FUNCTION**********************************************************************
 *
 * Function Name : DSPI_DRV_MasterDeinit
 * Description   : Shutdown a DSPI instance.
 * This function resets the DSPI peripheral, gates its clock, and disables the interrupt to
 * the core.
 *
 *END**************************************************************************/
dspi_status_t DSPI_DRV_MasterDeinit(uint32_t instance)
{
    /* instantiate local variable of type dspi_master_state_t and point to global state */
    dspi_master_state_t * dspiState = (dspi_master_state_t *)g_dspiStatePtr[instance];
    SPI_T *pSPI_INSTANCE = getSpiInstance(instance);

    /* Disable TX FIFO threshold interrupt and RX FIFO time-out interrupt */
    SPI_DisableInt(pSPI_INSTANCE, SPI_FIFO_TXTH_INT_MASK | SPI_FIFOCTL_RXTOIEN_Msk);
    NVIC_DisableIRQ(g_dspiIrqId[instance]);
    SPI_Close(pSPI_INSTANCE);

    /* destroy the interrupt sync object.*/
    vSemaphoreDelete(dspiState->irqSync);

    /* Clear state pointer. */
    g_dspiStatePtr[instance] = NULL;

    return kStatus_DSPI_Success;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : DSPI_DRV_MasterIRQHandler
 * Description   : Interrupt handler for DSPI master mode.
 * This handler uses the buffers stored in the dspi_master_state_t structs to transfer data.
 * This is not a public API as it is called whenever an interrupt occurs.
 *
 *END**************************************************************************/
static void DSPI_DRV_MasterIRQHandler(uint32_t instance)
{
    /* instantiate local variable of type dspi_master_state_t and point to global state */
    dspi_master_state_t *dspiState = g_dspiStatePtr[instance];
    SPI_T *pSPI_INSTANCE = getSpiInstance(instance);

    /* RECEIVE IRQ handler: Check read buffer only if there are remaining bytes to read. */
    uint16_t wordReceived; /* Maximum supported data bit length in master mode is 16-bits */
    if (dspiState->remainingReceiveByteCount)
    {
        /* Check read buffer.*/
        /* If bits/frame is greater than one byte */
        if (dspiState->bitsPerFrame > 8)
        {
            while (SPI_GET_RX_FIFO_EMPTY_FLAG(pSPI_INSTANCE) == 0)
            {
                wordReceived = SPI_READ_RX(pSPI_INSTANCE);
            #if DEBUG_SPI
                printf( "RX %d\n", wordReceived );
            #endif

                /* Store read bytes into rx buffer only if a buffer pointer was provided */
                if (dspiState->receiveBuffer)
                {   /* For the last word received, if there is an extra byte due to the odd transfer
                     * byte count, only save the the last byte and discard the upper byte
                     */
                    if ((dspiState->remainingReceiveByteCount == 2) && (dspiState->extraByte))
                    {
                        *dspiState->receiveBuffer = wordReceived; /* Write first data byte */
                    }
                    else
                    {
                        *dspiState->receiveBuffer = wordReceived; /* Write first data byte */
                        ++dspiState->receiveBuffer; /* increment to next data byte */
                        *dspiState->receiveBuffer = wordReceived >> 8; /* Write second data byte */
                        ++dspiState->receiveBuffer; /* increment to next data byte */
                    }
                }
                dspiState->remainingReceiveByteCount -= 2;

                if (dspiState->remainingReceiveByteCount <= 0)
                {
                    break;
                }
            } /* End of RX FIFO drain while loop */
        }
        /* Optimized for bits/frame less than or equal to one byte. */
        else
        {
            while (SPI_GET_RX_FIFO_EMPTY_FLAG(pSPI_INSTANCE) == 0)
            {
                wordReceived = SPI_READ_RX(pSPI_INSTANCE);
            #if DEBUG_SPI
                printf( "RX %d\n", wordReceived );
            #endif                

                /* Store read bytes into rx buffer only if a buffer pointer was provided */
                if (dspiState->receiveBuffer)
                {
                    *dspiState->receiveBuffer = wordReceived;
                    ++dspiState->receiveBuffer;
                }

                dspiState->remainingReceiveByteCount--;
                if (dspiState->remainingReceiveByteCount <= 0)
                {
                    break;
                }
            } /* End of RX FIFO drain while loop */
        }
    }

    /* Check write buffer. We always have to send a word in order to keep the transfer
     * moving. So if the caller didn't provide a send buffer, we just send a zero.
     */
    if (dspiState->remainingSendByteCount > 0)
    {
        DSPI_DRV_MasterFillupTxFifo(instance);
    }

    /* Check the RX FIFO time-out interrupt flag */
    if (SPI_GetIntFlag(pSPI_INSTANCE, SPI_FIFO_RXTO_INT_MASK))
    {
        if (dspiState->bitsPerFrame > 8)
        {
            /* If RX FIFO is not empty, read RX FIFO. */
            while((pSPI_INSTANCE->STATUS & SPI_STATUS_RXEMPTY_Msk) == 0)
            {
            	wordReceived = SPI_READ_RX(pSPI_INSTANCE);
            #if DEBUG_SPI
                printf( "RX2 %d\n", wordReceived );
            #endif            	

                /* Store read bytes into rx buffer only if a buffer pointer was provided */
                if (dspiState->receiveBuffer)
                {   /* For the last word received, if there is an extra byte due to the odd transfer
                     * byte count, only save the the last byte and discard the upper byte
                     */
                    if ((dspiState->remainingReceiveByteCount == 2) && (dspiState->extraByte))
                    {
                        *dspiState->receiveBuffer = wordReceived; /* Write first data byte */
                    }
                    else
                    {
                        *dspiState->receiveBuffer = wordReceived; /* Write first data byte */
                        ++dspiState->receiveBuffer; /* increment to next data byte */
                        *dspiState->receiveBuffer = wordReceived >> 8; /* Write second data byte */
                        ++dspiState->receiveBuffer; /* increment to next data byte */
                    }
                }
                dspiState->remainingReceiveByteCount -= 2;

                if (dspiState->remainingReceiveByteCount <= 0)
                {
                    break;
                }
            }
        }
        else /* Optimized for bits/frame less than or equal to one byte. */
        {
            /* If RX FIFO is not empty, read RX FIFO. */
            while((pSPI_INSTANCE->STATUS & SPI_STATUS_RXEMPTY_Msk) == 0)
            {
            	wordReceived = SPI_READ_RX(pSPI_INSTANCE);
            #if DEBUG_SPI
                printf( "RX2 %d\n", wordReceived );
            #endif              	

                /* Store read bytes into rx buffer only if a buffer pointer was provided */
                if (dspiState->receiveBuffer)
                {
                    *dspiState->receiveBuffer = wordReceived;
                    ++dspiState->receiveBuffer;
                }

                dspiState->remainingReceiveByteCount--;
                if (dspiState->remainingReceiveByteCount <= 0)
                {
                    break;
                }
            }
        }

    }

    /* Check if we're done with this transfer.*/
    if ((dspiState->remainingSendByteCount <= 0) && (dspiState->remainingReceiveByteCount <= 0))
    {
        /* Complete the transfer and disable the interrupts */
        DSPI_DRV_MasterCompleteTransfer(instance);
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : DSPI_DRV_MasterTransferBlocking
 * Description   : Perform a blocking SPI master mode transfer.
 * This function simultaneously sends and receives data on the SPI bus, as SPI is naturally
 * a full-duplex bus. The function will not return until the transfer is complete.
 *
 *END**************************************************************************/
dspi_status_t DSPI_DRV_MasterTransferBlocking( uint32_t instance, const uint8_t *sendBuffer, uint8_t *receiveBuffer, size_t transferByteCount, uint32_t msTimeout )
{
    /* instantiate local variable of type dspi_master_state_t and point to global state */
    dspi_status_t error = kStatus_DSPI_Success;
    SPI_T *pSPI_INSTANCE = getSpiInstance(instance);
    dspi_master_state_t *dspiState = g_dspiStatePtr[instance];
    
    /* If the transfer count is zero, then return immediately.*/
    if (transferByteCount == 0)
    {
        return error;
    }
    
    /* fill in members of the run-time state struct*/
    dspiState->isTransferBlocking = true; /* Indicates this is a blocking transfer */
    dspiState->sendBuffer = (const uint8_t *)sendBuffer;
    dspiState->receiveBuffer = (uint8_t *)receiveBuffer;
    dspiState->remainingSendByteCount = transferByteCount;
    dspiState->remainingReceiveByteCount = transferByteCount;
    
    /* start the transfer process*/
    if (DSPI_DRV_MasterStartTransfer(instance) == kStatus_DSPI_Busy)
    {
        return kStatus_DSPI_Busy;
    }
    
    /* ---- As this is a synchronous transfer, wait until the transfer is complete. ----*/
    BaseType_t success = xSemaphoreTake(dspiState->irqSync, pdMS_TO_TICKS(msTimeout));
    if ( success != pdTRUE )
    {
        printf("SPI transfer timed out.\n");
        /* The transfer is complete.*/
        dspiState->isTransferInProgress = false;
        error = kStatus_DSPI_Timeout;
    }
    
    /* Disable TX FIFO threshold interrupt and RX FIFO time-out interrupt */
    SPI_DisableInt(pSPI_INSTANCE, SPI_FIFO_TXTH_INT_MASK | SPI_FIFOCTL_RXTOIEN_Msk);
    NVIC_DisableIRQ(g_dspiIrqId[instance]);
#if DEBUG_SPI    
    printf("SPI transfer completed OK.\n");
#endif
    return error;
}

/*!
 * @brief Initiate (start) a transfer. This is not a public API as it is called from other
 *  driver functions
 */
static dspi_status_t DSPI_DRV_MasterStartTransfer(uint32_t instance)
{
    /* instantiate local variable of type dspi_master_state_t and point to global state */
    SPI_T *pSPI_INSTANCE = getSpiInstance(instance);
    dspi_master_state_t *dspiState = g_dspiStatePtr[instance];

    /* Check that we're not busy.*/
    if (dspiState->isTransferInProgress)
    {
        return kStatus_DSPI_Busy;
    }

    /* Check the transfer byte count. If bits/frame > 8, meaning 2 bytes and if
     * the transfer byte count is an odd count we'll have to increase the transfer byte count
     * by one and assert a flag to indicate to the send and receive functions that it will
     * need to handle an extra byte.
     */
    if ((dspiState->bitsPerFrame > 8) && (dspiState->remainingSendByteCount & 1UL))
    {
        dspiState->remainingSendByteCount += 1;
        dspiState->remainingReceiveByteCount += 1;
        dspiState->extraByte = true;
    }
    else
    {
        dspiState->extraByte = false;
    }

    /* Save information about the transfer for use by the ISR.*/
    dspiState->isTransferInProgress = true;
    
    /* >>>>>>>>>>>> Set TX FIFO threshold, enable TX FIFO threshold interrupt and RX FIFO time-out interrupt */
    SPI_EnableInt(pSPI_INSTANCE, SPI_FIFO_TXTH_INT_MASK | SPI_FIFO_RXTO_INT_MASK);
    /* enable the interrupt*/
    NVIC_EnableIRQ(g_dspiIrqId[instance]);


    /* Fill up the DSPI FIFO (even if one word deep, data still written to data buffer) */
    DSPI_DRV_MasterFillupTxFifo(instance);

    return kStatus_DSPI_Success;
}



/*!
 * @brief Fill up the TX FIFO with data. - called from ISR context
 * This function fills up the TX FIFO with initial data for start of transfers where it will
 * first clear the transfer count.  Otherwise, if the TX FIFO fill is part of an ongoing transfer
 * then do not clear the transfer count.  The param "isInitialData" is used to determine if this
 * is an initial data fill.
 * This is not a public API as it is called from other driver functions.
 */
static void DSPI_DRV_MasterFillupTxFifo(uint32_t instance)
{
    /* instantiate local variable of type dspi_master_state_t and point to global state */
    SPI_T *pSPI_INSTANCE = getSpiInstance(instance);
    dspi_master_state_t *dspiState = g_dspiStatePtr[instance];
    uint16_t wordToSend = 0;
    
    /* Declare variables for storing volatile data later in the code */
    int32_t remainingReceiveByteCount, remainingSendByteCount;

    /* Store the DSPI state struct volatile member variables into temporary
     * non-volatile variables to allow for MISRA compliant calculations
     */
    remainingSendByteCount = dspiState->remainingSendByteCount;
    remainingReceiveByteCount = dspiState->remainingReceiveByteCount;

    /* Architectural note: When developing the TX FIFO fill functionality, it was found that to
     * achieve more efficient run-time performance, it was better to first check the bits/frame
     * setting and then proceed with the FIFO fill management process, rather than to clutter the
     * FIFO fill process with continual checks of the bits/frame setting.
     */

    /* If bits/frame is greater than one byte */
    if (dspiState->bitsPerFrame > 8)
    {
        /* Fill the fifo until it is full or until the send word count is 0 or until the difference
         * between the remainingReceiveByteCount and remainingSendByteCount equals the FIFO depth.
         * The reason for checking the difference is to ensure we only send as much as the
         * RX FIFO can receive.
         * For this case where bitsPerFrame > 8, each entry in the FIFO contains 2 bytes of the
         * send data, hence the difference between the remainingReceiveByteCount and
         * remainingSendByteCount must be divided by 2 to convert this difference into a
         * 16-bit (2 byte) value.
         */
        while((SPI_GET_TX_FIFO_FULL_FLAG(pSPI_INSTANCE) == 0) &&
              ((remainingReceiveByteCount - remainingSendByteCount)/2 < g_dspiFifoSize[instance]))
        {
            if (dspiState->remainingSendByteCount == 2)
            {   /* If there is an extra byte to send due to an odd byte count, prepare the final
                 * wordToSend here
                 */
                if (dspiState->sendBuffer)
                {
                    if (dspiState->extraByte)
                    {
                        wordToSend = *(dspiState->sendBuffer);
                    }
                    else
                    {
                        wordToSend = *(dspiState->sendBuffer);
                        ++dspiState->sendBuffer; /* increment to next data byte */
                        wordToSend |= (unsigned)(*(dspiState->sendBuffer)) << 8U;
                    }
                }
            }
            /* For all words except the last word */
            else
            {   /* If a send buffer was provided, the word comes from there. Otherwise we just send
                 * a zero (initialized above).
                 */
                if (dspiState->sendBuffer)
                {
                    wordToSend = *(dspiState->sendBuffer);
                    ++dspiState->sendBuffer; /* increment to next data byte */
                    wordToSend |= (unsigned)(*(dspiState->sendBuffer)) << 8U;
                    ++dspiState->sendBuffer; /* increment to next data byte */
                }
            }

            /* Write to TX FIFO */
            if ( dspiState->remainingSendByteCount > 0 )
            {
	        #if DEBUG_SPI
				printf("TX( %d ), %d remain\n", wordToSend, dspiState->remainingSendByteCount-2);
	        #endif
				SPI_WRITE_TX(pSPI_INSTANCE, wordToSend);
				dspiState->remainingSendByteCount -= 2; /* decrement remainingSendByteCount by 2 */
            }

            /* exit loop if send count is zero, else update local variables for next loop */
            if (dspiState->remainingSendByteCount <= 0)
            {
                break;
            }
            else
            {   /* Store the DSPI state struct volatile member variables into temporary
                 * non-volatile variables to allow for MISRA compliant calculations
                 */
                remainingSendByteCount = dspiState->remainingSendByteCount;
            }
        } /* End of TX FIFO fill while loop */
    }
    /* Optimized for bits/frame less than or equal to one byte. */
    else
    {   /* Fill the fifo until it is full or until the send word count is 0 or until the difference
         * between the remainingReceiveByteCount and remainingSendByteCount equals the FIFO depth.
         * The reason for checking the difference is to ensure we only send as much as the
         * RX FIFO can receive.
         */
        while((SPI_GET_TX_FIFO_FULL_FLAG(pSPI_INSTANCE) == 0) &&
              ((remainingReceiveByteCount - remainingSendByteCount) < g_dspiFifoSize[instance]))
        {   /* If a send buffer was provided, the word comes from there. Otherwise we just send
             * a zero (initialized above).
             */
            if (dspiState->sendBuffer)
            {
                wordToSend = *(dspiState->sendBuffer);
                ++dspiState->sendBuffer; /* increment to next data word*/
            }

            /* Write to TX FIFO */
            if ( dspiState->remainingSendByteCount > 0 )
            {
	    #if DEBUG_SPI
				printf("TX( %d ), %d remain\n", wordToSend, dspiState->remainingSendByteCount-1);
	    #endif
				SPI_WRITE_TX(pSPI_INSTANCE, wordToSend);
				dspiState->remainingSendByteCount--; /* decrement remainingSendByteCount*/
            }

            /* exit loop if send count is zero, else update local variables for next loop */
            if (dspiState->remainingSendByteCount <= 0)
            {
                break;
            }
            else
            {   /* Store the DSPI state struct volatile member variables into temporary
                 * non-volatile variables to allow for MISRA compliant calculations
                 */
                remainingSendByteCount = dspiState->remainingSendByteCount;
            }
        } /* End of TX FIFO fill while loop */
    }

    if ( remainingSendByteCount <= 0 )
    {
    	SPI_DisableInt(pSPI_INSTANCE, SPI_FIFO_TXTH_INT_MASK); /* Disable TX FIFO threshold interrupt */
    }
}

/*!
 * @brief Finish up a transfer - called from ISR context.
 * Cleans up after a transfer is complete. Interrupts are disabled, and the DSPI module
 * is disabled. This is not a public API as it is called from other driver functions.
 */
static void DSPI_DRV_MasterCompleteTransfer(uint32_t instance)
{
    /* instantiate local variable of type dspi_master_state_t and point to global state */
    SPI_T *pSPI_INSTANCE = getSpiInstance(instance);
    dspi_master_state_t *dspiState = g_dspiStatePtr[instance];

    /* The transfer is complete.*/
    dspiState->isTransferInProgress = false;
    
    /* Disable TX FIFO threshold interrupt and RX FIFO time-out interrupt */
    SPI_DisableInt(pSPI_INSTANCE, SPI_FIFO_TXTH_INT_MASK | SPI_FIFOCTL_RXTOIEN_Msk);
    NVIC_DisableIRQ(g_dspiIrqId[instance]);

    if (dspiState->isTransferBlocking)
    {
        /* Signal the synchronous completion object */
    	xSemaphoreGiveFromISR(dspiState->irqSync, NULL);
    }
}
