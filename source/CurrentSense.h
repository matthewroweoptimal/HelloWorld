/*
 * current_sense.h
 *
 *  Created on: Dec 8, 2015
 *      Author: Ben.Huber
 */

#ifndef CURRENT_SENSE_H_
#define CURRENT_SENSE_H_

#include "MQX_To_FreeRTOS.h"
//#include "lwevent.h"

extern "C" {
#include "oly.h"
}


#define ADC_START_BYTE			0x01
#define ADC_SINGLE_ENDED		0x80
#define CURRENT_FULLSCALE_COUNT 1023
#define NUM_CURRENT_CHANNELS	3
#define VOLTAGE_FULLSCALE_COUNT 0X7FFFFF
#define OVERVOLTAGE_FRACTION    0.67

//structure to hold current/impedance calibration

typedef struct {
	uint32_t	adc_channel;
	uint32_t	volt_mode;    //bridged or unbridged
} current_sense_config_t;

typedef struct {
	float32		v_fullScale_volts;    //used to calibrate the full scale current
	float32		v_valid_threshold_volts; //ignore readings below this
	float32 	v_over_threshold_volts;
} current_sense_v_cal_t;

typedef struct {
	float32		i_fullScale_amps;    //used to calibrate the full scale current
	uint32_t	i_offset_raw;          //offset (idle noise) may be used to improve accuracy
	float32		i_valid_threshold_amps; //ignore readings below this
} current_sense_i_cal_t;


typedef struct {
	float32 		currentLast;
	bool 		currentRampUp;
	float32		voltageLast;
	bool		voltageRampUp;
} impedance_old_data_t;

typedef struct {
	float32 	openLowerLimit;
	float32		shortUpperLimit;
} current_sense_Z_limits_t;

typedef enum {
	NORMAL = 0x00,
	OPEN = 0x01,
	SHORT = 0x02,
	UNKNOWN = 0xff,
} current_sense_fault_status_t;


#define AVERAGE_WEIGHT_FILTER  64
#define MAX_IMPEDANCE 100.0
#define INVALID_IMPEDANCE 1000.0
#define INITIAL_IMPEDANCE 6.0
typedef struct {
	float32	 impedanceAveraged;
	LOUD_cs_fault	 fault_status;
	float32	 impedance;
	float32	 voltage;
	float32	 current;
	uint32_t	 voltage_raw;
	uint32_t	 current_raw;
} current_sense_Z_result_t;

extern current_sense_Z_result_t current_sense_Z_result[];
extern current_sense_v_cal_t current_sense_v_cal[];
extern current_sense_i_cal_t current_sense_i_cal[];
extern current_sense_Z_limits_t current_sense_Z_limits[];


uint16_t CurrentSenseRawRead(uint8_t chan_num);
bool CurrentSenseProcessVoltage(uint8_t chan_num, uint32_t voltage);
void CurrentSenseCheckImpedance(uint8_t chan_num);
void CurrentSenseInit();

void CurrentSenseFanRelayOn(void);
void CurrentSenseFanRelayOff(void);

#endif /* CURRENT_SENSE_H_ */
