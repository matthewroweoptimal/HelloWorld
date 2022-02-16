/*
 * board.h
 *
 *  Created on: 16 Feb 2022
 *      Author: Matthew.Rowe
 */

#ifndef HARDWARE_BOARD_H_
#define HARDWARE_BOARD_H_
#include "NuMicro.h"
#include "Pin.h"

enum LED_t {
	RED_LED,
	YELLOW_LED,
	GREEN_LED,
	NUM_LEDS
};

static Pin ledPins[NUM_LEDS] = {
	{GPIOH_PIN_BASE, 0},	// red led
	{GPIOH_PIN_BASE, 1},	// yellow led
	{GPIOH_PIN_BASE, 2}		// green led
};

enum BUTTON_t {
	SW2,
	SW3,
	NUM_SWITCHES
};

static Pin buttonPins[NUM_SWITCHES] = {
	{GPIOG_PIN_BASE, 15},	// sw2
	{GPIOF_PIN_BASE, 11}	// sw3
};

class Leds
{
	Leds();

public:
	static void setLed(LED_t led, bool state) {
		ledPins[led].Write(state ? 0:1);
	};

	static void toggleLed(LED_t led) {
		ledPins[led].Write( ledPins[led].Read() ? 0:1 );
	}
};

class Buttons
{
	Buttons();

public:
	static bool readButton(BUTTON_t button) {
		return !buttonPins[button].Read();
	}
};

#endif /* HARDWARE_BOARD_H_ */
