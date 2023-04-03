

/* spi_aux_csense. */

#include "spi_aux_csense.h"

dspi_master_state_t spi_aux_csense_MasterState;
uint32_t spi_aux_csense_calculatedBaudRate = 0;

/*
 * const dspi_device_t spi_aux_BusConfig0 = {
  .bitsPerSec = 1250000U,
  .dataBusConfig.bitsPerFrame = 8U,
  .dataBusConfig.clkPolarity = kDspiClockPolarity_ActiveLow,
  .dataBusConfig.clkPhase = kDspiClockPhase_SecondEdge,
  .dataBusConfig.direction = kDspiMsbFirst,
};
 */

const dspi_device_t spi_aux_BusConfig0 = {
	1250000U,						// bitsPerSec
  { 8U,								// bitsPerFrame
    kDspiClockPolarity_ActiveLow,	// clkPolarity
    kDspiClockPhase_SecondEdge,		// clkPhase
	kDspiMsbFirst					// direction
  }
};

/*
const dspi_master_user_config_t spi_aux_MasterConfig0 = {
  .whichCtar = kDspiCtar0,
  .isSckContinuous = false,
  .isChipSelectContinuous = false,
  .whichPcs = kDspiPcs0,
  .pcsPolarity = kDspiPcs_ActiveLow,
};
*/

const dspi_master_user_config_t spi_aux_MasterConfig0 = {
  false,				// isSckContinuous
  false,				// isChipSelectContinuous
  kDspiPcs_ActiveLow,	// pcsPolarity
};



// See UserInterrupts.h for SPI_IRQHandler()
#if 0
void SPI2_IRQHandler(void)
{   // SHARC connected on SPI0
    DSPI_DRV_IRQHandler(FSL_SPI_SHARC);
}
#endif


/* END spi_sharc. */

