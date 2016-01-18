#ifndef _MODEL_H_
#define _MODEL_H_

#include "vec3f.h"

typedef struct {
 VEC3F v[3], n;
 VEC2F t[3];
}RAW_TRI;

typedef struct {
 VEC3F v;
 VEC2F tex;
 int t, q, c;
}TVERT;

void set_raw_tri(RAW_TRI *tri, VEC3F a, VEC3F b, VEC3F c)
{
 tri->v[0] = a;
 tri->v[1] = b;
 tri->v[2] = c;
}

int vec3f_to_int(VEC3F v)
{
 return makecol32((v.x + 1.0) * 127.5, (v.y + 1.0) * 127.5, (v.z + 1.0) * 127.5);
}

void set_tvert(TVERT *tvert, VEC3F v, VEC2F tex, int t, int q)
{
 tvert->v = v;
 tvert->t = t;
 tvert->q = q;
 tvert->tex = tex;
 tvert->c = vec3f_to_int(v);
}

int tvert_comp(const void *_a, const void *_b)
{
 const TVERT *a = (const TVERT *)_a;
 const TVERT *b = (const TVERT *)_b;

 if(a->c == b->c)
  return 0;
 if(a->c < b->c)
  return -1;
 return 1;
}

int raw_to_index_tri(RAW_TRI raw_tri[], int raw_tri_num, VERTEX **vert, VEC3F **vertnorm, VEC2F **texcoord, POLY3D **tri)
{
 int i, j;
 int tvert_num = raw_tri_num * 3, vnum;
 int last, index;

 TVERT *tvert = (TVERT *)malloc(tvert_num * sizeof(TVERT));

 *tri = (POLY3D *)malloc(sizeof(POLY3D) * raw_tri_num);

 for(i = 0; i < raw_tri_num; i++)
  {
   set_tvert(&tvert[i * 3], raw_tri[i].v[0], raw_tri[i].t[0], i, 0);
   set_tvert(&tvert[i * 3 + 1], raw_tri[i].v[1], raw_tri[i].t[1], i, 1);
   set_tvert(&tvert[i * 3 + 2], raw_tri[i].v[2], raw_tri[i].t[2], i, 2);
   create_poly3d(&(*tri)[i], random_color(), 3, 0, 0, 0);
  }

 qsort(tvert, tvert_num, sizeof(TVERT), tvert_comp);

 last = -1;
 vnum = 0;

 for(i = 0; i < tvert_num; i++)
  if(tvert[i].c != last)
   {
    last = tvert[i].c;
    vnum++;
   }

 *vert = (VERTEX *)malloc(sizeof(VERTEX) * vnum);
 *vertnorm = (VEC3F *)malloc(sizeof(VEC3F) * vnum);
 *texcoord = (VEC2F *)malloc(sizeof(VEC2F) * vnum);

 last = -1;
 index = -1;

 int count;
 VEC3F average_normal;

 for(i = 0; i < tvert_num; i++)
  {
   if(tvert[i].c != last)
    {
     last = tvert[i].c;
     index++;
     (*vert)[index].object = tvert[i].v;
     (*texcoord)[index] = tvert[i].tex;

     if(index > 0)
      (*vertnorm)[index - 1] = USCALE_VEC3F(average_normal, 1.0 / count);
     
     average_normal = ZERO_VEC3F;
     count = 0;
    }

   (*tri)[tvert[i].t].vind[tvert[i].q] = index;
   average_normal = VEC3F_SUM(average_normal, raw_tri[tvert[i].t].n);
   count++;
  }

 (*vertnorm)[vnum - 1] = USCALE_VEC3F(average_normal, 1.0 / count);

 free(tvert);
 return vnum;
}

#endif
