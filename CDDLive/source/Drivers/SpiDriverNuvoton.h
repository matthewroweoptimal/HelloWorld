#ifndef __SPIDRIVERNUVOTON
#define __SPIDRIVERNUVOTON
#include "M480.h"
#include "MQX_To_FreeRTOS.h"


#define SPI_INSTANCE_COUNT   4       // SPI0..3, but in CDDLive we only use : SPI_1 (SHARC) and SPI_2 (AUX) ?

//#define FSL_SPI_SHARC   0   // SPI0
//#define FSL_SPI_AUX     2   // SPI2  Needed ??

typedef struct DspiMasterState {
//    dspi_ctar_selection_t   whichCtar;              /*!< Desired Clock and Transfer Attributes Register (CTAR)*/
    uint32_t                bitsPerFrame;           /*!< Desired number of bits per frame */
//    dspi_which_pcs_config_t whichPcs;               /*!< Desired Peripheral Chip Select (pcs) */
    bool                    isChipSelectContinuous; /*!< Option to enable the continuous assertion of chip select
                                                            between transfers*/
//    uint32_t                dspiSourceClock;        /*!< Module source clock*/
    volatile bool           isTransferInProgress;   /*!< True if there is an active transfer.*/
    const uint8_t *         sendBuffer;             /*!< The buffer from which transmitted bytes are taken.*/
    uint8_t *               receiveBuffer;          /*!< The buffer into which received bytes are placed.*/
    volatile int32_t        remainingSendByteCount; /*!< Number of bytes remaining to send.*/
    volatile int32_t        remainingReceiveByteCount; /*!< Number of bytes remaining to receive.*/
    volatile bool           isTransferBlocking;     /*!< True if transfer is a blocking transaction. */
    SemaphoreHandle_t       irqSync;                /*!< Used to wait for ISR to complete its business.*/
    bool                    extraByte;              /*!< Flag used for 16-bit transfers with odd byte count */
} dspi_master_state_t;


/*! @brief DSPI clock polarity configuration for a given CTAR*/
typedef enum _dspi_clock_polarity {
    kDspiClockPolarity_ActiveHigh = 0,   /*!< Active-high DSPI clock (idles low)*/
    kDspiClockPolarity_ActiveLow = 1     /*!< Active-low DSPI clock (idles high)*/
} dspi_clock_polarity_t;

/*! @brief DSPI clock phase configuration for a given CTAR*/
typedef enum _dspi_clock_phase {
    kDspiClockPhase_FirstEdge = 0,       /*!< Data is captured on the leading edge of the SCK and
                                              changed on the following edge.*/
    kDspiClockPhase_SecondEdge = 1       /*!< Data is changed on the leading edge of the SCK and
                                              captured on the following edge.*/
} dspi_clock_phase_t;

/*! @brief DSPI data shifter direction options for a given CTAR*/
typedef enum _dspi_shift_direction {
    kDspiMsbFirst = 0,   /*!< Data transfers start with most significant bit.*/
    kDspiLsbFirst = 1    /*!< Data transfers start with least significant bit.*/
} dspi_shift_direction_t;

/*! @brief DSPI Peripheral Chip Select (PCS) Polarity configuration.*/
typedef enum _dspi_pcs_polarity_config {
    kDspiPcs_ActiveHigh = 0, /*!< PCS Active High (idles low) @internal gui name="Active high" */
    kDspiPcs_ActiveLow  = 1 /*!< PCS Active Low (idles high) @internal gui name="Active low" */
} dspi_pcs_polarity_config_t;

/*!
 * @brief DSPI data format settings configuration structure
 *
 * This structure contains the data format settings.  These settings apply to a specific
 * CTARn register, which the user must provide in this structure.
 */
typedef struct DspiDataFormatConfig {
    uint32_t bitsPerFrame;        /*!< Bits per frame, minimum 4, maximum 16 */
    dspi_clock_polarity_t clkPolarity;   /*!< Active high or low clock polarity*/
    dspi_clock_phase_t clkPhase;     /*!< Clock phase setting to change and capture data*/
    dspi_shift_direction_t direction; /*!< MSB or LSB data shift direction
                                           This setting relevant only in master mode and
                                           can be ignored in slave  mode */
} dspi_data_format_config_t;

