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
#include "TcpThread.h"
#include "ConfigThread.h"

class Threads
{
public:
    void StartThreads();
    void StartScheduler();

private:
    /* Threads */
    MainThread 		*_mainThread;
    MonitoringTimer *_monitoring;
    TcpThread 		*_tcpThread;
    ConfigThread	*_configThread;

    /* Timers */
    SecondTimer 	*_secondTimer;

};

#endif /* THREADS_H_ */
