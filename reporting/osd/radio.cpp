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

void radio_reset() {
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

static void _initProc() {
  radio_reset();
}

static AsyncStarter _init("t/radio", _initProc);

static uint32_t _nextCheck = 0;

bool radio_is_sleeping = false; // Should use NV flag

void radioCheck(uint32_t clock) {
  if (clock < _nextCheck) return;
  _nextCheck = clock + 600;
  char buffer[128];
  sprintf(buffer, "radioCheck %x (should be %d) sleeping=%d", rfm69.readReg(REG_BITRATEMSB), 
	  RF_BITRATEMSB_100000, radio_is_sleeping);
  sdlog("radioCheck", buffer);
}

void radioSendFrame(unsigned len, const char *s) {
  if (radio_is_sleeping) return;
  led_set(led_white, 0);
  _init.ready();
  if (len < RF69_MAX_DATA_LEN - 1) {
    ((char *)s)[len] = computeCRC(len, s);
    ++len;
  } else {
    printf("radio packet too long !");
  }
  led_set(led_blue, led_white);
  Timer t;
  t.start();
  rfm69.send(99, s, len, false);
  t.stop();
  led_set(led_white, 0);
  printf("%d bytes sent in %d us\n", len, t.read_us());
  rfm69.receiveDone();
  led_set(0, led_white | led_blue);
}

static char buffer[RF69_MAX_DATA_LEN + 1];

char *readRadioPacket() {
  if (radio_is_sleeping) return NULL;
  _init.ready();
  if (!rfm69.receiveDone()) return NULL;
  unsigned len = MIN(rfm69.DATALEN, RF69_MAX_DATA_LEN);
  memcpy(buffer, (void *)rfm69.DATA, len);
  buffer[len] = '\0';
  if (!strncmp(buffer, "pi ng ", 6)) return buffer; // pi ng isn't echoed
  char _buffer[128];
  snprintf(_buffer, 128, ">>%s", buffer); // Echo back command
  radioSendFrame(strlen(_buffer), _buffer);
  return buffer;
}


int16_t getRSSI() {
  return rfm69.RSSI;
}

void radio_sleep() {
  if (radio_is_sleeping) return;
  radio_is_sleeping = true;
  rfm69.sleep();
}

static const char _radioWakeupStr[] = ">radio wakeup";

void radio_wakeup() {
  if (!radio_is_sleeping) return;
  radio_is_sleeping = false;
  //  radioSendFrame(strlen(_radioWakeupStr), _radioWakeupStr);
}
