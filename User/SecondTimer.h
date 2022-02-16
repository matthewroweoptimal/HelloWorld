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
#include "NuMicro.h"

using Timer = cpp_freertos::Timer;

class SecondTimer : public Timer
{
public:
	SecondTimer() :
		Timer("SecondTimer", 1000, true) {};

protected:
	void Run()
	{
		//printf("SecondTimer tick\n");
		// toggle an led
		PH->DOUT = ledState ? 1:0;
		ledState = !ledState;
	}

	bool ledState = 0;
};


#endif /* SECONDTIMER_H_ */
