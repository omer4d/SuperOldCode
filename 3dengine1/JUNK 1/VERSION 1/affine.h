#include "vec2f.h"
#include "vec3f.h"

#define LIGHTMAP_SIZE (32)

BITMAP *LM_buffer;

#define LM_PIXEL(x, y) ((long *)LM_buffer->line[y])[x]

typedef struct {
 int sx, sy;
 VEC3F w;
}LM_POINT;

int ray_triangle_x(VEC3F p, VEC3F d, VEC3F v0, VEC3F v1, VEC3F v2)
{
 VEC3F e1, e2, h, s, q;
 float a, f, t, u, v;

 e1 = VEC3F_DIFF(v1, v0);
 e2 = VEC3F_DIFF(v2, v0);
 h = vec3f_cross(d, e2);
 a = VEC3F_DOT_PRODUCT(e1, h);

 if(a > -0.00001 && a < 0.00001)
  return 0;

 f = 1.0 / a;
 s = VEC3F_DIFF(p, v0);
 u = f * VEC3F_DOT_PRODUCT(s, h);

 if (u < 0.0 || u > 1.0)
  return 0;

 q = vec3f_cross(s, e1);
 v = f * VEC3F_DOT_PRODUCT(d, q);

 if(v < 0.0 || u + v > 1.0)
  return 0;

 t = f * VEC3F_DOT_PRODUCT(e2, q);
 if(t > 0.00001)
  {
   return 1;
   //printf("!!!");
  }

 return 0;
}

void lm_point_hsort(LM_POINT v[], int n)
{
 int i, j;
 LM_POINT temp;

 for(i = n - 1; i >= 0; i--)
  for(j = 1; j <= i; j++)
   if(v[j - 1].sy > v[j].sy)
    {
     temp = v[j - 1];
     v[j - 1] = v[j];
     v[j] = temp;
    }
}

void lm_resterize_tri_side(int x_coord[], VEC3F w[], LM_POINT p1, LM_POINT p2, int type)
{
 int i, miny = p1.sy, maxy = p2.sy;

 if(miny == p1.sy) { x_coord[p1.sy] = p1.sx; w[p1.sy] = p1.w; }
 if(maxy == p2.sy) { x_coord[p2.sy] = p2.sx; w[p2.sy] = p2.w; }

 if(miny != maxy)
  {
   if(p1.sx - p2.sx != 0)
    {
     float d = (float)(p2.sx - p1.sx) / (float)(p2.sy - p1.sy), x = p1.sx;
     for(i = miny; i < maxy; i++)
      {
       x_coord[i] = (int)(x + 0.5);
       x += d;
      }
     }

   else for(i = miny; i < maxy; i++) { x_coord[i] = p1.sx; }

   float d = 1.0 / (float)(p2.sy - p1.sy), k = 1.0;

   for(i = miny; i < maxy + 1; i++)
    {
     w[i] = VEC3F_BLEND(p1.w, p2.w, k);
     k -= d;
    }
  }
}

void lm_render_scanline(int x1, int y, int x2, VEC3F w1, VEC3F w2, VEC3F normal, TRI tri[], VERTEX vert[], int curr, int max)
{
 int i, j;
 float d = 1.0 / (float)(x2 - x1), k = 1.0;
 int minx = x1, maxx = x2 + 1;
 VEC3F w;
 VEC3F lw = vec3f(50.0, 50.0, 50.0), l;

 for(i = minx; i < maxx; i++)
  {
   w.x = (w1.x * k + w2.x * (1.0 - k));
   w.y = (w1.y * k + w2.y * (1.0 - k));
   w.z = (w1.z * k + w2.z * (1.0 - k));
   l = NORMALIZED_VEC3F(VEC3F_DIFF(lw, w));
   float br = VEC3F_DOT_PRODUCT(l, normal);
   if(br > 1.0) { br = 1.0; }
   if(br < 0.0) { br = 0.0; }
   
   int terminate = 0;

   for(j = 0; j < max && !terminate; j++)
    if(j != curr)
     if(ray_triangle_x(w, l, vert[tri[j].a].local, vert[tri[j].b].local, vert[tri[j].c].local))
      {
       terminate = 1;
       br = 0.0;
      }

   LM_PIXEL(i, y) = makecol(64 + 191 * br, 64 + 191 * br, 64 + 191 * br);//RI_TEXEL(iu, iv);
   k -= d;
  }
}

void lm_triangle(LM_POINT tri[3], VEC3F normal, TRI soup[], VERTEX vert[], int curr, int max)
{
 int i, l, r;
 int LM_left_x[LIGHTMAP_SIZE], LM_right_x[LIGHTMAP_SIZE];
 VEC3F LM_left_w[LIGHTMAP_SIZE], LM_right_w[LIGHTMAP_SIZE];

 if(tri[0].sy == tri[1].sy && tri[1].sy == tri[2].sy)
  return;

 lm_point_hsort(tri, 3);

 int temp_x = (tri[1].sy * (tri[0].sx - tri[2].sx) + tri[0].sy * tri[2].sx - tri[0].sx * tri[2].sy) / (tri[0].sy - tri[2].sy);

 if(tri[1].sx < temp_x) { l = 1; r = 2; }
 else { l = 2; r = 1; }

 lm_resterize_tri_side(LM_left_x, LM_left_w, tri[0], tri[l], 1);
 lm_resterize_tri_side(LM_right_x, LM_right_w, tri[0], tri[r], -1);

 if(tri[1].sx >= temp_x) { lm_resterize_tri_side(LM_right_x, LM_right_w, tri[1], tri[2], -1); }
 else { lm_resterize_tri_side(LM_left_x, LM_left_w, tri[1], tri[2], 1); }

 for(i = tri[0].sy; i < tri[2].sy + 1; i++)
  lm_render_scanline(LM_left_x[i], i, LM_right_x[i], LM_left_w[i], LM_right_w[i], normal, soup, vert, curr, max);
}
