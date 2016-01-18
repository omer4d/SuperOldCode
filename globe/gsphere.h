#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "model.h"

VEC2F gen_spherical_texcoord(VEC3F p, float f)
{
 VEC2F tex;
 VEC3F a = USCALE_VEC3F(p, 1.0 / f);

 tex.x = atan2(a.x, a.z) / (2.0 * M_PI) + 0.75;
 tex.y = 1.0 + asin(a.y) / M_PI + 0.5;

 return tex;
}

void mirror_raw_tri(RAW_TRI *dest, RAW_TRI *src, float sx, float sy, float sz)
{
 dest->v[0].x = src->v[0].x * sx;
 dest->v[0].y = src->v[0].y * sy;
 dest->v[0].z = src->v[0].z * sz;
 dest->v[1].x = src->v[1].x * sx;
 dest->v[1].y = src->v[1].y * sy;
 dest->v[1].z = src->v[1].z * sz;
 dest->v[2].x = src->v[2].x * sx;
 dest->v[2].y = src->v[2].y * sy;
 dest->v[2].z = src->v[2].z * sz;
}

int generate_raw_gsphere(RAW_TRI **tri, int iter_num)
{
 int i, j, tri_num = pow(4, iter_num), next = 0;
 VEC3F mida, midb, midc;
 RAW_TRI temp;

 (*tri) = (RAW_TRI *)malloc(tri_num * 8 * sizeof(RAW_TRI));
 (*tri)[0].v[0] = vec3f(0.0, 1.0, 0.0);
 (*tri)[0].v[1] = vec3f(0.0, 0.0, 1.0);
 (*tri)[0].v[2] = vec3f(1.0, 0.0, 0.0);

 for(i = 0; i < iter_num; i++)
  for(j = 0; j < pow(4, i); j++)
   {
    mida = NORMALIZED_VEC3F(USCALE_VEC3F(VEC3F_SUM((*tri)[j].v[0], (*tri)[j].v[1]), 0.5));
    midb = NORMALIZED_VEC3F(USCALE_VEC3F(VEC3F_SUM((*tri)[j].v[1], (*tri)[j].v[2]), 0.5));
    midc = NORMALIZED_VEC3F(USCALE_VEC3F(VEC3F_SUM((*tri)[j].v[2], (*tri)[j].v[0]), 0.5));
    temp = (*tri)[j];

    set_raw_tri(&(*tri)[j], temp.v[0], mida, midc);
    set_raw_tri(&(*tri)[next + 1], mida, temp.v[1], midb);
    set_raw_tri(&(*tri)[next + 2], mida, midb, midc);
    set_raw_tri(&(*tri)[next + 3], midb, midc, temp.v[2]);

    next += 3;
   }

 for(i = 0; i < tri_num; i++)
  {
   mirror_raw_tri(&(*tri)[tri_num + i], &(*tri)[i], -1.0, 1.0, 1.0);
   mirror_raw_tri(&(*tri)[tri_num * 2 + i], &(*tri)[i], 1.0, -1.0, 1.0);
   mirror_raw_tri(&(*tri)[tri_num * 3 + i], &(*tri)[i], -1.0, -1.0, 1.0);
   mirror_raw_tri(&(*tri)[tri_num * 4 + i], &(*tri)[i], 1.0, 1.0, -1.0);
   mirror_raw_tri(&(*tri)[tri_num * 5 + i], &(*tri)[i], -1.0, 1.0, -1.0);
   mirror_raw_tri(&(*tri)[tri_num * 6 + i], &(*tri)[i], 1.0, -1.0, -1.0);
   mirror_raw_tri(&(*tri)[tri_num * 7 + i], &(*tri)[i], -1.0, -1.0, -1.0);
  }

 tri_num *= 8;

 for(i = 0; i < tri_num; i++)
  {
   VEC3F n = NORMALIZED_VEC3F(vec3f_normal((*tri)[i].v[0], (*tri)[i].v[1], (*tri)[i].v[2]));
   VEC3F c = VEC3F_DIFF(ZERO_VEC3F, (*tri)[i].v[0]);

   if(VEC3F_DOT_PRODUCT(n, c) > 0.0)
    {
     VEC3F t = (*tri)[i].v[0];
     (*tri)[i].v[0] = (*tri)[i].v[2];
     (*tri)[i].v[2] = t;
     n = USCALE_VEC3F(n, -1.0);
    }

   (*tri)[i].t[0] = gen_spherical_texcoord((*tri)[i].v[0], 1.0);
   (*tri)[i].t[1] = gen_spherical_texcoord((*tri)[i].v[1], 1.0);
   (*tri)[i].t[2] = gen_spherical_texcoord((*tri)[i].v[2], 1.0);
   (*tri)[i].n = n;
  }

 return tri_num;
}

void generate_gsphere(VERTEX **vert, VEC3F **vertnorm, VEC2F **texcoord, POLY3D **tri, int iter_num, int *vnum, int *tnum)
{
 RAW_TRI *raw_tri;

 *tnum = generate_raw_gsphere(&raw_tri, iter_num);
 *vnum = raw_to_index_tri(raw_tri, (*tnum), vert, vertnorm, texcoord, tri);

 free(raw_tri);
}

#endif
