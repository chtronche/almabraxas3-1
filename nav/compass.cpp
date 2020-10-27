#include "FXOS8700.h"
#include "sdlog.h"
#include "wiring.h"

static FXOS8700 compass(ALMA_SDA, ALMA_SCL);

uint8_t getMagneticHeading() {
  float mag[3];
  compass.acquire_mag_data_uT(mag);
  int heading_ = atan2(mag[1], mag[0]) * 128 / M_PI;
  if (heading_ < 0) heading_ += 256;
  //heading_ = (heading_ + 90) % 256;
  uint8_t heading = heading_;
  return heading;
}

void compass_init() {
  compass.mag_config();
  sdlog("up", "compass");
}
