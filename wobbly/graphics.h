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
 line(buffer, a.x, a.y, b.x, b.y, color);
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

void connected_circles(BITMAP *buffer, VECTOR c1, VECTOR c2, float r1, float r2, int col)
{
 VECTOR poly[6];
 float d = VECTOR_LENGTH(VECTOR_DIFF(c1, c2)), k = r1 - r2, alpha = acos(k / d),
       beta = acos(-k / d), ang;

 ang = angle_between_vector_and_up(c1, c2);
 poly[0].x = c1.x + r1 * cos(alpha + ang + M_PI * 1.5);
 poly[0].y = c1.y + r1 * sin(alpha + ang + M_PI * 1.5);
 poly[2].x = c1.x + r1 * cos(-alpha + ang + M_PI * 1.5);
 poly[2].y = c1.y + r1 * sin(-alpha + ang + M_PI * 1.5);

 ang = angle_between_vector_and_up(c2, c1);
 poly[3].x = c2.x + r2 * cos(beta + ang + M_PI * 1.5);
 poly[3].y = c2.y + r2 * sin(beta + ang + M_PI * 1.5);
 poly[5].x = c2.x + r2 * cos(-beta + ang + M_PI * 1.5);
 poly[5].y = c2.y + r2 * sin(-beta + ang + M_PI * 1.5);

 poly[1] = c1;
 poly[4] = c2;

 circlefill(buffer, c1.x, c1.y, r1, col);
 circlefill(buffer, c2.x, c2.y, r2, col);
 if(d > fabs(k))
  vector_poly(buffer, poly, 6, col);
}

#endif
