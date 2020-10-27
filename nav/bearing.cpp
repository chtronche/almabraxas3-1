#include "mbed.h"

#include "nav.h"
#include "sdlog.h"

////////////////////////////////////////////////////////////////////////////////
//
// Circular buffer of last lat / lon fixes. Use to compute "ground" track.
//
////////////////////////////////////////////////////////////////////////////////

static const int _bufferSize = 64; // power of two please

class _CircularBuffer { // Fixed size, always full

  friend class _Iterator;

public:
  _CircularBuffer();

  void add(float lat, float lon);

private:
  cell _buffer[_bufferSize];
  int _index;
  
};

_CircularBuffer::_CircularBuffer() {
  _index = 0;
  for(int i = _bufferSize - 1; i >= 0; --i) {
    // You're not supposed to be here, and if you are, it's ok anyway.
    add(27.986065, 86.922623);
  }
}

void _CircularBuffer::add(float lat, float lon) {
  _index = (_index + 1) % _bufferSize;
  cell &p = _buffer[_index];
  p.lat = lat;
  p.lon = lon;
}

class _Iterator {
public:
  _Iterator(const _CircularBuffer &b):_buffer(b._buffer), _pos(b._index), _n(_bufferSize) { }
  bool atEnd() const { return _n <= 0; }

  void prev() {
    if (atEnd()) return;
    --_pos;
    --_n;
    if (_pos < 0) _pos = _bufferSize - 1;
  }

  const cell &value() const { return _buffer[_pos]; }

private:
  const cell *_buffer;
  int _pos;
  int _n;
};

////////////////////////////////////////////////////////////////////////////////
//
// Now the real meat
//
////////////////////////////////////////////////////////////////////////////////

_CircularBuffer _buffer;

uint8_t magneticHeading = 0;

static void computeBearing_fromTracking(float lat, float lon,
					const cell **last,
					uint16_t *heading,
					bool *headingIsMagnetic) {
  if (lat > 95) return; // No fix
  _buffer.add(lat, lon);
  _Iterator i(_buffer);
  // Starts at -1, since current coords are at the end of the buffer
  for(i.prev(); !i.atEnd(); i.prev()) {
    float distanceM, bearing;
    const cell &c = i.value();
    *last = &c;
    distAndHeading(c.lat, c.lon, lat, lon, distanceM, bearing);    
    //printf("===%f\t%f\t%f\t%f\t%f\t%f\n", c.lat, c.lon, lat, lon, distanceM, bearing / M_PI * 180);
    // Shouldn't these values be configurable ?
    if (distanceM < 20) continue;
    if (distanceM > 1000) return; // We're lacking some fixes (or cruising at mach 0.6...)
    *headingIsMagnetic = false;
    *heading = int8_t(bearing * 128 / M_PI);
    return;
  }
}

static void computeBearing(float lat, float lon, uint16_t *heading, bool *headingIsMagnetic) {
  magneticHeading = *heading = getMagneticHeading();
  *headingIsMagnetic = true; // We'll change that if we get heading from tracking
  const cell *last;
  computeBearing_fromTracking(lat, lon, &last, heading, headingIsMagnetic);
  char buffer[128];
  snprintf(buffer, 128, "%f\t%f\t%f\t%f\tMH=%d\t%u\t%d", last->lat, last->lon, lat, lon,
	   magneticHeading, *heading & 0xff, *headingIsMagnetic);
  sdlog("bearing", buffer);
}

uint16_t heading;
bool headingIsMagnetic;
volatile uint8_t targetHeading;

// This is where PID should take place
// Called by the GPS reading loop

void bearing_loop(float lat, float lon) {
  computeBearing(lat, lon, &heading, &headingIsMagnetic);
  if (lat > 95) return;
  float targetHeading_ = computeTargetHeading(lat, lon);
  int n = targetHeading_ * 128 / M_PI;
  targetHeading = uint8_t(n); // Not clear why doing it directly doesn't work for negative values
}
