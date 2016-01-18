struct NODE_tag {
 int val;
 VECTOR a, b, n;
 struct NODE_tag *parent, *left, *right;
};

typedef struct NODE_tag NODE;

#define EPSILON 0.001
#define INFRONT 1
#define BEHIND 0
#define SPLIT 2

float which_side(VECTOR a, VECTOR n, VECTOR p)
{
 return VECTOR_DOT_PRODUCT(VECTOR_DIFF(a, p), n);
}

float avoid_zero(float k)
{
 if(fabs(k) < EPSILON)
  {
   if(k >= 0.0) return EPSILON;
   else return -EPSILON;
  }
 return k;
}

int classify_segment(NODE *node, VECTOR a, VECTOR b, VECTOR *p)
{
 float d1, d2, k;

 d1 = which_side(node->a, node->n, a);
 d2 = which_side(node->a, node->n, b);

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
   *p = VECTOR_SUM(a, USCALE_VECTOR(VECTOR_DIFF(b, a), k));
   return SPLIT;
  }
}

int in_view(VECTOR p, VECTOR v, VECTOR a, VECTOR b)
{
 if(which_side(p, v, a) < 0.0 || which_side(p, v, b) < 0.0)
  return 1;
 return 0;
}

void add_node(NODE *curr, int *val, VECTOR a, VECTOR b)
{
 NODE *last;
 VECTOR p;
 int seg_class = classify_segment(curr, a, b, &p);

 if(seg_class == INFRONT)
  {
   if(curr->right == NULL)
    {
     last = curr;
     curr = (NODE *)malloc(sizeof(NODE));
     curr->val = *val;
     curr->a = a;
     curr->b = b;
     curr->n = NORMALIZED_NORMAL(a, b);
     curr->parent = last;
     curr->left = NULL;
     curr->right = NULL;
     last->right = curr;
    }

   else
    add_node(curr->right, val, a, b);
  }

 if(seg_class == BEHIND)
  {
   if(curr->left == NULL)
    {
     last = curr;
     curr = (NODE *)malloc(sizeof(NODE));
     curr->val = *val;
     curr->a = a;
     curr->b = b;
     curr->n = NORMALIZED_NORMAL(a, b);
     curr->parent = last;
     curr->left = NULL;
     curr->right = NULL;
     last->left = curr;
    }

   else
    add_node(curr->left, val, a, b);
  }

 if(seg_class == SPLIT)
  {
   add_node(curr, val, a, p);
   (*val)++;
   add_node(curr, val, p, b);
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
