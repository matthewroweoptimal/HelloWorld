

/* spi_sharc. */

#include "spi_sharc.h"

dspi_master_state_t spi_sharc_MasterState;
uint32_t spi_sharc_calculatedBaudRate = 0;
  
/*
const dspi_device_t spi_sharc_bootConfig = {
  .bitsPerSec = 535714U,	// bitsPerSec
  .dataBusConfig.bitsPerFrame = 8U,	// bitsPerFrame
  .dataBusConfig.clkPolarity = kDspiClockPolarity_ActiveLow,	// clkPolarity
  .dataBusConfig.clkPhase = kDspiClockPhase_SecondEdge,	// clkPhase
  .dataBusConfig.direction = kDspiLsbFirst,	// direction
};
*/
const dspi_device_t spi_sharc_bootConfig = {
	535714U,						// bitsPerSec
  { 8U,								// bitsPerFrame
    kDspiClockPolarity_ActiveLow,	// clkPolarity
    kDspiClockPhase_SecondEdge,		// clkPhase
    kDspiLsbFirst					// direction
  }
};

/*
const dspi_device_t spi_sharc_hostConfig = {
  .bitsPerSec = 6000000U,
  .dataBusConfig.bitsPerFrame = 16U,
  .dataBusConfig.clkPolarity = kDspiClockPolarity_ActiveLow,
  .dataBusConfig.clkPhase = kDspiClockPhase_SecondEdge,
  .dataBusConfig.direction = kDspiMsbFirst,
};
*/
const dspi_device_t spi_sharc_hostConfig = {
	6000000U,						// bitsPerSec
  { 16U,							// bitsPerFrame
    kDspiClockPolarity_ActiveLow,	// clkPolarity
    kDspiClockPhase_SecondEdge,		// clkPhase
	kDspiMsbFirst					// direction
  }
};

/*
const dspi_master_user_config_t spi_sharc_MasterConfig_boot = {
//  .whichCtar = kDspiCtar0,
  .isSckContinuous = false,
  .isChipSelectContinuous = true,
//  .whichPcs = kDspiPcs0,
  .pcsPolarity = kDspiPcs_ActiveLow,
};
*/
const dspi_master_user_config_t spi_sharc_MasterConfig_boot = {
  false,				// isSckContinuous
  true,					// isChipSelectContinuous
  kDspiPcs_ActiveLow,	// pcsPolarity
};

/*
const dspi_master_user_config_t spi_sharc_MasterConfig_host = {
//  .whichCtar = kDspiCtar0,
  .isSckContinuous = false,
  .isChipSelectContinuous = true,
//  .whichPcs = kDspiPcs0,
  .pcsPolarity = kDspiPcs_ActiveLow,
};
*/
const dspi_master_user_config_t spi_sharc_MasterConfig_host = {
  false,				// isSckContinuous
  true,					// isChipSelectContinuous
  kDspiPcs_ActiveLow,	// pcsPolarity
};

void dsp_sharc_SetMode_Host(void)
{
	dspi_status_t 	dsp_result;
	uint8_t return_status;
	uint32_t retval;

	do{
		dsp_result = DSPI_DRV_MasterDeinit(FSL_SPI_SHARC);
		return_status = *(uint8_t*) &dsp_result;
	} while(return_status);

//	OSA_InstallIntHandler(SPI0_IRQn, spi_sharc_IRQHandler);

	DSPI_DRV_MasterInit(FSL_SPI_SHARC, &spi_sharc_MasterState, &spi_sharc_MasterConfig_host, &spi_sharc_hostConfig);

//	DSPI_DRV_MasterConfigureBus(FSL_SPI_SHARC, &spi_sharc_hostConfig, &spi_sharc_calculatedBaudRate);
//    DSPI_DRV_MasterSetDelay(FSL_SPI_SHARC, kDspiPcsToSck, 64, &retval);
//    DSPI_DRV_MasterSetDelay(FSL_SPI_SHARC, kDspiLastSckToPcs, 64, &retval);

}


// See UserInterrupts.h for SPI_IRQHandler()
#if 0
void SPI0_IRQHandler(void)
{   // SHARC connected on SPI0
    DSPI_DRV_IRQHandler(FSL_SPI_SHARC);
}
#endif


/* END spi_sharc. */

