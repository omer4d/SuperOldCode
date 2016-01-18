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
 if(set_gfx_mode(GFX_AUTODETECT, GFX_W, GFX_H, 0, 0))
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

 while(!exit_flag)
  {
   set_texture_mapping_mode(0);

   if(key[KEY_ESC]) { exit_flag = 1; }
   if(key[KEY_LEFT]) { pos.x -= 0.01; }
   if(key[KEY_RIGHT]) { pos.x += 0.01; }
   if(key[KEY_UP]) { pos.z += 0.02; }
   if(key[KEY_DOWN]) { pos.z -= 0.02; }
   if(key[KEY_SPACE]) { set_texture_mapping_mode(1); }

   ang.x = M_PI * 1.5;
   ang.y = 1.7;

   convert_md2_frame_to_base(model_verts, &mdl, frame, 0.02);
   //frame += 0.1;

   reset_mat16f(tmat);
   rotate_mat16f(tmat, ang.x, ang.y, ang.z);
   translate_mat16f(tmat, pos.x, pos.y, pos.z);

   for(i = 0; i < mdl.header.num_vertices; i++)
    transform_vec3f(&model_verts[i].world, model_verts[i].local, tmat);

   clear_bitmap(buffer);
   clear_to_color(zbuffer, ZBUFFER_PRECISION);

   for(i = 0; i < mdl.header.num_tris; i++)
    {
     update_tri_normal(&model_tris[i], model_verts);
     model_tris[i].n = NORMALIZED_VEC3F(model_tris[i].n);
     if(cull_backface(&model_tris[i], model_verts))
      render_tri(&model_tris[i], model_verts);
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
