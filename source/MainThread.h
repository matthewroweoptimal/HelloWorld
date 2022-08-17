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
	MainThread(uint16_t usStackDepth, UBaseType_t uxPriority, SemaphoreHandle_t& semMainThreadComplete);

protected:
	void Run();

private:
    SemaphoreHandle_t   _semMainThreadComplete;
};

#endif /* MAINTHREAD_H_ */
