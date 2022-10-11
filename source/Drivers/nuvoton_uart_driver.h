/*
 * IQ hack for Nuvoton compaitibility for Ultimo UART
 *
 * Only UART_DRV_SendDataBlocking & Initialise functions required
 *
 */

#ifndef __NUVOTON_UART_DRIVER__
#define __NUVOTON_UART_DRIVER__
#include "M480.h"
#include "MQX_To_FreeRTOS.h"

#include <stdint.h>
#include <stdbool.h>
//#include "fsl_os_abstraction.h"
//#include "fsl_uart_hal.h"

#define UART_INSTANCE_COUNT   6       // SPI0..6, but in CDDLive we only use : UART0 (trace) and UART1 (Ultimo) ?

#define NU_ULTIMO_UART	1
#define OSA_WAIT_FOREVER 0

/*! @brief Error codes for the UART driver. */
typedef enum _uart_status
{
    kStatus_UART_Success                  = 0x00U,
    kStatus_UART_Fail                     = 0x01U,
    kStatus_UART_BaudRateCalculationError = 0x02U,
    kStatus_UART_RxStandbyModeError       = 0x03U,
    kStatus_UART_ClearStatusFlagError     = 0x04U,
    kStatus_UART_TxNotDisabled            = 0x05U,
    kStatus_UART_RxNotDisabled            = 0x06U,
    kStatus_UART_TxOrRxNotDisabled        = 0x07U,
    kStatus_UART_TxBusy                   = 0x08U,
    kStatus_UART_RxBusy                   = 0x09U,
    kStatus_UART_NoTransmitInProgress     = 0x0AU,
    kStatus_UART_NoReceiveInProgress      = 0x0BU,
    kStatus_UART_Timeout                  = 0x0CU,
    kStatus_UART_Initialized              = 0x0DU,
    kStatus_UART_NoDataToDeal             = 0x0EU,
    kStatus_UART_RxOverRun                = 0x0FU
} uart_status_t;


/*!
 * @addtogroup uart_driver
 * @{
 */

/*!
 * @file
 *
 * Some devices count the UART instances with LPUART(e.g., KL27) or UART0(e.g.,
 * KL25) together. However, they are  different IPs with separate drivers:
 * LPSCI for UART0, LPUART for LPUART. In such cases, this UART driver 
 * only works with specific UART instances.
 *
 * For example, in KL27, there is LPUART0, LPUART1 and UART2. For LPUART0 and
 * LPUART1,  use the LPUART driver. For UART2, use this driver and
 * pass in 2 as the instance number. 0 and 1 are not valid.
 */

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Table of base addresses for UART instances. */
/*extern UART_T * const g_uartBase[UART_INSTANCE_COUNT]; calculated */

/*! @brief Table to save UART IRQ enumeration numbers defined in the CMSIS header file */
extern const IRQn_Type g_uartRxTxIrqId[UART_INSTANCE_COUNT];

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief UART receive callback function type */
typedef void (* uart_rx_callback_t)(uint32_t instance, void *  uartState, bool bRxOverflow);

/*! @brief UART transmit callback function type */
typedef void (* uart_tx_callback_t)(uint32_t instance, void * uartState);

/*!
 * @brief Runtime state of the UART driver.
 *
 * This structure holds data that are used by the UART peripheral driver to
 * communicate between the transfer function and the interrupt handler. The
 * interrupt handler also uses this information to keep track of its progress.
 * The user passes in the memory for the run-time state structure and the
 * UART driver fills out the members.
 */
typedef struct UartState {
    uint8_t txFifoEntryCount;      /*!< Number of data word entries in TX FIFO. */
    const uint8_t * txBuff;        /*!< The buffer of data being sent.*/
    uint8_t * rxBuff;              /*!< The buffer of received data. */
    volatile size_t txSize;        /*!< The remaining number of bytes to be transmitted. */
    volatile size_t rxSize;        /*!< The remaining number of bytes to be received. */
    volatile bool isTxBusy;        /*!< True if there is an active transmit. */
    volatile bool isRxBusy;        /*!< True if there is an active receive. */
    volatile bool isTxBlocking;    /*!< True if transmit is blocking transaction. */
    volatile bool isRxBlocking;    /*!< True if receive is blocking transaction. */
    SemaphoreHandle_t txIrqSync;         /*!< Used to wait for ISR to complete its TX business. */
    SemaphoreHandle_t rxIrqSync;         /*!< Used to wait for ISR to complete its RX business. */
    uart_rx_callback_t rxCallback; /*!< Callback to invoke after receiving byte.*/
    void * rxCallbackParam;        /*!< Receive callback parameter pointer.*/
    uart_tx_callback_t txCallback; /*!< Callback to invoke after transmitting byte.*/
    void * txCallbackParam;        /*!< Transmit callback parameter pointer.*/
} uart_state_t;

