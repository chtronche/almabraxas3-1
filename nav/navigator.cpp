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

void distAndHeading(float lat0, float lon0, float lat1, float lon1,
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

static cell feuch1[] = {
  48.87794375473663,1.975363849732992,
  48.8780274694703,1.975102689568917,
  48.87803093815558,1.975353484818965,
  48.87790650705719,1.975809665021118,
  48.8776829288465,1.97670312231099,
  48.87746875696516,1.977669886056208,
  48.87711098266111,1.977430197980181,

  1000,1000
};

static cell *navPlan = feuch1;

#include "reporting.h"

static float followingBearing = 10;
static float cos_followingBearing, sin_followingBearing;
static const cell *currentTarget = NULL;

static bool lastPoint = false;

static unsigned bearing_degree(float bearing_radian) {
  int v = int(bearing_radian * 180.0 / M_PI);
  return v >= 0 ? v : 360 + v;
}

static void setTarget(unsigned navPlan_index) {
  if (uNavPnt == navPlan_index or lastPoint) return;
  currentTarget = navPlan + navPlan_index;
  NV<uint16_t>::set("uNavPnt", &uNavPnt, navPlan_index);
  const cell *nextTarget = currentTarget + 1;
  if (nextTarget->lon > 400) {
    lastPoint = true;
    return; // done
  }
  float discard;
  distAndHeading(currentTarget->lat, currentTarget->lon, nextTarget->lat, nextTarget->lon,
		 discard, followingBearing);
  cos_followingBearing = cos(followingBearing);
  sin_followingBearing = sin(followingBearing);
}

// NVStore_init first

void nav_init() {
  //getu("uNavPnt", &uNavPnt);
  vars_register("uNavPnt", &uNavPnt);
  NV<uint16_t>::get("uNavPnt", &uNavPnt);
  uNavPnt = -1; // debug
  setTarget(0);
  //retrieve navPlan
  sdlog("up", "nav");
}

float computeTargetHeading(float lat, float lon) {
  const cell *target = navPlan + uNavPnt;
  float distanceM, bearing;
  distAndHeading(lat, lon, target->lat, target->lon, distanceM, bearing);
  char buffer[128];
  buffer[126] = '@';
  buffer[127] = '\0';
  snprintf(buffer, 126, "%d lat=%f\tlon=%f\ttlat=%f\ttlon=%f\td=%f\tH=%d\tfH=%f",
	  uNavPnt,
	  lat, lon, target->lat, target->lon, distanceM, bearing_degree(bearing), bearing);
  sdlog("nav", buffer);
  
  if (lastPoint) return bearing; // We turn around the last point

 // dot product of unit vectors
  float cos_angle = cos_followingBearing * cos(bearing) + sin_followingBearing * sin(bearing);
  snprintf(buffer, 126, "FB=%d\tcos=%f", bearing_degree(followingBearing), cos_angle);
  sdlog("nav/2", buffer);

  // if cos < 0 ( angle between trajectory to next point and next
  // point to the following point > +/- 90 degrees), we consider we
  // have passed the point and go to the next one. This means this
  // planner can't handle angles > 90 degrees, if you have a 90+
  // degrees turn, you must decompose in into two less steep turns.
  if (cos_angle >= 0) return bearing;

  setTarget(uNavPnt + 1);
  distAndHeading(lat, lon, currentTarget->lat, currentTarget->lon, distanceM, bearing);
  return bearing;
}
