#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <math.h>

typedef struct {
 float x, y;
}VECTOR;

VECTOR vector(float x, float y)
{
 VECTOR v;

 v.x = x;
 v.y = y;

 return v;
}

VECTOR *vector_array(int n)
{
 VECTOR *array;
 array = (VECTOR *)malloc(n * sizeof(VECTOR));
 return array;
}

#define SQUARED(n) ((n) * (n))
#define ZERO_VECTOR vector(0.0, 0.0)
#define VECTOR_SUM(a, b) vector(a.x + b.x, a.y + b.y)
#define VECTOR_DIFF(a, b) vector(a.x - b.x, a.y - b.y)
#define USCALE_VECTOR(v, k) vector(v.x * (k), v.y * (k))
#define VECTOR_DOT_PRODUCT(a, b) (a.x * b.x + a.y * b.y)
#define VECTOR_NORMAL(v) vector(-v.y, v.x)
#define VECTOR_LENGTH(v) (sqrt(VECTOR_DOT_PRODUCT(v, v)))
#define VECTOR_SLOPE(v) (v.y / (v.x + 0.1))
#define NORMALIZED_VECTOR(v) (USCALE_VECTOR(v, 1.0 / VECTOR_LENGTH(v)))
#define NORMALIZED_NORMAL(a, b) (NORMALIZED_VECTOR(VECTOR_NORMAL(VECTOR_DIFF(a, b))))

float sq_dist(VECTOR a, VECTOR b)
{
 VECTOR d = VECTOR_DIFF(a, b);
 return VECTOR_DOT_PRODUCT(d, d);
}

float approx_vector_length(VECTOR v)
{
 float dx = fabs(v.x), dy = fabs(v.y);

 if(dy > dx)
  return 0.41 * dx + 0.941246 * dy;
 return 0.41 * dy + 0.941246 * dx;
}

float angle_between_vectors(VECTOR o, VECTOR a, VECTOR b)
{
 VECTOR t1 = VECTOR_DIFF(a, o), t2 = VECTOR_DIFF(b, o);
 float result;

 result = atan2(t1.y, t1.x) - atan2(t2.y, t2.x);

 if(result < 0.0)
  result += 2.0 * M_PI;

 return result;
}

float angle_between_vector_and_up(VECTOR o, VECTOR a)
{
 VECTOR t1 = VECTOR_DIFF(a, o);
 float result;

 result = atan2(t1.y, t1.x) + M_PI * 0.5;

 if(result < 0.0)
  result += 2.0 * M_PI;

 return result;
}

#endif
