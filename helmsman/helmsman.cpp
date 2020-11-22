#include <stdlib.h>

#include "alma_math.h"
#include "helmsman.h"
#include "nav.h"
#include "powerManager.h"

int8_t forcedSteering = false;
bool forcedSteering_reverse = false;

int16_t leftPower = 0, rightPower = 0;
int8_t helm;

// Blue ESC:
// 0 is 1500 us with a +/- 25 dead band
// 1900 is max, 1100 is full reverse

static const int deadband = 25;

static int adjust_forBlueESC(int pwm) {
  if (!pwm) return 1500;
  if (pwm == -1) pwm = -2; // if it's two slow, the motor won't start in some configuration, stopping the ESC until power = 0
  return 1500 + (pwm < 0 ? MAX(-400, pwm - deadband) : MIN(400, pwm + deadband));
}

// helm is 0 for straight forward, -64 for full left (50% reverse
// left, 50% forward right), 64 for full right.

static void steer(uint16_t powerBudget, int helm_, bool reverse) {
  helm = helm_;
  int leftPower_fraction = helm_ <= 32 ? helm_ + 32: 96 - helm_;
  int rightPower_fraction = helm_ <= -32 ? helm_ + 96 : 32 - helm_;

  if (reverse) {
    leftPower_fraction = -leftPower_fraction;
    rightPower_fraction = -rightPower_fraction;
  }

  leftPower = powerBudget * leftPower_fraction / 64;
  rightPower = powerBudget * rightPower_fraction / 64;

  setMotorPower(adjust_forBlueESC(leftPower), adjust_forBlueESC(rightPower));
}

void helmsman_dispatchPower(uint16_t powerBudget, uint8_t heading, uint8_t targetHeading) {
  (void)heading;
  (void)targetHeading;
  if (!fixOk && mpptOn && forcedSteering == -128) {
    // We don't know where we are (and thus where to go)
    steer(0, 0, false);
    return;
  }
  
  if (forcedSteering != -128) {
    steer(powerBudget, forcedSteering, forcedSteering_reverse);
    return;
  }

  // PID should take place here
  steer(powerBudget, (targetHeading - heading) / 2, forcedSteering_reverse);
}
