// Part of the Almabraxas 3-1 project https://tronche.com/wiki/
// (c) Ch. Tronche 2020 (ch@tronche.com)
// MIT License

#ifndef _ALMA_FLAGS_H_
#define _ALMA_FLAGS_H_

extern bool flag_copy_radio_to_serial;
extern bool flag_copy_sdlog_to_serial;

static const int FLAG_COPY_RADIO_TO_SERIAL = 0;
static const int FLAG_COPY_SDLOG_TO_SERIAL = 1;

void setFlag(uint8_t flag, bool value);

void flag_init();

#endif // _ALMA_FLAGS_H_
