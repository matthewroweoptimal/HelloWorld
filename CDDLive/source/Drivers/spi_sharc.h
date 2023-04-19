
#ifndef __spi_sharc_H
#define __spi_sharc_H
/* MODULE spi_sharc. */

#include "SpiDriverNuvoton.h"


/*! @brief Device instance number */
#define FSL_SPI_SHARC 1
/*! @brief spi_sharc component mode */
#define SPI_SHARC_DMA_MODE 0U



/*! @brief Driver state structure. Generated due to enabled auto initialization - this is declared in spi_sharc.cpp!!*/
extern dspi_master_state_t spi_sharc_MasterState;

/*! @brief Driver calculated baudrate is returned from DSPI_DRV_MasterInit() call.
    Generated due to enabled auto initialization */
extern uint32_t spi_sharc_calculatedBaudRate;
  
/*! @brief Master bus configuration declaration */
extern const dspi_device_t spi_sharc_bootConfig;
extern const dspi_device_t spi_sharc_hostConfig;
    
/*! @brief Master configuration declaration */
extern const dspi_master_user_config_t spi_sharc_MasterConfig_boot;
extern const dspi_master_user_config_t spi_sharc_MasterConfig_host;

//void SPI0_IRQHandler(void);
void dsp_sharc_SetMode_Host(void);
    
#endif
/* ifndef __spi_sharc_H */
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
