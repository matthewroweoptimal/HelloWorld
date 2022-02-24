/*
 * TcpThread.h
 *
 *  Created on: 21 Feb 2022
 *      Author: Matthew.Rowe
 */

#ifndef TCPTHREAD_H_
#define TCPTHREAD_H_

#include "thread.hpp"
#include "lwip/netifapi.h"
#include "lwip/tcpip.h"
#include "netif/ethernetif.h"

using Thread = cpp_freertos::Thread;

class TcpThread : public Thread
{
public:
	TcpThread(uint16_t usStackDepth, UBaseType_t uxPriority);

protected:
    void Run();

    struct netif netif;
};

#endif /* TCPTHREAD_H_ */
