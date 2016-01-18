#ifndef _RENDERING_H_
#define _RENDERING_H_

#include "vec2f.h"
#include "vec3f.h"
#include "base.h"
#include "triangle.h"
#include "clipping.h"

int RITT_seg_count;
REND_INT_POINT RITT_first_point;

int rend_int_to_tri(int curr, REND_INT_POINT *v0, REND_INT_POINT *v1)
{
 REND_INT_POINT tri[3];

 if(RITT_seg_count == 0) { RITT_first_point = *v0; }
 else
  {
   if(fabs(v1->affine_interp[0] - RITT_first_point.affine_interp[0]) + fabs(v1->y - RITT_first_point.y) < FLOAT_EPSILON)
    return;
   tri[0] = RITT_first_point;
   tri[1] = *v0;
   tri[2] = *v1;
   rasterize_triangle(tri);
  }

 RITT_seg_count++;
}

int wclip_to_rend_int(int curr, WCLIP_POINT *v0, WCLIP_POINT *v1)
{
 REND_INT_POINT a, b;
 int i;

 a.affine_interp[0] = v0->screen.x;
 a.affine_interp[1] = 1.0 / v0->screen.z;
 a.y = v0->screen.y;

 b.affine_interp[0] = v1->screen.x;
 b.affine_interp[1] = 1.0 / v1->screen.z;
 b.y = v1->screen.y;

 for(i = 0; i < MAX_PERSPC_INTERP; i++)
  {
   a.perspc_interp[i] = v0->perspc_interp[i];
   b.perspc_interp[i] = v1->perspc_interp[i];
  }

 for(i = 2; i < MAX_AFFINE_INTERP; i++)
  {
   a.affine_interp[i] = v0->affine_interp[i];
   b.affine_interp[i] = v1->affine_interp[i];
  }

 scr_clip_func[0](0, &a, &b);

 return 0;
}

VEC3F global_n;
VEC3F A, B, C, V0, T0;
float DIST;

void get_tc(VEC3F n, VEC3F v0, VEC3F t0, float D, float f, float mx, float my, float kx, float ky, float sx, float sy, float *k, float *txk, float *tyk)
{
 VEC3F v = vec3f(my * (sx - kx), mx * (sy - ky), my * mx * f);
 *k = VEC3F_DOT_PRODUCT(v, n);
 *txk = -(VEC3F_DOT_PRODUCT(v0, A) - t0.x) * (*k) - VEC3F_DOT_PRODUCT(v, A) * D;
 *tyk = -(VEC3F_DOT_PRODUCT(v0, B) - t0.y) * (*k) - VEC3F_DOT_PRODUCT(v, B) * D;
}

void render_poly3d(POLY3D *poly, VERTEX v[])
{
 int i;
 WCLIP_POINT a, b;
 VEC3F nn = NORMALIZED_VEC3F(poly->normal);

 set_flat_color(makecol((nn.x + 1.0) * 127.5, (nn.y + 1.0) * 127.5, (nn.z + 1.0) * 127.5));

 for(i = 0; i < MAX_SCR_CLIP_PLANES; i++)
  scr_clip_param[i].count = 0;
 for(i = 0; i < MAX_WORLD_CLIP_PLANES; i++)
  world_clip_param[i].count = 0;

 RITT_seg_count = 0;

 for(i = 0; i < poly->vnum - 1; i++)
  {
   a.trans = v[poly->vind[i]].trans;
   a.screen = v[poly->vind[i]].screen;
   a.perspc_interp[0] = poly->texcoord[i].x;
   a.perspc_interp[1] = poly->texcoord[i].y;
   get_tc(global_n, V0, T0, DIST, BASE_INT_focal_dist,
          BASE_INT_cs_mx, BASE_INT_cs_my, BASE_INT_cs_kx, BASE_INT_cs_ky, a.screen.x, a.screen.y,
          &a.affine_interp[2], &a.affine_interp[3], &a.affine_interp[4]);

   b.trans = v[poly->vind[i + 1]].trans;
   b.screen = v[poly->vind[i + 1]].screen;
   b.perspc_interp[0] = poly->texcoord[i + 1].x;
   b.perspc_interp[1] = poly->texcoord[i + 1].y;
   get_tc(global_n, V0, T0, DIST, BASE_INT_focal_dist,
          BASE_INT_cs_mx, BASE_INT_cs_my, BASE_INT_cs_kx, BASE_INT_cs_ky, b.screen.x, b.screen.y,
          &b.affine_interp[2], &b.affine_interp[3], &b.affine_interp[4]);

   world_clip_func[0](0, &a, &b);
  }

 a.trans = v[poly->vind[i]].trans;
 a.screen = v[poly->vind[i]].screen;
 a.perspc_interp[0] = poly->texcoord[i].x;
 a.perspc_interp[1] = poly->texcoord[i].y;
 get_tc(global_n, V0, T0, DIST, BASE_INT_focal_dist,
        BASE_INT_cs_mx, BASE_INT_cs_my, BASE_INT_cs_kx, BASE_INT_cs_ky, a.screen.x, a.screen.y,
        &a.affine_interp[2], &a.affine_interp[3], &a.affine_interp[4]);

 b.trans = v[poly->vind[0]].trans;
 b.screen = v[poly->vind[0]].screen;
 b.perspc_interp[0] = poly->texcoord[0].x;
 b.perspc_interp[1] = poly->texcoord[0].y;
 get_tc(global_n, V0, T0, DIST, BASE_INT_focal_dist,
        BASE_INT_cs_mx, BASE_INT_cs_my, BASE_INT_cs_kx, BASE_INT_cs_ky, b.screen.x, b.screen.y,
        &b.affine_interp[2], &b.affine_interp[3], &b.affine_interp[4]);

 world_clip_func[0](0, &a, &b);
}

#endif