/*!
 * @brief User configuration structure for the UART driver.
 *
 * Use an instance of this structure with the UART_DRV_Init()function. This enables configuration of the
 * most common settings of the UART peripheral with a single function call. Settings include:
 * UART baud rate, UART parity mode: disabled (default), or even or odd, the number of stop bits, and 
 * the number of bits per data word.
 * @internal gui name="UART configuration" id="Configuration" 
 */

/*---------------------------------------------------------------------------------------------------------*/
/* UART_LINE constants definitions                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/*#define UART_WORD_LEN_5     (0ul) /*!< UART_LINE setting to set UART word length to 5 bits \hideinitializer */
/*#define UART_WORD_LEN_6     (1ul) /*!< UART_LINE setting to set UART word length to 6 bits \hideinitializer */
/*#define UART_WORD_LEN_7     (2ul) /*!< UART_LINE setting to set UART word length to 7 bits \hideinitializer */
/*#define UART_WORD_LEN_8     (3ul) /*!< UART_LINE setting to set UART word length to 8 bits \hideinitializer */

/*#define UART_PARITY_NONE    (0x0ul << UART_LINE_PBE_Pos) /*!< UART_LINE setting to set UART as no parity   \hideinitializer */
/*#define UART_PARITY_ODD     (0x1ul << UART_LINE_PBE_Pos) /*!< UART_LINE setting to set UART as odd parity  \hideinitializer */
/*#define UART_PARITY_EVEN    (0x3ul << UART_LINE_PBE_Pos) /*!< UART_LINE setting to set UART as even parity \hideinitializer */
/*#define UART_PARITY_MARK    (0x5ul << UART_LINE_PBE_Pos) /*!< UART_LINE setting to keep parity bit as '1'  \hideinitializer */
/*#define UART_PARITY_SPACE   (0x7ul << UART_LINE_PBE_Pos) /*!< UART_LINE setting to keep parity bit as '0'  \hideinitializer */

/*#define UART_STOP_BIT_1     (0x0ul << UART_LINE_NSB_Pos) /*!< UART_LINE setting for one stop bit  \hideinitializer */
/*#define UART_STOP_BIT_1_5   (0x1ul << UART_LINE_NSB_Pos) /*!< UART_LINE setting for 1.5 stop bit when 5-bit word length  \hideinitializer */
/*#define UART_STOP_BIT_2     (0x1ul << UART_LINE_NSB_Pos) /*!< UART_LINE setting for two stop bit when 6, 7, 8-bit word length \hideinitializer */
/*---------------------------------------------------------------------------------------------------------*/

/*! @brief UART parity mode configuration.*/
typedef enum _uart_parity_mode {
	kUartParityDisabled = UART_PARITY_NONE,
	kUartParityOdd  = UART_PARITY_ODD,
	kUartParityEven  = UART_PARITY_EVEN,
	kUartParityMark  = UART_PARITY_MARK,
	kUartParitySpace  = UART_PARITY_SPACE
} uart_parity_mode_t;

/*! @brief UART parity mode configuration.*/
typedef enum _uart_stop_bit_count {
	kUartOneStopBit = UART_STOP_BIT_1,
	kUartOnePfiveStopBits  = UART_STOP_BIT_1_5,
	kUartTwoStopBits  = UART_STOP_BIT_2
} uart_stop_bit_count_t;

/*! @brief UART parity mode configuration.*/
typedef enum _uart_bit_count_per_char {
	kUart5BitsPerChar = UART_WORD_LEN_5,
	kUart6BitsPerChar  = UART_WORD_LEN_6,
	kUart7BitsPerChar = UART_WORD_LEN_7,
	kUart8BitsPerChar  = UART_WORD_LEN_8,
} uart_bit_count_per_char_t;


