#ifndef _ALMA_CLOCK_H_
#define _ALMA_CLOCK_H_

#include "mbed.h"

extern uint32_t alma_clock;  // s since boot

void alma_clock_init();
void alma_clock_advance();

#endif // _ALMA_CLOCK_H_
