/*
 * oly_logo.h
 *
 *  Created on: Aug 19, 2015
 *      Author: Kris.Simonsen
 */

#ifndef OLY_LOGO_H_
#define OLY_LOGO_H_

#include "Model_Info.h"

typedef enum {
	LOGO_OFF,
	LOGO_ON_RED,
	LOGO_ON_WHITE,
	LOGO_ON_AMBER,
	LOGO_SLOW_RED,
	LOGO_SLOW_WHITE,
	LOGO_SLOW_AMBER,
	LOGO_FAST_RED,
	LOGO_FAST_WHITE,
	LOGO_FAST_AMBER,
} oly_Logo_State_t;

typedef enum {
	LOGO_RUN_STATE_BOOT,
	LOGO_RUN_STATE_NORMAL,
	LOGO_RUN_STATE_LIMITING,
	LOGO_RUN_STATE_FAULT,
	LOGO_RUN_STATE_ADVERTISE,
	LOGO_RUN_STATE_IDENTIFY,
	LOGO_RUN_STATE_IDENTIFY_ALT,
} oly_Logo_Run_State_t;

#define LOGO_SLOW_MS	420
#define LOGO_FAST_MS	240

#if USE_OLY_UI
												/* BOOT		 	 NORMAL			LIMITING			FAULT			ADVERTISE		IDENTIFY			ADVERTIDENTIFY */
const oly_Logo_State_t USER_MODE_ON[] = 		{ LOGO_ON_WHITE, LOGO_ON_WHITE, LOGO_ON_WHITE,		LOGO_ON_WHITE,	LOGO_ON_AMBER,	LOGO_SLOW_WHITE, 	LOGO_SLOW_AMBER };
const oly_Logo_State_t USER_MODE_ON_LIMITING[]= { LOGO_ON_WHITE, LOGO_OFF, 		LOGO_FAST_WHITE,	LOGO_ON_WHITE,	LOGO_ON_AMBER,	LOGO_SLOW_WHITE,	LOGO_SLOW_AMBER };
const oly_Logo_State_t USER_MODE_OFF[] = 		{ LOGO_OFF, 	 LOGO_OFF, 		LOGO_OFF, 		 	LOGO_OFF,		LOGO_ON_AMBER,	LOGO_SLOW_WHITE,	LOGO_SLOW_AMBER };
const oly_Logo_State_t USER_MODE_FAULT_ONLY[] = { LOGO_OFF, 	 LOGO_OFF, 		LOGO_OFF, 		 	LOGO_ON_RED,	LOGO_ON_AMBER,	LOGO_SLOW_WHITE,	LOGO_SLOW_AMBER };
const oly_Logo_State_t USER_MODE_SPECIAL[] =    { LOGO_OFF, 	 LOGO_ON_WHITE, LOGO_ON_AMBER, 		LOGO_ON_RED,	LOGO_ON_AMBER,	LOGO_SLOW_WHITE,	LOGO_SLOW_AMBER };

#elif USE_CDD_UI
												/* BOOT		 	 NORMAL			LIMITING		 FAULT		 	  ADVERTISE	     IDENTIFY	  	  IDENTIFY_ALT */
const oly_Logo_State_t USER_MODE_ON[] = 		{ LOGO_ON_RED, LOGO_ON_RED, 	LOGO_ON_RED, 	 LOGO_ON_RED,     LOGO_ON_RED,   LOGO_ON_RED,     LOGO_ON_RED };
const oly_Logo_State_t USER_MODE_ON_LIMITING[]= { LOGO_ON_RED, LOGO_ON_RED, 	LOGO_ON_RED, 	 LOGO_ON_RED,     LOGO_ON_RED,   LOGO_ON_RED,     LOGO_ON_RED };
const oly_Logo_State_t USER_MODE_OFF[] = 		{ LOGO_OFF, LOGO_OFF, 		LOGO_OFF,		 LOGO_OFF, 	 	  LOGO_OFF, 	 LOGO_OFF,        LOGO_OFF };
const oly_Logo_State_t USER_MODE_FAULT_ONLY[] =	{ LOGO_OFF, LOGO_OFF, 		LOGO_OFF,		 LOGO_OFF, 	 	  LOGO_OFF, 	 LOGO_OFF,        LOGO_OFF };
#endif


/* API Functions */
void LogoSetState(oly_Logo_State_t state);
void LogoUpdateRunState(oly_Logo_Run_State_t run_state, LOUD_logo_mode user_mode);

/* Test Functions */
const oly_Logo_State_t logoMfgDemoSeq[6] = {LOGO_SLOW_WHITE, LOGO_FAST_WHITE, LOGO_SLOW_AMBER, LOGO_FAST_AMBER, LOGO_SLOW_RED, LOGO_FAST_RED};

void LogoDemoCycle(void);
void LogoMfgDemoCycleInc(void);
void LogoMfgDemoCycleDec(void);
oly_Logo_State_t LogoGetState();
void logo_state_changed();
void BlinkLed(int period, uint32_t led);
void poll_logo(int ms);
void PerformAnimation(int period, int index);

oly_Logo_State_t LogoMeterState(uint32_t size, uint32_t index);

#endif /* OLY_LOGO_H_ */
