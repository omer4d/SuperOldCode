#ifndef _RENDERING_H_
#define _RENDERING_H_

#include "vec2f.h"
#include "vec3f.h"
#include "base.h"

//#define FLOAT_TO_INT(in, out) out = (int)in;
#define FLOAT_TO_INT(in, out) __asm__ __volatile__ ("fistpl %0" : "=m" (out) : "t" (in) : "st");

#define WCLIP_COMP z
#define AFFINE_EPSILON (0.00001)
#define ZBUFFER_PRECISION (1000000000)


BITMAP *REND_INT_buffer, *REND_INT_zbuffer, *REND_INT_texture;

int *REND_INT_left_x, *REND_INT_right_x;
long int *REND_INT_left_z, *REND_INT_right_z;
VEC2F *REND_INT_left_uv, *REND_INT_right_uv;

int REND_INT_affine_flag = 0;

VEC3F REND_INT_light;

#define RI_TEXEL(x, y) ((long *)REND_INT_texture->line[BIT_WRAP(y, REND_INT_texture->h - 1)])[BIT_WRAP(x, REND_INT_texture->w - 1)]
#define RI_PIXEL(x, y) ((long *)REND_INT_buffer->line[y])[x]
#define RI_PIXEL_Z(x, y) ((long *)REND_INT_zbuffer->line[y])[x]

typedef struct {
 int sx, sy;
 VEC2F t;
 float z;
}REND_INT_POINT;

void init_renderer(int h)
{
 REND_INT_left_x = (int *)malloc(h * sizeof(int));
 REND_INT_right_x = (int *)malloc(h * sizeof(int));
 REND_INT_left_z = (long int *)malloc(h * sizeof(long int));
 REND_INT_right_z = (long int *)malloc(h * sizeof(long int));
 REND_INT_left_uv = (VEC2F *)malloc(h * sizeof(VEC2F));
 REND_INT_right_uv = (VEC2F *)malloc(h * sizeof(VEC2F));
}

void bind_color_buffer(BITMAP *buffer) { REND_INT_buffer = buffer; }
void bind_zbuffer(BITMAP *buffer) { REND_INT_zbuffer = buffer; }
void bind_texture(BITMAP *texture) { REND_INT_texture = texture; }
void set_texture_mapping_mode(int n) { REND_INT_affine_flag = n; }

void deinit_renderer()
{
 free(REND_INT_left_x);
 free(REND_INT_right_x);
 free(REND_INT_left_z);
 free(REND_INT_right_z);
 free(REND_INT_left_uv);
 free(REND_INT_right_uv);
}

void rend_int_point_hsort(REND_INT_POINT v[], int n)
{
 int i, j;
 REND_INT_POINT temp;

 for(i = n - 1; i >= 0; i--)
  for(j = 1; j <= i; j++)
   if(v[j - 1].sy > v[j].sy)
    {
     temp = v[j - 1];
     v[j - 1] = v[j];
     v[j] = temp;
    }
}

void resterize_tri_side(int x_coord[], long int z[], VEC2F tc[], REND_INT_POINT p1, REND_INT_POINT p2, int type)
{
 int i, miny = p1.sy, maxy = p2.sy;

 if(miny == p1.sy) { x_coord[p1.sy] = p1.sx; tc[p1.sy] = p1.t; z[p1.sy] = (long int)(p1.z * ZBUFFER_PRECISION); }
 if(maxy == p2.sy) { x_coord[p2.sy] = p2.sx; tc[p2.sy] = p2.t; z[p2.sy] = (long int)(p2.z * ZBUFFER_PRECISION); }

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
   float inv_z1 = 1.0 / p1.z, inv_z2 = 1.0 / p2.z, inv_z = inv_z1;
   float inv_dz = (inv_z2 - inv_z1) * d, q = 1.0 / (p2.z - p1.z), real_z;

   if(fabs(p1.z - p2.z) < AFFINE_EPSILON || REND_INT_affine_flag)
    for(i = miny; i < maxy + 1; i++)
     {
      tc[i] = VEC2F_BLEND(p1.t, p2.t, k);
      z[i] = ZBUFFER_PRECISION / inv_z;
      inv_z += inv_dz;
      k -= d;
     }

    else
     for(i = miny; i < maxy + 1; i++)
      {
       real_z = (1.0 / inv_z);
       k = (p2.z - real_z) * q;
       tc[i] = VEC2F_BLEND(p1.t, p2.t, k);
       z[i] = ZBUFFER_PRECISION * real_z;
       inv_z += inv_dz;
      }
  }
}

int additive_blend(int c1, int c2)
{
 int r1 = getr(c1), g1 = getg(c1), b1 = getb(c1);
 int r2 = getr(c2), g2 = getg(c2), b2 = getb(c2);
 int r = r1 + r2, g = g1 + g2, b = b1 + b2;
 if(r > 255) { r = 255; }
 if(g > 255) { g = 255; }
 if(b > 255) { b = 255; }
 return makecol(r, g, b);
}

