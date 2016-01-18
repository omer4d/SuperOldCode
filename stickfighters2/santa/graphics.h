#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <allegro.h>
#include "vector.h"

#define ASPECT_RATIO ((float)SCREEN_W / (float)SCREEN_H)
#define SCREEN_HW ((float)(SCREEN_W >> 1))
#define SCREEN_HH ((float)(SCREEN_H >> 1))

float GFX_TX = 0.0, GFX_TY = 0.0, GFX_TZ = 256.0;

VECTOR transform(VECTOR v)
{
 VECTOR vt;

 vt.x = 256.0 * (v.x + GFX_TX) / GFX_TZ;
 vt.y = 256.0 * (v.y + GFX_TY) / GFX_TZ;
 vt.x = vt.x * SCREEN_HW + SCREEN_HW;
 vt.y = vt.y * ASPECT_RATIO * SCREEN_HH + SCREEN_HH;

 return vt;
}

void vector_point(BITMAP *buffer, VECTOR p, float radius, int color)
{
 p = transform(p);
 circlefill(buffer, (int)p.x, (int)p.y,
                    (int)((256.0 * radius / GFX_TZ) * SCREEN_HW), color);
}

void vector_line(BITMAP *buffer, VECTOR a, VECTOR b, int color)
{
 a = transform(a);
 b = transform(b);
 line(buffer, (int)a.x, (int)a.y,
              (int)b.x, (int)b.y, color);
}

void vector_poly(BITMAP *buffer, VECTOR poly[], int n, int color)
{
 int *points, i;

 if(n > 2)
 {
  points = (int *)malloc((n << 1) * sizeof(int));

  for(i = 0; i < n; i++)
   {
    poly[i] = transform(poly[i]);
    points[i << 1] = (int)poly[i].x;
    points[(i << 1) + 1] = (int)poly[i].y;
   }

  polygon(buffer, n, points, color);
  free(points);
 }

 else
  vector_line(buffer, poly[0], poly[1], color);
}

void vector_segment(BITMAP *buffer, VECTOR a, VECTOR b, float radius, int color)
{
 VECTOR n = USCALE_VECTOR(normalized_vector(VECTOR_NORMAL(VECTOR_DIFF(a, b))), radius),
        temp[4] = {VECTOR_DIFF(a, n), VECTOR_SUM(a, n), VECTOR_SUM(b, n), VECTOR_DIFF(b, n)};

 vector_point(buffer, a, radius, color);
 vector_point(buffer, b, radius, color);

 vector_poly(buffer, temp, 4, color);
}

void vector_fillrect(BITMAP *buffer, VECTOR min, VECTOR max, int color)
{
 max = transform(max), min = transform(min);
 rectfill(buffer, (int)min.x, (int)min.y, (int)max.x, (int)max.y, color);
}

#endif
