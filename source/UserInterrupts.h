/*
 * Userinterrupts.h
 *
 *  Created on: 18 Feb 2022
 *      Author: Matthew.Rowe
 */

#ifndef USERINTERRUPTS_H_
#define USERINTERRUPTS_H_

//#include "fsl_ftm.h"

// TODO: Fix this for Nuvoton port

extern "C" {
static uint32_t RTOS_RunTimeCounter; /* runtime counter, used for configGENERATE_RUNTIME_STATS */

void FTM0_IRQHandler(void) {
  /* Clear interrupt flag.*/
  //FTM_ClearStatusFlags(FTM0, kFTM_TimeOverflowFlag);
  //RTOS_RunTimeCounter++; /* increment runtime counter */
}


void RTOS_AppConfigureTimerForRuntimeStats(void) {
  RTOS_RunTimeCounter = 0;
  //EnableIRQ(FTM0_IRQn);
}

uint32_t RTOS_AppGetRuntimeCounterValueFromISR(void) {
  return RTOS_RunTimeCounter;
}

}

#endif /* USERINTERRUPTS_H_ */
