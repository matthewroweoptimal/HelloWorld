/*
 * MonitoringTimer.cpp
 *
 *  Created on: 16 Feb 2022
 *      Author: Matthew.Rowe
 */

#include "MonitoringTimer.h"
#include "board.h"


MonitoringTimer::MonitoringTimer(uint16_t usStackDepth, UBaseType_t uxPriority, TickType_t periodInTicks)
	: EventTimerTask(&monitoringEventGroup, "Monitoring", usStackDepth, uxPriority, periodInTicks)
{
	RegisterEventBits(ALL_EVENTS);	// register events with the base class for OtherEvent callback
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
	Leds::toggleLed(YELLOW_LED);
}

void MonitoringTimer::OtherEvent(EventBits_t eventBits)
{

}

