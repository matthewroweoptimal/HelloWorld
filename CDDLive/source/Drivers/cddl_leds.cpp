/*
 * cddl_leds.c
 *
 *  Created on: April 4, 2016
 *      Author: ben.huber
 *
 *	This covers cddl LED functions, driving the polled blink logic
 */

#include "oly.h"
#include "pin.h"
#include "cddl_leds.h"
#include <stdio.h>

const cddl_ui_preset_led_Sate ui_preset_lookup[] = {CDDL_PRESET1, CDDL_PRESET2, CDDL_PRESET3, CDDL_PRESET4, CDDL_PRESET5, CDDL_PRESETall};
const cddl_ui_meter_led_Sate ui_meter_lookup[] = {CDDL_LED1, CDDL_LED2, CDDL_LED3, CDDL_LED4};


uint32_t cddl_led_shadow=0;
uint32_t cddl_meter_led_shadow=0;
uint32_t cddl_network_led_shadow=0;
uint32_t cddl_blinkTimer = 0;
uint32_t cddl_blinkRate = 0;
uint32_t cddl_network_blinkRate = 0;
uint32_t cddl_enable_meter_override = BACKPANEL_LED_MODE;

CURRENT_LED_STATE storedLedState = {BACKPANEL_LED_MODE, 0x00};

void cddl_show_preset(int32_t current_preset, uint32_t blink_rate)
{
	uint32_t temp_mask;
	if(current_preset > NUMBER_OF_PRESETS) return;
	if(current_preset == 0) temp_mask =  CDDL_PRESETedit;
	else temp_mask = ui_preset_lookup[current_preset-1];
	cddl_show_LEDs(temp_mask);
	cddl_led_shadow = temp_mask;
	cddl_blinkRate = blink_rate;
}

void cddl_show_meter(float db)
{
	uint32_t temp_mask;
	if (db == METER_THRESH_LIMIT) { //only used 3 leds for meter, 4th is for limit
		temp_mask = ui_meter_lookup[3];
	} else if (db >= METER_THRESH_3) {
		temp_mask = ui_meter_lookup[2];
	} else if (db >= METER_THRESH_2) {
		temp_mask = ui_meter_lookup[1];
	} else if (db >= METER_THRESH_1) {
		temp_mask = ui_meter_lookup[0];
	} else {
		temp_mask = 0;
	}
	cddl_meter_led_shadow = temp_mask;
}

void cddl_set_meter_override(uint32_t override)
{
	cddl_enable_meter_override = override;
	storedLedState.overrideState = override;
}

uint32_t cddl_get_meter_override()
{
	return cddl_enable_meter_override;
}

void cddl_show_network_status(int32_t status, uint32_t blink_rate)
{

	cddl_show_network_LED(status);
	cddl_network_led_shadow = status;
	cddl_network_blinkRate = blink_rate;
}

//call this periodically to drive the blink logic
void cddl_poll_led_blink(int ms)
{
	cddl_blinkTimer+=ms;
	if(cddl_enable_meter_override == BACKPANEL_LED_MODE) {
		if(cddl_blinkRate != 0) {
			if (((cddl_blinkTimer/cddl_blinkRate) % 2) == 0) {
				cddl_show_LEDs(0); //off
			}
			else {
				cddl_show_LEDs(cddl_led_shadow);  //on
			}
		}
		else
			cddl_show_LEDs(cddl_led_shadow);  //always update
	}
	else if(cddl_enable_meter_override == BACKPANEL_LED_METER) {
		cddl_show_LEDs(cddl_meter_led_shadow);  //override with meter if meter data is valid
	}
	else
		cddl_show_LEDs(CDDL_LED4);  //override with meter if meter data is valid

	if(cddl_network_blinkRate != 0) {
		if (((cddl_blinkTimer/cddl_network_blinkRate) % 2) == 0) {
			cddl_show_network_LED(0); //off
		}
		else {
			cddl_show_network_LED(cddl_network_led_shadow);  //on
		}
	}

}

//---------------------------------------------------------------------------
// Set the next firmware upgrade LED indication
//---------------------------------------------------------------------------
static uint32_t m_fwUpdateLedPattern = 0x01;
static bool bFwUpdateInProgress = false;
void cddl_showNextFwUpdateLedPattern()
{
	bFwUpdateInProgress = true;
	if(m_fwUpdateLedPattern & 1) CDDP_PANEL_LED1(1); else CDDP_PANEL_LED1(0);
	if(m_fwUpdateLedPattern & 2) CDDP_PANEL_LED2(1); else CDDP_PANEL_LED2(0);
	if(m_fwUpdateLedPattern & 4) CDDP_PANEL_LED3(1); else CDDP_PANEL_LED3(0);
	if(m_fwUpdateLedPattern & 8) CDDP_PANEL_LED4(1); else CDDP_PANEL_LED4(0);

	m_fwUpdateLedPattern = m_fwUpdateLedPattern << 1;
	if (m_fwUpdateLedPattern == (1 << 4))
		m_fwUpdateLedPattern = 0x01;	// Back to LED1
}

void cddl_show_LEDs(uint32_t led_mask)
{	// Only update with this function if not in firmware update mode
	if ( !bFwUpdateInProgress )
	{	// This mapping is inefficient but universal:
		if(led_mask & 1) CDDP_PANEL_LED1(1); else CDDP_PANEL_LED1(0);
		if(led_mask & 2) CDDP_PANEL_LED2(1); else CDDP_PANEL_LED2(0);
		if(led_mask & 4) CDDP_PANEL_LED3(1); else CDDP_PANEL_LED3(0);
		if(led_mask & 8) CDDP_PANEL_LED4(1); else CDDP_PANEL_LED4(0);
		storedLedState.ledMask = led_mask;
	}
}

void cddl_getLedStatePriorToFwUpdate( CURRENT_LED_STATE &ledState )
{	// Restore the specified condition to the LEDs
	ledState.overrideState = storedLedState.overrideState;
	ledState.ledMask = storedLedState.ledMask;

}

void cddl_restoreLedState( CURRENT_LED_STATE ledState )
{	// Restore the specified condition to the LEDs
	cddl_enable_meter_override = ledState.overrideState;
	cddl_show_LEDs( ledState.ledMask );
	bFwUpdateInProgress = false;
}

void cddl_show_network_LED(uint32_t led_mask)
{
	if(led_mask) CDDP_PANEL_LED_NETWORK(1); else CDDP_PANEL_LED_NETWORK(0);
}

uint32_t standby_status = 0;
uint32_t standby_timer = 0;
void cddl_standby_meter_trigger() {
	if(standby_status == 1) {
		GPIO_DRV_WritePinOutput(AMP1_2_STANDBY_CNTRL, 0);
		standby_status = 0;
		printf("CDDL: Leave STANDBY mode\n");
	}
	standby_timer = 0;
}


// cddl_standby_500ms_timer_callback()
// This drives the standby oneshot timer
//This could be done with a retrigger timer from OS. But do not want to make OS call for each meter trigger.
//Just use simple available timer, which is the life LED timer.
//This routine is called from that timer and drives the standby.
//Note the standby timer is 32 bit, so it should never overflow during normal use.
void cddl_standby_500ms_timer_callback() {
	if(standby_status == 0){
		standby_timer++;
		if(standby_timer == STANDBY_NUMBER_TICKS)
		{
			GPIO_DRV_WritePinOutput(AMP1_2_STANDBY_CNTRL, 1);
			standby_status = 1;
			printf("CDDL: Enter STANDBY mode\n");
		}
	}
}