/*!
 * @brief Data structure containing information about a device on the SPI bus.
 *
 * The user must populate these members to set up the DSPI master  and
 * properly communicate with the SPI device.
 */
typedef struct DspiDevice {
    uint32_t bitsPerSec;                 /*!< @brief Baud rate in bits per second.*/
    dspi_data_format_config_t dataBusConfig;  /* data format configuration structure*/
} dspi_device_t;


/*!
 * @brief The user configuration structure for the DSPI master driver.
 *
 * Use an instance of this structure with the DSPI_DRV_MasterInit() function. This allows the user to configure
 * the most common settings of the DSPI peripheral with a single function call.
 * @internal gui name="Master configuration" id="dspiMasterCfg"
 */
typedef struct DspiMasterUserConfig {
//    dspi_ctar_selection_t whichCtar; /*!< Desired Clock and Transfer Attributes Register(CTAR) @internal gui name="CTAR selection" id="MasterCtar" */
    bool isSckContinuous;                  /*!< Disable or Enable continuous SCK operation @internal gui name="Continuous SCK" id="MasterContSck" */
    bool isChipSelectContinuous;  /*!< Option to enable the continuous assertion of chip select
                                       between transfers @internal gui name="Continuous chip select" id="MasterContCs" */
//    dspi_which_pcs_config_t whichPcs;        /*!< Desired Peripheral Chip Select (pcs) @internal gui name="Chip select" id="MasterCs" */
    dspi_pcs_polarity_config_t pcsPolarity;  /*!< Peripheral Chip Select (pcs) polarity setting. @internal gui name="Chip select polarity" id="MasterPolarity" */
} dspi_master_user_config_t;


/*! @brief Error codes for the DSPI driver.*/
typedef enum _dspi_status
{
    kStatus_DSPI_Success = 0,
    kStatus_DSPI_SlaveTxUnderrun,           /*!< DSPI Slave Tx Under run error*/
    kStatus_DSPI_SlaveRxOverrun,            /*!< DSPI Slave Rx Overrun error*/
    kStatus_DSPI_Timeout,                   /*!< DSPI transfer timed out*/
    kStatus_DSPI_Busy,                      /*!< DSPI instance is already busy performing a
                                                 transfer.*/
    kStatus_DSPI_NoTransferInProgress,      /*!< Attempt to abort a transfer when no transfer
                                                 was in progress*/
    kStatus_DSPI_InvalidBitCount,           /*!< bits-per-frame value not valid*/
    kStatus_DSPI_InvalidInstanceNumber,     /*!< DSPI instance number does not match current count*/
    kStatus_DSPI_OutOfRange,                /*!< DSPI out-of-range error  */
    kStatus_DSPI_InvalidParameter,          /*!< DSPI invalid parameter error */
    kStatus_DSPI_NonInit,                   /*!< DSPI driver does not initialize, not ready */
    kStatus_DSPI_Initialized,               /*!< DSPI driver has initialized, cannot re-initialize*/
    kStatus_DSPI_DMAChannelInvalid,         /*!< DSPI driver could not request DMA channel(s) */
    kStatus_DSPI_Error,                     /*!< DSPI driver error */
    kStatus_DSPI_EdmaStcdUnaligned32Error   /*!< DSPI Edma driver STCD unaligned to 32byte error */
} dspi_status_t;

extern void DSPI_DRV_IRQHandler(uint32_t instance);
extern dspi_status_t DSPI_DRV_MasterInit(uint32_t instance, dspi_master_state_t * dspiState, const dspi_master_user_config_t * userConfig, const dspi_device_t * busConfig);
extern dspi_status_t DSPI_DRV_MasterDeinit(uint32_t instance);
extern dspi_status_t DSPI_DRV_MasterTransferBlocking( uint32_t instance, const uint8_t *sendBuffer, uint8_t *receiveBuffer, size_t transferByteCount, uint32_t msTimeout );


#endif // __SPIDRIVERNUVOTON

