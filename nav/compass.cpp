#define private public // Access FXOS8700 I2C for init
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
  char byte;
  byte = FXOS8700_WHO_AM_I;
  compass.accelmagi2c.write(FXOS8700_I2C_ADDRESS, &byte, 1, true);
  compass.accelmagi2c.read(FXOS8700_I2C_ADDRESS, &byte, 1);
  sdlog("compass", byte == 0xc7 ? "up" : "init error");
}
