/*
 * Threads.h
 *
 *  Created on: 15 Feb 2022
 *      Author: Matthew.Rowe
 */

#ifndef THREADS_H_
#define THREADS_H_

#include "MainThread.h"
#include "SecondTimer.h"
#include "MonitoringTimer.h"

class Threads
{
public:
    void StartThreads();
    void StartScheduler();

private:
    /* Threads */
    MainThread *mainThread;
    MonitoringTimer *monitoring;

    /* Timers */
    SecondTimer *secondTimer;

};

#endif /* THREADS_H_ */
