#include "mbed.h"

#include "powerManager.h"
#include "wiring.h"

static PwmOut _left(PWM_LEFT);
static PwmOut _right(PWM_RIGHT);

void powerManager_setMotorPower_wait_for_init() {
    _left.pulsewidth_us(590);
    _right.pulsewidth_us(1500);
    wait_ms(7000);
}

void powerManager_setMotorPower(int16_t leftPower, int16_t rightPower) {
  _left.pulsewidth_us(590 + leftPower);
  _right.pulsewidth_us(1500 + rightPower);
}
