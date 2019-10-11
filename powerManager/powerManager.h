// Part of the Almabraxas 3-1 project https://tronche.com/wiki/
// (c) Ch. Tronche 2018 (ch@tronche.com)
// MIT License

#ifndef _POWER_MANAGER_H_
#define _POWER_MANAGER_H_

#include <sys/types.h>

typedef uint16_t power_t;

// Compute left and right power using heading and targetHeading

void powerManager_loop_cb(uint16_t v, uint16_t i);

extern volatile uint8_t voltage; // in V/10
extern volatile uint8_t current; // in A/10
extern int8_t mppt_direction;    // +2 if increasing, -2 if decreasing
extern  uint16_t powerBudget;     // in PWM unit
extern volatile int16_t leftPower, rightPower; // in PWM unit

// Then set left and right power

void powerManager_setMotorPower_wait_for_init();
void powerManager_setMotorPower(int16_t leftPower, int16_t rightPower);

// Control

extern bool mpptOn; // false to set powerBudget manually

//extern unsigned hysteresis;

// debug

extern power_t peakPower;
extern uint16_t voltageReading, currentReading;
extern unsigned hysteresis;

#endif // _POWER_MANAGER_H_
