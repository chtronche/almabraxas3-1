#include <math.h>

#include "mbed.h"
#include "nav.h"
#include "NVStore.h"
#include "sdlog.h"
#include "vars.h"

uint16_t uNavPnt = 0;

static float radians(float degrees) {
  return degrees * M_PI / 180.0;
}

// static float degrees(float radians) {
//   return radians * 180.0 / M_PI;
// }

static inline float sqr(float x) { // Come on guys
  return x * x;
}

static const float _2_pi = M_PI * 2;

// (Haversine formula)
// Input: latitude / longitude of both points in degrees and fraction of degrees
// Output: distanceM the distance in meters and bearing the bearing
// from first to second point in radians in [-pi,pi]

void distAndHeading(float lon0, float lat0, float lon1, float lat1,
			   float &distanceM, float &bearing) {
  lon0 = radians(lon0);
  lat0 = radians(lat0);
  lon1 = radians(lon1);
  lat1 = radians(lat1);
    
  float deltaLat = lat1 - lat0;
  float deltaLon = lon1 - lon0;
  float clat0 = cos(lat0);
  float clat1 = cos(lat1);

  float a = sqr(sin(deltaLat / 2)) + clat0 * clat1 * sqr(sin(deltaLon / 2));
  float c = 2 * atan2(sqrt(a), sqrt(1-a));
  distanceM = 6371000.0 * c;

  float y = sin(deltaLon) * clat1;
  float x = clat0 * sin(lat1) - sin(lat0) * clat1 * cos(deltaLon);
  bearing = atan2(y, x);
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

static cell feuch1[] = {
  1.975363849732992,48.87794375473663,
  1.975102689568917,48.8780274694703,
  1.975353484818965,48.87803093815558,
  1.975809665021118,48.87790650705719,
  1.97670312231099,48.8776829288465,
  1.977669886056208,48.87746875696516,
  1.977430197980181,48.87711098266111,

  1000,1000
};

static cell *navPlan = feuch1;

#include "reporting.h"

static char buffer[128];

static float followingBearing = 10;
static float cos_followingBearing, sin_followingBearing;
static const cell *currentTarget = NULL;

static bool lastPoint = false;

static void setTarget(unsigned navPlan_index) {
  if (uNavPnt == navPlan_index or lastPoint) return;
  currentTarget = navPlan + navPlan_index;
  const cell *nextTarget = currentTarget + 1;
  if (nextTarget->lon > 400) {
    lastPoint = true;
    return; // done
  }
  NV<uint16_t>::set("uNavPnt", &uNavPnt, navPlan_index);
  float discard;
  distAndHeading(currentTarget->lon, currentTarget->lat, nextTarget->lon, nextTarget->lat,
		 discard, followingBearing);
  cos_followingBearing = cos(followingBearing);
  sin_followingBearing = sin(followingBearing);
}

// NVStore_init first

void nav_init() {
  //getu("uNavPnt", &uNavPnt);
  vars_register("uNavPnt", &uNavPnt);
  NV<uint16_t>::get("uNavPnt", &uNavPnt);
  printf("nav up\n");
  //setTarget(0);
  //retrieve navPlan
}

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
    sprintf(buffer, "%f\t%f\t%f\t%f", _lon1, _lat1, d, radians(bearing));
    reporting_debug_print(buffer);
    _lon0 = _lon1;
    lon0 = lon1;
    lat0 = lat1;
  }
}

float computeTargetHeading(float lon, float lat) {
  const cell *target = navPlan + uNavPnt;
  float distanceM, bearing;
  distAndHeading(lon, lat, target->lon, target->lat, distanceM, bearing);
  char buffer[128];
  sprintf(buffer, "lon=%f\tlat=%f\ttlon=%f\ttlat=%f\td=%f\tH=%d",
	  lon, lat, target->lon, target->lat, distanceM, int(bearing));
  sdlog("nav", buffer);
  
  if (lastPoint) return bearing; // We turn around the last point

 // dot product of unit vectors
  float cos_angle = cos_followingBearing * cos(bearing) + sin_followingBearing * sin(bearing);

  // if cos < 0 ( angle between trajectory to next point and next
  // point to the following point > +/- 90 degrees), we consider we
  // have passed the point and go to the next one. This means this
  // planner can't handle angles > 90 degrees, if you have a 90+
  // degrees turn, you must decompose in into two less steep turns.
  if (cos_angle >= 0) return bearing;

  setTarget(uNavPnt + 1);
  distAndHeading(lon, lat, currentTarget->lon, currentTarget->lat, distanceM, bearing);
  return bearing;
}
