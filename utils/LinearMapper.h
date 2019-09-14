// Part of the Almabraxas 3-1 project https://tronche.com/wiki/
// (c) Ch. Tronche 2018 (ch@tronche.com)
// MIT License

#ifndef _LINEAR_MAPPER_H_
#define _LINEAR_MAPPER_H_

class LinearMapper {

 public:
  LinearMapper() { }
  LinearMapper(int from0, float to0, int from1, float to1) {computeFactor(from0, to0, from1, to1);}
  void computeFactor(int from0, float to0, int from1, float to1);
  float convert(int v) { return a * v + b; }

 public:
  float a, b;
};

#endif // _LINEAR_MAPPER_H_
