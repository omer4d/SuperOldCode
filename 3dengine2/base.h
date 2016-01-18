#ifndef _BASE_H_
#define _BASE_H_

#include <allegro.h>
#include "common.h"
#include "vec2f.h"
#include "vec3f.h"
#include "mat16f.h"

#define MAX_AFFINE_INTERP 4

#define AFFINE_EPSILON (0.000000001)

typedef struct {
 VEC3F object, trans, screen;
}VERTEX;

typedef struct {
 int vnum, col;
 long *vind;
 VEC2F *texcoord;
 VEC3F normal;
}POLY3D;

typedef struct {
 VEC3F trans, screen;
 float affine_interp[MAX_AFFINE_INTERP - 2];
}WCLIP_POINT;

typedef struct {
 float y;
 float affine_interp[MAX_AFFINE_INTERP];
}REND_INT_POINT;

#define POLY3D_MAX_VNUM 20
#define FOCAL_DISTANCE(h, fov) (0.5 * h / tan(RAD(fov) * 0.5))

//#define RI_TEXEL(x, y) ((long *)REND_INT_texture->line[BIT_WRAP(y, REND_INT_texture->h - 1)])[BIT_WRAP(x, REND_INT_texture->w - 1)]
#define RI_PIXEL(x, y) ((long *)BASE_INT_color_buffer->line[y])[x]
#define RI_PIXEL_Z(x, y) ((long *)BASE_INT_z_buffer->line[y])[x]

float BASE_INT_focal_dist;
float BASE_INT_cs_mx, BASE_INT_cs_my, BASE_INT_cs_mz;
float BASE_INT_cs_kx, BASE_INT_cs_ky, BASE_INT_cs_kz;
float BASE_INT_znear, BASE_INT_zfar;
float BASE_INT_minx, BASE_INT_miny, BASE_INT_maxx, BASE_INT_maxy;
float BASE_INT_l, BASE_INT_r, BASE_INT_b, BASE_INT_t;

long BASE_INT_z_buffer_precision = 2147483647; // 2^31-1 = 2147483647

BITMAP *BASE_INT_main_color_buffer, *BASE_INT_main_z_buffer;
BITMAP *BASE_INT_color_buffer, *BASE_INT_z_buffer;

void setup_projection(float fov, float l1, float r1, float b1, float t1, float n1, float f1)
{
 float l2 = 0.0, r2 = (float)BASE_INT_color_buffer->w,
       b2 = (float)BASE_INT_color_buffer->h, t2 = 0.0,
       n2 = 0.0, f2 = 1.0;

 if(n1 < 0.01) { n1 = 0.01; }
 if(f1 < n1 + 1.0) { f1 = n1 + 1.0; }

 BASE_INT_l = l1;
 BASE_INT_r = r1;
 BASE_INT_b = b1;
 BASE_INT_t = t1;

 BASE_INT_focal_dist = FOCAL_DISTANCE(fabs(t1 - b1), fov);
 BASE_INT_znear = n1;
 BASE_INT_zfar = f1;

 BASE_INT_cs_mx = (r2 - l2) / (r1 - l1);
 BASE_INT_cs_my = (t2 - b2) / (t1 - b1);
 BASE_INT_cs_mz = (f2 - n2) / (f1 - n1);
 BASE_INT_cs_kx = l2 - l1 * BASE_INT_cs_mx;
 BASE_INT_cs_ky = b2 - b1 * BASE_INT_cs_my;
 BASE_INT_cs_kz = n2 - n1 * BASE_INT_cs_mz;
}

void build_view_volume(VEC3F volume[5])
{
 float k = BASE_INT_zfar / BASE_INT_focal_dist;
 volume[0] = ZERO_VEC3F;
 volume[1] = vec3f(BASE_INT_l * k, BASE_INT_t * k, BASE_INT_zfar);
 volume[2] = vec3f(BASE_INT_r * k, BASE_INT_t * k, BASE_INT_zfar);
 volume[3] = vec3f(BASE_INT_r * k, BASE_INT_b * k, BASE_INT_zfar);
 volume[4] = vec3f(BASE_INT_l * k, BASE_INT_b * k, BASE_INT_zfar);
}

