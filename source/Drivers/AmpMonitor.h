/*
 * AmpMonitor.h
 *
 *  Created on: Dec 17, 2015
 *      Author: althaf.ismail
 */

#ifndef SOURCES_AMPMONITOR_H_
#define SOURCES_AMPMONITOR_H_

//#include "fsl_adc16_driver.h"
#include <stdint.h>

#define DEFAULT_TEMP_TARGET			30.0
#define DEFAULT_TEMP_HYSTERESIS		4.0

void ADCInit( void );

//void FanControl( void );
float CalculateTemp( uint16_t adcValue );
int ConvFloatToInt_Dec( float value );
uint16_t ConvFloatToInt_Frac( float value );
void ReportAmpTemperature( void );
float ReadAmpTemperature( uint32_t whichAmp );
//void SetLowTempThreshold( float target, float hysteresis );
//void SetHighTempThreshold( float target, float hysteresis );
//void SetFanControlTempThreshold( float target, float hysteresis );

void EADC_Init();

#endif /* SOURCES_AMPMONITOR_H_ */
