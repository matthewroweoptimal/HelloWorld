/*
 * GUIThread.h
 *
 *  Created on: 29 Sept 2022
 *      Author: Iain.Quarmby
 */

#ifndef GUITHREAD_H_
#define GUITHREAD_H_

#include "thread.hpp"


using Thread = cpp_freertos::Thread;

class GuiThread : public Thread
{
public:
	GuiThread(uint16_t usStackDepth, UBaseType_t uxPriority, SemaphoreHandle_t& semMainThreadComplete);

protected:
    void Run();

    
private:
    SemaphoreHandle_t   _semMainThreadComplete;    
};

class MetersThread : public Thread
{
public:
	MetersThread(uint16_t usStackDepth, UBaseType_t uxPriority, SemaphoreHandle_t& semMainThreadComplete);

protected:
    void Run();


private:
    SemaphoreHandle_t   _semMainThreadComplete;
};

class TimerKeeperThread : public Thread
{
public:
	TimerKeeperThread(uint16_t usStackDepth, UBaseType_t uxPriority, SemaphoreHandle_t& semMainThreadComplete);

protected:
    void Run();


private:
    SemaphoreHandle_t   _semMainThreadComplete;
};


class SysEventsThread : public Thread
{
public:
	SysEventsThread(uint16_t usStackDepth, UBaseType_t uxPriority, SemaphoreHandle_t& semMainThreadComplete);

protected:
    void Run();


private:
    SemaphoreHandle_t   _semMainThreadComplete;
};





#endif /* GUITHREAD_H_ */
