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

 for(i = 2; i < MAX_AFFINE_INTERP; i++)
  {
   a.affine_interp[i] = v0->affine_interp[i - 2];
   b.affine_interp[i] = v1->affine_interp[i - 2];
  }

 scr_clip_func[0](0, &a, &b);

 return 0;
}

void render_poly3d(POLY3D *poly, VERTEX v[])
{
 int i;
 WCLIP_POINT a, b;
 //VEC3F nn = NORMALIZED_VEC3F(poly->normal);

 //set_flat_color(makecol((nn.x + 1.0) * 127.5, (nn.y + 1.0) * 127.5, (nn.z + 1.0) * 127.5));
 set_flat_color(poly->col);

 for(i = 0; i < MAX_SCR_CLIP_PLANES; i++)
  scr_clip_param[i].count = 0;
 for(i = 0; i < MAX_WORLD_CLIP_PLANES; i++)
  world_clip_param[i].count = 0;

 RITT_seg_count = 0;
 float invz;

 for(i = 0; i < poly->vnum - 1; i++)
  {
   a.trans = v[poly->vind[i]].trans;
   a.screen = v[poly->vind[i]].screen;

   invz = 1.0 / v[poly->vind[i]].screen.z;
   a.affine_interp[0] = poly->texcoord[i].x * invz;
   a.affine_interp[1] = poly->texcoord[i].y * invz;

   b.trans = v[poly->vind[i + 1]].trans;
   b.screen = v[poly->vind[i + 1]].screen;

   invz = 1.0 / v[poly->vind[i + 1]].screen.z;
   b.affine_interp[0] = poly->texcoord[i + 1].x * invz;
   b.affine_interp[1] = poly->texcoord[i + 1].y * invz;

   world_clip_func[0](0, &a, &b);
  }

 a.trans = v[poly->vind[i]].trans;
 a.screen = v[poly->vind[i]].screen;

 invz = 1.0 / v[poly->vind[i]].screen.z;
 a.affine_interp[0] = poly->texcoord[i].x * invz;
 a.affine_interp[1] = poly->texcoord[i].y * invz;

 b.trans = v[poly->vind[0]].trans;
 b.screen = v[poly->vind[0]].screen;

 invz = 1.0 / v[poly->vind[0]].screen.z;
 b.affine_interp[0] = poly->texcoord[0].x * invz;
 b.affine_interp[1] = poly->texcoord[0].y * invz;

 world_clip_func[0](0, &a, &b);
}

#endif
