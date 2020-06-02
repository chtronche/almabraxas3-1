// Part of the Almabraxas 3-1 project https://tronche.com/wiki/
// (c) Ch. Tronche 2018 (ch@tronche.com)
// MIT License

#ifndef _LINEAR_MAPPER_H_
#define _LINEAR_MAPPER_H_

#include <stdint.h>

class LinearMapper {

 public:
  LinearMapper() { }
  LinearMapper(int from0, float to0, int from1, float to1);
  virtual void resetPoint(int from, float to, bool highPoint);
  
  float convert(int v) const { return a * v + b; };

 public:
  float a, b;

 protected:
  void computeFactor(int16_t from0, float to0, int16_t from1, float to1);

 protected:
  int16_t f0, f1;
  float t0, t1;
};

class NVLinearMapper: public LinearMapper {
 public:
  NVLinearMapper(const char *f0Name, const char *t0Name, const char *f1Name, const char *t1Name);
  void retrieve();
  virtual void resetPoint(int16_t from, float to, bool highPoint);

 private:
  const char *_f0Name, *_t0Name, *_f1Name, *_t1Name;
};

#endif // _LINEAR_MAPPER_H_
