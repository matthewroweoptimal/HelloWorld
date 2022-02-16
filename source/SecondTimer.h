/*
 * SecondTimer.h
 *
 *  Created on: 15 Feb 2022
 *      Author: Matthew.Rowe
 */

#ifndef SECONDTIMER_H_
#define SECONDTIMER_H_

#include "timer.hpp"
#include <stdbool.h>
#include "board.h"

using Timer = cpp_freertos::Timer;

class SecondTimer : public Timer
{
public:
	SecondTimer() :
		Timer("SecondTimer", 1000 / portTICK_RATE_MS , true) {};

protected:
	void Run()
	{
		// toggle an led
		Leds::toggleLed(GREEN_LED);
		Leds::setLed(RED_LED, Buttons::readButton(SW2));
	}
};

#endif /* SECONDTIMER_H_ */
