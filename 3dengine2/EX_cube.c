#include <allegro.h>
#include "common.h"
#include "vec2f.h"
#include "vec3f.h"
#include "mat16f.h"
#include "base.h"
#include "rendering.h"

#define CUBE_SIZE 10.0
#define CAM_SPEED 0.1

BITMAP *buffer, *texture;

int fps, frame_count = 0;

void update_fps()
{
 fps = frame_count;
 frame_count = 0;
}
END_OF_FUNCTION(update_fps)

#define RI_TEXEL(x, y) ((long *)texture->line[BIT_WRAP(y, texture->h - 1)])[BIT_WRAP(x, texture->w - 1)]

void tex_hline(int y, float x2, float interp1[], float didx[])
{
 int x, minx, maxx, iu, iv;
 float curr[3], k, u, v;
 long z;

 minx = (int)ceil(interp1[0]);
 maxx = (int)ceil(x2) - 1;

 if(minx > maxx)
  return;

 curr[0] = interp1[1] + (minx - interp1[0]) * didx[0];
 curr[1] = interp1[2] + (minx - interp1[0]) * didx[1];
 curr[2] = interp1[3] + (minx - interp1[0]) * didx[2];

 for(x = minx; x < maxx + 1; x++)
  {
   //z = (long)(BASE_INT_z_buffer_precision / curr[0]);

   //if(z < RI_PIXEL_Z(x, y))
    {
     k = 1.0 / curr[0];
     u = curr[1] * k;
     v = curr[2] * k;
     FAST_ROUND(u * (texture->w - 1), iu);
     FAST_ROUND(v * (texture->h - 1), iv);
     RI_PIXEL(x, y) = RI_TEXEL(iu, iv);
     //RI_PIXEL_Z(x, y) = z;
    }

   curr[0] += didx[0];
   curr[1] += didx[1];
   curr[2] += didx[2];
  }
}

void tex_sides(int miny, int maxy, float didy1[], float accum1[], float dxdy2, float *accum2, float didx[])
{
 int minx, maxx, y, i;

 for(y = miny; y < maxy + 1; y++)
  {
   tex_hline(y, *accum2, accum1, didx);
   accum1[0] += didy1[0];
   accum1[1] += didy1[1];
   accum1[2] += didy1[2];
   accum1[3] += didy1[3];
   (*accum2) += dxdy2;
  }
}

void init()
{
 allegro_init();
 set_color_depth(32);

 if(set_gfx_mode(GFX_AUTODETECT, 320, 240, 0, 0))
  {
   allegro_message("%s.", allegro_error);
   exit(1);
  }

 init_engine(SCREEN_W, SCREEN_H);
 point_at_color_buffer(&buffer);
 set_clipping_rect(1, 1, buffer->w - 1, buffer->h - 1);
 set_znear(1.0);
 init_clipper();
 set_rasterizer_function(tex_sides);
 set_world_clip_func(2, wclip_to_rend_int, 0, 0, 0, 0);
 set_scr_clip_func(4, rend_int_to_tri, 0, 0, 0, 0);

 install_keyboard();
 install_mouse();
 srand(time(NULL));
}

