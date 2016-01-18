#ifndef _BASE_H_
#define _BASE_H_

#include "vec2f.h"
#include "vec3f.h"
#include "mat16f.h"

#define RAD(d) (d * M_PI / 180.0)
#define FOCAL_DISTANCE(h, fov) (0.5 * h / tan(RAD(fov) * 0.5))
#define BIT_WRAP(x, y) (abs((int)x & y))

int ROUND(float f)
{
 if(f < 0.0)
  return (int)(f - 0.5);
 return (int)(f + 0.5);
}

float BASE_INT_focal_dist;
float BASE_INT_cs_mx, BASE_INT_cs_my, BASE_INT_cs_mz;
float BASE_INT_cs_kx, BASE_INT_cs_ky, BASE_INT_cs_kz;

typedef struct {
 VEC3F local, world, screen;
 int sx, sy;
}VERTEX;

typedef struct {
 int vis;
 long a, b, c;
 VEC2F tex[3];
 VEC3F n;
}TRI;

typedef struct {
 int vnum, *vind;
 VEC3F normal;
}POLY3D;

#define TRI_AW(tri, vert) vert[tri->a].world
#define TRI_BW(tri, vert) vert[tri->b].world
#define TRI_CW(tri, vert) vert[tri->c].world

int frange(float x, float min, float max)
{
 if(x < min) return 0;
 if(x > max) return 0;
 return 1;
}

int on_bitmap(int x, int y, int w, int h)
{
 if(x < 0 || x > (w - 1) || y < 0 || y > (h - 1))
  return 0;
 return 1; 
}

int wrap(int x, int min, int max)
{
 int w;

 x -= min;
 max -= min;
 w = x % max;
 if(w < 0) w += max;
 w += min;

 return w;
}

int clamp(int x, int min, int max)
{
 if(x < min) return min;
 if(x > max) return max;
 return x;
}

void setup_projection(float fov, float l1, float r1, float b1, float t1, float n1, float f1)
{
 float l2 = 0.0, r2 = (float)SCREEN_W, b2 = (float)SCREEN_H, t2 = 0.0, n2 = 0.0, f2 = 1.0;

 BASE_INT_cs_mx = (r2 - l2) / (r1 - l1);
 BASE_INT_cs_my = (t2 - b2) / (t1 - b1);
 BASE_INT_cs_mz = (f2 - n2) / (f1 - n1);
 BASE_INT_cs_kx = l2 - l1 * BASE_INT_cs_mx;
 BASE_INT_cs_ky = b2 - b1 * BASE_INT_cs_my;
 BASE_INT_cs_kz = n2 - n1 * BASE_INT_cs_mz;
 BASE_INT_focal_dist = FOCAL_DISTANCE(fabs(t1 - b1), fov);
}

void project_vertex(VERTEX *v)
{
 float k = BASE_INT_focal_dist / (v->world.z + BASE_INT_focal_dist);

 v->screen.x = v->world.x * k * BASE_INT_cs_mx + BASE_INT_cs_kx;
 v->screen.y = v->world.y * k * BASE_INT_cs_my + BASE_INT_cs_ky;
 v->screen.z = v->world.z * BASE_INT_cs_mz + BASE_INT_cs_kz;
 v->sx = ROUND(v->screen.x);
 v->sy = ROUND(v->screen.y);
}

int cull_backface(TRI *tri, VERTEX vert[])
{
 VEC3F v = VEC3F_DIFF(vec3f(0.0, 0.0, -BASE_INT_focal_dist), vert[tri->a].world);

 if(VEC3F_DOT_PRODUCT(v, tri->n) > 0.0)
  {
   tri->vis = 0;
   return 0;
  }

 return 1;
}

int quick_frustum_cull(TRI *tri, VERTEX vert[], float znear, float zfar)
{
 if(!frange(TRI_AW(tri, vert).z, znear, zfar) ||
    !frange(TRI_BW(tri, vert).z, znear, zfar) ||
    !frange(TRI_CW(tri, vert).z, znear, zfar))
  {
   tri->vis = 0;
   return 0;
  }

 tri->vis = 1;
 return 1;
}

void update_tri_normal(TRI *tri, VERTEX vert[])
{
 tri->n = vec3f_normal(vert[tri->a].world, vert[tri->b].world, vert[tri->c].world);
 //tri->n = NORMALIZED_NORMAL_VEC3F(vert[tri->a].local, vert[tri->b].local, vert[tri->c].local);
}

void local_update_tri_normal(TRI *tri, VERTEX vert[])
{
 //tri->n = vec3f_normal(vert[tri->a].world, vert[tri->b].world, vert[tri->c].world);
 tri->n = NORMALIZED_NORMAL_VEC3F(vert[tri->a].local, vert[tri->b].local, vert[tri->c].local);
}

#endif
