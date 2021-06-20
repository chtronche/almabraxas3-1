// Part of the Almabraxas 3-1 project https://tronche.com/wiki/
// (c) Ch. Tronche 2018 (ch@tronche.com)
// MIT License

#ifndef _REPORTING_H_
#define _REPORTING_H_

void reporting_debug_print(const char *);

const char *reporting_serial_read();
void reporting_init();
void reporting_loop();

void reporting_get_description(unsigned);

void radio_sleep();
void radio_wakeup();

extern bool radio_is_sleeping;

void satellite_init(); // initialize NVStore first
void satellite_armClock(const char *date);
void satellite_loop();

void radio_reset();

// Impl

void radioSendFrame(unsigned len, const char *);
char *readRadioPacket();

int16_t getRSSI();

void reporting_init();
void reporting_serial_init();

#endif // _REPORTING_H_
