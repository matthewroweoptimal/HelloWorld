/*
* trace.h
*
* Created on: 1 Jun 2022
* Author: Matthew.Rowe
*/



#ifndef SOURCE_TRACE_H_

#define SOURCE_TRACE_H_


#if SHOW_TRACE
#define TRACE() printf("%s %s\n", __func__, __FILE__);
#else
#define TRACE()
#endif





#endif /* SOURCE_TRACE_H_ */

