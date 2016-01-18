#ifndef _STATIC_BSP_3D_H_
#define _STATIC_BSP_3D_H_

#include "vec3f.h"
#include "base.h"
#include "common.h"

typedef struct {
 POLY3D poly;
 long left, right;
 int flag;
}STATIC_BSP_NODE;

void load_static_bsp(char fn[], VERTEX *vertex[], int *vnum, STATIC_BSP_NODE *tree[], int *node_num)
{
 int i, j;
 VEC3F v;
 int ti;
 long tl;

 FILE *stream = fopen(fn, "r");
 fscanf(stream, "%d", vnum);
 fscanf(stream, "%d", node_num);

 *vertex = (VERTEX *)malloc((*vnum) * sizeof(VERTEX));
 *tree = (STATIC_BSP_NODE *)malloc((*node_num) * sizeof(STATIC_BSP_NODE));

 for(i = 0; i < (*vnum); i++)
  {
   fscanf(stream, "%f", &v.x);
   fscanf(stream, "%f", &v.y);
   fscanf(stream, "%f", &v.z);
   (*vertex)[i].object = v;
  }

 for(i = 0; i < (*node_num); i++)
  {
   fscanf(stream, "%ld", &tl); (*tree)[i].left = tl;
   fscanf(stream, "%ld", &tl); (*tree)[i].right = tl;
   fscanf(stream, "%d", &ti); (*tree)[i].poly.vnum = ti;
   (*tree)[i].poly.vind = (long *)malloc(ti * sizeof(long));
   (*tree)[i].poly.texcoord = (VEC2F *)malloc(ti * sizeof(VEC2F));
   fscanf(stream, "%d", &ti); (*tree)[i].poly.col = ti;

   for(j = 0; j < (*tree)[i].poly.vnum; j++)
    {
     fscanf(stream, "%ld", &tl);
     (*tree)[i].poly.vind[j] = tl;
    }

   (*tree)[i].poly.normal = vec3f_normal((*vertex)[(*tree)[i].poly.vind[0]].object,
                                         (*vertex)[(*tree)[i].poly.vind[1]].object,
                                         (*vertex)[(*tree)[i].poly.vind[2]].object);
  }

 fclose(stream);
}

int classify_volume(VEC3F a, VEC3F n, VEC3F volume[], int vnum)
{
 int i, behind = 0, infront = 0;
 float d;

 for(i = 0; i < vnum; i++)
  {
   d = which_side(a, n, volume[i]);
   if(d < -BSP_EPSILON) { behind++; }
   if(d > BSP_EPSILON) { infront++; }
  }

 if(behind && !infront)
  return BSP_BEHIND;
 if(infront && ! behind)
  return BSP_INFRONT;
 if(!behind && !infront)
  return BSP_INFRONT;
 return BSP_SPANNED;
}

#endif
