#ifndef _SAT_2D_H_
#define _SAT_2D_H_

#include "vec2f.h"

typedef struct {
 float min, max;
}INTERVAL;

int intervals_intersect(INTERVAL int0, INTERVAL int1)
{
 if(int0.min > int1.max || int1.min > int0.max)
  return 0;
 return 1;
}

INTERVAL poly_interval_on_axis(VEC2F poly[], int n, VEC2F axis)
{
 int i;
 float temp;
 INTERVAL interval;

 interval.min = interval.max = VEC2F_DOT_PRODUCT(axis, poly[0]);

 for(i = 1; i < n; i++)
  {
   temp = VEC2F_DOT_PRODUCT(axis, poly[i]);

   if(temp < interval.min) { interval.min = temp; }
   if(temp > interval.max) { interval.max = temp; }
  }

 return interval;
}

int poly_intersection_test(VEC2F a[], VEC2F b[], VEC2F an[], VEC2F bn[], int n0, int n1)
{
 VEC2F axis;
 int i;

 for(i = 0; i < n0; i++)
  if(!intervals_intersect(poly_interval_on_axis(a, n0, an[i]), poly_interval_on_axis(b, n1, an[i])))
   return 0;

 for(i = 0; i < n1; i++)
  if(!intervals_intersect(poly_interval_on_axis(a, n0, bn[i]), poly_interval_on_axis(b, n1, bn[i])))
   return 0;

 return 1;
}

#endif