void render_scanline(int x1, int y, int x2, VEC2F t1, VEC2F t2, long int zl[], long int zr[], VEC3F normal)
{
 int i;
 float real_z1 = (float)zl[y] / (float)ZBUFFER_PRECISION;
 float real_z2 = (float)zr[y] / (float)ZBUFFER_PRECISION;
 float real_z;
 float inv_z1 = 1.0 / real_z1, inv_z2 = 1.0 / real_z2, inv_z = inv_z1, q = 1.0 / (real_z2 - real_z1);
 float d = 1.0 / (float)(x2 - x1), u = t1.x, v = t1.y, buff_z, k = 1.0;
 float inv_dz = (float)(inv_z2 - inv_z1) * d;
 int iu, iv, i_buff_z;
 int minx = x1, maxx = x2 + 1;
 int last_iv = -1, last_iu = -1, last_texel = -1;

 if(fabs(inv_z1 - inv_z2) < AFFINE_EPSILON || REND_INT_affine_flag)
  for(i = minx; i < maxx; i++)
   {
    real_z = 1.0 / inv_z;
    buff_z = ZBUFFER_PRECISION * real_z;

    if(buff_z < RI_PIXEL_Z(i, y))
     {
      u = (t1.x * k + t2.x * (1.0 - k)) * REND_INT_texture->w;
      v = (t1.y * k + t2.y * (1.0 - k)) * REND_INT_texture->h;
      FLOAT_TO_INT(v, iv);
      FLOAT_TO_INT(u, iu);
      FLOAT_TO_INT(buff_z, i_buff_z);
      int final = additive_blend(RI_PIXEL(i, y), RI_TEXEL(iu, iv));
      RI_PIXEL(i, y) = final;

      //if(iv != last_iv || iu != last_iu)
       //last_texel = RI_TEXEL(iu, iv);

      //RI_PIXEL(i, y) = last_texel;
      RI_PIXEL_Z(i, y) = i_buff_z;
      last_iu = iu;
      last_iv = iv;
     }

    inv_z += inv_dz;
    k -= d;
   }

 else
  for(i = minx; i < maxx; i++)
   {
    real_z = 1.0 / inv_z;
    buff_z = ZBUFFER_PRECISION * real_z;

    if(buff_z < RI_PIXEL_Z(i, y))
     {
      k = (real_z2 - real_z) * q;
      u = (t1.x * k + t2.x * (1.0 - k)) * (REND_INT_texture->w - 1);
      v = (t1.y * k + t2.y * (1.0 - k)) * (REND_INT_texture->h - 1);
      FLOAT_TO_INT(v, iv);
      FLOAT_TO_INT(u, iu);
      FLOAT_TO_INT(buff_z, i_buff_z);
      int final = additive_blend(RI_PIXEL(i, y), RI_TEXEL(iu, iv));
      RI_PIXEL(i, y) = final;

      //if(iv != last_iv || iu != last_iu)
       //last_texel = RI_TEXEL(iu, iv);

      //RI_PIXEL(i, y) = last_texel;
      RI_PIXEL_Z(i, y) = i_buff_z;
      last_iu = iu;
      last_iv = iv;
     }

    inv_z += inv_dz;
   }
}

void my_triangle(REND_INT_POINT tri[3], VEC3F normal)
{
 int i, l, r;

 if(tri[0].sy == tri[1].sy && tri[1].sy == tri[2].sy)
  return;

 rend_int_point_hsort(tri, 3);

 int temp_x = (tri[1].sy * (tri[0].sx - tri[2].sx) + tri[0].sy * tri[2].sx - tri[0].sx * tri[2].sy) / (tri[0].sy - tri[2].sy);

 if(tri[1].sx < temp_x) { l = 1; r = 2; }
 else { l = 2; r = 1; }

 resterize_tri_side(REND_INT_left_x, REND_INT_left_z, REND_INT_left_uv, tri[0], tri[l], 1);
 resterize_tri_side(REND_INT_right_x, REND_INT_right_z, REND_INT_right_uv, tri[0], tri[r], -1);

 if(tri[1].sx >= temp_x) { resterize_tri_side(REND_INT_right_x, REND_INT_right_z, REND_INT_right_uv, tri[1], tri[2], -1); }
 else { resterize_tri_side(REND_INT_left_x, REND_INT_left_z, REND_INT_left_uv, tri[1], tri[2], 1); }

 for(i = tri[0].sy; i < tri[2].sy; i++)
  render_scanline(REND_INT_left_x[i], i, REND_INT_right_x[i], REND_INT_left_uv[i], REND_INT_right_uv[i],
                  REND_INT_left_z, REND_INT_right_z, normal);
}

