/*
 * EventTimerTask.h
 *
 *  Created on: 26 Sep 2019
 *      Author: Matthew.Rowe
 */

#ifndef OSEXTENSIONS_EVENTTIMERTASK_H_
#define OSEXTENSIONS_EVENTTIMERTASK_H_

#include "timer.hpp"
#include "event_groups.hpp"
#include "thread.hpp"
#include "EventTimer.h"

using Timer 		= cpp_freertos::Timer;
using EventGroup	= cpp_freertos::EventGroup;
using Thread		= cpp_freertos::Thread;

/* Event bits used by this task */
namespace nsEventTimerTask {
	const uint32_t	TIMER_TASK_BIT						= ( 1 << 0 );	// reserved bit for the timer
}

class EventTimerTask : public Thread {
public:
	EventTimerTask( EventGroup *eventGroup,
					const char * const eventTimerTaskName,
					uint16_t StackDepth,
					UBaseType_t Priority,
					TickType_t periodInTicks
					)
	 : Thread(eventTimerTaskName, StackDepth, Priority),
	   eventGroup(eventGroup),
	   eventTimer("EventTaskTimer", periodInTicks),
	   eventBits(0),
	   eventsRegistered(false)
	{

	};

    void RegisterEventBits(EventBits_t eventBits)
	{
    	// keep a copy of the event bits to wait on, including our timer
    	this->eventBits	= eventBits | nsEventTimerTask::TIMER_TASK_BIT;
    	// timer just needs to know the timer event bit
    	eventTimer.RegisterEventGroup(eventGroup, nsEventTimerTask::TIMER_TASK_BIT);
    	eventsRegistered = true;
    }

    void Start()
    {
    	configASSERT(eventsRegistered);
    	Thread::Start();
    	eventTimer.Start();
    }

    void StopTimer()
    {
    	eventTimer.Stop();
    }

    void RestartTimer()
    {
    	eventTimer.Start();
    }

protected:
    void WaitOnEvents()
    {
        EventBits_t expiredEventBits = eventGroup->WaitBits( eventBits /*uxBitsToWaitFor*/,
												 	 	 	true /*bool xClearOnExit*/,
															false /*bool xWaitForAllBits*/,
															portMAX_DELAY /* xTicksToWait*/);
        // one or more of the event flags has signalled
        if (expiredEventBits & nsEventTimerTask::TIMER_TASK_BIT)	// timer event has fired
        {
        	TimerEvent();
        }

        if (expiredEventBits & ~nsEventTimerTask::TIMER_TASK_BIT)	// any other events have fired
        {
        	OtherEvent(expiredEventBits);
        }
    }

    // pure virtual - must implement these.
    virtual void Run() = 0;
    virtual void TimerEvent() = 0;
    virtual void OtherEvent(EventBits_t eventBits) = 0;

    EventGroup* eventGroup;
    EventTimer	eventTimer;
    EventBits_t eventBits;
    bool		eventsRegistered = false;
};

#endif /* OSEXTENSIONS_EVENTTIMERTASK_H_ */
