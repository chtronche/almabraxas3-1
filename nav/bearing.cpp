#include "mbed.h"

#include "nav.h"

static const int bufferSize = 64; // power of two please

class _CircularBuffer {

  friend class _Iterator;

public:
  _CircularBuffer() { reset(); }

  void reset() { _index = 0; _n = 0; }

  void add(float lat, float lon) {
    cell &p = _buffer[_index];
    p.lat = lat;
    p.lon = lon;
    _index = (_index + 1) % bufferSize;
    if (_n < bufferSize) ++_n;
  }
  
private:
  cell _buffer[bufferSize];
  int _index, _n;
  
};

class _Iterator {
public:
  _Iterator(const _CircularBuffer &b):_buffer(b), _pos((b._index - 1) % bufferSize), _n(_buffer._n) { }
  bool atEnd() const { return _n; }

  void prev() {
    _pos = (_pos - 1) % bufferSize;
    --_n;
  }

  const cell &value() const { return _buffer._buffer[_pos]; }

private:
  const _CircularBuffer &_buffer;
  int _pos;
  int _n;
};

_CircularBuffer _buffer;

uint8_t magneticHeading = 0;

static void computeBearing(float lat, float lon, uint16_t *heading, bool *headingIsMagnetic) {
  uint8_t heading_ = getMagneticHeading();
  magneticHeading = heading_;
  bool headingIsMagnetic_ = true;

  _buffer.add(lat, lon);
  _Iterator i(_buffer);
  i.prev();
  for(; !i.atEnd(); i.prev()) {
    const cell &c = i.value();
    float distanceM, bearing;
    distAndHeading(c.lon, c.lat, lon, lat, distanceM, bearing);
    if (distanceM >= 20) {
      heading_ = uint8_t(bearing * 128 / M_PI);
      headingIsMagnetic_ = false;
      break;
    }
  }
  *heading = heading_;
  *headingIsMagnetic = headingIsMagnetic_;
}

void noFix() {
  _buffer.reset();
}

// This is where PID should take place
// Called by the GPS reading loop

uint16_t heading;
bool headingIsMagnetic;

void bearing_loop(float lat, float lon) {
  computeBearing(lat, lon, &heading, &headingIsMagnetic);
  float targetHeading = computeTargetHeading(lat, lon);
  //heading = XXX;
}