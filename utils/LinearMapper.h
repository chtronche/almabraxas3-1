// Part of the Almabraxas 3-1 project https://tronche.com/wiki/
// (c) Ch. Tronche 2018 (ch@tronche.com)
// MIT License

#ifndef _LINEAR_MAPPER_H_
#define _LINEAR_MAPPER_H_

class LinearMapper {

 public:
  LinearMapper() { }
  LinearMapper(int from0, float to0, int from1, float to1);
  void resetPoint(int from, float to, bool highPoint);
  
  float convert(int v) const { return a * v + b; }

 public:
  float a, b;

 private:
  void computeFactor(int from0, float to0, int from1, float to1);

 private:
  int f0, f1;
  float t0, t1;
};

#endif // _LINEAR_MAPPER_H_
