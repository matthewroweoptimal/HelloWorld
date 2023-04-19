
#ifndef __spi_aux_csense_H
#define __spi_aux_csense_H
/* MODULE spi_aux_csense.h */

#include "SpiDriverNuvoton.h"


/*! @brief Device instance number */
#define FSL_SPI_AUX 2

/*! @brief Driver state structure. Generated due to enabled auto initialization - this is declared in spi_sharc.cpp!!*/
extern dspi_master_state_t spi_aux_csense_MasterState;

/*! @brief Driver calculated baudrate is returned from DSPI_DRV_MasterInit() call.
    Generated due to enabled auto initialization */
extern uint32_t spi_aux_csense_calculatedBaudRate;
  

/*! @brief Master bus configuration declaration */
extern const dspi_device_t spi_aux_BusConfig0;
    
/*! @brief Master configuration declaration */
extern const dspi_master_user_config_t spi_aux_MasterConfig0;

    
#endif
/* ifndef __spi_aux_csense_H */
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
