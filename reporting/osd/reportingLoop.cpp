#include <mbed.h>
#include <string.h>

#include "adebug.h"
#include "alma_flags.h"
#include "reporting.h"

static Serial pc(SERIAL_TX, SERIAL_RX);

void reporting_debug_print(const char *buffer) {
  if (!flag_copy_radio_to_serial) return;
  pc.printf(buffer);
  pc.printf("\n");
}

// =================== Read commands from serial =================================

static const int _bufferSize = 64;

static char _command[_bufferSize];
static char *const _bufferEnd = _command + _bufferSize - 1;
static bool _commandAvailable = false;

static void _readCB(int events) {
  led(1, 0xf);
  _commandAvailable = true;
}

static event_callback_t _readCBE = _readCB; // Implicit attach I guess

const char *reporting_serial_read() {
  if (_commandAvailable) {
    _commandAvailable = false;
    flag_copy_sdlog_to_serial = true;
    led(0, 1);
    char *end = (char *)memchr(_command, '\n', _bufferSize);
    if (!end) end = _bufferEnd;
    *end = '\0';
    return _command;
  }
  pc.read((uint8_t *)_command, _bufferSize, _readCBE, SERIAL_EVENT_RX_CHARACTER_MATCH, '\n');
  return NULL;
}

void reporting_serial_init() {
  pc.baud(115200);
  printf("Serial up\n");
}
