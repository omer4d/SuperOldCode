#include <stdio.h>
#include <math.h>
#include <allegro.h>
#include "vec2f.h"
#include "vec3f.h"
#include "mat16f.h"
#include "base.h"
#include "md2.h"
#include "rendering.h"

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
 if(set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, GFX_W, GFX_H, 0, 0))
  {
   allegro_message("Error: %s.", allegro_error);
   exit(1);
  }

 install_keyboard();
 install_mouse();

 buffer = create_bitmap(SCREEN_W, SCREEN_H);
 zbuffer = create_bitmap_ex(32, SCREEN_W, SCREEN_H);
 texture = load_bitmap("data/babe_skin.bmp", NULL);

 setup_projection(90.0, -1.333, 1.333, -1.0, 1.0, -1.0, 20.0);
 init_renderer(GFX_H);
 bind_color_buffer(buffer);
 bind_zbuffer(zbuffer);
 bind_texture(texture);
}

int main()
{
 int exit_flag = 0, i;
 MAT16F tmat;
 VEC3F pos = vec3f(0.0, 0.0, 0.5), ang = vec3f(0.0, 0.0, 0.0);
 MD2_MODEL mdl;
 VERTEX *model_verts;
 TRI *model_tris;

 if(load_md2("data/babe.md2", &mdl))
  {
   allegro_message("Error: I need the MD2 model, stupid!");
   exit(1);
  }

 convert_md2_to_base(&model_verts, &model_tris, &mdl, 0.02);

 init();
 
 LOCK_VARIABLE(fps);
 LOCK_VARIABLE(frame_count);
 LOCK_FUNCTION(update_fps);
 install_int(update_fps, 1000);

 float frame = 0.0;
 int flag = 0, flag2 = 0;
 int anim_flag = 0, point_flag = 0;

 while(!exit_flag)
  {
   set_texture_mapping_mode(0);

   if(key[KEY_ESC]) { exit_flag = 1; }
   if(key[KEY_LEFT]) { pos.x -= 0.01; }
   if(key[KEY_RIGHT]) { pos.x += 0.01; }
   if(key[KEY_UP]) { pos.z += 0.02; }
   if(key[KEY_DOWN]) { pos.z -= 0.02; }
   if(key[KEY_SPACE]) { frame += 0.1; }
   if(key[KEY_P]) { anim_flag = anim_flag ? 0 : 1; key[KEY_P] = 0; }
   if(key[KEY_O]) { point_flag = point_flag ? 0 : 1; key[KEY_O] = 0; }

   VEC2F mp, cp;
   cp = vec2f(mouse_x, mouse_y);

   if(mouse_b == 1)
    {
     if(flag == 0)
       mp = cp;
     flag = 1;

     ang.y -= ((cp.x - mp.x) * 0.0002);
    }

   else
    flag = 0;

   if(mouse_b == 2)
    {
     if(flag2 == 0)
       mp = cp;
     flag2 = 1;

     pos.z -= ((cp.y - mp.y) * 0.0002);
    }

   else
    flag2 = 0;

   ang.x = M_PI * 1.5;

   convert_md2_frame_to_base(model_verts, &mdl, frame, 0.02);
   
   if(anim_flag)
   frame += 0.1 * (60.0 / (fps + 1.0));

   reset_mat16f(tmat);
   rotate_mat16f(tmat, ang.x, ang.y, ang.z);
   translate_mat16f(tmat, pos.x, pos.y, pos.z);

   for(i = 0; i < mdl.header.num_vertices; i++)
    {
     transform_vec3f(&model_verts[i].world, model_verts[i].local, tmat);
     project_vertex(&model_verts[i]);
    }

   clear_bitmap(buffer);
   clear_to_color(zbuffer, ZBUFFER_PRECISION);

   for(i = 0; i < mdl.header.num_tris; i++)
    {
     update_tri_normal(&model_tris[i], model_verts);
     frustum_cull(&model_tris[i], model_verts, -0.95, 20.0);
     cull_backface(&model_tris[i], model_verts);
    }

   for(i = 0; i < mdl.header.num_tris; i++)
    if(model_tris[i].vis)
     my_triangle(buffer, model_verts, model_tris[i]);

   if(point_flag)
   for(i = 0; i < mdl.header.num_tris; i++)
    if(model_tris[i].vis)
     {
      int x1 = model_verts[model_tris[i].a].sx, y1 = model_verts[model_tris[i].a].sy,
          x2 = model_verts[model_tris[i].b].sx, y2 = model_verts[model_tris[i].b].sy,
          x3 = model_verts[model_tris[i].c].sx, y3 = model_verts[model_tris[i].c].sy;

      float z, z1 = model_verts[model_tris[i].a].screen.z,
               z2 = model_verts[model_tris[i].b].screen.z,
               z3 = model_verts[model_tris[i].c].screen.z;

      if(on_bitmap(x1, y1, SCREEN_W, SCREEN_H))
       {
        z = (float)RI_PIXEL_Z(x1, y1) / (float)(ZBUFFER_PRECISION);
        if(z1 < z + 0.001) circlefill(buffer, x1, y1, 1, makecol(255, 0, 0));
       }

      if(on_bitmap(x2, y2, SCREEN_W, SCREEN_H))
       {
        z = (float)RI_PIXEL_Z(x2, y2) / (float)(ZBUFFER_PRECISION);
        if(z2 < z + 0.001) circlefill(buffer, x2, y2, 1, makecol(255, 0, 0));
       }

      if(on_bitmap(x3, y3, SCREEN_W, SCREEN_H))
       {
        z = (float)RI_PIXEL_Z(x3, y3) / (float)(ZBUFFER_PRECISION);
        if(z3 < z + 0.001) circlefill(buffer, x3, y3, 1, makecol(255, 0, 0));
       }
     }

   textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), 0, "FPS: %d", fps);
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   frame_count++;
  }

 free_md2(&mdl);
 free(model_tris);
 free(model_verts);
 destroy_bitmap(texture);
 destroy_bitmap(zbuffer);
 destroy_bitmap(buffer);
 deinit_renderer();

 return 0;
}
END_OF_MAIN()