int main()
{
 int exit = 0, i;
 VEC3F pos = vec3f(0.0, 0.0, 0.0), ang = ZERO_VEC3F;
 MAT16F mat, sky_mat;
 VERTEX cube_vertex[8];
 POLY3D cube_poly[6];

 init();

 BITMAP *skybox[6];

 skybox[0] = load_bitmap("data/back.bmp", NULL);
 skybox[1] = load_bitmap("data/front.bmp", NULL);
 skybox[2] = load_bitmap("data/up.bmp", NULL);
 skybox[3] = load_bitmap("data/down.bmp", NULL);
 skybox[4] = load_bitmap("data/left.bmp", NULL);
 skybox[5] = load_bitmap("data/right.bmp", NULL);

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

 cube_poly[0].texcoord[0] = vec2f(1.0, 0.0);
 cube_poly[0].texcoord[1] = vec2f(0.0, 0.0);
 cube_poly[0].texcoord[2] = vec2f(0.0, 1.0);
 cube_poly[0].texcoord[3] = vec2f(1.0, 1.0);
 
 cube_poly[1].texcoord[0] = vec2f(0.0, 1.0);
 cube_poly[1].texcoord[1] = vec2f(1.0, 1.0);
 cube_poly[1].texcoord[2] = vec2f(1.0, 0.0);
 cube_poly[1].texcoord[3] = vec2f(0.0, 0.0);

 cube_poly[2].texcoord[0] = vec2f(1.0, 0.0);
 cube_poly[2].texcoord[1] = vec2f(0.0, 0.0);
 cube_poly[2].texcoord[2] = vec2f(0.0, 1.0);
 cube_poly[2].texcoord[3] = vec2f(1.0, 1.0);

 cube_poly[3].texcoord[0] = vec2f(1.0, 0.0);
 cube_poly[3].texcoord[1] = vec2f(0.0, 0.0);
 cube_poly[3].texcoord[2] = vec2f(0.0, 1.0);
 cube_poly[3].texcoord[3] = vec2f(1.0, 1.0);

 cube_poly[4].texcoord[0] = vec2f(0.0, 0.0);
 cube_poly[4].texcoord[1] = vec2f(0.0, 1.0);
 cube_poly[4].texcoord[2] = vec2f(1.0, 1.0);
 cube_poly[4].texcoord[3] = vec2f(1.0, 0.0);

 cube_poly[5].texcoord[0] = vec2f(0.0, 0.0);
 cube_poly[5].texcoord[1] = vec2f(0.0, 1.0);
 cube_poly[5].texcoord[2] = vec2f(1.0, 1.0);
 cube_poly[5].texcoord[3] = vec2f(1.0, 0.0);

 LOCK_VARIABLE(fps);
 LOCK_VARIABLE(frame_count);
 LOCK_FUNCTION(update_fps);
 install_int(update_fps, 1000);
 
 VEC3F cam_pos = vec3f(0.0, 0.0, 0.0), cam_dir, cam_dir_normal, cam_ang = vec3f(0.0, 0.0, 0.0);

 while(!exit)
  {
   if(key[KEY_ESC]) { exit = 1; }

   int mx, my;
   get_mouse_mickeys(&mx, &my);
   position_mouse(SCREEN_W / 2, SCREEN_H / 2);

   cam_ang.x += my * 0.001;
   cam_ang.y -= mx * 0.001;
   cam_dir.x = CAM_SPEED * cos(0.5 * M_PI + cam_ang.y);
   cam_dir.y = CAM_SPEED * cos(0.5 * M_PI + cam_ang.x);
   cam_dir.z = CAM_SPEED * sin(0.5 * M_PI + cam_ang.y);
   cam_dir_normal = vec3f(-cam_dir.z, 0.0, cam_dir.x);

   if(key[KEY_ESC]) { exit = 1; }
   if(key[KEY_W]) { cam_pos = VEC3F_SUM(cam_pos, cam_dir); }
   if(key[KEY_S]) { cam_pos = VEC3F_DIFF(cam_pos, cam_dir); }
   if(key[KEY_A]) { cam_pos = VEC3F_SUM(cam_pos, cam_dir_normal); }
   if(key[KEY_D]) { cam_pos = VEC3F_DIFF(cam_pos, cam_dir_normal); }

   set_fov(90.0);
   if(mouse_b > 1)
    set_fov(30.0);

   reset_mat16f(mat);
   translate_mat16f(mat, -cam_pos.x, -cam_pos.y, -cam_pos.z);
   rotate_z_mat16f(mat, 0.0);
   rotate_y_mat16f(mat, -cam_ang.y);
   rotate_x_mat16f(mat, -cam_ang.x);
   
   reset_mat16f(sky_mat);
   rotate_z_mat16f(sky_mat, 0.0);
   rotate_y_mat16f(sky_mat, -cam_ang.y);
   rotate_x_mat16f(sky_mat, -cam_ang.x);

   for(i = 0; i < 8; i++)
    {
     transform_vec3f(&cube_vertex[i].trans, cube_vertex[i].object, sky_mat);
     project_vertex(&cube_vertex[i]);
    }

   clear_to_color(buffer, 0);
   clear_to_color(BASE_INT_z_buffer, BASE_INT_z_buffer_precision);

   for(i = 0; i < 6; i++)
    {
     update_poly3d_normal(&cube_poly[i], cube_vertex);
     texture = skybox[i];

     if(cull_backface(&cube_poly[i], cube_vertex))
      render_poly3d(&cube_poly[i], cube_vertex);
    }

   textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), 0, "FPS: %d", fps);
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   frame_count++;
  }

 for(i = 0; i < 6; i++)
  {
   destroy_poly3d(&cube_poly[i]);
   destroy_bitmap(skybox[i]);
  }

 deinit_engine();
 return 0;
}
END_OF_MAIN()
