#include "mbed.h"
#include "AsyncStarter.h"
#include "RFM69.h"
#include "RFM69registers.h"

#include "adebug.h"
#include "alma_math.h"
#include "crc.h"
#include "wiring.h"
#include "sdlog.h"

RFM69 rfm69(RFM69_MOSI, RFM69_MISO, RFM69_SCLK, RFM69_CS, RFM69_INT);
static DigitalOut reset(RFM69_RST);

static void _initProc() {
  reset.write(1);
  wait_us(100);
  reset.write(0);
  wait_us(5000);
  rfm69.initialize(RF69_433MHZ, 'P', 21);
  rfm69.setPowerLevel(31);
  rfm69.promiscuous();
  rfm69.writeReg(REG_PALEVEL, 0x7f);
  rfm69.writeReg(REG_OCP, 0x0f);
  rfm69.writeReg(REG_BITRATEMSB, RF_BITRATEMSB_100000);
  rfm69.writeReg(REG_BITRATELSB, RF_BITRATELSB_100000);
}

static AsyncStarter _init(_initProc);

static uint32_t _nextCheck = 0;

void radioCheck(uint32_t clock) {
  if (clock < _nextCheck) return;
  _nextCheck = clock + 600;
  char buffer[128];
  sprintf(buffer, "radioCheck %x", rfm69.readReg(REG_BITRATEMSB));
  sdlog("radioCheck", buffer);
}

void radioSendFrame(unsigned len, const char *s) {
  led(0x4, 0xc);
  _init.ready();
  if (len < 58) {
    ((char *)s)[len] = computeCRC(len, s);
    ++len;
  }
  led(0x8, 0xc);
  rfm69.send(99, s, len, false);
  led(0xc, 0xc);
  rfm69.receiveDone();
  led(0, 0xc);
}

static char buffer[RF69_MAX_DATA_LEN + 1];

char *readRadioPacket() {
  _init.ready();
  if (!rfm69.receiveDone()) return NULL;
  unsigned len = MIN(rfm69.DATALEN, RF69_MAX_DATA_LEN);
  memcpy(buffer, (void *)rfm69.DATA, len);
  buffer[len] = '\0';
  return buffer;
}

int16_t getRSSI() {
  return rfm69.RSSI;
}
