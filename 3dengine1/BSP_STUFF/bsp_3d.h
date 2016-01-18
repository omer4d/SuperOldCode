#ifndef _BSP_3D_H_
#define _BSP_3D_H_

#include "vec3f.h"
#include "base.h"
#include "common.h"

struct BSP_NODE_tag {
 int id;
 POLY3D poly;
 struct BSP_NODE_tag *parent, *left, *right;
};

typedef struct BSP_NODE_tag BSP_NODE;

int classify_polygon(VEC3F a, VEC3F normal, VEC3F **vertex, int *vnum, POLY3D *in, POLY3D *out0, POLY3D *out1)
{
 int i, behind = 0, infront = 0, flag;
 int count0 = 0, count1 = 0, new_vert_num = 0, new_vert_index = 0;
 float *d = (float *)malloc(in->vnum * sizeof(float)), k;

 for(i = 0; i < in->vnum; i++)
  {
   d[i] = which_side(a, normal, (*vertex)[in->vind[i]]);
   if(d[i] < -BSP_EPSILON) { behind++; }
   if(d[i] > BSP_EPSILON) { infront++; }
  }

 if(behind && !infront)
  return BSP_BEHIND;
 if(infront && ! behind)
  return BSP_INFRONT;
 if(!behind && !infront)
  return BSP_INFRONT;

 inline void clip_count(int v0, int v1)
  {
   flag = 0;
   if(d[v0] > BSP_EPSILON) { flag++; out0->vnum++; }
   else                    { out1->vnum++; }
   if(d[v1] > BSP_EPSILON) { flag++; }

   if(flag == 1)
    {
     out0->vnum++;
     out1->vnum++;
     new_vert_num++;
    }
  }

 inline void clip_seg(int i0, int i1, int v0, int v1)
  {
   flag = 0;
   if(d[i0] > BSP_EPSILON) { out0->vind[count0] = v0; flag++; count0++; }
   else { out1->vind[count1] = v0; count1++; }
   if(d[i1] > BSP_EPSILON) { flag++; }

   if(flag == 1)
    {
     k = fabs(d[i0]) / (fabs(d[i0]) + fabs(d[i1]));
     (*vertex)[(*vnum) + new_vert_index] = VEC3F_SUM((*vertex)[in->vind[i0]], USCALE_VEC3F(VEC3F_DIFF((*vertex)[in->vind[i1]], (*vertex)[in->vind[i0]]), k));
     out0->vind[count0] = (*vnum) + new_vert_index;
     out1->vind[count1] = (*vnum) + new_vert_index;
     count0++;
     count1++;
     new_vert_index++;
    }
  }

 out0->vnum = 0;
 out1->vnum = 0;

 for(i = 0; i < in->vnum - 1; i++)
  clip_count(i, i + 1);
 clip_count(in->vnum - 1, 0);

 out0->vind = (long *)malloc(out0->vnum * sizeof(long));
 out1->vind = (long *)malloc(out1->vnum * sizeof(long));
 *vertex = (VEC3F *)realloc((*vertex), sizeof(VEC3F) * ((*vnum) + new_vert_num));

 for(i = 0; i < in->vnum - 1; i++)
  clip_seg(i, i + 1, in->vind[i], in->vind[i + 1]);
 clip_seg(in->vnum - 1, 0, in->vind[in->vnum - 1], in->vind[0]);

 out0->normal = in->normal;
 out1->normal = in->normal;
 (*vnum) += new_vert_num; 
 free(d);

 return BSP_SPANNED;
}

void add_bsp_root(BSP_NODE **curr, POLY3D *poly)
{
 int i;

 *curr = (BSP_NODE *)malloc(sizeof(BSP_NODE));

 (*curr)->poly.vnum = poly->vnum;
 (*curr)->poly.vind = (long *)malloc(poly->vnum * sizeof(long));

 for(i = 0; i < (*curr)->poly.vnum; i++)
  (*curr)->poly.vind[i] = poly->vind[i];

 (*curr)->id = 0;
 (*curr)->poly.normal = poly->normal;
 (*curr)->poly.col = random_color();
 (*curr)->parent = NULL;
 (*curr)->left = NULL;
 (*curr)->right = NULL;
}

void add_bsp_node(BSP_NODE *curr, POLY3D *poly, VEC3F **vertex, int *vnum, int *id)
{
 int i;
 BSP_NODE *last;
 POLY3D out0, out1;

 int seg_class = classify_polygon((*vertex)[curr->poly.vind[0]], curr->poly.normal, vertex, vnum, poly, &out0, &out1);

 if(seg_class == BSP_INFRONT)
  {
   if(curr->right == NULL)
    {
     last = curr;
     curr = (BSP_NODE *)malloc(sizeof(BSP_NODE));
     curr->id = *id;
     curr->poly.vnum = poly->vnum;
     curr->poly.vind = (long *)malloc(curr->poly.vnum * sizeof(long));
     for(i = 0; i < curr->poly.vnum; i++)
      curr->poly.vind[i] = poly->vind[i];
     curr->poly.normal = poly->normal;
     curr->poly.col = random_color();

     curr->parent = last;
     curr->left = NULL;
     curr->right = NULL;
     last->right = curr;
     (*id)++;
    }

   else
    add_bsp_node(curr->right, poly, vertex, vnum, id);
  }

 if(seg_class == BSP_BEHIND)
  {
   if(curr->left == NULL)
    {
     last = curr;
     curr = (BSP_NODE *)malloc(sizeof(BSP_NODE));

     curr->id = *id;
     curr->poly.vnum = poly->vnum;
     curr->poly.vind = (long *)malloc(curr->poly.vnum * sizeof(long));
     for(i = 0; i < curr->poly.vnum; i++)
      curr->poly.vind[i] = poly->vind[i];
     curr->poly.normal = poly->normal;
     curr->poly.col = random_color();

     curr->parent = last;
     curr->left = NULL;
     curr->right = NULL;
     last->left = curr;
     (*id)++;
    }

   else
    add_bsp_node(curr->left, poly, vertex, vnum, id);
  }

 if(seg_class == BSP_SPANNED)
  {
   add_bsp_node(curr, &out0, vertex, vnum, id);
   add_bsp_node(curr, &out1, vertex, vnum, id);
   free(out0.vind);
   free(out1.vind);
  }
}

void destroy_bsp_tree(BSP_NODE *node)
{
 if(node != NULL)
  {
   if(node->right != NULL)
    destroy_bsp_tree(node->right);
   if(node->left != NULL)
    destroy_bsp_tree(node->left);
   free(node->poly.vind);
   free(node);
  }
}

#endif
