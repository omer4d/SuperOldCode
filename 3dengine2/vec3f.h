#ifndef _VEC3F_H_
#define _VEC3F_H_

#include <math.h>
//#include "common.h"

typedef struct {
 float x, y, z;
}VEC3F;

VEC3F vec3f(float x, float y, float z)
{
 VEC3F v;

 v.x = x;
 v.y = y;
 v.z = z;

 return v;
}

VEC3F *vec3f_array(int n)
{
 VEC3F *array;
 array = (VEC3F *)malloc(n * sizeof(VEC3F));
 return array;
}

#define ZERO_VEC3F vec3f(0.0, 0.0, 0.0)
#define VEC3F_SUM(a, b) vec3f(a.x + b.x, a.y + b.y, a.z + b.z)
#define VEC3F_DIFF(a, b) vec3f(a.x - b.x, a.y - b.y, a.z - b.z)
#define USCALE_VEC3F(v, k) vec3f(v.x * (k), v.y * (k), v.z * (k))
#define VEC3F_DOT_PRODUCT(a, b) (a.x * b.x + a.y * b.y + a.z * b.z)
#define VEC3F_LENGTH(v) (sqrt(VEC3F_DOT_PRODUCT(v, v)))
#define NORMALIZED_VEC3F(v) (USCALE_VEC3F(v, 1.0 / VEC3F_LENGTH(v)))
#define VEC3F_BLEND(a, b, k) vec3f(a.x * k + b.x * (1.0 - k), a.y * k + b.y * (1.0 - k), a.z * k + b.z * (1.0 - k))
#define VEC3F_CROSS_PRODUCT(a, b) vec3f(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x)

float vec3f_sq_dist(VEC3F a, VEC3F b)
{
 VEC3F d = VEC3F_DIFF(a, b);
 return VEC3F_DOT_PRODUCT(d, d);
}

VEC3F vec3f_normal(VEC3F v0, VEC3F v1, VEC3F v2)
{
 VEC3F a, b, n;

 a = VEC3F_DIFF(v0, v1);
 b = VEC3F_DIFF(v1, v2);
 n = VEC3F_CROSS_PRODUCT(a, b);

 return n;
}

#define NORMALIZED_NORMAL_VEC3F(a, b, c) NORMALIZED_VEC3F(vec3f_normal(a, b, c))

#endif
