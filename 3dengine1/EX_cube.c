#include <allegro.h>
#include "common.h"
#include "vec2f.h"
#include "vec3f.h"
#include "mat16f.h"
#include "base.h"
#include "rendering.h"

#define CUBE_SIZE 0.1
#define MOTION_SPEED 0.02

BITMAP *buffer;

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

 if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, 320, 240, 0, 0))
  {
   allegro_message("%s.", allegro_error);
   exit(1);
  }

 init_engine(SCREEN_W, SCREEN_H);
 point_at_color_buffer(&buffer);
 set_clipping_rect(10, 10, buffer->w - 11, buffer->h - 11);
 set_znear(0.05);
 init_clipper();
 set_rasterizer_function(flat_zbuff_sides);
 set_world_clip_func(2, wclip_to_rend_int, 0, 0, 0, 0);
 set_scr_clip_func(4, rend_int_to_tri, 0, 0, 0, 0);

 install_keyboard();
 install_mouse();
 srand(time(NULL));
}

int main()
{
 int exit = 0, i;
 VEC3F pos = vec3f(0.0, 0.0, 0.3), ang = ZERO_VEC3F;
 MAT16F mat;
 VERTEX cube_vertex[8];
 POLY3D cube_poly[6];

 init();

 cube_vertex[0].object = vec3f(-CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE);
 cube_vertex[1].object = vec3f(CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE);
 cube_vertex[2].object = vec3f(CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE);
 cube_vertex[3].object = vec3f(-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE);
 cube_vertex[4].object = vec3f(-CUBE_SIZE, CUBE_SIZE, CUBE_SIZE);
 cube_vertex[5].object = vec3f(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE);
 cube_vertex[6].object = vec3f(CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE);
 cube_vertex[7].object = vec3f(-CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE);

 create_poly3d(&cube_poly[0], random_color(), 4, 0, 1, 2, 3);
 create_poly3d(&cube_poly[1], random_color(), 4, 7, 6, 5, 4);
 create_poly3d(&cube_poly[2], random_color(), 4, 4, 5, 1, 0);
 create_poly3d(&cube_poly[3], random_color(), 4, 6, 7, 3, 2);
 create_poly3d(&cube_poly[4], random_color(), 4, 0, 3, 7, 4);
 create_poly3d(&cube_poly[5], random_color(), 4, 5, 6, 2, 1);

 LOCK_VARIABLE(fps);
 LOCK_VARIABLE(frame_count);
 LOCK_FUNCTION(update_fps);
 install_int(update_fps, 1000);

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

   for(i = 0; i < 8; i++)
    {
     transform_vec3f(&cube_vertex[i].trans, cube_vertex[i].object, mat);
     project_vertex(&cube_vertex[i]);
    }

   clear_to_color(buffer, 0);
   clear_to_color(BASE_INT_z_buffer, BASE_INT_z_buffer_precision);

   for(i = 0; i < 6; i++)
    {
     update_poly3d_normal(&cube_poly[i], cube_vertex);
     if(!cull_backface(&cube_poly[i], cube_vertex))
     render_poly3d(&cube_poly[i], cube_vertex);
    }

   textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), 0, "FPS: %d", fps);
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   frame_count++;
  }

 for(i = 0; i < 6; i++)
  destroy_poly3d(&cube_poly[i]);

 deinit_engine();
 return 0;
}
END_OF_MAIN()
