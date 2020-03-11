#ifndef _HELMSMAN_H_
#define _HELMSMAN_H_

#include <stdint.h>

extern int16_t leftPower, rightPower; // in PWM unit

extern int8_t forcedSteering; // -128 for cruise
extern bool forcedSteering_reverse; // When forced

// That is, steer

void helmsman_dispatchPower(uint16_t powerBudget, uint8_t heading, uint8_t targetHeading);

// OSD

void setMotorPower(int leftPower, int rightPower);

#endif // _HELMSMAN_H_
