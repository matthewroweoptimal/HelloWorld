/*
 * AmpMonitor.c
 *
 *  Created on: Dec 17, 2015
 *      Author: althaf.ismail
 */

#include "AmpMonitor.h"
#include <stdio.h>
#include "oly.h"
#include "pin.h"


/* Global variable declaration */
volatile uint32_t g_u32AdcIntFlag;//, g_u32COVNUMFlag = 0;
uint32_t  i32ConversionData;

static float hiTempThreshold;
static float loTempThreshold;

const float celciusPerVolt = 30.303f;

//const float adcPerVolt = 1241.2f;
const float adcPerVolt = 1321.29f;	//change for 3.1V rail change from 3.3V


#if 0 //IQ comment out - not needed for Nuvoton
const adc16_converter_config_t amp1_mon_hw_config_0 =
{
		.lowPowerEnable = true,
		.clkDividerMode = kAdc16ClkDividerOf8,
		.longSampleTimeEnable = false,
		.resolution = kAdc16ResolutionBitOfSingleEndAs16,
		.clkSrc = kAdc16ClkSrcOfBusClk,
		.asyncClkEnable = true,
		.highSpeedEnable = false,
		.longSampleCycleMode = kAdc16LongSampleCycleOf24,
		.hwTriggerEnable = false,
		.refVoltSrc = kAdc16RefVoltSrcOfVref,
		.continuousConvEnable = false,
		.dmaEnable = false
};

const adc16_converter_config_t amp2_mon_hw_config_0 =
{
		.lowPowerEnable = true,
		.clkDividerMode = kAdc16ClkDividerOf8,
		.longSampleTimeEnable = false,
		.resolution = kAdc16ResolutionBitOfSingleEndAs16,
		.clkSrc = kAdc16ClkSrcOfBusClk,
		.asyncClkEnable = true,
		.highSpeedEnable = false,
		.longSampleCycleMode = kAdc16LongSampleCycleOf24,
		.hwTriggerEnable = false,
		.refVoltSrc = kAdc16RefVoltSrcOfVref,
		.continuousConvEnable = false,
		.dmaEnable = false
};

const adc16_hw_average_config_t amp1_mon_avg_config_0 =
{
		.hwAverageEnable = true,
		.hwAverageCountMode = kAdc16HwAverageCountOf32
};

const adc16_hw_average_config_t amp2_mon_avg_config_0 =
{
		.hwAverageEnable = true,
		.hwAverageCountMode = kAdc16HwAverageCountOf32
};

const adc16_chn_config_t amp1_mon_ch_config_0 =
{
		.chnIdx = kAdc16Chn23,
		.convCompletedIntEnable = false,
		.diffConvEnable = false
};

const adc16_chn_config_t amp2_mon_ch_config_0 =
{
		.chnIdx = kAdc16Chn23,
		.convCompletedIntEnable = false,
		.diffConvEnable = false
};

adc16_calibration_param_t amp1_mon_cal_config_0;
adc16_calibration_param_t amp2_mon_cal_config_0;

static float hiTempThreshold;
static float loTempThreshold;

const float celciusPerVolt = 30.303f;

const float adcPerVolt = 19859.4f;



void ADCInit( void )
{
    ADC16_DRV_Init(0, &amp1_mon_hw_config_0);
    ADC16_DRV_Init(1, &amp2_mon_hw_config_0);

    ADC16_DRV_ConfigHwAverage(0, &amp1_mon_avg_config_0);
    ADC16_DRV_ConfigHwAverage(1, &amp2_mon_avg_config_0);

    ADC16_DRV_GetAutoCalibrationParam(0, &amp1_mon_cal_config_0);
    ADC16_DRV_GetAutoCalibrationParam(1, &amp2_mon_cal_config_0);

    ADC16_DRV_SetCalibrationParam(0, &amp1_mon_cal_config_0);
    ADC16_DRV_SetCalibrationParam(1, &amp2_mon_cal_config_0);

//    SetLowTempThreshold(DEFAULT_TEMP_TARGET, DEFAULT_TEMP_HYSTERESIS);
//    SetHighTempThreshold(DEFAULT_TEMP_TARGET, DEFAULT_TEMP_HYSTERESIS);
}
#endif

void EADC_Init()
{
    /* Set input mode as single-end and enable the A/D converter */
    EADC_Open(EADC, EADC_CTL_DIFFEN_SINGLE_END);

    /* Configure the sample module 0 for analog input channel 2 and software trigger source.*/
    EADC_ConfigSampleModule(EADC, 0, EADC_SOFTWARE_TRIGGER, 0);
    /* Configure the sample module 0 for analog input channel 2 and software trigger source.*/
    EADC_ConfigSampleModule(EADC, 1, EADC_SOFTWARE_TRIGGER, 1);

    /* Clear the A/D ADINT0 interrupt flag for safe */
    EADC_CLR_INT_FLAG(EADC, EADC_STATUS2_ADIF0_Msk);
}



float ReadAmpTemperature( uint32_t whichAmp )
{
	uint16_t adc_value;
	float temp;

    /* Software trigger ADC to start converting */
    EADC_START_CONV(EADC, BIT1|BIT0);
    /* Wait ADC convertion complete */
    while(EADC_IS_BUSY(EADC));
    /* Get the conversion result of the sample module 0 */
    adc_value = uint16_t EADC_GET_CONV_DATA(EADC, whichAmp-1);
    temp = CalculateTemp(adc_value);
    //printf("Conversion result of temp, amp %d: 0x%X (%d) Actual Temp= %f\n\n", whichAmp, adc_value, adc_value, temp);
    return temp;

}

float CalculateTemp( uint16_t adcValue )
{
	float temp;
	temp = (float) adcValue * celciusPerVolt / adcPerVolt;
	return temp;
}

int ConvFloatToInt_Dec( float value )
{
	return (int) value;		//TODO:  Should still do rounding here not just cast
}

uint16_t ConvFloatToInt_Frac( float value )
{
	uint32_t temp;
	value -= ConvFloatToInt_Dec( value );
	if( value < 0 )	value = value * -1;
	value = value*1000;
	temp = (uint32_t) value;
	return temp;
}


void ReportAmpTemperature( void )
{
	printf("Amp1 Temp=%i.%03iC\t", ConvFloatToInt_Dec(ReadAmpTemperature(1)), ConvFloatToInt_Frac(ReadAmpTemperature(1)) );
	printf("Amp2 Temp=%i.%03iC\n", ConvFloatToInt_Dec(ReadAmpTemperature(2)), ConvFloatToInt_Frac(ReadAmpTemperature(2)) );
}

void SetLowTempThreshold( float target, float hysteresis )
{
	loTempThreshold = target - (hysteresis/2);
}



void SetHighTempThreshold( float target, float hysteresis )
{
	hiTempThreshold = target + (hysteresis/2);;
}



void SetFanControlTempThreshold( float target, float hysteresis )
{
	loTempThreshold = target - (hysteresis/2);
	hiTempThreshold = target + (hysteresis/2);;
}