void set_clipping_rect(int minx, int miny, int maxx, int maxy)
{
 if(minx < 0) { minx = 0; }
 if(miny < 0) { miny = 0; }
 if(maxx > BASE_INT_color_buffer->w - 1) { maxx = BASE_INT_color_buffer->w - 1; }
 if(maxy > BASE_INT_color_buffer->h - 1) { maxy = BASE_INT_color_buffer->h - 1; }

 BASE_INT_minx = minx;
 BASE_INT_maxx = maxx;
 BASE_INT_miny = miny;
 BASE_INT_maxy = maxy;
}

void set_znear(float n)
{
 if(n < 0.01) { n = 0.01; }
 BASE_INT_znear = n;
}

void set_zfar(float f)
{
 float n = BASE_INT_znear;
 if(f < n + 1.0) { f = n + 1.0; }
 BASE_INT_zfar = f;
}

void set_fov(float fov)
{
 BASE_INT_focal_dist = FOCAL_DISTANCE(fabs(BASE_INT_t - BASE_INT_b), fov);
}

void init_engine(int w, int h)
{
 BASE_INT_main_color_buffer = create_bitmap(w, h);
 BASE_INT_main_z_buffer = create_bitmap_ex(32, w, h);

 BASE_INT_color_buffer = BASE_INT_main_color_buffer;
 BASE_INT_z_buffer = BASE_INT_main_z_buffer;
 BASE_INT_minx = 0;
 BASE_INT_miny = 0;
 BASE_INT_maxx = BASE_INT_color_buffer->w - 1;
 BASE_INT_maxy = BASE_INT_color_buffer->h - 1;

 setup_projection(90.0, -1.333, 1.333, -1.0, 1.0, 0.0, 1000.0);
}

void deinit_engine()
{
 destroy_bitmap(BASE_INT_main_color_buffer);
 destroy_bitmap(BASE_INT_main_z_buffer);
}

void bind_color_buffer(BITMAP *buffer)
{
 if(buffer != NULL)
  BASE_INT_color_buffer = buffer;
 else
  BASE_INT_color_buffer = BASE_INT_main_color_buffer;
}

void bind_z_buffer(BITMAP *buffer)
{
 if(buffer != NULL)
  BASE_INT_z_buffer = buffer;
 else
  BASE_INT_z_buffer = BASE_INT_main_z_buffer;
}

void point_at_color_buffer(BITMAP **buffer)
{
 *buffer = BASE_INT_color_buffer;
}

void point_at_z_buffer(BITMAP **buffer)
{
 *buffer = BASE_INT_z_buffer;
}

void project_vertex(VERTEX *v)
{
 float k = BASE_INT_focal_dist / v->trans.z;

 v->screen.x = v->trans.x * k * BASE_INT_cs_mx + BASE_INT_cs_kx;
 v->screen.y = v->trans.y * k * BASE_INT_cs_my + BASE_INT_cs_ky;
 v->screen.z = v->trans.z * BASE_INT_cs_mz + BASE_INT_cs_kz;
}

void project_wclip_point(WCLIP_POINT *v)
{
 float k = BASE_INT_focal_dist / v->trans.z;

 v->screen.x = v->trans.x * k * BASE_INT_cs_mx + BASE_INT_cs_kx;
 v->screen.y = v->trans.y * k * BASE_INT_cs_my + BASE_INT_cs_ky;
 v->screen.z = v->trans.z * BASE_INT_cs_mz + BASE_INT_cs_kz;
}

void create_poly3d(POLY3D *poly, int col, int vnum, ...)
{
 int i;

 poly->vnum = vnum;
 poly->vind = (long *)malloc(vnum * sizeof(long));
 poly->texcoord = (VEC2F *)malloc(vnum * sizeof(VEC2F));
 poly->col = col;

 va_list list_pointer;
 va_start(list_pointer, vnum);

 for(i = 0; i < vnum; i++)
  poly->vind[i] = va_arg(list_pointer, long);

 va_end(list_pointer);
}

void update_poly3d_normal(POLY3D *poly, VERTEX v[])
{
 poly->normal = vec3f_normal(v[poly->vind[0]].trans, v[poly->vind[1]].trans, v[poly->vind[2]].trans);
}

int cull_backface(POLY3D *poly, VERTEX vert[])
{
 VEC3F v = VEC3F_DIFF(ZERO_VEC3F, vert[poly->vind[0]].trans);

 if(VEC3F_DOT_PRODUCT(v, poly->normal) > 0.0)
  return 0;
 return 1;
}

void destroy_poly3d(POLY3D *poly)
{
 free(poly->vind);
 free(poly->texcoord);
}

#endif
