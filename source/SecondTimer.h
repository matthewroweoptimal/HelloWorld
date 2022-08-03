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
const uint32_t MEMORY_DISPLAY_TICKS = 5;

static void printFreeRTOSHeapStats()
{
return;	// Whilst DEBUGGING OTHER STUFF

	static xHeapStats stats = {0};
	vPortGetHeapStats( &stats );
	printf("FreeRTOS heap\n  Free = %u, biggest = %u, smallest = %u\n",
		   stats.xAvailableHeapSpaceInBytes, stats.xSizeOfLargestFreeBlockInBytes, stats.xSizeOfSmallestFreeBlockInBytes );
	printf("  systime: %d\n", RTOS_AppGetRuntimeCounterValueFromISR());
}


class SecondTimer : public Timer
{
public:
	SecondTimer() :
		Timer("SecondTimer", pdMS_TO_TICKS(1000), true) {};

protected:
	void Run()
	{
		// toggle an led
		Leds::toggleLed(GREEN_LED);
		Leds::setLed(RED_LED, Buttons::readButton(SW2));

		if (!--tickCounter)
		{
			printFreeRTOSHeapStats();
			tickCounter = MEMORY_DISPLAY_TICKS;
		}
	}

	uint32_t tickCounter = MEMORY_DISPLAY_TICKS;
};

#endif /* SECONDTIMER_H_ */
