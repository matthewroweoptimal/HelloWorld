/*
 * DanteThread.h
 *
 *  Created on: 29 Sept 2022
 *      Author: Iain.Quarmby
 */

#ifndef DANTETHREAD_H_
#define DANTETHREAD_H_

#include "thread.hpp"


using Thread = cpp_freertos::Thread;

class DanteThread : public Thread
{
public:
	DanteThread(uint16_t usStackDepth, UBaseType_t uxPriority, SemaphoreHandle_t& semMainThreadComplete);

protected:
    void Run();

    
private:
    SemaphoreHandle_t   _semMainThreadComplete;    
};

#endif /* DANTETHREAD_H_ */
