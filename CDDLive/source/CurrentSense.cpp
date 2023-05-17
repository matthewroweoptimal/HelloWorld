/*
 * currentSense.cpp
 *
 *  Created on: Dec 8, 2015
 *      Author: Ben.Huber
 */

#include "CurrentSense.h"
#include "spi_aux_csense.h"
#include "pin.h"

extern "C" {
#include "oly.h"
}



//{i_fullScale_amps, i_offset_raw, i_valid_threshold_amps, v_fullScale_volt, v_valid_threshold_volts}

//current channel 0 is J3-p3,4  (high gain, no bridge)
//        channel 1 is J3,p1,2  (low gain, bridged allowd)
//        channel 2 is j4, p1,2 (low gain, bridged allowd)
current_sense_config_t current_sense_config[NUM_CURRENT_CHANNELS] = {
#if AMP1_BRIDGED
		{1, 1},  // LF current channel, voltage type (bridged/non_bridged)
#else
		{1, 0},
#endif
		{2, 1},   // MF  -- use as 2nd LF for now
#if HF_CURRENT_HIGHGAIN
		{0, 0}    // HF
#elif AMP2_BRIDGED
		{2, 1}    // LF
#else
		{2, 0}    // HF
#endif
};

current_sense_v_cal_t current_sense_v_cal[2] = {
		{92,1.0, 65.0},
		{184,2.0, 130.0}
};

current_sense_i_cal_t current_sense_i_cal[NUM_CURRENT_CHANNELS] = {
		{3.6,12,3.6/100.},
		{15.0,9,15.0/100.},
		{15.0,9,15.0/100.}
};

//{openLowerLimit, shortUpperLimit} in Ohms
current_sense_Z_limits_t current_sense_Z_limits[NUM_CURRENT_CHANNELS] = {
		{16.0, 2.0},
		{16.0, 2.0},
		{16.0, 2.0},
};

LWEVENT_STRUCT current_sense_event;

current_sense_Z_result_t  current_sense_Z_result[NUM_CURRENT_CHANNELS] = {
		{INITIAL_IMPEDANCE, eCS_FAULT_NONE, INVALID_IMPEDANCE, 0.0,0.0,0,0},
		{INITIAL_IMPEDANCE, eCS_FAULT_NONE, INVALID_IMPEDANCE, 0.0,0.0,0,0},
		{INITIAL_IMPEDANCE, eCS_FAULT_NONE, INVALID_IMPEDANCE, 0.0,0.0,0,0},
};

impedance_old_data_t  impedance_old_data[NUM_CURRENT_CHANNELS];


/* Routine to read a current ADC channel on the current sense board
 *
 * For MCP3008 AtoD Protocol is 3 bytes full duplex:
 * TX
 * bit 0-6 = x
 * bit 7 = 1, start
 * bit 8,9,10 is channel number,  MSB first
 * bit 11-23 = x
 *
 * RX
 * bit 0-10  = x
 * bit 11-23 = ADC value,  MSB first
 */
uint16_t CurrentSenseRawRead(uint8_t chan_num){
	uint8_t result[3];
	uint8_t send_data[3];

	//GPIO_DRV_ClearPinOutput(IMON_SPI_SS); AUX SPI on for c sense. Do not need to manually control SS
	send_data[0] = ADC_START_BYTE;
	send_data[1] = ADC_SINGLE_ENDED | ((chan_num & 0x07) << 4);
	send_data[2] = 0;
	DSPI_DRV_MasterTransferBlocking(FSL_SPI_AUX,send_data,result,sizeof(send_data),1000);

	//GPIO_DRV_SetPinOutput(IMON_SPI_SS); AUX SPI on for c sense. Do not need to manually control SS

	return ((result[1] & 0x03) << 8 | result[2]);
}

/* calibrate the raw current value (peak held, 10 bit ADC) */
float32 calibrate_current(uint8_t chan_num, uint32_t current_raw){
	float32 temp;
	float32 current;
	if(chan_num >= NUM_CURRENT_CHANNELS) return (0);
	temp = (float32)((int32_t)current_raw - (int32_t)current_sense_i_cal[chan_num].i_offset_raw);
	if(temp < 0.0) temp = 0.0;
	current = (temp*current_sense_i_cal[chan_num].i_fullScale_amps) / (float32)CURRENT_FULLSCALE_COUNT;
	return (current);
}
/* calibrate the raw voltage value (24 bit signed value from the DAC)*/
float32 calibrate_voltage(uint8_t chan_num, uint32_t voltage_raw){
	float32 voltage;
	if(chan_num >= 2) return (0);
	voltage = ((float32)voltage_raw*current_sense_v_cal[chan_num].v_fullScale_volts) / (float32)VOLTAGE_FULLSCALE_COUNT;
	return (voltage);
}


void CurrentSenseInit(){
	uint32_t i;
	for(i=0; i<NUM_CURRENT_CHANNELS; i++){
		current_sense_Z_result[i].fault_status = eCS_FAULT_INVALID;
		current_sense_Z_result[i].impedanceAveraged = INVALID_IMPEDANCE;
		current_sense_Z_result[i].impedance = INVALID_IMPEDANCE;
		current_sense_Z_result[i].voltage = 0.0;
		current_sense_Z_result[i].current = 0.0;
	}
	return;
}
/*Process the voltage that arrives from the DSP DAC
 *
 * Use the arrival of the voltage to drive the impedance calculation
 *
 * It is important that the voltage is determined at 30ms or faster intervals for good operation
 * since the peak hold on the current has a 100ms RC time constant
 *
 * Impedance measurement core operation:
 * a peak detected current is digitized every 30ms or faster (if possible) after the voltage (DAC output value) is determined.
 * since the voltage is sent from DSP via SPI, the value is old and any current measured after the voltage data
 * arrives on the host will lag the voltage.
 * The stream of voltage and current values are calibrated and stored. The code then looks for a peak in the voltage
 * by looking for the point where the voltage gets smaller using a flag for the slope. Once a rising slope on voltage
 * has been detected followed by a decrease in the voltage, the current is  calculated by
 * using the pre decrease voltage and current values.
 * the algorithm is intended to calculate the impedance at the voltage peaks
 * returns true if the impedance was recalculated*/
