#ifndef _BSP_H_
#define _BSP_H_

#include "vec2f.h"

struct NODE_tag {
 int val;
 int a, b;
 VEC2F n;
 struct NODE_tag *parent, *left, *right;
};

typedef struct NODE_tag NODE;

#define EPSILON 0.001
#define INFRONT 1
#define BEHIND 0
#define SPLIT 2

float which_side(VEC2F a, VEC2F n, VEC2F p)
{
 return VEC2F_DOT_PRODUCT(VEC2F_DIFF(a, p), n);
}

int classify_segment(NODE *node, VEC2F a, VEC2F b, VEC2F *p, VEC2F vertex[])
{
 float d1, d2, k;

 d1 = which_side(vertex[node->a], node->n, a);
 d2 = which_side(vertex[node->a], node->n, b);

 if(fabs(d1) < EPSILON || fabs(d2) < EPSILON)
  {
   if(d1 + d2 > 0.0)
    return INFRONT;
   else
    return BEHIND;
  }
 else if(d1 > 0.0 && d2 > 0.0)
  return INFRONT;
 else if(d1 < 0.0 && d2 < 0.0)
  return BEHIND;
 else
  {
   k = fabs(d1) / (fabs(d1) + fabs(d2));
   *p = VEC2F_SUM(a, USCALE_VEC2F(VEC2F_DIFF(b, a), k));
   return SPLIT;
  }
}

int in_view(VEC2F p, VEC2F v, VEC2F a, VEC2F b)
{
 if(which_side(p, v, a) < 0.0 || which_side(p, v, b) < 0.0)
  return 1;
 return 0;
}

void add_node(NODE *curr, int *val, int a_index, int b_index, VEC2F **vertex, int *vnum)
{
 NODE *last;
 VEC2F p, a = (*vertex)[a_index], b = (*vertex)[b_index];

 int seg_class = classify_segment(curr, a, b, &p, *vertex);

 if(seg_class == INFRONT)
  {
   if(curr->right == NULL)
    {
     last = curr;
     curr = (NODE *)malloc(sizeof(NODE));
     curr->val = *val;
     curr->a = a_index;
     curr->b = b_index;
     curr->n = NORMALIZED_NORMAL_VEC2F(a, b);
     curr->parent = last;
     curr->left = NULL;
     curr->right = NULL;
     last->right = curr;
    }

   else
    add_node(curr->right, val, a_index, b_index, vertex, vnum);
  }

 if(seg_class == BEHIND)
  {
   if(curr->left == NULL)
    {
     last = curr;
     curr = (NODE *)malloc(sizeof(NODE));
     curr->val = *val;
     curr->a = a_index;
     curr->b = b_index;
     curr->n = NORMALIZED_NORMAL_VEC2F(a, b);
     curr->parent = last;
     curr->left = NULL;
     curr->right = NULL;
     last->left = curr;
    }

   else
    add_node(curr->left, val, a_index, b_index, vertex, vnum);
  }

 if(seg_class == SPLIT)
  {
   (*vnum)++;
   *vertex = (VEC2F *)realloc((*vertex), sizeof(VEC2F) * (*vnum));
   (*vertex)[(*vnum) - 1] = p;
   add_node(curr, val, a_index, (*vnum) - 1, vertex, vnum);
   (*val)++;
   add_node(curr, val, (*vnum) - 1, b_index, vertex, vnum);
  }
}

void destroy_tree(NODE *node)
{
 if(node != NULL)
  {
   if(node->right != NULL)
    destroy_tree(node->right);
   if(node->left != NULL)
    destroy_tree(node->left);
   free(node);
  }
}

#endif
