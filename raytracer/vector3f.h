#ifndef _VECTOR3F_H_
#define _VECTOR3F_H_

#include <math.h>

#define SQUARED(n) ((n) * (n))
#define DOT(a, b) (a.x * b.x + a.y * b.y + a.z * b.z)

class cVector3f {
 public:
  float x, y, z;

  cVector3f();
  cVector3f operator + (cVector3f);
  cVector3f operator - (cVector3f);
  cVector3f operator * (float);
  float length();
  void normalize();
};

cVector3f vector3f(float, float, float);

#endif
