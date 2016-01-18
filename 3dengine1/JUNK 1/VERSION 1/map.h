#ifndef _MAP_H_
#define _MAP_H_

#include <stdio.h>
#include "vec2f.h"
#include "vec2f_graph.h"
#include "base.h"

typedef struct {
 int n, closed;
 VEC2F *point;
}LINE_LOOP;

typedef struct {
 int a, b;
}WALL_SEGMENT;

LINE_LOOP new_shape(int n, int c)
{
 LINE_LOOP shape;

 shape.n = n;
 shape.closed = c;
 shape.point = vec2f_array(n);

 return shape;
}

void save_map(char fn[], VEC2F map_vertex[], WALL_SEGMENT map_segment[], int vn, int sn)
{
 int i, j;

 FILE *stream = fopen(fn, "w+");
 fprintf(stream, "%d ", vn);
 fprintf(stream, "%d ", sn);

 for(i = 0; i < vn; i++)
  fprintf(stream, "%f %f ", map_vertex[i].x, map_vertex[i].y);
 for(i = 0; i < sn; i++)
  fprintf(stream, "%d %d ", map_segment[i].a, map_segment[i].b);

 fclose(stream);
}

void load_map(char fn[], VEC2F **map_vertex, WALL_SEGMENT **map_segment, int *vnum, int *snum, float scale)
{
 FILE *stream;
 LINE_LOOP *loop;
 int v, s, n, c, i, j;
 VEC2F vertex;
 WALL_SEGMENT segment;

 stream = fopen(fn, "r");

 fscanf(stream, "%d", &v);
 fscanf(stream, "%d", &s);

 *map_vertex = (VEC2F *)malloc(sizeof(VEC2F) * v);
 *map_segment = (WALL_SEGMENT *)malloc(sizeof(WALL_SEGMENT) * s);

 for(i = 0; i < v; i++)
  {
   fscanf(stream, "%f", &vertex.x);
   fscanf(stream, "%f", &vertex.y);
   (*map_vertex)[i] = vertex;
  }

 for(i = 0; i < s; i++)
  {
   fscanf(stream, "%d", &segment.a);
   fscanf(stream, "%d", &segment.b);
   (*map_segment)[i] = segment;
  }

 *vnum = v;
 *snum = s;
 fclose(stream);
}

void draw_map_sketch(BITMAP *buffer, LINE_LOOP *loop, int t)
{
 int i, j;
     
  for(j = 0; j < t; j++)
   {
    if(loop[j].n > 4 && !loop[j].closed)
     {
      int pn = loop[j].n / 2;
      vec2f_loop(buffer, loop[j].point, pn, loop[j].closed, 0);
      for(i = 0; i < pn - 1; i++)
        vec2f_line(buffer, loop[j].point[i + pn], loop[j].point[i + pn + 1], 0);
      for(i = 0; i < pn; i++)
       vec2f_line(buffer, loop[j].point[i], loop[j].point[i + pn], 0);
     }

    if(loop[j].n > 2 && loop[j].closed)
     {
      int pn = loop[j].n;
      vec2f_loop(buffer, loop[j].point, loop[j].n, loop[j].closed, 0);
     }

    if(loop[j].n == 4 && !loop[j].closed)
     {
      vec2f_line(buffer, loop[j].point[0], loop[j].point[1], 0);
      vec2f_line(buffer, loop[j].point[2], loop[j].point[3], 0);
      vec2f_line(buffer, loop[j].point[0], loop[j].point[2], 0);
      vec2f_line(buffer, loop[j].point[1], loop[j].point[3], 0);
     }
   }
}

void free_map(LINE_LOOP *loop, int n)
{
 int i;
 for(i = 0; i < n; i++)
  free(loop[i].point);
 free(loop);
}

