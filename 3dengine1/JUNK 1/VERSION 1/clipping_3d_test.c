#include <stdio.h>
#include <math.h>
#include <allegro.h>
#include "vec2f.h"
#include "vec3f.h"
#include "mat16f.h"
#include "base.h"
#include "md2.h"
#include "rendering.h"
#include "bsp_3d.h"

#define GFX_W (640)
#define GFX_H (480)

BITMAP *buffer, *zbuffer, *texture;

int fps, frame_count = 0;

void update_fps()
{
 fps = frame_count;
 frame_count = 0;
}
END_OF_FUNCTION(update_fps)

void init()
{
 allegro_init();

 set_color_depth(32);
 if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, GFX_W, GFX_H, 0, 0))
  {
   allegro_message("Error: %s.", allegro_error);
   exit(1);
  }

 install_keyboard();
 install_mouse();

 buffer = create_bitmap(SCREEN_W, SCREEN_H);
 zbuffer = create_bitmap_ex(32, SCREEN_W, SCREEN_H);
 texture = load_bitmap("data/wall.bmp", NULL);

 setup_projection(90.0, -1.333, 1.333, -1.0, 1.0, -1.0, 20.0);
 init_renderer(GFX_H);
 bind_color_buffer(buffer);
 bind_zbuffer(zbuffer);
 bind_texture(texture);
}

void omg_poly(POLY3D poly, VERTEX map_vertex[])
{
 int j;
 TRI tri;

 for(j = 1; j < poly.vnum - 1; j++)
  {
   tri.a = poly.vind[0];
   tri.b = poly.vind[j];
   tri.c = poly.vind[j + 1];
   tri.tex[0] = vec2f(0.0, 0.0);
   tri.tex[1] = vec2f(1.0, 0.0);
   tri.tex[2] = vec2f(1.0, 1.0);
   render_tri(&tri, map_vertex);
  }
}

#define VNUM 10

int main()
{
 int exit_flag = 0, i;
 MAT16F tmat;

 init();
 
 LOCK_VARIABLE(fps);
 LOCK_VARIABLE(frame_count);
 LOCK_FUNCTION(update_fps);
 install_int(update_fps, 1000);

 VEC3F *vertex_3f;
 POLY3D *poly;
 VERTEX *vertex;

 vertex_3f = (VEC3F *)malloc(VNUM * sizeof(VEC3F));
 poly = (POLY3D *)malloc(1 * sizeof(POLY3D));

 poly[0].vind = (int *)malloc(VNUM * sizeof(int));
 poly[0].vnum = VNUM;
 
 int vnum = VNUM;

  for(i = 0; i < VNUM; i++)
  {
   vertex_3f[i].x = 0.7 * cos(i * 2.0 * M_PI / VNUM);
   vertex_3f[i].y = 0.7 * sin(i * 2.0 * M_PI / VNUM);
   vertex_3f[i].z = 0.3;
   poly[0].vind[i] = i;
  }

  reset_mat16f(tmat);
   rotate_mat16f(tmat, 0.0, 0.5, 0.0);

  for(i = 0; i < VNUM; i++)
   transform_vec3f(&vertex_3f[i], vertex_3f[i], tmat);
 

 POLY3D out0, out1;
 classify_polygon(vec3f(0.0, 0.0, 0.0), NORMALIZED_VEC3F(vec3f(0.0, 0.1, 1.0)), &vertex_3f, &vnum, &poly[0], &out0, &out1);

  vertex = (VERTEX *)malloc(vnum * sizeof(VERTEX));
  for(i = 0; i < vnum; i++)
   vertex[i].local = vertex_3f[i];

   reset_mat16f(tmat);
   //translate_mat16f(tmat, pos.x, pos.y, pos.z);
    for(i = 0; i < vnum; i++)
     transform_vec3f(&vertex[i].world, vertex[i].local, tmat);
 
 printf("%d, %d, %d", vnum, out0.vnum, out1.vnum);

 while(!exit_flag)
  {
   if(key[KEY_ESC]) { exit_flag = 1; }

   clear_bitmap(buffer);
   clear_to_color(zbuffer, ZBUFFER_PRECISION);

   textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), 0, "FPS: %d", fps);
   if(key[KEY_ENTER])
    {
     omg_poly(poly[0], vertex);
    }
   
   if(key[KEY_SPACE])
    omg_poly(out0, vertex);
   else
    omg_poly(out1, vertex);

   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   frame_count++;
  }

 free(vertex);
 free(vertex_3f);
 free(poly[0].vind);
 free(poly);
 destroy_bitmap(texture);
 destroy_bitmap(zbuffer);
 destroy_bitmap(buffer);
 deinit_renderer();

 return 0;
}
END_OF_MAIN()