int clip_z(VERTEX in_vert[], VEC2F in_tex[], VERTEX out_vert[], VEC2F out_tex[], int n, float p, int sign)
{
 int i = 0, j, flag;
 float k;

 if(n == 0)
  return 0;

 inline void clip_seg(VERTEX *v0, VERTEX *v1, VEC2F *t0, VEC2F *t1)
  {
   flag = 0;
   if(v0->world.WCLIP_COMP * sign > p * sign) { out_vert[i] = *v0; out_tex[i] = *t0; flag++; i++; }
   if(v1->world.WCLIP_COMP * sign > p * sign) { flag++; }
   if(flag == 1)
    {
     k = (p - v0->world.WCLIP_COMP) / (v1->world.WCLIP_COMP - v0->world.WCLIP_COMP);
     out_vert[i].world.x = v0->world.x + k * (v1->world.x - v0->world.x);
     out_vert[i].world.y = v0->world.y + k * (v1->world.y - v0->world.y);
     out_vert[i].world.z = v0->world.z + k * (v1->world.z - v0->world.z);
     out_tex[i] = VEC2F_BLEND((*t1), (*t0), k);
     i++;
    }
  }

 for(j = 0; j < n - 1; j++)
  clip_seg(&in_vert[j], &in_vert[j + 1], &in_tex[j], &in_tex[j + 1]);
 clip_seg(&in_vert[n - 1], &in_vert[0], &in_tex[n - 1], &in_tex[0]);

 return i;
}

int clip_x(REND_INT_POINT in[], REND_INT_POINT out[], int n, float p, int sign)
{
 int i = 0, j, flag;
 float k;

 if(n == 0)
  return 0;

 inline void clip_seg(REND_INT_POINT *v0, REND_INT_POINT *v1)
  {
   float dz = v1->z - v0->z;

   flag = 0;
   if(v0->sx * sign > p * sign) { out[i] = *v0; flag++; i++; }
   if(v1->sx * sign > p * sign) { flag++; }
   if(flag == 1)
    {
     k = (p - v0->sx) / (v1->sx - v0->sx);
     out[i].sx = v0->sx + k * (v1->sx - v0->sx);
     out[i].sy = v0->sy + k * (v1->sy - v0->sy);
     out[i].z = -v1->z * v0->z / (k * (v1->z - v0->z) - v1->z);
     if(fabs(dz) > AFFINE_EPSILON) k = (out[i].z - v0->z) / dz;
     out[i].t = VEC2F_BLEND(v1->t, v0->t, k);
     i++;
    }
  }

 for(j = 0; j < n - 1; j++)
  clip_seg(&in[j], &in[j + 1]);
 clip_seg(&in[n - 1], &in[0]);

 return i;
}

int clip_y(REND_INT_POINT in[], REND_INT_POINT out[], int n, float p, int sign)
{
 int i = 0, j, flag;
 float k;

 if(n == 0)
  return 0;

 inline void clip_seg(REND_INT_POINT *v0, REND_INT_POINT *v1)
  {
   float dz = v1->z - v0->z;

   flag = 0;
   if(v0->sy * sign > p * sign) { out[i] = *v0; flag++; i++; }
   if(v1->sy * sign > p * sign) { flag++; }
   if(flag == 1)
    {
     k = (p - v0->sy) / (v1->sy - v0->sy);
     out[i].sx = v0->sx + k * (v1->sx - v0->sx);
     out[i].sy = v0->sy + k * (v1->sy - v0->sy);
     out[i].z = -v1->z * v0->z / (k * (v1->z - v0->z) - v1->z);
     if(fabs(dz) > AFFINE_EPSILON) k = (out[i].z - v0->z) / dz;
     out[i].t = VEC2F_BLEND(v1->t, v0->t, k);
     i++;
    }
  }

 for(j = 0; j < n - 1; j++)
  clip_seg(&in[j], &in[j + 1]);
 clip_seg(&in[n - 1], &in[0]);

 return i;
}

void render_convex_poly(VERTEX v[], VEC2F t[], int n_orig, VEC3F normal)
{
 int n, i;
 float minx = 10, miny = 10, maxx = SCREEN_W - 10, maxy = SCREEN_H - 10;
 REND_INT_POINT arr1[10], arr2[10], tri[3];

 for(i = 0; i < n_orig; i++)
  { arr1[i].sx = v[i].sx; arr1[i].sy = v[i].sy; arr1[i].z = v[i].screen.z; arr1[i].t = t[i]; }

 n = clip_x(arr1, arr2, n_orig, minx, 1);
 n = clip_x(arr2, arr1, n, maxx, -1);
 n = clip_y(arr1, arr2, n, miny, 1);
 n = clip_y(arr2, arr1, n, maxy, -1);

 if(n > 0)
  for(i = 1; i < n - 1; i++)
   {
    tri[0] = arr1[0];
    tri[1] = arr1[i];
    tri[2] = arr1[i + 1];
    my_triangle(tri, normal);
   }
}

void render_tri(TRI *tri, VERTEX v[])
{
 int i, n = 3;

 VERTEX vert1[10], vert2[10];
 VEC2F tex1[10], tex2[10];

 vert1[0] = v[tri->a]; vert1[1] = v[tri->b]; vert1[2] = v[tri->c];
 tex1[0] = tri->tex[0]; tex1[1] = tri->tex[1]; tex1[2] = tri->tex[2];

 n = clip_z(vert1, tex1, vert2, tex2, 3, -0.95, 1);
 n = clip_z(vert2, tex2, vert1, tex1, n, 200.0, -1);

 if(n > 0)
 {
  for(i = 0; i < n; i++)
   project_vertex(&vert1[i]);
  render_convex_poly(vert1, tex1, n, tri->n);
 }
}

#endif
