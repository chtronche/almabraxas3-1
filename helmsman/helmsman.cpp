#include <stdlib.h>

#include "helmsman.h"
#include "nav.h"
#include "powerManager.h"

int8_t forcedSteering = false;
bool forcedSteering_reverse = false;

int16_t leftPower = 0, rightPower = 0;

// Blue ESC:
// 0 is 1500 us with a +/- 25 dead band
// 1900 is max, 1400 is full reverse

#define MAX(a,b) ((a) >= (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

static const int deadband = 15;

static inline int adjust_forBlueESC(int pwm) {
  if (!pwm) return 1500;
  return 1500 + (pwm < 0 ? MAX(-400, pwm - deadband) : MIN(400, pwm + deadband));
}

// helm is 0 for straight forward, -100 for full left (50% reverse
// left, 50% forward right), +100 for full right

static void steer(uint16_t powerBudget, int helm) {
  int leftPower_percent = helm <= 50 ? helm + 50 : 150 - helm;
  int rightPower_perCent = helm <= -50 ? helm + 150 : 50 - helm;

  leftPower = powerBudget * leftPower_percent / 100;
  rightPower = powerBudget * rightPower_perCent / 100;

  setMotorPower(adjust_forBlueESC(leftPower), adjust_forBlueESC(rightPower));
}

void helmsman_dispatchPower(uint16_t powerBudget, uint8_t heading, uint8_t targetHeading) {
  (void)heading;
  (void)targetHeading;
  if (!fixOk && mpptOn && forcedSteering == -128) {
    // We don't know where we are (and thus where to go)
    steer(0, 0);
    return;
  }
  
  if (forcedSteering != -128) {
    steer(powerBudget, forcedSteering);
    return;
  }

  // PID should take place here
  steer(powerBudget, (targetHeading - heading) * 100 / 128);
}