typedef struct UartUserConfig {
    uint32_t baudRate;            /*!< UART baud rate @internal gui name="Baud rate" id="BaudRate" */
    uart_parity_mode_t parityMode;      /*!< parity mode, disabled (default), even, odd @internal gui name="Parity mode" id="Parity" */
    uart_stop_bit_count_t stopBitCount; /*!< number of stop bits, 1 stop bit (default) or 2 stop bits @internal gui name="Stop bits" id="StopBits" */
    uart_bit_count_per_char_t bitCountPerChar; /*!< number of bits, 8-bit (default) or 9-bit in
                                                    a word (up to 10-bits in some UART instances) @internal gui name="Bits per char" id="DataBits" */
} uart_user_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name UART Interrupt Driver
 * @{
 */

/*!
 * @brief Initializes an UART instance for operation.
 *
 * This function initializes the run-time state structure to keep track of the on-going
 * transfers, un-gates the clock to the UART module, initializes the module
 * to user-defined settings and default settings, configures the IRQ state structure, and enables
 * the module-level interrupt to the core, and  the UART module transmitter and receiver.
 * This example shows how to set up the uart_state_t and the
 * uart_user_config_t parameters and how to call the UART_DRV_Init function by passing
 * in these parameters:
   @code
    uart_user_config_t uartConfig;
    uartConfig.baudRate = 9600;
    uartConfig.bitCountPerChar = kUart8BitsPerChar;
    uartConfig.parityMode = kUartParityDisabled;
    uartConfig.stopBitCount = kUartOneStopBit;
    uart_state_t uartState;
    UART_DRV_Init(instance, &uartState, &uartConfig);
    @endcode
 *
 * @param instance The UART instance number.
 * @param uartStatePtr A pointer to the UART driver state structure memory. The user 
 * passes in the memory for this run-time state structure. The UART driver
 *  populates the members. The run-time state structure keeps track of the
 *  current transfer in progress.
 * @param uartUserConfig The user configuration structure of type uart_user_config_t. The user
 *  populates the members of this structure and passes the pointer of this structure
 *  to this function.
 * @return An error code or kStatus_UART_Success.
 */
uart_status_t UART_DRV_Init(uint32_t instance, uart_state_t * uartStatePtr,
                        const uart_user_config_t * uartUserConfig);

/*!
 * @brief Shuts down the UART by disabling interrupts and the transmitter/receiver.
 *
 * This function disables the UART interrupts, the transmitter and receiver, and
 * flushes the FIFOs (for modules that support FIFOs).
 *
 * @param instance The UART instance number.
 * @return An error code or kStatus_UART_Success.
 */
uart_status_t UART_DRV_Deinit(uint32_t instance);

/*!
 * @brief Installs the callback function for the UART receive.
 *
 * @note After a callback is installed, it bypasses part of the UART IRQHandler logic.
 * So, the callback needs to handle the indexes of rxBuff, rxSize.
 *
 * @param instance The UART instance number.
 * @param function The UART receive callback function.
 * @param rxBuff The receive buffer used inside IRQHandler. This buffer must be kept as long as the callback is alive.
 * @param callbackParam The UART receive callback parameter pointer.
 * @param alwaysEnableRxIrq Whether always enable receive IRQ or not.
 * @return Former UART receive callback function pointer.
 */
uart_rx_callback_t UART_DRV_InstallRxCallback(uint32_t instance, 
                                              uart_rx_callback_t function, 
                                              uint8_t * rxBuff, 
                                              void * callbackParam,
                                              bool alwaysEnableRxIrq);
/*!
 * @brief Installs the callback function for the UART transmit.
 *
 * @note After a callback is installed, it bypasses part of the UART IRQHandler logic.
 * Therefore, the callback needs to handle the txBuff and txSize indexes.
 *
 * @param instance The UART instance number.
 * @param function The UART transmit callback function.
 * @param txBuff The transmit buffer used inside IRQHandler. This buffer must be kept as long as the callback is alive.
 * @param callbackParam The UART transmit callback parameter pointer.
 * @return Former UART transmit callback function pointer.
 */
uart_tx_callback_t UART_DRV_InstallTxCallback(uint32_t instance, 
                                              uart_tx_callback_t function, 
                                              uint8_t * txBuff, 
                                              void * callbackParam);

/*!
 * @brief Sends (transmits) data out through the UART module using a blocking method.
 *
 * A blocking (also known as synchronous) function means that the function does not return until
 * the transmit is complete. This blocking function is used to send data through the UART port.
 *
 * @param instance The UART instance number.
 * @param txBuff A pointer to the source buffer containing 8-bit data chars to send.
 * @param txSize The number of bytes to send.
 * @param timeout A timeout value for RTOS abstraction sync control in milliseconds (ms).
 * @return An error code or kStatus_UART_Success.
 */
uart_status_t UART_DRV_SendDataBlocking(uint32_t instance, 
                                        const uint8_t * txBuff,
                                        uint32_t txSize, 
                                        uint32_t timeout);

/*!
 * @brief Sends (transmits) data through the UART module using a non-blocking method.
 *
 * A non-blocking (also known as synchronous) function means that the function returns
 * immediately after initiating the transmit function. The application has to get the
 * transmit status to see when the transmit is complete. In other words, after calling non-blocking
 * (asynchronous) send function, the application must get the transmit status to check if transmit
 * is complete.
 * The asynchronous method of transmitting and receiving allows the UART to perform a full duplex
 * operation (simultaneously transmit and receive).
 *
 * @param instance The UART module base address.
 * @param txBuff A pointer to the source buffer containing 8-bit data chars to send.
 * @param txSize The number of bytes to send.
 * @return An error code or kStatus_UART_Success.
 */
uart_status_t UART_DRV_SendData(uint32_t instance, const uint8_t * txBuff, uint32_t txSize);

/*!
 * @brief Returns whether the previous UART transmit has finished.
 *
 * When performing an a-sync transmit, call this function to ascertain the state of the
 * current transmission: in progress (or busy) or complete (success). If the
 * transmission is still in progress, the user can obtain the number of words that have been
 * transferred.
 *
 * @param instance The UART module base address.
 * @param bytesRemaining A pointer to a value that is filled in with the number of bytes that
 *                       are remaining in the active transfer.
 * @return The transmit status.
 * @retval kStatus_UART_Success The transmit has completed successfully.
 * @retval kStatus_UART_TxBusy The transmit is still in progress. @a bytesTransmitted is
 *     filled with the number of bytes which are transmitted up to that point.
 */
uart_status_t UART_DRV_GetTransmitStatus(uint32_t instance, uint32_t * bytesRemaining);

/*!
 * @brief Terminates an asynchronous UART transmission early.
 *
 * During an a-sync UART transmission, the user can terminate the transmission early
 * if the transmission is still in progress.
 *
 * @param instance The UART module base address.
 * @return Whether the aborting success or not.
 * @retval kStatus_UART_Success The transmit was successful.
 * @retval kStatus_UART_NoTransmitInProgress No transmission is currently in progress.
 */
uart_status_t UART_DRV_AbortSendingData(uint32_t instance);

/*!
 * @brief Gets (receives) data from the UART module using a blocking method.
 *
 * A blocking (also known as synchronous) function means that the function does not return until
 * the receive is complete. This blocking function sends data through the UART port.
 *
 * @param instance The UART module base address.
 * @param rxBuff A pointer to the buffer containing 8-bit read data chars received.
 * @param rxSize The number of bytes to receive.
 * @param timeout A timeout value for RTOS abstraction sync control in milliseconds (ms).
 * @return An error code or kStatus_UART_Success.
 */
uart_status_t UART_DRV_ReceiveDataBlocking(uint32_t instance,
                                           uint8_t * rxBuff,
                                           uint32_t rxSize,
                                           uint32_t timeout);

/*!
 * @brief Gets (receives) data from the UART module using a non-blocking method.
 *
 * A non-blocking (also known as synchronous) function means that the function returns
 * immediately after initiating the receive function. The application has to get the
 * receive status to see when the receive is complete. In other words, after calling non-blocking
 * (asynchronous) get function, the application must get the receive status to check if receive
 * is completed or not.
 * The asynchronous method of transmitting and receiving allows the UART to perform a full duplex
 * operation (simultaneously transmit and receive).
 *
 * @param instance The UART module base address.
 * @param rxBuff  A pointer to the buffer containing 8-bit read data chars received.
 * @param rxSize The number of bytes to receive.
 * @return An error code or kStatus_UART_Success.
 */
uart_status_t UART_DRV_ReceiveData(uint32_t instance, uint8_t * rxBuff, uint32_t rxSize);

/*!
 * @brief Returns whether the previous UART receive is complete.
 *
 * When performing an a-sync receive, call this function to find out the state of the
 * current receive progress: in progress (or busy) or complete (success). In addition, if the
 * receive is still in progress, the user can obtain the number of words that have been
 * currently received.
 *
 * @param instance The UART module base address.
 * @param bytesRemaining A pointer to a value that is filled in with the number of bytes which
 *                       still need to be received in the active transfer.
 * @return The receive status.
 * @retval kStatus_UART_Success The receive has completed successfully.
 * @retval kStatus_UART_RxBusy The receive is still in progress. @a bytesReceived is
 *     filled with the number of bytes which are received up to that point.
 */
uart_status_t UART_DRV_GetReceiveStatus(uint32_t instance, uint32_t * bytesRemaining);

/*!
 * @brief Terminates an asynchronous UART receive early.
 *
 * During an a-sync UART receive, the user can terminate the receive early
 * if the receive is still in progress.
 *
 * @param instance The UART module base address.
 * @return Whether the aborting success or not.
 * @retval kStatus_UART_Success The receive was successful.
 * @retval kStatus_UART_NoTransmitInProgress No receive is currently in progress.
 */
uart_status_t UART_DRV_AbortReceivingData(uint32_t instance);

/*@}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* __FSL_UART_DRIVER_H__*/
/*******************************************************************************
 * EOF
 ******************************************************************************/

