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

static const PinName ALMA_MOSI_1 = PB_15; // SPI2, clk = PCKL1
static const PinName ALMA_MISO_1 = PB_14;
static const PinName ALMA_SCLK_1 = PB_13;

static const PinName RFM69_MOSI = ALMA_MOSI_1;
static const PinName RFM69_MISO = ALMA_MISO_1;
static const PinName RFM69_SCLK = ALMA_SCLK_1;
static const PinName RFM69_CS = PB_1;
static const PinName RFM69_INT = PA_9;
static const PinName RFM69_RST = PC_7;

static const PinName SD_MOSI = PB_5;
static const PinName SD_MISO = PB_4;
static const PinName SD_SCK = PB_3; // SPI1, clk = PCKL2 [ou 3 (ALT), clk = PCKL1]
static const PinName SD_CS = PB_10;

static const PinName ALMA_SCL = PB_8;
static const PinName ALMA_SDA = PB_9;

static const PinName GPS_TX = PB_6;
static const PinName GPS_RX = PA_10;

static const PinName DHT_22_1WIRE = PC_3;

static const PinName MPPT_SWITCH_INT = PA_6;

#endif // _WIRING_H_
