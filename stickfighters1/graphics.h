#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <allegro.h>
#include "vector.h"


float gfx_scale = 1.0;

void vector_point(BITMAP *buffer, VECTOR p, float radius, int color)
{
 circlefill(buffer, (int)(p.x * gfx_scale), (int)(p.y * gfx_scale), (int)(radius * gfx_scale), color);
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
    points[i << 1] = (int)(poly[i].x * gfx_scale);
    points[(i << 1) + 1] = (int)(poly[i].y * gfx_scale);
   }

  polygon(buffer, n, points, color);
  free(points);
 }

 else
  vector_line(buffer, poly[0], poly[1], color);
}
/*
void vector_segment(BITMAP *buffer, VECTOR a, VECTOR b, float radius, int color, int flag)
{
 VECTOR n = USCALE_VECTOR(normalized_vector(VECTOR_NORMAL(VECTOR_DIFF(a, b))), radius),
        temp[4] = {VECTOR_DIFF(a, n), VECTOR_SUM(a, n), VECTOR_SUM(b, n), VECTOR_DIFF(b, n)};

 vector_point(buffer, a, radius * 0.95, color);
 vector_point(buffer, b, radius * 0.95, color);
 
 if(flag == 1)
 {
circle(buffer, (int)a.x, (int)a.y, (int)(radius * 0.95), 0);
circle(buffer, (int)b.x, (int)b.y, (int)(radius * 0.95), 0);
}

 vector_poly(buffer, temp, 4, color);
 //vector_line(buffer, temp[0], temp[1], 0);
 vector_line(buffer, temp[1], temp[2], 0); 
 //vector_line(buffer, temp[2], temp[3], 0); 
 vector_line(buffer, temp[3], temp[0], 0); 
}
*/

void vector_segment(BITMAP *buffer, VECTOR a, VECTOR b, float radius, int color, int flag)
{
 VECTOR n = USCALE_VECTOR(normalized_vector(VECTOR_NORMAL(VECTOR_DIFF(a, b))), radius),
        temp[4] = {VECTOR_DIFF(a, n), VECTOR_SUM(a, n), VECTOR_SUM(b, n), VECTOR_DIFF(b, n)};

 vector_point(buffer, a, radius * 0.95, color);
 vector_point(buffer, b, radius * 0.95, color);

 vector_poly(buffer, temp, 4, color);
}

void outlined_vector_segment(BITMAP *buffer, VECTOR a, VECTOR b, float radius, int color, int flag)
{
 VECTOR n = USCALE_VECTOR(normalized_vector(VECTOR_NORMAL(VECTOR_DIFF(a, b))), radius),
        temp[4] = {VECTOR_DIFF(a, n), VECTOR_SUM(a, n), VECTOR_SUM(b, n), VECTOR_DIFF(b, n)};

 vector_point(buffer, a, radius * 0.95, color);
 vector_point(buffer, b, radius * 0.95, color);

 vector_poly(buffer, temp, 4, color);
 
  vector_line(buffer, temp[0], temp[1], 0);
 vector_line(buffer, temp[1], temp[2], 0); 
 vector_line(buffer, temp[2], temp[3], 0); 
 vector_line(buffer, temp[3], temp[0], 0); 
 circle(buffer, (int)a.x, (int)a.y, (int)(radius * 0.95), 0);
circle(buffer, (int)b.x, (int)b.y, (int)(radius * 0.95), 0);
}

#endif
