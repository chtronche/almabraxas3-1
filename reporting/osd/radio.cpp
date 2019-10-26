#include "mbed.h"
#include "AsyncStarter.h"
#include "RFM69.h"
#include "RFM69registers.h"

#include "crc.h"
#include "wiring.h"

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
}

static AsyncStarter _init(_initProc);

void radioSendFrame(unsigned len, const char *s) {
  _init.ready();
  if (len < 58) {
    ((char *)s)[len] = computeCRC(len, s);
    ++len;
  }
  rfm69.send(99, s, len, false);
  rfm69.receiveDone();
}

static char buffer[RF69_MAX_DATA_LEN + 1];

#define MIN(a, b) ((a) > (b) ? (b) : (a))

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
