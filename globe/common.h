#ifndef _COMMON_H_
#define _COMMON_H_

#include <math.h>
#include "vec3f.h"

#define FLOAT_EPSILON (0.0001)
#define RAD(d) (d * M_PI / 180.0)
#define BIT_WRAP(x, y) (abs((int)x & y))
//#define FLOAT_TO_INT(in, out) out = (int)in;
#define FAST_ROUND(in, out) __asm__ __volatile__ ("fistpl %0" : "=m" (out) : "t" (in) : "st");

#define BSP_EPSILON 0.1
#define BSP_BEHIND 0
#define BSP_INFRONT 1
#define BSP_SPANNED 2

#ifndef SQUARED
 #define SQUARED(n) ((n) * (n))
#endif

float which_side(VEC3F a, VEC3F n, VEC3F p)
{
 return VEC3F_DOT_PRODUCT(VEC3F_DIFF(a, p), n);
}

int random_color()
{
 return makecol(56 + rand() % 200, 56 + rand() % 200, 56 + rand() % 200);
}

int inverse_color(int c)
{
 return makecol(255 - getr(c), 255 - getg(c), 255 - getb(c));
}

int fade_to_black(int c, float k)
{
 if(k < 0.0) { k = 0.0; }
 if(k > 1.0) { k = 1.0; }
 return makecol((int)(getr(c) * k), (int)(getg(c) * k), (int)(getb(c) * k));
}

int fade_to_col(int c, float k, int f)
{
 //if(k < 0.0) { k = 0.0; }
 //if(k > 1.0) { k = 1.0; }
 
 return makecol32((int)(getr32(c) * k + getr32(f) * (1.0 - k)),
                  (int)(getg32(c) * k + getg32(f) * (1.0 - k)),
                  (int)(getb32(c) * k + getb32(f) * (1.0 - k)));
}

int additive_blend(int c1, int c2)
{
 int r1 = getr(c1), g1 = getg(c1), b1 = getb(c1);
 int r2 = getr(c2), g2 = getg(c2), b2 = getb(c2);
 int r = r1 + r2, g = g1 + g2, b = b1 + b2;
 if(r > 255) { r = 255; }
 if(g > 255) { g = 255; }
 if(b > 255) { b = 255; }
 return makecol(r, g, b);
}

float avoid_zero(float k)
{
 if(fabs(k) < FLOAT_EPSILON)
  {
   if(k < 0.0) { return -FLOAT_EPSILON; }
   else { return FLOAT_EPSILON; }
  }

 return k;
}

int ROUND(float f)
{
 if(f < 0.0)
  return (int)(f - 0.5);
 return (int)(f + 0.5);
}

int wrap_int(int x, int min, int max)
{
 int w;

 x -= min;
 max -= min;
 w = x % max;
 if(w < 0) w += max;
 w += min;

 return w;
}

int clamp_int(int x, int min, int max)
{
 if(x < min) return min;
 if(x > max) return max;
 return x;
}

int on_bitmap(int x, int y, int w, int h)
{
 if(x < 0 || x > (w - 1) || y < 0 || y > (h - 1))
  return 0;
 return 1; 
}

int frange(float x, float min, float max)
{
 if(x < min) return 0;
 if(x > max) return 0;
 return 1;
}

#endif