#define WALL_HEIGHT 150.0
/*
void convert_map_to_3d(char fn[50], VEC3F **map_vertex, POLY3D **map_poly, int *vnum, int *pnum)
{
 int i, vnum_2f, segnum_2f;
 VEC2F *map_vec2f;
 WALL_SEGMENT *map_segment;

 load_map(fn, &map_vec2f, &map_segment, &vnum_2f, &segnum_2f, 1.0);
 *vnum = vnum_2f * 2;
 *pnum = segnum_2f;
 *map_vertex = (VEC3F *)malloc((*vnum) * sizeof(VEC3F));
 *map_poly = (POLY3D *)malloc((*pnum) * sizeof(POLY3D));

 for(i = 0; i < vnum_2f; i++)
  {
   (*map_vertex)[i * 2] = USCALE_VEC3F(vec3f(map_vec2f[i].x, 0.0, map_vec2f[i].y), 0.1);
   (*map_vertex)[i * 2 + 1] = USCALE_VEC3F(vec3f(map_vec2f[i].x, WALL_HEIGHT, map_vec2f[i].y), 0.1);
  }

 for(i = 0; i < segnum_2f; i++)
  {
   (*map_poly)[i].vnum = 4;
   (*map_poly)[i].vind = (int *)malloc(4 * sizeof(int));
   (*map_poly)[i].vind[0] = map_segment[i].a * 2 + 1;
   (*map_poly)[i].vind[1] = map_segment[i].b * 2 + 1;
   (*map_poly)[i].vind[2] = map_segment[i].b * 2;
   (*map_poly)[i].vind[3] = map_segment[i].a * 2;
   (*map_poly)[i].normal = vec3f_normal((*map_vertex)[(*map_poly)[i].vind[0]],
                                        (*map_vertex)[(*map_poly)[i].vind[1]],
                                        (*map_vertex)[(*map_poly)[i].vind[2]]);
  }
}
*/

void convert_map_to_3d(char fn[50], VEC3F **map_vertex, POLY3D **map_poly, int *vnum, int *pnum)
{
 int i, vnum_2f, segnum_2f;
 VEC2F *map_vec2f;
 WALL_SEGMENT *map_segment;

 load_map(fn, &map_vec2f, &map_segment, &vnum_2f, &segnum_2f, 1.0);
 *vnum = vnum_2f * 2 + 4;
 *pnum = segnum_2f + 1;
 *map_vertex = (VEC3F *)malloc((*vnum) * sizeof(VEC3F));
 *map_poly = (POLY3D *)malloc((*pnum) * sizeof(POLY3D));

 for(i = 0; i < vnum_2f; i++)
  {
   (*map_vertex)[i * 2] = USCALE_VEC3F(vec3f(map_vec2f[i].x, 0.0, map_vec2f[i].y), 0.1);
   (*map_vertex)[i * 2 + 1] = USCALE_VEC3F(vec3f(map_vec2f[i].x, WALL_HEIGHT, map_vec2f[i].y), 0.1);
  }

 (*map_vertex)[vnum_2f * 2] = vec3f(-50.0, 0.0, 50.0);
 (*map_vertex)[vnum_2f * 2 + 1] = vec3f(50.0, 0.0, 50.0);
 (*map_vertex)[vnum_2f * 2 + 2] = vec3f(50.0, 0.0, -50.0);
 (*map_vertex)[vnum_2f * 2 + 3] = vec3f(-50.0, 0.0, -50.0);

 for(i = 0; i < segnum_2f; i++)
  {
   (*map_poly)[i].vnum = 4;
   (*map_poly)[i].vind = (int *)malloc(4 * sizeof(int));
   (*map_poly)[i].vind[0] = map_segment[i].a * 2 + 1;
   (*map_poly)[i].vind[1] = map_segment[i].b * 2 + 1;
   (*map_poly)[i].vind[2] = map_segment[i].b * 2;
   (*map_poly)[i].vind[3] = map_segment[i].a * 2;
   (*map_poly)[i].normal = vec3f_normal((*map_vertex)[(*map_poly)[i].vind[0]],
                                        (*map_vertex)[(*map_poly)[i].vind[1]],
                                        (*map_vertex)[(*map_poly)[i].vind[2]]);
  }

 (*map_poly)[segnum_2f].vnum = 4;
 (*map_poly)[segnum_2f].vind = (int *)malloc(4 * sizeof(int));
 (*map_poly)[segnum_2f].vind[0] = vnum_2f * 2;
 (*map_poly)[segnum_2f].vind[1] = vnum_2f * 2 + 1;
 (*map_poly)[segnum_2f].vind[2] = vnum_2f * 2 + 2;
 (*map_poly)[segnum_2f].vind[3] = vnum_2f * 2 + 3;
 (*map_poly)[segnum_2f].normal = vec3f_normal((*map_vertex)[(*map_poly)[segnum_2f].vind[0]],
                                              (*map_vertex)[(*map_poly)[segnum_2f].vind[1]],
                                              (*map_vertex)[(*map_poly)[segnum_2f].vind[2]]);
}

#endif
