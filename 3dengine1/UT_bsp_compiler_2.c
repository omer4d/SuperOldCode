#include <stdio.h>
#include <conio.h>
#include <math.h>
#include "vec2f.h"
#include "vec3f.h"
#include "base.h"
#include "bsp_3d.h"
#include "static_bsp_3d.h"

void compile_static_bsp_tree(STATIC_BSP_NODE **static_tree, BSP_NODE *node, int node_num)
{
 int i;

 *static_tree = (STATIC_BSP_NODE *)malloc(sizeof(STATIC_BSP_NODE) * node_num);

 void convert(BSP_NODE *node)
  {
   if(node != NULL)
    {
     (*static_tree)[node->id].poly.vnum = node->poly.vnum;
     (*static_tree)[node->id].poly.vind = (long *)malloc(node->poly.vnum * sizeof(long));
     for(i = 0; i < node->poly.vnum; i++)
      (*static_tree)[node->id].poly.vind[i] = node->poly.vind[i];
     (*static_tree)[node->id].poly.normal = node->poly.normal;
     (*static_tree)[node->id].poly.col = node->poly.col;

     if(node->right != NULL)
      {
       (*static_tree)[node->id].right = node->right->id;
       convert(node->right);
      }

     else
      (*static_tree)[node->id].right = -1;

     if(node->left != NULL)
      {
       (*static_tree)[node->id].left = node->left->id;
       convert(node->left);
      }

     else
      (*static_tree)[node->id].left = -1;
    }
  }

 convert(node);
}

void save_static_bsp_tree(char fn[], VEC3F vertex[], int vnum, STATIC_BSP_NODE tree[], int node_num)
{
 int i, j;

 FILE *stream = fopen(fn, "w+");
 fprintf(stream, "%d ", vnum);
 fprintf(stream, "%d ", node_num);

 for(i = 0; i < vnum; i++)
  fprintf(stream, "%f %f %f ", vertex[i].x, vertex[i].y, vertex[i].z);

 for(i = 0; i < node_num; i++)
  {
   fprintf(stream, "%ld ", tree[i].left);
   fprintf(stream, "%ld ", tree[i].right);
   fprintf(stream, "%d ", tree[i].poly.vnum);
   fprintf(stream, "%d ", tree[i].poly.col);

   for(j = 0; j < tree[i].poly.vnum; j++)
    fprintf(stream, "%ld ", tree[i].poly.vind[j]);
  }

 fclose(stream);
}

void load_map(char fn[], VEC3F **vertex, POLY3D **poly, int *vnum, int *pnum, float scale)
{
 FILE *stream;
 int v, p, i, j, t;
 VEC3F temp_vec3f;

 stream = fopen(fn, "r");

 if(stream == NULL)
  {
   allegro_message("Unable to load map file: %s.", fn);
   exit(1);
  }

 fscanf(stream, "%d", &v);
 *vertex = (VEC3F *)malloc(sizeof(VEC3F) * v);

 for(i = 0; i < v; i++)
  {
   fscanf(stream, "%f", &temp_vec3f.z);
   fscanf(stream, "%f", &temp_vec3f.x);
   fscanf(stream, "%f", &temp_vec3f.y); 
   (*vertex)[i] = USCALE_VEC3F(temp_vec3f, scale);
  }

 *vnum = v;

 fscanf(stream, "%d", &p);
 *poly = (POLY3D *)malloc(sizeof(POLY3D) * p);

 for(i = 0; i < p; i++)
  {
   fscanf(stream, "%d", &v);
   //fscanf(stream, "%f", &temp_vec3f.x);
   //fscanf(stream, "%f", &temp_vec3f.y);
   //fscanf(stream, "%f", &temp_vec3f.z); 
   //(*poly)[i].normal = temp_vec3f;

   (*poly)[i].vnum = v;
   (*poly)[i].vind = (long *)malloc(v * sizeof(long));

   for(j = 0; j < v; j++)
    {
     fscanf(stream, "%d", &t);
     (*poly)[i].vind[j] = t;
    }

   (*poly)[i].normal = vec3f_normal((*vertex)[(*poly)[i].vind[0]],
                                    (*vertex)[(*poly)[i].vind[1]],
                                    (*vertex)[(*poly)[i].vind[2]]);
  }

 *pnum = p;
 fclose(stream);
}

void map_to_bsp(char in[], char out[], float wall_height, float scale)
{
 int i, vnum, pnum, node_id;
 VEC3F *map_vec3f;
 POLY3D *map_poly;
 STATIC_BSP_NODE *tree;

 //convert_map_to_3d(in, &map_vec3f, &map_poly, &vnum, &pnum, wall_height, scale);
 load_map(in, &map_vec3f, &map_poly, &vnum, &pnum, scale);
 printf("%d vertices, %d polygons... Converting...\n", vnum, pnum);

 BSP_NODE *root;
 add_bsp_root(&root, &map_poly[0]);
 node_id = 1;
 for(i = 1; i < pnum; i++)
  add_bsp_node(root, &map_poly[i], &map_vec3f, &vnum, &node_id);

 compile_static_bsp_tree(&tree, root, node_id);
 save_static_bsp_tree(out, map_vec3f, vnum, tree, node_id);

 destroy_bsp_tree(root);

 for(i = 0; i < pnum; i++)
  destroy_poly3d(&map_poly[i]);
 free(map_poly);
 free(map_vec3f);
 for(i = 0; i < node_id; i++)
  free(tree[i].poly.vind);
 free(tree);
 printf("Finished.\nResult: %d vertices, %d polygons (nodes).\n", vnum, node_id);
}

int main()
{
 char in[100], out[100];
 float height, scale;

 set_color_depth(32);

 printf("Input filename: ");
 scanf("%s", in);
 printf("Output filename: ");
 scanf("%s", out);
 printf("Wall height: ");
 scanf("%f", &height);
 printf("Scale: ");
 scanf("%f", &scale);
 printf("----------------------------------------------------------\n");

 map_to_bsp(in, out, height, scale);
 getch();
 return 0;
}
END_OF_MAIN()
