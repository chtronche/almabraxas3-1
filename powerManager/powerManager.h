// Part of the Almabraxas 3-1 project https://tronche.com/wiki/
// (c) Ch. Tronche 2018 (ch@tronche.com)
// MIT License

#ifndef _POWER_MANAGER_H_
#define _POWER_MANAGER_H_

#include <sys/types.h>
#include "LinearMapper.h"

typedef uint32_t power_t;

void powerManager_init();
void powerManager_loop_cb(uint16_t v, uint16_t i);

extern volatile uint8_t voltage; // in V/10
extern volatile uint8_t current; // in A/10
extern volatile int8_t mppt_direction;    // +2 if increasing, -2 if decreasing, 0 if mppt algorithm is off (budget set manually, possibly to 0)
extern volatile uint16_t powerBudget;     // in PWM unit

extern volatile NVLinearMapper vMapper, iMapper;

// MPPT hardware switch 

void mpptSwitch_init();
void mpptSwitch_on();
void mpptSwitch_off();

// debug

extern power_t peakPower;
extern uint16_t voltageReading, currentReading;

void currentSamplerLogger_start(const char *fileName);
void currentSamplerLogger_stop();

#endif // _POWER_MANAGER_H_
