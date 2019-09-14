// Part of the Almabraxas 3-1 project https://tronche.com/wiki/
// (c) Ch. Tronche 2018 (ch@tronche.com)
// MIT License

#ifndef _POWER_MANAGER_H_
#define _POWER_MANAGER_H_

#include <sys/types.h>

typedef uint16_t power_t; // in W/100

extern volatile uint8_t voltage; // in V/10
extern volatile uint8_t current; // in A/10

extern int8_t mppt_direction; // +2 if increasing, -2 if decreasing
extern uint16_t powerBudget; // in PWM unit
extern volatile int16_t leftPower, rightPower; // in PWM unit

extern bool mpptOn; // false to set powerBudget manually
extern unsigned hysteresis;

// debug

extern volatile uint16_t voltageReading;
extern volatile uint16_t currentReading;
extern volatile power_t peakPower;

void powerManager_loop(uint8_t v, uint8_t i);
void powerManager_setMotorPower(int16_t leftPower, int16_t rightPower);

#endif // _POWER_MANAGER_H_
