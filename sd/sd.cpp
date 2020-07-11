#include "mbed.h"

#include "FATFileSystem.h"
#include "SDBlockDevice.h"
#define DEVICE_SPI_COUNT
#include "spi_api.h"

#include "wiring.h"

extern "C" SPIName spi_get_peripheral_name(PinName mosi, PinName miso, PinName mclk);

static SDBlockDevice blockDevice(PB_5, PB_4, PB_3, PB_10, 5625000, 1);
static FATFileSystem fileSystem("fs");

void sd_init() {
  int err = fileSystem.mount(&blockDevice);
  printf("sd mount res = %d\n", err);
}
