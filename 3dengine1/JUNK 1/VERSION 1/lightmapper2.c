#include <stdio.h>
#include <math.h>
#include <allegro.h>
#include "vec2f.h"
#include "vec3f.h"
#include "mat16f.h"
#include "base.h"
#include "md2.h"
#include "rendering.h"
#include "map.h"
#include "affine.h"

#define GFX_W (320)
#define GFX_H (240)

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

 setup_projection(90.0, -1.333, 1.333, -1.0, 1.0, -1.0, 200.0);
 init_renderer(GFX_H);
 bind_color_buffer(buffer);
 bind_zbuffer(zbuffer);
}

#define WALL_HEIGHT 150.0
#define SPEED (0.3)

int main()
{
 int exit_flag = 0, i;
 MAT16F tmat;
 VEC3F pos = vec3f(0.0, 0.0, 0.0), ang = vec3f(0.0, 0.0, 0.0);

 int vnum, snum;
 VEC2F *map_vec2f;
 WALL_SEGMENT *map_segment;
 VERTEX *map_vertex;
 TRI *map_tri;

 init();

 load_map("map.txt", &map_vec2f, &map_segment, &vnum, &snum, 1.0);
 map_vertex = (VERTEX *)malloc((vnum * 2 + 4) * sizeof(VERTEX));
 map_tri = (TRI *)malloc((snum * 2 + 2) * sizeof(TRI));

 for(i = 0; i < vnum; i++)
  {
   map_vertex[i * 2].local = USCALE_VEC3F(vec3f(map_vec2f[i].x, 0.0, map_vec2f[i].y), 0.1);
   map_vertex[i * 2 + 1].local = USCALE_VEC3F(vec3f(map_vec2f[i].x, WALL_HEIGHT, map_vec2f[i].y), 0.1);
  }

 map_vertex[vnum * 2].local = vec3f(-50.0, 0.0, 50.0);
 map_vertex[vnum * 2 + 1].local = vec3f(50.0, 0.0, 50.0);
 map_vertex[vnum * 2 + 2].local = vec3f(50.0, 0.0, -50.0);
 map_vertex[vnum * 2 + 3].local = vec3f(-50.0, 0.0, -50.0);

 printf("%d", snum * 2 + 2);

 for(i = 0; i < snum; i++)
  {
   TRI temp[2];
   float l = VEC2F_LENGTH(VEC2F_DIFF(map_vec2f[map_segment[i].a],
                                     map_vec2f[map_segment[i].b])) / 100.0;

   temp[0].a = map_segment[i].a * 2 + 1;
   temp[0].b = map_segment[i].b * 2 + 1;
   temp[0].c = map_segment[i].b * 2;
   temp[0].tex[0] = vec2f(0.0, 0.0);
   temp[0].tex[1] = vec2f(1.0, 0.0);
   temp[0].tex[2] = vec2f(1.0, 1.0);

   temp[1].a = map_segment[i].b * 2;
   temp[1].b = map_segment[i].a * 2;
   temp[1].c = map_segment[i].a * 2 + 1;
   temp[1].tex[0] = vec2f(0.0, 0.0);
   temp[1].tex[1] = vec2f(1.0, 0.0);
   temp[1].tex[2] = vec2f(1.0, 1.0);

   map_tri[i * 2] = temp[0];
   map_tri[i * 2 + 1] = temp[1];
  }

 map_tri[snum * 2].c = vnum * 2; map_tri[snum * 2].b = vnum * 2 + 1; map_tri[snum * 2].a = vnum * 2 + 2;
 map_tri[snum * 2].tex[0] = vec2f(0.0, 0.0); 
 map_tri[snum * 2].tex[1] = vec2f(1.0, 0.0); 
 map_tri[snum * 2].tex[2] = vec2f(1.0, 1.0);

 map_tri[snum * 2 + 1].c = vnum * 2 + 2; map_tri[snum * 2 + 1].b = vnum * 2 + 3; map_tri[snum * 2 + 1].a = vnum * 2;
 map_tri[snum * 2 + 1].tex[0] = vec2f(0.0, 0.0);
 map_tri[snum * 2 + 1].tex[1] = vec2f(1.0, 0.0);
 map_tri[snum * 2 + 1].tex[2] = vec2f(1.0, 1.0);

 int tri_num = snum * 2 + 2;

 BITMAP **lightmap = (BITMAP**)malloc(tri_num * sizeof(BITMAP *));
 for(i = 0; i < tri_num; i++)
  lightmap[i] = create_bitmap(LIGHTMAP_SIZE, LIGHTMAP_SIZE);

    LM_POINT lm_tri[3];
 for(i = 0; i < tri_num; i++)
  {
   lm_tri[0].sx = 0;  lm_tri[0].sy = 0;  lm_tri[0].w = map_vertex[map_tri[i].a].local;
   lm_tri[1].sx = LIGHTMAP_SIZE - 1; lm_tri[1].sy = 0;  lm_tri[1].w = map_vertex[map_tri[i].b].local;
   lm_tri[2].sx = LIGHTMAP_SIZE - 1; lm_tri[2].sy = LIGHTMAP_SIZE - 1; lm_tri[2].w = map_vertex[map_tri[i].c].local;
   LM_buffer = lightmap[i];
   clear_to_color(lightmap[i], 0);

   local_update_tri_normal(&map_tri[i], map_vertex);
   lm_triangle(lm_tri, USCALE_VEC3F(map_tri[i].n, -1.0), map_tri, map_vertex, i, tri_num);
   //if(i == 69)
    //printf("\n%f %f %f\n", map_tri[i].n.x, map_tri[i].n.y, map_tri[i].n.z);
  }

 LOCK_VARIABLE(fps);
 LOCK_VARIABLE(frame_count);
 LOCK_FUNCTION(update_fps);
 install_int(update_fps, 1000);

 float frame = 0.0;

 VEC3F cam_pos = vec3f(0.0, 3.0, 0.0), cam_dir, cam_dir_normal, cam_ang = vec3f(0.0, 0.0, 0.0);

 while(!exit_flag)
  {              
   set_texture_mapping_mode(0);

   int mx, my;
   get_mouse_mickeys(&mx, &my);
   position_mouse(SCREEN_W / 2, SCREEN_H / 2);

   cam_ang.x += my * 0.001;
   cam_ang.y -= mx * 0.001;
   cam_dir.x = SPEED * cos(0.5 * M_PI + cam_ang.y);
   cam_dir.y = 0.0;
   cam_dir.z = SPEED * sin(0.5 * M_PI + cam_ang.y);
   cam_dir_normal = vec3f(-cam_dir.z, 0.0, cam_dir.x);

   if(key[KEY_ESC]) { exit_flag = 1; }
   if(key[KEY_W]) { cam_pos = VEC3F_SUM(cam_pos, cam_dir); }
   if(key[KEY_S]) { cam_pos = VEC3F_DIFF(cam_pos, cam_dir); }
   if(key[KEY_A]) { cam_pos = VEC3F_SUM(cam_pos, cam_dir_normal); }
   if(key[KEY_D]) { cam_pos = VEC3F_DIFF(cam_pos, cam_dir_normal); }
   if(key[KEY_SPACE]) { set_texture_mapping_mode(1); }

   reset_mat16f(tmat);
   translate_mat16f(tmat, -cam_pos.x, -cam_pos.y, -cam_pos.z);
   rotate_mat16f(tmat, -cam_ang.x, -cam_ang.y, 0.0);

   for(i = 0; i < vnum * 2 + 4; i++)
    transform_vec3f(&map_vertex[i].world, map_vertex[i].local, tmat);

   clear_bitmap(buffer);
   clear_to_color(zbuffer, ZBUFFER_PRECISION);

   for(i = 0; i < snum * 2 + 2; i++)
    {
     update_tri_normal(&map_tri[i], map_vertex);
     //map_tri[i].n = USCALE_VEC3F(NORMALIZED_VEC3F(map_tri[i].n), -1.0);
     bind_texture(lightmap[i]);

     if(cull_backface(&map_tri[i], map_vertex))
     render_tri(&map_tri[i], map_vertex);
    }

   //draw_sprite(buffer, lightmap[20], 100, 100);
   textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), 0, "FPS: %d", fps);
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   frame_count++;
  }

 for(i = 0; i < tri_num; i++)
  destroy_bitmap(lightmap[i]);
 free(lightmap);
 free(map_tri);
 free(map_vertex);
 free(map_vec2f);
 free(map_segment);
 destroy_bitmap(texture);
 destroy_bitmap(zbuffer);
 destroy_bitmap(buffer);
 deinit_renderer();

 return 0;
}
END_OF_MAIN()
