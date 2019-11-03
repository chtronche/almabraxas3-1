#include <math.h>

static float radians(float degrees) {
  return degrees * M_PI / 180.0;
}

static float degrees(float radians) {
  return radians * 180.0 / M_PI;
}

static float sqr(float x) { // Come on guys
  return x * x;
}

// (Haversine formula)
// Input: latitude / longitude of both points in degrees and fraction of degrees
// Output: distanceM the distance in meters and bearing the bearing
// from first to second point in positive degrees

static void distAndHeading(float lon0, float lat0, float lon1, float lat1,
			   float &distanceM, float &bearing) {
  float deltaLat = lat1 - lat0;
  float deltaLon = lon1 - lon0;
  float clat0 = cos(lat0);
  float clat1 = cos(lat1);

  float a = sqr(sin(deltaLat / 2)) + clat0 * clat1 * sqr(sin(deltaLon / 2));
  float c = 2 * atan2(sqrt(a), sqrt(1-a));
  distanceM = 6371000.0 * c;

  float y = sin(deltaLon) * clat1;
  float x = clat0 * sin(lat1) - sin(lat0) * clat1 * cos(deltaLon);
  float b = degrees(atan2(y, x));
  if (b < 0) b += 360.0;
  bearing = b;
}

static float lac[] = {
  1.952664207039667,49.00105361353885,
  1.949870378296534,49.00136230736817,
  1.948094574055879,49.00162388841866,
  1.948151244254832,49.00241504599636,
  1.948520433020271,49.00330612030971,
  1.948790348736489,49.00375444307392,
  1.95025982583839,49.00353519293997,
  1.951547771439659,49.003192138575,
  1.952646509745837,49.0027685170443,
  1.953405899517102,49.00239759104883,
  1.953658568078296,49.00153104880626,
  1.954872174029409,49.00116264717977,

  -2.822731870550616,46.17390701395785,
  -37.57502725067546,41.06302499630559,
  -66.85599729490329,32.9313371535174,
  -81.0577800180538,29.99534456797493,

  1000, 1000
};

#include "mbed.h"
#include "reporting.h"

static char buffer[128];

void test_nav() {
  float *p = lac - 1;
  
  float _lon0 = *++p;
  float lon0 = radians(_lon0);
  float lat0 = radians(*++p);
  while(_lon0 < 400) {
    float d, bearing;
    float _lon1 = *++p;
    float lon1 = radians(_lon1);
    float _lat1 = *++p;
    float lat1 = radians(_lat1);
    distAndHeading(lon0, lat0, lon1, lat1, d, bearing);
    sprintf(buffer, "%f\t%f\t%f\t%f", _lon1, _lat1, d, bearing);
    reporting_debug_print_serial(buffer);
    _lon0 = _lon1;
    lon0 = lon1;
    lat0 = lat1;
  }
}
