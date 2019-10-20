// Part of the Almabraxas 3-1 project https://tronche.com/wiki/
// (c) Ch. Tronche 2018 (ch@tronche.com)
// MIT License

#ifndef _REPORTING_H_
#define _REPORTING_H_

uint32_t getClock();

void reporting_debug_print_serial(const char *);
void reporting_debug_print(const char *);

const char *reporting_serial_read();
void reporting_init();
void reporting_loop();

// Impl

void radioSendFrame(unsigned len, const char *);
char *readRadioPacket();

int16_t getRSSI();

#endif // _REPORTING_H_
