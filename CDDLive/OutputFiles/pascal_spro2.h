/*
 * pascal_spro2.h
 *
 *  Created on: Apr 28, 2015
 *      Author: Kris.Simonsen
 */

#ifndef PASCAL_SPRO2_H_
#define PASCAL_SPRO2_H_

#include "oly.h"
//#include "gpio_pins.h"

#define AMP1_MUTE_ON			(GPIO_DRV_WritePinOutput(AMP1_DISABLE_MUTE_CNTRL, 1))		// Logic inverted by external transistor
#define AMP2_MUTE_ON			(GPIO_DRV_WritePinOutput(AMP2_DISABLE_MUTE_CNTRL, 1))
#define AMP1_MUTE_OFF			(GPIO_DRV_WritePinOutput(AMP1_DISABLE_MUTE_CNTRL, 0))		// Logic inverted by external transistor
#define AMP2_MUTE_OFF			(GPIO_DRV_WritePinOutput(AMP2_DISABLE_MUTE_CNTRL, 0))
#define AMP1_2_BRIDGE_MODE_ON	(GPIO_DRV_WritePinOutput(BTL4R_CONTROL, 1))					// Logic inverted by external transistor
#define AMP1_2_BRIDGE_MODE_OFF	(GPIO_DRV_WritePinOutput(BTL4R_CONTROL, 0))
#define AMP1_2_STANDBY_ON		(GPIO_DRV_WritePinOutput(AMP1_2_STANDBY_CNTRL, 1))			// Logic inverted by external transistor
#define AMP1_2_STANDBY_OFF		(GPIO_DRV_WritePinOutput(AMP1_2_STANDBY_CNTRL, 0))
#define AMP1_MON_TEMP			(GPIO_DRV_WritePinOutput(AMP1_TEMP_VAC_SEL, 0))				// Logic inverted by external transistor
#define AMP2_MON_TEMP			(GPIO_DRV_WritePinOutput(AMP2_TEMP_VAC_SEL, 0))
#define AMP1_MON_VAC			(GPIO_DRV_WritePinOutput(AMP1_TEMP_VAC_SEL, 1))
#define AMP2_MON_VAC			(GPIO_DRV_WritePinOutput(AMP2_TEMP_VAC_SEL, 1))

void amp_init(void);
void amp_mute(bool state);
void amp_standby(bool state);

#endif /* PASCAL_SPRO2_H_ */
