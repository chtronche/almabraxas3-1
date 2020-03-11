#include "mbed.h"

#include "AsyncStarter.h"
#include "powerManager.h"
#include "wiring.h"

#include "reporting.h"

static PwmOut _left(PWM_LEFT);
static PwmOut _right(PWM_RIGHT);

static void setMotorPower_wait_for_init() {
    _left.pulsewidth_us(1500);
    _right.pulsewidth_us(1500);
    wait_ms(7000);
}

static AsyncStarter _init(setMotorPower_wait_for_init);

void setMotorPower(int leftPWM, int rightPWM) {
  _init.ready();
  _left.pulsewidth_us(leftPWM);
  _right.pulsewidth_us(rightPWM);
}
