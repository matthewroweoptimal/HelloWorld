/*
 * oly_logo.c
 *
 *  Created on: Aug 19, 2015
 *      Author: Kris.Simonsen
 *
 *	This covers LED functions including timing
 */

#include "oly.h"
#include "oly_logo.h"
//#include "gpio_pins.h"
#include "pin.h"
#include "LOUD_enums.h"

static oly_Logo_State_t LogoState = LOGO_OFF;
static oly_Logo_State_t LastLogoState = LOGO_OFF;

static uint32_t blinkCounter = 0;
static uint32_t	animationCounter = 0;
static bool 	blinkOn = false;
static uint32_t	blinkPeriod = 0;
static bool		animating = false;

const oly_Logo_State_t logoDemoSeq[6] = {LOGO_OFF, LOGO_ON_WHITE, LOGO_OFF, LOGO_ON_AMBER, LOGO_OFF, LOGO_ON_RED};

int8_t logoDemoIdx = 0;
int8_t logoMfgDemoIdx = 0;

void LogoSetState(oly_Logo_State_t state)
{
	LogoState = state;
	switch (state) {
		case (LOGO_OFF):
			GPIO_DRV_ClearPinOutput(LOGO_LED_AMBER);
			GPIO_DRV_ClearPinOutput(LOGO_LED_WHITE);
			GPIO_DRV_ClearPinOutput(LOGO_LED_RED);
			blinkPeriod = 0;
			break;
		case (LOGO_ON_RED):
			GPIO_DRV_ClearPinOutput(LOGO_LED_AMBER);
			GPIO_DRV_ClearPinOutput(LOGO_LED_WHITE);
			GPIO_DRV_SetPinOutput(LOGO_LED_RED);
			blinkPeriod = 0;
			break;
		case (LOGO_ON_WHITE):
			GPIO_DRV_ClearPinOutput(LOGO_LED_AMBER);
			GPIO_DRV_SetPinOutput(LOGO_LED_WHITE);
			GPIO_DRV_ClearPinOutput(LOGO_LED_RED);
			blinkPeriod = 0;
			break;
		case (LOGO_ON_AMBER):
			GPIO_DRV_SetPinOutput(LOGO_LED_AMBER);
			GPIO_DRV_ClearPinOutput(LOGO_LED_WHITE);
			GPIO_DRV_ClearPinOutput(LOGO_LED_RED);
			blinkPeriod = 0;
			break;
		case (LOGO_SLOW_RED):
			GPIO_DRV_ClearPinOutput(LOGO_LED_AMBER);
			GPIO_DRV_ClearPinOutput(LOGO_LED_WHITE);
			GPIO_DRV_WritePinOutput(LOGO_LED_RED, blinkOn);
			blinkPeriod = LOGO_SLOW_MS;
			break;
		case (LOGO_SLOW_WHITE):
			GPIO_DRV_ClearPinOutput(LOGO_LED_AMBER);
			GPIO_DRV_ClearPinOutput(LOGO_LED_RED);
			GPIO_DRV_WritePinOutput(LOGO_LED_WHITE, blinkOn);
			blinkPeriod = LOGO_SLOW_MS;
			break;
		case (LOGO_SLOW_AMBER):
			GPIO_DRV_ClearPinOutput(LOGO_LED_WHITE);
			GPIO_DRV_ClearPinOutput(LOGO_LED_RED);
			GPIO_DRV_WritePinOutput(LOGO_LED_AMBER, blinkOn);
			blinkPeriod = LOGO_SLOW_MS;
			break;
		case (LOGO_FAST_RED):
			GPIO_DRV_ClearPinOutput(LOGO_LED_AMBER);
			GPIO_DRV_ClearPinOutput(LOGO_LED_WHITE);
			GPIO_DRV_WritePinOutput(LOGO_LED_RED, blinkOn);
			blinkPeriod = LOGO_FAST_MS;
			break;
		case (LOGO_FAST_WHITE):
			GPIO_DRV_ClearPinOutput(LOGO_LED_AMBER);
			GPIO_DRV_ClearPinOutput(LOGO_LED_RED);
			GPIO_DRV_WritePinOutput(LOGO_LED_WHITE, blinkOn);
			blinkPeriod = LOGO_FAST_MS;
			break;
		case (LOGO_FAST_AMBER):
			GPIO_DRV_ClearPinOutput(LOGO_LED_WHITE);
			GPIO_DRV_ClearPinOutput(LOGO_LED_RED);
			GPIO_DRV_WritePinOutput(LOGO_LED_AMBER, blinkOn);
			blinkPeriod = LOGO_FAST_MS;
			break;
	}

	if (LogoState != LastLogoState){	
		LastLogoState = LogoState;
		blinkOn = false;
		blinkCounter = 0;
	}
}

void LogoUpdateRunState(oly_Logo_Run_State_t run_state, LOUD_logo_mode user_mode)
{
	switch(user_mode) {
	case eLOGO_MODE_ON:
		LogoSetState(USER_MODE_ON[run_state]);
		break;
	case eLOGO_MODE_ON_LIMITING:
		LogoSetState(USER_MODE_ON_LIMITING[run_state]);
		break;
	case eLOGO_MODE_OFF:
		LogoSetState(USER_MODE_OFF[run_state]);
		break;
	case eLOGO_MODE_FAULT_ONLY:
		LogoSetState(USER_MODE_FAULT_ONLY[run_state]);
		break;
	case eLOGO_MODE_SPECIAL:
		break;
	}

}

oly_Logo_State_t LogoGetState()
{
	return LogoState;
}

void poll_logo(int ms)
{
	if (animating)
	{
		animationCounter+=ms;
	}
	if (blinkPeriod)
	{
		blinkCounter+=ms;
		if (blinkCounter > (blinkPeriod>>1))
		{
			if (!blinkOn)
			{
				blinkOn = true;
				LogoSetState(LogoState);
			}
			if (blinkCounter >= blinkPeriod)
			{
				blinkOn = false;
				blinkCounter = 0;
				LogoSetState(LogoState);
			}
		}
	}
}

void PerformAnimation(int period, int index)
{
	animating = true;
}

void LogoMfgDemoCycleInc(void)
{
	logo_state_changed();
	if (++logoMfgDemoIdx >= 6)
		logoMfgDemoIdx = 0;
	LogoSetState(logoMfgDemoSeq[logoMfgDemoIdx]);
}

void LogoMfgDemoCycleDec(void)
{
	logo_state_changed();
	if (--logoMfgDemoIdx < 0)
		logoMfgDemoIdx = 5;
	LogoSetState(logoMfgDemoSeq[logoMfgDemoIdx]);
}


void LogoDemoCycle(void)
{
	logo_state_changed();
	if (++logoDemoIdx >= 6)
		logoDemoIdx = 0;
	LogoSetState(logoDemoSeq[logoDemoIdx]);
}
