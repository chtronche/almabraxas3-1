#include "AsyncStarter.h"
#include "FXOS8700.h"
#include "wiring.h"

static FXOS8700 compass(ALMA_SDA, ALMA_SCL);

uint8_t getMagneticHeading() {
  float mag[3];
  compass.acquire_mag_data_uT(mag);
  // No need to compensate for negative number: magic of modulo !
  int8_t heading_ = int8_t(atan2(mag[1], mag[0]) * 128 / M_PI);
  uint8_t heading = heading_; // Strangely, if you don't do this 2-step conversion, you end up with0 for a negative number ?
  return heading;
}

static void initProc() {
  compass.mag_config();
}

static AsyncStarter _init(initProc);
