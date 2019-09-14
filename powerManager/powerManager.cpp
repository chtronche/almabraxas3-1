// Part of the Almabraxas 3-1 project https://tronche.com/wiki/
// (c) Ch. Tronche 2018 (ch@tronche.com)
// MIT License

#include <sys/types.h>

#include "powerManager.h"

volatile uint8_t voltage; // in V/10
volatile uint8_t current; // in A/100
volatile int16_t leftPower = 0, rightPower = 0;

unsigned hysteresis = 2000;
uint16_t powerBudget;
int8_t mppt_direction = 2; // start by increasing
