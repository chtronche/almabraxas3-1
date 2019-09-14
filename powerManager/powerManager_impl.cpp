#include "mbed.h"

#include "powerManager.h"
#include "wiring.h"

volatile uint16_t voltageReading = 0;
volatile uint16_t currentReading = 0;
volatile uint16_t peakPower; // in PWM unit

// ================== Sampling loop ======================

static AnalogIn _vin(SAMPLER_VOLTAGE_PIN);
static AnalogIn _iin(SAMPLER_CURRENT_PIN);

static PwmOut _left(PWM_LEFT);
static PwmOut _right(PWM_RIGHT);

void _loopFn() {
  _right.pulsewidth_ms(1500);
  wait_ms(7000);
  _right.pulsewidth_us(1600);
  for(;;) {
    voltageReading = _vin.read_u16() >> 4;
    currentReading = _iin.read_u16() >> 4;
    powerManager_loop(0, 0);
    wait_ms(10);
  }
}

static Thread _loop;

static struct _initC {
  _initC() {
    _loop.start(_loopFn);
  }
} _init;

void powerManager_setMotorPower(int16_t leftPower, int16_t rightPower) {
  _left.pulsewidth_us(590 + leftPower);
  _right.pulsewidth_us(1500 + rightPower);
}
