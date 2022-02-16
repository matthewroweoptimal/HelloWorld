/*
 * MonitoringTimer.h
 *
 *  Created on: 16 Feb 2022
 *      Author: Matthew.Rowe
 */

#ifndef MONITORINGTIMER_H_
#define MONITORINGTIMER_H_

#include "timer.hpp"
#include "thread.hpp"
#include "event_groups.hpp"
#include "EventTimerTask.h"

using Timer = cpp_freertos::Timer;

const uint32_t	ALL_EVENTS = 0;	// add other event flags here for drivers

class MonitoringTimer : public EventTimerTask
{
public:
	MonitoringTimer(uint16_t usStackDepth, UBaseType_t uxPriority, TickType_t periodInTicks);

protected:
    void Run();
	void TimerEvent();
	void OtherEvent(EventBits_t eventBits);

private:
	void processAlarmMute(void);
	void setPowerLedState(void);

	EventGroup monitoringEventGroup;

};

#endif /* MONITORINGTIMER_H_ */