uint32_t currentDebug_print_counter =0;
uint32_t impedance_startup_reading_counter =0;
bool CurrentSenseProcessVoltage(uint8_t chan_num, uint32_t voltage_raw){
	float32 temp, current, voltage, impedance;
	uint32_t current_raw, volt_mode, itemp;
	bool ret =  FALSE;
	//if(chan_num >= NUM_CURRENT_CHANNELS) ret;
	//read current channel. Assume this data will have any current peak from a voltage peak in it since we read it after voltage
	current_raw = CurrentSenseRawRead(current_sense_config[chan_num].adc_channel);

	current = calibrate_current(current_sense_config[chan_num].adc_channel, current_raw);
	volt_mode = current_sense_config[chan_num].volt_mode;
	voltage = calibrate_voltage(volt_mode, voltage_raw);
	//raw debug storage - could be removed
	current_sense_Z_result[chan_num].current_raw = current_raw;
	current_sense_Z_result[chan_num].voltage_raw = voltage_raw;
	if((impedance_startup_reading_counter > 200) && //make sure we do not use initial readings
		(impedance_old_data[chan_num].voltageLast > voltage) && //if we are ramping down
		(impedance_old_data[chan_num].voltageRampUp  == 1) &&//and we just ramped up
		(impedance_old_data[chan_num].voltageLast > current_sense_v_cal[volt_mode].v_valid_threshold_volts) &&  //make sure signal large enough then -calc impedance
		(impedance_old_data[chan_num].voltageLast < current_sense_v_cal[volt_mode].v_over_threshold_volts)) //make sure we do not engage amp limiters
		{
		//use actual peak for voltage, which is last value and store for analysis
		current_sense_Z_result[chan_num].current = impedance_old_data[chan_num].currentLast;
		current_sense_Z_result[chan_num].voltage = impedance_old_data[chan_num].voltageLast;

		//fetch last current and make sure it is not too small
		temp = impedance_old_data[chan_num].currentLast;
		// clip impedance to MAX_IMPEDANCE
		if(temp == 0.0) {
			impedance =  MAX_IMPEDANCE;
		}
		else {
			impedance = (impedance_old_data[chan_num].voltageLast)/temp;
			if(impedance >  MAX_IMPEDANCE) impedance =  MAX_IMPEDANCE;
		}
		//check for first valid reading
		if(current_sense_Z_result[chan_num].impedance == INVALID_IMPEDANCE){
			current_sense_Z_result[chan_num].impedanceAveraged = impedance;
		}
		else{
			current_sense_Z_result[chan_num].impedanceAveraged =
					(current_sense_Z_result[chan_num].impedanceAveraged*(AVERAGE_WEIGHT_FILTER-1) + impedance)/AVERAGE_WEIGHT_FILTER;

		}
		current_sense_Z_result[chan_num].impedance = impedance;
		itemp = impedance*100.0;
		CurrentSenseCheckImpedance(chan_num);
#if 0 // wuit print for debugging
		if((currentDebug_print_counter%5) == 0) {
			printf("CS ch=%d adc=%d mode=%d  I=%d V=%d Z=%d\n", chan_num,
					current_sense_config[chan_num].adc_channel, volt_mode, current_raw, voltage_raw, itemp);
		}
		currentDebug_print_counter++;
#endif
		ret = TRUE;
	}
	impedance_startup_reading_counter++;
	if(impedance_old_data[chan_num].currentLast > current) impedance_old_data[chan_num].currentRampUp = 0;  //going down now
	if(impedance_old_data[chan_num].currentLast < current) impedance_old_data[chan_num].currentRampUp = 1;  //going down now
	if(impedance_old_data[chan_num].voltageLast > voltage) impedance_old_data[chan_num].voltageRampUp = 0;  //going down now
	if(impedance_old_data[chan_num].voltageLast < voltage) impedance_old_data[chan_num].voltageRampUp = 1;  //going down now
	impedance_old_data[chan_num].currentLast = current;
	impedance_old_data[chan_num].voltageLast = voltage;
	return ret;
}


void CurrentSenseCheckImpedance(uint8_t chan_num){
	if(chan_num >= NUM_CURRENT_CHANNELS) return;
	if(current_sense_Z_result[chan_num].impedanceAveraged < current_sense_Z_limits[chan_num].shortUpperLimit)
		current_sense_Z_result[chan_num].fault_status = eCS_FAULT_SHORT;
	else if(current_sense_Z_result[chan_num].impedanceAveraged > current_sense_Z_limits[chan_num].openLowerLimit)
		current_sense_Z_result[chan_num].fault_status = eCS_FAULT_OPEN;
	else
		current_sense_Z_result[chan_num].fault_status = eCS_FAULT_NONE;
}

void CurrentSenseFanRelayOn(void){
	GPIO_DRV_SetPinOutput(FAN_CONTROL);
}

void CurrentSenseFanRelayOff(void){
	GPIO_DRV_ClearPinOutput(FAN_CONTROL);
}


