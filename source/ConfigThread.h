/*
 * ConfigThread.h
 *
 *  Created on: 1 Aug 2022
 *      Author: Stuart Campbell
 */

#ifndef CONFIGTHREAD_H_
#define CONFIGTHREAD_H_

#include "thread.hpp"

using Thread = cpp_freertos::Thread;

class ConfigThread : public Thread
{
public:
	ConfigThread(uint16_t usStackDepth, UBaseType_t uxPriority);

protected:
    void Run();

};

#endif /* CONFIGTHREAD_H_ */
