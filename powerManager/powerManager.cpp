// Part of the Almabraxas 3-1 project https://tronche.com/wiki/
// (c) Ch. Tronche 2018 (ch@tronche.com)
// MIT License

#include <stdlib.h>
#include <sys/types.h>

#include "calibration.h"
#include "LinearMapper.h"
#include "nav.h"
#include "powerManager.h"

volatile uint8_t voltage; // in V/10
volatile uint8_t current; // in A/10
volatile int16_t leftPower = 0, rightPower = 0;

uint16_t powerBudget = 0; // in PWM units
static uint16_t peakPowerBudget = 0; // in PWM unit

bool mpptOn = false;
int8_t left_forcedPower = false;
bool right_negativePower = false;

// ========================= dispatchPower ====================================

static void powerManager_dispatchPower(
    uint16_t powerBudget, uint8_t heading, uint8_t targetHeading, 
    int16_t &leftPower, int16_t &rightPower) {
  (void)heading;
  (void)targetHeading;
  if (!fixOk && mpptOn && left_forcedPower == -128) {
    // We don't know where we are (and thus where to go)
    leftPower = rightPower = 0;
    return;
  }
  
  if (left_forcedPower != -128) {
    leftPower = powerBudget * float(left_forcedPower) / 100;
  } else
    leftPower = powerBudget / 2;
  rightPower = powerBudget - abs(leftPower);
  if (right_negativePower) rightPower = -rightPower;
}

// ============================== getPowerBudget ================================

//bool mpptOn = true;
power_t peakPower;

int8_t mppt_direction = 2; // start by increasing
unsigned hysteresis = 200; // That is 2W

static void reverseMPPTDirection() {
  mppt_direction = -mppt_direction;
 // We go back to throttle giving the best power we ever had
  powerBudget = peakPowerBudget;
  // We reset peak power, so we'll measure power at previous peak
  // power throttle on next step.
  peakPower = peakPowerBudget = 0;
}

static void powerManager_getPowerBudget(uint8_t voltage, uint8_t current) {
  if (!mpptOn) return;
  if (voltage < 100) {
    powerBudget = peakPower = peakPowerBudget = 0;
    mppt_direction = 2;
    return;
  }
  // if (current < 0.1Amp) {
  //   // Powered by capacitor, we decrease consumption until PV voltage balance capacitor voltage
  //   powerDirection = -2;
  //   return;
  // }

  if (!powerBudget) {
    // Lower bound
    // We're at a power budget so low (may be not enough light, or at
    // the beginning). We try to get more power. For now we can't tell
    // anything about max power.
    mppt_direction = 2;
    peakPower = peakPowerBudget = 0;
  } else if (powerBudget >= CALIBRATION_MAX_PWM * 2) {
    // Upper bound
    // Most likely, the load can't absorb all the power ! Which isn't
    // very good. We try reducing throttle.
    reverseMPPTDirection();
  } else {
    // Not lower nor upper bound
    power_t power = voltage * current; // in W / 1000
    if (power > peakPower) {
      peakPower = power;
      peakPowerBudget = powerBudget;
    } else {
      // Hysteresis is 1.5 x V to protect from +1/-1 unit aliasing on the current reading (~100 mA)
      // At most, hysteresis is ~ 250 x 1.5 = 375, that is 3.75W, but
      // for peak power, we're around 18V, so it's ~2.7W
      if (power < peakPower - hysteresis) {
      // We are out of the maximum power window in either direction,
      // so we change.
      reverseMPPTDirection();
      }
    }
    // If we're here, nothing special happened, and we go on in the
    // same direction for next step.
  }
  powerBudget += mppt_direction;
}

// ================================= Loop =============================

static LinearMapper _vMapper(875, 70, 3178, 200);
static LinearMapper _iMapper(2515, 10, 2875, 100);

#include "adebug.h"
#include "mbed.h"

uint16_t voltageReading, currentReading;

void powerManager_loop_cb(uint16_t v, uint16_t i) {
  voltageReading = v;
  currentReading = i;
  voltage = _vMapper.convert(v);
  current = _iMapper.convert(i);
  
  powerManager_getPowerBudget(voltage, current);
  int16_t l, r;
  powerManager_dispatchPower(powerBudget, 0, 0, l, r);
  leftPower = l;
  rightPower = r;
  powerManager_setMotorPower(l, r);
}
