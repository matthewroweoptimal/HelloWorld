/*
 * cddl_leds.h
 *
 *  Created on: April 1, 2016
 *      Author: ben.huber
 */

#ifndef _CDDLLEDS_H_
#define _CDDLLEDS_H_

#include "Model_Info.h"

#define CDDP_PANEL_LED1(value)		(GPIO_DRV_WritePinOutput(DISPLAY_D2, ! value))
#define CDDP_PANEL_LED2(value)		(GPIO_DRV_WritePinOutput(DISPLAY_D3, ! value))
#define CDDP_PANEL_LED3(value)		(GPIO_DRV_WritePinOutput(DISPLAY_D4, ! value))
#define CDDP_PANEL_LED4(value)		(GPIO_DRV_WritePinOutput(DISPLAY_D5, ! value))
#define CDDP_PANEL_LED_NETWORK(value)		(GPIO_DRV_WritePinOutput(DISPLAY_D6, ! value))

#define METER_THRESHOLD_STANDBY  (-78.)  //this level will  retrigger threshold timer
#define METER_THRESHOLD_STANDBY_DISABLE  (-90.)  //this level will disable the standby system - set at 100.0 for disable
#define METER_THRESH_1    (-70.)     // db value greater than this turns on 1 LED
#define METER_THRESH_2    (-35.)	// db value greater than this turns on 2 LED
#define METER_THRESH_3    (-25.)	// db value greater than this turns on 3 LED
#define METER_THRESH_4    (-10.)  	//<-- not used
#define METER_THRESH_LIMIT  (0.0)	// Not used as level, used as flag to activate limit LED
#define STANDBY_NUMBER_MINUTES 10
#define STANDBY_NUMBER_TICKS (STANDBY_NUMBER_MINUTES*60*2)

#ifdef USER_PRESETS
#define NUMBER_OF_PRESETS  (USER_PRESETS-1)
#else
#define NUMBER_OF_PRESETS 5
#endif


#if NUMBER_OF_PRESETS == 5
typedef enum
{
	CDDL_PRESET1 = 0x01,
	CDDL_PRESET2 = 0x02,
	CDDL_PRESET3 = 0x05,
	CDDL_PRESET4 = 0x06,
	CDDL_PRESET5 = 0x08,
	CDDL_PRESETall= 0x0f,
	CDDL_PRESETedit= 0x00
} cddl_ui_preset_led_Sate;
#else

typedef enum
{
	CDDL_PRESET1 = 0x01,
	CDDL_PRESET2 = 0x02,
	CDDL_PRESET3 = 0x04,
	CDDL_PRESET4 = 0x08,
	CDDL_PRESET5 = 0x07, //dummy
	CDDL_PRESETall = 0x0F,
	CDDL_PRESETedit= 0x00
} cddl_ui_preset_led_Sate;
#endif

typedef enum
{
	CDDL_LED1 = 0x01,
	CDDL_LED2 = 0x03,
	CDDL_LED3 = 0x07,
	CDDL_LED4 = 0x0f,
} cddl_ui_meter_led_Sate;
#define BACKPANEL_LED_MODE  0
#define BACKPANEL_LED_METER  1
#define BACKPANEL_LED_IP_RESET  2

#define CDDL_BLINK_PERIOD 500
#define CDDL_NETWORK_BLINK_PERIOD 200

typedef struct
{
	uint32_t  overrideState;
	uint32_t  ledMask;
} CURRENT_LED_STATE;

void cddl_show_preset(int32_t current_preset, uint32_t blink_rate);
void cddl_show_LEDs(uint32_t led_mask);
void cddl_show_meter(float db);

void cddl_show_network_status(int32_t status, uint32_t blink_rate);
void cddl_show_network_LED(uint32_t led_mask);

void cddl_poll_led_blink(int ms);
void cddl_blinkLed(int period, uint32_t led);
void cddl_set_meter_override(uint32_t override);
uint32_t cddl_get_meter_override();
void cddl_standby_500ms_timer_callback();
void cddl_standby_meter_trigger();

void cddl_showNextFwUpdateLedPattern( bool bClear );
void cddl_getLedStatePriorToFwUpdate( CURRENT_LED_STATE &ledState );
void cddl_restoreLedState( CURRENT_LED_STATE ledState );


#endif /* _CDDLLEDS_H_ */
