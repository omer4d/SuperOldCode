#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <allegro.h>
#include "vector.h"

#define ASPECT_RATIO ((float)SCREEN_W / (float)SCREEN_H)
#define SCREEN_HW ((float)(SCREEN_W >> 1))
#define SCREEN_HH ((float)(SCREEN_H >> 1))

#define WHITE makecol(255, 255, 255)
#define RED makecol(255, 0, 0)
#define GREEN makecol(0, 255, 0)
#define BLUE makecol(0, 0, 255)

BITMAP *GFX_SURFACE;
int GFX_COLOR = 0, GFX_CAPPED_SEGMENT = 1;
float GFX_POINT_RADIUS = 0.01, GFX_SEGMENT_WIDTH = 0.02,
      GFX_TX = 0.0, GFX_TY = 0.0, GFX_TZ = 256.0;

VECTOR transform(VECTOR v)
{
 VECTOR vt;

 vt.x = 256.0 * (v.x + GFX_TX) / GFX_TZ;
 vt.y = 256.0 * (v.y + GFX_TY) / GFX_TZ;
 vt.x = vt.x * SCREEN_HW + SCREEN_HW;
 vt.y = vt.y * ASPECT_RATIO * SCREEN_HH + SCREEN_HH;

 return vt;
}

void vector_point(VECTOR p)
{
 p = transform(p);
 circlefill(GFX_SURFACE, (int)p.x, (int)p.y,
                         (int)((256.0 * GFX_POINT_RADIUS / GFX_TZ) * SCREEN_HW), GFX_COLOR);
}

void vector_line(VECTOR a, VECTOR b)
{
 a = transform(a);
 b = transform(b);
 line(GFX_SURFACE, (int)a.x, (int)a.y,
                   (int)b.x, (int)b.y, GFX_COLOR);
}

void vector_poly(VECTOR poly[], int n)
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

  polygon(GFX_SURFACE, n, points, GFX_COLOR);
  free(points);
 }

 else
  vector_line(poly[0], poly[1]);
}

void vector_segment(VECTOR a, VECTOR b)
{
 VECTOR n = USCALE_VECTOR(normalized_vector(VECTOR_NORMAL(VECTOR_DIFF(a, b))), GFX_SEGMENT_WIDTH * 0.5),
        temp[4] = {VECTOR_DIFF(a, n), VECTOR_SUM(a, n), VECTOR_SUM(b, n), VECTOR_DIFF(b, n)};
 float t = GFX_POINT_RADIUS;

 if(GFX_CAPPED_SEGMENT)
  {
   GFX_POINT_RADIUS = GFX_SEGMENT_WIDTH * 0.5;
   vector_point(a);
   vector_point(b);
   GFX_POINT_RADIUS = t;
  }

 vector_poly(temp, 4);
}

void vector_fillrect(VECTOR min, VECTOR max)
{
 max = transform(max), min = transform(min);
 rectfill(GFX_SURFACE, (int)min.x, (int)min.y, (int)max.x, (int)max.y, GFX_COLOR);
}

int blend(int c1, int c2, float a)
{
 int r1, g1, b1, r2, g2, b2;

 r1 = getr(c1); g1 = getg(c1); b1 = getb(c1);
 r2 = getr(c2); g2 = getg(c2); b2 = getb(c2);

 return makecol((int)(r1 * a + r2 * (1.0 - a)),
                (int)(g1 * a + g2 * (1.0 - a)),
                (int)(b1 * a + b2 * (1.0 - a)));
}

#endif
