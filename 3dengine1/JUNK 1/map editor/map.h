#ifndef _MAP_H_
#define _MAP_H_

#include <stdio.h>
#include "vector.h"
#include "graphics.h"

typedef struct {
 int n, closed;
 VECTOR *point;
}LINE_LOOP;

LINE_LOOP new_shape(int n, int c)
{
 LINE_LOOP shape;

 shape.n = n;
 shape.closed = c;
 shape.point = vector_array(n);

 return shape;
}

void save_map(char fn[], LINE_LOOP *final_shape, int sn)
{
 int i, j;
     
 FILE *stream = fopen(fn, "w+");
 fprintf(stream, "%d ", sn);

 for(j = 0; j < sn; j++)
  {
   fprintf(stream, "%d %d ", final_shape[j].n, final_shape[j].closed);
   for(i = 0; i < final_shape[j].n; i++)
    fprintf(stream, "%f %f ", final_shape[j].point[i].x, final_shape[j].point[i].y);
  }

 fclose(stream);
}

LINE_LOOP *load_map(char fn[], int *snum, float scale)
{
 FILE *stream;
 LINE_LOOP *loop;
 int t, n, c, i, j;

 stream = fopen(fn, "r");

 fscanf(stream, "%d", &t);
 loop = (LINE_LOOP *)malloc(t * sizeof(LINE_LOOP));

 for(i = 0; i < t; i++)
  {
   fscanf(stream, "%d", &n);
   fscanf(stream, "%d", &c);
   loop[i] = new_shape(n, c);

   for(j = 0; j < n; j++)
    {
     fscanf(stream, "%f", &loop[i].point[j].x);
     fscanf(stream, "%f", &loop[i].point[j].y);
     loop[i].point[j].x *= scale;
     loop[i].point[j].y *= scale;
    }
  }

 *snum = t;
 fclose(stream);
 return loop;
}

void draw_map_sketch(BITMAP *buffer, LINE_LOOP *loop, int t)
{
 int i, j;
     
  for(j = 0; j < t; j++)
   {
    if(loop[j].n > 4 && !loop[j].closed)
     {
      int pn = loop[j].n / 2;
      vector_loop(buffer, loop[j].point, pn, loop[j].closed, 0);
      for(i = 0; i < pn - 1; i++)
        vector_line(buffer, loop[j].point[i + pn], loop[j].point[i + pn + 1], 0);
      for(i = 0; i < pn; i++)
       vector_line(buffer, loop[j].point[i], loop[j].point[i + pn], 0);
     }

    if(loop[j].n > 2 && loop[j].closed)
     {
      int pn = loop[j].n;
      vector_loop(buffer, loop[j].point, loop[j].n, loop[j].closed, 0);
     }

    if(loop[j].n == 4 && !loop[j].closed)
     {
      vector_line(buffer, loop[j].point[0], loop[j].point[1], 0);
      vector_line(buffer, loop[j].point[2], loop[j].point[3], 0);
      vector_line(buffer, loop[j].point[0], loop[j].point[2], 0);
      vector_line(buffer, loop[j].point[1], loop[j].point[3], 0);
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

#endif
