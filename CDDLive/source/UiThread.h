/*
 * UIThread.h
 *
 *  Created on: 29 Sept 2022
 *      Author: Iain.Quarmby
 */

#ifndef UITHREAD_H_
#define UITHREAD_H_

#include "thread.hpp"


using Thread = cpp_freertos::Thread;

class UiThread : public Thread
{
public:
	UiThread(uint16_t usStackDepth, UBaseType_t uxPriority, SemaphoreHandle_t& semMainThreadComplete);

protected:
    void Run();

    
private:
    SemaphoreHandle_t   _semMainThreadComplete;    
};

#endif /* GUITHREAD_H_ */
