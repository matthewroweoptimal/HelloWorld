/*
 * MonitoringTimer.cpp
 *
 *  Created on: 16 Feb 2022
 *      Author: Matthew.Rowe
 */


/*
 * MonitoringTimer.cpp
 *
 *  Created on: 25 Mar 2021
 *      Author: Matthew.Rowe
 */

#include "MonitoringTimer.h"
#include "board.h"

MonitoringTimer::MonitoringTimer(uint16_t usStackDepth, UBaseType_t uxPriority, TickType_t periodInTicks)
	: EventTimerTask(&monitoringEventGroup, "Monitoring", usStackDepth, uxPriority, periodInTicks)
{
	RegisterEventBits(ALL_EVENTS);	// register SPI event message queue event with base class
}

void MonitoringTimer::Run()
{

	printf("Monitoring Timer thread startup\n");

	while (true)
	{
		WaitOnEvents();
	}
}

void MonitoringTimer::TimerEvent()
{
	// toggle an led
	GPIO_TOGGLE(YELLOW_LED);
}

void MonitoringTimer::OtherEvent(EventBits_t eventBits)
{

}

