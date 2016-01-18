#ifndef _MAP_H_
#define _MAP_H_

#include <stdio.h>
#include "vec2f.h"
#include "base.h"

typedef struct {
 int a, b;
}WALL_SEGMENT;

void load_map(char fn[], VEC2F **map_vertex, WALL_SEGMENT **map_segment, int *vnum, int *snum, float scale)
{
 FILE *stream;
 int v, s, i;
 VEC2F vertex;
 WALL_SEGMENT segment;

 stream = fopen(fn, "r");

 if(stream == NULL)
  {
   allegro_message("Unable to load map file: %s.", fn);
   exit(1);
  }

 fscanf(stream, "%d", &v);
 fscanf(stream, "%d", &s);

 *map_vertex = (VEC2F *)malloc(sizeof(VEC2F) * v);
 *map_segment = (WALL_SEGMENT *)malloc(sizeof(WALL_SEGMENT) * s);

 for(i = 0; i < v; i++)
  {
   fscanf(stream, "%f", &vertex.x);
   fscanf(stream, "%f", &vertex.y);
   (*map_vertex)[i] = USCALE_VEC2F(vertex, scale);
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

void convert_map_to_3d(char fn[], VEC3F **map_vertex, POLY3D **map_poly, int *vnum, int *pnum, float wall_height, float scale)
{
 int i, vnum_2f, segnum_2f;
 VEC2F *map_vec2f;
 WALL_SEGMENT *map_segment;

 load_map(fn, &map_vec2f, &map_segment, &vnum_2f, &segnum_2f, scale);
 *vnum = vnum_2f * 2 + 4;
 *pnum = segnum_2f + 1;
 *map_vertex = (VEC3F *)malloc((*vnum) * sizeof(VEC3F));
 *map_poly = (POLY3D *)malloc((*pnum) * sizeof(POLY3D));

 for(i = 0; i < vnum_2f; i++)
  {
   (*map_vertex)[i * 2] = vec3f(map_vec2f[i].x, 0.0, map_vec2f[i].y);
   (*map_vertex)[i * 2 + 1] = vec3f(map_vec2f[i].x, wall_height, map_vec2f[i].y);
  }

 (*map_vertex)[vnum_2f * 2] = vec3f(-50.0, 0.0, 50.0);
 (*map_vertex)[vnum_2f * 2 + 1] = vec3f(50.0, 0.0, 50.0);
 (*map_vertex)[vnum_2f * 2 + 2] = vec3f(50.0, 0.0, -50.0);
 (*map_vertex)[vnum_2f * 2 + 3] = vec3f(-50.0, 0.0, -50.0);

 for(i = 0; i < segnum_2f; i++)
  {
   (*map_poly)[i].vnum = 4;
   (*map_poly)[i].vind = (long *)malloc(4 * sizeof(long));
   (*map_poly)[i].vind[0] = map_segment[i].a * 2 + 1;
   (*map_poly)[i].vind[1] = map_segment[i].b * 2 + 1;
   (*map_poly)[i].vind[2] = map_segment[i].b * 2;
   (*map_poly)[i].vind[3] = map_segment[i].a * 2;
   (*map_poly)[i].normal = vec3f_normal((*map_vertex)[(*map_poly)[i].vind[0]],
                                        (*map_vertex)[(*map_poly)[i].vind[1]],
                                        (*map_vertex)[(*map_poly)[i].vind[2]]);
  }

 (*map_poly)[segnum_2f].vnum = 4;
 (*map_poly)[segnum_2f].vind = (long *)malloc(4 * sizeof(long));
 (*map_poly)[segnum_2f].vind[0] = vnum_2f * 2;
 (*map_poly)[segnum_2f].vind[1] = vnum_2f * 2 + 1;
 (*map_poly)[segnum_2f].vind[2] = vnum_2f * 2 + 2;
 (*map_poly)[segnum_2f].vind[3] = vnum_2f * 2 + 3;
 (*map_poly)[segnum_2f].normal = vec3f_normal((*map_vertex)[(*map_poly)[segnum_2f].vind[0]],
                                              (*map_vertex)[(*map_poly)[segnum_2f].vind[1]],
                                              (*map_vertex)[(*map_poly)[segnum_2f].vind[2]]);

 free(map_vec2f);
 free(map_segment);
}

#endif
