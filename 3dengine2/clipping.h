#ifndef _CLIPPING_H_
#define _CLIPPING_H_

#define MAX_SCR_CLIP_PLANES 4
#define MAX_WORLD_CLIP_PLANES 2

#include "base.h"

typedef struct {
 float p;
 int sign, comp, func_index, count;
 REND_INT_POINT last;
}SCR_CLIP_PARAM;

typedef struct {
 float p;
 int sign, comp, func_index, count;
 WCLIP_POINT last;
}WORLD_CLIP_PARAM;

int (*scr_clip_func[MAX_SCR_CLIP_PLANES + 1])(int, REND_INT_POINT *, REND_INT_POINT *);
SCR_CLIP_PARAM scr_clip_param[MAX_SCR_CLIP_PLANES + 1];

int (*world_clip_func[MAX_WORLD_CLIP_PLANES + 1])(int, WCLIP_POINT *, WCLIP_POINT *);
WORLD_CLIP_PARAM world_clip_param[MAX_WORLD_CLIP_PLANES + 1];

float scr_vcomp(REND_INT_POINT *v, int f)
{
 if(f == 0)
  return v->affine_interp[0];
 else
  return v->y;
}

void set_scr_clip_func(int n, int func(int, REND_INT_POINT *, REND_INT_POINT *), int comp, float p, int sign, int func_index)
{
 scr_clip_func[n] = func;
 scr_clip_param[n].sign = sign;
 scr_clip_param[n].comp = comp;
 scr_clip_param[n].p = p;
 scr_clip_param[n].func_index = func_index;
}

int scr_clip(int curr, REND_INT_POINT *v0, REND_INT_POINT *v1)
{
 REND_INT_POINT out[2];
 int flag = 0, i = 0, j = 0;
 float k;

 float p = scr_clip_param[curr].p;
 int sign = scr_clip_param[curr].sign, func_index = scr_clip_param[curr].func_index, comp = scr_clip_param[curr].comp;

 if(scr_vcomp(v0, comp) * sign > p * sign) { out[i] = *v0; flag++; i++; }
 if(scr_vcomp(v1, comp) * sign > p * sign) { flag++; }
 if(flag == 1)
  {
   k = (p - scr_vcomp(v0, comp)) / (scr_vcomp(v1, comp) - scr_vcomp(v0, comp));
   out[i].y = v0->y + k * (v1->y - v0->y);

   for(j = 0; j < MAX_AFFINE_INTERP; j++)
    out[i].affine_interp[j] = v0->affine_interp[j] + k * (v1->affine_interp[j] - v0->affine_interp[j]);

   if(scr_clip_param[curr].count == 0)
    {
     scr_clip_param[curr].last = out[i];
     scr_clip_param[curr].count++;
    }

   else
    {
     scr_clip_func[func_index](scr_clip_param[curr].func_index, &out[i], &scr_clip_param[curr].last);
     scr_clip_param[curr].count = 0;
    }

   i++;
  }

 if(scr_vcomp(v1, comp) * sign > p * sign) { out[i] = *v1; }
 if(flag)
  scr_clip_func[func_index](scr_clip_param[curr].func_index, &out[0], &out[1]);

 return flag;
}

float world_vcomp(WCLIP_POINT *v, int f)
{
 if(f == 2)
  return v->trans.z;
}

void set_world_clip_func(int n, int func(int, WCLIP_POINT *, WCLIP_POINT *), int comp, float p, int sign, int func_index)
{
 world_clip_func[n] = func;
 world_clip_param[n].sign = sign;
 world_clip_param[n].comp = comp;
 world_clip_param[n].p = p;
 world_clip_param[n].func_index = func_index;
}

int world_clip(int curr, WCLIP_POINT *v0, WCLIP_POINT *v1)
{
 WCLIP_POINT out[2];
 int flag = 0, i = 0, j;
 float k;

 float p = world_clip_param[curr].p;
 int sign = world_clip_param[curr].sign, func_index = world_clip_param[curr].func_index, comp = world_clip_param[curr].comp;

 if(world_vcomp(v0, comp) * sign > p * sign) { out[i] = *v0; flag++; i++; }
 if(world_vcomp(v1, comp) * sign > p * sign) { flag++; }
 if(flag == 1)
  {
   k = (p - world_vcomp(v0, comp)) / (world_vcomp(v1, comp) - world_vcomp(v0, comp));
   out[i].trans.x = v0->trans.x + k * (v1->trans.x - v0->trans.x);
   out[i].trans.y = v0->trans.y + k * (v1->trans.y - v0->trans.y);
   out[i].trans.z = v0->trans.z + k * (v1->trans.z - v0->trans.z);

   project_wclip_point(&out[i]);

   out[i].affine_interp[0] = (v0->affine_interp[0] * v0->screen.z + k * (v1->affine_interp[0] * v1->screen.z - v0->affine_interp[0] * v0->screen.z)) / out[i].screen.z;
   out[i].affine_interp[1] = (v0->affine_interp[1] * v0->screen.z + k * (v1->affine_interp[1] * v1->screen.z - v0->affine_interp[1] * v0->screen.z)) / out[i].screen.z;

   if(world_clip_param[curr].count == 0)
    {
     world_clip_param[curr].last = out[i];
     world_clip_param[curr].count++;
    }

   else
    {
     world_clip_func[func_index](world_clip_param[curr].func_index, &out[i], &world_clip_param[curr].last);
     world_clip_param[curr].count = 0;
    }

   i++;
  }

 if(world_vcomp(v1, comp) * sign > p * sign) { out[i] = *v1; }
 if(flag)
  world_clip_func[func_index](world_clip_param[curr].func_index, &out[0], &out[1]);

 return flag;
}

void init_clipper()
{
 set_scr_clip_func(0, scr_clip, 0, BASE_INT_maxx, -1, 1);
 set_scr_clip_func(1, scr_clip, 0, BASE_INT_minx, 1, 2);
 set_scr_clip_func(2, scr_clip, 1, BASE_INT_maxy, -1, 3);
 set_scr_clip_func(3, scr_clip, 1, BASE_INT_miny, 1, 4);

 set_world_clip_func(0, world_clip, 2, BASE_INT_zfar, -1, 1);
 set_world_clip_func(1, world_clip, 2, BASE_INT_znear, 1, 2);
}

#endif
