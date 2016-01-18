#ifndef _VEC2F_GRAPH_H_
#define _VEC2F_GRAPH_H_

#include <allegro.h>
#include "vec2f.h"

float VEC2F_GRAPH_tx = 0.0, VEC2F_GRAPH_ty = 0.0;

void vec2f_point(BITMAP *buffer, VEC2F p, float radius, int color)
{
 circlefill(buffer, (int)(p.x + VEC2F_GRAPH_tx),
                    (int)(p.y + VEC2F_GRAPH_ty), (int)(radius), color);
}

void vec2f_line(BITMAP *buffer, VEC2F a, VEC2F b, int color)
{
 //vec2f_point(buffer, a, 2, color);
 //vec2f_point(buffer, b, 2, color);
 line(buffer, (int)(a.x + VEC2F_GRAPH_tx), (int)(a.y + VEC2F_GRAPH_ty),
              (int)(b.x + VEC2F_GRAPH_tx), (int)(b.y + VEC2F_GRAPH_ty), color);
}

void vec2f_poly(BITMAP *buffer, VEC2F poly[], int n, int color)
{
 int *points, i;

 if(n > 2)
 {
  points = (int *)malloc((n << 1) * sizeof(int));

  for(i = 0; i < n; i++)
   {
    points[i << 1] = (int)(poly[i].x + VEC2F_GRAPH_tx);
    points[(i << 1) + 1] = (int)(poly[i].y + VEC2F_GRAPH_ty);
   }

  polygon(buffer, n, points, color);
  free(points);
 }

 else
  vec2f_line(buffer, poly[0], poly[1], color);
}

void vec2f_loop(BITMAP *buffer, VEC2F p[], int n, int flag, int color)
{
 int i;

 if(flag < 0)
  for(i = 0; i < n; i++)
   vec2f_point(buffer, p[i], -flag, color);

 else
  {
   for(i = 0; i < n - 1; i++)
    vec2f_line(buffer, p[i], p[i + 1], color);
   if(flag)
    vec2f_line(buffer, p[i], p[0], color);
  }
}

#endif
