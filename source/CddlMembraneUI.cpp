/*
 * CddlMembraneUI.cpp
 *
 *  Created on: Mar 31 6, 2016
 *      Author: ben.huber
 */


#include "ConfigManager.h"
#include "cddl_leds.h"
#include <stdio.h>
#include <string.h>

extern "C" {
#include "oly.h"
#include "pin.h"
#include "oly_logo.h"
}


namespace oly {

//preset 0 is hidden and should never be invoked directly
int32_t cddl_current_preset = 1;
// cddl has 4 (5) presets, 3(4) factory user presets
// in memory the user presets are ordered:
//                  internal  external preset index
// dummy preset      0          0
// user preset       1          4
// factory preset 1  2          1
// factory preset 2  3          2
// factory preset 3  4          3


void cddlMembraneUI::SW2()
{
#if MFG_TEST_MARTIN   // force UI override for manuf build
	uint32_t valid_indicator = 	0;
#else
	//uint32_t valid_indicator = 	pConfig->GetUserParamsValidIndicator();
	//IQ just checking 4th LED works!
	uint32_t valid_indicator = 1;
#endif
	if(cddl_get_meter_override() == BACKPANEL_LED_MODE){ //kill meter override if it is on instead of UI
		cddl_current_preset++;
		if(cddl_current_preset > NUMBER_OF_PRESETS) cddl_current_preset = 1;
		else if(cddl_current_preset == NUMBER_OF_PRESETS) {  // last preset is user
			if(valid_indicator == 0xFFFFFFFF) {
				cddl_current_preset = 1;
			}
		}
#if MFG_TEST_MARTIN   // force UI override for manuf build
		UpdateUser(cddl_current_preset);
		if(cddl_current_preset == 2)
			cddl_show_network_status(1, 0);
		else
			cddl_show_network_status(0, 0);

		if(cddl_current_preset == 3)
			GPIO_DRV_WritePinOutput(FAN_CONTROL, 1);
		else
			GPIO_DRV_WritePinOutput(FAN_CONTROL, 0);

		if(cddl_current_preset == 4)
			//LogoSetState(LOGO_ON_RED);
			GPIO_DRV_WritePinOutput(LOGO_ON_RED, 0);
		else
			//LogoSetState(LOGO_OFF);
			GPIO_DRV_WritePinOutput(LOGO_ON_RED, 1);


#else
		pConfig->SetActivePreset(eTARGET_USER, cddl_current_preset);
#endif
	}
	else {
		cddl_set_meter_override(BACKPANEL_LED_MODE);
	}
}

void cddlMembraneUI::SW2_long()
{
	if(cddl_get_meter_override() == BACKPANEL_LED_MODE)  //toggle override
		cddl_set_meter_override(BACKPANEL_LED_METER);
	else
		cddl_set_meter_override(BACKPANEL_LED_MODE);
}

void cddlMembraneUI::SW2_long_long()
{
	cddl_set_meter_override(BACKPANEL_LED_IP_RESET);
	pConfig->SwitchToDHCP();

	//need to flash leds?
	//cddlMembraneUI::SW2();  //we triggered meter mode, so now we need to go back to normal mode after extra long press
}

void cddlMembraneUI::UpdateUser(uint32_t instance)
{
	cddl_current_preset = instance;
	//instance 0 refers to internal user preset (hidden)
	if(instance > 0) {
		cddl_show_preset(cddl_current_preset, 0);
	}
	else {
		cddl_show_preset(cddl_current_preset, CDDL_BLINK_PERIOD);
	}
}


//remap user presets to actual memory: F1, F2, F3, (F4), U1

uint32_t cddlMembraneUI::cddlRemapUser(uint32_t instance)
{
	uint32_t user_preset_remap = instance;
	if(instance == (USER_PRESETS - 1))
		user_preset_remap = 1;
	else if(instance != 0)
		user_preset_remap = instance + 1;
	else
		user_preset_remap = 0;
	return user_preset_remap;
}

void cddlMembraneUI::UpdateInputSource(oly_audio_source_t source)
{
       if (source == src_dante)
    	   cddl_show_network_status(1, 0); //turn on led
       else
    	   cddl_show_network_status(0, 0); // Turn LED off
	   	   //cddl_show_network_status(1, CDDL_NETWORK_BLINK_PERIOD); //turn on blinking network led for test
} /* namespace oly */

cddlMembraneUI::cddlMembraneUI(Config *config)
{
	printf("\nCDDP UI  ");
	pConfig = config;
	cddl_current_preset = 1;
	cddl_show_LEDs(0); //turn off LEDs
	cddl_show_network_status(1, CDDL_NETWORK_BLINK_PERIOD); //turn on blinking network led for test

}

}
