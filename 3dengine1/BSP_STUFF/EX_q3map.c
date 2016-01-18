#include <allegro.h>
#include <stdio.h>
#include "common.h"
#include "vec2f.h"
#include "vec3f.h"
#include "mat16f.h"
#include "base.h"

#define MOTION_SPEED 0.02

BITMAP *buffer;

void init()
{
 allegro_init();
 set_color_depth(32);

 if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0))
  {
   allegro_message("%s.", allegro_error);
   exit(1);
  }

 init_engine(SCREEN_W, SCREEN_H);
 point_at_color_buffer(&buffer);
 set_clipping_rect(10, 10, buffer->w - 11, buffer->h - 11);

 install_keyboard();
 install_mouse();
 srand(time(NULL));
}

void load_map(char fn[], VERTEX **vertex, POLY3D **poly, int *vnum, int *pnum, float scale)
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
 *vertex = (VERTEX *)malloc(sizeof(VERTEX) * v);

 for(i = 0; i < v; i++)
  {
   fscanf(stream, "%f", &temp_vec3f.x);
   fscanf(stream, "%f", &temp_vec3f.y);
   fscanf(stream, "%f", &temp_vec3f.z); 
   (*vertex)[i].object = USCALE_VEC3F(temp_vec3f, scale);
  }

 *vnum = v;

 fscanf(stream, "%d", &p);
 *poly = (POLY3D *)malloc(sizeof(POLY3D) * p);

 for(i = 0; i < p; i++)
  {
   fscanf(stream, "%d", &v);
   (*poly)[i].vnum = v;
   (*poly)[i].vind = (long *)malloc(v * sizeof(long));

   for(j = 0; j < v; j++)
    {
     fscanf(stream, "%d", &t);
     (*poly)[i].vind[j] = t;
    }
  }

 *pnum = p;
 fclose(stream);
}

int main()
{
 int exit = 0, i, j;
 VEC3F pos = vec3f(0.0, 0.0, 1.0), ang = ZERO_VEC3F;
 MAT16F mat;

 int vnum, pnum;
 VERTEX *vertex;
 POLY3D *poly;

 init();

 load_map("quake_map.txt", &vertex, &poly, &vnum, &pnum, (1.0 / 1000.0));
 //printf("%d", poly[144].vind[0]);

 while(!exit)
  {
   if(key[KEY_ESC]) { exit = 1; }
   if(key[KEY_LEFT]) { pos.x -= MOTION_SPEED; }
   if(key[KEY_RIGHT]) { pos.x += MOTION_SPEED; }
   if(key[KEY_UP]) { pos.z += MOTION_SPEED; }
   if(key[KEY_DOWN]) { pos.z -= MOTION_SPEED; }

   ang.x += 0.01;
   ang.y += 0.01;
   ang.z += 0.01;

   reset_mat16f(mat);
   rotate_x_mat16f(mat, ang.x);
   rotate_y_mat16f(mat, ang.y);
   rotate_z_mat16f(mat, ang.z);
   translate_mat16f(mat, pos.x, pos.y, pos.z);

   for(i = 0; i < vnum; i++)
    {
     transform_vec3f(&vertex[i].trans, vertex[i].object, mat);
     project_vertex(&vertex[i]);
    }

   clear_to_color(buffer, 0);

/*
   for(i = 0; i < vnum; i++)
    if(vertex[i].trans.z > 0.01)
     putpixel(buffer, vertex[i].sx, vertex[i].sy, makecol(255, 255, 255));
*/

   for(i = 0; i < pnum; i++)
    {
     for(j = 0; j < poly[i].vnum / 3; j++)
      {
     line(buffer, vertex[poly[i].vind[j * 3]].sx, vertex[poly[i].vind[j * 3]].sy,
                  vertex[poly[i].vind[j * 3 + 1]].sx, vertex[poly[i].vind[j * 3 + 1]].sy,
                  makecol(255, 255, 255));
     line(buffer, vertex[poly[i].vind[j * 3 + 1]].sx, vertex[poly[i].vind[j * 3 + 1]].sy,
                  vertex[poly[i].vind[j * 3 + 2]].sx, vertex[poly[i].vind[j * 3 + 2]].sy,
                  makecol(255, 255, 255));
     line(buffer, vertex[poly[i].vind[j * 3 + 2]].sx, vertex[poly[i].vind[j * 3 + 2]].sy,
                  vertex[poly[i].vind[j * 3]].sx, vertex[poly[i].vind[j * 3]].sy,
                  makecol(255, 255, 255));
      }
    }

   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  }

 for(i = 0; i < pnum; i++)
  free(poly[i].vind);
 free(poly);
 free(vertex);
 deinit_engine();
 return 0;
}
END_OF_MAIN()
