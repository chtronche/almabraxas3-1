// Part of the Almabraxas 3-1 project https://tronche.com/wiki/
// (c) Ch. Tronche 2018 (ch@tronche.com)
// MIT License

#ifndef _WIRING_H_
#define _WIRING_H_

#include "mbed.h"

static const PinName DEBUG_LED_RED = PA_11;
static const PinName DEBUG_LED_GREEN = PA_12;
static const PinName DEBUG_LED_YELLOW = PC_5;
static const PinName DEBUG_LED_BLUE = PC_6;
static const PinName DEBUG_LED_WHITE = PC_8;

static const PinName SAMPLER_VOLTAGE_PIN = PC_0;
static const PinName SAMPLER_CURRENT_PIN = PC_1;

static const PinName PWM_LEFT = PA_0;
static const PinName PWM_RIGHT = PA_1;

static const PinName RFM69_MOSI = PB_15;
static const PinName RFM69_MISO = PB_14;
static const PinName RFM69_SCLK = PB_13;
static const PinName RFM69_CS = PB_1;
static const PinName RFM69_INT = PA_9;
static const PinName RFM69_RST = PC_7;

#endif // _WIRING_H_
