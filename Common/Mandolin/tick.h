// tick.h - Defines a simple interface to a running timer whose period per 'tick' is 1 ms, and whose value can never be 0.

#ifndef __TICK_H__
#define __TICK_H__

// Defines
#define TICK_COUNT_MAX 86400000 // A whole day at 1 tick per ms...

// Macros
#define TICKS_ELAPSED(now,then) (((now)<(then))?  TICK_COUNT_MAX - ((then) - (now)) : (now) - (then));

#endif // __TICK_H__
