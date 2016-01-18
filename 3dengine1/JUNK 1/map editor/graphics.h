#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <allegro.h>
#include "vector.h"

void vector_point(BITMAP *buffer, VECTOR p, float radius, int color)
{
 circlefill(buffer, (int)(p.x), (int)(p.y), (int)(radius), color);
}

void vector_line(BITMAP *buffer, VECTOR a, VECTOR b, int color)
{
 vector_point(buffer, a, 2, color);
 vector_point(buffer, b, 2, color);
 line(buffer, (int)a.x, (int)a.y, (int)b.x, (int)b.y, color);
}

void vector_poly(BITMAP *buffer, VECTOR poly[], int n, int color)
{
 int *points, i;

 if(n > 2)
 {
  points = (int *)malloc((n << 1) * sizeof(int));

  for(i = 0; i < n; i++)
   {
    points[i << 1] = (int)(poly[i].x);
    points[(i << 1) + 1] = (int)(poly[i].y);
   }

  polygon(buffer, n, points, color);
  free(points);
 }

 else
  vector_line(buffer, poly[0], poly[1], color);
}

void vector_loop(BITMAP *buffer, VECTOR p[], int n, int flag, int color)
{
 int i;

 if(flag < 0)
  for(i = 0; i < n; i++)
   vector_point(buffer, p[i], -flag, color);

 else
  {
   for(i = 0; i < n - 1; i++)
    vector_line(buffer, p[i], p[i + 1], color);
   if(flag)
    vector_line(buffer, p[i], p[0], color);
  }
}

#endif
