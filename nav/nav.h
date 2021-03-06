#ifndef _NAVIGATOR_H_
#define _NAVIGATOR_H_

extern bool fixOk;
extern uint8_t year, month, day, hour, min, sec;

extern uint16_t heading;
extern uint8_t magneticHeading;
extern volatile uint8_t targetHeading;

extern float latf, lonf;

extern uint16_t uNavPnt;

//void test_nav();

// From bearing

void bearing_loop(float lat, float lon);

// From compass

uint16_t getMagneticHeading(); // In 1/100 of degrees

// gps power control

void gps_sleep();
void gps_wakeup();

// From navigator

void distAndHeading(float lat0, float lon0, float lat1, float lon1,
		    float &distanceM, float &bearing);

float computeTargetHeading(float lon, float lat);

typedef struct {
  float lat, lon;
} cell;

void compass_init();
void nav_init();
void gpsLoop_init();

#endif // _NAVIGATOR_H_
