/*
 * MainThread.h
 *
 *  Created on: 16 Feb 2022
 *      Author: Matthew.Rowe
 */

#ifndef MAINTHREAD_H_
#define MAINTHREAD_H_

#include "thread.hpp"

using Thread = cpp_freertos::Thread;

class MainThread : public Thread
{
public:
	MainThread(uint16_t usStackDepth, UBaseType_t uxPriority) :
		Thread("MainThread", usStackDepth, uxPriority) {
		printf("MainThread created\n");
	};

protected:
	void Run()
	{
		printf("MainThread run. Suspend\n");
		// go to sleep
		Suspend();
	}
};

#endif /* MAINTHREAD_H_ */
