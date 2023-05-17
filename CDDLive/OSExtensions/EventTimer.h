/*
 * EventTimer.h
 *
 *  Created on: 26 Sep 2019
 *      Author: Matthew.Rowe
 */

#ifndef OSEXTENSIONS_EVENTTIMER_H_
#define OSEXTENSIONS_EVENTTIMER_H_

#include "timer.hpp"
#include "event_groups.hpp"

using Timer 		= cpp_freertos::Timer;
using EventGroup	= cpp_freertos::EventGroup;

class EventTimer : public Timer {
public:
	EventTimer( const char * const eventTimerName,
				TickType_t periodInTicks
				)
	 : Timer(eventTimerName, periodInTicks, true)
	{

	};

	void RegisterEventGroup(EventGroup* pEventGroup, EventBits_t eventBits)
	{
		this->eventBits = eventBits;
		this->pEventGroup = pEventGroup;
	}

protected:
	void Run() {
		//set event
		if (pEventGroup)
		{
			// set the event flag when this timer goes off
			pEventGroup->SetBits(eventBits);
		}
	}

	EventGroup*		pEventGroup		= NULL;
	EventBits_t 	eventBits		= 0;

};



#endif /* OSEXTENSIONS_EVENTTIMER_H_ */
