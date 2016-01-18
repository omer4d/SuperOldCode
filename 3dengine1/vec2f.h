#ifndef _VEC2F_H_
#define _VEC2F_H_

#include <math.h>
#include "common.h"

typedef struct {
 float x, y;
}VEC2F;

VEC2F vec2f(float x, float y)
{
 VEC2F v;

 v.x = x;
 v.y = y;

 return v;
}

VEC2F *vec2f_array(int n)
{
 VEC2F *array;
 array = (VEC2F *)malloc(n * sizeof(VEC2F));
 return array;
}

#define ZERO_VEC2F vec2f(0.0, 0.0)
#define VEC2F_SUM(a, b) vec2f(a.x + b.x, a.y + b.y)
#define VEC2F_DIFF(a, b) vec2f(a.x - b.x, a.y - b.y)
#define USCALE_VEC2F(v, k) vec2f(v.x * (k), v.y * (k))
#define VEC2F_DOT_PRODUCT(a, b) (a.x * b.x + a.y * b.y)
#define VEC2F_NORMAL(v) vec2f(-v.y, v.x)
#define VEC2F_LENGTH(v) (sqrt(VEC2F_DOT_PRODUCT(v, v)))
#define NORMALIZED_VEC2F(v) (USCALE_VEC2F(v, 1.0 / VEC2F_LENGTH(v)))
#define NORMALIZED_NORMAL_VEC2F(a, b) (NORMALIZED_VEC2F(VEC2F_NORMAL(VEC2F_DIFF(a, b))))
#define VEC2F_BLEND(a, b, k) vec2f(a.x * k + b.x * (1.0 - k), a.y * k + b.y * (1.0 - k))
#define VEC2F_SLOPE(v) (v.y / avoid_zero(v.x))

float vec2f_sq_dist(VEC2F a, VEC2F b)
{
 VEC2F d = VEC2F_DIFF(a, b);
 return VEC2F_DOT_PRODUCT(d, d);
}

float approx_vec2f_length(VEC2F v)
{
 float dx = fabs(v.x), dy = fabs(v.y);

 if(dy > dx)
  return 0.41 * dx + 0.941246 * dy;
 return 0.41 * dy + 0.941246 * dx;
}

float angle_between_vec2fs(VEC2F o, VEC2F a, VEC2F b)
{
 VEC2F t1 = VEC2F_DIFF(a, o), t2 = VEC2F_DIFF(b, o);
 float result;

 result = atan2(t1.y, t1.x) - atan2(t2.y, t2.x);

 if(result < 0.0)
  result += 2.0 * M_PI;

 return result;
}

float angle_between_vec2f_and_up(VEC2F o, VEC2F a)
{
 VEC2F t1 = VEC2F_DIFF(a, o);
 float result;

 result = atan2(t1.y, t1.x) + M_PI * 0.5;

 if(result < 0.0)
  result += 2.0 * M_PI;

 return result;
}

#endif
